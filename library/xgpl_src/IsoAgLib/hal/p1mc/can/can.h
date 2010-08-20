/*
  can.h: definition of Hardware Abstraction Layer for CAN functions
    for P1MC

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/p1mc/can/can.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the PC version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */

#ifndef _HAL_P1MC_CAN_H_
#define _HAL_P1MC_CAN_H_

#include "hal_can_interface.h"
#include "../errcodes.h"

#include "../../hal_can.h"

/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaptation to new platform
 */
namespace HAL {

  /* *********************************************************** */
  /** \name CAN BIOS functions for global status checking per BUS */
  /*@{*/

  /**
    test if the CAN BUS is in WARN state
    @param rui8_busNr number of the BUS to check (default 0)
    @return true == CAN BUS is in WARN state, else normal operation
  */
  inline bool can_stateGlobalWarn( uint8_t rui8_busNr ) {
    return __HAL::can_stateGlobalWarn( rui8_busNr );
  };

  /**
    test if the CAN BUS is in OFF state
    @param rui8_busNr number of the BUS to check (default 0)
    @return true == CAN BUS is in OFF state, else normal operation
  */
  inline bool can_stateGlobalOff( uint8_t rui8_busNr ) {
    return __HAL::can_stateGlobalOff( rui8_busNr );
  };

#ifdef USE_CAN_MEASURE_BUSLOAD
  /**
    deliver the baudrate of the CAN BUS in [kbaud]
    @param rui8_busNr number of the BUS to check (default 0)
    @return BUS load of the last second [kbaud]
  */
  inline int32_t can_stateGlobalBusload( uint8_t rui8_busNr ) {
    return __HAL::can_stateGlobalBusload( rui8_busNr );
  };
#endif

#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
#warning USE_CAN_SEND_DELAY_MEASUREMENT inline int32_t can_getMaxSendDelay( uint8_t aui8_busNr ) not implemented for this platform!
#endif

  /**
    check if a send try on this BUS caused an Bit1Error
    (occurs if >1 controller try to send msg with same ident)
    ==> ISO 11783 forces stop of retries in this case for
        adress claim
        @param rui8_busNr number of the BUS to check
        @return true -> Bit1Err occured
  */
  inline bool can_stateGlobalBit1err( uint8_t rui8_busNr ) {
    return __HAL::can_stateGlobalBit1err( rui8_busNr );
  };

  /*@}*/

  /* ******************************************************************* */
  /** \name CAN BIOS functions for status checking of one specific MsgObj */
  /*@{*/

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  /**
    test if buffer of a MsgObj is full (e.g. no more
    msg can be put into buffer (important for TX objects))
    @param rui8_busNr number of the BUS to check
    @param rui8_msgobjNr number of the MsgObj to check
    @return true -> buffer is full -> no further can_send allowed
  */
  inline bool can_stateMsgobjOverflow( uint8_t rui8_busNr, uint8_t rui8_msgobjNr ) {
    return __HAL::can_stateMsgobjOverflow( rui8_busNr, rui8_msgobjNr );
  };
#endif

  /**
    deliver amount of messages in buffer
    (interesting for RX objects)
    @param rui8_busNr number of the BUS to check
    @param rui8_msgobjNr number of the MsgObj to check
    @return number of messages in buffer or negative error code:
      C_CONFIG == BUS not initialised
      C_RANGE == wrong BUS or MsgObj number
  */
  inline uint16_t can_stateMsgobjBuffercnt( uint8_t rui8_busNr, uint8_t rui8_msgobjNr ) {
    return __HAL::can_stateMsgobjBuffercnt( rui8_busNr, rui8_msgobjNr );
  };

  /**
    deliver amount of messages which can be placed in buffer
    (interesting for TX objects)
    @param rui8_busNr number of the BUS to check
    @param rui8_msgobjNr number of the MsgObj to check
    @return number of messages which can be placed in buffer or negative error code:
      C_CONFIG == BUS not initialised
      C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_stateMsgobjFreecnt( uint8_t rui8_busNr, uint8_t rui8_msgobjNr ) {
    return __HAL::can_stateMsgobjFreecnt( rui8_busNr, rui8_msgobjNr );
  };

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
    @return C_NO_ERR == no error;
            C_RANGE == wrong BUS nr or wrong baudrate;
            C_WARN == BUS previously initialised - no problem if only masks had to be changed
  */
  inline int16_t can_configGlobalInit( uint8_t rui8_busNr, uint16_t rb_baudrate, uint16_t rb_maskStd,
                                       uint32_t rui32_maskExt, uint32_t rui32_maskLastmsg ) {
    return __HAL::can_configGlobalInit( rui8_busNr, rb_baudrate, rb_maskStd,
                                        rui32_maskExt, rui32_maskLastmsg );
  };

  /**
    set the global masks
    (11bit, 29bit and last msg mask)
    @param rui8_busNr number of the BUS to config (default 0)
    @param rui16_maskStd 11bit global mask
    @param rui32_maskExt 29bit global mask
    @param rui32_maskLastmsg mask of last CAN msg
    @return C_NO_ERR == no error;
            C_RANGE == wrong BUS nr
  */
  inline int16_t can_configGlobalMask( uint8_t rui8_busNr, uint16_t rb_maskStd, uint32_t rui32_maskExt, uint32_t rui32_maskLastmsg ) {
    return __HAL::can_configGlobalMask( rui8_busNr, rb_maskStd, rui32_maskExt, rui32_maskLastmsg );
  };

