/***************************************************************************
                          hal_can_interface.h - CAN interface between
                                        BIOS/OS and IsoAgLib to concentrate
                                        CAN handling abstraction within
                                        one module
                             -------------------
    begin                : Wed Jun 29 2001
    copyright            : (C) 2001 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
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
 * directories xgpl_src/IsoAgLib/hal/can/ .
 */
/* ********************************************************** */

#ifndef _HAL_PC_CAN_INTERFACE_H_
#define _HAL_PC_CAN_INTERFACE_H_

#include "../typedef.h"

namespace __IsoAgLib { class Ident_c; class CANPkg_c;}

namespace __HAL {
/* *********************************************************** */
/** \name CAN BIOS functions for global status checking per BUS */
/*@{*/

/**
  test if the CAN BUS is in WARN state
  @param rui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in WARN state, else normal operation
*/
bool can_stateGlobalWarn(uint8_t rui8_busNr = 0);

/**
  test if the CAN BUS is in OFF state
  @param rui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in OFF state, else normal operation
*/
bool can_stateGlobalOff(uint8_t rui8_busNr = 0);

/**
  test if the CAN BUS is in Blocked state, which can be a sign
  for CAN controllers which are configured with different baudrates;
  this is the case if neither succesfull sent nor received msg
  is detcted AND CAN controller is in WARN or OFF state
  (the time since last succ. send/rec and the time of WARN/OFF
   can be defined with CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING
   in the application specific config file isoaglib_config
   -> should not be to short to avoid false alarm)
  @param rui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in blocked state, else normal operation
*/
bool can_stateGlobalBlocked(uint8_t rui8_busNr = 0);

/**
  deliver the baudrate of the CAN BUS in [kbaud]
  @param rui8_busNr number of the BUS to check (default 0)
  @return BUS load of the last second [kbaud]
*/
int32_t can_stateGlobalBusload(uint8_t rui8_busNr = 0);

/**
  check if a send try on this BUS caused an Bit1Error
  (occurs if >1 controller try to send msg with same ident)
  ==> ISO 11783 forces stop of retries in this case for
      adress claim
      @param rui8_busNr number of the BUS to check
      @return true -> Bit1Err occured
*/
bool can_stateGlobalBit1err(uint8_t rui8_busNr = 0);
/*@}*/

/* ******************************************************************* */
/** \name CAN BIOS functions for status checking of one specific MsgObj */
/*@{*/

/**
  deliver the timestamp of last successfull CAN send action
  @param rui8_busNr number of the BUS to check
  @param rui8_msgobjNr number of the MsgObj to check
  @return timestamp of last successful send
          OR -1 if rui8_msgObjNr corresponds to no valid send obj
*/
int32_t can_stateMsgobjTxok(uint8_t rui8_busNr, uint8_t rui8_msgobjNr);

/**
  check if a send MsgObj can't send msgs from buffer to the
  BUS (detecetd by comparing the inactive time with
  CONFIG_CAN_MAX_SEND_WAIT_TIME (defined in isoaglib_config)
  @param rui8_busNr number of the BUS to check
  @param rui8_msgobjNr number of the MsgObj to check
  @return true -> longer than CONFIG_CAN_MAX_SEND_WAIT_TIME no msg sent on BUS
*/
bool can_stateMsgobjSendproblem(uint8_t rui8_busNr, uint8_t rui8_msgobjNr);

/**
  test if buffer of a MsgObj is full (e.g. no more
  msg can be put into buffer (important for TX objects))
  @param rui8_busNr number of the BUS to check
  @param rui8_msgobjNr number of the MsgObj to check
  @return true -> buffer is full -> no further can_send allowed
*/
bool can_stateMsgobjOverflow(uint8_t rui8_busNr, uint8_t rui8_msgobjNr);

/**
  deliver amount of messages in buffer
  (interesting for RX objects)
  @param rui8_busNr number of the BUS to check
  @param rui8_msgobjNr number of the MsgObj to check
  @return number of messages in buffer or negative error code:
    HAL_CONFIG_ERR == BUS not initialised
    HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_stateMsgobjBuffercnt(uint8_t rui8_busNr, uint8_t rui8_msgobjNr);

/**
  deliver amount of messages which can be placed in buffer
  (interesting for TX objects)
  @param rui8_busNr number of the BUS to check
  @param rui8_msgobjNr number of the MsgObj to check
  @return number of messages which can be placed in buffer or negative error code:
    HAL_CONFIG_ERR == BUS not initialised
    HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_stateMsgobjFreecnt(uint8_t rui8_busNr, uint8_t rui8_msgobjNr);
/*@}*/

/* ************************************************************ */
/** \name CAN BIOS functions for configuration of global CAN BUS */
/*@{*/

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
int16_t can_configGlobalInit(uint8_t rui8_busNr, uint16_t rb_baudrate, uint16_t rb_maskStd, uint32_t rui32_maskExt, uint32_t rui32_maskLastmsg);

/**
  set the global masks
  (11bit, 29bit and last msg mask)
  @param rui8_busNr number of the BUS to config (default 0)
  @param rui16_maskStd 11bit global mask
  @param rui32_maskExt 29bit global mask
  @param rui32_maskLastmsg mask of last CAN msg
  @return HAL_NO_ERR == no error;
          HAL_RANGE_ERR == wrong BUS nr
*/
int16_t can_configGlobalMask(uint8_t rui8_busNr, uint16_t rb_maskStd, uint32_t rui32_maskExt, uint32_t rui32_maskLastmsg);

/**
  change the the send rate for one MsgObj by setting the minimum
  pause time between two messages [msec.]
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @param rui16_minSendPause minimum send pause between two sent messages [msec.]
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjSendpause(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, uint16_t rui16_minSend);

/**
  close CAN BUS
  (important to close BUS before re-init with other baudrate)
  @param rui8_busNr number of the BUS to close (default 0)
  @return HAL_NO_ERR == no error;
          HAL_RANGE_ERR == wrong BUS nr;
          HAL_CONFIG_ERR == BUS previously not initialised
*/
int16_t can_configGlobalClose(uint8_t rui8_busNr);
/*@}*/


/* ***************************************************************** */
/** \name CAN BIOS functions for configuration of one specific MsgObj */
/*@{*/
/**
  config a MsgObj
  (buffer size is defined locally in implementation of this interface;
  all received msgs should get a time stamp;
  class Ident_c has ident and type 11/29bit)
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @param rrefc_ident filter ident of this MsgObj
  @param rb_rxtx 0==RX receive; 1==TX transmit
  @return HAL_NO_ERR == no error;
          C_BUSY == this MsgObj is already used
          HAL_CONFIG_ERR == BUS not initialised or error during buffer allocation
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjInit(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::Ident_c& rrefc_ident, uint8_t rb_rxtx);

/**
  change the Ident_c of an already initialised MsgObj
  (class __IsoAgLib::Ident_c has ident and type 11/29bit)
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @param rrefc_ident filter ident of this MsgObj
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjChgid(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::Ident_c& rrefc_ident);

/**
	lock a MsgObj to avoid further placement of messages into buffer.
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
	@param rb_doLock true==lock(default); false==unlock
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
	*/
int16_t can_configMsgobjLock( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, bool rb_doLock = true );

/**
  close a MsgObj
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised, MsgObj previously not used or buffer memory not freed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjClose(uint8_t rui8_busNr, uint8_t rui8_msgobjNr);
/*@}*/

/* ****************************************************** */
/** \name CAN BIOS functions for use of on specific MsgObj */
/*@{*/

/**
  send a message via a MsgObj;
  CANPkg_c (or derived object) must provide (virtual)
  functions:
  * Ident_c& getIdent() -> deliver ident of msg to send
  * void getData(MASK_TYPE& reft_ident, uint8_t& refui8_identType,
                 uint8_t& refb_dlcTarget, uint8_t* pb_dataTarget)
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
int16_t can_useMsgobjSend(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::CANPkg_c* rpc_data);

/**
  get the ident of a received message to decide about the further
  processing before the whole data string is retreived
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @param reflIdent reference to the var, where the ident should be inserted
  @return error code
  HAL_NO_ERR == No problem
  HAL_CONFIG_ERR == BUS not initialised, MsgObj is no RX object
  HAL_NOACT_ERR == BUS OFF
  HAL_OVERFLOW_ERR == send buffer overflowed
  HAL_RANGE_ERR == wrong BUS or MsgObj number
  HAL_WARN_ERR == BUS WARN or no received message
*/
int32_t can_useMsgobjReceivedIdent(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, int32_t &reflIdent);

/**
	transfer front element in buffer into the pointed CANPkg_c;
	DON'T clear this item from buffer.
	@see can_useMsgobjPopFront for explicit clear of this front item
  functions:
  * setIdent(Ident_c& rrefc_ident)
    -> set ident rrefc_ident of received msg in CANPkg
  * uint8_t setDataString(uint8_t* rpb_data, uint8_t rb_dlc)
    -> set DLC in CANPkg_c from rb_dlc and insert data from uint8_t string rpb_data
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @param rpc_data pointer to CANPkg_c instance with data to send
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised, MsgObj is no RX object
          HAL_NOACT_ERR == BUS OFF
          HAL_OVERFLOW_ERR == send buffer overflowed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
          HAL_WARN_ERR == BUS WARN or no received message
*/
int16_t can_useMsgobjGet(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::CANPkg_c* rpc_data);
/**
	Either register the currenct front item of buffer as not relevant,
	or just pop the front item, as it was processed.
	This explicit pop is needed, as one CAN message shall be served to
	several CANCustomer_c instances, as long as one of them indicates a
	succesfull process of the received message.
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
*/
void can_useMsgobjPopFront(uint8_t rui8_busNr, uint8_t rui8_msgobjNr);

/**
  clear th buffer of a MsgObj (e.g. to stop sending retries)
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_useMsgobjClear(uint8_t rui8_busNr, uint8_t rui8_msgobjNr);
/*@}*/

}
#endif
