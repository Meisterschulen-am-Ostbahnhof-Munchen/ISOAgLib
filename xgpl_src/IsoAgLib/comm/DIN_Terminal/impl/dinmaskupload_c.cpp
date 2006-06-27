/***************************************************************************
                          dinmaskupload_c.cpp - special class to handle GPS
                                       informations
                             -------------------
    begin                : Fri Sep 28 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "dinmaskupload_c.h"
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/SystemMgmt/DIN9684/impl/dinmonitor_c.h>
#include <IsoAgLib/comm/Process/impl/process_c.h>

#include <IsoAgLib/comm/Process/elementddi_s.h>

#include <IsoAgLib/comm/Multipacket/impl/multisend_c.h>

// activate following define and include RS232 modules into
// project, if RS232 debug of mask upload is wanted
// #define DEBUG_RS232
#if defined(DEBUG_RS232) || defined(DEBUG_HEAP_USEAGE)
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_syncProcTotal = 0;
#endif


#define LBS_PLUS_PROJECT_SIZE 12
#define FS_OLD_PROJECT_SIZE 0xD

namespace __IsoAgLib {

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique DINMaskUpload_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  DINMaskUpload_c& getDinMaskuploadInstance( uint8_t rui8_instance ) { return DINMaskUpload_c::instance( rui8_instance );};
#else
  /** C-style function, to get access to the unique DINMaskUpload_c singleton instance */
  DINMaskUpload_c& getDinMaskuploadInstance( void ) { return DINMaskUpload_c::instance();};
#endif


/** initialisation for IsoAgLibMaskUpload */
void DINMaskUpload_c::init( void )
{ // first register in Scheduler_c
  // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
  getSchedulerInstance4Comm().registerClient( this );
  // register additionally as individual instance -> needed for special handling
  // on DIN member name receive, which requires immediate call of DINMaskUpload_c::timeEvent
  // as a registered DIN terminal could have sent it's name
  // -> DINMaskUpload_c::timeEvent must create immediate specific local process data
  //    to answer on immediate requested process data (Varioterminal sends first sync
  //    process data request just 20 ms after sending it's own name
  getSchedulerInstance4Comm().registerDinMaskuploadInstance( this );

  c_devKey.setUnspecified();
  pc_terminal = NULL;

  CNAMESPACE::memset(psMaskDef,0 , REGISTRABLE_MASK_CNT);
  ui8_maskDefCnt = 0;
  ui8_maskDefInd = 0;

  en_maskUploadState = none;

  // set configure values
  pc_data = &(getMultiSendInstance4Comm().data());
}
/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void DINMaskUpload_c::close( void ) {
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    // clear vector of sync proc
    arrSyncproc.clear();
    #ifdef DEBUG_HEAP_USEAGE
    sui16_syncProcTotal = 0;
    #endif
    en_maskUploadState = none;
    ui8_maskDefCnt = 0;
    // unregister from Scheduler_c
    getSchedulerInstance4Comm().unregisterClient( this );
  }
};

/** default destructor which has nothing to do */
DINMaskUpload_c::~DINMaskUpload_c(){
}

/**
  deliver reference to data pkg as reference to CANPkgExt_c
  to implement the base virtual function correct
*/
CANPkgExt_c& DINMaskUpload_c::dataBase()
{
  return *pc_data;
}

