/*
  hal_can_interface.h: CAN interface between BIOS/OS and IsoAgLib to
    concentrate CAN handling abstraction within one module

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* ********************************************************** */
/** \file library/xgpl_src/IsoAgLib/hal/Dj1/can/hal_can_interface.h
 * The module halCanInterface implements a standard
 * interface for the CAN interactions of the IsoAgLib
 * This way the the adoption of the IsoAgLib can be
 * better restricted to the various BIOS specific
 * directories xgpl_src/IsoAgLib/hal/xx/can/ .
 */
/* ********************************************************** */

#ifndef _HAL_DJ1_CAN_INTERFACE_H_
#define _HAL_DJ1_CAN_INTERFACE_H_

#include <IsoAgLib/isoaglib_config.h>
#include "../typedef.h"


namespace __IsoAgLib { class Ident_c; class CanPkg_c; }

namespace __HAL 
{

  /**
    Get the Elaped time since the last recieved message for the given 
    message object
      @param aui8_busNr number of the BUS to check
      @param aui8_msgobjNr number of the MsgObj to check
      @return elapse_time  number of ms since last message on object
  */
  extern int32_t Can_TxObjElapseTime ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr );


  /**
    deliver amount of messages in buffer
    (interesting for RX objects)
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return number of messages in buffer or negative error code:
      HAL_CONFIG_ERR == BUS not initialised
      HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern uint16_t Can_ObjBufferCount ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr );


  /**
    deliver amount of messages which can be placed in buffer
    (interesting for TX objects)
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return number of messages which can be placed in buffer or negative error code:
      HAL_CONFIG_ERR == BUS not initialised
      HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_ObjBufferSpace ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr );


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
  extern int16_t Can_GlobalInit  ( uint8_t aui8_busNr, uint16_t ab_baudrate, 
              uint16_t aui16_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg );


  /**
    set the global masks
    (11bit, 29bit and last msg mask)
    @param aui8_busNr number of the BUS to config (default 0)
    @param aui16_maskStd 11bit global mask
    @param aui32_maskExt 29bit global mask
    @param aui32_maskLastmsg mask of last CAN msg
    @return HAL_NO_ERR == no error
            HAL_RANGE_ERR == wrong BUS number
  */
  extern int16_t Can_GlobalMask ( uint8_t aui8_busNr, uint16_t aui16_maskStd, 
                           uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg );


  /**
    close CAN BUS
    (important to close BUS before re-init with other baudrate)
    @param aui8_busNr number of the BUS to close (default 0)
    @return HAL_NO_ERR == no error
            HAL_RANGE_ERR == wrong BUS number
            HAL_CONFIG_ERR == BUS previously not initialised
  */
  extern int16_t Can_Close ( uint8_t aui8_busNr );


  /**
    config a MsgObj
    (buffer size is defined locally in implementation of this interface;
    all received msgs should get a time stamp;
    class Ident_c has ident and type 11/29bit)
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param arc_ident filter ident value of this MsgObj
    <!--@param Type  0=Std, 1=Ext, 2=Both  (Both goes to extended)-->
    @param ab_rxtx 0==RX receive; 1==TX transmit
    @return HAL_NO_ERR == no error;
            HAL_BUSY_ERR == this MsgObj is already used
            HAL_CONFIG_ERR == BUS not initialised or error during buffer allocation
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_ObjectInit ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, 
                               __IsoAgLib::Ident_c& arc_ident, uint8_t ab_rxtx );


  /**
    change the Ident_c of an already initialised MsgObj
    (class __IsoAgLib::Ident_c has ident and type 11/29bit)
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param arc_ident filter ident value of this MsgObj
    <!--@param Type  0=Std, 1=Ext, 2=Both  (Both goes to extended)-->
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_ObjectChange ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, 
                                             __IsoAgLib::Ident_c& arc_ident );


  /**
    lock a MsgObj to avoid further placement of messages into buffer.
    @param rui8_busNr number of the BUS to config
    @param rui8_msgobjNr number of the MsgObj to config
    @param rb_doLock true==lock(default); false==unlock
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_ObjectLock ( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, bool rb_doLock );


  /**
    close a MsgObj
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised, MsgObj previously not used or buffer memory not freed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_ObjClose ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr );


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
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised, MsgObj is no send object
            HAL_NOACT_ERR == BUS OFF
            HAL_OVERFLOW_ERR == send buffer overflowed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_ObjSend ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr,
                                               __IsoAgLib::CanPkg_c* apc_data );


  /**
    get the ident of a received message to decide about the further
    processing before the whole data string is retreived
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param reflIdent reference to the var, where the ident should be inserted
    @return HAL_NO_ERR       == No problem
            HAL_CONFIG_ERR   == BUS not initialised, MsgObj is no RX object
            HAL_NOACT_ERR    == BUS OFF
            HAL_OVERFLOW_ERR == Recieve buffer overflowed
            HAL_RANGE_ERR    == wrong BUS or MsgObj number
            HAL_WARN_ERR     == BUS WARN or no received message
  */
  extern int32_t Can_ReadObjRxIdent ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, 
                                                          int32_t &reflIdent );


  /**
    transfer front element in buffer into the pointed CanPkg_c;
    DON'T clear this item from buffer.
    @see can_useMsgobjPopFront for explicit clear of this front item
    functions:
    * setIdent(Ident_c& arc_ident)
      -> set ident arc_ident of received msg in CANPkg
    * uint8_t setData(uint8_t ab_dlc, uint8_t* apb_data)
      -> set DLC in CanPkg_c from ab_dlc and insert data from uint8_t string apb_data
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param apc_data pointer to CanPkg_c instance with data to send
    @return HAL_NO_ERR       == no error;
            HAL_CONFIG_ERR   == BUS not initialised, MsgObj is no RX object
            HAL_NOACT_ERR    == BUS OFF
            HAL_OVERFLOW_ERR == send buffer overflowed
            HAL_RANGE_ERR    == wrong BUS or MsgObj number
            HAL_WARN_ERR     == BUS WARN or no received message
  */
  extern int16_t Can_ReadObjRx ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, 
                                              __IsoAgLib::CanPkg_c* apc_data );


  /**
    clear the buffer of a MsgObj (e.g. to stop sending retries)
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  extern int16_t Can_PurgeObj ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr );

  int32_t can_getMaxSendDelay ( uint8_t aui8_busNr );

} /* end namespace __HAL */

#endif /* _HAL_DJ1_CAN_INTERFACE_H_ - This must be the last line of the file */


