/***************************************************************************
                          canio_c.cpp - master object for CAN communication;
                                      owns array of FilterBox_c and
                                      MsgObj_c instances
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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

#include "canio_c.h"
#include <functional>
#include <algorithm>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/util/impl/canpkgext_c.h>
#include <IsoAgLib/hal/system.h>
#ifdef USE_CAN_EEPROM_EDITOR
  #include <IsoAgLib/hal/eeprom.h>
#endif

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE) || defined(DEBUG_CAN_BUFFER_FILLING)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_msgObjTotal = 0;
static uint16_t sui16_filterBoxTotal = 0;
static uint16_t sui16_deconstructMsgObjCnt = 0;

#ifdef MASSERT
extern unsigned int AllocateHeapMalloc;
extern unsigned int DeallocateHeapMalloc;
#endif
#endif

namespace __IsoAgLib {
#if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique CANIO_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  CANIO_c& getCanInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return CANIO_c::instance( rui8_instance );
  }
#else
  /** C-style function, to get access to the unique CANIO_c singleton instance */
  CANIO_c& getCanInstance( void )
  {
    static CANIO_c &c_can_io = CANIO_c::instance();
    return c_can_io;
  }
#endif

/*******************************************/
/** definition of public element functions */
/*******************************************/

bool CANIO_c::sb_sendPrioritized=false;

void
CANIO_c::singletonInit()
{
  /// Settings taken form constructor
  ui8_busNumber = 0xFF;

  /// Default to NO maximum send delay detection
  i32_maxSendDelay = -1;

  /// singletonInit stuff
  init(0xFF, DEFAULT_BITRATE, DEFAULT_CONFIG_IDENT_TYPE, CONFIG_CAN_DEFAULT_MIN_OBJ_NR, CONFIG_CAN_DEFAULT_MAX_OBJ_NR);
}

