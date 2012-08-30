/*
  inputs_c.cpp - implmentation for the inputs management

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "inputs_c.h"
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>


namespace __IsoAgLib {

/** C-style function, to get access to the unique Inputs_c singleton instance */
Inputs_c &getInputsInstance()
{
  MACRO_SINGLETON_GET_INSTANCE_BODY(Inputs_c);
}


void
Inputs_c::init(
  uint8_t ab_digitalFirst, uint8_t ab_digitalLast,
  uint8_t ab_analogFirst,  uint8_t ab_analogLast,
  uint8_t ab_counterFirst, uint8_t ab_counterLast)
{
  setDigitalLimits(ab_digitalFirst, ab_digitalLast);
  setAnalogLimits(ab_analogFirst, ab_analogLast);
  setCounterLimits(ab_counterFirst, ab_counterLast);
}


Inputs_c::~Inputs_c()
{
}


void
Inputs_c::setDigitalLimits(uint8_t ab_digitalFirst, uint8_t ab_digitalLast){

  isoaglib_assert( ab_digitalFirst <= DIGITAL_INPUT_MAX );
#ifdef DIGITAL_INPUT_MIN_GREATER_ZERO
  isoaglib_assert( ab_digitalFirst >= DIGITAL_INPUT_MIN );
  isoaglib_assert( ab_digitalLast >= DIGITAL_INPUT_MIN);
#endif
  isoaglib_assert( ab_digitalLast <= DIGITAL_INPUT_MAX );

  setDigitalFirst(ab_digitalFirst);
  setDigitalLast(ab_digitalLast);

}


void
Inputs_c::setAnalogLimits(uint8_t ab_analogFirst, uint8_t ab_analogLast){
  isoaglib_assert( ab_analogFirst <= ANALOG_INPUT_MAX );
#ifdef ANALOG_INPUT_MIN_GREATER_ZERO
  isoaglib_assert( ab_analogFirst >= ANALOG_INPUT_MIN );
  isoaglib_assert( ab_analogLast >= ANALOG_INPUT_MIN );
#endif
  isoaglib_assert( ab_analogLast <= ANALOG_INPUT_MAX );

  setAnalogFirst( ab_analogFirst );
  setAnalogLast( ab_analogLast );
}


void
Inputs_c::setCounterLimits(uint8_t ab_counterFirst, uint8_t ab_counterLast){
  isoaglib_assert ( ab_counterFirst <= COUNTER_INPUT_MAX );
#ifdef COUNTER_INPUT_MIN_GREATER_ZERO
  isoaglib_assert( ab_counterFirst >= COUNTER_INPUT_MIN );
  isoaglib_assert( ab_counterLast >= COUNTER_INPUT_MIN );
#endif
  isoaglib_assert( ab_counterLast <= COUNTER_INPUT_MAX );

  setCounterFirst(ab_counterFirst);
  setCounterLast(ab_counterLast);
}


} // __IsoAgLib
