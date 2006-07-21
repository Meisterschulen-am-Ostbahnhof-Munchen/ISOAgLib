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
#include <IsoAgLib/hal/pc/can/can.h>
#include <IsoAgLib/hal/pc/can/can_target_extensions.h>
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

#ifdef WIN32
  #if defined( _MSC_VER )
    #include <windows.h>
    #include <MMSYSTEM.H>
  #else
    #include <time.h>
#endif
#else
  #include <sys/time.h>
  #include <sys/times.h>
  #include <unistd.h>
#endif

#ifdef DEBUG
#define DEBUG_PRINT(str) printf(str); fflush(0)
#define DEBUG_PRINT1(str,a) printf(str,a); fflush(0)
#define DEBUG_PRINT2(str,a,b) printf(str,a,b); fflush(0)
#define DEBUG_PRINT3(str,a,b,c) printf(str,a,b,c); fflush(0)
#define DEBUG_PRINT4(str,a,b,c,d) printf(str,a,b,c,d); fflush(0)
#else
#define DEBUG_PRINT(str)
#define DEBUG_PRINT1(str,a)
#define DEBUG_PRINT2(str,a,b)
#define DEBUG_PRINT3(str,a,b,c)
#define DEBUG_PRINT4(str,a,b,c,d)
#endif