/** Initialize the CAN hardware, and instantiate one msg object for
   sending of messages. Do configuration for BUS number, sending bitrate,
  CAN ident length, minx/max hardware/BIOS Msg Obj numbers by parameters;
  called by specified constructor or external functions;
  wrong BUS and msg obj numbers are rejected and cause set of Err_c:range

  If even the rui8_busNumber parameter has the default value 0xFF,
  then the configuration settings of a previous init call are not
  changed. In this case, the CAN BUS is only reset with the old settings.
  This is enabled by the default value 0xFF for rui8_busNumber, which is
  changed to CONFIG_CAN_DEFAULT_BUS_NUMBER for the first call of init() after the constructor.
  In all other cases, the special value 0xFF is indicator for empty parameter list.

  possible errors:
      * Err_c::range on undefined BUS,  msgOb_nr or sendPufferSize,
      * Err_c::hwConfig on uninitialized BUS, undef. msgType or CAN-BIOS mem-err,
      * Err_c::busy on already used sending Msg-Obj
  @param rui8_busNumber optional number of the CAN bus
  @param rui16_bitrate optional bitrate (default by define in isoaglib_config.h)
  @param identType_t optional length of the ident
    (S (11bit), E (29bit), B
    (send both standard and extended ident msg) (default by define in isoaglib_config.h)
  @param rui8_minObjNr optional minimum number for hardware CAN message object
    (important for sharing CAN controller with other tasks) (default by define in isoaglib_config.h)
  @param rui8_maxObjNr optional maximum number for hardware CAN message object
    (default by define in isoaglib_config.h)
  @return true -> correct initialisation without errors
  @see HAL::can_configGlobalInit
  @see HAL::can_configMsgobjInit
  @see Ident_c::t_identType
*/
  bool CANIO_c::init(uint8_t rui8_busNumber, uint16_t rui16_bitrate,
                  Ident_c::identType_t ren_identType,
                  uint8_t rui8_minObjNr,
                  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
                  uint8_t rui8_maxObjNr
                  #else
                  // this parameter is NOT needed with SYSTEM_WITH_ENHANCED_CAN_HAL
                  uint8_t /*rui8_maxObjNr*/
                  #endif
                  )
{ // first make shure that the base system is initialized
  getSystemInstance().init();
  b_runningCanProcess = false;
  // if FilterBox_c instances were created before the CAN_IO was
  // explicitly initialized, we must call reconfigureMsgObj NOW
  bool b_callReconfigureMsgObj = false;

  #ifdef DEBUG
  static bool firstDefaultInitCallStart = true;
  if ( firstDefaultInitCallStart ) {
    firstDefaultInitCallStart = false;
    INTERNAL_DEBUG_DEVICE
        << "Start CANIO_c::init() mit BUS: " << int(rui8_busNumber)
        << ", Bitrate: " << int(rui16_bitrate) << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif

  if ( ( rui8_busNumber != 0xFF ) || ( rui8_busNumber != ui8_busNumber ) )
  { // this is the first call of init after constructor,
    // or at least the BUS Number parameter is different from
    // default value -> interprete as explicit call which can
    // override old setting

    // close CAN Bus, if ui8_busNumber is valid at the
    // moment (this can be a reconfig after a default
    // config)
    if ( ui8_busNumber <= HAL_CAN_MAX_BUS_NR )
    {
      #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
      for (uint8_t i = 0; i < arrFilterBox.size(); i++)
        arrFilterBox[i].closeHAL();
      #endif
      HAL::can_configGlobalClose(ui8_busNumber);
    }

    /* ************************************* */
    /* *****set initial attribute values**** */
    /* ************************************* */
    // set t_mask to clear values
    en_identType = DEFAULT_CONFIG_IDENT_TYPE;

    // set object vars to 0 detect forgotten init call
    ui8_busNumber = 0;
    setMinHALMsgObjNr(0);
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    setMaxHALMsgObjNr(0);
    #endif
    ui16_bitrate = 0;
    i32_canErrStart = 0;
    i32_canErrEnd = 0;
    ui8_processedMsgCnt = 0;
    c_maskStd.set(~0, Ident_c::StandardIdent);
    c_maskExt.set(~0, Ident_c::ExtendedIdent);
    c_maskLastmsg.set(0, DEFAULT_IDENT_TYPE);

    // check if some FilterBox_c instances were already created, so that
    // we must call reconfigureMsgObj NOW
    if ( arrFilterBox.size() > 0 )
    {
      b_callReconfigureMsgObj = true;
      #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE
        #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
          << "Call reconfigureMsgObj as some FilterBox_c instances are already created"
        #else
          << "there were already some FilterBox_c instances, that should be reactivated with fresh initialized CAN"
        #endif
          << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
    }
  }
  if ( rui8_busNumber == 0xFF )
  { // called with default value -> do nothing till called with normal parameter
    #ifdef DEBUG
    static bool firstDefaultInitCallEnd = true;
    if ( ! firstDefaultInitCallEnd ) {
      firstDefaultInitCallEnd = false;
      INTERNAL_DEBUG_DEVICE
          << "Ende CANIO_c::init() mit Default Werten bei spc_instance == "
          << uint32_t( spc_instance )
          << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    #endif
    return false;
  }
  else
  {
    /* ****************************** */
    /* *****check input parameter**** */
    /* ****************************** */
    // check rui8_minObjNr, rui8_maxObjNr and rui8_busNumber
    if (
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
          (rui8_minObjNr > HAL_CAN_MAX_SEND_OBJ)
        ||(rui8_maxObjNr > HAL_CAN_MAX_SEND_OBJ)
      #if HAL_CAN_MIN_SEND_OBJ > 0
        ||(rui8_minObjNr < HAL_CAN_MIN_SEND_OBJ) // use this comparison only for (HAL_CAN_MIN_SEND_OBJ > 0), else it gives a warning!
        ||(rui8_maxObjNr < HAL_CAN_MIN_SEND_OBJ) // use this comparison only for (HAL_CAN_MIN_SEND_OBJ > 0), else it gives a warning!
      #endif
        ||
    #else
        #if HAL_CAN_MIN_SEND_OBJ > 0
        (rui8_minObjNr < HAL_CAN_MIN_SEND_OBJ)
        ||
        #endif
    #endif
        (rui8_busNumber > HAL_CAN_MAX_BUS_NR)
        )
    { // one of the range tests not passed
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE
          << "Ende CANIO_c::init() mit falschen Parametern" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return false; // exit function with error information
    }

    /* ******************************************************* */
    /* ****implementing of function with passed parameters**** */
    /* ******************************************************* */

    /** set min and max allowed obj numbers (important for multithreading
        where each threads manages only a portion of all objects)
    */
    setMinHALMsgObjNr(rui8_minObjNr);
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    setMaxHALMsgObjNr(rui8_maxObjNr);
    #endif
    // store given BUS number: if default value 0xFF is given, use
    // the default defined value
    ui8_busNumber = (rui8_busNumber != 0xFF)?rui8_busNumber:CONFIG_CAN_DEFAULT_BUS_NUMBER;
    // store wanted CAN identifier type
    en_identType = ren_identType;
  }
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE
      << "Ende CANIO_c::init()" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  // even for call of init without parameter the base can init
  // should be repeated
  // check and store bitrate, config CAN and send object(s)
  const bool b_result = baseCanInit(rui16_bitrate);

  if ( b_callReconfigureMsgObj )
  {
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    reconfigureMsgObj();
    #else
    for (uint8_t i = 0; i < arrFilterBox.size(); i++)
      arrFilterBox[i].configCan(ui8_busNumber, i + minReceiveObjNr() );
    #endif
  }
  return b_result;
}

/** every subsystem of IsoAgLib has explicit function for controlled shutdown */
void CANIO_c::close( void )
{
  if ( ui8_busNumber == 0xFF )
  { // CAN already closed -> don't call HAL close again
    return;
  }

  #if defined SYSTEM_WITH_ENHANCED_CAN_HAL
  for (uint8_t i = 0; i < arrFilterBox.size(); i++)
    arrFilterBox[i].closeHAL();
  #endif

  // call BIOS CAN close function - set error state if BIOS function cause error
  switch (HAL::can_configGlobalClose(ui8_busNumber) )
  {
    case HAL_NO_ERR:
      break;
    case HAL_CONFIG_ERR:
      // ignore this type also, as this is only the indication of try to close an already closed channel
      #if defined(DEBUG)
      INTERNAL_DEBUG_DEVICE << "\r\nBUS " << uint16_t(ui8_busNumber) << " was already closed before close call" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      break;
    default:
      // wrong channel
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
      break;
  }
  #ifdef DEBUG_HEAP_USEAGE
  sui16_filterBoxTotal -= arrFilterBox.size();
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    sui16_msgObjTotal -= arrMsgObj.size();
    #endif
  #endif
  arrFilterBox.clear();
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  arrMsgObj.clear();
  #endif
  ui8_busNumber = 0xFF;
}


/** periodically called function which does
  periodically needed actions; f.e. trigger watch
  dog and start processing received messages
  AND call init if ui16_bitrate is still set to 0 -> no CAN init performed
  @return true -> time events was performed
*/
bool CANIO_c::timeEvent( void ){
  // check if init was called
  if ( (ui16_bitrate == 0) || ( ui8_busNumber > HAL_CAN_MAX_BUS_NR ) )
  { // this can't be the case after correct call of init
    init(0xFF, DEFAULT_BITRATE, DEFAULT_CONFIG_IDENT_TYPE, CONFIG_CAN_DEFAULT_MIN_OBJ_NR
         , CONFIG_CAN_DEFAULT_MAX_OBJ_NR ); // call with default values
  }
  // if still not ready, CANIO_c is not yet initialised complete -> do nothing
  if ( (ui16_bitrate == 0) || ( ui8_busNumber > HAL_CAN_MAX_BUS_NR ) ) return false;

  #if ( CONFIG_CAN_USE_FALLBACK_ON_CAN_DEADLOCK )
  // check if CAN BUS is o.k.
  int32_t i32_now = HAL::getTime();
  int32_t i32_lastSend;
  bool b_sendProblem = false;
  uint8_t ui8_bufCnt;

  i32_lastSend = HAL::can_stateMsgobjTxok(ui8_busNumber, minHALMsgObjNr());
  ui8_bufCnt = HAL::can_stateMsgobjBuffercnt(ui8_busNumber, minHALMsgObjNr());

  if ( ((i32_now - i32_lastSend) > 3000)
     &&(ui8_bufCnt > 0)
     )
  {
    b_sendProblem = true;
  }
  if (en_identType == Ident_c::BothIdent)
  { // check for second obj if proprietary CAN and ISO are used
    i32_lastSend = HAL::can_stateMsgobjTxok(ui8_busNumber, minHALMsgObjNr()+1 );
    ui8_bufCnt = HAL::can_stateMsgobjBuffercnt(ui8_busNumber, minHALMsgObjNr()+1 );

    if ( ((i32_now - i32_lastSend) > 3000)
       &&(ui8_bufCnt > 0)
       )
    {
      b_sendProblem = true;
    }
  }
  if (HAL::can_stateGlobalBlocked()) b_sendProblem = true;
  if (b_sendProblem)
  { // a send problem occured -> close CAN and open again
    setBitrate(CONFIG_CAN_FALLBACK_BITRATE);
  }
  #endif //CONFIG_CAN_USE_FALLBACK_ON_CAN_DEADLOCK

  // start process of all received msg
  return (processMsg() >= 0)?true:false;
}

/** deliver actual BUS load in baud
  @return baudrate in [kbaud] on used CAN BUS
*/
uint16_t CANIO_c::getBusLoad() const
{
  return HAL::can_stateGlobalBusload(ui8_busNumber);
}

/** set the minimum delay in msec. between two sent CAN messages
  @param rui16_minDelay minimum time between two CAN messages [msec.]
*/
void CANIO_c::setSendpause(uint16_t rui16_minDelay) const
{ // set send MsgObj ID
  uint8_t ui8_sendObjNr = minHALMsgObjNr();

  HAL::can_configMsgobjSendpause(ui8_busNumber, ui8_sendObjNr, rui16_minDelay);
  // if both standard and extended ident can be sent -> use second send obj if data uses extended ident
  if (en_identType == Ident_c::BothIdent)
  {
    HAL::can_configMsgobjSendpause(ui8_busNumber, (ui8_sendObjNr + 1), rui16_minDelay);
  }
}

/** deliver the numbers which can be placed at the moment in the send puffer
  @param ren_identType type of searched ident: standard 11bit or extended 29bit
    (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
  @return number of msgs which fit into send buffer
*/
uint8_t CANIO_c::sendCanFreecnt(Ident_c::identType_t ren_identType)
{ // set send MsgObj ID
  uint8_t ui8_sendObjNr = minHALMsgObjNr();

  // if both standard and extended ident can be sent -> use second send obj if data uses extended ident
  if ((en_identType == Ident_c::BothIdent) && (ren_identType ==Ident_c::ExtendedIdent))
  {
    ui8_sendObjNr += 1;
  }
  const int16_t i16_result = HAL::can_stateMsgobjFreecnt(ui8_busNumber, ui8_sendObjNr);

  if ( i16_result >= 0 )
    return uint8_t(i16_result);
  else
    return 0;
}


/** clear the send buffer
  @param ren_identType type of searched ident: standard 11bit or extended 29bit
    (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
*/
void CANIO_c::sendCanClearbuf(Ident_c::identType_t ren_identType)
{
  uint8_t ui8_sendObjNr = minHALMsgObjNr();

  // if both standard and extended ident can be sent -> use second send obj if data uses extended ident
  if ((en_identType == Ident_c::BothIdent) && (ren_identType ==Ident_c::ExtendedIdent))
  {
    ui8_sendObjNr += 1;
  }
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE
   << "CANIO_c::sendCanClearbuf for MsgObjNr: " << uint16_t(ui8_sendObjNr) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  HAL::can_useMsgobjClear(ui8_busNumber, ui8_sendObjNr);
}


/** test if a FilterBox_c definition already exist
  (version expecial for standard ident, chosen at compile time)
  @param rref_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param rt_mask individual mask for this filter box
  @param rt_filter individual filter
  @param ren_identType type of searched ident: standard 11bit (default) or extended 29bit
  @param rpc_iter optional pointer Iterator to result FilterBox
  @return true -> same FilterBox_c already exist
*/
bool CANIO_c::existFilter(const __IsoAgLib::CANCustomer_c& rref_customer,
                          uint16_t rt_mask, uint16_t rt_filter,
                          Ident_c::identType_t ren_identType, ArrFilterBox::iterator* rpc_iter)
{
  // check if filter/t_mask are already inserted
  // return false if this setting isnt unique
  Ident_c c_compFilter = Ident_c(rt_filter, ren_identType);
  Ident_c c_compMask = Ident_c(rt_mask, ren_identType);
  c_compFilter.set(rt_filter, ren_identType);
  c_compMask.set(rt_mask, ren_identType);

  return existFilter(rref_customer, c_compMask, c_compFilter, rpc_iter);
}
/** test if a FilterBox_c definition already exist
  (version expecial for extended ident, chosen at compile time)
  @param rref_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param rt_mask individual mask for this filter box
  @param rt_filter individual filter
  @param ren_identType type of searched ident: standard 11bit (default) or extended 29bit
  @param rpc_iter optional pointer Iterator to result FilterBox
  @return true -> same FilterBox_c already exist
*/
bool CANIO_c::existFilter(const __IsoAgLib::CANCustomer_c& rref_customer,
                          uint32_t rt_mask, uint32_t rt_filter,
                          Ident_c::identType_t ren_identType, ArrFilterBox::iterator* rpc_iter)
{
  // check if filter/t_mask are already inserted
  // return false if this setting isnt unique
  Ident_c c_compFilter = Ident_c(rt_filter, ren_identType);
  Ident_c c_compMask = Ident_c(rt_mask, ren_identType);
  c_compFilter.set(rt_filter, ren_identType);
  c_compMask.set(rt_mask, ren_identType);

  return existFilter(rref_customer, c_compMask, c_compFilter, rpc_iter);
}

/** test if a FilterBox_c definition already exist
  (version with comper items as Ident_c class instances, chosen by compiler)
  @param rref_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param rc_compMask individual mask for this filter box
  @param rc_compFilter individual filter
  @param rpc_iter optional pointer Iterator to result FilterBox
  @return true -> same FilterBox_c already exist
*/
bool CANIO_c::existFilter(const __IsoAgLib::CANCustomer_c& rref_customer,
                          const Ident_c& rc_compMask, const Ident_c& rc_compFilter,
                          ArrFilterBox::iterator* rpc_iter)
{
  // check if filter/t_mask are already inserted
  // return false if this setting isn't unique
  bool b_identDefFound = false;

  // iterator for quick access to all array elements
  ArrFilterBox::iterator pc_iter = arrFilterBox.begin();

  // check if given FilterBox_c definition is not yet in array
  for (; pc_iter != arrFilterBox.end(); pc_iter++)
  {
    if ( ( pc_iter->equalFilterMask(rc_compMask, rc_compFilter ) )
      && ( pc_iter->equalCustomer( rref_customer )               ) )
    { // FilterBox_c with equal def found
      //-> don't insert complete ident def two times
      b_identDefFound = true;
      // don't search rest of array
      break;
    }
  }
  if (b_identDefFound && (rpc_iter != NULL)) *rpc_iter = pc_iter;
  return b_identDefFound;
}

/** create a Filter Box with specified rt_mask/rt_filter
  on ui8_busNr of object; reconfig HW CAN MsgObj_c only if
  rb_reconfigImmediate == true -> useful for
  avoiding unneeded reconfiguration during
  sequence of FilterBox_c insertions;
  by rref_customer CANIO_c (FilterBox_c) can start direct processing
  of received data in dedicated customer object (no search);
  uses BIOS functions

  This specialized function allows the user to use the same message
  object as the filter box that is given "rpc_connectedFilterBox"
  Use this feature if messages from two or more filterboxes have to be processed chronologically

  possible errors:
      * Err_c::badAlloc on not enough memory for new FilterBox_c instance or for new configured MsgObj_c's
  @see __IsoAgLib::CANCustomer
  @param rref_customer reference to __IsoAgLib::CANCustomer_c which needs filtered
      messages (-> on received msg call rref_customer.processMsg())
  @param rt_mask individual mask for this filter box
  @param rt_filter individual filter
  @param rb_reconfigImmediate true -> all Filter objects are reconfigured to according
      CAN hardware MsgObj after creating this filter
  @param rt_identType ident type of the created ident: standard 11bit (default) or extended 29bit
  @return != NULL -> if inserting and wanted reconfiguration are performed without errors, a reference to the created FilterBox is returned
  @exception badAlloc
*/
FilterBox_c* CANIO_c::insertFilter(__IsoAgLib::CANCustomer_c& rref_customer,
                                  uint32_t rt_mask, uint32_t rt_filter,
                                  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
                                  bool rb_reconfigImmediate,
                                  #else
                                  // this parameter is NOT needed with SYSTEM_WITH_ENHANCED_CAN_HAL
                                  bool /*rb_reconfigImmediate*/,
                                  #endif
                                  const Ident_c::identType_t rt_identType,
                                  int8_t ri8_dlcForce,
                                  FilterBox_c* rpc_connectedFilterBox)
{
  Ident_c c_newMask = Ident_c(rt_mask, rt_identType);
  Ident_c c_newFilter = Ident_c(rt_filter, rt_identType);

  if (existFilter(rref_customer, c_newMask, c_newFilter, NULL))
  { // filter,mask,cancustomer definition already inserted
    return NULL;
  }
  //look for existing mask/filter definition
  FilterBox_c* tempFilterBox_c = getFilterBox(c_newMask, c_newFilter);

  if (tempFilterBox_c != NULL)
  { //cancustomer does not exist -> insert
    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "filterbox mask/filter already exist -> insert cancustomer" << INTERNAL_DEBUG_DEVICE_ENDL;
    #ifdef SYSTEM_PC
    INTERNAL_DEBUG_DEVICE << "mask: " << std::hex << c_newMask.ident() << " filter: " << c_newFilter.ident() << std::dec << INTERNAL_DEBUG_DEVICE_ENDL;
    #else
    INTERNAL_DEBUG_DEVICE << "mask: " << c_newMask.ident() << " filter: " << c_newFilter.ident() << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    #endif
    tempFilterBox_c->insertCustomer (&rref_customer, ri8_dlcForce);
    //do not insert new filterbox because it already exists
    return tempFilterBox_c;
  }
  // now the filterBox def is new -> insert new FilterBox_c instance in array

  // use for insert a member var FilterBox_c to avoid creation of temp obj during insert
  // clear temp object
  c_tempFilterBox.clearData();

  // define temp FilterBox_c with new values
  c_tempFilterBox.set(c_newMask, c_newFilter, &rref_customer, ri8_dlcForce, rpc_connectedFilterBox);

  // insert new FilterBox_c and exit function if no dyn array growth is reported
  const uint8_t b_oldSize = arrFilterBox.size();

  #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( ui8_busNumber == 0xFF )
  { //if insertFilter is called before canio_c init call, insertFilterBox without setting busnumber
    //and msgobj number -> this will be done in init call
    arrFilterBox.push_back(c_tempFilterBox);
    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "---BEFORE CALL OF INIT---" << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "insertFilterBox with bus number: " << static_cast<int>(ui8_busNumber)  << " and FilterBoxNr: " << static_cast<int>(b_oldSize) << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "                     mask: " << std::hex << c_newMask.ident() << " filter: " << c_newFilter.ident() << std::dec << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return &arrFilterBox.back();
  }

  // #############################################
  // from now on, the CAN BUS is already in valid useable state, so that the FilterBox_c instance
  // can immediately connect to HAL
  // #############################################

  //check if an idle filterBox exist and if not insert new filterBox at the end of array
  bool filterBoxOverwrite = false;
  //search array for a filterbox that is currently not in use and therefore idle
  uint8_t ui8_overwritenFilterBoxIndex = 0;
  for(; ui8_overwritenFilterBoxIndex < arrFilterBox.size(); ui8_overwritenFilterBoxIndex++)
  {
    if ( arrFilterBox[ui8_overwritenFilterBoxIndex].isIdle() )
    { //if idle filterbox found overwrite it with the new filterbox
      arrFilterBox[ui8_overwritenFilterBoxIndex].set(c_newMask, c_newFilter, &rref_customer, ri8_dlcForce, rpc_connectedFilterBox);
      arrFilterBox[ui8_overwritenFilterBoxIndex].configCan(ui8_busNumber, ui8_overwritenFilterBoxIndex + minReceiveObjNr());
      filterBoxOverwrite = true;
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "overwrite FilterBox[" << int(ui8_overwritenFilterBoxIndex) << "] with busnumber: " << static_cast<int>(ui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "                    mask: " << std::hex << c_newMask.ident() << " filter: " << c_newFilter.ident() << std::dec << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      break;
    }
  }

  if (!filterBoxOverwrite)
  { //only insert filter box if bus number and msgobj number have the right range
    // no filterBox in the vector is idle -> push back
    // insert temp object in vector arrFilterBox -> can cause badAlloc exception
    uint8_t tempMsgObjNr = minReceiveObjNr() + b_oldSize;
    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "insertFilterBox with bus number: " << static_cast<int>(ui8_busNumber)  << " and FilterBoxNr: " << static_cast<int>(b_oldSize) << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "                     mask: "  << std::hex << c_newMask.ident() << " filter: " << c_newFilter.ident() << std::dec << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    arrFilterBox.push_back(c_tempFilterBox);
    arrFilterBox.back().configCan(ui8_busNumber, tempMsgObjNr);
  }
  #else // SYSTEM_WITH_ENHANCED_CAN_HAL
  // insert temp object in vector arrFilterBox -> can cause badAlloc exception
  arrFilterBox.push_front(c_tempFilterBox);
  #endif // SYSTEM_WITH_ENHANCED_CAN_HAL

  if (b_oldSize >= arrFilterBox.size())
  { // dynamic array didn't grow -> alloc error
    getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Can );
    return NULL; // exit the function
  }
  #ifdef DEBUG_HEAP_USEAGE
  else
  {
    sui16_filterBoxTotal++;
  }
  #endif

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  // if wanted, configure everything, to set CAN hardware MsgObj_c as wanted
  // for sequence of arrFilterBox insertions only configure after the last insert
  if ( rb_reconfigImmediate )
  {
    reconfigureMsgObj();
  } // if (rb_configImmediate)

  // insertion of FilterBox_c was successfull
  return &arrFilterBox.front();
  #else // SYSTEM_WITH_ENHANCED_CAN_HAL
  if (filterBoxOverwrite == true)
    return &arrFilterBox[ui8_overwritenFilterBoxIndex];
  else //new filterBox inserted at the end
    return &arrFilterBox.back();
  #endif
}

/** create a Filter Box with specified rt_mask/rt_filter
  on ui8_busNr of object; reconfig HW CAN MsgObj_c only if
  rb_reconfigImmediate == true -> useful for
  avoiding unneeded reconfiguration during
  sequence of FilterBox_c insertions;
  by rref_customer CANIO_c (FilterBox_c) can start direct processing
  of received data in dedicated customer object (no search);
  uses BIOS functions

  This specialized function allows the user to use the same message
  object as the filter box that exists with the given extra rt_mask/rt_filter pair!
  Use this feature if messages from two or more filterboxes have to be processed chronologically

  possible errors:
      * Err_c::badAlloc on not enough memory for new FilterBox_c instance or for new configured MsgObj_c's
  @see __IsoAgLib::CANCustomer
  @param rref_customer reference to __IsoAgLib::CANCustomer_c which needs filtered
      messages (-> on received msg call rref_customer.processMsg())
  @param rt_mask individual mask for this filter box
  @param rt_filter individual filter
  @param rb_reconfigImmediate true -> all Filter objects are reconfigured to according
      CAN hardware MsgObj after creating this filter
  @param rt_identType ident type of the created ident: standard 11bit (default) or extended 29bit
  @return != NULL -> if inserting and wanted reconfiguration are performed without errors, a reference to the created FilterBox is returned
  @exception badAlloc
*/
FilterBox_c* CANIO_c::insertFilter(__IsoAgLib::CANCustomer_c& rref_customer,
                                   uint32_t rt_mask, uint32_t rt_filter,
                                   bool rb_reconfigImmediate,
                                   const Ident_c::identType_t rt_identType, uint32_t rt_connectedMask,
                                   uint32_t rt_connectedFilter, const Ident_c::identType_t rt_connectedIdentType,
                                   int8_t ri8_dlcForce
                                  )
{
  Ident_c c_connectedMask = Ident_c(rt_connectedMask, rt_connectedIdentType);
  Ident_c c_connectedFilter = Ident_c(rt_connectedFilter, rt_connectedIdentType);

  bool b_connectedFilterFound = false;

  // check if given FilterBox_c definition is in array
  ArrFilterBox::iterator pc_iter = arrFilterBox.begin();
  for (; pc_iter != arrFilterBox.end(); pc_iter++)
  {
    if ( pc_iter->equalFilterMask(c_connectedMask, c_connectedFilter ) )
    { // FilterBox_c with equal def (mask, filter, NOT customer) found
      b_connectedFilterFound = true;
      break; // don't search rest of array
    }
  }

  if (!b_connectedFilterFound) // "existFilter()" not used as it also compares the customer!
    return NULL;

  return insertFilter(rref_customer, rt_mask, rt_filter, rb_reconfigImmediate, rt_identType, ri8_dlcForce, &(*pc_iter));
}

/** delete a FilterBox definition
  @param rref_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param rt_mask individual mask for this filter box
  @param rt_filter individual filter
  @param rt_identType ident type of the deleted ident: standard 11bit or extended 29bit
        (defualt DEFAULT_IDENT_TYPE defined in isoaglib_config.h)
  @return true -> FilterBox_c found and deleted
  // @todo: additional boolean parameter for skipping reconfigureMsgObj
*/
bool CANIO_c::deleteFilter(const __IsoAgLib::CANCustomer_c& rref_customer,
                           MASK_TYPE rt_mask, MASK_TYPE rt_filter,
                           const Ident_c::identType_t rt_identType)
{
  bool b_result = false;
  Ident_c c_deleteMask = Ident_c(rt_mask, rt_identType),
        c_deleteFilter = Ident_c(rt_filter, rt_identType);
  // iterator for quick access to all array elements
  ArrFilterBox::iterator pc_iter;

  if (existFilter(rref_customer, c_deleteMask, c_deleteFilter, &pc_iter))
  { // filter found -> delete element where pc_iter points to
    if ( pc_iter->deleteFilter(rref_customer) )
    { //no more cancustomer exist for the filterbox -> delete
      #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
      //to be deleted filterbox is set to idle
      while( arrFilterBox.back().isIdle() )
      { //remove idle filterBox if at the end of vector
        arrFilterBox.pop_back();
      }
      #else
      arrFilterBox.erase(pc_iter);
      #endif
    }

    #ifdef DEBUG_HEAP_USEAGE
    sui16_filterBoxTotal--;

    INTERNAL_DEBUG_DEVICE
      << sui16_filterBoxTotal << " x FilterBox_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(FilterBox_c), sui16_filterBoxTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(FilterBox_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(FilterBox_c), sui16_filterBoxTotal )
    #ifdef MASSERT
      << "\r\n__mall tot:" << AllocateHeapMalloc
      << ", _mall deal tot: " << DeallocateHeapMalloc
      << INTERNAL_DEBUG_DEVICE_ENDL;
    #else
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    #endif

    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    reconfigureMsgObj();
    #endif
    b_result = true;
  }
  return b_result;
}





bool CANIO_c::deleteAllFiltersForCustomer (const __IsoAgLib::CANCustomer_c& rref_customer)
{
  bool b_result = false;

  // iterator for quick access to all array elements
  ArrFilterBox::iterator pc_iter = arrFilterBox.begin();

  for (; pc_iter != arrFilterBox.end();)
  {
    if ( pc_iter->deleteFilter (rref_customer) )
    { // returned TRUE, so complete filterbox got empty -> delete complete filterbox
      pc_iter = arrFilterBox.erase (pc_iter);
      b_result = true;
    }
    else
    { // returned FALSE, so still some filters in that filterbox -> keep it
      pc_iter++;
    }
/** @todo According to another function, this has been done - but what's the sense of it in ENHANCED_CAN_HAL and do we need to care here, too?
      #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
      //to be deleted filterbox is set to idle
      while( arrFilterBox.back().isIdle() )
      { //remove idle filterBox if at the end of vector
        arrFilterBox.pop_back();
      }
      #else
      arrFilterBox.erase(pc_iter);
      #endif
*/
  }

  if (b_result)
  { // at least one filter/customer was removed
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    reconfigureMsgObj();
    #endif
  }

  return b_result;
}


/**
  initiate processing of all received msg
  check all active MsgObj_c for received CAN msg and
  initiate their processing
  @return <0 --> not enough time to process all messages.
         ==0 --> no messages were received.
         >0  --> all messages are processed, number of messages
*/
int16_t CANIO_c::processMsg(){
  // immediately leave this function, if this CAN_IO instance is marked as currently processing
  // --> detect and avoid CAN message processing loops
  if ( b_runningCanProcess ) return -1;
  b_runningCanProcess = true;
  ui8_processedMsgCnt = 0;

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  // first check if last Message Object is open,
  // and process these messages first
  // - this happens, if this is first processMsg call after
  //   reconfigureMsgObj
  if ( c_lastMsgObj.isOpen() )
  { // process all messages which where placed during the reconfig in the last message object
    ui8_processedMsgCnt += c_lastMsgObj.processMsg( ui8_busNumber, true );
    c_lastMsgObj.close();
  }

  for (ArrMsgObj::iterator pc_iter = arrMsgObj.begin(); pc_iter != arrMsgObj.end(); pc_iter++)
  { // stop processing of message buffers, if not enough time
    if ( ElementBase_c::getAvailableExecTime() == 0 )
    { // switch the flag back, so that further processings are enabled
      b_runningCanProcess = false;
      return -1;
    }
    System_c::triggerWd();
    ui8_processedMsgCnt += pc_iter->processMsg(ui8_busNumber);
  }
  #else //SYSTEM_WITH_ENHANCED_CAN_HAL

  #ifdef DEBUG_CAN_BUFFER_FILLING
  doDebug(ui8_busNumber);
  #endif
  // process all CAN msgs as long as some time available for processing
  // and messages exist in queue
  // ( the time check is needed to avoid blocking of other important tasks if
  //   to much CAN msgs are received )
  int32_t i32_ident, i32_nextMsgObjRetVal = 0;
  bool b_toProcess = true;
  bool b_forceProcessAll = false;
  //look for messages in queue
  for ( i32_nextMsgObjRetVal = HAL::can_useNextMsgobjNumber(ui8_busNumber, i32_ident);
        i32_nextMsgObjRetVal != HAL_UNKNOWN_ERR; // exit the for loop, as soon as HAL indicates that no more CAN msg are available
        i32_nextMsgObjRetVal = HAL::can_useNextMsgobjNumber(ui8_busNumber, i32_ident) )
  {
    b_toProcess = true;
    switch (i32_nextMsgObjRetVal)
    {
      case HAL_NO_ERR:
        break;
      case HAL_RANGE_ERR:
        getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "CAN-Receive Range Err" << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        HAL::can_useMsgobjPopFront(ui8_busNumber, i32_ident);
        b_runningCanProcess = false;
        return ui8_processedMsgCnt;
      case HAL_CONFIG_ERR:
        #if defined(DEBUG_CAN_BUFFER_FILLING) || defined(DEBUG)
        INTERNAL_DEBUG_DEVICE << "\r\nBUS not initialized or wrong BUS nr: " << uint16_t(ui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        getILibErrInstance().registerError( iLibErr_c::HwConfig, iLibErr_c::Can );
        HAL::can_useMsgobjPopFront(ui8_busNumber, i32_ident);
        b_runningCanProcess = false;
        return ui8_processedMsgCnt;
      case HAL_NOACT_ERR:
        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "CAN-Receive NoAct Err" << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        // wrong use of MsgObj (not likely) or CAN BUS OFF
        getILibErrInstance().registerError( iLibErr_c::CanOff, iLibErr_c::Can );
        HAL::can_useMsgobjPopFront(ui8_busNumber, i32_ident);
        b_runningCanProcess = false;
        return ui8_processedMsgCnt;
      case HAL_WARN_ERR:
        getILibErrInstance().registerError( iLibErr_c::CanWarn, iLibErr_c::Can );
        break;
      case HAL_OVERFLOW_ERR:
        // CAN BUFFER Overflow can most probably happen on process of
        // messages in last msg obj, where ALL CAN messages are placed
        // during reconfiguration
        getILibErrInstance().registerError( iLibErr_c::CanOverflow, iLibErr_c::Can );
        HAL::can_stateMsgobjOverflow(ui8_busNumber, i32_ident );
        #ifdef DEBUG_CAN_BUFFER_FILLING
        if ( ! b_detectedOverflow )
        {
          INTERNAL_DEBUG_DEVICE << "\r\nALARM!!!!!! CAN Buffer Overflow at MsgObj: "
            << uint16_t(i32_ident) << " at BUS: " << uint16_t(rui8_busNumber)
            << " with Ident: " << c_filter.ident()
            << INTERNAL_DEBUG_DEVICE_ENDL;
        }
        b_detectedOverflow = true;
        #endif
        b_forceProcessAll = true;
        break;
    }
    if ( b_toProcess )
    {
      if( arrFilterBox[i32_ident-minReceiveObjNr()].processMsg() )
        ui8_processedMsgCnt++;
    }
    HAL::can_useMsgobjPopFront(ui8_busNumber, i32_ident );
    if (( ElementBase_c::getAvailableExecTime() == 0 ) && (!b_forceProcessAll))
    { // switch the flag back, so that further processings are enabled
      b_runningCanProcess = false;
      return -1;
    }
    System_c::triggerWd();
  }
  #endif

  // switch the flag back, so that further processings are enabled
  b_runningCanProcess = false;
  // return the number of received telegrams
  return ui8_processedMsgCnt;
}

/** function for sending data out of CANPkgExt_c (uses BIOS function)
  if send puffer is full a local loop waits till puffer has enough space
  (every 100ms the watchdog is triggered, to avoid watchdog reset)

  possible errors:
      * Err_c::hwConfig on wrong configured CAN obj, not init BUS or no configured send obj
      * Err_c::range on undef BUS or BIOS send obj nr
      * Err_c::can_warn on physical CAN-BUS problems
      * Err_c::can_off on physical CAN-BUS off state
  @param rrefc_src CANPkgExt_c which holds the to be sent data
  @return reference to this CANIOExt_c instance ==> needed by commands like "c_can_io << pkg_1 << pkg_2 ... << pkg_n;"
*/
CANIO_c& CANIO_c::operator<<(CANPkgExt_c& refc_src)
{
  if ( ! isReady2Send() ) return *this;
  //check if source and destination address are valid
#if ((defined(USE_ISO_11783)) && ((CAN_INSTANCE_CNT > PRT_INSTANCE_CNT) || defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL)))
  // when ISO is compiled, we must make sure, that the ISO specific
  // resolving is only used for extended ident messages
  #ifdef ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL
  if ( ( b_canChannelCouldSendIso                           )
    && ( refc_src.identType() == Ident_c::ExtendedIdent     )
    && ( ! refc_src.isProprietaryMessageOnStandardizedCan() ) )
  #else
  if ( ( b_canChannelCouldSendIso ) && ( refc_src.identType() == Ident_c::ExtendedIdent ) )
  #endif // end of ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL
  #endif
  {
    if ( ! refc_src.resolveSendingInformation() )
    { // preconditions for correct sending are not fullfilled -> set error state
      getILibErrInstance().registerError(IsoAgLib::iLibErr_c::CanBus, IsoAgLib::iLibErr_c::Can);
      return *this;
    }
  }
  return CANIO_c::operator<<( static_cast<CANPkg_c&>(refc_src) );
}

/** function for sending data out of CANPkg_c (uses BIOS function)
  if send puffer is full a local loop waits till puffer has enough space
  (every 100ms the watchdog is triggered, to avoid watchdog reset)

  possible errors:
      * Err_c::hwConfig on wrong configured CAN obj, not init BUS or no configured send obj
      * Err_c::range on undef BUS or BIOS send obj nr
      * Err_c::can_warn on physical CAN-BUS problems
      * Err_c::can_off on physical CAN-BUS off state
  @param rrefc_src CANPkg_c which holds the to be sent data
  @return reference to this CANIO_c instance ==> needed by commands like "c_can_io << pkg_1 << pkg_2 ... << pkg_n;"
*/
CANIO_c& CANIO_c::operator<<(CANPkg_c& refc_src)
{ // immediately return if CAN is not yet configured for send
  if ( ! isReady2Send() ) return *this;

  // set send MsgObj ID
  uint8_t ui8_sendObjNr = minHALMsgObjNr();

  #if 0
  if ( ui8_busNumber == 1 ) {
    INTERNAL_DEBUG_DEVICE
      << "CANIO_c::operator<< mit MIN MsgObj Nr: " << uint16_t( ui8_sendObjNr )
      << "Xtd: " << refc_src.identType()
      << "\n\r";
  }
  #endif

  // if both standard and extended ident can be sent -> use second send obj if data uses extended ident
  if (en_identType == Ident_c::BothIdent)
  {
    if (refc_src.identType() == Ident_c::ExtendedIdent)
    {
      ui8_sendObjNr += 1;
    }
  }

  // wait till Msg can be placed in send buffer
  /** @todo check for better handling of full buffer - don't just loop 10 times */
  for ( uint16_t ind = 0;
        ( ( HAL::can_stateMsgobjFreecnt( ui8_busNumber, ui8_sendObjNr ) < 1 ) && ( ind < 10 ) ); ind++ )
  {  // perform wait loop
    int16_t i16_result = 0;
    for (int16_t i = 0; i < 200; i++) i16_result++;
    // it's time to trigger the watchdog
    HAL::wdTriggern();
    // exit loop, if CAN BUS is blocked and exit function
    if (HAL::can_stateGlobalBlocked(ui8_busNumber))
    {  // clear MsgObj CAN queue
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE
       << "CANIO_c::operator<< Blocked BUS Nr: " << uint16_t(ui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif

      HAL::can_useMsgobjClear(ui8_busNumber,ui8_sendObjNr);
      return *this;
    }
  }
  // it's time to trigger the watchdog
  HAL::wdTriggern();

  #ifdef DEBUG
  doDebug(ui8_busNumber, ui8_sendObjNr);
  #endif

  int16_t i16_sendFuncState = HAL::can_useMsgobjSend(ui8_busNumber, ui8_sendObjNr, &refc_src);

  #ifdef DEBUG
  doDebug(ui8_busNumber, ui8_sendObjNr);
  #endif

  // it's time to trigger the watchdog
  HAL::wdTriggern();
  // BIOS CAN send function with lowest configured msg-obj, which was
  // configured for send during init
  switch (i16_sendFuncState)
  {
    case HAL_NO_ERR:
      // everything o.k.
      break;
    case HAL_CONFIG_ERR:
      // no send obj or BUS not initialized -> should not happen, cause this is done
      // on init of this object
      // (and is done if timeEvent notices that CAN wasn't configured by init)
      #if defined(DEBUG)
        INTERNAL_DEBUG_DEVICE << "\r\nBUS " << uint16_t(ui8_busNumber) << " not initialized or MsgObj: " << uint16_t(ui8_sendObjNr) << " no send obj" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::HwConfig, iLibErr_c::Can );
      break;
    case HAL_NOACT_ERR:
      // BUS off
      #if defined(DEBUG)
      INTERNAL_DEBUG_DEVICE << "BUS " << uint16_t(ui8_busNumber) << " in BUS OFF STATE" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::CanOff, iLibErr_c::Can );
      break;
    case HAL_OVERFLOW_ERR:
      // overflow of send puffer
      getILibErrInstance().registerError( iLibErr_c::CanOverflow, iLibErr_c::Can );
      break;
    case HAL_RANGE_ERR:
      // BUS nr or obj nr outer allowed limits (shouldn't be the case after successful init call)
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
      break;
    case HAL_WARN_ERR:
      // signal for BUS-WARN problem
      #if defined(DEBUG)
      INTERNAL_DEBUG_DEVICE << "BUS " << uint16_t(ui8_busNumber) << " in WARN STATE" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::CanWarn, iLibErr_c::Can );
      break;
    case HAL_NEW_SEND_DELAY:
      #if defined(DEBUG)
      INTERNAL_DEBUG_DEVICE << "BUS " << uint16_t(ui8_busNumber) << " encountered new Maximum Send Delay.. \n";
      #endif
      if (i32_maxSendDelay >= 0)
      { // we're in mode: Test-MaxSendDelay
        if (HAL::can_getMaxSendDelay(ui8_busNumber) > i32_maxSendDelay)
        { // this is way too max, register an error!
          getILibErrInstance().registerError( iLibErr_c::CanTooSlowSend, iLibErr_c::Can );
      #ifdef DEBUG
          INTERNAL_DEBUG_DEVICE << ".. which has now registered an ERROR as it was higher (it was "<< HAL::can_getMaxSendDelay(ui8_busNumber) <<") than our trigger!" << INTERNAL_DEBUG_DEVICE_ENDL;
        }
        else INTERNAL_DEBUG_DEVICE << ".. but this " << HAL::can_getMaxSendDelay(ui8_busNumber) << " was not yet higher than our trigger!" << INTERNAL_DEBUG_DEVICE_ENDL;
      }
      else INTERNAL_DEBUG_DEVICE << ".. but ignored, as app didn't define a maxDelay-Trigger! " << INTERNAL_DEBUG_DEVICE_ENDL;
      #else
        }
      }
      #endif
      break;
  } //switch

  // return pointer to self for sequence of send operations
  return *this;
}

