/*
  targetExtensions.cc: source for C2C specific extensions for the HAL
    for central system

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file c2c/system/system_target_extensions.cpp
 * A module targetExtensions should be used
 * for all methods, which can't be simply 
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
 * ********************************************************** */

#include "system_target_extensions.h"
#include "../config.h"


namespace __HAL {
extern "C" {
  /** include the BIOS specific header with the part for CAN into __HAL */
  #include <commercial_BIOS/bios_c2c/c2c10osy.h>
  }
/** initialise static tSystem to complete zero, so that a call of
  * open_system can be reliable detected
  */
static tSystem t_biosextSysdata =
  {0,0,0,0,0,0,0,0,0,0,
    {0,0,0,0,0,0,0,0},
  0,
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  0,0};


int16_t open_system()
{
  const int16_t i16_result = open_c2c(&t_biosextSysdata);

  switch( i16_result )
  {
    case C_NOACT:
    case C_BUSY:
    case C_CHECKSUM:
      return i16_result;
    case C_DEFAULT:
    case C_NO_ERR:
      return HAL_NO_ERR;
  }

  return HAL_UNKNOWN_ERR;
}

// MSCHMIDT - I think there is a bug here...  
// I think it should read:
//    if( !get_on_off_switch() )
// Same is true for hal\esx\system\system_target_extensions.cc
//
int16_t closeSystem( void )
{ // if CAN_EN ist active -> shut peripherals off and stay in idle loop
  if ( get_on_off_switch() )
  { // CanEn still active
    power_down();
  }
  // trigger Watchdog, till CanEn is off
  while ( get_on_off_switch() ) trigger_wd();
  // power off
  power_down();
  return C_NO_ERR;
}

/** check if open_System() has already been called */
bool isSystemOpened( void )
{
  if ( ( t_biosextSysdata.bCPU_freq != 0 )
    && ( t_biosextSysdata.wRAMSize != 0 )
    && ( t_biosextSysdata.wROMSize != 0 )
    && ( t_biosextSysdata.bEESize != 0 ) ) return true;
  else return false;
}

/**
  configure the watchdog of the system with the
  settings of configC2C
  @return error state (C_NO_ERR == o.k.)
    or DATA_CHANGED on new values -> need call of wdReset to use new settings
  @see wdReset
*/
int16_t configWatchdog()
{
  byte bTime = WD_MAX_TIME;

  return config_wd(bTime);
}

} // end namespace __HAL
