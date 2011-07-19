/*
  system.h: definition of Hardware Abstraction Layer for central
    system functions for debug/simulation system on C2C

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/c2c/system/system.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity.
*/

#ifndef _HAL_C2C_SYSTEM_H_
#define _HAL_C2C_SYSTEM_H_

#include <cstdio>
#include "../errcodes.h"
#include "system_target_extensions.h"

#include <IsoAgLib/util/impl/util_funcs.h>

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_c2c/c2c10osy.h>
  }
}


namespace HAL
{
  inline int16_t open_system() { return __HAL::open_system(); }

  inline int16_t closeSystem() { return __HAL::closeSystem(); }

  inline bool isSystemOpened() { return __HAL::isSystemOpened(); }

  inline int16_t configWatchdog() { return __HAL::configWatchdog(); }

  inline void wdReset() { (void)__HAL::reset_wd(); }

  inline void wdTriggern() {__HAL::trigger_wd(); }

  inline int32_t getTime() { return __HAL::get_time(); }

  inline int16_t getSnr(uint8_t *snrDat) { return __HAL::get_snr(snrDat); }

  inline int32_t getSerialNr(int16_t* pi16_errCode)
  {
      uint8_t uint8 [6];
      int16_t errCode = __HAL::get_snr(uint8);
      if (pi16_errCode) *pi16_errCode = errCode;
    // ESX Serial number is coded in BCD. As we only get 21 bits,
    // we can take only a part of the information transmitted here.
    //  - uint8[0] is the year of construction -> 7 bits
    //  - uint8[2] and uint8[3] a contract numering -> 14 bits
      return (__IsoAgLib::bcd2dec(uint8[2]) * 100 + __IsoAgLib::bcd2dec(uint8[3])) + (__IsoAgLib::bcd2dec(uint8[0]) << 14);
  };

  inline int16_t getLokalId(uint8_t *Dat) { return __HAL::get_lokal_id(Dat); }

  inline void startTaskTimer() { __HAL::start_task_timer ( T_TASK_BASIC ); }

  /**
    init the Task Call
	This function permits cyclic and/or delayed calls of user functions. If 0 is tranferred as parameter
	for wInterval, the function call will occur only once. (For starting the tasks start task timer
	(word wBasicTick) has to be queried.)
	The ordering of the task into the interrupt system uses the transfer parameter wHandle. If a zero-pointer
	is used in the user function parameter, the function will stop when the handle is called.
	The maximum number of tasks is limited to 4.
  */
  inline int16_t initTaskCall( uint16_t wHandle, uint16_t wInterval, uint16_t wOffset, void (* pfFunction)(void) )
  { return __HAL::init_task_call( wHandle, wInterval, wOffset, pfFunction ); }

  /**
    Get Task Overload
	If a task has already been running and is called up a second time by a timer interrupt,
	a flag is set. The function get_task_overload returns the condition of this flag.
	With reset_task_overload this flag can be deleted.
  */
  inline int16_t getTaskOverload ( uint16_t /*wHandle*/ )
  { return __HAL::get_task_overload (); }

  inline int16_t getAdcUbat() { return (40 * __HAL::get_adc(GET_U_E)); }

  inline int16_t getAdc_u85() { return 8500; }

  inline int16_t getOn_offSwitch()
  {
    #if defined(NO_CAN_EN_CHECK)
      return ON;
    #else
      return __HAL::get_on_off_switch();
    #endif
  };

  inline void stayingAlive() { __HAL::staying_alive(); }

  inline void powerDown() {__HAL::power_down(); }

  inline void delay_us(unsigned int i_tm) {__HAL::delay_us(i_tm); }
  
  inline void sleep_max_ms( uint32_t ms ) { __HAL::delay_us( ms * 1000); }
}

#ifdef USE_MUTUAL_EXCLUSION
#include <IsoAgLib/hal/generic_utils/system/mutex_emulated.h>
#endif


#endif