/**********************************************/
/** definition of protected element functions */
/**********************************************/
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
/** evaluate common bits of all defined filterBox
    instances and set it in mask -> build up global mask */
void CANIO_c::getCommonFilterMask()
{
  // preset all bits with "1"
  c_maskStd.set(~0, Ident_c::StandardIdent);
  c_maskExt.set(~0, Ident_c::ExtendedIdent);

  // combine masks of all arrFilterBox with AND
  for (ArrFilterBox::iterator pc_iter = arrFilterBox.begin();
       pc_iter != arrFilterBox.end();
       pc_iter++
      )
      {
        if (pc_iter->identType() == Ident_c::StandardIdent) {
          c_maskStd.ident_bitAnd(pc_iter->mask());
          c_maskStd.ident_bitAnd(pc_iter->additionalMask());
        }
        else {
          c_maskExt.ident_bitAnd(pc_iter->mask());
          c_maskExt.ident_bitAnd(pc_iter->additionalMask());
        }
      }
}

/** correct global masks after merge based on merged IDs of all
    resulting MsgObj_c instances
 */
void CANIO_c::getCommonFilterMaskAfterMerge()
{

  // combine masks of all arrFilterBox with AND
  for (ArrMsgObj::iterator pc_iter = arrMsgObj.begin();
       pc_iter != arrMsgObj.end();
       pc_iter++
      )
  {
    if (pc_iter->filter().identType() == Ident_c::StandardIdent)
      pc_iter->commonFilterAfterMerge( c_maskStd );
    else pc_iter->commonFilterAfterMerge( c_maskExt );
  }
}

