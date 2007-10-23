/***************************************************************************
                          ianalogi_c.h  - interface header file for iAnalog_I_c,
                                         an object for analog sensor input
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

#ifndef IANALOG_I_H
#define IANALOG_I_H

#include "impl/analogi_c.h"
#include "isensor_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**Interface class for Analog input objects
  *@author Dipl.-Inform. Achim Spangler
  */
class iAnalogI_c : private __IsoAgLib::AnalogI_c  {
private:
  // private typedef alias names
  typedef __IsoAgLib::Sensor_c Sensor_c;
public:
  /**
    internal called constructor which creates a new input channel,initialize the hardware and configures conversion calculation

    possible errors:
        * Err_c::range wrong input number
        * Err_c::precondition wrong input type
    @see iSensorI_c::createAnalog
    @see iSensor_c::t_analogType
    @param ab_channel default-argument for the hardware channel of the input
    @param ren_analogType default-argument for choosing iSensor_c::voltage(default) or iSensor_c::current as input type
    @param ab_useMean default-argument for setting the calculation of mean value on true (false as default)
    @param ab_fastAdc default-argument for setting fast ADC method (false as default)
    @param ai32_scaleMin set the min return value where BIOS sensor value should be scaled (default no scale)
    @param ai32_scaleMax set the max return value where BIOS sensor value should be scaled (default no scale)
  */
  iAnalogI_c(uint8_t ab_channel = 0xFF, iSensor_c::analogType_t ren_analogType = iSensor_c::voltage, bool ab_useMean = false, bool ab_fastAdc = false )
    : AnalogI_c(ab_channel, ren_analogType, ab_useMean, ab_fastAdc ) {};
  /**
    internal called constructor which creates a new input channel,initialize the hardware and configures conversion calculation

    possible errors:
        * Err_c::range wrong input number
        * Err_c::precondition wrong input type
    @see iSensorI_c::createAnalog
    @see iSensor_c::t_analogType
    @param ab_channel default-argument for the hardware channel of the input
    @param ren_analogType default-argument for choosing iSensor_c::voltage(default) or iSensor_c::current as input type
    @param ab_useMean default-argument for setting the calculation of mean value on true (false as default)
    @param ab_fastAdc default-argument for setting fast ADC method (false as default)
    @param ai32_scaleMin set the min return value where BIOS sensor value should be scaled (default no scale)
    @param ai32_scaleMax set the max return value where BIOS sensor value should be scaled (default no scale)
  */
  void init(uint8_t ab_channel, iSensor_c::analogType_t ren_analogType = iSensor_c::voltage, bool ab_useMean = false, bool ab_fastAdc = false )
    { AnalogI_c::init(ab_channel, ren_analogType, ab_useMean, ab_fastAdc );};
  /** destructor which can close the hardware input channel */
  virtual ~iAnalogI_c() {};
  /**
    get the actual sensor value with the configured linear conversion (use the configured ADC method)
    (uses BIOS function)

    @return sensor value: A) Volt [mV], or B) Ampere [mA]
  */
  int16_t val() const {return AnalogI_c::val();};
  /**
    check if value is greater than 0

    possible errors:
        * Err_c::range wrong input number
    @return true if sensor value is different from 0, otherwise 0
  */
  bool active() const {return AnalogI_c::active();};
  /**
    configure fast ADC gathering
    @param ab_useFast default-argument for setting fast ADC (true as default)
  */
  void setFastAdc(bool ab_useFast=true){AnalogI_c::setFastAdc(ab_useFast);};
  /**
    deliver the channel number of the output object
    @return number to use for BIOS access to this channel
  */
  uint8_t channelNr() const { return AnalogI_c::channelNr();};
private:
  /**
    HIDDEN! copy constructor for AnalogI_c
    NEVER copy a AnalogI_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param arc_src source
  */
  iAnalogI_c(const iAnalogI_c& /*arc_src*/) : AnalogI_c() {};
  /**
    HIDDEN! assignment for AnalogI_c
    NEVER assign a AnalogI_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param arc_src source
  */
  iAnalogI_c& operator=(const iAnalogI_c& /*arc_src*/){ return *this;};
};
}
#endif
