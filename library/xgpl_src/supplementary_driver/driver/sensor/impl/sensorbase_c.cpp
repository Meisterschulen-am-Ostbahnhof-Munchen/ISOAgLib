/*
  sensorbase_c.cpp:
    implementation file for SensorBase_c, a base class for AnalogI_c
    and Digital_I

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "sensorbase_c.h"
#include "sensori_c.h"
// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


/**
  Basic constructor for a input channel object (only internal accessed)
  @param aui8_channelNr hardware channel of this input object
  @param ren_inputType input type, which is needed because of the relation
         of config number and gathering number which vary dependent on input type
  @see Sensor_c::input_type
*/
SensorBase_c::SensorBase_c(uint8_t aui8_channelNr, Sensor_c::inputType_t ren_inputType){
  // store the channel number (software side
  ui8_channelNr = aui8_channelNr;
  en_inputType = ren_inputType;
}
/**
  Basic constructor for an input channel object (only internal accessed)
  @param aui8_channelNr hardware channel of this input object
  @param ren_inputType input type, which is needed because of the relation
         of config number and gathering number which vary dependent on input type
  @see Sensor_c::input_type
*/
void SensorBase_c::init(uint8_t aui8_channelNr, Sensor_c::inputType_t ren_inputType )
{ // store the channel number (software side
  ui8_channelNr = aui8_channelNr;
  en_inputType = ren_inputType;
}
/** basic destructor of sensor input object with no function at the moment (only internal accessed) */
SensorBase_c::~SensorBase_c(){
}
/**
  deliver the value from the sensor (for digital input: active->1; else->0)
  @return value of the input channel (can be raw sensor value or calculated as configured on creation)
  @see AnalogI_c::val
  @see DigitalI_c::val
*/
int32_t SensorBase_c::valLong()
{ // as far as not overloaded otherwise answer simply val()
  return val();
}


} // end of namespace __IsoAgLib