/** build unique MsgObj_c's based on given
  global mask (different FilterBox_c def's can result in one
  MsgObj_c if differences are hidden by global mask), return count of obj

  possible errors:
      * Err_c::badAlloc on not enough memory for created MsgObj_c instances
  @see MsgObj
  @see FilterBox
*/
int16_t CANIO_c::FilterBox2MsgObj()
{
  int16_t i16_result = 0;
  Ident_c c_tempIdent;
  ArrMsgObj::iterator pc_iterMsgObj, pc_searchEnd = arrMsgObj.begin();

  // clear the settings in all previously created MsgObj_c instances
  for (pc_iterMsgObj = arrMsgObj.begin(); pc_iterMsgObj != arrMsgObj.end(); pc_iterMsgObj++)
  { // first process all messages in the buffer IF OPEN, and then close
    HAL::wdTriggern();
    if ( pc_iterMsgObj->isOpen() )
    { // process received messages
      pc_iterMsgObj->lock( true );
      pc_iterMsgObj->processMsg( ui8_busNumber, true );
    }
    pc_iterMsgObj->close();
    pc_iterMsgObj->lock( false );
  }

  HAL::wdTriggern();
  // gather globalMask dependent filter bits of each arrFilterBox
  // and create suitable arrMsgObj if not already created
  for (ArrFilterBox::iterator pc_iterFilterBox = arrFilterBox.begin();
       pc_iterFilterBox != arrFilterBox.end();
       pc_iterFilterBox++
      )
  { // get filter Ident_c setting from actual FilterBox
    c_tempIdent = pc_iterFilterBox->filter();
    // mask the actual filter Ident_c with the bits of the global mask
    if (c_tempIdent.identType() == Ident_c::StandardIdent)
      c_tempIdent.ident_bitAnd(c_maskStd);
    else c_tempIdent.ident_bitAnd(c_maskExt);

    for (pc_iterMsgObj = arrMsgObj.begin(); pc_iterMsgObj != pc_searchEnd; pc_iterMsgObj++)
    {
      if (pc_iterMsgObj->equalFilter(c_tempIdent)) break;
    }
    // if no item with this filter was found add new MsgObj
    if (pc_iterMsgObj == pc_searchEnd)
    {
      // no equal item found in arrMsgObj -> create new MsgObj_c in arrMsgObj
      if (pc_searchEnd != arrMsgObj.end())
      {
        // the arrMsgObj for this filter setting was
        // already previous created, can now be reused

        // set filter in arrMsgObj
        // new last element to be configured after actual last element
        pc_searchEnd->setFilter(c_tempIdent);
        pc_searchEnd->insertFilterBox(pc_iterFilterBox);
        pc_searchEnd++;
      }
      else
      {
        // previous less arrMsgObj was created -> create new arrMsgObj
        // clear temp obj (it is reused for each function call)
        c_tempObj.close();
        // set filter in new pc_lt_element
        c_tempObj.setFilter(c_tempIdent);
        c_tempObj.insertFilterBox(pc_iterFilterBox);
        // check if insertion try result in growed dyn array
        const uint16_t ui16_oldSize = arrMsgObj.size();
        // insert obj in vector
        arrMsgObj.push_front(c_tempObj);
        if (ui16_oldSize >= arrMsgObj.size())
        { // dyn array didn't grow -> set badAlloc error state
          getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Can );
        }
        #ifdef DEBUG_HEAP_USEAGE
        else
        {
          sui16_msgObjTotal++;
        }
        #endif
        pc_searchEnd = arrMsgObj.end();
      }
    }
    else
    {
      // same filter definition at position q already inserted in msgOb vector
      // insert new FilterBox_c reference
      if ( pc_iterMsgObj->insertFilterBox(pc_iterFilterBox) == false)
        i16_result = -1;
    }
  }
  // if less objects are created than already in vector, erase the unused
  // -> behind the last instance is NOT the list end
  if (pc_searchEnd!= arrMsgObj.end())
  {
    #ifdef DEBUG_HEAP_USEAGE
    const uint16_t cui16_oldSize = arrMsgObj.size();
    #endif
    HAL::wdTriggern();
    arrMsgObj.erase(pc_searchEnd, arrMsgObj.end());
    #ifdef DEBUG_HEAP_USEAGE
    sui16_msgObjTotal -= ( cui16_oldSize - arrMsgObj.size() );
    sui16_deconstructMsgObjCnt += (cui16_oldSize - arrMsgObj.size() );
    #endif
  }

  if ( i16_result >= 0) i16_result = arrMsgObj.size();
  #ifdef DEBUG_HEAP_USEAGE
  INTERNAL_DEBUG_DEVICE
    << "MsgObj: " << sui16_msgObjTotal << " -> ";
  #endif
  HAL::wdTriggern();

  return i16_result;

}

