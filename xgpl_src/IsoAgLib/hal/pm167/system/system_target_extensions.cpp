/***************************************************************************
                          targetExtensions.cc - source for PM167 specific
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
/** \file IsoAgLib/hal/pm167/system/SystemTargetExtensions.cc
 * A module targetExtensions should be used
 * for all methods, which can't be simply 
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
/* ********************************************************** */

#include "target_extensions.h"
#include "../config.h"


namespace __HAL {
extern "C" {
  /** include the BIOS specific header with the part for CAN into __HAL */
  #include <commercial_BIOS/bios_pm167/mios1.h>
}

static tSystem t_biosextSysdata;

/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system()
{
  return kOpen_pm167(&t_biosextSysdata);
}
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void )
{ // if CAN_EN ist active -> shut peripherals off and stay in idle loop
  #if defined(NO_CAN_EN_CHECK)
  if ( getOn_offSwitch() )
  #endif
  { // CanEn still active
    setRelais( OFF );
    powerDown();
  }
  #if defined(NO_CAN_EN_CHECK)
  // trigger Watchdog, till CanEn is off
  while ( getOn_offSwitch() ) wdTriggern();
  // close ESX as soon as
  closeEsx();
  #else
  while ( true ) wdTriggern();
  #endif
}

/**
  configure the watchdog of the system with the
  settings of configImi
  @return error state (C_NO_ERR == o.k.)
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

} // end namespace __HAL
