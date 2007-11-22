/***************************************************************************
                          system_target_extensions.cpp - source for A2 specific
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
/** \file IsoAgLib/hal/a2/system/SystemTargetExtensions.cc
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
#if 0 //BW, TMP, 4/18/06
  /** include the BIOS specific header with the part for CAN into __HAL */
  #include <commercial_BIOS/bios_a2/Xos20esx.h>

  #ifdef _INIT_BABYBOARD_
  #include <commercial_BIOS/bios_a2/Module/xma20.h>
  #endif
#endif //BW

/** task function which is called each 50msec to trigger the BIOS WD
    if the App has called wdTriggern(void) max cui16_maxTaskTriggeredWdResets
		rounds ago
*/
void taskTriggerWatchdog();
}

//BW, TMP, 4/18/06

extern "C"
{
 #include "cap_interface.h"
 }

/*
//copy from D3, remove this later
static sSET_PALETTE 	pal;
void ToPaletteClear()	   		//FOND VERT CLAIR
{
	u8 i;
	
	u8 palette_vga[48] =
	{
 		13,13,13, 		//grey MODIFIED = gris moyen
		0,0,12, 	   	//blue MODIFIED = bleu moins flash
		2,12,1, 	   	//lime MODIFIED = vert moins flash
		15,8,0, 		//cyan MODIFED = ORANGE	 AGCO
		15,0,0, 	   	//red
		14,15,14, 		//magenta MODIFIED = vert clair
		15,14,0, 		//yellow modified = Challenger
		15,15,15, 		//white
		0,0,0, 			//black
		11,13,15,		//navy MODIFIED = bleu pale
		2,6,6, 			//green MODIFED = sulky burel
		0,7,7, 			//teal
		7,0,0, 			//marron 
		14,14,14, 		//purple MODIFIED = gris moyen-1
		12,12,12, 		//olive modifed = gris moyen +1 
		10,10,10		//silver modified = gris moyen+2
	}; 
	
	for (i=0;i<48;i++)
		pal.palette[i] = palette_vga[i];

	setPalette(&pal);
}

*/	  

//Copied from ./hal/pc/system_target_extension.cpp
int get_snr(byte *snr_dat)   //int16_t getSnr(uint8_t *snrDat)
{
  snr_dat[0] = 0x99;
  snr_dat[1] = 0x12;
  snr_dat[2] = 0x34;
  snr_dat[3] = 0x56;
  snr_dat[4] = 0x00;
  snr_dat[5] = 0x01;

  return 0;//HAL_NO_ERR;
}

//BW, 4/17/06
//The following functions are related to WatchDog, which is no practical use in IsoAgLib
int  init_task_call (word wHandle,word wIntervall,word wOffset,void(*pfFunktion)(void)){return 0;}
void start_task_timer (word wT5_reload){return;}
int  config_wd(tWDConfig *tConfigArray){return 0;} /* configuration of the system supervisor*/
void wd_triggern(void){return;}                  /* watchdog triggering*/
int  wd_reset(void){return 0;}                    /* to force a reset*/


//BW, 4/17/06, clean up typedef later 
int32_t get_time(void) //return time in milliseconds 
{
	sTIMER timer;
	getTimerTicks(&timer);
//	return timer.ticks*4; //According to Datatronics 3 code, 1 tick=4 ms
// Brian Wei, 5/18/06 BIOS, timer ticks changed 1 tick= 8ms
	return timer.ticks*8;
 }

//BW, 4/17/06
//For A2, no sleep function available, wait endlessly here
//Clean up type definition later, change int, long, word to i32_t, i16_t, etc
//NOTE: delay long time in A2 may cause other application to starve
void delay_us(word wTimer) /* delay micro secounds*/
{
   long ci32_endWait;
    ci32_endWait = get_time()+(wTimer/1000);//A2 can reach millisecond
   while (get_time()<ci32_endWait);
}


//BW, 4/17/06
//Related to close_system, shall never be invoked. Just run empty operation for now.
    void staying_alive(void) 
    { 
   // printf("staying alive \n"); 
    return;
    }  // to activate the power selfholding
    void power_down(void)
    { 
    //printf("System Stop aufgerufen\n"); 
    return;
    } // to deactivate the power selfholding
	//BW, 4/20/06, get_on_off_switch must return true, otherwise system can not run
	int  get_on_off_switch(void){return true;}            // the evaluation of the on/off-switch (D+)
    void set_relais(boolean bitState){return;}       // switch relais on or off, relais=relays

/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system()
{
//Brian Wei Latest of bios (since 06_05_18bios) cap_init is no longer available
// 	cap_init();
    return 0;
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
  return C_NO_ERR;
}

/** check if open_System() has already been called */
bool isSystemOpened( void )
{
return true;
#if 0
  if ( ( t_biosextSysdata.bCPU_freq != 0 )
    && ( t_biosextSysdata.wRAMSize != 0 )
    && ( t_biosextSysdata.wROMSize != 0 )
    && ( t_biosextSysdata.bEESize != 0 ) ) return true;
  else return false;
#endif
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

//BW, 4/18/06, commented off based on TaskingController VC7 project
//	if ( ! sb_isWdTriggerTaskRunning )
//	{
//		init_task_call( TASKLEVEL_2A, 5, 5, taskTriggerWatchdog );
//		start_task_timer( 20 );
//		sb_isWdTriggerTaskRunning = true;
//	}

  return config_wd(&t_watchdogConf);
}

} // end namespace __HAL
