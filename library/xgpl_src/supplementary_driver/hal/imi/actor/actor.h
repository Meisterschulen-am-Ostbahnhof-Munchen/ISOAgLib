/*
  actor.h: definition of HAL namesapce layer for IMI

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file supplementary_driver/hal/imi/actor/actor.h
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

#ifndef _HAL_IMI_ACTOR_H_
#define _HAL_IMI_ACTOR_H_

#include <IsoAgLib/hal/imi/config.h>
#include <IsoAgLib/hal/imi/typedef.h>
#include <IsoAgLib/hal/imi/errcodes.h>

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
    #include <commercial_BIOS/bios_imi/adis10.h>
  }
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
    for the single output UCout
    @param bOutputGroup PWM output channel 0 (is ignored)
    @param dwFrequency PWM frequency in mHz [5x10^3..4,29x10^9]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel OR frequency)
  */
  inline int16_t setPwmFreq(uint8_t bOutputGroup, uint32_t dwFrequency)
    {return __HAL::setPwmFreq(dwFrequency);};

  /**
    retrieve maximal PWM frequency -> setting to this value results in maximal output
    @param aui8_channel channel number of output [0..11]
    @return max possible PWM value
  */
  inline uin16_t getMaxPwmDigout(uint8_t aui8_channel)
  { __HALL::tOutput tOutputstatus; __HAL::get_digout_status(&tOutputstatus);
    return tOutputstatus.wMaxOutput;
  }

  /**
    set pwm value 0 ... 100 %
    for the single output UCout
    <!--@param bOutputNo channel number of output 0 (is ignored)-->
    @param wPWMValue Value to set; depends on configured PWM freq; [0..0xFFFF]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel)
  */
  inline int16_t setDigout(uint8_t /* bOutputNo */, uint16_t wPWMValue)
    {__HAL::set_out_UD(wPWMValue);return HAL_NO_ERR;};

	/** deliver the actual current of the digital output.
		* This is UNDEFINED for ADIS - IMI
    <!--* @param aui8_channel channel to check-->
    * @return current in [mA] ( if specified channel doesn't support current measurement, -1 is returned )
    */
  inline int16_t getDigoutCurrent( uint8_t /* aui8_channel */ )
  {
    return -1;
  }
  /** deliver the state of a digital output.
		* This is UNDEFINED for ADIS - IMI
    <!--* @param aui8_channel channel to check
    * @param aui16_minCurrent minimal allowed current in [mA]
    * @param aui16_maxCurrent maximum allowed current in [mA]-->
    * @return HAL_NO_ERR, HAL_DIGOUT_OPEN, HAL_DIGOUT_SHORTCUT, HAL_DIGOUT_OVERTEMP,
              HAL_DIGOUT_UNDERVOLT, HAL_DIGOUT_OVERVOLT
    */
  inline int16_t getDigoutDiagnose(uint8_t /* aui8_channel */, uint16_t /* aui16_minCurrent */, uint16_t /* aui16_maxCurrent */ )
	{ return HAL_NO_ERR;};

	/** deliver the measure voltage at the PWM output.
		Use this for application specific state evaluation for cases, where the standard
		getDigoutDiagnose function can go wrong.
		ADIS uses ADC channel ANA9 for this purpose.
		@return voltage at PWM channel [mV]
	*/
	int16_t getDigoutAdc( uint8_t /* aui8_channel */ ) { return __HAL::get_analogin_mean(ANA9);};

  /*@}*/
}
#endif
