/*
  inputs_c.cpp: implmentation for the inputs management

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "inputs_c.h"
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/driver/system/impl/system_c.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** C-style function, to get access to the unique Inputs_c singleton instance */
Inputs_c &getInputsInstance()
{
  MACRO_MULTITON_GET_INSTANCE_BODY(Inputs_c, 1, 0);
}


void
Inputs_c::init(
  uint8_t ab_digitalFirst, uint8_t ab_digitalLast,
  uint8_t ab_analogFirst,  uint8_t ab_analogLast,
  uint8_t ab_counterFirst, uint8_t ab_counterLast)
{ // store the channel limits for dig and analog
  setDigitalLimits(ab_digitalFirst, ab_digitalLast);
  setAnalogLimits(ab_analogFirst, ab_analogLast);
  setCounterLimits(ab_counterFirst, ab_counterLast);
}


Inputs_c::~Inputs_c()
{
}


void
Inputs_c::setDigitalLimits(uint8_t ab_digitalFirst, uint8_t ab_digitalLast){
  // check if input values are correct
  if (
       (ab_digitalFirst <= DIGITAL_INPUT_MAX)
#ifdef DIGITAL_INPUT_MIN_GREATER_ZERO
     &&(ab_digitalFirst >= DIGITAL_INPUT_MIN)
     &&(ab_digitalLast >= DIGITAL_INPUT_MIN)
#endif
     &&(ab_digitalLast <= DIGITAL_INPUT_MAX)
     )
  { // correct range
    setDigitalFirst(ab_digitalFirst);
    setDigitalLast(ab_digitalLast);
  }
  else
  { // wrong range
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Input );
  }
}


void
Inputs_c::setAnalogLimits(uint8_t ab_analogFirst, uint8_t ab_analogLast){
  // check if input values are correct
  if (
       (ab_analogFirst <= ANALOG_INPUT_MAX)
#ifdef ANALOG_INPUT_MIN_GREATER_ZERO
     &&(ab_analogFirst >= ANALOG_INPUT_MIN)
     &&(ab_analogLast >= ANALOG_INPUT_MIN)
#endif
     &&(ab_analogLast <= ANALOG_INPUT_MAX)
     )
  { // correct range
    setAnalogFirst(ab_analogFirst);
    setAnalogLast(ab_analogLast);
  }
  else
  { // wrong range
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Input );
  }
}


void
Inputs_c::setCounterLimits(uint8_t ab_counterFirst, uint8_t ab_counterLast){
  // check if input values are correct
  if (
       (ab_counterFirst <= COUNTER_INPUT_MAX)
#ifdef COUNTER_INPUT_MIN_GREATER_ZERO
     &&(ab_counterFirst >= COUNTER_INPUT_MIN)
     &&(ab_counterLast >= COUNTER_INPUT_MIN)
#endif
     &&(ab_counterLast <= COUNTER_INPUT_MAX)
     )
  { // correct range
    setCounterFirst(ab_counterFirst);
    setCounterLast(ab_counterLast);
  }
  else
  { // wrong range
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Input );
  }
}


void
Inputs_c::registerAccessFlt( void )
{
  getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Input );
  // throw exception if defined to do
  THROW_CONT_EL_NONEXIST
}



} // end of namespace __IsoAgLib
