/***************************************************************************
                          5_1_ProprietaryPgn.cpp - Test application for using
                          proprietary-PGN-messages using IsoAgLib
                             -------------------
    begin                 2006
    copyright            : (C) 2006 - 2007 by Christoph Staub
    email                : c.staub@osb-ag:de
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
 * Copyright (C) 2000 - 2007 Dipl.-Inform. Achim Spangler                  *
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
#include "5_1_ProprietaryPgn.h"
#include "IsoAgLib/comm/SystemMgmt/ISO11783/iisomonitor_c.h"


/** for a second client (look at c_remoteECU and c_myIdent)
    compile with the following define set: */
//#define PROP_CLIENT_B


// the interface objects of the IsoAgLib are placed in the IsoAgLib namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;

iISOName_c c_remoteECU (true, // rb_selfConf
                        2,    // rui8_indGroup
                        7,    // rui8_devClass
                        0,    // rui8_devClassInst
                        25,   // rb_func
                        0x7FF,// rui16_manufCode
                        27,   // rui32_serNo
                        0,    // rb_funcInst
                        #ifdef PROP_CLIENT_B
                        0     // rb_ecuInst second client "1"
                        #else
                        1     // rb_ecuInst second client "0"
                        #endif
                        );

  /** process a can message - receive data -> reimplemented fom proprietarymessageclient */
  void ProprietaryCanClient_c::processMsg()
  {
    /** data evaluation part */
    /** takes the ident from the stored data */
    uint32_t rc_ident = getDataReceive().getIdent();

    /** switch to handle Proprietary PGNs */
    switch ( (rc_ident & 0x3FF0000) >> 8 )
    {
      /** in this case the PGN is 0x0EF00 (Proprietary_A_PGN) */
      case PROPRIETARY_A_PGN:
      {
        /** next part is for user specific evaluation */

        #ifdef SYSTEM_PC
        CNAMESPACE::cout << "received A PGN" << CNAMESPACE::endl;
        CNAMESPACE::cout << "received ident from message: " << STL_NAMESPACE::hex << getDataReceive().getIdent() << CNAMESPACE::endl;
        CNAMESPACE::cout << "(when received thru (E)TP, there's no PRIO in the Ident!)" << CNAMESPACE::endl;
        CNAMESPACE::cout << "my address is: 0x" << STL_NAMESPACE::hex << ( (getDataReceive().getIdent() & 0x00FF00) >> 8 ) << CNAMESPACE::endl;
        CNAMESPACE::cout << "sender address is: 0x" << STL_NAMESPACE::hex << ( (getDataReceive().getIdent() & 0x0000FF) ) << CNAMESPACE::endl;
        CNAMESPACE::cout << "received bytes: ";
        const uint32_t ui32_datacnt = getDataReceive().getLen();
        for (uint32_t ui32_loopcnt = 0; ui32_loopcnt < ui32_datacnt; ui32_loopcnt++)
        {
          CNAMESPACE::cout << STL_NAMESPACE::hex << ( int )( getDataReceive().getDataUi8( ui32_loopcnt ) )<< " ";
        }
        CNAMESPACE::cout << CNAMESPACE::endl;
        #endif
        break;
      }
    }
  }

  /** this is only called when an address is claimed
   */
  void ProprietaryCanClient_c::mainEvent()
  {
    #ifdef PROP_CLIENT_B
    /** CLIENT B sends on its own every 1000 ms */
    if ( lastTimeSinceUpdate() >= 1000 )
    {
      if (pc_identItem->isClaimedAddress())
      {
        /** data structure for sending */
        IsoAgLib::iGenericData_c& refs_sendDataCan = getDataSend();

        /// You have two possibilities here:
        /// A) Clear the sendData, fill it with the data and then send it! (use this if you send different messages all the time)
        /// B) Have the sendData filled at some init-point and only change some contents here! (use this if you send similar messages only)

        // we're using a mixture of A) and B) right now below which is NOT optimal, but it works for demonstrating!

        /** data vector is cleared */
        refs_sendDataCan.clearVector();
        refs_sendDataCan.setDataUi32(0,0x11122233);
        refs_sendDataCan.setDataUi32(4,0x34445556);
        refs_sendDataCan.setDataUi32(8,0x66777888); // will need multi-packet

        /** test for proprietary A PGN */
        refs_sendDataCan.setIdent (PROPRIETARY_A_PGN << 8);

        /** sends the data to the proprietary message handler */
        sendDataToHandler();
      }
      updateTime( iSystem_c::getTime() );
    }
    #else
    /** CLIENT A does nothing but change the send-data (sending is done automatically periodically) */
    if ( lastTimeSinceUpdate() >= 100 )
    {
      /** data structure for sending */
      IsoAgLib::iGenericData_c& refs_sendDataCan = getDataSend();

      /** Modify the send-data */
      uint32_t ui32_data = refs_sendDataCan.getDataUi32(0);
      ui32_data++;
      refs_sendDataCan.setDataUi32(0,ui32_data);

      updateTime( iSystem_c::getTime() );
    }
    #endif
  }


  /** initialize proprietary Can client Application */
  void ProprietaryCanClient_c::init( const IsoAgLib::iIdentItem_c& rrefc_item, const IsoAgLib::iISOName_c& rrefc_rremoteECU )
  {
    /** pc_identItem is used in main event */
    pc_identItem = &rrefc_item;
    /** filter for proprietary A1 */
    uint32_t ui32_filter = PROPRIETARY_A_PGN << 8;
    /** Mask for Proprietary A1 */
    uint32_t ui32_mask   = 0x3FF0000;
    /** define receive filter. Note: ident only for proprietary A, A1 PGN, otherwise NULL */
    defineReceiveFilter( ui32_mask,  ui32_filter, rrefc_rremoteECU, &rrefc_item);

    #ifndef PROP_CLIENT_B
    /** CLIENT A uses periodically sending! */
    /** data structure for sending */
    IsoAgLib::iGenericData_c& refs_sendDataCan = getDataSend();
    /** Initialize Telegram */
    refs_sendDataCan.clearVector();
    refs_sendDataCan.setDataUi32(0,0x12345678); // will fit into single-packet
    /** set to proprietary A PGN */
    refs_sendDataCan.setIdent (PROPRIETARY_A_PGN << 8);
    /** set periodic sending to 500 msec */
    setSendPeriodMsec(500);
    /** sends the data to the proprietary message handler NOW and also starts periodic activity! */
    sendDataToHandler();
    #endif

  }


