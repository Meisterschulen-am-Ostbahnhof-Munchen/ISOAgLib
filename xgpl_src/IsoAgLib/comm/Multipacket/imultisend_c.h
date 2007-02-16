/***************************************************************************
                             imultisend_c.h - transfer of multi message
                                                 data
                             -------------------
    begin                : Fri Sep 28 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
#ifndef IMULTI_SEND_H
#define IMULTI_SEND_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/multisend_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  This class implements the various multi message data
  transfer specifications of Fieldstar, LBS+ and ISO 11783
  for send of more the 8 bytes of data.

  @author Dipl.-Inform. Achim Spangler
*/
class iMultiSend_c : private __IsoAgLib::MultiSend_c {
public:

  /**
    send an ISO target multipacket message with active retrieve of data-parts to send
    @param rb_send dynamic member no of sender
    @param rb_empf dynamic member no of receiver
    @param rpc_mss allow active build of data stream parts for upload by deriving data source class
                  from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                  retrieve of data to send. This is especially important for ISO_Terminal,
                  which assembles the data pool dependent on the terminal capabilities during upload
                  ( e.g. bitmap variants )
    @param ri32_pgn PGN to use for the upload
    @param rrefen_sendSuccessNotify -> pointer to send state var, where the current state
            is written by MultiSend_c
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoTarget (const iISOName_c& rrefc_isoNameSender, const iISOName_c& rrefc_isoNameReceiver, iMultiSendStreamer_c* rpc_mss, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
  { return MultiSend_c::sendIsoTarget (rrefc_isoNameSender, rrefc_isoNameReceiver, rpc_mss, ri32_pgn, rrefen_sendSuccessNotify);}

  /**
    send a ISO target multipacket message
    @param rb_send dynamic member no of sender
    @param rb_empf dynamic member no of receiver
    @param rhpb_data HUGE_MEM pointer to the data
    @param ri32_dataSize size of the complete mask
    @param ri32_pgn PGN to use for the upload
    @param rpen_sendSuccessNotify -> pointer to send state var, where the current state
            is written by MultiSend_c
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoTarget (const iISOName_c& rrefc_isoNameSender, const iISOName_c& rrefc_isoNameReceiver, HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
    { return MultiSend_c::sendIsoTarget (rrefc_isoNameSender, rrefc_isoNameReceiver, rhpb_data, ri32_dataSize, ri32_pgn, rrefen_sendSuccessNotify ); }

  /**
    send a ISO broadcast multipacket message
    @param rb_send dynamic member no of sender
    @param rb_empf dynamic member no of receiver
    @param hpb_data HUGE_MEM pointer to the data
    @param ri32_dataSize size of the complete mask
    @param ri32_pgn PGN to use for the upload
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoBroadcast (const iISOName_c& rrefc_isoNameSender, HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
    { return MultiSend_c::sendIsoBroadcast (rrefc_isoNameSender, rhpb_data, ri32_dataSize, ri32_pgn, rrefen_sendSuccessNotify); }

  /** check if at least one multisend stream is running */
  bool isMultiSendRunning() const { return MultiSend_c::isMultiSendRunning(); }

  /** user function for explicit abort of any running matching stream. */
  void abortSend (const iISOName_c& rrefc_isoNameSender, const iISOName_c& rrefc_isoNameReceiver)
    { MultiSend_c::abortSend (rrefc_isoNameSender, rrefc_isoNameReceiver); }


private:
  /** allow getIMultiSendInstance() access to shielded base class.
      otherwise __IsoAgLib::getMultiSendInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iMultiSend_c& getIMultiSendInstance( uint8_t rui8_instance );
  #else
  friend iMultiSend_c& getIMultiSendInstance( void );
  #endif
}; // end class


#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiSend_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iMultiSend_c& getIMultiSendInstance( uint8_t rui8_instance = 0 )
  { return static_cast<iMultiSend_c&>(__IsoAgLib::getMultiSendInstance(rui8_instance)); }
#else
  /** C-style function, to get access to the unique MultiSend_c singleton instance */
  inline iMultiSend_c& getIMultiSendInstance( void )
  { return static_cast<iMultiSend_c&>(__IsoAgLib::getMultiSendInstance()); }
#endif

} // end namespace IsoAgLib
#endif
