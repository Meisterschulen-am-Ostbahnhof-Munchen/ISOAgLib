/*
  ioutputs.h:
    header for the outputs management object

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

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

  /**
    check if digital output object to given ab_channel exist
    @see iOutputs_c::createDigital
    @see iOutputs_c::deleteDigital
    @param ab_channel number of the tested input channel
    @return true -> searched input object exist
  */
  bool existDigital(uint8_t ab_channel)
    { return Outputs_c::existDigital(ab_channel); }

  /**
    deliver reference to requested digital channel object to access this output;
    IMPORTANT: an digital output channel object with the wanted number must exist
               -> creating with createDigital and checking with existDigital
               (throw exception if exceptions are activated on compile time)

    possible errors:
        * Err_c::elNonexistant wanted digital input with given channel no does not exist
    @see iOutputs_c::createDigital
    @see iOutputs_c::existDigital
    @see iDigitalO_c::iDigital_O
    @param ab_channel channel of the digital input
    @return reference to the wanted digital output channel
    @exception containerElementNonexistant
  */
  iDigitalO_c& digital(uint8_t ab_channel)
    { return static_cast<iDigitalO_c&>(Outputs_c::digital(ab_channel)); }

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