/** checks if count of preconfigured msgObj
  is allowed; shrink by merging the objects
  with smallest Bit distance
  -> all existing MsgObj_c instances can be
     configured in hardware after execution
  @see MsgObj
*/
void CANIO_c::CheckSetCntMsgObj()
{
  // set i16_minDistance to the max possible bit difference
  // => amount of bits in data type for ident
  int16_t i16_minDistance = sizeof(MASK_TYPE)*8,
      i16_tempDist;
  // maxHALMsgObjNr() delivers also the special lastMsgObj, which cannot be used for normal receiveMsgObj
  // ==> subtract from (max - min + 1 ) again one item
  uint8_t ui8_allowedSize = maxHALMsgObjNr() - minReceiveObjNr() + 1 - 1;

  unsigned int ui_lsbDiffMin = 0;
  unsigned int ui_lsbDiffTemp = 0;

  // before any preparation of further merge work - check whether we have to merge anyway
  if (arrMsgObj.size() <= ui8_allowedSize) return;

  // NOW WE HAVE TO MERGE AT LEAST TWO MsgObj_c together

  ArrMsgObj::iterator pc_minLeft = arrMsgObj.begin(),
          pc_minRight = arrMsgObj.begin();

  Ident_c* pc_useGlobalMask = NULL;

  // [min;max] allowed, but first one or two reserved for send
  bool b_continueMerging = true;
  while (b_continueMerging)
  { // more objects than allowed are in arrMsgObj, because intervall between
    // min+1 and max is smaller than size -> shrink array
    if (arrMsgObj.size() <= ui8_allowedSize) b_continueMerging = false;

    // start a comparison(left_ind, right_ind) loop for all elements
    for (ArrMsgObj::iterator pc_leftInd = arrMsgObj.begin(); pc_leftInd != arrMsgObj.end(); pc_leftInd++)
    {
      ArrMsgObj::iterator pc_rightInd = pc_leftInd;
      for (pc_rightInd++; pc_rightInd != arrMsgObj.end(); pc_rightInd++)
      {
        // retreive bit distance between instances left_ind and right_ind -> use Bit-XOR
        i16_tempDist = pc_leftInd->filter().bitDiff(pc_rightInd->filter(), ui_lsbDiffTemp);

        // a) store new min only if capacity of left is enough for objects of right
        // b) if bitDiff equal => prefere filters with differ in the most significant bits
        // c) prefere diffs in the two upper bytes even when bittDiff is not optimal
        if (((i16_tempDist < i16_minDistance) ||
             ((i16_tempDist == i16_minDistance) && (ui_lsbDiffTemp > ui_lsbDiffMin)) ||
             ((i16_tempDist-1 == i16_minDistance) && (ui_lsbDiffTemp > ui_lsbDiffMin) && (ui_lsbDiffMin<16) && (ui_lsbDiffTemp>=16)))
          && (pc_leftInd->filter().identType() == pc_rightInd->filter().identType())
           )
        {
          // new max similar filter found
          pc_minLeft = pc_leftInd;
          pc_minRight = pc_rightInd;
          i16_minDistance = i16_tempDist;
          ui_lsbDiffMin = ui_lsbDiffTemp;
          if ( i16_tempDist == 0 )
          {
            b_continueMerging = true; // merge the compared MsgObj_c IN ANY CASE, as they are equal
            break; // stop searching for "smaller" difference if 0 is found
          }
        }
      }
    }
    if ( b_continueMerging )
    { // we have to merge the found min bit difference because:
      // a) amount of MsgObj_c still too large
      // b) a pair of MsgObj_c with EQUAL (BitDiff of zero) has been found

      // now min_dist is minimal bit distance of most similar filters at i16_minLeft and
      // i16_minRight -> merge them
      // merge  right filter into the left
      pc_useGlobalMask = &c_maskExt;
      c_maskExt.set( c_maskExt.ident() & (~( pc_minLeft->filter().ident() ^ pc_minRight->filter().ident() ) ), Ident_c::ExtendedIdent );

      HAL::wdTriggern();

      pc_minLeft->merge(*pc_minRight);
      arrMsgObj.erase(pc_minRight);
      #ifdef DEBUG_HEAP_USEAGE
      sui16_msgObjTotal--;
      sui16_deconstructMsgObjCnt++;
      #endif
      // update the filters in the filters in the existing MsgObj_c to the changed mask
      for (ArrMsgObj::iterator c_iter = arrMsgObj.begin(); c_iter != arrMsgObj.end(); c_iter++)
      {
        c_iter->updateFilterWithMask( *pc_useGlobalMask );
      }


      // reset search arguments for posible next search
      pc_minRight = pc_minLeft = arrMsgObj.begin();
      i16_minDistance = sizeof(MASK_TYPE)*8;
    }
  }

  // now the amount of arrMsgObj is allowed
  #ifdef DEBUG_HEAP_USEAGE
  INTERNAL_DEBUG_DEVICE
    << sui16_msgObjTotal << " x MsgObj_c: Mal-Alloc: "
    <<  sizeSlistTWithMalloc( sizeof(MsgObj_c), sui16_msgObjTotal )
    << "/" << sizeSlistTWithMalloc( sizeof(MsgObj_c), 1 )
    << ", Chunk-Alloc: "
    << sizeSlistTWithChunk( sizeof(MsgObj_c), sui16_msgObjTotal )
  #ifdef MASSERT
    << "\r\n__mall tot:" << AllocateHeapMalloc
    << ", _mall deal tot: " << DeallocateHeapMalloc
    << INTERNAL_DEBUG_DEVICE_ENDL;
  #else
    << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  #endif

   HAL::wdTriggern();

}