/**
  perform periodical actions
  @return true -> all planned activities performed in allowed time
*/
bool DINMaskUpload_c::timeEvent( void )
{
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

  uint8_t ui8_testInd;
  const DevKey_c c_oldTerminalDevKey = c_devKey;
  switch(en_maskUploadState)
  {
    case none:
      // check if one of the registered terminals is active
      for (ui8_testInd = 0; ui8_testInd < ui8_maskDefCnt; ui8_testInd++)
      {
        if (getDinMonitorInstance4Comm().existDinMemberDevKey( psMaskDef[ui8_testInd]->c_devKey, true ))
        {
          DINItem_c* pc_dinTerminalItem = &(getDinMonitorInstance4Comm().dinMemberDevKey(psMaskDef[ui8_testInd]->c_devKey, true));
          if (CNAMESPACE::memcmp(pc_dinTerminalItem->name(), psMaskDef[ui8_testInd]->pb_termName, 7) == 0)
          { // registered terminal at index ui8_testInd is found
            pc_terminal = pc_dinTerminalItem;
            c_devKey = pc_terminal->devKey();
            ui8_maskDefInd = ui8_testInd;

            // check if terminal wants syncronisation process data with DEV_KEY of terminal
            switch (activeMask().en_terminalType)
            {
              case IsoAgLib::FendtVario:
              case IsoAgLib::AgrocomACT:
                // activate in ProcessPkg_c the automatic conversion of DEV_KEY
                // sent/received process data msgs so that DEV_KEY can be normally handled
                // in higher level algorithms
                getProcessInstance4Comm().data().useTermDevKeyForLocalProc(c_devKey);
                break;
              case IsoAgLib::FieldstarOld:
                getProcessInstance4Comm().data().useTermDevKeyForLocalProc(c_devKey, DevKey_c(0,0));
                break;
              default:
                break;
            }
            // request immediately names of other members
            // -> trigger terminal to start sync-procedure
            getDinMonitorInstance4Comm().requestDinMemberNames();

            // only create new entries if terminal changed or if no synproc
            // entries exist ( avoid double creation if several tries are needed )
            if ( ( c_oldTerminalDevKey !=  c_devKey ) || ( ! arrSyncproc.empty() ) )
            { // now create the process data for syncronisation values
              if ( activeMask().en_terminalType != IsoAgLib::FieldstarOld) createLbsplusProcdata();
              else createFieldstarProcdata();
            }
            #if defined(DEBUG_RS232) || defined(DEBUG_HEAP_USEAGE)
            getRs232Instance() << "Starte DIN Upload zu Terminal DevClass: "
              << uint16_t( c_devKey.getDevClass() )
              << ", Pos: " << uint16_t( c_devKey.getDevClassInst() )
              << "\n\r";
            #endif
            en_maskUploadState = await_start;

          } // if registered terminal at index ui8_testInd is active
        } // exist DINItem_c with tested devKey
      } // for all registered terminals
      break;
    case running:
      // test if upload finished
      if (en_sendSuccess==MultiSend_c::SendSuccess) en_maskUploadState = success;
      else if (en_sendSuccess==MultiSend_c::SendAborted) {
        en_maskUploadState = none ; //abort;
      }
      break;
    case await_start:
      if ( ( ( c_devKey.isSpecified() ) && (getDinMonitorInstance4Comm().existDinMemberDevKey(c_devKey, true)) )
        && ( getSystemMgmtInstance4Comm().existActiveLocalDinMember()                                 ) )
      { // resend the mask sync data every 500msec
        tryResendMaskSyncData();
      }
    case success:
    case abort: /** @todo This part is never reached (please double check first someone), as above the state is set to "none" instead of "abort" */
      if ( ( c_devKey.isSpecified() ) && (!(getDinMonitorInstance4Comm().existDinMemberDevKey(c_devKey, true))) )
      { // lately terminal found - but not active any more -> reset
        c_devKey.setUnspecified();
        pc_terminal = NULL;
        if (en_maskUploadState == running)
        { // abort upload
          const uint8_t cui8_termNr = pc_terminal->nr();
          if (getSystemMgmtInstance4Comm().existActiveLocalDinMember())
          {
            const uint8_t cui8_localNr = getSystemMgmtInstance4Comm().getActiveLocalDinMember().nr();
            getMultiSendInstance4Comm().abortSend(cui8_localNr, cui8_termNr);
          }
          en_maskUploadState = none;
        }
        #ifdef DEBUG_RS232
        getRs232Instance() << "Terminal ging verloren\n\r";
        #endif
      }
      break;
  } // switch
  if ( ( c_devKey.isUnspecified() ) || (!(getDinMonitorInstance4Comm().existDinMemberDevKey(c_devKey, true))) ) {
    // none of the registered terminals active on BUS
    // -> clear array of process data, reset terminal pointer
    if ( ! arrSyncproc.empty() ) {
      arrSyncproc.clear();
      #ifdef DEBUG_HEAP_USEAGE
      sui16_syncProcTotal = 0;
      #endif
      #ifdef DEBUG_RS232
      getRs232Instance() << "loesche zuvor aktiven Eintrag\n\r";
      #endif
    }
    #ifdef DEBUG_RS232
    getRs232Instance() << "loesche zuvor aktiven Eintrag\n\r";
    #endif
    c_devKey.setUnspecified();
    pc_terminal = NULL;
    en_maskUploadState = none;
  }

  return true;
};

