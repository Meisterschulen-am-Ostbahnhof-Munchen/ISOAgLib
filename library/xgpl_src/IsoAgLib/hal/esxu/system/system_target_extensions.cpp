/***************************************************************************
                          targetExtensions.cc - source for ESXu specific
                                                 extensions for the HAL
                                                 for central system
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
						 : This file was based on the corresponding file in
						 : the ESX HAL and modified for the ESXu HAL.
						 : These changes (C) 2004 - 2005 Michael D. Schmidt
    email                : a.spangler@osb-ag:de
						 : mike.schmidt@agcocorp:com
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
/** \file IsoAgLib/hal/esxu/system/SystemTargetExtensions.cc
 * A module targetExtensions should be used
 * for all methods, which can't be simply 
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
 * ********************************************************** */

#include "system.h"

namespace __HAL {

static bool system_is_opened = false;

/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system()
{
	int16_t i16_result = C_NO_ERR;
	if( !system_is_opened )
		{
		tSystem t_biosextSysdata;
		i16_result = open_micro(&t_biosextSysdata);

		system_is_opened = true;

#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "open_micro( &t_biosextSysdata ) returns " << i16_result << "\r";

uint8_t buf[64];
CNAMESPACE::sprintf( (char*)buf, "%u ms - open_micro() returns %i\r"
, (uint16_t)__HAL::get_time()
, (int16_t) i16_result
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
		}

	return i16_result;
}
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/

// MSCHMIDT - I think there is a bug here...  
// I think it should read:
//    if( !get_on_off_switch() )
// Same is true for hal\esx\system\system_target_extensions.cc
//
int16_t closeSystem( void )
{ // if CAN_EN ist active -> shut peripherals off and stay in idle loop
  if ( get_on_off_switch() )
  { // CanEn still active
    set_relais( OFF );
    power_down();
  }
  // trigger Watchdog, till CanEn is off
  while ( get_on_off_switch() ) trigger_wd();
  // power off
  int16_t retval = close_micro();
  system_is_opened = false;

#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "close_micro() returns " << retval << "\r";

uint8_t buf[64];
CNAMESPACE::sprintf( (char*)buf, "%u ms - close_micro() returns %i\r"
, (uint16_t)__HAL::get_time()
, (int16_t) retval
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif

  return retval;
}

/** check if open_System() has already been called */
bool isSystemOpened( void )
{
	return system_is_opened;
}

/**
  configure the watchdog of the system with the
  settings of configESXu
  @return error state (C_NO_ERR == o.k.)
    or DATA_CHANGED on new values -> need call of wdReset to use new settings
  @see wdReset
*/
int16_t configWatchdog()
{
  tWDConfig t_watchdogConf = {
      WD_MAX_TIME,
      UD_MIN,
      UD_MAX
  };

  int16_t retval = config_wd( &t_watchdogConf );

#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "config_wd( &t_watchdogConf( "
//<< t_watchdogConf.wWDTime_ms << ", "
//<< t_watchdogConf.wUEmin_mV << ", "
//<< t_watchdogConf.wUEmax_mV
//<< " ) ) returns " << retval << "\r";

uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - config_wd( &t_watchdogConf( %u, %u, %u ) returns %i\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)t_watchdogConf.wWDTime_ms
, (uint16_t)t_watchdogConf.wUEmin_mV
, (uint16_t)t_watchdogConf.wUEmax_mV
, (int16_t) retval
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif

	return retval;
}

} // end namespace __HAL
