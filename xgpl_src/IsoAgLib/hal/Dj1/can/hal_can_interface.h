/***************************************************************************
             hal_can_interface.h - CAN interface between
                           BIOS/OS and IsoAgLib to concentrate
                           CAN handling abstraction within
                           one module
                -------------------
    begin    : 12 Jun 2006
    copyright: (C) 2006 Joel T. Morton (DICKEY-john Corp.)
    email    : jmorton@dickey-john:com
    type     : Header
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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

/* ********************************************************** */
/** \file hal_can_interface.h
 * The module halCanInterface implements a standard
 * interface for the CAN interactions of the IsoAgLib
 * This way the the adoption of the IsoAgLib can be
 * better restricted to the various BIOS specific
 * directories xgpl_src/IsoAgLib/hal/xx/can/ .
 */
/* ********************************************************** */

#ifndef _HAL_DJ1_CAN_INTERFACE_H_
#define _HAL_DJ1_CAN_INTERFACE_H_

#include "../typedef.h"


namespace __IsoAgLib { class Ident_c; class CANPkg_c; }

namespace __HAL 
{

  /**
    Get the Elaped time since the last recieved message for the given 
    message object
      @param rui8_busNr number of the BUS to check
      @param rui8_msgobjNr number of the MsgObj to check
      @return elapse_time  number of ms since last message on object
  */
  extern int32_t Can_TxObjElapseTime ( uint8_t rui8_busNr, uint8_t rui8_msgobjNr );


  /**
    deliver amount of messages in buffer
    (interesting for RX objects)
    @param rui8_busNr number of the BUS to check
    @param rui8_msgobjNr number of the MsgObj to check
    @return number of messages in buffer or negative error code:
      HAL_CONFIG_ERR == BUS not initialised
      HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern uint16_t Can_ObjBufferCount ( uint8_t rui8_busNr, uint8_t rui8_msgobjNr );


  /**
    deliver amount of messages which can be placed in buffer
    (interesting for TX objects)
    @param rui8_busNr number of the BUS to check
    @param rui8_msgobjNr number of the MsgObj to check
    @return number of messages which can be placed in buffer or negative error code:
      HAL_CONFIG_ERR == BUS not initialised
      HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_ObjBufferSpace ( uint8_t rui8_busNr, uint8_t rui8_msgobjNr );


  /**
    init CAN BUS with given baudrate and masks
    (11bit, 29bit and last msg mask)
    @param rui8_busNr number of the BUS to init (default 0)
    @param rb_baudrate baudrate in [kbaud]
    @param rui16_maskStd 11bit global mask
    @param rui32_maskExt 29bit global mask
    @param rui32_maskLastmsg mask of last CAN msg
    @return HAL_NO_ERR == no error;
            HAL_RANGE_ERR == wrong BUS nr or wrong baudrate;
            HAL_WARN_ERR == BUS previously initialised - no problem if only masks had to be changed
  */
  extern int16_t Can_GlobalInit  ( uint8_t rui8_busNr, uint16_t rb_baudrate, 
              uint16_t rb_maskStd, uint32_t rui32_maskExt, uint32_t rui32_maskLastmsg );


  /**
    set the global masks
    (11bit, 29bit and last msg mask)
    @param rui8_busNr number of the BUS to config (default 0)
    @param rui16_maskStd 11bit global mask
    @param rui32_maskExt 29bit global mask
    @param rui32_maskLastmsg mask of last CAN msg
    @return HAL_NO_ERR == no error
            HAL_RANGE_ERR == wrong BUS number
  */
  extern int16_t Can_GlobalMask ( uint8_t rui8_busNr, uint16_t rb_maskStd, 
                           uint32_t rui32_maskExt, uint32_t rui32_maskLastmsg );


  /**
    close CAN BUS
    (important to close BUS before re-init with other baudrate)
    @param rui8_busNr number of the BUS to close (default 0)
    @return HAL_NO_ERR == no error
            HAL_RANGE_ERR == wrong BUS number
            HAL_CONFIG_ERR == BUS previously not initialised
  */
  extern int16_t Can_Close ( uint8_t rui8_busNr );


