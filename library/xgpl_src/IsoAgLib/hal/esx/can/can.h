/*
  can.h: definition of Hardware Abstraction Layer for CAN functions
    for ESX

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/esx/can/can.h
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

#ifndef _HAL_ESX_CAN_H_
#define _HAL_ESX_CAN_H_

#include "hal_can_interface.h"
#include "../errcodes.h"

namespace HAL
{
  inline bool can_stateGlobalWarn(uint8_t aui8_busNr)
    {return __HAL::can_stateGlobalWarn(aui8_busNr);}

  inline bool can_stateGlobalOff(uint8_t aui8_busNr)
    {return __HAL::can_stateGlobalOff(aui8_busNr);}

 #ifdef USE_CAN_MEASURE_BUSLOAD
  inline int32_t can_stateGlobalBusload(uint8_t aui8_busNr)
    {return __HAL::can_stateGlobalBusload(aui8_busNr);}
 #endif

  inline bool can_stateGlobalBit1err(uint8_t aui8_busNr)
    {return __HAL::can_stateGlobalBit1err(aui8_busNr);}

  inline bool can_stateMsgobjOverflow(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_stateMsgobjOverflow(aui8_busNr, aui8_msgobjNr);}

  inline uint16_t can_stateMsgobjBuffercnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_stateMsgobjBuffercnt(aui8_busNr, aui8_msgobjNr);}

  inline int16_t can_stateMsgobjFreecnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_stateMsgobjFreecnt(aui8_busNr, aui8_msgobjNr);}

  inline int16_t can_configGlobalInit(uint8_t aui8_busNr, uint16_t ab_baudrate, uint16_t aui16_maskStd,
        uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg)
    {return __HAL::can_configGlobalInit(aui8_busNr, ab_baudrate, aui16_maskStd,
        aui32_maskExt, aui32_maskLastmsg);}

  inline int16_t can_configGlobalMask(uint8_t aui8_busNr, uint16_t aui16_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg)
    {return __HAL::can_configGlobalMask(aui8_busNr, aui16_maskStd, aui32_maskExt, aui32_maskLastmsg);}

  inline int16_t can_configGlobalClose(uint8_t aui8_busNr)
    {return __HAL::can_configGlobalClose(aui8_busNr);}

  inline bool can_waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval )
  { return __HAL::can_waitUntilCanReceiveOrTimeout( aui16_timeoutInterval );}

  inline int16_t can_configMsgobjInit(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, uint8_t ab_rxtx)
    {return __HAL::can_configMsgobjInit(aui8_busNr, aui8_msgobjNr, arc_ident, ab_rxtx);}

  inline int16_t can_configMsgobjSendpause(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, uint16_t aui16_minSend)
    {return __HAL::can_configMsgobjSendpause(aui8_busNr, aui8_msgobjNr, aui16_minSend);}

  inline int16_t can_configMsgobjClose(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_configMsgobjClose(aui8_busNr, aui8_msgobjNr);}

  inline int16_t can_useMsgobjSend(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data)
    {return __HAL::can_useMsgobjSend(aui8_busNr, aui8_msgobjNr, apc_data);}

  inline int16_t can_useMsgobjClear(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
    {return __HAL::can_useMsgobjClear(aui8_busNr, aui8_msgobjNr);}

#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
  inline int32_t can_getMaxSendDelay(uint8_t aui8_busN)
    {return __HAL::can_getMaxSendDelay();}
#endif

  /*@}*/
}
#endif
