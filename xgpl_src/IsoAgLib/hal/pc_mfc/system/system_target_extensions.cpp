/***************************************************************************
                          system_target_extensions.cpp - source for PC specific
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
/** \file IsoAgLib/hal/pc/system/SystemTargetExtensions.cc
 * A module targetExtensions should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
 * ********************************************************** */

#include "system_target_extensions.h"
#include <IsoAgLib/hal/pc_mfc/can/can.h>
#include <IsoAgLib/hal/pc_mfc/can/can_target_extensions.h>
#include <IsoAgLib/util/config.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef PC_OS_Linux
#include <conio.h>
#else
  #include <unistd.h>
#endif
#include <fcntl.h>
#include <iostream>

#include <windows.h>
#include <MMSYSTEM.H>

namespace __HAL {
static tSystem t_biosextSysdata = { 0,0,0,0,0,0};
static bool onOffSwitch = false;
static bool bRelais = false;

/**
  open the system with system specific function call
  @return error state (HAL_NO_ERR == o.k.)
*/
int16_t open_system()
{ // init system start time
  getTime();
  t_biosextSysdata.wRAMSize = 1000;
  return can_startDriver();
}
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void )
{ // remember if are are already talked
  static bool sb_firstCall = true;
  if ( !sb_firstCall ) return HAL_NO_ERR;
  sb_firstCall = false;
  // if CAN_EN ist active -> shut peripherals off and stay in idle loop
  #if defined(NO_CAN_EN_CHECK)
  if ( getOn_offSwitch() )
  #endif
  { // CanEn still active
    setRelais( OFF );
    powerDown();
  }
  #if defined(NO_CAN_EN_CHECK)
  // trigger Watchdog, till CanEn is off
  // while ( getOn_offSwitch() ) wdTriggern();
  // close ESX as soon as
  setRelais( OFF );
  powerDown();
  #else
  // while ( true ) wdTriggern();
  #endif
  return HAL_NO_ERR;
}
/** check if open_System() has already been called */
bool isSystemOpened( void )
{
  if ( t_biosextSysdata.wRAMSize != 0 ) return true;
  else return false;
}

/**
  configure the watchdog of the system with the
  settings of configEsx
  @return error state (HAL_NO_ERR == o.k.)
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

  return configWd(&t_watchdogConf);
}

  // VC++ with native Win32 API provides very accurate
  // msec timer - use that
  int32_t getTime()
  { // returns time in msec
    return timeGetTime();
  }

/* serial number of esx */
int16_t getSnr(uint8_t *snrDat)
{
	static uint8_t serNo[] = { 0x05, 0x01, 0x66, 0x74, 0x20, 0x19 };
	memmove((void*)snrDat, (void*)serNo,6);
	return HAL_NO_ERR;
}

/* configuration of the system supervisor*/
int16_t  configWd(tWDConfig *tConfigArray)
{
  printf("configWd aufgerufen mit MaxTime %hu, MinTime %hu, UDmax %hu, UDmin %hd\n",
    tConfigArray->bWDmaxTime, tConfigArray->bWDminTime, tConfigArray->bUDmax, tConfigArray->bUDmin);
  return 0;
}
void wdTriggern(void)
{
  //printf("<WD>");
}
int16_t wdReset(void)
{
  printf("WD reset\n");
  return 1;
}
void startTaskTimer ( void )
{
  printf("startTaskTimer mit %d aufgerufen\n", T_TASK_BASIC );
}
/* get the cpu frequency*/
int16_t  getCpuFreq(void)
{
  printf("getCpuFreq aufgerufen\n");
  return 20;
}

/* to activate the power selfholding*/
void stayingAlive(void)
{
  printf("staying alive aktiviert\n");
}

/* to deactivate the power selfholding*/
void powerDown(void)
{
  if ( getOn_offSwitch() == 0 )
  { // CAN_EN is OFF -> stop now system
    can_stopDriver();
  }
}

/* the evaluation of the on/off-switch (D+)*/
int16_t  getOn_offSwitch(void)
{
	return onOffSwitch ? 1 : 0;
}
void setOn_offSwitch(bool flag)
{
	onOffSwitch = flag;
}

/* switch relais on or off*/
void setRelais(boolean bitState)
{
	bRelais = bitState ? true : false;
}

int16_t KeyGetByte(uint8_t *p)
{
    if (!_kbhit()) return 0;
    *p = _getch();
    return 1;
}

void delay_us(__HAL::word /*wTimer */)
{
}

} // end namespace __HAL