/* *************************************** */
/* definition of private element functions */
/* *************************************** */

/** call the needed HAL function for setting the new global masks,
  without invalidating already open send and last msg obj.
  */
bool CANIO_c::registerChangedGlobalMasks(void)
{
  int16_t i16_retvalInit = HAL::can_configGlobalMask(ui8_busNumber, c_maskStd.ident(), c_maskExt.ident(),
                                                     c_maskLastmsg.ident());

  // check for error state
  if (i16_retvalInit == HAL_RANGE_ERR)
  { // BIOS complains about limits of BUSnr or msgObj
    // seldom, because before checked with defined LIMITS
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }
  else
  {
    return true;
  }
}

/** helper function to search all FilterBoxes for matching
  instance which maps to received CAN messages
  ( needed if the coordinated FilterBoxes don't match,
    especially important to process messages from global
    CAN MsgObj which doesn't have explicit connected FilterBox
    instances )
  @param rui32_ident Ident of received CAN message
  @return is another filterbox found?
*/
bool CANIO_c::canMsg2FilterBox( uint32_t rui32_ident, Ident_c::identType_t rt_type, ArrFilterBox::iterator& rref_arrFilterBoxIter, bool rb_start )
{
  if (rb_start)
  { // init
    rref_arrFilterBoxIter = arrFilterBox.begin();
  }
  else
  {
    rref_arrFilterBoxIter++;
  }

  while (rref_arrFilterBoxIter != arrFilterBox.end())
  {
    if ( rref_arrFilterBoxIter->matchMsgId( rui32_ident, rt_type ) )
    { // matching FilterBox_c found
      return true;
    }
    rref_arrFilterBoxIter++;
  }
  // if execution reaches this point, no matching FilterBox_c found
  return false;
}
#endif //end not defined SYSTEM_WITH_ENHANCED_CAN_HAL

