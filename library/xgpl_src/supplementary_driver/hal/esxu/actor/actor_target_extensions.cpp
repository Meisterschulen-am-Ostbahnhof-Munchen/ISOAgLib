/***************************************************************************
                          actor_target_extensions.cpp - implement diagnose state
                             -------------------
    begin                : Thu Dec 16 2004
    copyright            : (C) 2004 - 2004 Dipl.-Inform. Achim Spangler
						 : This file was based on the corresponding file in
						 : the ESX HAL and modified for the ESXu HAL.
						 : These changes (C) 2004 - 2005 Michael D. Schmidt
    email                : a.spangler@osb-ag:de
						 : mike.schmidt@agcocorp:com
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

#include <IsoAgLib/util/impl/util_funcs.h>
#include <supplementary_driver/driver/rs232/irs232io_c.h>


namespace HAL {

static const int16_t cui16_openLow  = ( 170000L / 4000L ); // 1700mV * 100 / 40mV * 100

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
		int16_t i16_adc = getDigoutCurrent(aui8_channel);

		if( i16_adc < 0 )
			return HAL_NO_ERR;	// Pin doesn't support current checking - should really do something else here!!!

		bool b_hiSet = ( aui16_maxCurrent > 0 )?true:false;
		uint16_t cui16_useLowCompare = cui16_openLow;

		if ( b_hiSet )
		{ // if active PWM ( setting > 0 ) has lower voltage than
			// the open gain low level, something is wrong
			// ( if the PWM setting is really low, this evaluation could go wring )
			if ( i16_adc < cui16_useLowCompare ) return HAL_DIGOUT_SHORTCUT;
			else return HAL_NO_ERR;
		}
		else
		{
			if ( i16_adc >= cui16_useLowCompare ) return HAL_DIGOUT_OPEN;
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
		if( aui8_channel > OUT4 )
			return -1;

		int16_t ci16_result = __HAL::get_adc(__HAL::getPwmoutAdcCheckNr(aui8_channel));

#if defined( DEBUG_HAL )
IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
<< "get_adc( "
<< (uint16_t)__HAL::getPwmoutAdcCheckNr(aui8_channel)
<< " ) returns " << ci16_result << "\r";
#endif

		if ( ci16_result != HAL_NO_ERR ) return ci16_result;
		return __IsoAgLib::mul1Div1Mul2Div2(ci16_result, 3743, 1, 100);
	}



};