/**
  in some cases the Varioterminal doesn't detect the implement.
  Retrigger send of sync data every 500 msec.
*/
void DINMaskUpload_c::tryResendMaskSyncData()
{
  static int32_t si32_nextSyncRetrigger = 0;
  if ( System_c::getTime() >= si32_nextSyncRetrigger )
  { // resend mask data sync informations
    si32_nextSyncRetrigger = System_c::getTime() + 1000;
    getDinMonitorInstance4Comm().requestDinMemberNames();
    for ( Vec_SyncProcIterator pc_iter = arrSyncproc.begin();
          pc_iter != arrSyncproc.end();
          pc_iter++ )
    { // send proc data at pos
      if (activeMask().en_terminalType < IsoAgLib::FieldstarPlus) pc_iter->sendMasterMeasurementVal( c_devKey );
      else pc_iter->setpoint().sendMasterSetpointVal( c_devKey );
    }
  }
}



/**
  register a terminal LBS+ for potential mask upload
  @param rpsMask pointer to mask identifying structure
  @return true -> mask was registrated for upload
*/
bool DINMaskUpload_c::registerLbsPlusMask(IsoAgLib::t_maskDefinition* rpsMask)
{
  // call init() of MultiSend_c to be shure that this
  // class is initialized
  MultiSend_c::instance().init();
  if (ui8_maskDefCnt < REGISTRABLE_MASK_CNT)
  {
    psMaskDef[ui8_maskDefCnt] = rpsMask;
    ui8_maskDefCnt++;
    return true;
  }
  else
  {
    return false;
  }
}

