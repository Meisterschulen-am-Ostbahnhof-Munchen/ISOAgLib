/***************************************************************************
                          isensor_i.h  -  header for the sensor input management object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/
#ifndef ISENSOR_I_H
#define ISENSOR_I_H

#include "impl/sensor_i.h"
#include "ianalog_i.h"
#include "idigital_i.h"
#include "icounter_i.h"
#include "isensor.h"

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
    @param rb_digitalFirst smallest allowed digital input channel number (DIGITAL_INPUT_MIN)
    @param rb_digitalLast greatest allowed digital input channel number (DIGITAL_INPUT_MAX)
    @param rb_analogFirst smallest allowed analog input channel number (ANALOG_INPUT_MIN)
    @param rb_analogLast greatest allowed analog input channel number (ANALOG_INPUT_MAX)
    @param rb_counterFirst smallest allowed counter input channel number (COUNTER_INPUT_MIN)
    @param rb_counterLast greatest allowed counter input channel number (COUNTER_INPUT_MAX)
    @see masterHeader
  */
  init(uint8_t rb_digitalFirst = DIGITAL_INPUT_MIN, uint8_t rb_digitalLast = DIGITAL_INPUT_MIN,
            uint8_t rb_analogFirst = ANALOG_INPUT_MIN, uint8_t rb_analogLast = ANALOG_INPUT_MAX,
           uint8_t rb_counterFirst = COUNTER_INPUT_MIN, uint8_t rb_counterLast = COUNTER_INPUT_MAX)
  { init(rb_digitalFirst, rb_digitalLast, rb_analogFirst, rb_analogLast,rb_counterFirst, rb_counterLast);};

  /** destructor for the sensor input manager object */
  ~iSensorI_c() {};

  /**
    set the limits for digital input channels (first setting can be done by constructor parameters)

    possible errors:
        * Err_c::range given limits are not possible
    @param rb_digitalFirst number of the smallest allowed digital input channel
    @param rb_digitalLast number of the greatest allowed digital input channel
  */
  void setDigitalLimits(uint8_t rb_digitalFirst, uint8_t rb_digitalLast)
    {SensorI_c::setDigitalLimits(rb_digitalFirst, rb_digitalLast);};

  /**
    set the limits for analog input channels (first setting can be done by constructor parameters)

    possible errors:
        * Err_c::range given limits are not possible
    @param rb_analogFirst number of the smallest allowed analog input channel
    @param rb_analogLast number of the greatest allowed analog input channel
  */
  void setAnalogLimits(uint8_t rb_analogFirst, uint8_t rb_analogLast)
    {SensorI_c::setAnalogLimits(rb_analogFirst, rb_analogLast);};

  /**
    set the limits for counter input channels (first setting can be done by constructor parameters)

    possible errors:
        * Err_c::range given limits are not possible
    @param rb_counterFirst number of the smallest allowed counter input channel
    @param rb_counterLast number of the greatest allowed counter input channel
  */
  void setCounterLimits(uint8_t rb_counterFirst, uint8_t rb_counterLast)
    {SensorI_c::setCounterLimits(rb_counterFirst, rb_counterLast);};

  /**
    check if digital input object to given rb_channel exist
    @see iSensorI_c::createDigital
    @see iSensorI_c::deleteDigital
    @see iSensorI_c::existAnalog
    @see iSensorI_c::existCounter
    @param rb_channel number of the tested input channel
    @return true -> searched input object exist
  */
  bool existDigital(uint8_t rb_channel) {return SensorI_c::existDigital(rb_channel);};
  /**
    check if analog input object to given rb_channel exist
    @see iSensorI_c::createAnalog
    @see iSensorI_c::deleteAnalog
    @see iSensorI_c::existDigital
    @see iSensorI_c::existCounter
    @param rb_channel number of the tested input channel
    @return true -> searched input object exist
  */
  bool existAnalog(uint8_t rb_channel) {return SensorI_c::existAnalog(rb_channel);};
  /**
    check if counter input object to given rb_channel exist
    @see iSensorI_c::createCounter
    @see iSensorI_c::deleteCounter
    @see iSensorI_c::existAnalog
    @see iSensorI_c::existDigital
    @param rb_channel number of the tested input channel
    @return true -> searched input object exist
  */
  bool existCounter(uint8_t rb_channel) {return SensorI_c::existCounter(rb_channel);};
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
    @param rb_channel channel of the analog sensor input
    @return reference to the wanted analog sensor input channel
    @exception containerElementNonexistant
  */
  iAnalogI_c& analog(uint8_t rb_channel) {return static_cast<iAnalogI_c&>(SensorI_c::analog(rb_channel));};
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
    @param rb_channel channel of the digital sensor input
    @return reference to the wanted digital sensor input channel
    @exception containerElementNonexistant
  */
  iDigitalI_c& digital(uint8_t rb_channel) {return static_cast<iDigitalI_c&>(SensorI_c::digital(rb_channel));};
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
    @param rb_channel channel of the counter sensor input
    @return reference to the wanted counter sensor input channel
    @exception containerElementNonexistant
  */
  iCounterI_c& counter(uint8_t rb_channel) {return static_cast<iCounterI_c&>(SensorI_c::counter(rb_channel));};
private:
  /** allow getIsensorInstance() access to shielded base class.
      otherwise __IsoAgLib::getIsensorInstance() wouldn't be accepted by compiler
    */
  friend iSensorI_c& getIsensorInstance( void );
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of iSensorI_c within application
    */
  iSensorI_c( void ) : SensorI_c() {};
};

/** C-style function, to get access to the unique iSensorI_c singleton instance */
iSensorI_c& getIsensorInstance( void ) { return static_cast<iSensorI_c&>(__IsoAgLib::getSensorInstance());};
}

#endif
