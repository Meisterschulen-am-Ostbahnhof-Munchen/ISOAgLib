/***************************************************************************
                          system_target_extensions.cpp - source for ESX specific
                                                 extensions for the HAL
                                                 for central system
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

/* ********************************************************** */
/** \file IsoAgLib/hal/mitron167/system/SystemTargetExtensions.cc
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
  return ( sui32_detectStart == 0x9876 )?true:false;
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
