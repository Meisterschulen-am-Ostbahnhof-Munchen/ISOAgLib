/***************************************************************************
                          can.h - definition of Hardware Abstraction
                                  Layer for CAN functions
                                  for A2
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
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

/* ************************************************************ */
/** \file IsoAgLib/hal/esx/can/can.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adopted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the PC version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */

#ifndef _HAL_A2_CAN_H_
#define _HAL_A2_CAN_H_

#include "hal_can_interface.h"
#include "../errcodes.h"

/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaption to new platform
 */
namespace HAL
{
  /* *********************************************************** */
  /** \name CAN BIOS functions for global status checking per BUS */
/*@{*/

  /**
    test if the CAN BUS is in WARN state
    @param aui8_busNr number of the BUS to check (default 0)
    @return true == CAN BUS is in WARN state, else normal operation
  */
  inline bool can_stateGlobalWarn(uint8_t aui8_busNr = 0)
    {return __HAL::can_stateGlobalWarn(aui8_busNr);};

  /**
    test if the CAN BUS is in OFF state
    @param aui8_busNr number of the BUS to check (default 0)
    @return true == CAN BUS is in OFF state, else normal operation
  */
  inline bool can_stateGlobalOff(uint8_t aui8_busNr = 0)
    {return __HAL::can_stateGlobalOff(aui8_busNr);};

  /**
    test if the CAN BUS is in Blocked state, which can be a sign
    for CAN controllers which are configured with different baudrates;
    this is the case if neither succesfull sent nor received msg
    is detcted AND CAN controller is in WARN or OFF state
    (the time since last succ. send/rec and the time of WARN/OFF
     can be defined with CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING
     in the application specific config file isoaglib_config
     -> should not be to short to avoid false alarm)
    @param aui8_busNr number of the BUS to check (default 0)
    @return true == CAN BUS is in blocked state, else normal operation
  */
  inline bool can_stateGlobalBlocked(uint8_t aui8_busNr = 0)
    {return __HAL::can_stateGlobalBlocked(aui8_busNr);};

  /**
    deliver the baudrate of the CAN BUS in [kbaud]
    @param aui8_busNr number of the BUS to check (default 0)
    @return BUS load of the last second [kbaud]
  */
  inline int32_t can_stateGlobalBusload(uint8_t aui8_busNr = 0)
    {return __HAL::can_stateGlobalBusload(aui8_busNr);};

  /**
    check if a send try on this BUS caused an Bit1Error
    (occurs if >1 controller try to send msg with same ident)
    ==> ISO 11783 forces stop of retries in this case for
        adress claim
        @param aui8_busNr number of the BUS to check
        @return true -> Bit1Err occured
  */
  inline bool can_stateGlobalBit1err(uint8_t aui8_busNr = 0)
    {return __HAL::can_stateGlobalBit1err(aui8_busNr);};

  /*@}*/

  /* ******************************************************************* */
  /** \name CAN BIOS functions for status checking of one specific MsgObj */
/*@{*/

  /**
    deliver the timestamp of last successfull CAN send action
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return timestamp of last successful send
            OR -1 if aui8_msgObjNr corresponds to no valid send obj
  */
  inline int32_t can_stateMsgobjTxok(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_stateMsgobjTxok(aui8_busNr, aui8_msgobjNr);};

  /**
    check if a send MsgObj can't send msgs from buffer to the
    BUS (detecetd by comparing the inactive time with
    CONFIG_CAN_MAX_SEND_WAIT_TIME (defined in isoaglib_config)
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return true -> longer than CONFIG_CAN_MAX_SEND_WAIT_TIME no msg sent on BUS
  */
  inline bool can_stateMsgobjSendproblem(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_stateMsgobjSendproblem(aui8_busNr, aui8_msgobjNr);};

  /**
    test if buffer of a MsgObj is full (e.g. no more
    msg can be put into buffer (important for TX objects))
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return true -> buffer is full -> no further can_send allowed
  */
  inline bool can_stateMsgobjOverflow(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_stateMsgobjOverflow(aui8_busNr, aui8_msgobjNr);};

