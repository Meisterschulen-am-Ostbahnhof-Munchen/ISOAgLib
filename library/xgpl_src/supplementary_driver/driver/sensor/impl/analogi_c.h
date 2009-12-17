/*
  analogi_c.h:
    header file for Analog_I_c, an object for analog sensor input

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ANALOG_I_H
#define ANALOG_I_H

#include "sensorbase_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  Input object for analog sensors;
  has options like automatic calculation of linear
  y = (mMult/mDiv*x+t) for easy evaluating the sensor values;
  can use fast or slow ADC method by config setting
  @see SensorI_c
  @see DigitalI_c
  @see SensorBase_c
  @short AnalogI_c serves an easy interface to sensor signals with linear transformation.
  @author Dipl.-Inform. Achim Spangler
  */
class AnalogI_c : public SensorBase_c  {
public:
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
  AnalogI_c(uint8_t ab_channel = 0xFF, Sensor_c::analogType_t ren_analogType = Sensor_c::voltage, bool ab_useMean = false,
             bool ab_fastAdc = false);
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
  void init(uint8_t ab_channel, Sensor_c::analogType_t ren_analogType = Sensor_c::voltage, bool ab_useMean = false,
             bool ab_fastAdc = false);
  /** destructor which can close the hardware input channel */
  virtual ~AnalogI_c();
  /**
    get the actual sensor value with the configured linear conversion (use the configured ADC method)
    (uses BIOS function)

    @return sensor value: A) Volt [mV], or B) Ampere [mA]
  */
  int16_t val() const ;
  /**
    check if value is greater than 0
    (uses BIOS function)

    possible errors:
        * iLibErr_c::Range wrong input number
    @return true if sensor value is different from 0, otherwise 0
  */
  bool active() const ;
  /**
    configure fast ADC gathering
    (uses BIOS function)
    @param ab_useFast default-argument for setting fast ADC (true as default)
  */
  void setFastAdc(bool ab_useFast=true);
  bool operator==( uint8_t aui8_key ) const { return ( aui8_key == channelNr() );};
protected:
  /**
    HIDDEN! copy constructor for AnalogI_c
    NEVER copy a AnalogI_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param acrc_src source
  */
  AnalogI_c(const AnalogI_c& acrc_src) : SensorBase_c(acrc_src) {};
  /**
    HIDDEN! assignment for AnalogI_c
    NEVER assign a AnalogI_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  AnalogI_c& operator=(const AnalogI_c& /*acrc_src*/){ return *this;};
private: // Private attributes
  /**
    input type: voltage or current
    @see __IsoAgLib::Sensor_c::t_analogType
  */
  Sensor_c::analogType_t en_analogType;
  /** if true a mean of the sensor values is retreived */
  bool b_useMean;
  /** if true the fast analog input method is used */
  bool b_fastAdc;
};
}

#endif
