/*
  digitalo_c.cpp - implementation for DigitalO_c, an object for digital output

  (C) Copyright 2009 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "digitalo_c.h"
#include <IsoAgLib/util/iassert.h>


namespace __IsoAgLib {

DigitalO_c::DigitalO_c(uint8_t aui8_channel)
  : OutputBase_c(aui8_channel)
  , ui16_minAllowedCurrent( 0 )
  , ui16_maxAllowedCurrent( 0 )
{
}


DigitalO_c::DigitalO_c()
  : OutputBase_c(0xFF) // uninitialized
  , ui16_minAllowedCurrent( 0 )
  , ui16_maxAllowedCurrent( 0 )
{
}


DigitalO_c::~DigitalO_c()
{
}


void
DigitalO_c::setFreq(uint32_t aui32_val)
{
  // set output PWM frequency with BIOS call
  if (HAL::setPwmFreq(channelNr(), aui32_val) != HAL_NO_ERR)
  { // wrong channel number or wrong frequency
    isoaglib_assert( !"setFreq error" );
  }
}


void
DigitalO_c::set(uint16_t aui16_val)
{
  HAL::setDigout(channelNr(), aui16_val);
}


void
DigitalO_c::set(bool ab_state)
{
  set( ab_state ? getMaxOutputPwm() : uint16_t(0) );
}


bool
DigitalO_c::good() const
{
  if ( get() == 0 )
    return true;
    // as even in OFF mode the digout reports currnt of ca. 3 mA, only the ACTIVE state is
    // used to decide about state
    // return (HAL::getDigoutDiagnose( channelNr(), 0, 0 ) == HAL_NO_ERR );
  else
    return (HAL::getDigoutDiagnose( channelNr(), ui16_minAllowedCurrent, ui16_maxAllowedCurrent ) == HAL_NO_ERR );
}


DigitalO_c::dout_err_t
DigitalO_c::getState() const
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
    case HAL_DIGOUT_OVERVOLT:   return dout_overvoltErr;
    case HAL_DIGOUT_UNDERVOLT:  return dout_undervoltErr;
    default: return noDoutErr;
  }
}

} // __IsoAgLib