  /**
    close CAN BUS
    (important to close BUS before re-init with other baudrate)
    @param rui8_busNr number of the BUS to close (default 0)
    @return C_NO_ERR == no error;
            C_RANGE == wrong BUS nr;
            C_CONFIG == BUS previously not initialised
  */
  inline int16_t can_configGlobalClose( uint8_t rui8_busNr ) {
    return __HAL::can_configGlobalClose( rui8_busNr );
  };

  /** wait until specified timeout or until next CAN message receive
   *  @return true -> there are CAN messages waiting for process. else: return due to timeout
   */
  inline bool can_waitUntilCanReceiveOrTimeout( uint16_t rui16_timeoutInterval ) {
    return __HAL::can_waitUntilCanReceiveOrTimeout( rui16_timeoutInterval );
  };

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
    @return C_NO_ERR == no error;
            C_BUSY == this MsgObj is already used
            C_CONFIG == BUS not initialised or error during buffer allocation
            C_RANGE == wrong BUS or MsgObj number
  */
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  inline int16_t can_configMsgobjInit( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::Ident_c& rrefc_ident, uint8_t rb_rxtx ) {
    return __HAL::can_configMsgobjInit( rui8_busNr, rui8_msgobjNr, rrefc_ident, rb_rxtx );
  };
#else
#warning SYSTEM_WITH_ENHANCED_CAN_HAL not implemented for this platform!
// int16_t can_configMsgobjInit( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, __IsoAgLib::Ident_c& arc_mask, uint8_t ab_rxtx );
#endif

  inline int16_t can_configMsgobjSendpause( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, uint16_t rui16_minSend ) {
    return __HAL::can_configMsgobjSendpause( rui8_busNr, rui8_msgobjNr, rui16_minSend );
  };

  /**
    close a MsgObj
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised, MsgObj previously not used or buffer memory not freed
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_configMsgobjClose( uint8_t rui8_busNr, uint8_t rui8_msgobjNr ) {
    return __HAL::can_configMsgobjClose( rui8_busNr, rui8_msgobjNr );
  };

  /*@}*/

  /* ****************************************************** */
  /** \name CAN BIOS functions for use of on specific MsgObj */
  /*@{*/

  /**
    send a message via a MsgObj;
    CanPkg_c (or derived object) must provide (virtual)
    functions:
    * Ident_c& getIdent() -> deliver ident of msg to send
    * uint8_t getData(MASK_TYPE& reft_ident, uint8_t& refui8_identType,
                      uint8_t& refb_dlc, uint8_t* pb_data)
      -> put DLC in referenced ref_dlc and insert data in uint8_t string pb_data
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @param rpc_data pointer to CanPkg_c instance with data to send
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised, MsgObj is no send object
            C_NOACT == BUS OFF
            C_OVERFLOW == send buffer overflowed
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_useMsgobjSend( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::CanPkg_c* rpc_data ) {
    return __HAL::can_useMsgobjSend( rui8_busNr, rui8_msgobjNr, rpc_data );
  };

#if defined SYSTEM_WITH_ENHANCED_CAN_HAL
  /**
    transfer front element in buffer into the pointed CanPkg_c;
    DON'T clear this item from buffer.
    @see can_useMsgobjPopFront for explicit clear of this front item
    functions:
    * setIdent(Ident_c& rrefc_ident)
      -> set ident rrefc_ident of received msg in CANPkg
    * uint8_t setData(uint8_t rb_dlc, uint8_t* rpb_data)
      -> set DLC in CanPkg_c from rb_dlc and insert data from uint8_t string rpb_data
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @param rpc_data pointer to CanPkg_c instance with data to send
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised, MsgObj is no RX object
            C_NOACT == BUS OFF
            C_OVERFLOW == send buffer overflowed
            C_RANGE == wrong BUS or MsgObj number
            C_WARN == BUS WARN or no received message
  */
  inline int16_t can_useMsgobjGet( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::CanPkg_c* rpc_data ) {
    return __HAL::can_useMsgobjGet( rui8_busNr, rui8_msgobjNr, rpc_data );
  };

  /**
    Either register the currenct front item of buffer as not relevant,
    or just pop the front item, as it was processed.
    This explicit pop is needed, as one CAN message shall be served to
    several CANCustomer_c instances, as long as one of them indicates a
    succesfull process of the received message.
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
  */
  inline void can_useMsgobjPopFront( uint8_t rui8_busNr, uint8_t rui8_msgobjNr ) {
    __HAL::can_useMsgobjPopFront( rui8_busNr, rui8_msgobjNr );
  };

//inline int32_t can_useNextMsgobjNumber( uint8_t aui8_busNr, uint32_t& reflIdent, uint32_t& refui32_msgId, uint8_t& refui8_msgtype, int32_t& i32_rcvTime )
#warning SYSTEM_WITH_ENHANCED_CAN_HAL can_useNextMsgobjNumber not implemented for this plaform
#endif

  /**
    clear th buffer of a MsgObj (e.g. to stop sending retries)
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_useMsgobjClear( uint8_t rui8_busNr, uint8_t rui8_msgobjNr ) {
    return __HAL::can_useMsgobjClear( rui8_busNr, rui8_msgobjNr );
  };

  /*@}*/
}

#endif
