/*
  outputs_c.h:
    header for the outputs management object

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef OUTPUTS_C_H
#define OUTPUTS_C_H

#include <IsoAgLib/util/impl/singleton.h>

#include "digitalo_c.h"


namespace __IsoAgLib {

/**
  Hardware dependent object for hardware independent getting of output data;
  @see DigitalO_c
  @see OutputBase_c
  */
class Outputs_c
{
public:
  /** destructor for the outputs manager object */
  ~Outputs_c();

  /** control the relay which is responsible for activation of the PWM output */
  void setMainRelais( bool ab_active ); 

  /**
    initialisation for the outputs management which sets the allowed number
    ranges for digital ouptut channels.
    As the constructor is often not called for static instances, the init function
    is used by the Singleton base class, to set the unique instance in a well defined
    initial state

    possible errors:
        * Err_c::range given limits are not possible
    @param ab_digitalFirst smallest allowed digital output channel number (DIGITAL_OUTPUT_MIN)
    @param ab_digitalLast greatest allowed digital output channel number (DIGITAL_OUTPUT_MAX)
    @see masterHeader
  */
  void init(uint8_t ab_digitalFirst = DIGITAL_OUTPUT_MIN, uint8_t ab_digitalLast = DIGITAL_OUTPUT_MAX);

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close() {}
  
  /**
    set the limits for digital output channels (first setting can be done by constructor parameters)

    possible errors:
        * Err_c::range given limits are not possible
    @param ab_digitalFirst number of the smallest allowed digital output channel
    @param ab_digitalLast number of the greatest allowed digital output channel
  */
  void setDigitalLimits(uint8_t ab_digitalFirst, uint8_t ab_digitalLast);

private:
  friend class DigitalO_c;
    /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of Outputs_c within application
    */
  Outputs_c() { }

  /**
    deliver min channel no limit
    @return min channel no limit
  */
  uint8_t min_digitalLimit() const {return b_min_digitalLimit;};
  /**
    deliver max channel no limit
    @return max channel no limit
  */
  uint8_t maxDigitalLimit() const {return b_maxDigitalLimit;};
  /**
    set min channel no limit
    @param ab_minLimit min channel no limit
  */
  void setMinDigitalLimit(uint8_t ab_minLimit) {b_min_digitalLimit = ab_minLimit;};
  /**
    set max channel no limit
    @param ab_maxLimit max channel no limit
  */
  void setMaxDigitalLimit(uint8_t ab_maxLimit) {b_maxDigitalLimit = ab_maxLimit;};

private: // Private attributes
  /** min digital channel limit */
  uint8_t b_min_digitalLimit;
  /** max digital channel limit */
  uint8_t b_maxDigitalLimit;

  friend Outputs_c &getOutputsInstance();
};

/** C-style function, to get access to the unique Outputs_c singleton instance */
Outputs_c &getOutputsInstance();

}
#endif
