/*
  counteri_c.h:
    header file for Counter_I_c, an object for digital sensor input

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef COUNTER_I_H
#define COUNTER_I_H

#include "sensorbase_c.h"

namespace IsoAgLib {
class iCounterI_c;
};

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  Input object for counting of digital impulses
  @see Sensor_I
  @see Digital_I
  @see Analog_I
  @see SensorBase
  @author Dipl.-Inform. Achim Spangler
  */
class CounterI_c : public SensorBase_c  {
public:
  /**
    internal called constructor for a new digital input channel which performs configuration of hardware
    (uses BIOS function)

    possible errors:
        * iLibErr_c::Range wrong input number
    @see SensorI_c::createCounter
    @param ab_channel default-argument for setting hardware channel for this input
    @param aui16_timebase default-argument for setting the timebase which should be
            greater than max time distance between signals and should be small
            enough to avoid overflow of signals in one timebase
    @param ab_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
    @param ab_risingEdge true -> counter triggers on rising edge; else on falling edge
  */
  CounterI_c(uint8_t ab_channel = 0xFF, uint16_t aui16_timebase = 0, bool ab_activHigh = true,
              bool ab_risingEdge = true);
  /**
    internal called constructor for a new digital input channel which performs configuration of hardware
    (uses BIOS function)

    possible errors:
        * iLibErr_c::Range wrong input number
    @see SensorI_c::createCounter
    @param ab_channel default-argument for setting hardware channel for this input
    @param aui16_timebase default-argument for setting the timebase which should be
            greater than max time distance between signals and should be small
            enough to avoid overflow of signals in one timebase
    @param ab_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
    @param ab_risingEdge true -> counter triggers on rising edge; else on falling edge
  */
  void init(uint8_t ab_channel, uint16_t aui16_timebase = 0, bool ab_activHigh = true,
              bool ab_risingEdge = true);
  /**  destructor of the input object which can close explicit the hardware input */
  virtual ~CounterI_c();
  /**
    check for the input value (uses BIOS function)
    @return true for counter > 0
  */
  int32_t valLong();
  /**
    check for the input value (uses BIOS function)
    @return true for counter > 0
  */
  int16_t val() const ;
  /**
    check for the input value (uses BIOS function)
    @return amount of signals since initialisation or last reset
  */
  bool active() const ;
  /**
    reset the given counter

    possible errors:
        * iLibErr_c::Range wrong input number
  */
  void reset();
  /**
    get period of counter channel
    @return time between last two signals or 0xFFFF if time is longer than initially
             given timebase
  */
  uint16_t period();
  /**
    get frequency of counter channel
    @return frequency calculated from time between last two signals
            or 0 if time is longer than initially given timebase
  */
  uint16_t frequency();
  /**
   get time since last signal
   @return time since last signal [msec.]
  */
  uint32_t lastSignalAge();
  bool operator==( uint8_t aui8_key ) const { return ( aui8_key == channelNr() );};
private:
  /**
    HIDDEN! copy constructor for CounterI_c
    NEVER copy a CounterI_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  CounterI_c(const CounterI_c& /*acrc_src*/) : SensorBase_c(0) {};
  /**
    HIDDEN! assignment for CounterI_c
    NEVER assign a CounterI_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!-- @param acrc_src source -->
  */
  CounterI_c& operator=(const CounterI_c& /*acrc_src*/){ return *this;};
};
}
#endif
