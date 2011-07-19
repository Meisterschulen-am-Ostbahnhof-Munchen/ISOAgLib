/*
  system_target_extensions.cpp: source for ESX specific extensions for
    the HAL for central system

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file esx/system/system_target_extensions.cpp
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
  const int16_t i16_result = open_esx(&t_biosextSysdata);


  #ifdef _INIT_BABYBOARD_
   /************initialization of Babyboard at Position 1*********/
  BA_init(POSITION_1, VARIANT_MIX );
  BA_set_pwm_freq(POSITION_1, PIN_1, 500);
  BA_set_pwm_freq(POSITION_1, PIN_2, 500);
  BA_set_pwm_freq(POSITION_1, PIN_3, 500);
  BA_set_pwm_freq(POSITION_1, PIN_4, 500);
  #endif

  switch( i16_result )
  {
    case C_NOACT:
    case C_BUSY:
    case C_CHECKSUM:
      return i16_result;
    case C_CONFIG:
    case C_NO_ERR:
      return HAL_NO_ERR;
  }

  return HAL_UNKNOWN_ERR;
}
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void )
{ // if CAN_EN ist active -> shut peripherals off and stay in idle loop
  if ( get_on_off_switch() )
  { // CanEn still active
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
  tWDConfig t_watchdogConf = {
      WD_MAX_TIME,
      WD_MIN_TIME,
      UD_MAX,
      UD_MIN,
      CONFIG_RELAIS,
      CONFIG_RESET
  };

   int iReturn = config_wd( &t_watchdogConf );
   if ((iReturn == DATA_CHANGED) || (iReturn == C_CHECKSUM)) 
   {  /* Daten wurden geaendert bzw. Checksummenfehler festgestellt -> RESET*/
	  iReturn = wd_reset();
   }

	if ( ! sb_isWdTriggerTaskRunning )
	{
		init_task_call( TASKLEVEL_2A, 5, 5, taskTriggerWatchdog );
		start_task_timer( 20 );
		sb_isWdTriggerTaskRunning = true;
	}


  return iReturn;
}

} // end namespace __HAL
