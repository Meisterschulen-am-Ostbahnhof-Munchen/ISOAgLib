/*
  icounter_i.h - header file for iCounterI_c

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ICOUNTERI_C_H
#define ICOUNTERI_C_H

#include "impl/counteri_c.h"


namespace IsoAgLib {

/**
  Input object for counting of digital impulses
  @see iCounterI_c
  @see iDigitalI_c
  @see iAnalogI_c
  @author Dipl.-Inform. Achim Spangler
  */
class iCounterI_c : private __IsoAgLib::CounterI_c {
public:
  /**
    internal called constructor for a new digital input channel which performs configuration of hardware
    @param ab_channel default-argument for setting hardware channel for this input
    @param aui16_timebase default-argument for setting the timebase which should be
            greater than max time distance between signals and should be small
            enough to avoid overflow of signals in one timebase
    @param ab_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
    @param ab_risingEdge true -> counter triggers on rising edge; else on falling edge
  */
  iCounterI_c(uint8_t ab_channel = 0xFF, uint16_t aui16_timebase = 0, bool ab_activHigh = true, bool ab_risingEdge = true)
    : CounterI_c(ab_channel, aui16_timebase, ab_activHigh, ab_risingEdge) {}

  /**
    internal called constructor for a new digital input channel which performs configuration of hardware
    @param ab_channel default-argument for setting hardware channel for this input
    @param aui16_timebase default-argument for setting the timebase which should be
            greater than max time distance between signals and should be small
            enough to avoid overflow of signals in one timebase
    @param ab_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
    @param ab_risingEdge true -> counter triggers on rising edge; else on falling edge
  */
  void init(uint8_t ab_channel, uint16_t aui16_timebase = 0, bool ab_activHigh = true, bool ab_risingEdge = true)
    { CounterI_c::init(ab_channel, aui16_timebase, ab_activHigh, ab_risingEdge); }

  virtual ~iCounterI_c(){}

  /**
    check for the input value (uses BIOS function)
  */
  uint32_t val() const {return CounterI_c::val();}

  /**
    check for the input value (uses BIOS function)
    @return amount of signals since initialisation or last reset
  */
  bool active() const {return CounterI_c::active();}

  /**
    reset the given counter
  */
  void reset() {CounterI_c::reset();}

  /**
    get period of counter channel
    @return time between last two signals or 0xFFFF if time is longer than initially
             given timebase
  */
  uint16_t period() {return CounterI_c::period();}

  /**
    get frequency of counter channel
    @return frequency calculated from time between last two signals
            or 0 if time is longer than initially given timebase
  */
  uint32_t frequency() {return CounterI_c::frequency();}

  /**
    get time since last signal
    @return time since last signal [msec.]
  */
  uint32_t lastSignalAge() {return CounterI_c::lastSignalAge();}

  /**
    deliver the channel number of the output object
    @return number to use for BIOS access to this channel
  */
  uint8_t channelNr() const { return CounterI_c::channelNr(); }
};

} // IsoAgLib

#endif
