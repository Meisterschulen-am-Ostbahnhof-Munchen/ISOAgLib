/***************************************************************************
                          system_c.cpp  -  central object for encapsulation of
                                       platform dependent elements (mostly BIOS)
                             -------------------
    begin                : Thu Jul 29 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#include "system_c.h"

#ifdef DEBUG
#include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
#endif

namespace __IsoAgLib {
/** C-style function, to get access to the unique System_c singleton instance */
System_c& getSystemInstance( void ) { return System_c::instance();};

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
bool System_c::init( bool rb_forceReinit, IsoAgLib::SystemPowerdownStrategy_t rt_strategy ){
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
		#ifdef DEBUG
		__IsoAgLib::getRs232Instance()
        << "Fehler bei Systemstart: " << ci_err << "\r\n";
		#endif
      getLbsErrInstance().registerError( LibErr_c::SystemOpen, LibErr_c::HwSystem ); // something is still wrong
      b_result = false;
    }
  }
  if ( rb_forceReinit || b_firstCall )
  { // init and config the Watchdog
    if (!initWd())
    {  // init of watchdog without success
       b_result = false;
    }

    // start the task timer (also needed for CAN)
    HAL::startTaskTimer();
    // configure POWER HOLD after loss of CAN_EN
		setPowerdownStrategy( rt_strategy );
    // set Relais to ON
    HAL::setRelais(ON);
  }
  // avoid second call of sensible ECU functions
  b_firstCall = false;
  return b_result;
}
/**
	default behaviour of IsoAgLib is to activate power hold, so that
	the application can decide on its own, if a CAN_EN loss shall cause
	a power down of the target. This allows to inhibit stop of application
	on short power supply voltage low bursts.
	@param rt_strategy PowerdownByExplcitCall -> stop system only on explicit call of System_c::close()
											PowerdownOnCanEnLoss   -> let BIOS/OS automatically switch off on CAN_EN loss
*/
void System_c::setPowerdownStrategy( IsoAgLib::SystemPowerdownStrategy_t rt_strategy )
{
	switch ( rt_strategy )
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
      getLbsErrInstance().registerError( LibErr_c::SystemWatchdog, LibErr_c::HwSystem );
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
	return ( ci32_now - si32_lastCanEnActive <= CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC )?true:false;
	#endif
}

} // end of namespace __IsoAgLib
