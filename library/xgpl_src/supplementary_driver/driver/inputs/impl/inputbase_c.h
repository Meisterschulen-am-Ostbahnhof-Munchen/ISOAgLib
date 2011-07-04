/*
  inputbase_c.h:
    header file for InputBase_c, a base class for AnalogI_c,
    DigitalI_c and CounterI_c

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef INPUTBASE_C_H
#define INPUTBASE_C_H

#include <supplementary_driver/hal/hal_inputs.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  Base Class for input channels;
  holds information for channel number
  delivers some basically input request methods
  @see AnalogI_c
  @see DigitalI_c
  @see SensorI_c
  @author Dipl.-Inform. Achim Spangler
  */
class InputBase_c {
public:
  /** enum type input type of analog: voltage or current (values equivalent to BIOS) */
  enum analogType_t {voltage = 0, current = 1};

  /** enum type for state of an sensor input */
  enum state_t {good = 0, error = 1};

  /** enum type for mapping input HI/LO to on/off */
  enum onoff_t {OnHigh = 1, OnLow = 0};

  /** enum type for the different input type object: at the moment digital and analog */
  enum inputType_t {undef_input = 0, digital = 1, analog = 2, counter = 3};

  /**
    Basic constructor for an input channel object (only internal accessed)
    @param aui8_channelNr hardware channel of this input object
    @param ren_inputType input type, which is needed because of the relation
           of config number and gathering number which vary dependent on input type
    @see input_type
  */
  InputBase_c(uint8_t aui8_channelNr, inputType_t ren_inputType = undef_input);

  /**
    Basic constructor for an input channel object (only internal accessed)
    @param aui8_channelNr hardware channel of this input object
    @param ren_inputType input type, which is needed because of the relation
           of config number and gathering number which vary dependent on input type
    @see input_type
  */
  void init(uint8_t aui8_channelNr, inputType_t ren_inputType = undef_input);

  /** basic destructor of sensor input object with no function at the moment (only internal accessed) */
  virtual ~InputBase_c();

  /**
    deliver the value from the sensor (for digital input: active->1; else->0)
    @return value of the input channel (can be raw sensor value or calculated as configured on creation)
    @see AnalogI_c::val
    @see DigitalI_c::val
  */
  virtual uint16_t val() const  = 0;

  /**
    deliver the value from the sensor (for digital input: active->1; else->0)
    @return value of the input channel (can be raw sensor value or calculated as configured on creation)
    @see AnalogI_c::val
    @see DigitalI_c::val
  */
  virtual uint32_t valLong();

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
  uint8_t channelNr() const { return ui8_channelNr;}

private:
  /** input type of this channel */
  inputType_t en_inputType;

protected:
  /** channel number of this input */
  uint8_t ui8_channelNr;
};

} // end of namespace __IsoAgLib

#endif


