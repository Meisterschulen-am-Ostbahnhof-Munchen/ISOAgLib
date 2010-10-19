/*
  digitalo_c.cpp:
    implementation for DigitalO_c, an object for digital actor output

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "digitalo_c.h"
#include "actoro_c.h"

#include <IsoAgLib/util/iliberr_c.h>


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
  @param aui8_channel default-argument for setting hardware channel for this input
*/
DigitalO_c::DigitalO_c(uint8_t aui8_channel)
  : ActorBase_c(aui8_channel){
  init( aui8_channel );
}
/** init this instance
  @param aui8_channel default-argument for setting hardware channel for this input
*/
void DigitalO_c::init(uint8_t aui8_channel)
{
  ActorBase_c::init( aui8_channel );
  // config the PWM freq with BIOS call
  if (HAL::setPwmFreq(aui8_channel, CONFIG_PWM_DEFAULT_FREQUENCY) == HAL_RANGE_ERR)
  { // wrong channel or PWM
    ui16_maxOutputPwmFreq = 0xFFFF;
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Actor );
  }
  else
  { // correct channel and PWM - now register the valid new analog input into ActorO_c
    // retrieve max allowed PWM freq
    ui16_maxOutputPwmFreq = HAL::getMaxPwmDigout( aui8_channel );

    getActorInstance().registerClient( this );
  }
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
  @param aui32_val value to use as PWM frequency in [mHz]
*/
void DigitalO_c::setFreq(uint32_t aui32_val){
  // set output PWM frequency with BIOS call
  if (HAL::setPwmFreq(channelNr(), aui32_val) == HAL_RANGE_ERR)
  { // wrong channel number or wrong frequency
	ui16_maxOutputPwmFreq = 0xFFFF;
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Actor );
  }
  else
  {
    // retrieve max allowed PWM freq
    ui16_maxOutputPwmFreq = HAL::getMaxPwmDigout( channelNr() );
  }
}

/**
  set the output PWM value
  (uses BIOS function)

  possible errors:
     * Err_c::range wrong output channel number
  @param aui16_val value to set for the output channel [0..0xffff]
*/
void DigitalO_c::set(uint16_t aui16_val){
  // set output PWM signal with BIOS call
  if (HAL::setDigout(channelNr(), aui16_val) == HAL_RANGE_ERR)
  { // wrong channel number
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Actor );
  }
  else
  { // correct channel number
    ui16_value = aui16_val;
  }
}
/**
  switch PWM to total OFF or ON (simple switch function)
  (uses BIOS function)

  possible errors:
     * Err_c::range wrong output channel number
  @param ab_state
*/
void DigitalO_c::set(bool ab_state){
  if ( ab_state ) set( ui16_maxOutputPwmFreq );
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
    // return (HAL::getDigoutDiagnose( channelNr(), 0, 0 ) == HAL_NO_ERR );
  else
    return (HAL::getDigoutDiagnose( channelNr(), ui16_minAllowedCurrent, ui16_maxAllowedCurrent ) == HAL_NO_ERR );
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
