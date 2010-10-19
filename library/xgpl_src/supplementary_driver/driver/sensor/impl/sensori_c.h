/*
  sensori_c.h: header for the sensor input management object

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SENSOR_I_H
#define SENSOR_I_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/container.h>

#include "analogi_c.h"
#include "digitali_c.h"
#include "counteri_c.h"
#include "sensor_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class SensorI_c;
/** C-style function, to get access to the unique SensorI_c singleton instance */
SensorI_c& getSensorInstance( void );

/**
  Hardware dependent object for hardware independent getting of sensor data;
  works with the help of object SensorBase_c and derived classes
  the input from different data sources
  @see Analog_I
  @see Digital_I
  @see SensorBase
  @short Hardware dependent object for hardware independent getting of sensor data.
  @author Dipl.-Inform. Achim Spangler
  */
class SensorI_c : public SingletonDerived<SensorI_c,ClientBase> {
public:
  /**
    initialisation for the sensor input management which sets the allowed number
    ranges for analog,digital,counter input channels.
    As the constructor is often not called for static instances, the init function
    is used by the Singleton base class, to set the unique instance in a well defined
    initial state


    possible errors:
        * iLibErr_c::Range given limits are not possible
    @param ab_digitalFirst smallest allowed digital input channel number (DIGITAL_INPUT_MIN)
    @param ab_digitalLast greatest allowed digital input channel number (DIGITAL_INPUT_MAX)
    @param ab_analogFirst smallest allowed analog input channel number (ANALOG_INPUT_MIN)
    @param ab_analogLast greatest allowed analog input channel number (ANALOG_INPUT_MAX)
    @param ab_counterFirst smallest allowed counter input channel number (COUNTER_INPUT_MIN)
    @param ab_counterLast greatest allowed counter input channel number (COUNTER_INPUT_MAX)
    @see isoaglib_config.h
  */
  void init(uint8_t ab_digitalFirst = DIGITAL_INPUT_MIN, uint8_t ab_digitalLast = DIGITAL_INPUT_MAX,
            uint8_t ab_analogFirst = ANALOG_INPUT_MIN,   uint8_t ab_analogLast = ANALOG_INPUT_MAX,
            uint8_t ab_counterFirst = COUNTER_INPUT_MIN, uint8_t ab_counterLast = COUNTER_INPUT_MAX);

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void ){}

  /** destructor for the sensor input manager object */
  ~SensorI_c();
  /**
    set the limits for digital input channels (first setting can be done by constructor parameters)

    possible errors:
        * iLibErr_c::Range given limits are not possible
    @param ab_digitalFirst number of the smallest allowed digital input channel
    @param ab_digitalLast number of the greatest allowed digital input channel
  */
  void setDigitalLimits(uint8_t ab_digitalFirst, uint8_t ab_digitalLast);
  /**
    set the limits for analog input channels (first setting can be done by constructor parameters)

    possible errors:
        * iLibErr_c::Range given limits are not possible
    @param ab_analogFirst number of the smallest allowed analog input channel
    @param ab_analogLast number of the greatest allowed analog input channel
  */
  void setAnalogLimits(uint8_t ab_analogFirst, uint8_t ab_analogLast);
  /**
    set the limits for counter input channels (first setting can be done by constructor parameters)

    possible errors:
        * iLibErr_c::Range given limits are not possible
    @param ab_counterFirst number of the smallest allowed counter input channel
    @param ab_counterLast number of the greatest allowed counter input channel
  */
  void setCounterLimits(uint8_t ab_counterFirst, uint8_t ab_counterLast);
    /** handler function for access to undefined client.
    * the base Singleton calls this function, if it detects an error
    */
  void registerAccessFlt( void );

  /**
    check if analog input object to given ab_channel exist
    @see SensorI_c::createAnalog
    @see SensorI_c::deleteAnalog
    @see SensorI_c::existDigital
    @see SensorI_c::existCounter
    @param ab_channel number of the tested input channel
    @return true -> searched input object exist
  */
  bool existAnalog(uint8_t ab_channel){ return existC1( ab_channel );};
  /**
    check if digital input object to given ab_channel exist
    @see SensorI_c::createDigital
    @see SensorI_c::deleteDigital
    @see SensorI_c::existAnalog
    @see SensorI_c::existCounter
    @param ab_channel number of the tested input channel
    @return true -> searched input object exist
  */
  bool existDigital(uint8_t ab_channel){ return existC2( ab_channel );};
  /**
    check if counter input object to given ab_channel exist
    @see SensorI_c::createCounter
    @see SensorI_c::deleteCounter
    @see SensorI_c::existDigital
    @see SensorI_c::existAnalog
    @param ab_channel number of the tested input channel
    @return true -> searched input object exist
  */
  bool existCounter(uint8_t ab_channel){ return existC3( ab_channel );};
  /**
    deliver reference to requested analog channel object to access this sensor input;
    IMPORTANT: an analog input channel object with the wanted number  must exist
               -> creating with createAnalog and checking with existAnalog
               (throw exception if exceptions are activated on compile time)

    possible errors:
        * iLibErr_c::elNonexistant wanted analog input with given channel no does not exist
    @see SensorI_c::createAnalog
    @see SensorI_c::existAnalog
    @see SensorI_c::digital
    @see SensorI_c::counter
    @see AnalogI_c::Analog_I
    @param ab_channel channel of the analog sensor input
    @return reference to the wanted analog sensor input channel
    @exception containerElementNonexistant
  */
  AnalogI_c& analog(uint8_t ab_channel) { return getC1( ab_channel );};
  /**
    deliver reference to requested digital channel object to access this sensor input;
    IMPORTANT: an digital input channel object with the wanted number must exist
               -> creating with createDigital and checking with existDigital
               (throw exception if exceptions are activated on compile time)

    possible errors:
        * iLibErr_c::elNonexistant wanted digital input with given channel no does not exist
    @see SensorI_c::createDigital
    @see SensorI_c::existDigital
    @see DigitalI_c::Digital_I
    @see SensorI_c::analog
    @see SensorI_c::counter
    @param ab_channel channel of the digital sensor input
    @return reference to the wanted digital sensor input channel
    @exception containerElementNonexistant
  */
  DigitalI_c& digital(uint8_t ab_channel) { return getC2( ab_channel );};
  /**
    deliver reference to requested counter channel object to access this sensor input;
    IMPORTANT: an counter input channel object with the wanted number must exist
               -> creating with createCounter and checking with existCounter
               (throw exception if exceptions are activated on compile time)

    possible errors:
        * iLibErr_c::elNonexistant wanted counter input with given channel no does not exist
    @see SensorI_c::createCounter
    @see SensorI_c::existCounter
    @see CounterI_c::Counter_I
    @see SensorI_c::digital
    @see SensorI_c::analog
    @param ab_channel channel of the counter sensor input
    @return reference to the wanted counter sensor input channel
    @exception containerElementNonexistant
  */
  CounterI_c& counter(uint8_t ab_channel) { return getC3( ab_channel );};
