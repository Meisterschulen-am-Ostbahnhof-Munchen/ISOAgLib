/***************************************************************************
    6_0_ProprietaryCan.cpp -
                             -------------------
    begin                 Fri Apr 07 2000
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

#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_ProprietaryCan.h
#endif
#include "../IsoAgLib_Examples/tutorial/6_ProprietaryCan/proprietarycanclient_c.h"
#include "IsoAgLib/comm/SystemMgmt/ISO11783/iisomonitor_c.h"

// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace __IsoAgLib;

/** for a second client */
//#define PROP_CLIENT_B

// only when POOL:Call to init iIsoTerminal instance and initialize object pool! and start address claim
// IsoAgLib::getIisoTerminalInstance().initAndRegisterIsoObjectPool (c_myIdent, PropProt_c,"Prop1.0");

IsoAgLib::iISOName_c c_remoteECU (true, // rb_selfConf
                                  2,    // rui8_indGroup
                                  7,    // rui8_devClass
                                  0,    // rui8_devClassInst
                                  25,   // rb_func
                                  0x7FF,// rui16_manufCode
                                  27,   // rui32_serNo
                                  0,    // rb_funcInst
                                  #ifdef PROP_CLIENT_B
                                  0
                                  #else
                                  1
                                  #endif
                                  );   // rb_ecuInst Client A = 0 Client B = 1

  /** process a can message - receive data -> reimplemented fom proprietarymessagecliet */
  bool ProprietaryCanClient_c::processMsg()
  {
    uint16_t ui16_pos = 0;

    getDataReceive().getDataUi8(ui16_pos);

    IsoAgLib::getIrs232Instance() << "Received CAN message with ident: " << getDataReceive().getIdent();
    // << " and data string: " << getDataReceive().getDataStream(0);

    return true;

/*    switch ((c_data.isoPgn() & 0x1FF00))
    {
      case PROPRIETARY_A_PGN:
      break;
      default:
      break;
    }*/
  }

  /** send a ISO11783 information.
    * this is only called when an address is claimed
      @return true */
  void ProprietaryCanClient_c::mainEvent()
  {
//    IsoAgLib::iCANPkgExt_c c_data;
//    IsoAgLib::iCANIO_c &c_can = IsoAgLib::getIcanInstance();

    /* every 200 ms and by a request from the client */
    if ( lastTimeSinceUpdate()  >= 200 )
    {
      if (pc_identItem->isClaimedAddress())
      {
        CNAMESPACE::cout << "LOCAL: Claimed" << CNAMESPACE::endl;
        /* set data to send */

        if (IsoAgLib::getIisoMonitorInstance().existIsoMemberISOName(c_remoteECU))
        {
          CNAMESPACE::cout << "REMOTE: Claimed" << CNAMESPACE::endl;
        }
        /** data structure */
        IsoAgLib::GenericData_c& refs_sendDataCan = getDataSend();
        /** set data */
        refs_sendDataCan.setDataUi32(0,0x12345678);
//        refs_sendDataCan.setDataUi32(4,0xD00FAFFE);
//        refs_sendDataCan.setDataUi32(8,0xD00FAFFE);

        refs_sendDataCan.setIdent (PROPRIETARY_A_PGN << 8);
        /**  */
        sendDataToHandler();
      }

      /* updating the timer */
      updateTime( Scheduler_c::getLastTimeEventTrigger() );
    }
  }


  /** initialize proprietary Can client */
  void ProprietaryCanClient_c::init( const IsoAgLib::iIdentItem_c& rrefc_item, const IsoAgLib::iISOName_c& rrefc_rremoteECU )
  {
    pc_identItem = &rrefc_item;

    /** filter for proprietary A1 */
    uint32_t ui32_filter = 0x0EF0000;
    /** Mask for Proprietary A1 */
    uint32_t ui32_mask   = 0x0FF0000;

    /** define receive filter. Note: ident only for proprietary A, A1 PGN, otherwise NULL */
//    defineReceiveFilter( ui32_mask,  ui32_filter, rrefc_rremoteECU, &rrefc_item);
    defineReceiveFilter( ui32_mask,  ui32_filter, rrefc_rremoteECU, &rrefc_item);
  }


int main()
{
  // simply call startImi
  IsoAgLib::getIcanInstance().init( 0, 250 );

  // start address claim of the local identity/member
  IsoAgLib::iIdentItem_c c_myIdent (2,    // rui8_indGroup
                                    7,    // rui8_devClass
                                    0,    // rui8_devClassInst
                                    25,   // rb_func
                                    0x7FF,// rui16_manufCode
                                    27,   // rui32_serNo
                                    254,  // No preferred SA, auto-allocate in range 0x80 upwards...
                                    0x100,// EEPROM-Address to store the allocated SA for re-use after power-cycle
                                    0,    // rb_funcInst
                                    #ifdef PROP_CLIENT_B
                                    1,    // rb_ecuInst
                                    #else
                                    0,    // rb_ecuInst
                                    #endif
                                    true, // rb_selfConf
                                    0);   // 0 means: We're WS-Master and have 0 WS-Slaves
                                    // further parameters use the default: NULL /* so no list given either */, 0 /* singletonVecKey */

  /** Client */
  ProprietaryCanClient_c c_ProprietaryApp;
  /** initialize client */
  c_ProprietaryApp.init (c_myIdent, c_remoteECU);
  /** run main loop */
  while ( IsoAgLib::iSystem_c::canEn() )
  {
    /** call main timeEvent function */
    IsoAgLib::getISchedulerInstance().timeEvent();
    c_ProprietaryApp.mainEvent();
    IsoAgLib::iCANIO_c::waitUntilCanReceiveOrTimeout( 50 );
  }
  return 1;
};

