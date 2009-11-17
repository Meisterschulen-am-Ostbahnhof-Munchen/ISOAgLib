/***************************************************************************
                          system.h - definition of Hardware Abstraction
                                     Layer for central system functions
                                     for debug/simulation
                                     hecu - Holmer ECU
                             -------------------
    begin                : 04/20/09 11:50:15 AM CEST
    copyright            : (C) 2009 OSB-AG
    email                : m.wodok@osb-ag:de
 ***************************************************************************/

#ifndef _HAL_EES_SYSTEM_H_
#define _HAL_EES_SYSTEM_H_

#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"

#include <hal_timer.h>

#include <IsoAgLib/util/impl/util_funcs.h>

namespace HAL {

  static uint64_t startupTime;

  /**
    get the system time in [ms]
    @return [ms] since start of the system
  */
  inline int32_t getTime( void ) {
    return (( hal_n::timer_c::getUs() / 1000L ) - startupTime );
  };

  /**
    open the system with system specific function call
    @return error state (HAL_NO_ERR == o.k.)
  */
  inline int16_t open_system() {
    startupTime = hal_n::timer_c::getUs() / 1000L;
    return HAL_NO_ERR;
  };

  /**
    close the system with system specific function call
    @return error state (HAL_NO_ERR == o.k.)
  */

  inline int16_t  closeSystem() {
    return HAL_NO_ERR;
  };

  /** check if open_System() has already been called */
  inline bool isSystemOpened( void ) {
    return true;
  };

  /**
    configure the watchdog of the system with the
    settings of configEsx
    @return error state (HAL_NO_ERR == o.k.)
      or DATA_CHANGED on new values -> need call of wdReset to use new settings
    @see wdReset
  */
  inline int16_t configWatchdog() {
    return HAL_NO_ERR;
  };

  /**
    reset the watchdog to use new configured watchdog settings
    @see configWd
  */
  inline int16_t  wdReset( void ) {
    return HAL_NO_ERR;
  };

  /** trigger the watchdog */
  inline void wdTriggern( void ) { };

  inline int16_t getSnr( uint8_t* /* snrDat */ ) {
    return HAL_NO_ERR;
  };

  inline int32_t getSerialNr( int16_t /* pi16_errCode = NULL */ ) {
    return 0L;
  };

  /**
    start the Task Timer -> time between calls of Task Manager
  */
  inline void startTaskTimer( void ) { };

  /**
    check if D+/CAN_EN is active
    (if NO_CAN_EN_CHECK is defined this function return always return ON)
    @return ON(1) or OFF(0)
  */
  inline int16_t getOn_offSwitch( void ) {
    return 1;
  };

  /**
    get the main power voltage
    @return voltage of power [mV]
  */
  inline int16_t getAdcUbat( void ) {
    return HAL_NO_ERR;
  };
  /**
    get the voltage of the external reference 8.5Volt for work of external sensors
    @return voltage at external reference [mV]
  */
  inline int16_t getAdc_u85( void ) {
    return 0;
  };

  /**
    activate the power selfholding to perform system
    stop (f.e. store values) actions after loss of CAN_EN
  */
  inline void stayingAlive( void ) { };

  /**
    shut off the whole system (set power down)
  */
  inline void powerDown( void ) { };

  /**
    switch relais on or off
    @param bitState true -> Relais ON
  */
  inline void setRelais( bool /* bitState */ ) { };


//end HAL
}
#endif
