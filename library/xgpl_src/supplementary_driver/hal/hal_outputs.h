/*
  hal_outputs.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    output control

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

#ifndef _HAL_INDEPENDENT_OUTPUTS_H_
#define _HAL_INDEPENDENT_OUTPUTS_H_

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/hal_config.h>

#define HAL_DIGOUT_OPEN         (int16_t)(-(20 * 0x0001))
#define HAL_DIGOUT_SHORTCUT     (int16_t)(-(21 * 0x0001))
// HAL_DIGOUT_OVERTEMP is currently not handled by ISOAgLib driver code!
#define HAL_DIGOUT_OVERTEMP     (int16_t)(-(22 * 0x0001))
#define HAL_DIGOUT_UNDERVOLT    (int16_t)(-(23 * 0x0001))
#define HAL_DIGOUT_OVERVOLT     (int16_t)(-(24 * 0x0001))


namespace HAL
{
  /**
    switch main relais on or off
    @param bitState true -> Main Relais ON
  */
  void setMainRelais( bool bitState );

  /**
    define the frequency of the pwm signal
    @param bOutput PWM output channel
    @param dwFrequency PWM frequency in mHz [5x10^3..4,29x10^9]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel OR frequency)
  */
  int16_t setPwmFreq(uint8_t bOutput, uint32_t dwFrequency);

  /**
    retrieve maximal PWM value for configured frequency -> setting to this value results in maximal output
    @param aui8_channel channel number of output
    @return max possible PWM value
  */
  uint16_t getMaxPwmDigout(uint8_t aui8_channel);

  /**
    set pwm value 0 ... 100 %
    @param aui8_channel channel number of output
    @param wPWMValue Value to set; depends on configured PWM freq; [0..0xFFFF]
  */
  void setDigout( uint8_t aui8_channel, uint16_t wPWMValue);

  /**
    get pwm value 0 ... 100 %
    @param aui8_channel channel number of output
    @return Value; depends on configured PWM freq; [0..0xFFFF]
  */
  uint16_t getDigout( uint8_t aui8_channel );

  /** deliver the actual current of the digital output
    * @param aui8_channel channel to check
    * @return current in [mA] ( if specified channel doesn't support current measurement, -1 is returned )
    */
  int16_t getDigoutCurrent( uint8_t aui8_channel );

  /** deliver the state of a digital output
		* This function evaluates the current where possible, otherwise it evaluates
		* the measured voltage at the output. The latter interpretation can go wrong
		* if the PWM setting is >0 but has a very low value, so that even under normal
		* conditions the voltage with connected consuming device is lower than to open
		* connector state at low level.
    * @param aui8_channel channel to check
    * @param aui16_minCurrent minimal allowed current in [mA]
    * @param aui16_maxCurrent maximum allowed current in [mA]
    * @return HAL_NO_ERR, HAL_DIGOUT_OPEN, HAL_DIGOUT_SHORTCUT, HAL_DIGOUT_OVERTEMP,
              HAL_DIGOUT_UNDERVOLT, HAL_DIGOUT_OVERVOLT
    */
  int16_t getDigoutDiagnose(uint8_t aui8_channel, uint16_t aui16_minCurrent, uint16_t aui16_maxCurrent);
  
  /**
    deliver the measure voltage at the PWM output.
    Use this for application specific state evaluation for cases, where the standard
    getDigoutDiagnose function can go wrong.
    @return voltage at PWM channel [mV]
  */
  int16_t getDigoutAdc( uint8_t aui8_channel );
} // HAL


/// INCLUDE CONCRETE SYSTEM-HAL-IMPLEMENTATION
/// ==========================================

#ifdef HAL_PATH_SUPPLEMENTARY_OUTPUTS
#  define _hal_outputs_header_ <HAL_PATH_SUPPLEMENTARY_OUTPUTS/outputs.h>
#  include _hal_outputs_header_
#endif


#endif
