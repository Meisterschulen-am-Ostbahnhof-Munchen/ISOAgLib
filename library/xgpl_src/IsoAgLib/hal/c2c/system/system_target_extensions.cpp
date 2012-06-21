/*
  targetExtensions.cc: source for C2C specific extensions for the HAL
    for central system

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

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

#include "system.h"

namespace __HAL {

static bool system_is_opened = false;

int16_t open_system()
{
  tSystem t_biosextSysdata;
  const int16_t i16_result = open_c2c( &t_biosextSysdata );

  switch( i16_result )
  {
    case C_NOACT:
    case C_BUSY:
    case C_CHECKSUM:
    case C_RANGE:
    case C_RD_WR:
      return i16_result;
    case C_DEFAULT:
    case C_NO_ERR:
      system_is_opened = true;
      return HAL_NO_ERR;
  }

  return HAL_UNKNOWN_ERR;
}

int16_t closeSystem( void )
{
  system_is_opened = false;
  return HAL_NO_ERR;
}

/** check if open_System() has already been called */
bool isSystemOpened( void )
{
  return system_is_opened;
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