/**
  start processing of a process msg
  ignore all invalid messages where SEND is not of a member with claimed address,
  or where EMPF isn't valid

  possible errors:
    * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
  @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
*/
bool DINMaskUpload_c::processMsg(){
  if (pc_terminal == NULL) return false;
  uint8_t ui8_localNr,
          ui8_termNr = pc_terminal->nr();

  if (getSystemMgmtInstance4Comm().existActiveLocalDinMember())
  {
    ui8_localNr = getSystemMgmtInstance4Comm().getActiveLocalDinMember().nr();
    // now check if received msg is a file upload start cmd
    if ( ((en_maskUploadState != none) && (en_maskUploadState != running)) && (data().send() == ui8_termNr) && (data().empf() == ui8_localNr) )
    { // terminal found -> check if start cmd received
      uint16_t ui16_msgCnt, ui16_ident, ui16_bytePerMsg;
      uint8_t b_uploadCmd;
      int32_t i32_dataSize;
      int32_t i32_swVers = 0xFFFFFFFF;
      HUGE_MEM uint8_t* phb_data;
      if (data().data(0) == 0)
      { // set the uint8_t order for 2 uint8_t integers specific for terminal
        if (activeMask().en_terminalType >= IsoAgLib::FieldstarPlus)
        { // FieldstarPlus or FieldstarOld
          data().setByteOrder(MSB_LSB);
          if (activeMask().en_terminalType == IsoAgLib::FieldstarOld)
          {
            float f_swVers;
            for (uint8_t ui8_syncInd = 0; ui8_syncInd < activeMask().ui8_syncCnt; ui8_syncInd++)
            {
              if (activeMask().pt_syncArray[ui8_syncInd].ui8_ind == FS_OLD_REQ_TERM_VERSION)
              {
                f_swVers = (float)((float)activeMask().pt_syncArray[ui8_syncInd].i32_val / 100.0F);
                uint8_t pb_temp1[4], pb_temp2[4];
                float2Int(&f_swVers, pb_temp1);
                // swap all bytes 1->4 ==> 4->1

                for (uint8_t ui8_tempInd = 0; ui8_tempInd < 4; ui8_tempInd++)
                {
                  pb_temp2[ui8_tempInd] = pb_temp1[3 - ui8_tempInd];
                }
                numberRef2LittleEndianString( i32_swVers, pb_temp2 );
                break;
              }
            }
          }
        }
        else
        { // set usual uint8_t order
          data().setByteOrder(LSB_MSB);
        }
        switch (data().data(1))
        {
          case 1: // request for mask upload
            // start cmd received -> send
            ui16_bytePerMsg = 1024;
            b_uploadCmd = 2;
            i32_dataSize = activeMask().ui32_maskSize;
            phb_data = activeMask().phb_maskData;
            break;
          case 3: // request for config upload
            ui16_bytePerMsg = 0x80;
            b_uploadCmd = 4;
            i32_dataSize = activeMask().ui16_configSize;
            phb_data = activeMask().phb_configData;
            break;
        } // switch
        ui16_msgCnt = i32_dataSize / ui16_bytePerMsg;
        // if size is not exact factor of ui16_bytePerMsg then increase amount of messages by 1
        if ( (i32_dataSize % ui16_bytePerMsg) != 0)ui16_msgCnt += 1;
        data().setData(1, b_uploadCmd);
        data().setData(0, (uint8_t)0);
        data().setData_2ByteInteger(2, ui16_msgCnt);
        data().setData(4, i32_swVers);
        ui16_ident = (0x700 | (ui8_termNr << 4) | ui8_localNr);
        // data().setIdent(ui16_ident, Ident_c::S);
        // CANPkg_c::setIdent changed to static
        CANPkg_c::setIdent(ui16_ident, Ident_c::StandardIdent);
        getCanInstance4Comm() << data();
        // register the send of multi msg stream
        en_maskUploadState = running;
        bool b_abortOnTimeout = (activeMask().en_terminalType == IsoAgLib::FendtVario)?true:false;
        getMultiSendInstance4Comm().sendDin(ui8_localNr, ui8_termNr, phb_data,
            i32_dataSize, ui16_bytePerMsg, en_sendSuccess, b_uploadCmd, b_abortOnTimeout);
      }
      return true;
    }
  }
  return false;
}

