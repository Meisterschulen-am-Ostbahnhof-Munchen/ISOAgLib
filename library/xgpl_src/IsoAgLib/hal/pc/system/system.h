/*
  system.h: definition of Hardware Abstraction Layer for central
    system functions for debug/simulation system on PC

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/pc/system/system.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the PC version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */
#ifndef _HAL_PC_SYSTEM_H_
#define _HAL_PC_SYSTEM_H_

#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"
#include "system_target_extensions.h"
#include "isoaglibthread_c.h"

#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/hal/pc/can/can_typedef.h>


/** decide if HAL for PC shall use simulated sensor input for U_BAT and
    reference voltage monitoring - but in this case the sensor package
		of IsoAgLib must be installed */
#define USE_SENSOR_FOR_SYSTEM_MONITORING 0

#if USE_SENSOR_FOR_SYSTEM_MONITORING
	#include <supplementary_driver/hal/pc/sensor/sensor_target_extensions.h>
#endif

#include <cstdio>



/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaptation to new platform
 */
namespace HAL
{
  /**
    open the system with system specific function call
    @return error state (HAL_NO_ERR == o.k.)
  */
  inline int16_t  open_system()
  {
      return __HAL::open_system();
  };
  /**
    close the system with system specific function call
    @return error state (HAL_NO_ERR == o.k.)
  */
  inline int16_t  closeSystem()
  {
      return __HAL::closeSystem();
  };
  /** check if open_System() has already been called */
  inline bool isSystemOpened( void ) { return __HAL::isSystemOpened();};

  /**
    configure the watchdog of the system with the
    settings of configEsx
    @return error state (HAL_NO_ERR == o.k.)
      or DATA_CHANGED on new values -> need call of wdReset to use new settings
    @see wdReset
  */
  inline int16_t configWatchdog()
    {return __HAL::configWatchdog();};

  /**
    reset the watchdog to use new configured watchdog settings
    @see configWd
  */
  inline int16_t  wdReset(void)
    {return __HAL::wdReset();};

  /** trigger the watchdog */
  inline void wdTriggern(void)
    {__HAL::wdTriggern();};

  /**
    get the system time in [ms]
    @return [ms] since start of the system
  */
  inline int32_t getTime(void)
    {return __HAL::getTime();};

  inline int16_t getSnr(uint8_t *snrDat)
    {return __HAL::getSnr(snrDat);};

  inline int32_t getSerialNr(int16_t* pi16_errCode = NULL)
  {
    uint8_t uint8 [6];
    int16_t errCode = __HAL::getSnr(uint8);
    if (pi16_errCode) *pi16_errCode = errCode;
    // ESX Serial number is coded in BCD. As we only get 21 bits,
    // we can take only a part of the information transmitted here.
    //  - uint8[0] is the year of construction -> 7 bits
    //  - uint8[2] and uint8[3] a contract numering -> 14 bits
    return (__IsoAgLib::bcd2dec(uint8[2]) * 100 + __IsoAgLib::bcd2dec(uint8[3])) + (__IsoAgLib::bcd2dec(uint8[0]) << 14);
  };

  /**
    start the Task Timer -> time between calls of Task Manager
  */
  inline void startTaskTimer ( void )
    {__HAL::startTaskTimer ();};

  /**
    check if D+/CAN_EN is active
    (if NO_CAN_EN_CHECK is defined this function return always return ON)
    @return ON(1) or OFF(0)
  */
  inline int16_t  getOn_offSwitch(void)
  {
    #if defined(NO_CAN_EN_CHECK)
      return ON;
    #else
      return __HAL::getOn_offSwitch();
    #endif
  };

  /**
    get the main power voltage
    @return voltage of power [mV]
  */
  inline int16_t  getAdcUbat( void )
	#if USE_SENSOR_FOR_SYSTEM_MONITORING
    {return (33 * __HAL::getAdc(GET_U_C));};
	#else
		{return 13500;};
	#endif
  /**
    get the voltage of the external reference 8.5Volt for work of external sensors
    @return voltage at external reference [mV]
  */
  inline int16_t  getAdc_u85( void )
	#if USE_SENSOR_FOR_SYSTEM_MONITORING
    {int16_t i16_temp = __HAL::getAdc(GET_U_EXT_8_5_V);
     return ((i16_temp * 14) + ((i16_temp * 67)/100));};
	#else
	{return 8500;};
	#endif

  /**
    activate the power selfholding to perform system
    stop (f.e. store values) actions after loss of CAN_EN
  */
  inline void stayingAlive(void)
    {__HAL::stayingAlive();};

  /**
    shut off the whole system (set power down)
  */
  inline void powerDown(void)
    {__HAL::powerDown();};

  /**
    switch relais on or off
    @param bitState true -> Relais ON
  */
  inline void setRelais(bool bitState)
  {__HAL::setRelais(bitState);};

/*@}*/

} //end HAL

#ifdef USE_MUTUAL_EXCLUSION
#include <IsoAgLib/hal/generic_utils/system/mutex_pthread.h>
#endif

#endif
