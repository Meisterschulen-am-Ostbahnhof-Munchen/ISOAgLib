/*
  actor_target_extensions_hal_simulator.cpp:
    source for HAL simulator for Actors (Outputs) This file was based
    on target_extension_actor_HALSimulator.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "actor_target_extensions.h"
#include <IsoAgLib/hal/hal_typedef.h>
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