private:
//private methods
  friend class AnalogI_c;
  friend class DigitalI_c;
  friend class CounterI_c;
  friend class SingletonDerived<SensorI_c,ClientBase>;
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of SensorI_c within application
    */
  SensorI_c( void )
    : CONTAINER_CLIENT1_CTOR_INITIALIZER_LIST
    , CONTAINER_CLIENT2_CTOR_INITIALIZER_LIST
    , CONTAINER_CLIENT3_CTOR_INITIALIZER_LIST
  { }

  /** register a pointer to an external analog input object
    * @param apc_object const pointer to new AnalogI_c instance,
    *        which should be registered in the vector
    * @return true -> instance is now in vector, false -> memory error
    */
  void registerClient( AnalogI_c* apc_object ) { registerC1( apc_object );};
  /** register a pointer to an external digital input object
    * @param apc_object const pointer to new DigitalI_c instance,
    *        which should be registered in the vector
    * @return true -> instance is now in vector, false -> memory error
    */
  void registerClient( DigitalI_c* apc_object ) { registerC2( apc_object );};
  /** register a pointer to an external counter input object
    * @param apc_object const pointer to new CounterI_c instance,
    *        which should be registered in the vector
    * @return true -> instance is now in vector, false -> memory error
    */
  void registerClient( CounterI_c* apc_object ) { registerC3( apc_object );};

  /** unregister a pointer to an external analog input object
    * @param apc_object const pointer to AnalogI_c instance,
    *        which is deconstructed, so that pointer to it shall be deleted from
    *        the vector
    * @return true -> pointer to given instance WAS in vector, and is now deleted
    */
  void unregisterClient( AnalogI_c* apc_object ) { unregisterC1( apc_object );};
  /** unregister a pointer to an external digital input object
    * @param apc_object const pointer to DigitalI_c instance,
    *        which is deconstructed, so that pointer to it shall be deleted from
    *        the vector
    * @return true -> pointer to given instance WAS in vector, and is now deleted
    */
  void unregisterClient( DigitalI_c* apc_object ) { unregisterC2( apc_object );};
  /** unregister a pointer to an external counter input object
    * @param apc_object const pointer to CounterI_c instance,
    *        which is deconstructed, so that pointer to it shall be deleted from
    *        the vector
    * @return true -> pointer to given instance WAS in vector, and is now deleted
    */
  void unregisterClient( CounterI_c* apc_object ) { unregisterC3( apc_object );};
  /**
    deliver number of the first analog channel
    @return number of the first analog channel
  */
  uint8_t analogFirst(){return b_min_analog;};
  /**
    deliver number of the last analog channel
    @return number of the last analog channel
  */
  uint8_t analogLast(){return b_maxAnalog;};
  /**
    set number of the first analog channel
    @param ab_val wanted first analog channel
  */
  void setAnalogFirst(uint8_t ab_val){b_min_analog = ab_val;};
  /**
    set number of the last analog channel
    @param ab_val wanted last analog channel
  */
  void setAnalogLast(uint8_t ab_val){b_maxAnalog = ab_val;};
  /**
    deliver number of the first digital channel
    @return number of the first digital channel
  */
  uint8_t digitalFirst(){return b_min_digital;};
  /**
    deliver number of the last digital channel
    @return number of the last digital channel
  */
  uint8_t digitalLast(){return b_maxDigital;};
  /**
    set number of the first digital channel
    @param ab_val wanted first digital channel
  */
  void setDigitalFirst(uint8_t ab_val){b_min_digital = ab_val;};
  /**
    set number of the last digital channel
    @param ab_val wanted last digital channel
  */
  void setDigitalLast(uint8_t ab_val){b_maxDigital = ab_val;};
  /**
    deliver number of the first counter channel
    @return number of the first counter channel
  */
  uint8_t counterFirst(){return b_min_counter;};
  /**
    deliver number of the last counter channel
    @return number of the last counter channel
  */
  uint8_t counterLast(){return b_maxCounter;};
  /**
    set number of the first counter channel
    @param ab_val wanted first counter channel
  */
  void setCounterFirst(uint8_t ab_val){b_min_counter = ab_val;};
  /**
    set number of the last counter channel
    @param ab_val wanted last counter channel
  */
  void setCounterLast(uint8_t ab_val){b_maxCounter = ab_val;};

private:
// Private attributes
  /** min analog input */
  uint8_t b_min_analog;
  /** max analog input */
  uint8_t b_maxAnalog;
  /** min counter input */
  uint8_t b_min_counter;
  /** max counter input */
  uint8_t b_maxCounter;
  /** min digital input */
  uint8_t b_min_digital;
  /** max digital input */
  uint8_t b_maxDigital;

  CONTAINER_CLIENT1_MEMBER_FUNCTIONS_MAIN(AnalogI_c);
  CONTAINER_CLIENT1_MEMBER_FUNCTIONS_KEY(AnalogI_c,uint8_t);

  CONTAINER_CLIENT2_MEMBER_FUNCTIONS_MAIN(DigitalI_c);
  CONTAINER_CLIENT2_MEMBER_FUNCTIONS_KEY(DigitalI_c,uint8_t);

  CONTAINER_CLIENT3_MEMBER_FUNCTIONS_MAIN(CounterI_c);
  CONTAINER_CLIENT3_MEMBER_FUNCTIONS_KEY(CounterI_c,uint8_t);
};
}

#endif
