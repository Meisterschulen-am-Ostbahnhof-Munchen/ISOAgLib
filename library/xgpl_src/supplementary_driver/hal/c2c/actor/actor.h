/*
  actor.h: definition of HAL namesapce layer for C2C

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file supplementary_driver/hal/c2c/actor/actor.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the PC version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */

#ifndef _HAL_C2C_ACTOR_H_
#define _HAL_C2C_ACTOR_H_

#include <IsoAgLib/hal/c2c/config.h>
#include <IsoAgLib/hal/c2c/typedef.h>
#include <IsoAgLib/hal/c2c/errcodes.h>

/* ******************************************************** */
/**
 * \name Basic BIOS limit constants
 */
/*@{*/
#define DIGITAL_OUTPUT_MIN 0
// only uncomment following line if  DIGITAL_OUTPUT_MIN > 0
//#define DIGITAL_OUTPUT_MIN_GREATER_ZERO
#define DIGITAL_OUTPUT_MAX 1
/*@}*/

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_c2c/c2c10osy.h>
  }
  /**
    deliver channel number for checking/requesting of
    ADC value at pwm output
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @param ab_channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmoutAdcCheckNr(uint8_t ab_channel)
    { return ab_channel+1; }
  /**
    deliver channel number for checking/requesting of
    current output through given PWM output
    @param ab_channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmCurrentCheckNr(uint8_t ab_channel)
    {return ab_channel;}
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
  /* ********************************* */
  /** \name Actor_c Output BIOS functions */
/*@{*/

  /**
    define the frequency of the pwm signal
    @param bOutput PWM output channel [OUT1..OUT2]
    @param dwFrequency PWM frequency in mHz [5x10^3..1x10^8]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel OR frequency)
  */
  inline int16_t setPwmFreq(uint8_t bOutput, uint32_t dwFrequency)
  // C2C BIOS lets PWM channels OUT1, OUT2 configure individual PWM FREQ
    {
    return __HAL::set_pwm_freq(bOutput, dwFrequency);
	}

  /**
    retrieve maximal PWM frequency -> setting to this value results in maximal output
    @param aui8_channel channel number of output [OUT1..OUT2]
    @return max possible PWM value
  */
  inline uint16_t getMaxPwmDigout(uint8_t aui8_channel)
  { __HAL::tOutput tOutputstatus; __HAL::get_digout_status(aui8_channel,&tOutputstatus);
    return tOutputstatus.wMaxOutput;
  }

  /**
    set pwm value 0 ... 100 %
    @param aui8_channel channel number of output [OUT1..OUT2]
    @param wPWMValue Value to set; depends on configured PWM freq; [0..0xFFFF]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel)
  */
  inline int16_t setDigout(uint8_t aui8_channel, uint16_t wPWMValue)
    {return __HAL::set_digout(aui8_channel, wPWMValue);};

  /** deliver the actual current of the digital output
    * @param aui8_channel channel to check
    * @return current in [mA] ( if specified channel doesn't support current measurement, -1 is returned )
    */
  inline int16_t getDigoutCurrent( uint8_t aui8_channel )
  {
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
