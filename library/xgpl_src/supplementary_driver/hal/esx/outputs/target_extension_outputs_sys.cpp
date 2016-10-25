/*
  outputs_target_extensions.cpp: implement diagnose state

  (C) Copyright 2009 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "outputs.h"

#include <IsoAgLib/hal/esx/config.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include "../../hal_outputs.h"

#include <cstdlib>
#include <cstdio>


namespace HAL {

static const uint16_t cui16_openLow_4000mA_6_8  = ( 170000 / 3743 ); // 1700mV * 100 / 37.43mV * 100
static const uint16_t cui16_openLow_2500mA_9_12 = ( 125000 / 3743 ); // 1250mV * 100 / 37.43mV * 100
#ifdef _INIT_BABYBOARD_
static const uint16_t cui16_openLow_4000mA_13_17  = ( 170000 / 900 ); // 1700mV * 100 / 9mV * 100
#endif

  int16_t getDigoutDiagnose(uint8_t aui8_channel, uint16_t aui16_minCurrent, uint16_t aui16_maxCurrent)
  {
    if ( aui8_channel < 5 )
    {
      int16_t i16_currentTemp = __HAL::get_adc( __HAL::getPwmCurrentCheckNr( aui8_channel ));
      int16_t i16_current = ((i16_currentTemp * 3) + ((i16_currentTemp * 5)/100));
      if ( i16_current < aui16_minCurrent ) return HAL_DIGOUT_OPEN;
      if ( i16_current > aui16_maxCurrent ) return HAL_DIGOUT_SHORTCUT;
      return HAL_NO_ERR;
    }
    // execution reaches this point only for channels >= 5
    #ifndef _INIT_BABYBOARD_
    const int16_t i16_adc = __HAL::get_adc(__HAL::getPwmoutAdcCheckNr(aui8_channel));
    #else
    const int16_t i16_adc = (aui8_channel < 12)
      ?__HAL::get_adc(__HAL::getPwmoutAdcCheckNr(aui8_channel))
      :__HAL::BA_get_adc( POSITION_1, (aui8_channel-12) );;
    #endif

    const bool b_hiSet = ( aui16_maxCurrent > 0 );
    uint16_t ui16_useLowCompare = (aui8_channel<8)?cui16_openLow_4000mA_6_8:cui16_openLow_2500mA_9_12;
    #ifdef _INIT_BABYBOARD_
    if (aui8_channel>11) ui16_useLowCompare = cui16_openLow_4000mA_13_17;
    #endif

    if ( b_hiSet )
    { // if active PWM ( setting > 0 ) has lower voltage than
      // the open gain low level, something is wrong
      // ( if the PWM setting is really low, this evaluation could go wring )
      if ( i16_adc < ui16_useLowCompare ) return HAL_DIGOUT_SHORTCUT;
      else return HAL_NO_ERR;
    }
    else
    {
      if ( i16_adc >= ui16_useLowCompare ) return HAL_DIGOUT_OPEN;
      else return HAL_NO_ERR;
    }
  }

  int16_t getDigoutAdc( uint8_t aui8_channel )
  {
    #ifndef _INIT_BABYBOARD_
    const int16_t ci16_result = __HAL::get_adc(__HAL::getPwmoutAdcCheckNr(aui8_channel));
    if ( ci16_result != HAL_NO_ERR ) return ci16_result;
    return __IsoAgLib::mul1Div1Mul2Div2(ci16_result, 3743, 1, 100);
    #else
    if ( aui8_channel < 12 )
    { // no babyboard channel
      const int16_t ci16_result = __HAL::get_adc(__HAL::getPwmoutAdcCheckNr(aui8_channel));
      if ( ci16_result != HAL_NO_ERR ) return ci16_result;
      return __IsoAgLib::mul1Div1Mul2Div2(ci16_result, 3743, 1, 100);
    }
    else
    {
      const int16_t ci16_result = __HAL::BA_get_adc( POSITION_1, (aui8_channel-12) );
      if ( ci16_result != HAL_NO_ERR ) return ci16_result;
      return ci16_result*9; // 9mV per digit
    }
    #endif
  }


  static uint16_t PWMValue[DIGITAL_OUTPUT_MAX - DIGITAL_OUTPUT_MIN + 1];

  void setDigout(uint8_t aui8_channel, uint16_t wPWMValue)
  {
    isoaglib_assert( aui8_channel >= DIGITAL_OUTPUT_MIN && aui8_channel <= DIGITAL_OUTPUT_MAX );
    #ifndef _INIT_BABYBOARD_
    ( void )__HAL::set_digout(aui8_channel, wPWMValue);
    #else
    if ( aui8_channel < 12 )
      ( void )__HAL::set_digout(aui8_channel, wPWMValue);
    else 
    {
      __HAL::tBAOutput tOutputstatus;
      __HAL::BA_get_digout_status (POSITION_1, (aui8_channel-12), &tOutputstatus);
      const uint16_t cui16_percent = ( uint32_t(wPWMValue) * 100UL ) / uint32_t(tOutputstatus.wOutputFreq);
      ( void )__HAL::BA_set_digout(POSITION_1, (aui8_channel-12), cui16_percent);
    }
    #endif

    PWMValue[aui8_channel - DIGITAL_OUTPUT_MIN] = wPWMValue;
  }

  uint16_t getDigout( uint8_t bOutputNo )
  {
    // could be changed to read the actual values from the BIOS.
    // but this implementation reflects the "old" version which just stored the value.
    isoaglib_assert( bOutputNo >= DIGITAL_OUTPUT_MIN && bOutputNo <= DIGITAL_OUTPUT_MAX );
    return PWMValue[bOutputNo - DIGITAL_OUTPUT_MIN];
  }

};
