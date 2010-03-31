/*
  system_target_extensions.cpp: source for PC specific extensions for
    the HAL for central system

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file hal/pc/system/system_target_extensions.cpp
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
#include <iostream>

#ifdef WIN32
  #ifndef WINCE
    #include <conio.h>
  #endif
  #if defined( _MSC_VER )
    #include <windows.h>
    #include <MMSYSTEM.H>
  #else
    #include <time.h>
  #endif
#else
  #include <fcntl.h>
  #include <sys/time.h>
  #include <sys/times.h>
  #include <unistd.h>
  #include <linux/version.h>
#endif

#if DEBUG_HAL
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

#ifndef LINUX_VERSION_CODE
#error "LINUX_VERSION_CODE is not defined"
#endif

int32_t getStartupTime()
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
  if ( msecPerClock != (1000 / sysconf(_SC_CLK_TCK)) )
  { // BIG PROBLEM -> THE DEFINE DURING COMPILE TIME DOES NOT MATCH THE RUNTIME
    INTERNAL_DEBUG_DEVICE << "\n\nVERY BIG PROBLEM!!!\nThis program was compiled with\n#define msecPerClock " << msecPerClock
        << "\nwhile the runtime system has " << (1000 / sysconf(_SC_CLK_TCK))
        << "\n\nSO PLEASE add\n#define msecPerClock " << (1000 / sysconf(_SC_CLK_TCK))
        << "\nto your project configuration header or Makefile, so that a matching binary is built. This program is aborted now, as none of any time calculations will match with this problem.\n\n"
        << INTERNAL_DEBUG_DEVICE_ENDL;
    MACRO_ISOAGLIB_ABORT();
  }
  static int32_t st_startup4Times = int32_t (times(NULL));
#else
  static int32_t st_startup4Times = int32_t (-1);
  if (st_startup4Times < 0)
  {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    st_startup4Times = int32_t(ts.tv_sec)*1000 + int32_t(ts.tv_nsec/1000000);
  }
#endif
  return st_startup4Times;
}
#else // WIN32
  #if defined( _MSC_VER )
  // VC++ with native Win32 API provides very accurate
  // msec timer - use that
  int32_t getStartupTime()
  { // returns time in msec
    static int32_t st_startup4Times = timeGetTime();
    return st_startup4Times;
  }
  #endif
  // MinGW doesn't need getStartupTime() AS IT SEEMS RIGHT NOW.
  // Building MinGW executables is currently not supported.
#endif


/**
  open the system with system specific function call
  @return error state (HAL_NO_ERR == o.k.)
*/
int16_t open_system()
{ // init system start time
  getTime();

  t_biosextSysdata.wRAMSize = 1000;
  DEBUG_PRINT("DEBUG: open_system called.\n");
  DEBUG_PRINT("DEBUG: PRESS RETURN TO STOP PROGRAM!!!\n\n");
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
    return timeGetTime() - getStartupTime();
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
  // Building MinGW executables is currently not supported.
  #endif
#else


 // use gettimeofday for native LINUX system
int32_t getTime()
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
  // sysconf(_SC_CLK_TCK) provides clock_t ticks per second
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
#else
  /** linux-2.6 */
  static timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  const int32_t ci_now = int32_t(ts.tv_sec)*1000 + int32_t(ts.tv_nsec/1000000);
  return ci_now - getStartupTime();
#endif
}
#endif


/* serial number of esx */
int16_t getSnr(uint8_t *snrDat)
{
  snrDat[0] = 0x99;
  snrDat[1] = 0x12;
  snrDat[2] = 0x34;
  snrDat[3] = 0x56;
  snrDat[4] = 0x00;
  snrDat[5] = 0x01;

  return HAL_NO_ERR;
}

/* configuration of the system supervisor*/
int16_t  configWd(tWDConfig *tConfigArray)
{
  DEBUG_PRINT4("DEBUG: configWd called with MaxTime %hu, MinTime %hu, UDmax %hu, UDmin %hd\n",
    tConfigArray->bWDmaxTime, tConfigArray->bWDminTime, tConfigArray->bUDmax, tConfigArray->bUDmin);
  tConfigArray = tConfigArray; // Just suppress warning.
  return 0;
}
void wdTriggern(void)
{
  //DEBUG_PRINT("<WD>");
}
int16_t wdReset(void)
{
  DEBUG_PRINT("DEBUG: WD reset\n");
  return 1;
}
void startTaskTimer ( void )
{
  DEBUG_PRINT1("DEBUG: startTaskTimer with %d called\n", T_TASK_BASIC );
}
/* get the cpu frequency*/
int16_t  getCpuFreq(void)
{
  DEBUG_PRINT("DEBUG: getCpuFreq called\n");
  return 20;
}

/* to activate the power selfholding*/
void stayingAlive(void)
{
  DEBUG_PRINT("DEBUG: staying alive aktiviert\n");
}

/* to deactivate the power selfholding*/
void powerDown(void)
{
  if ( getOn_offSwitch() == 0 )
  { // CAN_EN is OFF -> stop now system
    can_stopDriver();
    DEBUG_PRINT("DEBUG: Power Down called.\n");
  }
}

#if defined(USE_SENSOR_I)
typedef void (*_counterIrqFunction)(...);
static _counterIrqFunction _irqFuncArr[16] = {NULL, NULL, NULL, NULL, NULL, NULL,
 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
int32_t i32_lastTime[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#endif

/* the evaluation of the on/off-switch (D+)*/
int16_t  getOn_offSwitch(void)
{
#if defined(USE_SENSOR_I)
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
    if ( STL_NAMESPACE::cin.rdbuf()->in_avail() > 0 ) return 0;
    else return 1;
  #endif
}

/* switch relais on or off*/
void setRelais(boolean bitState)
{
  DEBUG_PRINT1("DEBUG: setRelais(%d) called\n", bitState);
  bitState = bitState; // Just suppress warning
}

int16_t KeyGetByte(uint8_t *p)
{
  #ifndef WIN32
    // fcntl( 0,
    return read(0, p, sizeof *p) == sizeof *p;
  #else
    #ifdef WINCE
      return 0;  /// @todo WINCE-176 add key handling for WINCE
    #else
      if (!_kbhit()) return 0;
      *p = _getch();
    #endif
    return 1;
  #endif
}

} // end namespace __HAL
