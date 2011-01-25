/*
  isensori_c.h: header for the sensor input management object

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISENSOR_I_H
#define ISENSOR_I_H

#include "impl/sensori_c.h"
#include "ianalogi_c.h"
#include "idigitali_c.h"
#include "icounteri_c.h"
#include "isensor_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  Hardware dependent object for hardware independent getting of sensor data;
  @see iAnalog_I
  @see iDigital_I
  @author Dipl.-Inform. Achim Spangler
  */
class iSensorI_c : private __IsoAgLib::SensorI_c {
public:
  /**
    initialisation for the sensor input management which sets the allowed number
    ranges for analog,digital,counter input channels.
    As the constructor is often not called for static instances, the init function
    is used by the Singleton base class, to set the unique instance in a well defined
    initial state


    possible errors:
        * Err_c::range given limits are not possible
    @param ab_digitalFirst smallest allowed digital input channel number (DIGITAL_INPUT_MIN)
    @param ab_digitalLast greatest allowed digital input channel number (DIGITAL_INPUT_MAX)
    @param ab_analogFirst smallest allowed analog input channel number (ANALOG_INPUT_MIN)
    @param ab_analogLast greatest allowed analog input channel number (ANALOG_INPUT_MAX)
    @param ab_counterFirst smallest allowed counter input channel number (COUNTER_INPUT_MIN)
    @param ab_counterLast greatest allowed counter input channel number (COUNTER_INPUT_MAX)
    @see masterHeader
  */
  void init(uint8_t ab_digitalFirst = DIGITAL_INPUT_MIN, uint8_t ab_digitalLast = DIGITAL_INPUT_MIN,
            uint8_t ab_analogFirst = ANALOG_INPUT_MIN, uint8_t ab_analogLast = ANALOG_INPUT_MAX,
           uint8_t ab_counterFirst = COUNTER_INPUT_MIN, uint8_t ab_counterLast = COUNTER_INPUT_MAX)
  { init(ab_digitalFirst, ab_digitalLast, ab_analogFirst, ab_analogLast,ab_counterFirst, ab_counterLast); }

  /** close sub-system */
  void close() {}

  /** destructor for the sensor input manager object */
  ~iSensorI_c() {}

  /**
    set the limits for digital input channels (first setting can be done by constructor parameters)

    possible errors:
        * Err_c::range given limits are not possible
    @param ab_digitalFirst number of the smallest allowed digital input channel
    @param ab_digitalLast number of the greatest allowed digital input channel
  */
  void setDigitalLimits(uint8_t ab_digitalFirst, uint8_t ab_digitalLast)
    {SensorI_c::setDigitalLimits(ab_digitalFirst, ab_digitalLast);};

  /**
    set the limits for analog input channels (first setting can be done by constructor parameters)

    possible errors:
        * Err_c::range given limits are not possible
    @param ab_analogFirst number of the smallest allowed analog input channel
    @param ab_analogLast number of the greatest allowed analog input channel
  */
  void setAnalogLimits(uint8_t ab_analogFirst, uint8_t ab_analogLast)
    {SensorI_c::setAnalogLimits(ab_analogFirst, ab_analogLast);};

  /**
    set the limits for counter input channels (first setting can be done by constructor parameters)

    possible errors:
        * Err_c::range given limits are not possible
    @param ab_counterFirst number of the smallest allowed counter input channel
    @param ab_counterLast number of the greatest allowed counter input channel
  */
  void setCounterLimits(uint8_t ab_counterFirst, uint8_t ab_counterLast)
    {SensorI_c::setCounterLimits(ab_counterFirst, ab_counterLast);};

  /**
    check if digital input object to given ab_channel exist
    @see iSensorI_c::createDigital
    @see iSensorI_c::deleteDigital
    @see iSensorI_c::existAnalog
    @see iSensorI_c::existCounter
    @param ab_channel number of the tested input channel
    @return true -> searched input object exist
  */
  bool existDigital(uint8_t ab_channel) {return SensorI_c::existDigital(ab_channel);};
  /**
    check if analog input object to given ab_channel exist
    @see iSensorI_c::createAnalog
    @see iSensorI_c::deleteAnalog
    @see iSensorI_c::existDigital
    @see iSensorI_c::existCounter
    @param ab_channel number of the tested input channel
    @return true -> searched input object exist
  */
  bool existAnalog(uint8_t ab_channel) {return SensorI_c::existAnalog(ab_channel);};
  /**
    check if counter input object to given ab_channel exist
    @see iSensorI_c::createCounter
    @see iSensorI_c::deleteCounter
    @see iSensorI_c::existAnalog
    @see iSensorI_c::existDigital
    @param ab_channel number of the tested input channel
    @return true -> searched input object exist
  */
  bool existCounter(uint8_t ab_channel) {return SensorI_c::existCounter(ab_channel);};
  /**
    deliver reference to requested analog channel object to access this sensor input;
    IMPORTANT: an analog input channel object with the wanted number  must exist
               -> creating with createAnalog and checking with existAnalog
               (throw exception if exceptions are activated on compile time)

    possible errors:
        * Err_c::elNonexistant wanted analog input with given channel no does not exist
    @see iSensorI_c::createAnalog
    @see iSensorI_c::existAnalog
    @see iAnalogI_c::Analog_I
    @see iSensorI_c::counter
    @see iSensorI_c::digital
    @param ab_channel channel of the analog sensor input
    @return reference to the wanted analog sensor input channel
    @exception containerElementNonexistant
  */
  iAnalogI_c& analog(uint8_t ab_channel) {return (iAnalogI_c&)(SensorI_c::analog(ab_channel));};
  /**
    deliver reference to requested digital channel object to access this sensor input;
    IMPORTANT: an digital input channel object with the wanted number must exist
               -> creating with createDigital and checking with existDigital
               (throw exception if exceptions are activated on compile time)

    possible errors:
        * Err_c::elNonexistant wanted digital input with given channel no does not exist
    @see iSensorI_c::createDigital
    @see iSensorI_c::existDigital
    @see iDigitalI_c::iDigital_I
    @see iSensorI_c::analog
    @see iSensorI_c::counter
    @param ab_channel channel of the digital sensor input
    @return reference to the wanted digital sensor input channel
    @exception containerElementNonexistant
  */
  iDigitalI_c& digital(uint8_t ab_channel) {return (iDigitalI_c&)(SensorI_c::digital(ab_channel));};
  /**
    deliver reference to requested counter channel object to access this sensor input;
    IMPORTANT: an counter input channel object with the wanted number  must exist
               -> creating with createCounter and checking with existCounter
               (throw exception if exceptions are activated on compile time)

    possible errors:
        * Err_c::elNonexistant wanted counter input with given channel no does not exist
    @see iSensorI_c::createCounter
    @see iSensorI_c::existCounter
    @see iCounterI_c::Counter_I
    @see iSensorI_c::analog
    @see iSensorI_c::digital
    @param ab_channel channel of the counter sensor input
    @return reference to the wanted counter sensor input channel
    @exception containerElementNonexistant
  */
  iCounterI_c& counter(uint8_t ab_channel) {return (iCounterI_c&)(SensorI_c::counter(ab_channel));};
private:
  /** allow getIsensorInstance() access to shielded base class.
      otherwise __IsoAgLib::getIsensorInstance() wouldn't be accepted by compiler
    */
  friend iSensorI_c& getIsensorInstance( void );
};

/** C-style function, to get access to the unique iSensorI_c singleton instance */
iSensorI_c& getIsensorInstance( void ) { return static_cast<iSensorI_c&>(__IsoAgLib::getSensorInstance());};
}

#endif
