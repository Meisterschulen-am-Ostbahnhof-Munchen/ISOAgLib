/***************************************************************************
                          idigital_i.h - interface header file for
                                         DigitalI_c, an object for
                                         digital sensor input
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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

#ifndef IDIGITAL_I_H
#define IDIGITAL_I_H

#include "impl/digitali_c.h"
#include "isensor_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {
// Usage of iSensorEventHandler
// 1) Derive clas from iSensorEventHandler with REAL function
//    void handleDigitalEvent( uint8_t rui8_channel );
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
    * @param rui8_channel channel of the input object, which received the IRQ
    *        from HAL
    */
  virtual void handleDigitalEvent( uint8_t rui8_channel );
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
    @param rb_channel default-argument for setting hardware channel for this input
    @param ren_onoff default-argument for setting whether 1 should be returned on High(Default: iSensor_c::OnHigh) or Low signal
    @param rb_static default-argument for setting if hardware input should be gathered static (default false with no static)
    @param rpc_handler optional pointer to handler class, which can be called, if an HAL irq event occurs
  */
  iDigitalI_c(uint8_t rb_channel = 0xFF, iSensor_c::onoff_t ren_onoff = iSensor_c::OnHigh,
      bool rb_static = false, iSensorEventHandler* rpc_handler = NULL )
    : DigitalI_c(rb_channel, ren_onoff, rb_static, rpc_handler){};
  /**
    internal called constructor for a new digital input channel which performs configuration of hardware
    (uses BIOS function)

    possible errors:
        * LibErr_c::Range wrong input number
    @see SensorI_c::createDigital
    @see Sensor_c::t_onoff
    @param rb_channel default-argument for setting hardware channel for this input
    @param ren_onoff default-argument for setting whether 1 should be returned on High(Default: Sensor_c::OnHigh) or Low signal
    @param rb_static default-argument for setting if hardware input should be gathered static (default false with no static)
    @param rpc_handler optional pointer to handler class, which can be called, if an HAL irq event occurs
  */
  void init(uint8_t rb_channel, iSensor_c::onoff_t ren_onoff = iSensor_c::OnHigh,
              bool rb_static = false, iSensorEventHandler* rpc_handler = NULL )
    { DigitalI_c::init(rb_channel, ren_onoff, rb_static, rpc_handler);};

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
    @param rrefc_src source
  */
  iDigitalI_c(const iDigitalI_c& rrefc_src) : DigitalI_c() {};
  /**
    HIDDEN! assignment for iDigitalI_c
    NEVER assign a iDigitalI_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param rrefc_src source
  */
  iDigitalI_c& operator=(const iDigitalI_c& rrefc_src){return *this;};

};
}
#endif