/**
  create the syncronisation process data for IsoAgLib+ mask upload
*/
void DINMaskUpload_c::createLbsplusProcdata()
{
  DevKey_c *pc_localDevKey = activeMask().pc_localDevKey;
  IsoAgLib::t_syncTupel* pt_syncTupel = activeMask().pt_syncArray;

  #ifdef USE_ISO_11783
  IsoAgLib::ElementDDI_s s_TempElementDDI[1] =
  {
    // termination entry
    {0xFFFF, false, GeneralCommand_c::noValue}
  };
  #endif

  arrSyncproc.push_front (syncproc_t (
                          #ifdef USE_ISO_11783
                          s_TempElementDDI, 0xFFFF,
                          #endif
                          0, 0xF, 4, LBS_PLUS_PROJECT_SIZE,
                          *pc_localDevKey,2, *pc_localDevKey, pc_localDevKey));
  if (activeMask().en_terminalType < IsoAgLib::FieldstarPlus) {
    arrSyncproc.begin()->setMasterMeasurementVal(static_cast<int32_t>(activeMask().ui32_maskSize));
    arrSyncproc.begin()->sendMasterMeasurementVal( c_devKey );
  }
  else {
    arrSyncproc.begin()->setpoint().setSetpointMasterVal(static_cast<int32_t>(activeMask().ui32_maskSize));
    arrSyncproc.begin()->setpoint().sendMasterSetpointVal( c_devKey );
  }

  // now create all syncronisation process data, where value is != 0
  for (uint8_t ui8_syncNoInd = 0; ui8_syncNoInd < activeMask().ui8_syncCnt; ui8_syncNoInd++)
  {
    if ((pt_syncTupel[ui8_syncNoInd].ui8_ind == 0) || (pt_syncTupel[ui8_syncNoInd].ui8_ind == LBS_PLUS_PROJECT_SIZE)) continue;

    arrSyncproc.push_front (syncproc_t (
                            #ifdef USE_ISO_11783
                            s_TempElementDDI, 0xFFFF,
                            #endif
                            0, 0xF, 4, pt_syncTupel[ui8_syncNoInd].ui8_ind,
                            *pc_localDevKey, 2, *pc_localDevKey, pc_localDevKey));

    if (activeMask().en_terminalType < IsoAgLib::FieldstarPlus) {
      arrSyncproc.begin()->setMasterMeasurementVal(pt_syncTupel[ui8_syncNoInd].i32_val);
      #if 1
      arrSyncproc.front().sendMasterMeasurementVal( c_devKey );
      #else
      if ( pt_syncTupel[ui8_syncNoInd].ui8_ind == LBS_PLUS_SYSTEM_STATUS ) {
        // send initial project state
        arrSyncproc.front().sendMasterMeasurementVal( c_devKey );
      }
      #endif
    }
    else {
      arrSyncproc.begin()->setpoint().setSetpointMasterVal(pt_syncTupel[ui8_syncNoInd].i32_val);
      #if 1
      arrSyncproc.front().setpoint().sendMasterSetpointVal( c_devKey );
      #else
      if ( pt_syncTupel[ui8_syncNoInd].ui8_ind == LBS_PLUS_SYSTEM_STATUS ) {
        // send initial project state
        arrSyncproc.front().setpoint().sendMasterSetpointVal( c_devKey );
      }
      #endif
    }
  }
  #ifdef DEBUG_HEAP_USEAGE
  sui16_syncProcTotal += ( activeMask().ui8_syncCnt + 1 );

  getRs232Instance()
    << sui16_syncProcTotal << " x DINMaskUpload_c(syncproc_t): Mal-Alloc: "
    <<  sizeSlistTWithMalloc( sizeof(syncproc_t), sui16_syncProcTotal )
    << "/" << sizeSlistTWithMalloc( sizeof(syncproc_t), 1 )
    << ", Chunk-Alloc: "
    << sizeSlistTWithChunk( sizeof(syncproc_t), sui16_syncProcTotal )
    << "\r\n\r\n";
  #endif
}