  /**
    config a MsgObj
    (buffer size is defined locally in implementation of this interface;
    all received msgs should get a time stamp;
    class Ident_c has ident and type 11/29bit)
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @param Ident filter ident value of this MsgObj
    @param Type  0=Std, 1=Ext, 2=Both  (Both goes to extended)
    @param rb_rxtx 0==RX receive; 1==TX transmit
    @return HAL_NO_ERR == no error;
            HAL_BUSY_ERR == this MsgObj is already used
            HAL_CONFIG_ERR == BUS not initialised or error during buffer allocation
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_ObjectInit ( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, 
                               __IsoAgLib::Ident_c& rrefc_ident, uint8_t rb_rxtx );


  /**
    change the Ident_c of an already initialised MsgObj
    (class __IsoAgLib::Ident_c has ident and type 11/29bit)
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @param Ident filter ident value of this MsgObj
    @param Type  0=Std, 1=Ext, 2=Both  (Both goes to extended)
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_ObjectChange ( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, 
                                             __IsoAgLib::Ident_c& rrefc_ident );


  /**
    close a MsgObj
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised, MsgObj previously not used or buffer memory not freed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_ObjClose ( uint8_t rui8_busNr, uint8_t rui8_msgobjNr );


  /**
    send a message via a MsgObj;
    CANPkg_c (or derived object) must provide (virtual)
    functions:
    * Ident_c& getIdent() -> deliver ident of msg to send
    * uint8_t getData(uint8_t& refb_dlc, uint8_t* pb_data)
      -> put DLC in referenced ref_dlc and insert data in uint8_t string pb_data
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @param rpc_data pointer to CANPkg_c instance with data to send
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised, MsgObj is no send object
            HAL_NOACT_ERR == BUS OFF
            HAL_OVERFLOW_ERR == send buffer overflowed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_ObjSend ( uint8_t rui8_busNr, uint8_t rui8_msgobjNr,
                                               __IsoAgLib::CANPkg_c* rpc_data );


  /**
    get the ident of a received message to decide about the further
    processing before the whole data string is retreived
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @param reflIdent reference to the var, where the ident should be inserted
    @return HAL_NO_ERR       == No problem
            HAL_CONFIG_ERR   == BUS not initialised, MsgObj is no RX object
            HAL_NOACT_ERR    == BUS OFF
            HAL_OVERFLOW_ERR == Recieve buffer overflowed
            HAL_RANGE_ERR    == wrong BUS or MsgObj number
            HAL_WARN_ERR     == BUS WARN or no received message
  */
  extern int32_t Can_ReadObjRxIdent ( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, 
                                                          int32_t &reflIdent );


  /**
    transfer front element in buffer into the pointed CANPkg_c;
    DON'T clear this item from buffer.
    @see can_useMsgobjPopFront for explicit clear of this front item
    functions:
    * setIdent(Ident_c& rrefc_ident)
      -> set ident rrefc_ident of received msg in CANPkg
    * uint8_t setData(uint8_t rb_dlc, uint8_t* rpb_data)
      -> set DLC in CANPkg_c from rb_dlc and insert data from uint8_t string rpb_data
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @param rpc_data pointer to CANPkg_c instance with data to send
    @return HAL_NO_ERR       == no error;
            HAL_CONFIG_ERR   == BUS not initialised, MsgObj is no RX object
            HAL_NOACT_ERR    == BUS OFF
            HAL_OVERFLOW_ERR == send buffer overflowed
            HAL_RANGE_ERR    == wrong BUS or MsgObj number
            HAL_WARN_ERR     == BUS WARN or no received message
  */
  extern int16_t Can_ReadObjRx ( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, 
                                              __IsoAgLib::CANPkg_c* rpc_data );


  /**
    clear the buffer of a MsgObj (e.g. to stop sending retries)
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_PurgeObj ( uint8_t rui8_busNr, uint8_t rui8_msgobjNr );

  int32_t can_getMaxSendDelay ( uint8_t rui8_busNr );

} /* end namespace __HAL */

#endif /* _HAL_DJ1_CAN_INTERFACE_H_ - This must be the last line of the file */


