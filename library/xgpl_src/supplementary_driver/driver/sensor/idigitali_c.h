/*
  idigital_i.h:
    interface header file for DigitalI_c, an object for digital sensor
    input

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef IDIGITAL_I_H
#define IDIGITAL_I_H

#include "impl/digitali_c.h"
#include "isensor_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {
// Usage of iSensorEventHandler
// 1) Derive clas from iSensorEventHandler with REAL function
//    void handleDigitalEvent( uint8_t aui8_channel );
//    which overloads the function of the base class
// 2) provide pointer to an instance of the derived class to the
//    constructor of iDigitalI_c
// IMPROTANT: The following block of code shall only SHOW
//            the body of the base class (the ACTIVE copy
//            of these source code lines is in "impl/digital_i.h"
#if 0
/** virtual base class for classes in applications, which want
  * to be called on a sensor related HAL IRQ event
  * a special application class must overload the appropriate
  * event handler function for the specific type;
  * additionally, the contructor of the DigitalI_c, AnalogI_c
  * or CounterI_c instance must be called with a pointer to the
  * handling object, which must be derived from this class
  */
class iSensorEventHandler {
 public:
  /** function to handle a DigitalI_c event from HAL
    * @param aui8_channel channel of the input object, which received the IRQ
    *        from HAL
    */
  virtual void handleDigitalEvent( uint8_t aui8_channel );
}
#endif

/**Interface class for Analog input objects
  @author Dipl.-Inform. Achim Spangler
  */
class iDigitalI_c : private __IsoAgLib::DigitalI_c {
private:
  // private typedef alias names
  typedef __IsoAgLib::Sensor_c Sensor_c;
public:
  /**
    internal called constructor for a new digital input channel which performs configuration of hardware

    possible errors:
        * Err_c::range wrong input number
    @see iSensorI_c::createDigital
    @see iSensor_c::t_onoff
    @param ab_channel default-argument for setting hardware channel for this input
    @param ren_onoff default-argument for setting whether 1 should be returned on High(Default: iSensor_c::OnHigh) or Low signal
    @param ab_static default-argument for setting if hardware input should be gathered static (default false with no static)
    @param apc_handler optional pointer to handler class, which can be called, if an HAL irq event occurs
  */
  iDigitalI_c(uint8_t ab_channel = 0xFF, iSensor_c::onoff_t ren_onoff = iSensor_c::OnHigh,
      bool ab_static = false, iSensorEventHandler* apc_handler = NULL )
    : DigitalI_c(ab_channel, ren_onoff, ab_static, apc_handler){};
  /**
    internal called constructor for a new digital input channel which performs configuration of hardware
    (uses BIOS function)

    possible errors:
        * iLibErr_c::Range wrong input number
    @see SensorI_c::createDigital
    @see Sensor_c::t_onoff
    @param ab_channel default-argument for setting hardware channel for this input
    @param ren_onoff default-argument for setting whether 1 should be returned on High(Default: Sensor_c::OnHigh) or Low signal
    @param ab_static default-argument for setting if hardware input should be gathered static (default false with no static)
    @param apc_handler optional pointer to handler class, which can be called, if an HAL irq event occurs
  */
  void init(uint8_t ab_channel, iSensor_c::onoff_t ren_onoff = iSensor_c::OnHigh,
              bool ab_static = false, iSensorEventHandler* apc_handler = NULL )
    { DigitalI_c::init(ab_channel, ren_onoff, ab_static, apc_handler);};

  /**  destructor of the input object which can close explicit the hardware input */
  virtual ~iDigitalI_c(){};
  /** change detection mode of activity to OnHigh */
  void setOnHigh( void ) { DigitalI_c::setOnHigh();};
  /** change detection mode of activity to OnLow */
  void setOnLow( void ) { DigitalI_c::setOnLow();};
  /**
    check for the input value

    possible errors:
        * Err_c::range wrong input number
    @return 1 for (High signal AND ren_onoff==OnHigh)(Default!) or (Low signal AND ren_onoff==OnLow); otherwise 0
  */
  int16_t val()const{return DigitalI_c::val();};
  /**
    check for the input value (uses BIOS function)

    possible errors:
        * Err_c::range wrong input number
    @return true for (High signal AND ren_onoff==OnHigh)(Default!) or (Low signal AND ren_onoff==OnLow); otherwise false
  */
  bool active()const {return DigitalI_c::active();};
  /**
    deliver the channel number of the output object
    @return number to use for BIOS access to this channel
  */
  uint8_t channelNr() const { return DigitalI_c::channelNr();};
private:
// Private attributes
  /**
    HIDDEN! copy constructor for iDigitalI_c
    NEVER copy a iDigitalI_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  iDigitalI_c(const iDigitalI_c& /*acrc_src*/) : DigitalI_c() {};
  /**
    HIDDEN! assignment for iDigitalI_c
    NEVER assign a iDigitalI_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  iDigitalI_c& operator=(const iDigitalI_c& /*acrc_src*/){return *this;};

};
}
#endif
