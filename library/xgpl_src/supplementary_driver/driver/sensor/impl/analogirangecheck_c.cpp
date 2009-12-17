/*
  analogirangecheck_c.cpp:
    implementation file for AnalogIRangeCheck_c, an object for analog
    sensor input

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "analogirangecheck_c.h"
// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  internal called constructor which creates a new input channel,initialize the hardware and configures conversion calculation
  (uses BIOS function)
  
  possible errors:
      * iLibErr_c::Range wrong input number
      * iLibErr_c::Precondition wrong input type
  @see SensorI_c::createAnalog
  @see Sensor_c::t_analogType
  @param ab_channel default-argument for the hardware channel of the input
  @param ren_analogType default-argument for choosing Sensor_c::voltage(default) or Sensor_c::current as input type
  @param ab_useMean default-argument for setting the calculation of mean value on true (false as default)
  @param ab_fastAdc default-argument for setting fast ADC method (false as default)
  @param aui16_minValid default min allowed value (min limit of range)
  @param aui16_maxValid default max allowed value (max limit of range)
*/
AnalogIRangeCheck_c::AnalogIRangeCheck_c(uint8_t ab_channel, Sensor_c::analogType_t ren_analogType, bool ab_useMean, bool ab_fastAdc,
  uint16_t aui16_minValid, uint16_t aui16_maxValid )
  : AnalogI_c( ab_channel, ren_analogType, ab_useMean, ab_fastAdc ) {
  setRange( aui16_minValid, aui16_maxValid );
}
/**
  internal called constructor which creates a new input channel,initialize the hardware and configures conversion calculation
  (uses BIOS function)

  possible errors:
      * iLibErr_c::Range wrong input number
      * iLibErr_c::Precondition wrong input type
  @see SensorI_c::createAnalog
  @see Sensor_c::t_analogType
  @param ab_channel default-argument for the hardware channel of the input
  @param ren_analogType default-argument for choosing Sensor_c::voltage(default) or Sensor_c::current as input type
  @param ab_useMean default-argument for setting the calculation of mean value on true (false as default)
  @param ab_fastAdc default-argument for setting fast ADC method (false as default)
  @param aui16_minValid default min allowed value (min limit of range)
  @param aui16_maxValid default max allowed value (max limit of range)
*/
void AnalogIRangeCheck_c::init(uint8_t ab_channel, Sensor_c::analogType_t ren_analogType, bool ab_useMean, bool ab_fastAdc,
  uint16_t aui16_minValid, uint16_t aui16_maxValid )
{
  AnalogI_c::init( ab_channel, ren_analogType, ab_useMean, ab_fastAdc );
  setRange( aui16_minValid, aui16_maxValid );
}
/** destructor which can close the hardware input channel */
AnalogIRangeCheck_c::~AnalogIRangeCheck_c(){
}
/* set range
  @param aui16_minValid default min allowed value (min limit of range)
  @param aui16_maxValid default max allowed value (max limit of range)
*/
void AnalogIRangeCheck_c::setRange( uint16_t aui16_minValid, uint16_t aui16_maxValid ) {
  // store given values
  ui16_minValid = aui16_minValid;
  ui16_maxValid = aui16_maxValid;
}

/** get validate val
  @param rb_tooLow  reference to bool value which is set dependent on ( value < minLimit )
  @param rb_tooHigh reference to bool value which is set dependent on ( value > maxLimit )
  @return sensor value
*/
int16_t AnalogIRangeCheck_c::validatedVal( bool &rb_tooLow, bool &rb_tooHigh ) const {
  const int16_t ci16_tempVal = val();
  rb_tooLow  = ( ci16_tempVal < ui16_minValid );
  rb_tooHigh = ( ci16_tempVal > ui16_maxValid );
  return ci16_tempVal;
}

/** check if sensor value is valid */
bool AnalogIRangeCheck_c::good( void ) const {
 const uint16_t ui16_tempVal = val();
 if ( ( ui16_tempVal >= ui16_minValid )
   && ( ui16_tempVal <= ui16_maxValid ) ) {
   return true;
 }
 else {
   return false;
  }
}

/** check if sensor value is in error state */
bool AnalogIRangeCheck_c::error( void ) const {
  return checkRangeError();
}
/** check if sensor value is out of range */
bool AnalogIRangeCheck_c::checkRangeError( void ) const{
 const uint16_t ui16_tempVal = val();
 if ( ( ui16_tempVal < ui16_minValid )
   || ( ui16_tempVal > ui16_maxValid ) ) {
   return true;
 }
 else {
   return false;
  }
}
/** check if value is too high */
bool AnalogIRangeCheck_c::checkTooHigh( void ) const {
 const uint16_t ui16_tempVal = val();
 if ( ui16_tempVal > ui16_maxValid ) {
   return true;
 }
 else {
   return false;
  }
}
/** check if value is too low */
bool AnalogIRangeCheck_c::checkTooLow( void ) const {
 const uint16_t ui16_tempVal = val();
if ( ui16_tempVal < ui16_minValid ) {
   return true;
 }
 else {
   return false;
  }
}


} // end of namespace __IsoAgLib
