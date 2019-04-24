/*
  outputs_target_extensions.cpp: source for PC specific extensions for
    the HAL for outputs

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
#include "outputs_target_extensions.h"

#include <IsoAgLib/hal/pc/errcodes.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include "../../hal_outputs.h"

#include <cstdio>


namespace __HAL {


void
setMainRelais( bool on )
{
  printf("Outputs: MainRelais set to %d\n", on);
}


int16_t
setPwmFreq(uint8_t bOutputGroup, uint32_t dwFrequency)
{
  printf("Outputs: Channel %i: PwmFreq set to %i.\n", bOutputGroup, dwFrequency);
  return HAL_NO_ERR;
}


static uint16_t PWMValue[DIGITAL_OUTPUT_MAX - DIGITAL_OUTPUT_MIN + 1];

void
setDigout( uint8_t bOutputNo, uint16_t wPWMValue )
{
  isoaglib_assert( bOutputNo >= DIGITAL_OUTPUT_MIN && bOutputNo <= DIGITAL_OUTPUT_MAX );

  printf("Outputs: Channel %i: DigOut set to %i.\n", bOutputNo, wPWMValue);

  PWMValue[bOutputNo - DIGITAL_OUTPUT_MIN] = wPWMValue;
}

uint16_t
getDigout(uint8_t bOutputNo )
{
  isoaglib_assert( bOutputNo >= DIGITAL_OUTPUT_MIN && bOutputNo <= DIGITAL_OUTPUT_MAX );

  return PWMValue[bOutputNo - DIGITAL_OUTPUT_MIN];
}

int16_t
getDigoutDiagnose(uint8_t /*aui8_channel*/, uint16_t /*aui16_minCurrent*/, uint16_t /*aui16_maxCurrent*/ )
{ 
  return HAL_NO_ERR;
}


}
