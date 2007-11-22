/***************************************************************************
                          analogi_c.h  -  header file for Analog_I_c, an object
                                         for analog sensor input
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
  bool operator==( uint8_t aui8_key ) const { return ( aui8_key == channelNr() )?true:false;};
protected:
  /**
    HIDDEN! copy constructor for AnalogI_c
    NEVER copy a AnalogI_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param arc_src source
  */
  AnalogI_c(const AnalogI_c& arc_src) : SensorBase_c(arc_src) {};
  /**
    HIDDEN! assignment for AnalogI_c
    NEVER assign a AnalogI_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param arc_src source
  */
  AnalogI_c& operator=(const AnalogI_c& /*arc_src*/){ return *this;};
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
