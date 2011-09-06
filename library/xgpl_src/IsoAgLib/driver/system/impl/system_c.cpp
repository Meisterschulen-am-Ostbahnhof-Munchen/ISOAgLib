/*
  system_c.cpp: central object for encapsulation of platform dependent
    elements (mostly BIOS)

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

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


bool
System_c::close()
{
  if ( !HAL::isSystemOpened() )
    return false;

  HAL::closeSystem();

  return true;
}


bool
System_c::init( IsoAgLib::SystemPowerdownStrategy_t at_strategy )
{
  if ( HAL::isSystemOpened() )
    return false;

  if ( HAL::open_system() != HAL_NO_ERR)
  {
    getILibErrInstance().registerError( iLibErr_c::SystemOpen, iLibErr_c::HwSystem ); // something is still wrong
    return false;
  }

  // init and config the Watchdog
  if (!initWd())
    return false;

  // start the task timer (also needed for CAN)
  HAL::startTaskTimer();

  // configure POWER HOLD after loss of CAN_EN
  setPowerdownStrategy( at_strategy );

  return true;
}


void
System_c::setPowerdownStrategy( IsoAgLib::SystemPowerdownStrategy_t at_strategy )
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


bool
System_c::initWd ( void )
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


bool
System_c::canEn( void )
{
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
