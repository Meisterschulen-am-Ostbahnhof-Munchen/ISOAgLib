/***************************************************************************
                          hal_can_interface.h - CAN interface between
                                        BIOS/OS and IsoAgLib to concentrate
                                        CAN handling abstraction within
                                        one module
                             -------------------
    begin                : Wed Jun 29 2001
    copyright            : (C) 2001 - 2004 Dipl.-Inform. Achim Spangler
             : This file was based on the corresponding file in
             : the ESX HAL and modified for the C2C HAL.
             : These changes (C) 2004 - 2005 Michael D. Schmidt
    email                : a.spangler@osb-ag:de
             : mike.schmidt@agcocorp:com
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
/** \file library/xgpl_src/IsoAgLib/hal/c2c/can/hal_can_interface.h
 * The module halCanInterface implements a standard
 * interface for the CAN interactions of the IsoAgLib
 * This way the the adaptation of the IsoAgLib can be
 * better restricted to the various BIOS specific
 * directories xgpl_src/IsoAgLib/hal/xx/can/ .
 */
/* ********************************************************** */

#ifndef _HAL_C2C_CAN_INTERFACE_H_
#define _HAL_C2C_CAN_INTERFACE_H_

#include "../typedef.h"


namespace __IsoAgLib { class Ident_c; class CanPkg_c;}

namespace __HAL {

/* ************************************************** */
/** \name Global Status Per BUS
 *  Functions for status check of global CAN BUS      */
/* ************************************************** */
/*@{*/

#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
int32_t can_getMaxSendDelay(void);
#endif

/**
  test if the CAN BUS is in WARN state
  @param aui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in WARN state, else normal operation
*/
bool can_stateGlobalWarn(uint8_t aui8_busNr = 0);

/**
  test if the CAN BUS is in OFF state
  @param aui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in OFF state, else normal operation
*/
bool can_stateGlobalOff(uint8_t aui8_busNr = 0);

#ifdef USE_CAN_MEASURE_BUSLOAD

/**
  deliver the baudrate of the CAN BUS in [kbaud]
  @param aui8_busNr number of the BUS to check (default 0)
  @return BUS load of the last second [kbaud]
*/
int32_t can_stateGlobalBusload(uint8_t aui8_busNr = 0);

#endif
/**
  check if a send try on this BUS caused an Bit1Error
  (occurs if >1 controller try to send msg with same ident)
  ==> ISO 11783 forces stop of retries in this case for
      adress claim
      @param aui8_busNr number of the BUS to check
      @return true -> Bit1Err occured
*/
bool can_stateGlobalBit1err(uint8_t aui8_busNr = 0);
/*@}*/

/* ************************************************** */
/** \name Specific for one MsgObj
 *  Functions for status check of a single MsgObj     */
/* ************************************************** */
/*@{*/

/**
  test if buffer of a MsgObj is full (e.g. no more
  msg can be put into buffer (important for TX objects))
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check
  @return true -> buffer is full -> no further can_send allowed
*/
bool can_stateMsgobjOverflow(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);


/**
  deliver amount of messages in buffer
  (interesting for RX objects)
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check
  @return number of messages in buffer or negative error code:
    HAL_CONFIG_ERR == BUS not initialised
    HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_stateMsgobjBuffercnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);

/**
  deliver amount of messages which can be placed in buffer
  (interesting for TX objects)
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check
  @return number of messages which can be placed in buffer or negative error code:
    HAL_CONFIG_ERR == BUS not initialised
    HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_stateMsgobjFreecnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);

/*@}*/

/* ************************************************** */
/** \name Global Configuration Per BUS
 *  Functions to configure the global CAN BUS         */
/* ************************************************** */
/*@{*/

/**
  init CAN BUS with given baudrate and masks
  (11bit, 29bit and last msg mask)
  @param aui8_busNr number of the BUS to init (default 0)
  @param ab_baudrate baudrate in [kbaud]
  @param aui16_maskStd 11bit global mask
  @param aui32_maskExt 29bit global mask
  @param aui32_maskLastmsg mask of last CAN msg
  @return HAL_NO_ERR == no error;
          HAL_RANGE_ERR == wrong BUS nr or wrong baudrate;
          HAL_WARN_ERR == BUS previously initialised - no problem if only masks had to be changed
*/
int16_t can_configGlobalInit(uint8_t aui8_busNr, uint16_t ab_baudrate, uint16_t ab_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg);

/**
  set the global masks
  (11bit, 29bit and last msg mask)
  @param aui8_busNr number of the BUS to config (default 0)
  @param aui16_maskStd 11bit global mask
  @param aui32_maskExt 29bit global mask
  @param aui32_maskLastmsg mask of last CAN msg
  @return HAL_NO_ERR == no error;
          HAL_RANGE_ERR == wrong BUS nr
*/
int16_t can_configGlobalMask(uint8_t aui8_busNr, uint16_t ab_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg);

/**
  change the the send rate for one MsgObj by setting the minimum
  pause time between two messages [msec.]
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param aui16_minSendPause minimum send pause between two sent messages [msec.]
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjSendpause(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, uint16_t aui16_minSend);

/**
  close CAN BUS
  (important to close BUS before re-init with other baudrate)
  @param aui8_busNr number of the BUS to close (default 0)
  @return HAL_NO_ERR == no error;
          HAL_RANGE_ERR == wrong BUS nr;
          HAL_CONFIG_ERR == BUS previously not initialised
*/
int16_t can_configGlobalClose(uint8_t aui8_busNr);

/** wait until specified timeout or until next CAN message receive
 *  @return true -> there are CAN messages waiting for process. else: return due to timeout
 */
bool can_waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval );
/*@}*/

int16_t can_useMsgobjSend(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data);

/* ************************************************** */
/** \name Configuration specific for one MsgObj
 *  Functions to configure a specific MsgObj          */
/* ************************************************** */
/*@{*/

/**
  config a MsgObj
  (buffer size is defined locally in implementation of this interface;
  all received msgs should get a time stamp;
  class Ident_c has ident and type 11/29bit)
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param arc_ident filter ident of this MsgObj
  @param ab_rxtx 0==RX receive; 1==TX transmit
  @return HAL_NO_ERR == no error;
          C_BUSY == this MsgObj is already used
          HAL_CONFIG_ERR == BUS not initialised or error during buffer allocation
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjInit(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, uint8_t ab_rxtx);



/**
  close a MsgObj
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised, MsgObj previously not used or buffer memory not freed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjClose(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);
/*@}*/

/* ******************************************** */
/** \name Use of MsgObj
 *  Functions to use a specific MsgObj          */
/* ******************************************** */
/*@{*/

/**
  clear th buffer of a MsgObj (e.g. to stop sending retries)
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_useMsgobjClear(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);
/*@}*/
}
#endif
