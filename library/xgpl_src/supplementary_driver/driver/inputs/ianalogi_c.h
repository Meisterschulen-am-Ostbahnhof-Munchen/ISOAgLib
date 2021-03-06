/*
  ianalogi_c.h:
    interface header file for iAnalogI_c, an object for analog input

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IANALOGI_C_H
#define IANALOGI_C_H

#include "impl/analogi_c.h"


namespace IsoAgLib {

/**
  Interface class for Analog input objects
  @author Dipl.-Inform. Achim Spangler
*/
class iAnalogI_c : private __IsoAgLib::AnalogI_c
{
public:
  /** enum for error states of analog input
    * values: noAinErr, ain_openErr, ain_shortcutErr,
    *         ain_overvoltErr, ain_undervoltErr
    */
  typedef enum { noAinErr, ain_openErr, ain_shortcutErr, ain_overvoltErr, ain_undervoltErr } ain_err_t;

  /**
    internal called constructor which creates a new input channel,initialize the hardware and configures conversion calculation
    @param ab_channel default-argument for the hardware channel of the input
    @param ren_analogType default-argument for choosing voltage(default) or current as input type
    @param ab_useMean default-argument for setting the calculation of mean value on true (false as default)
    @param ab_fastAdc default-argument for setting fast ADC method (false as default)
  */
  iAnalogI_c(uint8_t ab_channel = 0xFF, IsoAgLib::iInput_c::analogType_t ren_analogType = IsoAgLib::iInput_c::voltage, bool ab_useMean = false, bool ab_fastAdc = false )
    : AnalogI_c(ab_channel, ren_analogType, ab_useMean, ab_fastAdc ) {}

  /**
    internal called constructor which creates a new input channel,initialize the hardware and configures conversion calculation
    @param ab_channel default-argument for the hardware channel of the input
    @param ren_analogType default-argument for choosing voltage(default) or current as input type
    @param ab_useMean default-argument for setting the calculation of mean value on true (false as default)
    @param ab_fastAdc default-argument for setting fast ADC method (false as default)
  */
  void init(uint8_t ab_channel, IsoAgLib::iInput_c::analogType_t ren_analogType = IsoAgLib::iInput_c::voltage, bool ab_useMean = false, bool ab_fastAdc = false )
    { AnalogI_c::init(ab_channel, ren_analogType, ab_useMean, ab_fastAdc );}

  virtual ~iAnalogI_c() {}

  /**
    get the actual input value with the configured linear conversion (use the configured ADC method)
    @return input value: A) Volt [mV], or B) Ampere [mA]
  */
  uint16_t val() const {return AnalogI_c::val();}

  /**
    check if value is greater than 0
    @return true if input value is different from 0, otherwise 0
  */
  bool active() const {return AnalogI_c::active();}

  /**
    configure fast ADC gathering
    @param ab_useFast default-argument for setting fast ADC (true as default)
  */
  void setFastAdc(bool ab_useFast=true){AnalogI_c::setFastAdc(ab_useFast);}

  /**
    deliver the channel number of the output object
    @return number to use for BIOS access to this channel
  */
  uint8_t channelNr() const { return AnalogI_c::channelNr();}

  /** deliver detailed error state information for this Analog Input
    * @return ain_err_t [noAinErr|ain_openErr|ain_shortcutErr|ain_overvoltErr|ain_undervoltErr]
    */
  iAnalogI_c::ain_err_t getState( void ) const { return iAnalogI_c::ain_err_t((uint16_t)AnalogI_c::getState());}

  /**
    set the min and max override range and activate override range errors in case of valid min/max (max >= min)
    @param aui16_minValue minimum range value
    @param aui16_maxValue maximum range value
  */
  void setOverrideRangeValues(uint16_t aui16_minRangeValue, uint16_t aui16_maxRangeValue) { AnalogI_c::setOverrideRangeValues(aui16_minRangeValue, aui16_maxRangeValue); }

private:
  /**
    HIDDEN! copy constructor for AnalogI_c
    NEVER copy a AnalogI_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  iAnalogI_c(const iAnalogI_c &);

  /**
    HIDDEN! assignment for AnalogI_c
    NEVER assign a AnalogI_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  iAnalogI_c& operator=(const iAnalogI_c &);
};

} // IsoAgLib

#endif
