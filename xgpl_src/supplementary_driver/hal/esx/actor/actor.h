/***************************************************************************
                          actor.h - definition of HAL
                                    namesapce layer for ESX
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

/* ************************************************************ */
/** \file supplementary_driver/hal/esx/actor/actor.h
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

#ifndef _HAL_ESX_ACTOR_H_
#define _HAL_ESX_ACTOR_H_

#include <IsoAgLib/hal/esx/config.h>
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
  }
  /**
    deliver channel number for checking/requesting of
    ADC value at pwm output
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @param channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmoutAdcCheckNr(uint8_t rb_channel)
    {
    if (rb_channel < 4) return (43 - (rb_channel * 2));
    else if (rb_channel == 4) return 44;
    else if (rb_channel < 8) return ((47+5) - rb_channel);
    else return ((93+8) - rb_channel);
    };
  /**
    deliver channel number for checking/requesting of
    current output through given PWM output
    @param channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmCurrentCheckNr(uint8_t rb_channel)
    {return (42 - (2 * rb_channel));};
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
    @param bOutputChannel PWM output channel [0..11] ([0..15] with babyboard)
        (4 sets for ESX equivalent freq for channels [4..11])
    @param dwFrequency PWM frequency in mHz [5x10^3..4,29x10^9]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel OR frequency)
  */
  inline int16_t setPwmFreq(uint8_t bOutputChannel, uint32_t dwFrequency)
  // ESX BIOS lets PWM channels 0 to 3 configure individual PWM FREQ
  // rest of PWM channels [4..11] use common PWM freq
    {if (bOutputChannel < 4) return __HAL::set_pwm_freq(bOutputChannel, dwFrequency);
    #ifndef _INIT_BABYBOARD_
    else return __HAL::set_pwm_freq(4, dwFrequency);};
    #else
    else if (bOutputChannel < 12) return __HAL::set_pwm_freq(4, dwFrequency);
    // the BA_set_pwm_freq function counts the babyboard PWM channels beginning with 0
    // --> 12 channels on core ESX --> use offset 12
    else return __HAL::BA_set_pwm_freq(POSITION_1, (bOutputChannel-12), dwFrequency);}
    #endif


  /**
    retrieve maximal PWM frequency -> setting to this value results in maximal output
    @param rui8_channel channel number of output [0..11] ([0..15] with babyboard)
    @return max possible PWM value
  */
  inline uint16_t getMaxPwmDigout(uint8_t rui8_channel)
  { __HAL::tOutput tOutputstatus;
    #ifndef _INIT_BABYBOARD_
    __HAL::get_digout_status(rui8_channel,&tOutputstatus);
    #else
    if ( rui8_channel < 12 ) __HAL::get_digout_status(rui8_channel,&tOutputstatus);
    else __HAL::BA_get_digout_status(POSITION_1, (rui8_channel-12),&tOutputstatus);
    #endif
    return tOutputstatus.wMaxOutput;
  }

  /**
    set pwm value 0 ... 100 %
    @param rui8_channel channel number of output [0..11] ([0..15] with babyboard)
    @param wPWMValue Value to set; depends on configured PWM freq; [0..0xFFFF]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel)
  */
  inline int16_t setDigout(uint8_t rui8_channel, uint16_t wPWMValue)
    #ifndef _INIT_BABYBOARD_
    {return __HAL::set_digout(rui8_channel, wPWMValue);};
    #else
    {
      if ( rui8_channel < 12 )return __HAL::set_digout(rui8_channel, wPWMValue);
      else __HAL::BA_set_digout(POSITION_1, (rui8_channel-12), wPWMValue);
    }
    #endif




  /** deliver the actual current of the digital output
    * @param rui8_channel channel to check [0..11] ([0..15] with babyboard)
    * @return current in [mA] ( if specified channel doesn't support current measurement, -1 is returned )
    */
  inline int16_t getDigoutCurrent( uint8_t rui8_channel )
  {
    if ( rui8_channel < 5 )
    {
      int16_t i16_currentTemp = __HAL::get_adc( __HAL::getPwmCurrentCheckNr( rui8_channel ));
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
    * @param rui8_channel channel to check [0..11] ([0..15] with babyboard)
    * @param rui16_minCurrent minimal allowed current in [mA]
    * @param rui16_maxCurrent maximum allowed current in [mA]
    * @return HAL_NO_ERR, HAL_DIGOUT_OPEN, HAL_DIGOUT_SHORTCUT, HAL_DIGOUT_OVERTEMP,
              HAL_DIGOUT_UNDERVOLT, HAL_DIGOUT_OVERVOLT
    */
  int16_t getDigoutDiagnose(uint8_t rui8_channel, uint16_t rui16_minCurrent, uint16_t rui16_maxCurrent);

	/** deliver the measure voltage at the PWM output.
		Use this for application specific state evaluation for cases, where the standard
		getDigoutDiagnose function can go wrong.
		@return voltage at PWM channel [mV]
	*/
	int16_t getDigoutAdc( uint8_t rui8_channel );

  /*@}*/
}
#endif
