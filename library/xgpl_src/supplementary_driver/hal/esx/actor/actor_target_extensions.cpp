/***************************************************************************
                          actor_target_extensions.cpp - implement diagnose state
                             -------------------
    begin                : Thu Dec 16 2004
    copyright            : (C) 2004 - 2009 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Module
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                  *
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

/* ********************************************************** */
/** \file
 * 
 * A module targetExtensions should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
 * ********************************************************** */

#include "actor.h"
#include <IsoAgLib/hal/esx/config.h>
#include <IsoAgLib/util/impl/util_funcs.h>

#include <cstdlib>
#include <cstdio>

namespace HAL {

static const uint16_t cui16_openLow_4000mA_6_8  = ( 170000 / 3743 ); // 1700mV * 100 / 37.43mV * 100
static const uint16_t cui16_openLow_2500mA_9_12 = ( 125000 / 3743 ); // 1250mV * 100 / 37.43mV * 100
#ifdef _INIT_BABYBOARD_
static const uint16_t cui16_openLow_4000mA_13_17  = ( 170000 / 900 ); // 1700mV * 100 / 9mV * 100
#endif

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

		const bool b_hiSet = ( aui16_maxCurrent > 0 )?true:false;
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

	/** deliver the measure voltage at the PWM output.
		Use this for application specific state evaluation for cases, where the standard
		getDigoutDiagnose function can go wrong.
		@return voltage at PWM channel [mV]
	*/
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



};
