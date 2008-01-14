/***************************************************************************
                          system_target_extensions.cpp - source for IMI specific
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
/** \file imi/system/system_target_extensions.cpp
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
  #include <commercial_BIOS/bios_imi/adis10.h>
}

#define CAN_EN_IN       11 // CAN-EN als Sensor_c Eingang

static tSystem t_biosextSysdata =
  {0,0,0,0,0,0,0,
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  0,0,0,0,0};

/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system( void )
{
  return open_adis(&t_biosextSysdata);
}
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void )
{ // trigger Watchdog, till system is stopped (IMI detects no CanEn)
  while ( true ) wd_triggern();
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
  check if the D+ signal is active
  @return 1 (== ON) -> the CAN BUS is still active
*/
int16_t getOn_offSwitch(void)
{
  uint16_t ui16_canEn = get_analogin_mean(CAN_EN_IN);
  if ((ui16_canEn > 200) && (ui16_canEn < 500)) return 1;
  else return 0;
}



/** max amount of task triggered WD resets */
static const uint16_t cui16_maxTaskTriggeredWdResets = 400;

//TEST static const uint16_t cui16_maxTaskTriggeredWdResets = 400;
/** counter for task triggered WD events
    --> the application called function triggerWd()
		    resets this count to 0;
	  --> if the WD trigger task is activated it checks for this counter
		  ==> on < cui16_maxTaskTriggeredWdResets -> trigger BIOS wd
			==> else -> do nothing so that WD resets the system
*/
static uint16_t sui16_watchdogCounter = 0;

/** task function which is called each 50msec to trigger the BIOS WD
    if the App has called wdTriggern(void) max cui16_maxTaskTriggeredWdResets
		rounds ago
*/
void taskTriggerWatchdog()
{
	if ( sui16_watchdogCounter < cui16_maxTaskTriggeredWdResets )
	{ // fine - last WD call by app was not too long ago
		wd_triggern();
		sui16_watchdogCounter++;
	}
}

/** trigger the watchdog */
void wdTriggern(void)
{
	sui16_watchdogCounter = 0;
}

/** flag to control if WD-Task is started already */
static bool sb_isWdTriggerTaskRunning = false;

/**
  configure the watchdog of the system with the
  settings of configEsx
  @return error state (C_NO_ERR == o.k.)
    or DATA_CHANGED on new values -> need call of wdReset to use new settings
  @see wdReset
*/
int16_t configWatchdog()
{
  if ( ! sb_isWdTriggerTaskRunning )
	{
		init_task_call( TASKLEVEL_2A, 5, 5, taskTriggerWatchdog );
		start_task_timer( 20 );
		sb_isWdTriggerTaskRunning = true;
	}

#if defined(SYSTEM_IMI_2CAN)
  return config_wd(WD_TIME1000ms);
#else
  return C_NO_ERR;
#endif
}



} // end namespace __HAL
