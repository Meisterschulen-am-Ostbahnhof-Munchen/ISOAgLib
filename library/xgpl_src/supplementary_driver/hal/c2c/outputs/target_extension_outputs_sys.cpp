/*
  outputs_target_extensions.cpp: implement diagnose state

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "outputs.h"

#include <IsoAgLib/hal/c2c/config.h>
#include <IsoAgLib/util/impl/util_funcs.h>

#include <cstdlib>
#include <cstdio>


namespace HAL {

// Measured voltage with NO PWM signal, the get_adc() function always returns around:
// 		85 * 40 = 3400 mV		with NO CONNECTION	
// 		27 * 40 = 1080 mV		with small resistor / transistor
//		0 * 40 = 0 mV			SHORT CIRCUIT to GROUND situation
// So, we can safely say if less than 10 * 40 = 400 mV, then there is a definite problem!
static const int16_t cui16_openLow  = 10;	// 800mV (40mV per bit)

  /** deliver the state of a digital output.
		* This function evaluates the current where possible, otherwise it evaluates
		* the measured voltage at the output. The latter interpretation can go wrong
		* if the PWM setting is >0 but has a very low value, so that even under normal
		* conditions the voltage with connected consuming device is lower than to open
		* connector state at low level.
    * @param aui8_channel channel to check
    * @param aui16_minCurrent minimal allowed current in [mA]
    * @param aui16_maxCurrent maximum allowed current in [mA]
    * @return HAL_NO_ERR, HAL_DIGOUT_OPEN, HAL_DIGOUT_SHORTCUT    */
  int16_t getDigoutDiagnose(uint8_t aui8_channel, uint16_t aui16_minCurrent, uint16_t aui16_maxCurrent)
  {
		const int16_t i16_adc = __HAL::get_adc(__HAL::getPwmoutAdcCheckNr(aui8_channel));

		const bool b_hiSet = ( aui16_maxCurrent > 0 );
		const uint16_t cui16_useLowCompare = cui16_openLow;

		if ( b_hiSet )
		{ // if active PWM ( setting > 0 ) has lower voltage than
			// the open gain low level, something is wrong
			// ( if the PWM setting is really low, this evaluation could go wring )
			if ( i16_adc < cui16_useLowCompare ) return HAL_DIGOUT_SHORTCUT;
			else return HAL_NO_ERR;
		}
		else
		{
			if ( i16_adc >= cui16_useLowCompare ) return HAL_DIGOUT_OPEN;
			else return HAL_NO_ERR;
		}
  }

	/** deliver the measure voltage at the PWM output.
		Use this for application specific state evaluation for cases, where the standard
		getDigoutDiagnose function can go wrong.
		@return voltage at PWM channel [mV]
	*/
	int16_t getDigoutAdc( uint8_t aui8_channel )
	{
		const int16_t ci16_result = __HAL::get_adc(__HAL::getPwmoutAdcCheckNr(aui8_channel));
		if ( ci16_result != HAL_NO_ERR ) return ci16_result;
		return __IsoAgLib::mul1Div1Mul2Div2(ci16_result, 3743, 1, 100);
	}



};
