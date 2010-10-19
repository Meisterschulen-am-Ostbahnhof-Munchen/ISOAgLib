/*
  sensorbase_c.h:
    header file for Sensor_Base_c, a base class for AnalogI_c and
    Digital_I

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SENSOR_BASE_H
#define SENSOR_BASE_H

#include "sensor_c.h"

#include <supplementary_driver/hal/hal_sensor.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  Base Class for sensor input channels;
  holds information for channel number
  delivers some basically input request methods
  @see Analog_I
  @see Digital_I
  @see Sensor_I
  @see Sensor
  @author Dipl.-Inform. Achim Spangler
  */
class SensorBase_c {
public:
  /**
    Basic constructor for an input channel object (only internal accessed)
    @param aui8_channelNr hardware channel of this input object
    @param ren_inputType input type, which is needed because of the relation
           of config number and gathering number which vary dependent on input type
    @see Sensor_c::input_type
  */
  SensorBase_c(uint8_t aui8_channelNr, Sensor_c::inputType_t ren_inputType = Sensor_c::undef_input);
  /**
    Basic constructor for an input channel object (only internal accessed)
    @param aui8_channelNr hardware channel of this input object
    @param ren_inputType input type, which is needed because of the relation
           of config number and gathering number which vary dependent on input type
    @see Sensor_c::input_type
  */
  void init(uint8_t aui8_channelNr, Sensor_c::inputType_t ren_inputType = Sensor_c::undef_input);
  /** basic destructor of sensor input object with no function at the moment (only internal accessed) */
  virtual ~SensorBase_c();
  /**
    deliver the value from the sensor (for digital input: active->1; else->0)
    @return value of the input channel (can be raw sensor value or calculated as configured on creation)
    @see AnalogI_c::val
    @see DigitalI_c::val
  */
  virtual int16_t val() const  = 0;
  /**
    deliver the value from the sensor (for digital input: active->1; else->0)
    @return value of the input channel (can be raw sensor value or calculated as configured on creation)
    @see AnalogI_c::val
    @see DigitalI_c::val
  */
  virtual int32_t valLong();
  /**
    check if the input channel is active (for analog input true if |value| > 0)
    @return true if the active assertion on the input channel is true (dependent on configurationHigh or Low)
    @see AnalogI_c::active
    @see DigitalI_c::active
  */
  virtual bool active() const  = 0;
  /**
    deliver the channel number of the output object
    @return number to use for BIOS access to this channel
  */
  uint8_t channelNr() const { return ui8_channelNr;};
private: // Private attributes
  /** input type of this channel */
  Sensor_c::inputType_t en_inputType;
protected: // Protected methods
  /** channel number of this input */
  uint8_t ui8_channelNr;
};

} // end of namespace __IsoAgLib

#endif