/**
  create the syncronisation process data for IsoAgLib+ mask upload
*/
void DINMaskUpload_c::createFieldstarProcdata()
{
  DevKey_c *pc_localDevKey = activeMask().pc_localDevKey;
  IsoAgLib::t_syncTupel* pt_syncTupel = activeMask().pt_syncArray;

#ifdef USE_FLOAT_DATA_TYPE
  #ifdef USE_ISO_11783
  // now create all syncronisation process data, where value is != 0
  IsoAgLib::ElementDDI_s s_TempElementDDI[1] =
  {
    // termination entry
    {0xFFFF, false, GeneralCommand_c::noValue}
  };
  #endif
  arrSyncproc.push_front (syncproc_t (
                          #ifdef USE_ISO_11783
                          s_TempElementDDI, 0xFFFF,
                          #endif
                          3, 0x0, FS_OLD_PROJECT_SIZE, 0xFF,
                          *pc_localDevKey, 5, *pc_localDevKey, pc_localDevKey));
  float f_temp = (float)activeMask().ui32_maskSize;
  arrSyncproc.begin()->setpoint().setSetpointMasterVal(f_temp);
  arrSyncproc.begin()->setpoint().sendMasterSetpointVal( c_devKey );

  for (uint8_t ui8_syncNoInd = 0; ui8_syncNoInd < activeMask().ui8_syncCnt; ui8_syncNoInd++)
  {
    if ((pt_syncTupel[ui8_syncNoInd].ui8_ind == 0) || (pt_syncTupel[ui8_syncNoInd].ui8_ind == FS_OLD_PROJECT_SIZE)) continue;
    if (pt_syncTupel[ui8_syncNoInd].ui8_ind == FS_OLD_SW_VERSION)
      arrSyncproc.push_front (syncproc_t (
                              #ifdef USE_ISO_11783
                              s_TempElementDDI, 0xFFFF,
                              #endif
                              3, 0xF, 0x3, 0xFF, *pc_localDevKey, 1,
                              *pc_localDevKey, pc_localDevKey));
    else
      arrSyncproc.push_front (syncproc_t (
                              #ifdef USE_ISO_11783
                              s_TempElementDDI, 0xFFFF,
                              #endif
                              3, 0x0, pt_syncTupel[ui8_syncNoInd].ui8_ind, 0xFF,
                              *pc_localDevKey , 5, *pc_localDevKey, pc_localDevKey));
    f_temp = (float)((float)pt_syncTupel[ui8_syncNoInd].i32_val/100.0F);
    arrSyncproc.begin()->setpoint().setSetpointMasterVal(f_temp);
    arrSyncproc.begin()->setpoint().sendMasterSetpointVal( c_devKey );
  }

#else
  // normally oldstyle fieldstar needs float version no - but if float is else not needed use normal
  // integer
  // main prog version at position 4
  // now create all syncronisation process data, where value is != 0
  arrSyncproc.push_front(syncproc_t(3, *pc_localDevKey, 0x0, 0xC, 0xFF, 5, *pc_localDevKey, pc_localDevKey));
  arrSyncproc.begin()->setpoint().setSetpointMasterVal(activeMask().ui32_maskSize);
  arrSyncproc.begin()->setpoint().sendMasterSetpointVal( c_devKey );

  for (uint8_t ui8_syncNoInd = 0; ui8_syncNoInd < activeMask().ui8_syncCnt; ui8_syncNoInd++)
  {
    if ((pt_syncTupel[ui8_syncNoInd].ui8_ind == 0) || (pt_syncTupel[ui8_syncNoInd].ui8_ind == LBS_PLUS_PROJECT_SIZE)) continue;
    if (pt_syncTupel[ui8_syncNoInd].ui8_ind == FS_OLD_SW_VERSION)
      arrSyncproc.push_front(syncproc_t(3, *pc_localDevKey, 0xF, 0x3, 0xFF, 1, *pc_localDevKey, pc_localDevKey));
    else
      arrSyncproc.push_front(syncproc_t(3, *pc_localDevKey, 0x0, pt_syncTupel[ui8_syncNoInd].ui8_ind, 0xFF, 5, *pc_localDevKey, pc_localDevKey));
    arrSyncproc.begin()->setpoint().setSetpointMasterVal(pt_syncTupel[ui8_syncNoInd].i32_val);
    arrSyncproc.begin()->setpoint().sendMasterSetpointVal( c_devKey );
  }
#endif
  #ifdef DEBUG_HEAP_USEAGE
  sui16_syncProcTotal += ( activeMask().ui8_syncCnt + 1 );

  getRs232Instance()
    << sui16_syncProcTotal << " x DINMaskUpload_c(syncproc_t): Mal-Alloc: "
    <<  sizeSlistTWithMalloc( sizeof(syncproc_t), sui16_syncProcTotal )
    << "/" << sizeSlistTWithMalloc( sizeof(syncproc_t), 1 )
    << ", Chunk-Alloc: "
    << sizeSlistTWithChunk( sizeof(syncproc_t), sui16_syncProcTotal )
    << "\r\n\r\n";
  #endif
}

} // end namespace __IsoAgLib
