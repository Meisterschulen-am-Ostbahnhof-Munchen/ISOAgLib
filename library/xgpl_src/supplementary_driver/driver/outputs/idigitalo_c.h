/*
  idigital_o.h:
    header file for iDigital_O_c, an object for digital output

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IDIGITAL_O_H
#define IDIGITAL_O_H

#include "impl/digitalo_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  Output object for simple digital output;
  @see Outputs_c
  @see OutputBase_c
  @author Dipl.-Inform. Achim Spangler
*/
class iDigitalO_c : private __IsoAgLib::DigitalO_c {
public:
  /** enum for error states of digital output
    * values: noDoutErr, dout_openErr, dout_shortcutErr,
    *         dout_overvoltErr, dout_untervoltErr
    */
  typedef enum { noDoutErr, dout_openErr, dout_shortcutErr } dout_err_t;
  //typedef __IsoAgLib::DigitalO_c::dout_err_t dout_err_t;

  /**
    internal called constructor for a new digital input channel which performs configuration of hardware

    possible errors:
        * Err_c::range wrong input number
    @see iOutputs_c::createDigital
    @param aui8_channel default-argument for setting hardware channel for this input
  */
  iDigitalO_c(uint8_t aui8_channel = 0) : DigitalO_c(aui8_channel){};
  /** init this instance
    @param aui8_channel default-argument for setting hardware channel for this input
  */
  void init(uint8_t aui8_channel) { DigitalO_c::init( aui8_channel );};
  /**  destructor of the input object which can close explicit the hardware input */
  virtual ~iDigitalO_c() {};
  /**
    set the output PWM frequency
    (uses BIOS function)

    possible errors:
       * Err_c::range wrong output channel number or wrong frequency
    @param aui32_val value to use as PWM frequency in [mHz]
  */
  void setFreq(uint32_t aui32_val){DigitalO_c::setFreq(aui32_val);};
  /**
    set the output PWM value

    possible errors:
        * Err_c::range wrong output channel number
    @param aui16_val value to set for the output channel
  */
  void set(uint16_t aui16_val) {DigitalO_c::set(aui16_val);};
  /**
    switch PWM to total OFF or ON (simple switch function)
    (uses BIOS function)

    possible errors:
       * Err_c::range wrong output channel number
    @param ab_state
  */
  void set(bool ab_state) {DigitalO_c::set(ab_state);};

  /**
    deliver actual set value
    @return last set value [0..0xffff]
  */
  uint16_t get()const{return DigitalO_c::get();};
  /**
    * set the allowed current limits for active state
    * @param aui16_minActiveCurrent minimal allowed current in active state
    * @param aui16_maxActiveCurrent maximum allowed current in active state
    */
  void setActiveCurrentLimits( uint16_t aui16_minActiveCurrent, uint16_t aui16_maxActiveCurrent )
  {DigitalO_c::setActiveCurrentLimits(aui16_minActiveCurrent,aui16_maxActiveCurrent );};
  /** deliver the actual current of the digital output
    * @return current in [mA] ( if this digital out instance doesn't support current measurement, -1 is returned )
    */
  inline int16_t getDigoutCurrent( void ) const { return DigitalO_c::getDigoutCurrent();};
  /** check if this digital output channel is in FAIL condition
    * @return true -> channel is in fail condition
    */
  bool operator!( void ) const { return DigitalO_c::operator!();};
  /** check if this digital output channel is in GOOD condition
    * @return true -> channel is in GOOD condition
    */
  bool good( void ) const { return DigitalO_c::good();};
  /** deliver detailed error state information for this Digital Output
    * This function evaluates the current where possible, otherwise it evaluates
    * the measured voltage at the output. The latter interpretation can go wrong
    * if the PWM setting is >0 but has a very low value, so that even under normal
    * conditions the voltage with connected consuming device is lower than to open
    * connector state at low level.
    * @return dout_err_t [noDoutErr|dout_openErr|dout_shortcutErr]
    */
  iDigitalO_c::dout_err_t getState( void ) const { return iDigitalO_c::dout_err_t((uint16_t)DigitalO_c::getState());};
  /** deliver the measure voltage at the PWM output.
    Use this for application specific state evaluation for cases, where the standard
    getDigoutDiagnose function can go wrong.
    @return voltage at PWM output [mV]
  */
  int16_t getDigoutAdc( void ) const { return DigitalO_c::getDigoutAdc();};
  /** deliver the max allowed PWM -> setting this value results in max output
    @return max allowed PWM for the current PWM frequency setting
  */
  uint16_t getMaxOutputPwmFreq() const { return DigitalO_c::getMaxOutputPwmFreq();};

private:
  /**
    HIDDEN! copy constructor for Digital_O
    NEVER copy a iDigitalO_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  iDigitalO_c(const iDigitalO_c& /*acrc_src*/) : DigitalO_c() {};
  /**
    HIDDEN! assignment for Digital_O
    NEVER assign a iDigitalO_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  iDigitalO_c& operator=(const iDigitalO_c& /*acrc_src*/){ return *this;};

  friend class iOutputs_c;
};

}
#endif
