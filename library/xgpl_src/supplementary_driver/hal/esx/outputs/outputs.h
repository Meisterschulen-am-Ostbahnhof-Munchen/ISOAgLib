/*
  outputs.h: definition of HAL namesapce layer for ESX

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_ESX_OUTPUTS_H_
#define _HAL_ESX_OUTPUTS_H_

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/esx/typedef.h>
#include <IsoAgLib/hal/esx/errcodes.h>

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

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_esx/Xos20esx.h>

    #ifdef _INIT_BABYBOARD_
    #include <commercial_BIOS/bios_esx/Module/xma20.h>
    #endif
  }

  /**
    deliver channel number for checking/requesting of
    ADC value at pwm output
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @param channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmoutAdcCheckNr(uint8_t channel)
    {
    if (channel < 4) return (43 - (channel * 2));
    else if (channel == 4) return 44;
    else if (channel < 8) return ((47+5) - channel);
    else return ((93+8) - channel);
    };

  /**
    deliver channel number for checking/requesting of
    current output through given PWM output
    @param channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmCurrentCheckNr(uint8_t channel)
    {return (42 - (2 * channel));};
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
    @param bOutput PWM output channel [0..11] ([0..15] with babyboard)
        (4 sets for ESX equivalent freq for channels [4..11])
    @param dwFrequency PWM frequency in mHz [5x10^3..4,29x10^9]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel OR frequency)
  */
  inline int16_t setPwmFreq(uint8_t bOutput, uint32_t dwFrequency)
  // ESX BIOS lets PWM channels 0 to 3 configure individual PWM FREQ
  // rest of PWM channels [4..11] use common PWM freq
    {if (bOutput < 4) return __HAL::set_pwm_freq(bOutput, dwFrequency);
    #ifndef _INIT_BABYBOARD_
    else return __HAL::set_pwm_freq(4, dwFrequency);};
    #else
    else if (bOutput < 12) return __HAL::set_pwm_freq(4, dwFrequency);
    // the BA_set_pwm_freq function counts the babyboard PWM channels beginning with 0
    // --> 12 channels on core ESX --> use offset 12
    // the babyboard PWM Freq is given directly in Hz (and not mHz as for standard core PWMs)
    else return __HAL::BA_set_pwm_freq(POSITION_1, (bOutput-12), dwFrequency/100);}
    #endif


  /**
    retrieve maximal PWM frequency -> setting to this value results in maximal output
    @param aui8_channel channel number of output [0..11] ([0..15] with babyboard)
    @return max possible PWM value
  */
  inline uint16_t getMaxPwmDigout(uint8_t aui8_channel)
  { 
    #ifndef _INIT_BABYBOARD_
    __HAL::tOutput tOutputstatus;
    __HAL::get_digout_status(aui8_channel,&tOutputstatus);
    return tOutputstatus.wMaxOutput;
    #else
    if ( aui8_channel < 12 )
    { __HAL::tOutput tOutputstatus;
      __HAL::get_digout_status(aui8_channel,&tOutputstatus);
      return tOutputstatus.wMaxOutput;
    }
    else
    { __HAL::tBAOutput tOutputstatus;
      __HAL::BA_get_digout_status(POSITION_1, (aui8_channel-12),&tOutputstatus);
      return tOutputstatus.wMaxOutput;
    }
    #endif
  }

  /**
    set pwm value 0 ... 100 %
    @param aui8_channel channel number of output [0..11] ([0..15] with babyboard)
    @param wPWMValue Value to set; depends on configured PWM freq; [0..0xFFFF]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel)
  */
  inline int16_t setDigout(uint8_t aui8_channel, uint16_t wPWMValue)
    #ifndef _INIT_BABYBOARD_
    {return __HAL::set_digout(aui8_channel, wPWMValue);};
    #else
    {
      if ( aui8_channel < 12 )return __HAL::set_digout(aui8_channel, wPWMValue);
      else 
      { __HAL::tBAOutput tOutputstatus;
        __HAL::BA_get_digout_status (POSITION_1, (aui8_channel-12), &tOutputstatus);
        const uint16_t cui16_percent = ( uint32_t(wPWMValue) * 100UL ) / uint32_t(tOutputstatus.wOutputFreq);
        return __HAL::BA_set_digout(POSITION_1, (aui8_channel-12), cui16_percent);
      }
    }
    #endif




  /** deliver the actual current of the digital output
    * @param aui8_channel channel to check [0..11] ([0..15] with babyboard)
    * @return current in [mA] ( if specified channel doesn't support current measurement, -1 is returned )
    */
  inline int16_t getDigoutCurrent( uint8_t aui8_channel )
  {
    if ( aui8_channel < 5 )
    {
      int16_t i16_currentTemp = __HAL::get_adc( __HAL::getPwmCurrentCheckNr( aui8_channel ));
      int16_t i16_current = ((i16_currentTemp * 3) + ((i16_currentTemp * 5)/100));
      return i16_current;
    }
    return -1;
  }

  /** deliver the state of a digital output
		* This function evaluates the current where possible, otherwise it evaluates
		* the measured voltage at the output. The latter interpretation can go wrong
		* if the PWM setting is >0 but has a very low value, so that even under normal
		* conditions the voltage with connected consuming device is lower than to open
		* connector state at low level.
    * @param aui8_channel channel to check [0..11] ([0..15] with babyboard)
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
