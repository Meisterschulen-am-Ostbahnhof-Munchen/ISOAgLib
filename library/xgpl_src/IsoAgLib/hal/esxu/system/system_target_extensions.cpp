/*
  targetExtensions.cc: source for ESXu specific extensions for the HAL
    for central system

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file esxu/system/system_target_extensions.cpp
 * A module targetExtensions should be used
 * for all methods, which can't be simply 
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
 * ********************************************************** */

#include "system.h"

namespace __HAL {

static bool system_is_opened = false;

/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system()
{
	int16_t i16_result = C_NO_ERR;
	if( !system_is_opened )
		{
		tSystem t_biosextSysdata;
		i16_result = open_micro(&t_biosextSysdata);

		system_is_opened = true;

#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "open_micro( &t_biosextSysdata ) returns " << i16_result << "\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
uint8_t buf[64];
CNAMESPACE::sprintf( (char*)buf, "%u ms - open_micro() returns %i\r"
, (uint16_t)__HAL::get_time()
, (int16_t) i16_result
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif
		}

	return i16_result;
}
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/

// MSCHMIDT - I think there is a bug here...  
// I think it should read:
//    if( !get_on_off_switch() )
// Same is true for hal\esx\system\system_target_extensions.cc
//
int16_t closeSystem( void )
{ // if CAN_EN ist active -> shut peripherals off and stay in idle loop
  if ( get_on_off_switch() )
  { // CanEn still active
    set_relais( OFF );
    power_down();
  }
  // trigger Watchdog, till CanEn is off
  while ( get_on_off_switch() ) trigger_wd();
  // power off
  int16_t retval = close_micro();
  system_is_opened = false;

#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "close_micro() returns " << retval << "\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
uint8_t buf[64];
CNAMESPACE::sprintf( (char*)buf, "%u ms - close_micro() returns %i\r"
, (uint16_t)__HAL::get_time()
, (int16_t) retval
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif

  return retval;
}

/** check if open_System() has already been called */
bool isSystemOpened( void )
{
	return system_is_opened;
}

/**
  configure the watchdog of the system with the
  settings of configESXu
  @return error state (C_NO_ERR == o.k.)
    or DATA_CHANGED on new values -> need call of wdReset to use new settings
  @see wdReset
*/
int16_t configWatchdog()
{
  tWDConfig t_watchdogConf = {
      WD_MAX_TIME,
      UD_MIN,
      UD_MAX
  };

  int16_t retval = config_wd( &t_watchdogConf );

#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "config_wd( &t_watchdogConf( "
//<< t_watchdogConf.wWDTime_ms << ", "
//<< t_watchdogConf.wUEmin_mV << ", "
//<< t_watchdogConf.wUEmax_mV
//<< " ) ) returns " << retval << "\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
uint8_t buf[128];
sprintf( (char*)buf, "%u ms - config_wd( &t_watchdogConf( %u, %u, %u ) returns %i\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)t_watchdogConf.wWDTime_ms
, (uint16_t)t_watchdogConf.wUEmin_mV
, (uint16_t)t_watchdogConf.wUEmax_mV
, (int16_t) retval
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif

	return retval;
}

} // end namespace __HAL
