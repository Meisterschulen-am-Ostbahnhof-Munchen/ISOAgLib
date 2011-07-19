/*
  outputs.h:
    definition of Hardware Abstraction Layer for output functions for
    debug/simulation system on PC

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_PC_OUTPUTS_H_
#define _HAL_PC_OUTPUTS_H_

#include <IsoAgLib/hal/pc/config.h>
#include <IsoAgLib/hal/pc/typedef.h>
#include <IsoAgLib/hal/pc/errcodes.h>
#include "outputs_target_extensions.h"

#include <cstdio>


/* ******************************************************** */
/**
 * \name Basic BIOS limit constants
 */
/*@{*/
#define DIGITAL_OUTPUT_MIN 0
// only uncomment following line if  DIGITAL_OUTPUT_MIN > 0
//#define DIGITAL_OUTPUT_MIN_GREATER_ZERO
#define DIGITAL_OUTPUT_MAX 11
/*@}*/

/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaptation to new platform
 */
namespace HAL
{
  /* **************************** */
  /** \name Output BIOS functions */
  /*@{*/

  inline void setMainRelais( bool bitState ) { __HAL::setMainRelais(bitState); }

  /**
    define the frequency of the pwm signal
    @param bOutputGroup PWM output channel [0..4] (4 means channel 4 to 11)
    @param dwFrequency PWM frequency in mHz [5x10^3..4,29x10^9]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel OR frequency)
  */
  inline int16_t setPwmFreq(uint8_t bOutputGroup, uint32_t dwFrequency)
    {return __HAL::setPwmFreq(bOutputGroup, dwFrequency);};

  /**
    retrieve maximal PWM frequency -> setting to this value results in maximal output
    @param aui8_channel channel number of output [0..11]
    @return max possible PWM value
  */
  inline uint16_t getMaxPwmDigout(uint8_t /* aui8_channel */ )
  { return 0xFFFF; };

  /**
    set pwm value 0 ... 100 %
    @param bOutputNo channel number of output [0..11]
    @param wPWMValue Value to set; depends on configured PWM freq; [0..0xFFFF]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel)
  */
  inline int16_t setDigout(uint8_t bOutputNo, uint16_t wPWMValue)
    {return __HAL::setDigout(bOutputNo, wPWMValue);};

  /** deliver the actual current of the digital output
    * @param aui8_channel channel to check
    * @return current in [mA] ( if specified channel doesn't support current measurement, -1 is returned )
    */
  inline int16_t getDigoutCurrent( uint8_t aui8_channel )
  {
    if ( aui8_channel < 5 ) return 0;
    else return -1;
  };

  /** deliver the state of a digital output
    <!--* @param aui8_channel channel to check
    * @param aui16_minCurrent minimal allowed current in [mA]
    * @param aui16_maxCurrent maximum allowed current in [mA]-->
    * @return HAL_NO_ERR, HAL_DIGOUT_OPEN, HAL_DIGOUT_SHORTCUT, HAL_DIGOUT_OVERTEMP,
              HAL_DIGOUT_UNDERVOLT, HAL_DIGOUT_OVERVOLT
    */
  inline int16_t getDigoutDiagnose(uint8_t /* aui8_channel */, uint16_t /* aui16_minCurrent */, uint16_t /* aui16_maxCurrent */)
  {
    return HAL_NO_ERR;
  };

	/** deliver the measure voltage at the PWM output.
		Use this for application specific state evaluation for cases, where the standard
		getDigoutDiagnose function can go wrong.
		@return voltage at PWM channel [mV]
	*/
	inline int16_t getDigoutAdc( uint8_t /* aui8_channel */ ) { return 1000;};
  /*@}*/
}
#endif