/** search for a FilterBox with given mask and filter
    @param  rt_mask    filterBox mask
    @param  rt_filter  filterBox filter
    @return            located FilterBox or NULL if not exist
  */
FilterBox_c* CANIO_c::getFilterBox(Ident_c& rt_mask, Ident_c& rt_filter) const
{
  #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  for(uint8_t i = 0; i < arrFilterBox.size(); i++)
  {
    if( arrFilterBox[i].equalFilterMask(rt_mask, rt_filter) )
      return const_cast<FilterBox_c*>(&arrFilterBox[i]);
  }
  #else
  ArrFilterBox::const_iterator c_iter = arrFilterBox.begin();
  for(; c_iter != arrFilterBox.end(); c_iter++)
  {
    if( c_iter->equalFilterMask(rt_mask, rt_filter) )
      return const_cast<FilterBox_c*>(&*c_iter);
  }
  #endif
  //filterbox with mask, filter not found
  return NULL;
}

/** verify given BUS number and MsgObj number, if they are correct
  (mostly used by MsgObj_c to verify itself)
  @param rui8_busNr number of the BUS
  @param rui8_msgobjNr number of the MsgObj
  @return true -> values are correct
*/
bool CANIO_c::verifyBusMsgobjNr(uint8_t rui8_busNr, uint8_t rui8_objNr)
{ //check if there is an error
  if (
     (rui8_busNr > HAL_CAN_MAX_BUS_NR)
  || (rui8_objNr < HAL_CAN_MIN_REC_OBJ)
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  || (rui8_objNr > HAL_CAN_MAX_REC_OBJ)
  #endif
     )
  {
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }
  return true;
}

/** reconfigure the MsgObj after insert/delete of FilterBox */
bool CANIO_c::reconfigureMsgObj()
{ // verify that this instance is yet initialised complete (CAN BUS is dedicated)
  #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  // no msg objects exist only filterboxes and no register restrictions therefore no reconfigure must be done
  return true;
  #else
  if ( ui8_busNumber > HAL_CAN_MAX_BUS_NR )
  {
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }

  #ifdef DEBUG_HEAP_USEAGE
  INTERNAL_DEBUG_DEVICE
    << sui16_filterBoxTotal << " x FilterBox_c: Mal-Alloc: "
    <<  sizeSlistTWithMalloc( sizeof(FilterBox_c), sui16_filterBoxTotal )
    << "/" << sizeSlistTWithMalloc( sizeof(FilterBox_c), 1 )
    << ", Chunk-Alloc: "
    << sizeSlistTWithChunk( sizeof(FilterBox_c), sui16_filterBoxTotal )
  #ifdef MASSERT
    << "\r\n__mall tot:" << AllocateHeapMalloc
    << ", _mall deal tot: " << DeallocateHeapMalloc
    << INTERNAL_DEBUG_DEVICE_ENDL;
  #else
    << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  #endif

  bool b_result = true;
  // store old mask to check if global CAN BIOS mask must be changed
  Ident_c oldMaskStd = c_maskStd,
        oldMaskExt = c_maskExt;

  // build suitable global t_mask
  getCommonFilterMask();

  if ( c_lastMsgObj.isOpen() )
  { // process all messages which where placed during the reconfig in the last message object
    c_lastMsgObj.processMsg( ui8_busNumber, true );
    // unlock it in any case
    c_lastMsgObj.lock( false );
  }
  else
  { // not yet open -> open it now
    Ident_c tmp ( 0, Ident_c::ExtendedIdent );
    // configure the last MsgObj_c
    c_lastMsgObj.setFilter( tmp );
    c_lastMsgObj.configCan(ui8_busNumber, HAL_CAN_LAST_MSG_OBJ_NR );
  }

  // create according to new global t_mask all MsgObj_c with
  // unique filter settings -> merge all filter settings where
  // the the global t_mask delets all different bits
  if ( FilterBox2MsgObj() == false) b_result = false;

  // shrink number of MsgObj to fit the intervall of
  // [min_msgObjNr; maxMsgObjNr]

  CheckSetCntMsgObj();

  // check and correct global masks after merge in CheckSetCntMsgObj()
  getCommonFilterMaskAfterMerge();

  if ((oldMaskStd != c_maskStd) || (oldMaskExt != c_maskExt))
  {
    // global t_mask setting has changed -> reinit CAN with new global masks
    registerChangedGlobalMasks();
  }

  int16_t i = 0;
  // config the hardware -> config all MsgObj_c in CAN hardware
  for (ArrMsgObj::iterator pc_iterMsgObj = arrMsgObj.begin();
       pc_iterMsgObj != arrMsgObj.end();
       pc_iterMsgObj++
      )
  { // call the configCan function for each MsgObj
    pc_iterMsgObj->configCan(ui8_busNumber, i+minReceiveObjNr());
    i++;
  }

  // lock the MsgObj_c to avoid receive of further messages
  c_lastMsgObj.lock( true );

  // clear any CAN BUFFER OVERFLOW error that might occure
  // for last message object
  getILibErrInstance().clear( iLibErr_c::CanOverflow, iLibErr_c::Can );

  HAL::wdTriggern();

  return b_result;
  #endif //SYSTEM_WITH_ENHANCED_CAN_HAL
}

/** get offset for received messages
    @return offset for received messages
  */
uint8_t CANIO_c::updateMinReceiveObjNr()
{
  ui8_minReceiveObjNr = 1 + minHALMsgObjNr();
  //increase receive offset to 2 if both 11bit and 29bit ident CAN are used
  if (en_identType == Ident_c::BothIdent)
    ui8_minReceiveObjNr++;
  #ifdef USE_CAN_EEPROM_EDITOR
  // the MsgObj next to the sending MsgObj are used for the EEPROM Editor
  // --> increase the offset for the first receive MsgObj by furhter two items
  ui8_minReceiveObjNr += 2;
  #endif
  return ui8_minReceiveObjNr;
}

/** switch CAN bitrate (possible during runtime
  with automatic reconfiguring of CAN MsgObj)
  @param rui16_newSpeed
*/
bool CANIO_c::setBitrate(uint16_t rui16_newSpeed, bool rb_force)
{ // don't reconfigure if same bitrate
  if ((rui16_newSpeed == ui16_bitrate)&&(rb_force == false)) return true;

  bool b_success = true;

  // init CAN with new speed (it checks if bitrate is allowed and closes can first
  if (baseCanInit(rui16_newSpeed) == false) return false;

  // config the MsgObj with same filter but with new speed
  int16_t i = 0;

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  // config the hardware -> config all MsgObj_c in CAN hardware
  for (ArrMsgObj::iterator pc_iterMsgObj = arrMsgObj.begin();
       pc_iterMsgObj != arrMsgObj.end();
       pc_iterMsgObj++
      )
  { // call the configCan function for each MsgObj
    pc_iterMsgObj->configCan(ui8_busNumber, i+minReceiveObjNr() );
    i++;
  }
  #else
  // config the hardware -> config all FilterBox_c in CAN hardware
  for (ArrFilterBox::iterator pc_iterFilterBox = arrFilterBox.begin();
       pc_iterFilterBox != arrFilterBox.end();
       pc_iterFilterBox++
      )
  { // call the configCan function for each FilterBox
    pc_iterFilterBox->configCan(ui8_busNumber, i+minReceiveObjNr() );
    i++;
  }
  #endif
  return b_success;
}


/** private constructor which prevents direct instantiation in user application
  * NEVER define instance of CANIO_c within application
  * (set ui8_busNumber to 0xFF so that init() detects first call after constructor)
  */
CANIO_c::CANIO_c( void ) : arrFilterBox() {}


