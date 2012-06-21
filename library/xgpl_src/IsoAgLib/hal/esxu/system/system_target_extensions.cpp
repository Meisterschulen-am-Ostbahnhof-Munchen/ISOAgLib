/*
  targetExtensions.cc: source for ESXu specific extensions for the HAL
    for central system

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

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

void openSystem()
{
  tSystem t_biosextSysdata;
  switch( open_micro( &t_biosextSysdata ) )
  {
    case C_NO_ERR:
      system_is_opened = true;
      break;
    default:
      abort();
  }

}

void closeSystem()
{
  if( C_NO_ERR == close_micro() )
    abort();

  // close micro performs a CPU reset - so
  // the following is just for convenience
  system_is_opened = false;
}

bool isSystemOpened( void )
{
  return system_is_opened;
}

void configWatchdog()
{
  tWDConfig t_watchdogConf = {
      WD_MAX_TIME,
      UD_MIN,
      UD_MAX
  };

  const int16_t retval = config_wd( &t_watchdogConf );

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

  if( retval != C_NO_ERR ) {
    abort();
  }
}

} // end namespace __HAL
