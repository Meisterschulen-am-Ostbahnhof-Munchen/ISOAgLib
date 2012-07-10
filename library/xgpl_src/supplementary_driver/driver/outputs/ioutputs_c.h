/*
  ioutputs.h:
    header for the outputs management object

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IOUTPUTS_C_H
#define IOUTPUTS_C_H

#include "impl/outputs_c.h"
#include "idigitalo_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  Hardware dependent object for hardware independent controling of outputs.
  @see iDigital_O
  @author Dipl.-Inform. Achim Spangler
  */
class iOutputs_c : private __IsoAgLib::Outputs_c {
public:
  /**
    initialisation for the outputs management which sets the allowed number
    ranges for digital ouptut channels.

    possible errors:
        * Err_c::range given limits are not possible
    @param ab_digitalFirst smallest allowed digital output channel number (DIGITAL_OUTPUT_MIN)
    @param ab_digitalLast greatest allowed digital output channel number (DIGITAL_OUTPUT_MAX)
    @see masterHeader
  */
  void init(
      uint8_t ab_digitalFirst = DIGITAL_OUTPUT_MIN,
      uint8_t ab_digitalLast = DIGITAL_OUTPUT_MAX)
  {
    Outputs_c::init(ab_digitalFirst, ab_digitalLast);
  }

  // close sub-system
  void close() {}

  /** control the relay which is responsible for activation of the PWM output */
  void setMainRelais( bool ab_active ) { Outputs_c::setMainRelais( ab_active ); }

  /** destructor for the outputs manager object */
  ~iOutputs_c() {}

  /**
    set the limits for digital output channels (first setting can be done by constructor parameters)

    possible errors:
        * Err_c::range given limits are not possible
    @param ab_digitalFirst number of the smallest allowed digital input channel
    @param ab_digitalLast number of the greatest allowed digital input channel
  */
  void setDigitalLimits(uint8_t ab_digitalFirst, uint8_t ab_digitalLast)
    { Outputs_c::setDigitalLimits(ab_digitalFirst, ab_digitalLast); }

private:
  /** allow getIoutputsInstance() access to shielded base class.
      otherwise __IsoAgLib::getOutputsInstance() wouldn't be accepted by compiler
    */
  friend iOutputs_c& getIoutputsInstance( void );

  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of Outputs_c within application
    */
  iOutputs_c();
};

/** C-style function, to get access to the unique iOutputs_c singleton instance */
inline iOutputs_c& getIoutputsInstance( void ) { return static_cast<iOutputs_c&>(__IsoAgLib::getOutputsInstance());};

}
#endif
