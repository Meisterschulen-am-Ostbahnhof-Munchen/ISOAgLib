/***************************************************************************
                          idigital_o.h  - header file for iDigital_O_c, an object
                                         for digital actor output
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
#ifndef IDIGITAL_O_H
#define IDIGITAL_O_H

#include "impl/digitalo_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  Output object for simple digital output;
  @see Actor_O
  @see ActorBase
  @author Dipl.-Inform. Achim Spangler
*/
class iDigitalO_c : private __IsoAgLib::DigitalO_c {
public:
  /** enum for error states of digital output
    * values: noDoutErr, dout_openErr, dout_shortcutErr, dout_overtempErr,
    *         dout_overvoltErr, dout_untervoltErr
    */
  typedef enum { noDoutErr, dout_openErr, dout_shortcutErr, dout_overtempErr, dout_overvoltErr, dout_untervoltErr } dout_err_t;
  //typedef __IsoAgLib::DigitalO_c::dout_err_t dout_err_t;

  /**
    internal called constructor for a new digital input channel which performs configuration of hardware

    possible errors:
        * Err_c::range wrong input number
    @see iActorO_c::createDigital
    @param rui8_channel default-argument for setting hardware channel for this input
  */
  iDigitalO_c(uint8_t rui8_channel = 0) : DigitalO_c(rui8_channel){};
  /** init this instance
    @param rui8_channel default-argument for setting hardware channel for this input
  */
  void init(uint8_t rui8_channel) { DigitalO_c::init( rui8_channel );};
  /**  destructor of the input object which can close explicit the hardware input */
  virtual ~iDigitalO_c() {};
  /**
    set the output PWM frequency
    (uses BIOS function)

    possible errors:
       * Err_c::range wrong output channel number or wrong frequency
    @param rui32_val value to use as PWM frequency in [mHz]
  */
  void setFreq(uint32_t rui32_val){DigitalO_c::setFreq(rui32_val);};
  /**
    set the output PWM value

    possible errors:
        * Err_c::range wrong output channel number
    @param rui16_val value to set for the output channel
  */
  void set(uint16_t rui16_val) {DigitalO_c::set(rui16_val);};
  /**
    switch PWM to total OFF or ON (simple switch function)
    (uses BIOS function)

    possible errors:
       * Err_c::range wrong output channel number
    @param rb_state
  */
  void set(bool rb_state) {DigitalO_c::set(rb_state);};

  /**
    deliver actual set value
    @return last set value [0..0xffff]
  */
  uint16_t get()const{return DigitalO_c::get();};
  /**
    * set the allowed current limits for active state
    * @param rui16_minActiveCurrent minimal allowed current in active state
    * @param rui16_maxActiveCurrent maximum allowed current in active state
    */
  void setActiveCurrentLimits( uint16_t rui16_minActiveCurrent, uint16_t rui16_maxActiveCurrent )
  {DigitalO_c::setActiveCurrentLimits(rui16_minActiveCurrent,rui16_maxActiveCurrent );};
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
    * @return dout_err_t [noDoutErr|dout_openErr|dout_shortcutErr|dout_overtempErr|dout_overvoltErr|dout_untervoltErr]
    */
  iDigitalO_c::dout_err_t getState( void ) const { return iDigitalO_c::dout_err_t((uint16_t)DigitalO_c::getState());};
private:
  /**
    HIDDEN! copy constructor for Digital_O
    NEVER copy a iDigitalO_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param rrefc_src source
  */
  iDigitalO_c(const iDigitalO_c& rrefc_src) : DigitalO_c() {};
  /**
    HIDDEN! assignment for Digital_O
    NEVER assign a iDigitalO_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param rrefc_src source
  */
  iDigitalO_c& operator=(const iDigitalO_c& rrefc_src){ return *this;};
};

}
#endif
