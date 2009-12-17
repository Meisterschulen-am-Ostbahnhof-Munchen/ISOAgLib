/*
  counteri_c.cpp:
    implementation file for CounterI_c, an object for digital sensor
    input

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "counteri_c.h"
#include "sensori_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

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
CounterI_c::CounterI_c(uint8_t ab_channel, uint16_t aui16_timebase, bool ab_activHigh, bool ab_risingEdge)
  : SensorBase_c(ab_channel, Sensor_c::counter){
  if ( ab_channel != 0xFF ) init(ab_channel, aui16_timebase, ab_activHigh, ab_risingEdge);
}
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
void CounterI_c::init(uint8_t ab_channel, uint16_t aui16_timebase, bool ab_activHigh, bool ab_risingEdge )
{
  SensorBase_c::init(ab_channel, Sensor_c::counter);
  // now init the digital input
  if (HAL::init_counter(channelNr(), aui16_timebase, ab_activHigh, ab_risingEdge) == HAL_RANGE_ERR)
  { // wrong input channel no
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
  }
  else
  { // correct input channel no - now register the valid new analog input into SensorI_c
    getSensorInstance().registerClient( this );
  }
}
/**  destructor of the input object which can close explicit the hardware input */
CounterI_c::~CounterI_c(){
  // unregister element from SensorI_c
  getSensorInstance().unregisterClient( this );
}
/**
  check for the input value (uses BIOS function)
  @return true for counter > 0
*/
bool CounterI_c::active() const {
  return (val() > 0);
}
/**
  check for the input value (uses BIOS function)
  @return amount of signals since initialisation or last reset
*/
int16_t CounterI_c::val() const {
  return int16_t(HAL::getCounter(channelNr()));
}
/**
  check for the input value (uses BIOS function)
  @return amount of signals since initialisation or last reset
*/
int32_t CounterI_c::valLong(){
  return HAL::getCounter(channelNr());
}
/**
  reset the given counter

  possible errors:
      * iLibErr_c::Range wrong input number
*/
void CounterI_c::reset()
{
  if (HAL::resetCounter(channelNr()))
  { // wrong input channel no
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
  }
}

/**
  get period of counter channel
  @return time between last two signals or 0xFFFF if time is longer than initially
           given timebase
*/
uint16_t CounterI_c::period()
{
  return HAL::getCounterPeriod(channelNr());
}

/**
  get frequency of counter channel
  @return frequency calculated from time between last two signals
          or 0 if time is longer than initially given timebase
*/
uint16_t CounterI_c::frequency()
{
  return HAL::getCounterFrequency(channelNr());
}

/**
 get time since last signal
 @return time since last signal [msec.]
*/
uint32_t CounterI_c::lastSignalAge()
{
  return HAL::getCounterLastSignalAge(channelNr());
}

} // end of namespace __IsoAgLib
