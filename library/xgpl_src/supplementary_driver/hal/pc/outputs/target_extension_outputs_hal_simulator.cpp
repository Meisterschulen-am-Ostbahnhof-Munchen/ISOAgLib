/*
  actor_target_extensions_hal_simulator.cpp:
    source for HAL simulator for Actors (Outputs) This file was based
    on target_extension_actor_HALSimulator.cpp

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include <IsoAgLib/isoaglib_config.h>
#include "outputs_target_extensions.h"
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/hal/pc/errcodes.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/hal/hal_system.h>
#include "../../hal_outputs.h"


namespace __HAL {

void
setMainRelais( bool on )
{
  halSimulator().eventMainRelais( on );
}


int16_t
setPwmFreq(uint8_t bOutputGroup, uint32_t dwFrequency)
{
  if( bOutputGroup == 0xFF )
    return HAL_RANGE_ERR;

  // Callback to HAL Event Handler in the application to update the screen (or whatever)
  halSimulator().eventSetPwmFreq( bOutputGroup, dwFrequency );
  return HAL_NO_ERR;
}



static uint16_t PWMValue[DIGITAL_OUTPUT_MAX - DIGITAL_OUTPUT_MIN + 1];

void
setDigout(uint8_t bOutputNo, uint16_t wPWMValue )
{
  isoaglib_assert( bOutputNo >= DIGITAL_OUTPUT_MIN && bOutputNo <= DIGITAL_OUTPUT_MAX );

  // Callback to HAL Event Handler in the application to update the screen (or whatever)
  halSimulator().eventSetDigout( bOutputNo, wPWMValue );

  PWMValue[bOutputNo - DIGITAL_OUTPUT_MIN] = wPWMValue;
}

uint16_t
getDigout(uint8_t bOutputNo )
{
  isoaglib_assert( bOutputNo >= DIGITAL_OUTPUT_MIN && bOutputNo <= DIGITAL_OUTPUT_MAX );

  return PWMValue[bOutputNo - DIGITAL_OUTPUT_MIN];
}


int16_t
getDigoutDiagnose(uint8_t aui8_channel, uint16_t aui16_minCurrent, uint16_t aui16_maxCurrent )
{ 
  return halSimulator().getDigoutDiagnose(aui8_channel, aui16_minCurrent, aui16_maxCurrent );
}


}
