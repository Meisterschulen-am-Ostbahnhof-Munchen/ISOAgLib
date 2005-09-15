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
/** \file IsoAgLib/hal/esx/system/SystemTargetExtensions.cc
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
  #include <commercial_BIOS/bios_esx/Xos20esx.h>

  #ifdef _INIT_BABYBOARD_
  #include <commercial_BIOS/bios_esx/Module/xma20.h>
  #endif

/** task function which is called each 50msec to trigger the BIOS WD
    if the App has called wdTriggern(void) max cui16_maxTaskTriggeredWdResets
		rounds ago
*/
void taskTriggerWatchdog();
}
/** initialise static tSystem to complete zero, so that a call of
  * open_system can be reliable detected
  */
static tSystem t_biosextSysdata =
  {0,0,0,0,0,0,0,
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  0,0,0,0,0};


/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system()
{
  int16_t i16_result = open_esx(&t_biosextSysdata);
	

  #ifdef _INIT_BABYBOARD_
   /************initialization of Babyboard at Position 1*********/
  BA_init(POSITION_1, VARIANT_MIX );
  BA_set_pwm_freq(POSITION_1, PIN_1, 500);
  BA_set_digout (POSITION_1, PIN_1, 100);
  BA_set_pwm_freq(POSITION_1, PIN_2, 500);
  BA_set_digout (POSITION_1, PIN_2, 100);
  BA_set_pwm_freq(POSITION_1, PIN_3, 500);
  BA_set_digout (POSITION_1, PIN_3, 100);
  BA_set_pwm_freq(POSITION_1, PIN_4, 500);
  BA_set_digout (POSITION_1, PIN_4, 100);
  #endif

  return i16_result;
}
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void )
{ // if CAN_EN ist active -> shut peripherals off and stay in idle loop
  if ( get_on_off_switch() )
  { // CanEn still active
    set_relais( OFF );
    power_down();
  }
  // trigger Watchdog, till CanEn is off
  while ( get_on_off_switch() ) wd_triggern();
  // close ESX as soon as
  close_esx();
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

/** max amount of task triggered WD resets */
static const uint16_t cui16_maxTaskTriggeredWdResets = 40;

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
  #if 0
  tWDConfig t_watchdogConf = {
      WD_MAX_TIME,
      WD_MIN_TIME,
      UD_MAX,
      UD_MIN,
      0, // CONFIG_RELAIS,
      0 // CONFIG_RESET
  };
  #else
  tWDConfig t_watchdogConf = {
      WD_MAX_TIME,
      WD_MIN_TIME,
      UD_MAX,
      UD_MIN,
      CONFIG_RELAIS,
      CONFIG_RESET
  };
  #endif

	if ( ! sb_isWdTriggerTaskRunning )
	{
		init_task_call( TASKLEVEL_2A, 5, 5, taskTriggerWatchdog );
		start_task_timer( 20 );
		sb_isWdTriggerTaskRunning = true;
	}

  return config_wd(&t_watchdogConf);
}

} // end namespace __HAL
