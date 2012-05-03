/*
  outputs.h: definition of HAL namesapce layer for ESXu

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_ESXu_OUTPUTS_H_
#define _HAL_ESXu_OUTPUTS_H_

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_esxu/mos10osy.h>
  }
}

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/esxu/typedef.h>
#include <IsoAgLib/hal/esxu/errcodes.h>


/* ******************************************************** */
/**
 * \name Basic BIOS limit constants
 */
/*@{*/
#define DIGITAL_OUTPUT_MIN OUT1
// only uncomment following line if  DIGITAL_OUTPUT_MIN > 0
#if (DIGITAL_OUTPUT_MIN > 0)
#	define DIGITAL_OUTPUT_MIN_GREATER_ZERO
#endif
#define DIGITAL_OUTPUT_MAX OUT6
/*@}*/

namespace __HAL {
  /**
    deliver channel number for checking/requesting of
    ADC value at pwm output
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @param channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmoutAdcCheckNr(uint8_t ab_channel)
    { return GET_OUT_1-ab_channel; }

  /**
    deliver channel number for checking/requesting of
    current output through given PWM output
    @param channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmCurrentCheckNr(uint8_t ab_channel)
    {return GET_I_PWM_1-ab_channel;}
}

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

  inline void setMainRelais(bool bitState) { __HAL::set_relais(bitState); }

  /**
    define the frequency of the pwm signal
    @param bOutput PWM output channel [OUT1..OUT2]
    @param dwFrequency PWM frequency in mHz [5x10^3..1x10^8]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel OR frequency)
  */
  inline int16_t setPwmFreq(uint8_t bOutput, uint32_t dwFrequency)
  // ESXu BIOS lets PWM channels OUT1, OUT2 configure individual PWM FREQ
    {
    int16_t retval = __HAL::set_pwm_freq(bOutput, dwFrequency);

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - "
<< "set_pwm_freq( "
<< (uint16_t)bOutput << ", "
<< (uint16_t)dwFrequency
<< " ) returns " << retval << "\r";
#endif

    return retval;};


  /**
    retrieve maximal PWM frequency -> setting to this value results in maximal output
    @param aui8_channel channel number of output [OUT1..OUT2]
    @return max possible PWM value
  */
  inline uint16_t getMaxPwmDigout(uint8_t aui8_channel)
  {
  __HAL::tOutput tOutputstatus;
  int16_t retval = __HAL::get_digout_status(aui8_channel,&tOutputstatus);

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - "
<< "get_digout_status( "
<< (uint16_t)aui8_channel << ", "
<< "&tOutputstatus"
<< " ) returns " << retval << "\r";
#endif

   return tOutputstatus.wMaxOutput;
  }

  /**
    set pwm value 0 ... 100 %
    @param aui8_channel channel number of output [OUT1..OUT2]
    @param wPWMValue Value to set; depends on configured PWM freq; [0..0xFFFF]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel)
  */
  inline int16_t setDigout(uint8_t aui8_channel, uint16_t wPWMValue)
    {
    int16_t retval = __HAL::set_digout(aui8_channel, wPWMValue);

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - "
<< "set_digout( "
<< (uint16_t)aui8_channel << ", "
<< (uint16_t)wPWMValue
<< " ) returns " << retval << "\r";
#endif

    return retval;
    };

  /**
	This function sets the digital Output over mask.
	The Bit 0, 1, 2... represent the Output 1, 2, 3...
	If the bit is set with wOutputMask, the appropriate bit from wDigitalValue will be taken
	over the mask out.
	If the bit is not set in the mask, the output remains unchanged.

    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t setDigoutMask(uint16_t wOutputMask, uint16_t wDigitalValue)
    {
    int16_t retval = __HAL::set_digout_mask(wOutputMask, wDigitalValue);

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - "
<< "set_digout_mask( "
<< (uint16_t)wOutputMask << ", "
<< (uint16_t)wDigitalValue
<< " ) returns " << retval << "\r";
#endif

    return retval;
    };

  /** deliver the actual current of the digital output
    * @param aui8_channel channel to check
    * @return current in [mA] ( if specified channel doesn't support current measurement, -1 is returned )
    */
  inline int16_t getDigoutCurrent( uint8_t aui8_channel )
  {
//    return (aui8_channel <= OUT4) ? __HAL::get_adc( __HAL::getPwmCurrentCheckNr(aui8_channel) ) : -1;
	if( aui8_channel <= OUT4 )
		{
		int16_t retval = __HAL::get_adc( __HAL::getPwmCurrentCheckNr(aui8_channel) );

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - "
<< "get_adc( "
<< (uint16_t)__HAL::getPwmCurrentCheckNr(aui8_channel)
<< " ) returns " << retval << "\r";
#endif

		return retval;
		}
	else
		return -1;
  }

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

	/** deliver the measure voltage at the PWM output.
		Use this for application specific state evaluation for cases, where the standard
		getDigoutDiagnose function can go wrong.
		@return voltage at PWM channel [mV]
	*/
	int16_t getDigoutAdc( uint8_t aui8_channel );

  /*@}*/
}
#endif
