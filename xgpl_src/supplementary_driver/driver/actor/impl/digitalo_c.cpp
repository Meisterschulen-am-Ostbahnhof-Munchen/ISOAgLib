/***************************************************************************
                          digitalo_c.cpp - implementation for DigitalO_c, an object
                                         for digital actor output
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#include "digitalo_c.h"
#include "actoro_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  internal called constructor for a new digital input
  channel which performs configuration of hardware
  (configure PWM frequency to CONFIG_PWM_DEFAULT_FREQUENCY
  which is set in isoaglib_config.h)
  (uses BIOS function)

  possible errors:
     * Err_c::range wrong output channel number
  @see ActorO_c::createDigital
  @param rui8_channel default-argument for setting hardware channel for this input
*/
DigitalO_c::DigitalO_c(uint8_t rui8_channel)
  : ActorBase_c(rui8_channel){
  init( rui8_channel );
}
/** init this instance
  @param rui8_channel default-argument for setting hardware channel for this input
*/
void DigitalO_c::init(uint8_t rui8_channel)
{
  ActorBase_c::init( rui8_channel );
  // config the PWM freq with BIOS call
  if (HAL::setPwmFreq(rui8_channel, CONFIG_PWM_DEFAULT_FREQUENCY) == HAL_RANGE_ERR)
  { // wrong channel or PWM
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Actor );
  }
  else
  { // correct channel and PWM - now register the valid new analog input into ActorO_c
    getActorInstance().registerClient( this );
  }
  // retrieve max allowed PWM freq
  ui16_maxOutputPwmFreq = HAL::getMaxPwmDigout( rui8_channel );
}

/**  destructor of the input object which can close explicit the hardware input */
DigitalO_c::~DigitalO_c(){
  getActorInstance().unregisterClient( this );
}

/**
  set the output PWM frequency
  (uses BIOS function)

  possible errors:
     * Err_c::range wrong output channel number or wrong frequency
  @param rui32_val value to use as PWM frequency in [mHz]
*/
void DigitalO_c::setFreq(uint32_t rui32_val){
  // set output PWM frequency with BIOS call
  if (HAL::setPwmFreq(channelNr(), rui32_val) == HAL_RANGE_ERR)
  { // wrong channel number or wrong frequency
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Actor );
  }
}

/**
  set the output PWM value
  (uses BIOS function)

  possible errors:
     * Err_c::range wrong output channel number
  @param rui16_val value to set for the output channel [0..0xffff]
*/
void DigitalO_c::set(uint16_t rui16_val){
  // set output PWM signal with BIOS call
  if (HAL::setDigout(channelNr(), rui16_val) == HAL_RANGE_ERR)
  { // wrong channel number
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Actor );
  }
  else
  { // correct channel number
    ui16_value = rui16_val;
  }
}
/**
  switch PWM to total OFF or ON (simple switch function)
  (uses BIOS function)

  possible errors:
     * Err_c::range wrong output channel number
  @param rb_state
*/
void DigitalO_c::set(bool rb_state){
  if ( rb_state ) set( ui16_maxOutputPwmFreq );
  else set( uint16_t(0) );
}
/** check if this digital output channel is in GOOD condition
  * @return true -> channel is in GOOD condition
  */
bool DigitalO_c::good( void ) const
{
  if ( get() == 0 )  return true;
    // as even in OFF mode the digout reports currnt of ca. 3 mA, only the ACTIVE state is
	// used to decide about state
    // return (HAL::getDigoutDiagnose( channelNr(), 0, 0 ) == HAL_NO_ERR )?true:false;
  else
    return (HAL::getDigoutDiagnose( channelNr(), ui16_minAllowedCurrent, ui16_maxAllowedCurrent ) == HAL_NO_ERR )?true:false;
}

/** deliver detailed error state information for this Digital Output
	* This function evaluates the current where possible, otherwise it evaluates
	* the measured voltage at the output. The latter interpretation can go wrong
	* if the PWM setting is >0 but has a very low value, so that even under normal
	* conditions the voltage with connected consuming device is lower than to open
	* connector state at low level.
  * @return dout_err_t [noDoutErr|dout_openErr|dout_shortcutErr]
  */
DigitalO_c::dout_err_t DigitalO_c::getState( void ) const
{
  int16_t i16_stateHal;
  if ( get() == 0 )
    i16_stateHal = HAL::getDigoutDiagnose( channelNr(), 0, 0 );
  else
    i16_stateHal = HAL::getDigoutDiagnose( channelNr(), ui16_minAllowedCurrent, ui16_maxAllowedCurrent );
  switch ( i16_stateHal )
  {
    case HAL_DIGOUT_OPEN:       return dout_openErr;
    case HAL_DIGOUT_SHORTCUT:   return dout_shortcutErr;
		default: return noDoutErr;
  }
}

} // end of namespace __IsoAgLib
