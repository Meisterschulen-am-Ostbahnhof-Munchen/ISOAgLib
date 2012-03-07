/*
  hal_system.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    central system functions

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* ************************************************************ */
/** \file IsoAgLib/hal/hal_system.h
  * include dependent on used target (defined in
	  IsoAgLib/isoaglib_config.h) the suitable HAL
		specific header for central system functions.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDENT_SYSTEM_H_
#define _HAL_INDEPENDENT_SYSTEM_H_

#include <IsoAgLib/isoaglib_config.h>
#include "hal_config.h"
#include "hal_typedef.h"

namespace HAL
{
  /**
    open the system with system specific function call
    @return error state (HAL_NO_ERR == o.k.)
  */
  int16_t open_system();

  /**
    close the system with system specific function call
    @return error state (HAL_NO_ERR == o.k.)
  */
  int16_t closeSystem();

  /** check if open_System() has already been called */
  bool isSystemOpened();

  /**
    configure the watchdog of the system with the settings of configEsx
    @return error state (HAL_NO_ERR == o.k.)
      or DATA_CHANGED on new values -> need call of wdReset to use new settings
  */
  int16_t configWatchdog();

  /** reset the watchdog to use new configured watchdog settings */
  void wdReset();

  /** trigger the watchdog */
  void wdTriggern();

  /**
    get the system time in [ms]
    @return [ms] since start of the system
  */
  int32_t getTime(void);

  int16_t getSnr(uint8_t *snrDat);

  int32_t getSerialNr(int16_t* pi16_errCode = NULL);

  /** start the Task Timer -> time between calls of Task Manager */
  void startTaskTimer();

  /**
    check if D+/CAN_EN is active
    (if NO_CAN_EN_CHECK is defined this function return always return ON)
    @return ON(1) or OFF(0)
  */
  int16_t getOn_offSwitch();

  /**
    get the main power voltage
    @return voltage of power [mV]
  */
  int16_t getAdcUbat();

  /**
    get the voltage of the external reference 8.5Volt for work of external sensors
    @return voltage at external reference [mV]
  */
  int16_t getAdc_u85();

  /**
    activate the power selfholding to perform system
    stop (f.e. store values) actions after loss of CAN_EN
  */
  void stayingAlive();

  /** shut off the whole system (set power down) */
  void powerDown();

  /**
    Sleep and yield CPU for other tasks.
    Especially important for Multitasking environments
    as Windows/Linux running with CAN-Server for example.
    NOTE: The sleep can also be shorter depending on the
          operating system, even no-sleep on
          single-tasking systems!
   */    
  void sleep_max_ms( uint32_t ms );
}


/// INCLUDE CONCRETE SYSTEM-HAL-IMPLEMENTATION
/// ==========================================

#ifdef HAL_PATH_ISOAGLIB_SYSTEM
#  define _hal_system_header_ <HAL_PATH_ISOAGLIB_SYSTEM/system.h>
#  include _hal_system_header_
#else
#  error "No HAL_PATH_ISOAGLIB_SYSTEM set. This is a mandatory module!"
#endif


#endif
