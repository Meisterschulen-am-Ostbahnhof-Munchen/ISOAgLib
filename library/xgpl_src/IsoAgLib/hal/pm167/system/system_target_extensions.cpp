/*
  system_target_extensions.cpp: source for PM167 specific extensions
    for the HAL for central system

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file hal/pm167/system/system_target_extensions.cpp
 * A module targetExtensions should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
********************************************************** */

#include "system_target_extensions.h"
#include "../config.h"


namespace __HAL {
extern "C" {
  /** include the BIOS specific header with the part for CAN into __HAL */
  #include <commercial_BIOS/bios_pm167/mios1.h>
}

static tSystem t_biosextSysdata;

/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system()
{
  return k_open_pm167(&t_biosextSysdata);
}
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void )
{ // if CAN_EN ist active -> shut peripherals off and stay in idle loop
  #if defined(NO_CAN_EN_CHECK)
  if ( get_on_off_switch() )
  #endif
  { // CanEn still active
    set_relais( OFF );
    power_down();
  }
  #if defined(NO_CAN_EN_CHECK)
  // trigger Watchdog, till CanEn is off
  while ( get_on_off_switch() ) wd_triggern();
  // close ESX as soon as
  close_esx();
  #else
  while ( true ) wd_triggern();
  #endif
  return C_NO_ERR;
}

/**
  configure the watchdog of the system with the
  settings of configImi
  @return error state (C_NO_ERR == o.k.)
    or DATA_CHANGED on new values -> need call of wdReset to use new settings
  @see wdReset
*/
int16_t configWatchdog()
{
  tWDConfig t_watchdogConf = {
      WD_MAX_TIME,
      WD_MIN_TIME,
      UD_MAX,
      UD_MIN,
      CONFIG_RELAIS,
      CONFIG_RESET
  };

  return config_wd(&t_watchdogConf);
}

} // end namespace __HAL
