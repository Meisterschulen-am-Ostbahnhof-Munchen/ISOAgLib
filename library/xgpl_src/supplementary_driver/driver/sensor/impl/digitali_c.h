/***************************************************************************
                          digitali_c.h  -  header file for Digital_I_c, an object
                                          for digital sensor input
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#ifndef DIGITAL_I_H
#define DIGITAL_I_H

#include "sensorbase_c.h"

namespace IsoAgLib {
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
  virtual ~iSensorEventHandler() {};
};

};

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  Input object for simple digital input;
  has option for setting ON state to HI or LO and
  can request static input values
  @see Sensor_I
  @see Analog_I
  @see SensorBase
  @author Dipl.-Inform. Achim Spangler
  */
class DigitalI_c : public SensorBase_c  {
public:
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
  DigitalI_c(uint8_t ab_channel = 0xFF, Sensor_c::onoff_t ren_onoff = Sensor_c::OnHigh,
              bool ab_static = false, IsoAgLib::iSensorEventHandler* apc_handler = NULL );
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
  void init(uint8_t ab_channel, Sensor_c::onoff_t ren_onoff = Sensor_c::OnHigh,
              bool ab_static = false, IsoAgLib::iSensorEventHandler* apc_handler = NULL );

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
  int16_t val()const;
  /**
    check for the input value (uses BIOS function)

    possible errors:
        * iLibErr_c::Range wrong input number
    @return true for (High signal AND ren_onoff==OnHigh)(Default!) or (Low signal AND ren_onoff==OnLow); otherwise false
  */
  bool active() const;
  /** handler function which is called from HAL */
  static void handleHalIrqEvent( uint8_t aui8_channel );
  bool operator==( uint8_t aui8_key ) const { return ( aui8_key == channelNr() )?true:false;};
private:
// Private attributes
  /**
    HIDDEN! copy constructor for DigitalI_c
    NEVER copy a DigitalI_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  DigitalI_c(const DigitalI_c& /*acrc_src*/) : SensorBase_c(0) {};
  /**
    HIDDEN! assignment for DigitalI_c
    NEVER assign a DigitalI_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  DigitalI_c& operator=(const DigitalI_c& /*acrc_src*/){return *this;};
  /** true if the digit input should be gathered static */
  bool b_static;
  /**
    mapping type for sensor HI/LO -> ON/OFF
    @see Sensor_c::t_onoff
  */
  Sensor_c::onoff_t en_onoff;
  /** array of pointers to handler for IRQ events */
  #ifndef __TSW_CPP__
  static IsoAgLib::iSensorEventHandler* ppc_handler[16];
  #endif
};
}
#endif