namespace __HAL {
static tSystem t_biosextSysdata = { 0,0,0,0,0,0};

#ifndef WIN32
/** define the amount of MSec per Clock_t, in case the project config didn't this before */
#ifndef msecPerClock
#define msecPerClock 10LL
#endif

static clock_t st_startup4Times = 0;
clock_t getStartUpTime()
{
  if ( msecPerClock != (1000 / sysconf(_SC_CLK_TCK)) )
  { // BIG PROBLEM -> THE DEFINE DURING COMPILE TIME DOES NOT MATCH THE RUNTIME
    std::cerr << "\n\nVERY BIG PROBLEM!!!\nThis program was compiled with\n#define msecPerClock " << msecPerClock
        << "\nwhile the runtime system has " << (1000 / sysconf(_SC_CLK_TCK))
        << "\n\nSO PLEASE add\n#define msecPerClock " << (1000 / sysconf(_SC_CLK_TCK))
        << "\nto your project configuration header or Makefile, so that a matching binary is built. This program is aborted now, as none of any time calculations will match with this problem.\n\n"
        << std::endl;
    abort();
  }
  st_startup4Times = times(NULL);
  return st_startup4Times;
}
#endif


/**
  open the system with system specific function call
  @return error state (HAL_NO_ERR == o.k.)
*/
int16_t open_system()
{ // init system start time
  #ifdef WIN32
  getTime();
  #else
  getStartUpTime();
  #endif
  t_biosextSysdata.wRAMSize = 1000;
  DEBUG_PRINT("open_esx aufgerufen\n");
  DEBUG_PRINT("\n\nPRESS RETURN TO STOP PROGRAM!!!\n\n");
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

#ifdef WIN32
  #if defined( _MSC_VER )
  // VC++ with native Win32 API provides very accurate
  // msec timer - use that
  int32_t getTime()
  { // returns time in msec
    return timeGetTime();
  }
  #else
  // MinGW has neither simple access to timeGetTime()
  // nor to gettimeofday()
  // - but beware: at least in LINUX clock() measures
  //   only the times of the EXE in CPU core
  int32_t getTime()
  { // returns time in msec
    return (clock()/(CLOCKS_PER_SEC/1000));
  }
  #endif
#else


 // use gettimeofday for native LINUX system
int32_t getTime()
{ // sysconf(_SC_CLK_TCK) provides clock_t ticks per second
  //  static const int64_t ci64_mesecPerClock = 1000 / sysconf(_SC_CLK_TCK);
  static struct timeval now;
  gettimeofday(&now, 0);
  // fetch RAW - non normalized - time in scaling of gettimeofday()
  int64_t i64_time4Timeofday = int64_t(now.tv_sec)*1000LL + int64_t(now.tv_usec/1000);


  // store offset between gettimeofday() and system start
  static int64_t si64_systemStart4Timeofday = i64_time4Timeofday;
  // static store delta between times() normalization and gettimeofday() norm
  static int64_t si64_deltaStartTimes = i64_time4Timeofday - int64_t(getStartUpTime()) * msecPerClock;

  // derive change of the normalization delta
  const int64_t ci64_deltaChange = i64_time4Timeofday - int64_t(times(NULL)) * msecPerClock - si64_deltaStartTimes;
  if ( ( ci64_deltaChange >= 1000 ) || ( ci64_deltaChange <= -1000 ) )
  { // user changed the system clock inbetween
    si64_deltaStartTimes += ci64_deltaChange;
    si64_systemStart4Timeofday += ci64_deltaChange;
  }

  // now calculate the real time in [msec] since startup
  i64_time4Timeofday -= si64_systemStart4Timeofday;
  // now derive the well define overflows
  while ( i64_time4Timeofday > 0x7FFFFFFFLL )
  {
    i64_time4Timeofday         -= 0xFFFFFFFF;
    si64_systemStart4Timeofday += 0xFFFFFFFF;
    si64_deltaStartTimes       += 0xFFFFFFFF;
  }

  return i64_time4Timeofday;
}
#endif


/* serial number of esx */
int16_t getSnr(uint8_t *snrDat)
{
  memcpy(snrDat, "serienr",6);
  return HAL_NO_ERR;
}

/* configuration of the system supervisor*/
int16_t  configWd(tWDConfig *tConfigArray)
{
  DEBUG_PRINT4("configWd aufgerufen mit MaxTime %hu, MinTime %hu, UDmax %hu, UDmin %hd\n",
    tConfigArray->bWDmaxTime, tConfigArray->bWDminTime, tConfigArray->bUDmax, tConfigArray->bUDmin);
  return 0;
}
void wdTriggern(void)
{
  //DEBUG_PRINT("<WD>");
}
int16_t wdReset(void)
{
  DEBUG_PRINT("WD reset\n");
  return 1;
}
void startTaskTimer ( void )
{
  DEBUG_PRINT1("startTaskTimer mit %d aufgerufen\n", T_TASK_BASIC );
}
/* get the cpu frequency*/
int16_t  getCpuFreq(void)
{
  DEBUG_PRINT("getCpuFreq aufgerufen\n");
  return 20;
}

/* to activate the power selfholding*/
void stayingAlive(void)
{
  DEBUG_PRINT("staying alive aktiviert\n");
}

/* to deactivate the power selfholding*/
void powerDown(void)
{
  if ( getOn_offSwitch() == 0 )
  { // CAN_EN is OFF -> stop now system
    can_stopDriver();
    DEBUG_PRINT("System Stop aufgerufen\n");
  }
}

#if !defined(SYSTEM_PC_VC) && defined(USE_SENSOR_I)
typedef void (*_counterIrqFunction)(...);
static _counterIrqFunction _irqFuncArr[16] = {NULL, NULL, NULL, NULL, NULL, NULL,
 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
int32_t i32_lastTime[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#endif

/* the evaluation of the on/off-switch (D+)*/
int16_t  getOn_offSwitch(void)
{
#if !defined(SYSTEM_PC_VC) && defined(USE_SENSOR_I)
  // simulate digital RPM input
  uint8_t ui8_ind;
  int32_t i32_time = getTime();
  double dRand = ((double)rand())/(double)RAND_MAX;
  dRand *= 80000.0F;
  for (ui8_ind = 0; ui8_ind < 16; ui8_ind++)
  {
    if (_irqFuncArr[ui8_ind] != NULL)
    {
      if (((i32_time - i32_lastTime[ui8_ind])/100) >= (2 + dRand))
      {
        i32_lastTime[ui8_ind] = i32_time;
        (_irqFuncArr[ui8_ind])();
      }
    }
  }
#endif
  #if 0
    // old style execution stop detection when
    // application shoul stop if all CAN messages of
    // FILE based CAN simulation were processed
    return (getTime() - can_lastReceiveTime() < 1000)?1:0;
  #elif 0
    uint8_t b_temp;
    // exit function if key typed
    if (KeyGetByte(&b_temp) ==1) return 0;
    else return 1;
  #else
    // use std C++ cin function to check for unprocessed input
    // -> as soon as RETURN is hit, the programm stops
    if ( std::cin.rdbuf()->in_avail() > 0 ) return 0;
    else return 1;
  #endif
}

/* switch relais on or off*/
void setRelais(boolean bitState)
{
  DEBUG_PRINT1("setRelais(%d) aufgerufen", bitState);
}

int16_t KeyGetByte(uint8_t *p)
{
  #ifdef PC_OS_Linux
    // fcntl( 0,
    return read(0, p, sizeof *p) == sizeof *p;
  #else
    if (!_kbhit()) return 0;
    *p = _getch();
    return 1;
  #endif
}

} // end namespace __HAL
