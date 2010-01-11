/*
  can.h: definition of Hardware Abstraction Layer for CAN functions
    EES

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _HAL_EES_CAN_H_
#define _HAL_EES_CAN_H_

#include "../config.h"
#include "../errcodes.h"
#include "../typedef.h"

#include "hal_can_interface.h"

namespace HAL {
  /**
    test if the CAN BUS is in WARN state
    @param aui8_busNr number of the BUS to check (default 0)
    @return true == CAN BUS is in WARN state, else normal operation
  */
  inline bool can_stateGlobalWarn( uint8_t /* aui8_busNr */ ) {
    return false;
  };

  /**
    test if the CAN BUS is in OFF state
    @param aui8_busNr number of the BUS to check (default 0)
    @return true == CAN BUS is in OFF state, else normal operation
  */
  inline bool can_stateGlobalOff( uint8_t /* aui8_busNr = 0 */ ) {
    return false;
  };

  /**
    check if a send try on this BUS caused an Bit1Error
    (occurs if >1 controller try to send msg with same ident)
    ==> ISO 11783 forces stop of retries in this case for
        adress claim
        @param aui8_busNr number of the BUS to check
        @return true -> Bit1Err occured
  */
  inline bool can_stateGlobalBit1err( uint8_t /* aui8_busNr = 0 */ ) {
    return false;
  };

  /**
    deliver amount of messages in buffer
    (interesting for RX objects)
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return number of messages in buffer or negative error code:
      C_CONFIG == BUS not initialised
      C_RANGE == wrong BUS or MsgObj number
  */
  inline uint16_t can_stateMsgobjBuffercnt( uint8_t /* aui8_busNr */, uint8_t /* aui8_msgobjNr */ ) {
    return 0;
  };

  /**
    deliver amount of messages which can be placed in buffer
    (interesting for TX objects)
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return number of messages which can be placed in buffer or negative error code:
      C_CONFIG == BUS not initialised
      C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_stateMsgobjFreecnt( uint8_t /* aui8_busNr */, uint8_t /* aui8_msgobjNr */ ) {
    return 5;
  };

  /**
    init CAN BUS with given baudrate and masks
    (11bit, 29bit and last msg mask)
    @param aui8_busNr number of the BUS to init (default 0)
    @param ab_baudrate baudrate in [kbaud]
    @param aui16_maskStd 11bit global mask
    @param aui32_maskExt 29bit global mask
    @param aui32_maskLastmsg mask of last CAN msg
    @return HAL_NO_ERR == no error;
            C_RANGE == wrong BUS nr or wrong baudrate;
            C_WARN == BUS previously initialised - no problem if only masks had to be changed
  */
  inline int16_t can_configGlobalInit( uint8_t /* aui8_busNr */, uint16_t /* ab_baudrate */, uint16_t /* aui16_maskStd */,
                                       uint32_t /* aui32_maskExt */, uint32_t /* aui32_maskLastmsg */ ) {
    return HAL_NO_ERR;
  };

  /**
    set the global masks
    (11bit, 29bit and last msg mask)
    @param aui8_busNr number of the BUS to config (default 0)
    @param aui16_maskStd 11bit global mask
    @param aui32_maskExt 29bit global mask
    @param aui32_maskLastmsg mask of last CAN msg
    @return HAL_NO_ERR == no error;
            C_RANGE == wrong BUS nr
  */
  inline int16_t can_configGlobalMask( uint8_t /* aui8_busNr */, uint16_t /* aui16_maskStd */, uint32_t /* aui32_maskExt */, uint32_t /* aui32_maskLastmsg */ ) {
    return HAL_NO_ERR;
  };

  /**
    close CAN BUS
    (important to close BUS before re-init with other baudrate)
    @param aui8_busNr number of the BUS to close (default 0)
    @return HAL_NO_ERR == no error;
            C_RANGE == wrong BUS nr;
            C_CONFIG == BUS previously not initialised
  */
  inline int16_t can_configGlobalClose( uint8_t /* aui8_busNr */ ) {
    return HAL_NO_ERR;
  };

  /** wait until specified timeout or until next CAN message receive
   *  @return true -> there are CAN messages waiting for process. else: return due to timeout
   */
  inline bool can_waitUntilCanReceiveOrTimeout( uint16_t /* aui16_timeoutInterval */ ) {
    // event driven systems like EES can not get into some kind of blocked state....
    for ( ;; );
    return false;
  };

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
            C_CONFIG == BUS not initialised or error during buffer allocation
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_configMsgobjInit( uint8_t /* aui8_busNr */, uint8_t /* aui8_msgobjNr */, __IsoAgLib::Ident_c& /* arc_ident */, uint8_t /* ab_rxtx */ ) {
    return HAL_NO_ERR;
  };


  inline int16_t can_configMsgobjSendpause( uint8_t /* aui8_busNr */, uint8_t /* aui8_msgobjNr */, uint16_t /* aui16_minSend */ ) {
    return HAL_NO_ERR;
  };

  /**
    close a MsgObj
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @return HAL_NO_ERR == no error;
            C_CONFIG == BUS not initialised, MsgObj previously not used or buffer memory not freed
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_configMsgobjClose( uint8_t /* aui8_busNr */, uint8_t /* aui8_msgobjNr */ ) {
    return HAL_NO_ERR;
  };

  /**
    send a message via a MsgObj;
    CanPkg_c (or derived object) must provide (virtual)
    functions:
    * Ident_c& getIdent() -> deliver ident of msg to send
    * uint8_t getData(MASK_TYPE& rt_ident, uint8_t& rui8_identType,
                      uint8_t& rb_dlc, uint8_t* pb_data)
      -> put DLC in referenced r_dlc and insert data in uint8_t string pb_data
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param apc_data pointer to CanPkg_c instance with data to send
    @return HAL_NO_ERR == no error;
            C_CONFIG == BUS not initialised, MsgObj is no send object
            C_NOACT == BUS OFF
            C_OVERFLOW == send buffer overflowed
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_useMsgobjSend( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data ) {
    return __HAL::can_useMsgobjSend( aui8_busNr, aui8_msgobjNr, apc_data );
  };


  /**
    clear th buffer of a MsgObj (e.g. to stop sending retries)
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @return HAL_NO_ERR == no error;
            C_CONFIG == BUS not initialised
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_useMsgobjClear( uint8_t /* aui8_busNr */, uint8_t /* aui8_msgobjNr */ ) {
    return HAL_NO_ERR;
  };
}
#endif
