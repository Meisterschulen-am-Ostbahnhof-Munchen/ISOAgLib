/***************************************************************************
                          actor_target_extensions_hal_simulator.cpp
                             - source for HAL simulator for Actors (Outputs)
                          This file was based on target_extension_actor_HALSimulator.cpp
                             -------------------
    begin                : Mon Feb 16 2009
    author               : 2009 - Michael D. Schmidt
    email                : Mike.Schmidt@AgcoCorp.com
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                 *
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
#include "actor_target_extensions.h"
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/hal/pc/errcodes.h>
#include <cstdio>

#include <IsoAgLib/hal/pc/hal_simulator/hal_simulator_c.h>

namespace __HAL {

/* ************************************** */
/* ****** Actor_c O BIOS functions  ******* */
/* ************************************** */
/* define the frequency of the pwm signal */
int16_t  setPwmFreq(uint8_t bOutputGroup, uint32_t dwFrequency)
{
	// Callback to HAL Event Handler in the application to update the screen (or whatever)
    HALSimulator_c::GetHALSimulator_c().eventSetPwmFreq( bOutputGroup, dwFrequency );

    return HAL_NO_ERR;
}
/* set pwm value 0 ... 100 %*/
int16_t  setDigout(uint8_t bOutputNo, uint16_t wPWMValue )
{
	// Callback to HAL Event Handler in the application to update the screen (or whatever)
    HALSimulator_c::GetHALSimulator_c().eventSetDigout( bOutputNo, wPWMValue );

    return HAL_NO_ERR;
}

} // End of name space __HAL