int main()
{
  // init CAN-Bus
  getIcanInstance().init( 0, 250 );

  // start address claim of the local identity/member
  iIdentItem_c c_myIdent (2,    // rui8_indGroup
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
                          true);// rb_selfConf
                          // further parameters use the default: -1 /* no workingset at all */, NULL /* so no list given either */, 0 /* singletonVecKey */

  /** create Application */
  ProprietaryCanClient_c c_ProprietaryApp;

  /** initialize Application */
  c_ProprietaryApp.init (c_myIdent, c_remoteECU);

  /** run main loop */
  int32_t i32_idleTimeSpread;
  while ( iSystem_c::canEn() )
  {
    /** call main timeEvent function */
    i32_idleTimeSpread = getISchedulerInstance().timeEvent();

    /** call main event. will take use of the scheduler as soon as iElementBase_c is finally there!
        for now it's kinda hacked into here! */
    c_ProprietaryApp.mainEvent();

    /** That's why we limit the i32_idleTimeSpread.
        We don't get exact timing with mainEvent() but we don't need it for demonstrating the tutorial */
    if (i32_idleTimeSpread > 50) i32_idleTimeSpread = 50;

    // no need to sleep on single-task systems
    #ifdef SYSTEM_PC
     #ifdef WIN32
     if ( i32_idleTimeSpread > 0 ) Sleep( i32_idleTimeSpread );
     #else
     if ( i32_idleTimeSpread > 0 ) iCANIO_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
     #endif
    #endif
  }
  return 1;
};

