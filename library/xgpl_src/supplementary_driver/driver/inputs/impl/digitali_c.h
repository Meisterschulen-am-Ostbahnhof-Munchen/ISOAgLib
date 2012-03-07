/*
  digitali_c.h:
    header file for Digital_I_c, an object for digital sensor input

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef DIGITAL_I_H
#define DIGITAL_I_H

#include "inputbase_c.h"

namespace IsoAgLib {
/** virtual base class for classes in applications, which want
  * to be called on a sensor related HAL IRQ event
  * a special application class must overload the appropriate
  * event handler function for the specific type;
  * additionally, the contructor of the DigitalI_c, AnalogI_c
  * or CounterI_c instance must be called with a pointer to the
  * handling object, which must be derived from this class
  */
class iInputEventHandler {
 public:
  /** function to handle a DigitalI_c event from HAL
    * @param aui8_channel channel of the input object, which received the IRQ
    *        from HAL
    */
  virtual void handleDigitalEvent( uint8_t aui8_channel );
  virtual ~iInputEventHandler() {};
};

};

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  Input object for simple digital input;
  has option for setting ON state to HI or LO and
  can request static input values
  @see InputBase_c
  @author Dipl.-Inform. Achim Spangler
  */
class DigitalI_c : public InputBase_c  {
public:
  /**
    internal called constructor for a new digital input channel which performs configuration of hardware
    (uses BIOS function)

    possible errors:
        * iLibErr_c::Range wrong input number
    @see SensorI_c::createDigital
    @see t_onoff
    @param ab_channel default-argument for setting hardware channel for this input
    @param ren_onoff default-argument for setting whether 1 should be returned on High(Default: OnHigh) or Low signal
    @param ab_static default-argument for setting if hardware input should be gathered static (default false with no static)
    @param apc_handler optional pointer to handler class, which can be called, if an HAL irq event occurs
  */
  DigitalI_c(uint8_t ab_channel = 0xFF, onoff_t ren_onoff = OnHigh,
              bool ab_static = false, IsoAgLib::iInputEventHandler* apc_handler = NULL );

  /**
    internal called constructor for a new digital input channel which performs configuration of hardware
    (uses BIOS function)

    possible errors:
        * iLibErr_c::Range wrong input number
    @see SensorI_c::createDigital
    @see t_onoff
    @param ab_channel default-argument for setting hardware channel for this input
    @param ren_onoff default-argument for setting whether 1 should be returned on High(Default: OnHigh) or Low signal
    @param ab_static default-argument for setting if hardware input should be gathered static (default false with no static)
    @param apc_handler optional pointer to handler class, which can be called, if an HAL irq event occurs
  */
  void init(uint8_t ab_channel, onoff_t ren_onoff = OnHigh,
              bool ab_static = false, IsoAgLib::iInputEventHandler* apc_handler = NULL );

  /** change detection mode of activity to OnHigh */
  void setOnHigh( void );

  /** change detection mode of activity to OnLow */
  void setOnLow( void );

  /**  destructor of the input object which can close explicit the hardware input */
  virtual ~DigitalI_c();

  /**
    check for the input value (uses BIOS function)

    possible errors:
        * iLibErr_c::Range wrong input number
    @return 1 for (High signal AND ren_onoff==OnHigh)(Default!) or (Low signal AND ren_onoff==OnLow); otherwise 0
  */
  uint16_t val() const;

  /**
    check for the input value (uses BIOS function)

    possible errors:
        * iLibErr_c::Range wrong input number
    @return true for (High signal AND ren_onoff==OnHigh)(Default!) or (Low signal AND ren_onoff==OnLow); otherwise false
  */
  bool active() const;

  /** handler function which is called from HAL */
  static void handleHalIrqEvent( uint8_t aui8_channel );

  bool operator==( uint8_t aui8_key ) const { return ( aui8_key == channelNr() );};

private:
  /**
    HIDDEN! copy constructor for DigitalI_c
    NEVER copy a DigitalI_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  DigitalI_c(const DigitalI_c&);

  /**
    HIDDEN! assignment for DigitalI_c
    NEVER assign a DigitalI_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  DigitalI_c& operator=(const DigitalI_c&);

  /** true if the digit input should be gathered static */
  bool b_static;

  /**
    mapping type for sensor HI/LO -> ON/OFF
    @see t_onoff
  */
  onoff_t en_onoff;

  /** array of pointers to handler for IRQ events */
  #ifndef __TSW_CPP__
  static IsoAgLib::iInputEventHandler* ppc_handler[16];
  #endif
};

} // __IsoAgLib
#endif
