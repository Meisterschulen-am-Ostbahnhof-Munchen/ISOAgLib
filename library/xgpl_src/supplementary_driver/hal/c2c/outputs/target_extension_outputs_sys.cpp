/*
  outputs_target_extensions.cpp: implement diagnose state

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

#include "outputs.h"

#include <IsoAgLib/hal/c2c/config.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include "../../hal_outputs.h"

#include <cstdlib>
#include <cstdio>


namespace HAL {

// Measured voltage with NO PWM signal, the get_adc() function always returns around:
// 		85 * 40 = 3400 mV		with NO CONNECTION	
// 		27 * 40 = 1080 mV		with small resistor / transistor
//		0 * 40 = 0 mV			SHORT CIRCUIT to GROUND situation
// So, we can safely say if less than 10 * 40 = 400 mV, then there is a definite problem!
static const int16_t cui16_openLow  = 10;	// 800mV (40mV per bit)

  int16_t getDigoutDiagnose(uint8_t aui8_channel, uint16_t aui16_minCurrent, uint16_t aui16_maxCurrent)
  {
    const int16_t i16_adc = __HAL::get_adc(__HAL::getPwmoutAdcCheckNr(aui8_channel));

    const bool b_hiSet = ( aui16_maxCurrent > 0 );
    const uint16_t cui16_useLowCompare = cui16_openLow;

    if ( b_hiSet )
    { // if active PWM ( setting > 0 ) has lower voltage than
      // the open gain low level, something is wrong
      // ( if the PWM setting is really low, this evaluation could go wring )
      if ( i16_adc < cui16_useLowCompare )
        return HAL_DIGOUT_SHORTCUT;
      else
        return HAL_NO_ERR;
    }
    else
    {
      if ( i16_adc >= cui16_useLowCompare )
        return HAL_DIGOUT_OPEN;
      else
        return HAL_NO_ERR;
    }
  }

  int16_t getDigoutAdc( uint8_t aui8_channel )
  {
    const int16_t ci16_result = __HAL::get_adc(__HAL::getPwmoutAdcCheckNr(aui8_channel));
    if ( ci16_result != HAL_NO_ERR )
      return ci16_result;
    return __IsoAgLib::mul1Div1Mul2Div2(ci16_result, 3743, 1, 100);
  }


  static uint16_t PWMValue[DIGITAL_OUTPUT_MAX - DIGITAL_OUTPUT_MIN + 1];

  void setDigout(uint8_t aui8_channel, uint16_t wPWMValue)
  {
    isoaglib_assert( aui8_channel >= DIGITAL_OUTPUT_MIN && aui8_channel <= DIGITAL_OUTPUT_MAX );
    ( void )__HAL::set_digout(aui8_channel, wPWMValue);

    PWMValue[aui8_channel - DIGITAL_OUTPUT_MIN] = wPWMValue;
  }

  uint16_t getDigout( uint8_t bOutputNo )
  {
    // could be changed to read the actual values from the BIOS.
    // but this implementation reflects the "old" version which just stored the value.
    isoaglib_assert( bOutputNo >= DIGITAL_OUTPUT_MIN && bOutputNo <= DIGITAL_OUTPUT_MAX );
    return PWMValue[bOutputNo - DIGITAL_OUTPUT_MIN];
  }

}
