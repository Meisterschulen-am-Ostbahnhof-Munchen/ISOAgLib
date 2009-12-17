/*
  analogi_c.cpp:
    implementation file for AnalogI_c, an object for analog sensor
    input

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "analogi_c.h"
#include "sensori_c.h"
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/util/config.h>
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
*/
AnalogI_c::AnalogI_c(uint8_t ab_channel, Sensor_c::analogType_t ren_analogType, bool ab_useMean, bool ab_fastAdc )
  : SensorBase_c(ab_channel, Sensor_c::analog) {
  if ( ab_channel != 0xFF ) init( ab_channel, ren_analogType, ab_useMean, ab_fastAdc );
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
*/
void AnalogI_c::init(uint8_t ab_channel, Sensor_c::analogType_t ren_analogType, bool ab_useMean, bool ab_fastAdc)
{
  SensorBase_c::init(ab_channel, Sensor_c::analog);
  // store given values
  en_analogType = ren_analogType;
  b_useMean = ab_useMean;
  b_fastAdc = ab_fastAdc;

  // now configure the input with BIOS call
  int16_t iResult = ( ren_analogType == Sensor_c::voltage )
                 ?HAL::init_analoginVolt(channelNr()):HAL::init_analoginCurrent(channelNr());
  switch (iResult)
  {
    case HAL_NO_ERR:
      // everything o.k. -> set fast analogin if wanted
      if (ab_fastAdc) HAL::setFastAnalogin(true);
      // now register the valid new analog input into SensorI_c
      getSensorInstance().registerClient( this );
      break;
    case HAL_RANGE_ERR:
      // wrong input channel number
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
      break;
    case HAL_CONFIG_ERR:
      // wrong input type
      getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::Sensor );
      break;
  }
}
/** destructor which can close the hardware input channel */
AnalogI_c::~AnalogI_c(){
  // unregister element from SensorI_c
  getSensorInstance().unregisterClient( this );
}
/**
  get the actual sensor value with the configured linear conversion (use the configured ADC method)
  (uses BIOS function)

  @return sensor value: A) Volt [mV], or B) Ampere [mA]
*/
int16_t AnalogI_c::val()const{
  int16_t i16_sensor;
  if (b_useMean)
  { // deliver the mean value
    if ( en_analogType == Sensor_c::voltage ) i16_sensor =  HAL::getAdcMeanVoltage(channelNr());
    else i16_sensor =  HAL::getAdcMeanCurrent(channelNr());
  }
  else
  { // use normal value
    if ( en_analogType == Sensor_c::voltage ) i16_sensor =  HAL::getAdcVoltage(channelNr());
    else i16_sensor =  HAL::getAdcCurrent(channelNr());
  }
  // check if error occured
  if (i16_sensor < 0)
  {
    if (i16_sensor == HAL_RANGE_ERR)
    { // error during measure -> wrong input channel no
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
      // return error code for 16bit sensor values
      return ERROR_VAL_16S;
    }
  }
  return i16_sensor;
}
/**
  configure fast ADC gathering
  (uses BIOS function)
  @param ab_useFast default-argument for setting fast ADC (true as default)
*/
void AnalogI_c::setFastAdc(bool ab_useFast){
  b_fastAdc = ab_useFast;
  // configure the BIOS
  HAL::setFastAnalogin(ab_useFast);
}
/**
  check if value is greater than 0
  (uses BIOS function)

  possible errors:
      * iLibErr_c::Range wrong input number
  @return true if sensor value is different from 0, otherwise 0
*/
bool AnalogI_c::active() const {
  int16_t i16_val = val();
  if (i16_val != ERROR_VAL_16S)
  { // no range error
    return (i16_val > 0);
  }
  return false;
}

} // end of namespace __IsoAgLib