  /**
    deliver amount of messages in buffer
    (interesting for RX objects)
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return number of messages in buffer or negative error code:
      C_CONFIG == BUS not initialised
      C_RANGE == wrong BUS or MsgObj number
  */
  inline uint16_t can_stateMsgobjBuffercnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_stateMsgobjBuffercnt(aui8_busNr, aui8_msgobjNr);};

  /**
    deliver amount of messages which can be placed in buffer
    (interesting for TX objects)
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return number of messages which can be placed in buffer or negative error code:
      C_CONFIG == BUS not initialised
      C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_stateMsgobjFreecnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_stateMsgobjFreecnt(aui8_busNr, aui8_msgobjNr);};

  /**
    check if MsgObj is currently locked
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return true -> MsgObj is currently locked
  */
  inline bool can_stateMsgobjLocked( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
    {return __HAL::can_stateMsgobjLocked( aui8_busNr, aui8_msgobjNr );};
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
    @return C_NO_ERR == no error;
            C_RANGE == wrong BUS nr or wrong baudrate;
            C_WARN == BUS previously initialised - no problem if only masks had to be changed
  */
  inline int16_t can_configGlobalInit(uint8_t aui8_busNr, uint16_t ab_baudrate, uint16_t ab_maskStd,
        uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg)
    {return __HAL::can_configGlobalInit(aui8_busNr, ab_baudrate, ab_maskStd,
        aui32_maskExt, aui32_maskLastmsg);};

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
  inline int16_t can_configGlobalMask(uint8_t aui8_busNr, uint16_t ab_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg)
    {return __HAL::can_configGlobalMask(aui8_busNr, ab_maskStd, aui32_maskExt, aui32_maskLastmsg);};

  /**
    close CAN BUS
    (important to close BUS before re-init with other baudrate)
    @param aui8_busNr number of the BUS to close (default 0)
    @return C_NO_ERR == no error;
            C_RANGE == wrong BUS nr;
            C_CONFIG == BUS previously not initialised
  */
  inline int16_t can_configGlobalClose(uint8_t aui8_busNr)
    {return __HAL::can_configGlobalClose(aui8_busNr);};

  /** wait until specified timeout or until next CAN message receive
   *  @return true -> there are CAN messages waiting for process. else: return due to timeout
   */
  inline bool can_waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval )
  { return __HAL::can_waitUntilCanReceiveOrTimeout( aui16_timeoutInterval );};

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
    @return C_NO_ERR == no error;
            C_BUSY == this MsgObj is already used
            C_CONFIG == BUS not initialised or error during buffer allocation
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_configMsgobjInit(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, uint8_t ab_rxtx)
    {return __HAL::can_configMsgobjInit(aui8_busNr, aui8_msgobjNr, arc_ident, ab_rxtx);};

  /**
    change the Ident_c of an already initialised MsgObj
    (class __IsoAgLib::Ident_c has ident and type 11/29bit)
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param arc_ident filter ident of this MsgObj
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised or ident can't be changed
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_configMsgobjChgid(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident)
    {return __HAL::can_configMsgobjChgid(aui8_busNr, aui8_msgobjNr, arc_ident);};

  /**
    lock a MsgObj to avoid further placement of messages into buffer.
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param ab_doLock true==lock(default); false==unlock
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
    */
  inline int16_t can_configMsgobjLock( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, bool ab_doLock = true )
    {return __HAL::can_configMsgobjLock( aui8_busNr, aui8_msgobjNr, ab_doLock);};

  /**
    change the the send rate for one MsgObj by setting the minimum
    pause time between two messages [msec.]
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param aui16_minSendPause minimum send pause between two sent messages [msec.]
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised or ident can't be changed
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_configMsgobjSendpause(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, uint16_t aui16_minSend)
    {return __HAL::can_configMsgobjSendpause(aui8_busNr, aui8_msgobjNr, aui16_minSend);};

  /**
    close a MsgObj
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised, MsgObj previously not used or buffer memory not freed
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_configMsgobjClose(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_configMsgobjClose(aui8_busNr, aui8_msgobjNr);};

  /*@}*/

  /* ****************************************************** */
  /** \name CAN BIOS functions for use of on specific MsgObj */
/*@{*/

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
  inline int16_t can_useMsgobjSend(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data)
    {return __HAL::can_useMsgobjSend(aui8_busNr, aui8_msgobjNr, apc_data);};

  /**
    get the ident of a received message to decide about the further
    processing before the whole data string is retreived
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param reflIdent reference to the var, where the ident should be inserted
    @return error code
    C_NO_ERR == No problem
    C_CONFIG == BUS not initialised, MsgObj is no RX object
    C_NOACT == BUS OFF
    C_OVERFLOW == send buffer overflowed
    C_RANGE == wrong BUS or MsgObj number
    C_WARN == BUS WARN or no received message
  */
  inline int32_t can_useMsgobjReceivedIdent(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, int32_t &reflIdent)
    {return __HAL::can_useMsgobjReceivedIdent(aui8_busNr, aui8_msgobjNr, reflIdent);};

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
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised, MsgObj is no RX object
            C_NOACT == BUS OFF
            C_OVERFLOW == send buffer overflowed
            C_RANGE == wrong BUS or MsgObj number
            C_WARN == BUS WARN or no received message
  */
  inline int16_t can_useMsgobjGet(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data)
    {return __HAL::can_useMsgobjGet(aui8_busNr, aui8_msgobjNr, apc_data);};

  /**
    Either register the currenct front item of buffer as not relevant,
    or just pop the front item, as it was processed.
    This explicit pop is needed, as one CAN message shall be served to
    several CanCustomer_c instances, as long as one of them indicates a
    succesfull process of the received message.
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
  */
  inline void can_useMsgobjPopFront(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
  {__HAL::can_useMsgobjPopFront(aui8_busNr, aui8_msgobjNr);};


  /**
    clear th buffer of a MsgObj (e.g. to stop sending retries)
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @return C_NO_ERR == no error;
            C_CONFIG == BUS not initialised
            C_RANGE == wrong BUS or MsgObj number
  */
  inline int16_t can_useMsgobjClear(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_useMsgobjClear(aui8_busNr, aui8_msgobjNr);};

  /*@}*/
}
#endif
