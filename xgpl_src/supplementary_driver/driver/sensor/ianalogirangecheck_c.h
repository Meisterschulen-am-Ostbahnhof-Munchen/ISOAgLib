/***************************************************************************
                          ianalogirangecheck_c.h  - interface header file for iAnalogIRangeCheck_c,
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

#ifndef IANALOGIRANGECHECK_H
#define IANALOGIRANGECHECK_H

#include "impl/analogirangecheck_c.h"
#include "isensor_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**Interface class for Analog input objects
  *@author Dipl.-Inform. Achim Spangler
  */
class iAnalogIRangeCheck_c : private __IsoAgLib::AnalogIRangeCheck_c  {
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
    @param rb_channel default-argument for the hardware channel of the input
    @param ren_analogType default-argument for choosing iSensor_c::voltage(default) or iSensor_c::current as input type
    @param rb_useMean default-argument for setting the calculation of mean value on true (false as default)
    @param rb_fastAdc default-argument for setting fast ADC method (false as default)
    @param rui16_minValid default min allowed value (min limit of range)
    @param rui16_maxValid default max allowed value (max limit of range)
  */
  iAnalogIRangeCheck_c(uint8_t rb_channel = 0xFF, iSensor_c::analogType_t ren_analogType = iSensor_c::voltage, bool rb_useMean = false, bool rb_fastAdc = false,
    uint16_t rui16_minValid = 0, uint16_t rui16_maxValid = 0xFFFF )
    : AnalogIRangeCheck_c(rb_channel, ren_analogType, rb_useMean, rb_fastAdc, rui16_minValid, rui16_maxValid ) {};
  /**
    internal called constructor which creates a new input channel,initialize the hardware and configures conversion calculation

    possible errors:
        * Err_c::range wrong input number
        * Err_c::precondition wrong input type
    @see iSensorI_c::createAnalog
    @see iSensor_c::t_analogType
    @param rb_channel default-argument for the hardware channel of the input
    @param ren_analogType default-argument for choosing iSensor_c::voltage(default) or iSensor_c::current as input type
    @param rb_useMean default-argument for setting the calculation of mean value on true (false as default)
    @param rb_fastAdc default-argument for setting fast ADC method (false as default)
    @param rui16_minValid default min allowed value (min limit of range)
    @param rui16_maxValid default max allowed value (max limit of range)
  */
  void init(uint8_t rb_channel, iSensor_c::analogType_t ren_analogType = iSensor_c::voltage, bool rb_useMean = false, bool rb_fastAdc = false,
    uint16_t rui16_minValid = 0, uint16_t rui16_maxValid = 0xFFFF )
    { AnalogIRangeCheck_c::init(rb_channel, ren_analogType, rb_useMean, rb_fastAdc, rui16_minValid, rui16_maxValid );};
  /** destructor which can close the hardware input channel */
  virtual ~iAnalogIRangeCheck_c() {};
  /**
    get the actual sensor value with the configured linear conversion (use the configured ADC method)
    (uses BIOS function)

    @return sensor value: A) Volt [mV], or B) Ampere [mA]
  */
  int16_t val() const {return AnalogIRangeCheck_c::val();};
  /** get validate val
    @param refb_tooLow  reference to bool value which is set dependend on ( value < minLimit )
    @param refb_tooHigh reference to bool value which is set dependend on ( value > maxLimit )
    @return sensor value
  */
  int16_t validatedVal( bool &refb_tooLow, bool &refb_tooHigh ) const
   { return AnalogIRangeCheck_c::validatedVal( refb_tooLow, refb_tooHigh ); };
  /**
    check if value is greater than 0
  
    possible errors:
        * Err_c::range wrong input number
    @return true if sensor value is different from 0, otherwise 0
  */
  bool active() const {return AnalogIRangeCheck_c::active();};
  /**
    configure fast ADC gathering
    @param rb_useFast default-argument for setting fast ADC (true as default)
  */
  void setFastAdc(bool rb_useFast=true){AnalogIRangeCheck_c::setFastAdc(rb_useFast);};
  /**
    deliver the channel number of the output object
    @return number to use for BIOS access to this channel
  */
  uint8_t channelNr() const { return AnalogIRangeCheck_c::channelNr();};

  /** check if sensor value is valid */
  virtual bool good( void ) const { return AnalogIRangeCheck_c::good(); };
  /** check if sensor value is in error state */
  virtual bool error( void ) const { return AnalogIRangeCheck_c::error(); } ;
  /** check if sensor value is out of range */
  bool checkRangeError( void ) const { return AnalogIRangeCheck_c::checkRangeError(); } ;
  /** check if value is too high */
  bool checkTooHigh( void ) const { return AnalogIRangeCheck_c::checkTooHigh(); } ;
  /** check if value is too low */
  bool checkTooLow( void ) const { return AnalogIRangeCheck_c::checkTooLow(); } ;
  /* set range
    @param rui16_minValid default min allowed value (min limit of range)
    @param rui16_maxValid default max allowed value (max limit of range)
  */
  void setRange( uint16_t rui16_minValid, uint16_t rui16_maxValid )
    { AnalogIRangeCheck_c::setRange( rui16_minValid, rui16_maxValid ); };

  /** deliver total valid range in mV */
  uint16_t getRange( void ) const { return AnalogIRangeCheck_c::getRange(); } ;
  /** deliver min Limit */
  uint16_t getMinValid( void ) const { return AnalogIRangeCheck_c::getMinValid(); };
  /** deliver max Limit */
  uint16_t getMaxValid( void ) const { return AnalogIRangeCheck_c::getMaxValid(); };
private:
  /**
    HIDDEN! copy constructor for AnalogIRangeCheck_c
    NEVER copy a AnalogIRangeCheck_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param rrefc_src source
  */
  iAnalogIRangeCheck_c(const iAnalogIRangeCheck_c& rrefc_src) : AnalogIRangeCheck_c() {};
  /**
    HIDDEN! assignment for AnalogIRangeCheck_c
    NEVER assign a AnalogIRangeCheck_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param rrefc_src source
  */
  iAnalogIRangeCheck_c& operator=(const iAnalogIRangeCheck_c& rrefc_src){ return *this;};
};
}
#endif
