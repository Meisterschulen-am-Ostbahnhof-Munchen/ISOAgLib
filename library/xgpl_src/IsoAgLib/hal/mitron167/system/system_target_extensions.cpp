/*
  system_target_extensions.cpp: source for ESX specific extensions for
    the HAL for central system

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file mitron167/system/system_target_extensions.cpp
 * A module targetExtensions should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
 * ********************************************************** */

#include "system_target_extensions.h"
#include "../config.h"
#include <cstring>
#include <ctime>
#include <cstdlib>


namespace __HAL {
extern "C" {
  /** include the BIOS specific header with the part for CAN into __HAL */
  #include <C166.h>
  #include <reg167cs.h>
}

extern "C" STL_NAMESPACE::time_t _time( STL_NAMESPACE::time_t *pt );
static uint32_t sui32_detectStart = 0x1234;

/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system()
{
  STL_NAMESPACE::memcpy((void*)0x0000,(void*)0x90000,0x200); 	// vector table moved to start from 0h on RAM
  sui32_detectStart = 0x9876;
  //   globally enable interrupts
  IEN = 1;      // defined in reg167cs.h
  // init system time
  _time( 0 );

  return HAL_NO_ERR;
}
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void )
{ // simply exit
  return HAL_NO_ERR;
}

/** check if open_System() has already been called */
bool isSystemOpened( void )
{
  return ( sui32_detectStart == 0x9876 );
}
/**
  get the system time in [ms]
  @return [ms] since start of the system
*/
int32_t getTime(void) {
  return _time( 0 );
}
/** static var for WDTREL ( high byte of WDTCON ) == reload value for watchdog counter */
static uint16_t sui16_wdtrel = 0;
static uint16_t sui16_wdtin = 1;
/**
  configure the watchdog of the system with the
  settings of configEsx
  @return error state (C_NO_ERR == o.k.)
    or DATA_CHANGED on new values -> need call of wdReset to use new settings
  @see wdReset
*/
int16_t configWatchdog()
{
  // set sui8_wdt to get 419ms wait time
  // in combination with slow counting mode
  sui16_wdtrel = 0; // reload counter to 0 for count up
  sui16_wdtin = 1; // slow counting
  // write register setting for config
  //WDT = 1;
  WDTCON = ( ( WDTCON & 0xFE ) | ( sui16_wdtrel << 8 ) | ( sui16_wdtin & 0x1 ) );
  return HAL_NO_ERR;
}

/** trigger the watchdog */
void wdTriggern(void) {
  // if ( getTime() > 5000 ) return;
  // rewrite watchdog setting
  WDTCON = ( ( WDTCON & 0xFE ) | ( sui16_wdtrel << 8 ) | ( sui16_wdtin & 0x1 ) );
  // call special CPU specific function to trigger watchdog
  _srvwdt();
  // WDT = 1;
}


} // end namespace __HAL
