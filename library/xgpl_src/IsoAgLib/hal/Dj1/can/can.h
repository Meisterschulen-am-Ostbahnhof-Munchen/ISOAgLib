/*
  can.h: definition of Hardware Abstraction Layer for CAN functions
    for DICKEY-john hardware

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/Dj1/can/can.h
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

#ifndef _HAL_DJ1_CAN_H_
#define _HAL_DJ1_CAN_H_

#include "hal_can_interface.h"
#include "../errcodes.h"

namespace __HAL
{
  extern "C"
  {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_Dj1/DjBios1.h>
  }
}

namespace HAL
{
  inline bool can_stateGlobalWarn (uint8_t aui8_busNr)
  { return ( (__HAL::DjBios_CanGlobalWarn(aui8_busNr) == __HAL::BIOS_TRUE) ); }

  inline bool can_stateGlobalOff ( uint8_t aui8_busNr )
  { return ( (__HAL::DjBios_CanBusOff(aui8_busNr) == __HAL::BIOS_TRUE) ); }

  inline bool can_stateGlobalBlocked ( uint8_t aui8_busNr )
  { return ( (__HAL::DjBios_CanBlocked(aui8_busNr) == __HAL::BIOS_TRUE) ); }

  inline int32_t can_stateGlobalBusload ( uint8_t aui8_busNr )
  { return ( (__HAL::DjBios_CanBitCount ( aui8_busNr ) + 500) / 1000 ); }

  inline bool can_stateGlobalBit1err ( uint8_t aui8_busNr )
  { return ( (__HAL::DjBios_CanBit1Err(aui8_busNr) == __HAL::BIOS_TRUE) ); }

  inline int32_t can_stateMsgobjTxok ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
  { return ( __HAL::DjBios_CanObjTxTimeStamp ( aui8_busNr, aui8_msgobjNr ) ); }

  inline bool can_stateMsgobjOverflow ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
  { return ( (__HAL::DjBios_CanObjTxBuffFull(aui8_busNr, aui8_msgobjNr) == __HAL::BIOS_CAN_Q_FULL) ); }

  inline uint16_t can_stateMsgobjBuffercnt ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
  { return ( __HAL::Can_ObjBufferCount(aui8_busNr, aui8_msgobjNr) ); }

  inline int16_t can_stateMsgobjFreecnt ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
  { return ( __HAL::Can_ObjBufferSpace(aui8_busNr, aui8_msgobjNr) ); }

  inline bool can_stateMsgobjLocked ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
  { return ( __HAL::DjBios_CanIsObjLocked ( aui8_busNr, aui8_msgobjNr ) ); }

  inline int16_t can_configGlobalInit  ( uint8_t aui8_busNr, uint16_t ab_baudrate,
              uint16_t aui16_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg )
  { return ( __HAL::Can_GlobalInit(aui8_busNr, ab_baudrate, aui16_maskStd, aui32_maskExt, aui32_maskLastmsg) ); }

  inline int16_t can_configGlobalMask ( uint8_t aui8_busNr, uint16_t aui16_maskStd,
                           uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg )
  { return ( __HAL::Can_GlobalMask(aui8_busNr, aui16_maskStd, aui32_maskExt, aui32_maskLastmsg) ); }

  inline int16_t can_configGlobalClose(uint8_t aui8_busNr)
  { return ( __HAL::Can_Close(aui8_busNr) ); }

  inline bool can_waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval )
  { return ( __HAL::DjBios_CanWaitUntilRx(aui16_timeoutInterval) ); }

  inline int16_t can_configMsgobjInit(uint8_t aui8_busNr, uint8_t aui8_msgobjNr,
                            __IsoAgLib::Ident_c& arc_ident, uint8_t ab_rxtx)
  { return ( __HAL::Can_ObjectInit(aui8_busNr, aui8_msgobjNr, arc_ident, ab_rxtx) ); }

  inline int16_t can_configMsgobjChgid ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr,
                                           __IsoAgLib::Ident_c& arc_ident )
  { return ( __HAL::Can_ObjectChange(aui8_busNr, aui8_msgobjNr, arc_ident) ); }

  inline int16_t can_configMsgobjSendpause ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, uint16_t aui16_minSend )
  { return ( HAL_NO_ERR ); }

  inline int16_t can_configMsgobjClose ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
  { return ( __HAL::Can_ObjClose(aui8_busNr, aui8_msgobjNr) ); }

  inline int16_t can_useMsgobjSend ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data )
  { return ( __HAL::Can_ObjSend(aui8_busNr, aui8_msgobjNr, apc_data) ); }

  inline int32_t can_useMsgobjReceivedIdent ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, int32_t &reflIdent )
  { return ( __HAL::Can_ReadObjRxIdent(aui8_busNr, aui8_msgobjNr, reflIdent) ); }

  inline int16_t can_useMsgobjGet(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data)
  { return ( __HAL::Can_ReadObjRx(aui8_busNr, aui8_msgobjNr, apc_data) ); }

  inline void can_useMsgobjPopFront ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
  { (void)__HAL::DjBios_CanObjPop ( aui8_busNr, aui8_msgobjNr ); }

  inline int16_t can_useMsgobjClear ( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
  { return __HAL::Can_PurgeObj ( aui8_busNr, aui8_msgobjNr ); }

  inline int32_t can_getMaxSendDelay ( uint8_t aui8_busNr )
  { return __HAL::can_getMaxSendDelay ( aui8_busNr ); }
}
#endif
