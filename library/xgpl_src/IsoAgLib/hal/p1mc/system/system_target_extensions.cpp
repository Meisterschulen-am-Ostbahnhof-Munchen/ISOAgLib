/*
  system_target_extensions.cpp: source for P1MC specific extensions
    for the HAL for central system

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

 /** \file IsoAgLib/hal/p1mc/system/system_target_extensions.ccp
 * A module targetExtensions should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .*/
 /* ********************************************************** */

#include "../typedef.h"
#include "system_target_extensions.h"
#include "../config.h"

#include "datatypes.h"
#include "idapp.h"
#include "datacontainer.h"

extern "C"
{
  extern U32 os_gulGetTimeMs(void);
}

namespace __HAL {

/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system()
{
  // set repetition time for maintask to 1 ms
  OS.ExecTimeOut = 1;
  return C_NO_ERR;
}
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void )
{
  return C_NO_ERR;
}

/**
  configure the watchdog of the system
  @return error state (C_NO_ERR == o.k.)
    or DATA_CHANGED on new values -> need call of wdReset to use new settings
  @see wdReset
*/
int16_t configWatchdog()
{
  return 0;
}

int32_t getTime(void)
{
  return os_gulGetTimeMs();
}

int16_t  getSnr(uint8_t *snrDat)
{
  return -1;
}

void start_task_timer ( void )
{

}

void powerHold( bool )
{

}

int16_t __getAdcUbat()
{
  return -1;
}

int16_t getAdc_u85()
{
  return -1;
}

} // end namespace __HAL
