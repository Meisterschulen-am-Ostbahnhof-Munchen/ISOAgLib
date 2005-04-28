/***************************************************************************
                          icounter_i.h  -  header file for iCounter_I_c, an object
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
#ifndef ICOUNTER_I_H
#define ICOUNTER_I_H

#include "impl/counteri_c.h"
#include "isensor_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  Input object for counting of digital impulses
  @see iSensor_I
  @see iDigital_I
  @see iAnalog_I
  @author Dipl.-Inform. Achim Spangler
  */
class iCounterI_c : private __IsoAgLib::CounterI_c {
public:
  /**
    internal called constructor for a new digital input channel which performs configuration of hardware
    (uses BIOS function)

    possible errors:
        * Err_c::range wrong input number
    @see SensorI_c::createCounter
    @param rb_channel default-argument for setting hardware channel for this input
    @param rui16_timebase default-argument for setting the timebase which should be
            greater than max time distance between signals and should be small
            enough to avoid overflow of signals in one timebase
    @param rb_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
    @param rb_risingEdge true -> counter triggers on rising edge; else on falling edge
  */
  iCounterI_c(uint8_t rb_channel = 0xFF, uint16_t rui16_timebase = 0, bool rb_activHigh = true, bool rb_risingEdge = true)
    : CounterI_c(rb_channel, rui16_timebase, rb_activHigh, rb_risingEdge) {};
  /**
    internal called constructor for a new digital input channel which performs configuration of hardware
    (uses BIOS function)

    possible errors:
        * Err_c::range wrong input number
    @see SensorI_c::createCounter
    @param rb_channel default-argument for setting hardware channel for this input
    @param rui16_timebase default-argument for setting the timebase which should be
            greater than max time distance between signals and should be small
            enough to avoid overflow of signals in one timebase
    @param rb_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
    @param rb_risingEdge true -> counter triggers on rising edge; else on falling edge
  */
  void init(uint8_t rb_channel, uint16_t rui16_timebase = 0, bool rb_activHigh = true, bool rb_risingEdge = true)
    { CounterI_c::init(rb_channel, rui16_timebase, rb_activHigh, rb_risingEdge);};
  /**  destructor of the input object which can close explicit the hardware input */
  virtual ~iCounterI_c(){};
  /**
    check for the input value (uses BIOS function)
    @return true for counter > 0
  */
  int16_t val() const {return CounterI_c::val();};
  /**
    check for the input value (uses BIOS function)
    @return true for counter > 0
  */
  int32_t valLong(){return CounterI_c::valLong();};
  /**
    check for the input value (uses BIOS function)
    @return amount of signals since initialisation or last reset
  */
  bool active() const {return CounterI_c::active();};
  /**
    reset the given counter

    possible errors:
        * Err_c::range wrong input number
  */
  void reset(){CounterI_c::reset();};
  /**
    get period of counter channel
    @return time between last two signals or 0xFFFF if time is longer than initially
             given timebase
  */
  uint16_t period(){return CounterI_c::period();};
  /**
    get frequency of counter channel
    @return frequency calculated from time between last two signals
            or 0 if time is longer than initially given timebase
  */
  uint16_t frequency(){return CounterI_c::frequency();};
  /**
   get time since last signal
   @return time since last signal [msec.]
  */
  uint32_t lastSignalAge(){return CounterI_c::lastSignalAge();};
};
}
#endif
