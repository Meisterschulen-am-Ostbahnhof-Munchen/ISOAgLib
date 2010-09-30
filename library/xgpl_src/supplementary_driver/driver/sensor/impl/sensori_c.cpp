/*
  sensori_c.cpp: implmentation for the sensor input management

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "sensori_c.h"
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** C-style function, to get access to the unique SensorI_c singleton instance */
SensorI_c& getSensorInstance( void ) { return SensorI_c::instance();};


void
SensorI_c::init(
  uint8_t ab_digitalFirst, uint8_t ab_digitalLast,
  uint8_t ab_analogFirst,  uint8_t ab_analogLast,
  uint8_t ab_counterFirst, uint8_t ab_counterLast)
{ // store the channel limits for dig and analog
  setDigitalLimits(ab_digitalFirst, ab_digitalLast);
  setAnalogLimits(ab_analogFirst, ab_analogLast);
  setCounterLimits(ab_counterFirst, ab_counterLast);
}

/** destructor for the sensor input manager object */
SensorI_c::~SensorI_c(){
}

/**
  set the limits for digital input channels (first setting can be done by constructor parameters)
  possible errors:
      * iLibErr_c::Range given limits are not possible
  @param ab_digitalFirst number of the smallest allowed digital input channel
  @param ab_digitalLast number of the greatest allowed digital input channel
*/
void SensorI_c::setDigitalLimits(uint8_t ab_digitalFirst, uint8_t ab_digitalLast){
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
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
  }
}

/**
  set the limits for analog input channels (first setting can be done by constructor parameters)

  possible errors:
      * iLibErr_c::Range given limits are not possible
  @param ab_analogFirst number of the smallest allowed analog input channel
  @param ab_analogLast number of the greatest allowed analog input channel
*/
void SensorI_c::setAnalogLimits(uint8_t ab_analogFirst, uint8_t ab_analogLast){
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
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
  }
}

/**
  set the limits for counter input channels (first setting can be done by constructor parameters)

  possible errors:
      * iLibErr_c::Range given limits are not possible
  @param ab_counterFirst number of the smallest allowed counter input channel
  @param ab_counterLast number of the greatest allowed counter input channel
*/
void SensorI_c::setCounterLimits(uint8_t ab_counterFirst, uint8_t ab_counterLast){
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
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
  }
}
/** handler function for access to undefined client.
  * the base Singleton calls this function, if it detects an error
  */
void SensorI_c::registerAccessFlt( void )
{
  getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Sensor );
  // throw exception if defined to do
  THROW_CONT_EL_NONEXIST
}



} // end of namespace __IsoAgLib
