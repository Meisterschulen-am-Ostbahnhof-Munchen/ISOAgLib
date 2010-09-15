/***************************************************************************
                          actor.h - definition of HAL
                                    namesapce layer for ESXu
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
						 : This file was based on the corresponding file in
						 : the ESX HAL and modified for the ESXu HAL.
						 : These changes (C) 2004 - 2005 Michael D. Schmidt
    email                : a.spangler@osb-ag:de
						 : mike.schmidt@agcocorp:com
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
/** \file supplementary_driver/hal/esxu/actor/actor.h
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

#ifndef _HAL_ESXu_ACTOR_H_
#define _HAL_ESXu_ACTOR_H_

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_esxu/mos10osy.h>
  }
}

#include <IsoAgLib/hal/esxu/config.h>
#include <IsoAgLib/hal/esxu/typedef.h>
#include <IsoAgLib/hal/esxu/errcodes.h>

#include <supplementary_driver/driver/rs232/irs232io_c.h>

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
  inline uint8_t getPwmoutAdcCheckNr(uint8_t rb_channel)
    { return GET_OUT_1-rb_channel; }
  /**
    deliver channel number for checking/requesting of
    current output through given PWM output
    @param channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmCurrentCheckNr(uint8_t rb_channel)
    {return GET_I_PWM_1-rb_channel;}
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
    @param bOutputGroup PWM output channel [OUT1..OUT2]
    @param dwFrequency PWM frequency in mHz [5x10^3..1x10^8]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel OR frequency)
  */
  inline int16_t setPwmFreq(uint8_t bOutput, uint32_t dwFrequency)
  // ESXu BIOS lets PWM channels OUT1, OUT2 configure individual PWM FREQ
    {
    int16_t retval = __HAL::set_pwm_freq(bOutput, dwFrequency);

#if defined( DEBUG_HAL )
IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
<< "set_pwm_freq( "
<< (uint16_t)bOutput << ", "
<< (uint16_t)dwFrequency
<< " ) returns " << retval << "\r";
#endif

    return retval;};


  /**
    retrieve maximal PWM frequency -> setting to this value results in maximal output
    @param rui8_channel channel number of output [OUT1..OUT2]
    @return max possible PWM value
  */
  inline uint16_t getMaxPwmDigout(uint8_t rui8_channel)
  {
  __HAL::tOutput tOutputstatus;
  int16_t retval = __HAL::get_digout_status(rui8_channel,&tOutputstatus);

#if defined( DEBUG_HAL )
IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
<< "get_digout_status( "
<< (uint16_t)rui8_channel << ", "
<< "&tOutputstatus"
<< " ) returns " << retval << "\r";
#endif

   return tOutputstatus.wMaxOutput;
  }

  /**
    set pwm value 0 ... 100 %
    @param rui8_channel channel number of output [OUT1..OUT2]
    @param wPWMValue Value to set; depends on configured PWM freq; [0..0xFFFF]
    @return error state (C_NO_ERR == o.k.; C_RANGE == wrong channel)
  */
  inline int16_t setDigout(uint8_t rui8_channel, uint16_t wPWMValue)
    {
    int16_t retval = __HAL::set_digout(rui8_channel, wPWMValue);

#if defined( DEBUG_HAL )
IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
<< "set_digout( "
<< (uint16_t)rui8_channel << ", "
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

#if defined( DEBUG_HAL )
IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
<< "set_digout_mask( "
<< (uint16_t)wOutputMask << ", "
<< (uint16_t)wDigitalValue
<< " ) returns " << retval << "\r";
#endif

    return retval;
    };

  /** deliver the actual current of the digital output
    * @param rui8_channel channel to check
    * @return current in [mA] ( if specified channel doesn't support current measurement, -1 is returned )
    */
  inline int16_t getDigoutCurrent( uint8_t rui8_channel )
  {
//    return (rui8_channel <= OUT4) ? __HAL::get_adc( __HAL::getPwmCurrentCheckNr(rui8_channel) ) : -1;
	if( rui8_channel <= OUT4 )
		{
		int16_t retval = __HAL::get_adc( __HAL::getPwmCurrentCheckNr(rui8_channel) );

#if defined( DEBUG_HAL )
IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
<< "get_adc( "
<< (uint16_t)__HAL::getPwmCurrentCheckNr(rui8_channel)
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
    * @param rui8_channel channel to check
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
