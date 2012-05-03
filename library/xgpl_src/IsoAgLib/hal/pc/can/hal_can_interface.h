/*
  hal_can_interface.h: CAN interface between BIOS/OS and IsoAgLib to
    concentrate CAN handling abstraction within one module

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file hal/pc/can/hal_can_interface.h
 * The module halCanInterface implements a standard
 * interface for the CAN interactions of the IsoAgLib
 * This way the the adaptation of the IsoAgLib can be
 * better restricted to the various BIOS specific
 * directories xgpl_src/IsoAgLib/hal/can/ .
 */
/* ********************************************************** */

#ifndef _HAL_PC_CAN_INTERFACE_H_
#define _HAL_PC_CAN_INTERFACE_H_

#include <IsoAgLib/isoaglib_config.h>
#include "../typedef.h"


namespace __IsoAgLib { class Ident_c; class CanPkg_c;}


namespace __HAL {
/* *********************************************************** */
/** \name CAN BIOS functions for global status checking per BUS */
/*@{*/

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

/* ******************************************************************* */
/** \name CAN BIOS functions for status checking of one specific MsgObj */
/*@{*/

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
/**
  test if buffer of a MsgObj is full (e.g. no more
  msg can be put into buffer (important for TX objects))
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check
  @return true -> buffer is full -> no further can_send allowed
*/
bool can_stateMsgobjOverflow(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);

#endif

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

/* ************************************************************ */
/** \name CAN BIOS functions for configuration of global CAN BUS */
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
int16_t can_configGlobalInit(uint8_t aui8_busNr, uint16_t ab_baudrate, uint16_t aui16_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg);

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
int16_t can_configGlobalMask(uint8_t aui8_busNr, uint16_t aui16_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg);

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


/* ***************************************************************** */
/** \name CAN BIOS functions for configuration of one specific MsgObj */
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

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
int16_t can_configMsgobjInit(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, uint8_t ab_rxtx);
#else
int16_t can_configMsgobjInit(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, __IsoAgLib::Ident_c& arc_mask, uint8_t ab_rxtx);
#endif



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

/* ****************************************************** */
/** \name CAN BIOS functions for use of on specific MsgObj */
/*@{*/

/**
  send a message via a MsgObj;
  CanPkg_c (or derived object) must provide (virtual)
  functions:
  * Ident_c& getIdent() -> deliver ident of msg to send
  * void getData(MASK_TYPE& rt_ident, uint8_t& rui8_identType,
                 uint8_t& rb_dlcTarget, uint8_t* pb_dataTarget)
    -> put DLC in referenced r_dlc and insert data in uint8_t string pb_data
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param apc_data pointer to CanPkg_c instance with data to send
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised, MsgObj is no send object
          HAL_NOACT_ERR == BUS OFF
          HAL_OVERFLOW_ERR == send buffer overflowed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_useMsgobjSend(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data);

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
/**
  get the ident of a received message to decide about the further
  processing before the whole data string is retreived
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param reflIdent reference to the var, where the ident should be inserted
  @return error code
  HAL_NO_ERR == No problem
  HAL_CONFIG_ERR == BUS not initialised, MsgObj is no RX object
  HAL_NOACT_ERR == BUS OFF
  HAL_OVERFLOW_ERR == send buffer overflowed
  HAL_RANGE_ERR == wrong BUS or MsgObj number
  HAL_WARN_ERR == BUS WARN or no received message
*/


int32_t can_useNextMsgobjNumber(uint8_t aui8_busNr, uint32_t& reflIdent, uint32_t& refui32_msgId, uint8_t& refui8_msgtype, int32_t& i32_rcvTime);


/**
  transfer front element in buffer into the pointed CanPkg_c;
  DON'T clear this item from buffer.
  @see can_useMsgobjPopFront for explicit clear of this front item
  functions:
  * setIdent(Ident_c& arc_ident)
    -> set ident arc_ident of received msg in CANPkg
  * uint8_t setDataFromString(uint8_t* apb_data, uint8_t ab_dlc)
    -> set DLC in CanPkg_c from ab_dlc and insert data from uint8_t string apb_data
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param apc_data pointer to CanPkg_c instance with data to send
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised, MsgObj is no RX object
          HAL_NOACT_ERR == BUS OFF
          HAL_OVERFLOW_ERR == send buffer overflowed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
          HAL_WARN_ERR == BUS WARN or no received message
*/
int16_t can_useMsgobjGet(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data);
/**
  Either register the currenct front item of buffer as not relevant,
  or just pop the front item, as it was processed.
  This explicit pop is needed, as one CAN message shall be served to
  several CanCustomer_c  instances, as long as one of them indicates a
  succesfull process of the received message.
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
*/
void can_useMsgobjPopFront(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);

#endif

/**
  clear th buffer of a MsgObj (e.g. to stop sending retries)
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_useMsgobjClear(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);

#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
int32_t can_getMaxSendDelay(uint8_t aui8_busNr);
#endif

/*@}*/

}
#endif
