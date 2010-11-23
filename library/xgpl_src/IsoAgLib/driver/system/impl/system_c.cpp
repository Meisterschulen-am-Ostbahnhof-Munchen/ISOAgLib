/*
  system_c.cpp: central object for encapsulation of platform dependent
    elements (mostly BIOS)

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "system_c.h"

#if DEBUG_SYSTEM
	#ifdef SYSTEM_PC
		#include <iostream>
	#else
		#include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
	#endif
#endif

namespace __IsoAgLib {
/** C-style function, to get access to the unique System_c singleton instance */
System_c &getSystemInstance(uint8_t aui8_instance)
{
  MACRO_MULTITON_GET_INSTANCE_BODY(System_c, 1, aui8_instance);
}

/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  * the call of System_c::close() stimulates final shutdown of power
  */
void System_c::close( void )
{
  // stop ESX
  HAL::closeSystem();
}


/**
  Initialize the system hardware
  (uses BIOS function)

  possible errors:
      * Err_c::SystemOpen problem during start of system with BIOS call
      * Err_c::SystemWatchdog the System_c::init_wd call caused an error
      * Err_c::unspecified Bios calls for TaskTimer, Relais or StayAlive caused an error
  @return true -> everything without errors initialised
*/
bool System_c::init( bool ab_forceReinit, IsoAgLib::SystemPowerdownStrategy_t at_strategy ){
  bool b_result = true;
  static bool b_firstCall = true;

  // avoid call of open_system() if system is already started, and init is only called
  // for RE-Initialization
  #if 0
  if ( !HAL::isSystemOpened() )
  #else
  if ( b_firstCall )
  #endif
  { // open the system with the configured BIOS call - as not yet called
    const int16_t ci_err = HAL::open_system();

    if ( ci_err != HAL_NO_ERR)
    {
		#if DEBUG_SYSTEM
		INTERNAL_DEBUG_DEVICE
        << "Fehler bei Systemstart: " << ci_err << INTERNAL_DEBUG_DEVICE_ENDL;
		#endif
      getILibErrInstance().registerError( iLibErr_c::SystemOpen, iLibErr_c::HwSystem ); // something is still wrong
      b_result = false;
    }
  }
  if ( ab_forceReinit || b_firstCall )
  { // init and config the Watchdog
    if (!initWd())
    {  // init of watchdog without success
       b_result = false;
    }

    // start the task timer (also needed for CAN)
    HAL::startTaskTimer();
    // configure POWER HOLD after loss of CAN_EN
		setPowerdownStrategy( at_strategy );
		/** start Relais either for not defined CONFIG_DO_NOT_START_RELAIS_ON_STARTUP
				- update_makefile.sh creates CONFIG_DO_NOT_START_RELAIS_ON_STARTUP */
#ifndef CONFIG_DO_NOT_START_RELAIS_ON_STARTUP
    // set Relais to ON
    HAL::setRelais(true);
#endif
  }
  // avoid second call of sensible ECU functions
  b_firstCall = false;
  return b_result;
}
/** control the relay which is responsible for activation of the PWM output */
void System_c::setRelais( bool ab_activateRelaisForPwm )
{
  if ( ab_activateRelaisForPwm ) HAL::setRelais(true);
  else                           HAL::setRelais(false);
}

/**
	default behaviour of IsoAgLib is to activate power hold, so that
	the application can decide on its own, if a CAN_EN loss shall cause
	a power down of the target. This allows to inhibit stop of application
	on short power supply voltage low bursts.
	@param at_strategy PowerdownByExplcitCall -> stop system only on explicit call of System_c::close()
											PowerdownOnCanEnLoss   -> let BIOS/OS automatically switch off on CAN_EN loss
*/
void System_c::setPowerdownStrategy( IsoAgLib::SystemPowerdownStrategy_t at_strategy )
{
	switch ( at_strategy )
	{
		case IsoAgLib::PowerdownByExplcitCall:
			HAL::stayingAlive();
			break;
		case IsoAgLib::PowerdownOnCanEnLoss:
		default:
			HAL::powerDown();
			break;
	}
}

/**
  init the hardware watchdog
  (uses BIOS function)

  possible errors:
      * Err_c::SystemWatchdog BIOS watchdog configuration without success
  @return true -> watchdog successful configured
*/
bool System_c::initWd ( void )
{
  bool b_result = true;
  // perform the watchdog config (BIOS function)
  switch(HAL::configWatchdog())
  {
    case HAL_WD_DATA_CHANGED:
      // watchdog must be restarted to use the new values
      HAL::wdReset();
      break;
    case HAL_NO_ERR:
      // do nothing because nothing changed
      break;
    default:
      // something went wrong -> system runs with default values
      getILibErrInstance().registerError( iLibErr_c::SystemWatchdog, iLibErr_c::HwSystem );
      b_result = false;
      break;
  }
  return b_result;
}

/**
	deliver the CanEn setting -> if system goes down
	(bios_namespace.h always return ON independent from D+ signal
	if NO_CAN_EN_CHECK is defined in config_esx.h)
	@return true -> D+ or. CAN_EN is active OR D+ should not be checked
*/
bool System_c::canEn( void ) {
	#ifndef CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC
	return HAL::getOn_offSwitch();
	#else
	static int32_t si32_lastCanEnActive = 0;
	const int32_t ci32_now = getTime();
	if ( HAL::getOn_offSwitch() ) si32_lastCanEnActive = ci32_now;
	return ( ci32_now - si32_lastCanEnActive <= CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC );
	#endif
}

} // end of namespace __IsoAgLib
