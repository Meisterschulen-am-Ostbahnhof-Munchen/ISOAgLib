/*
  system_target_extensions.h: header for PC specific extensions for
    the HAL for central system

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file hal/pc/system/system_target_extensions.h
 * A module targetExtensions should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in map_bios_namespace.h .
 * ********************************************************** */

#ifndef _HAL_PC_SYSTEM_TARGET_EXTENSIONS_H_
#define _HAL_PC_SYSTEM_TARGET_EXTENSIONS_H_

#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"
#if !defined( WIN32 ) && !defined( __IAR_SYSTEMS_ICC__ )
#include <sys/time.h>
#include <time.h>
#endif

namespace __HAL {
#define  GET_U_C               35        /* UC (Boardspannung)   */
#define  GET_U_EXT_8_5_V       15        /* U 8,5 V EXT */

  /** \name General BIOS Extenders */
  /*@{*/

  /**
    open the system with system specific function call
    @return error state (C_NO_ERR == o.k.)
  */
  int16_t open_system();
  /**
    close the system with system specific function call
    @return error state (C_NO_ERR == o.k.)
  */
  int16_t closeSystem( void );
  /** check if open_System() has already been called */
  bool isSystemOpened( void );

  /**
    configure the watchdog of the system with the
    settings of configEsx
    @return error state (C_NO_ERR == o.k.)
      or DATA_CHANGED on new values -> need call of wdReset to use new settings
    @see wdReset
  */
  int16_t configWatchdog();

  int32_t getTime();
  int32_t getStartupTime();
  int16_t getSnr( uint8_t *snrDat );             /* serial number of target */

  int16_t  configWd( tWDConfig *tConfigArray ); /* configuration of the system supervisor*/
  void wdTriggern( void );
  int16_t wdReset( void );


  void startTaskTimer( void );
  int16_t  getCpuFreq( void );               /* get the cpu frequency*/
  void stayingAlive( void );              /* to activate the power selfholding*/

  void powerDown( void );                 /* to deactivate the power selfholding*/
  int16_t  getOn_offSwitch( void );          /* the evaluation of the on/off-switch (D+)*/

  void setRelais( boolean bitState );     /* switch relais on or off*/
  int16_t KeyGetByte( uint8_t *p );

  /*@}*/

} //end namespace __HAL
#endif
