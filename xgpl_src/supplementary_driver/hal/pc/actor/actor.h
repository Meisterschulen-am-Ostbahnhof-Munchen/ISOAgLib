/***************************************************************************
                          actor_actor.h - definition of Hardware Abstraction
                                    Layer for actor functions
                                    for debug/simulation
                                    system on PC
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 by Dipl.-Inform. Achim Spangler
    email                : A.Spangler@osb-ag:de
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
 * Copyright (C) 1999  Dipl.-Inform. Achim Spangler                        *
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
/** \file supplementary_driver/hal/pc/actor/actor.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adopted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module biosExtenders can be used to implement
   the needed activity. The implementation of the PC version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */

#ifndef _HAL_PC_ACTOR_H_
#define _HAL_PC_ACTOR_H_

#include <IsoAgLib/hal/pc/config.h>
#include <IsoAgLib/hal/pc/typedef.h>
#include <IsoAgLib/hal/pc/errcodes.h>
#include "actor_target_extensions.h"

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
       for adaption to new platform
 */
namespace HAL
{
  /* ********************************* */
  /** \name Actor_c Output BIOS functions */
  /*@{*/

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
    @param rui8_channel channel number of output [0..11]
    @return max possible PWM value
  */
  inline uint16_t getMaxPwmDigout(uint8_t /* rui8_channel */ )
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
    * @param rui8_channel channel to check
    * @return current in [mA] ( if specified channel doesn't support current measurement, -1 is returned )
    */
  inline int16_t getDigoutCurrent( uint8_t rui8_channel )
  {
    if ( rui8_channel < 5 ) return 0;
    else return -1;
  };

  /** deliver the state of a digital output
    * @param rui8_channel channel to check
    * @param rui16_minCurrent minimal allowed current in [mA]
    * @param rui16_maxCurrent maximum allowed current in [mA]
    * @return HAL_NO_ERR, HAL_DIGOUT_OPEN, HAL_DIGOUT_SHORTCUT, HAL_DIGOUT_OVERTEMP,
              HAL_DIGOUT_UNDERVOLT, HAL_DIGOUT_OVERVOLT
    */
  inline int16_t getDigoutDiagnose(uint8_t /* rui8_channel */, uint16_t /* rui16_minCurrent */, uint16_t /* rui16_maxCurrent */)
  {
    return HAL_NO_ERR;
  };
  /*@}*/
}
#endif
