/*
  hal_can.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    CAN communication

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* ************************************************************ */
/** \file IsoAgLib/hal/hal_can.h
  * include dependent on used target (defined in
	  IsoAgLib/isoaglib_config.h) the suitable HAL
		specific header for CAN communication.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDENT_CAN_H_
#define _HAL_INDEPENDENT_CAN_H_

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/hal_config.h>


/** CAN state labels for C167
		( and comparable ) CAN controllers
	*/
typedef enum C16xCtrlStatusReg_en
{
	CanStateNoErr = 0x0000,
	CanStateWarn = 0x4000,
	CanStateOff = 0x8000,
	CanStateWarnOrOff = 0xC000,
	CanStateTxOk = 0x0800,
	CanStateRxOk = 0x1000,
	CanStateLecErrMask = 0x0700,
	CanStateStuffErr = 0x0100,
	CanStateFormErr = 0x0200,
	CanStateAckErr = 0x0300,
	CanStateBit1Err = 0x0400,
	CanStateBit0Err = 0x0500,
	CanStateCrcErr = 0x0600
} C16xCtrlStatusReg_t;


namespace __IsoAgLib { class Ident_c; class CanPkg_c;}


namespace HAL
{
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
      C_CONFIG == BUS not initialised
      C_RANGE == wrong BUS or MsgObj number
  */
  uint16_t can_stateMsgobjBuffercnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);

  /**
    deliver amount of messages which can be placed in buffer
    (interesting for TX objects)
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return number of messages which can be placed in buffer or negative error code:
      C_CONFIG == BUS not initialised
      C_RANGE == wrong BUS or MsgObj number
  */
  int16_t can_stateMsgobjFreecnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);

  /**
    init CAN BUS with given baudrate and masks
    (11bit, 29bit and last msg mask)
    @param aui8_busNr number of the BUS to init (default 0)
    @param ab_baudrate baudrate in [kbaud]
    @param aui16_maskStd 11bit global mask
    @param aui32_maskExt 29bit global mask
    @param aui32_maskLastmsg mask of last CAN msg
    @return C_NO_ERR == no error;
            C_RANGE == wrong BUS nr or wrong baudrate;
            C_WARN == BUS previously initialised - no problem if only masks had to be changed
  */
  int16_t can_configGlobalInit(uint8_t aui8_busNr, uint16_t ab_baudrate, uint16_t aui16_maskStd,
        uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg);

  /**
    set the global masks
    (11bit, 29bit and last msg mask)
    @param aui8_busNr number of the BUS to config (default 0)
    @param aui16_maskStd 11bit global mask
    @param aui32_maskExt 29bit global mask
    @param aui32_maskLastmsg mask of last CAN msg
    @return C_NO_ERR == no error;
            C_RANGE == wrong BUS nr
  */
  int16_t can_configGlobalMask(uint8_t aui8_busNr, uint16_t aui16_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg);

  /**
    close CAN BUS
    (important to close BUS before re-init with other baudrate)
    @param aui8_busNr number of the BUS to close (default 0)
    @return C_NO_ERR == no error;
            C_RANGE == wrong BUS nr;
            C_CONFIG == BUS previously not initialised
  */
  int16_t can_configGlobalClose(uint8_t aui8_busNr);

  /** wait until specified timeout or until next CAN message receive
   *  @return true -> there are CAN messages waiting for process. else: return due to timeout
   */
  bool can_waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval );

  /**
    config a MsgObj
    (buffer size is defined locally in implementation of this interface;
    all received msgs should get a time stamp;
    class Ident_c has ident and type 11/29bit)
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param arc_ident filter ident of this MsgObj
    @param ab_rxtx 0==RX receive; 1==TX transmit
    @return C_NO_ERR == no error;
            C_BUSY == this MsgObj is already used
            C_CONFIG == BUS not initialised or error during buffer allocation
            C_RANGE == wrong BUS or MsgObj number
  */
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  int16_t can_configMsgobjInit(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, uint8_t ab_rxtx);
#else
  int16_t can_configMsgobjInit(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, __IsoAgLib::Ident_c& arc_mask, uint8_t ab_rxtx);
#endif

  /**
    change the the send rate for one MsgObj by setting the minimum pause
    time between two messages [msec.]

    Optional, can be implemented if it is supported by the BIOS.
    Currently not used by ISOAgLib.

    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param aui16_minSend minimum send pause between two sent messages [msec.]
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  int16_t can_configMsgobjSendpause(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, uint16_t aui16_minSend);

  /**
    close a MsgObj
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised, MsgObj previously not used or buffer memory not freed
            C_RANGE == wrong BUS or MsgObj number
  */
  int16_t can_configMsgobjClose(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);

  /**
    send a message via a MsgObj;
    CanPkg_c (or derived object) must provide (virtual)
    functions:
    * Ident_c& getIdent() -> deliver ident of msg to send
    * uint8_t getData(uint8_t& rb_dlc, uint8_t* pb_data)
      -> put DLC in referenced r_dlc and insert data in uint8_t string pb_data
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param apc_data pointer to CanPkg_c instance with data to send
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised, MsgObj is no send object
            C_NOACT == BUS OFF
            C_OVERFLOW == send buffer overflowed
            C_RANGE == wrong BUS or MsgObj number
  */
  int16_t can_useMsgobjSend(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data);

  /**
    clear th buffer of a MsgObj (e.g. to stop sending retries)
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised
            C_RANGE == wrong BUS or MsgObj number
  */
  int16_t can_useMsgobjClear(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  int32_t can_useNextMsgobjNumber(uint8_t aui8_busNr, uint32_t& reflIdent, uint32_t& refui32_msgId, uint8_t& refui8_msgtype, int32_t& i32_rcvTime);

  int16_t can_useMsgobjGet(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data);

  void can_useMsgobjPopFront(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);

  bool can_stateMsgobjOverflow(uint8_t aui8_busNr, uint8_t aui8_msgobjNr);
#endif

#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
  int32_t can_getMaxSendDelay(uint8_t aui8_busNr);
#endif
} // HAL


/// INCLUDE CONCRETE SYSTEM-HAL-IMPLEMENTATION
/// ==========================================

#ifdef HAL_PATH_ISOAGLIB_CAN
#  define _hal_can_header_ <HAL_PATH_ISOAGLIB_CAN/can.h>
#  include _hal_can_header_
#else
#  error "No HAL_PATH_ISOAGLIB_CAN set. (CAN is a mandatory module)"
#endif

#endif