/** perform bas init for CAN with set of speed and init of send object(s)
  @param rui16_bitrate wanted CAN bitrate
*/
bool CANIO_c::baseCanInit(uint16_t rui16_bitrate)
{
  HAL::wdTriggern();
  // check if rui16_bitrate is one of the allowed settings
  int16_t pi16_allowed[] = HAL_CAN_BITRATE_LIST;
  bool b_allowed = false;

  for (uint8_t ui8_index = 0; ui8_index < HAL_CAN_BITRATE_CNT; ui8_index++)
  {
    if (pi16_allowed[ui8_index] == rui16_bitrate)
    { // given bitrate is in allowed list at position ui8_index
      b_allowed = true; // store success state
      break; // exit search loop
    }
  }
  // if given bitrate fits to no allowed value set a range error state
  if (!b_allowed)
  {
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false; // exit function with error value
  }

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  bool b_lastMsgObjWasOpen = false;
  if ( c_lastMsgObj.isOpen() )
  { // the last MsgObj_c is open -> so it may contain some unprocessed messages
    // ==>> process all messages which where placed during the reconfig in the last message object
    c_lastMsgObj.processMsg( ui8_busNumber, true );
    c_lastMsgObj.close();
    b_lastMsgObjWasOpen = true;
  }
  #else
  for (uint8_t i = 0; i < arrFilterBox.size(); i++)
    arrFilterBox[i].closeHAL();
  #endif

  // store wanted bitrate for CAN communication
  ui16_bitrate = rui16_bitrate;

  // update the receive Filter offset
  updateMinReceiveObjNr();

  // to prevent double re-init of can close it first
  HAL::can_configGlobalClose(ui8_busNumber);
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE
      << "CANIO_c::baseCanInit( " << rui16_bitrate << " ) vor HAL::can_configGlobalInit" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // init CAN BUS (BIOS function)
  c_maskLastmsg.set(0, DEFAULT_IDENT_TYPE);

  int16_t i16_retvalInit = HAL::can_configGlobalInit(ui8_busNumber, ui16_bitrate, c_maskStd.ident(), c_maskExt.ident(),
                      c_maskLastmsg.ident());

  #ifdef USE_CAN_EEPROM_EDITOR
    /* if CAN init with success init CAN EEEditor */
    if ( ( i16_retvalInit == HAL_NO_ERR) && ( ui8_busNumber == CONFIG_EEPROM_USE_CAN_BUS ) )
    {
      uint8_t b_eepromType;
      if ((en_identType == Ident_c::BothIdent) || (en_identType == Ident_c::StandardIdent))
      {
        b_eepromType = Ident_c::StandardIdent;
      }
      else
      {
        b_eepromType = Ident_c::ExtendedIdent;
      }
      HAL::InitEEEditor( CONFIG_EEPROM_USE_CAN_BUS, minReceiveObjNr() - 2,
          (minReceiveObjNr() - 1), CONFIG_EEPROM_USE_CAN_REC_IDENT, b_eepromType,
          CONFIG_EEPROM_USE_CAN_BUFFER_SIZE, CONFIG_CAN_SEND_BUFFER_SIZE);
    }
  #endif

  // check for error state
  if (i16_retvalInit == HAL_RANGE_ERR)
  { // BIOS complains about limits of BUSnr or msgObj
    // seldom, because before checked with defined LIMITS
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( b_lastMsgObjWasOpen )
  { // open last message buffer during reconfig of other MSgObj as CAN is already activated and at least one MsgObj is already in use
    // --> avoid loss of CAN messages which would normally be stored in the already active MsgObj_c instances
    Ident_c tmp ( 0, Ident_c::ExtendedIdent );
    // configure the last MsgObj_c
    c_lastMsgObj.setFilter( tmp );
    c_lastMsgObj.configCan(ui8_busNumber, HAL_CAN_LAST_MSG_OBJ_NR );
  }
  #endif
  // init of BUS without error -> continue with send obj configuration

  // init BIOS send Msg Object (BIOS dependent)

  // return value which infomrs about success on BIOS configuration
  int16_t i16_retvalConfig = 0;
  // config the send CAN object with BIOS function
  bool b_configSuccess = true;
  Ident_c c_sendIdent;

  const uint8_t cui8_firstSendMsgObj = minHALMsgObjNr();
  const uint8_t cui8_potentialXtdSendMsgObj = minHALMsgObjNr() + 1;

  for (uint8_t ui8_msgObjNr = cui8_firstSendMsgObj; ui8_msgObjNr <= cui8_potentialXtdSendMsgObj; ui8_msgObjNr++)
  {
    switch (en_identType)
    {
      case Ident_c::StandardIdent:
        c_sendIdent.setIdentType(Ident_c::StandardIdent);
        break;
      case Ident_c::ExtendedIdent:
        c_sendIdent.setIdentType(Ident_c::ExtendedIdent);
        break;
      case Ident_c::BothIdent:
        if (ui8_msgObjNr == cui8_firstSendMsgObj) c_sendIdent.setIdentType(Ident_c::StandardIdent);
        else c_sendIdent.setIdentType(Ident_c::ExtendedIdent);
        break;
    }
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    i16_retvalConfig = HAL::can_configMsgobjInit(ui8_busNumber,ui8_msgObjNr, c_sendIdent, true);
    #else
    i16_retvalConfig = HAL::can_configMsgobjInit(ui8_busNumber,ui8_msgObjNr, c_sendIdent, c_sendIdent, true);
    #endif

    // check for error state
    switch (i16_retvalConfig)
    {
      case HAL_NO_ERR:
        break;
      case HAL_BUSY_ERR:
        // send obj already in use
        getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::Can );
        break;
      case HAL_CONFIG_ERR:
        // because CAN was initialized direct above without error, msgObj limits was
        // allowed and the SEND type is hardcoded to an allowed number,
        // HAL_CONFIG_ERR error can only be caused by allocating problem
        getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Can );
        b_configSuccess = false;
        break;
    }
    // only run second time if both ident types should be sent
    if (en_identType != Ident_c::BothIdent)
      break;
  } // end for


  #if ( ( CAN_INSTANCE_CNT > PRT_INSTANCE_CNT ) || defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL) )
    if ( getSingletonVecKey() >= PRT_INSTANCE_CNT ) b_canChannelCouldSendIso = false;
    else b_canChannelCouldSendIso = true;
  #endif

  // return true -> must be set according success
  return b_configSuccess;
}

/** check for can send conflict error and stop send retry on error
  (thus avoid BUS OFF)
  @return true -> there was send error and send retry stopped
*/
bool CANIO_c::stopSendRetryOnErr()
{
  bool b_result = false;

  bool b_bit1err = HAL::can_stateGlobalBit1err(ui8_busNumber);
  bool minObjNr = minHALMsgObjNr();

  bool b_sendproblem = HAL::can_stateMsgobjSendproblem(ui8_busNumber, minObjNr);

  if (b_bit1err && b_sendproblem)
  { // only send 1 has bit1 err -> clear msg obj
    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE
     << "CANIO_c::stopSendRetryOnErr() Blocked BUS Nr: " << uint16_t(ui8_busNumber)
     << ", ObjNr: " << minObjNr
     << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    HAL::can_useMsgobjClear(ui8_busNumber, minObjNr);
    b_result = true;
  }
  else if ( en_identType == Ident_c::BothIdent )
  {
    b_sendproblem = HAL::can_stateMsgobjSendproblem(ui8_busNumber, (minObjNr+1) );

    if (b_bit1err && b_sendproblem)
    { // only send 1 has bit1 err -> clear msg obj
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE
       << "CANIO_c::stopSendRetryOnErr() Blocked BUS Nr: " << uint16_t(ui8_busNumber)
       << ", ObjNr: " << (minObjNr+1) << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif

      HAL::can_useMsgobjClear(ui8_busNumber, (minObjNr+1) );
      b_result = true;
    }
  }
  return b_result;
}


/** set the new maximum send delay
    @param ri32_maxSendDelay new maximum send delay. negative values (like -1) for NOT using this feature!
 */
void CANIO_c::setMaxSendDelay (int32_t ri32_maxSendDelay)
{
  i32_maxSendDelay = ri32_maxSendDelay;
}


/** set this client to have send-priority
  @param rb_sendPrioritized enable (true) or disable (false) sending in Prioritized Mode
 */
void CANIO_c::setSendPriority(bool rb_sendPrioritized)
{ // set send MsgObj ID
  sb_sendPrioritized = rb_sendPrioritized;
}


#ifdef DEBUG
void CANIO_c::doDebug(uint8_t ui8_busNr, uint8_t ui8_sendObjNr)
{
  bool b_bit1err = HAL::can_stateGlobalBit1err(ui8_busNr);
  bool b_warnState = HAL::can_stateGlobalWarn(ui8_busNr);
  bool b_sendproblem = HAL::can_stateMsgobjSendproblem(ui8_busNr, ui8_sendObjNr);

  if ( b_bit1err )
  {
    INTERNAL_DEBUG_DEVICE << "BITERR VOR HAL::can_useMsgobjSend BUS: "
      << uint16_t(ui8_busNr) << ", sendObjNr: "
      << uint16_t(ui8_sendObjNr)
      << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  if ( b_sendproblem )
  {
    INTERNAL_DEBUG_DEVICE << "SENDPROBLEM VOR HAL::can_useMsgobjSend BUS: "
      << uint16_t(ui8_busNr) << ", sendObjNr: "
      << uint16_t(ui8_sendObjNr)
      << INTERNAL_DEBUG_DEVICE_ENDL;

  }
  if ( b_warnState )
  {
    INTERNAL_DEBUG_DEVICE << "CAN_WARN VOR HAL::can_useMsgobjSend BUS: "
      << uint16_t(ui8_busNr) << ", sendObjNr: "
      << uint16_t(ui8_sendObjNr)
      << INTERNAL_DEBUG_DEVICE_ENDL;
  }
}
#endif
} // end of namespace __IsoAgLib
