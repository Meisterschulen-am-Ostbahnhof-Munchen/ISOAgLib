/*
  counteri_c.cpp - implementation file for CounterI_c

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "counteri_c.h"
#include <IsoAgLib/util/iassert.h>


namespace __IsoAgLib {

CounterI_c::CounterI_c(uint8_t ab_channel, uint16_t aui16_timebase, bool ab_activHigh, bool ab_risingEdge)
  : InputBase_c(ab_channel, IsoAgLib::iInput_c::counter)
{
  if ( ab_channel != 0xFF )
    init(ab_channel, aui16_timebase, ab_activHigh, ab_risingEdge);
}


void
CounterI_c::init(uint8_t ab_channel, uint16_t aui16_timebase, bool ab_activHigh, bool ab_risingEdge )
{
  InputBase_c::init(ab_channel, IsoAgLib::iInput_c::counter);
  // now init the digital input
  const bool r = ( HAL::init_counter(channelNr(), aui16_timebase, ab_activHigh, ab_risingEdge) != HAL_RANGE_ERR);
  isoaglib_assert( r ); (void)r;
}


bool
CounterI_c::active() const
{
  return (val() > 0);
}


uint32_t
CounterI_c::val() const
{
  return HAL::getCounter(channelNr());
}


void
CounterI_c::reset()
{
  HAL::resetCounter(channelNr());
}


uint16_t
CounterI_c::period()
{
  return HAL::getCounterPeriod(channelNr());
}


uint32_t
CounterI_c::frequency()
{
  return HAL::getCounterFrequency(channelNr());
}


uint32_t
CounterI_c::lastSignalAge()
{
  return HAL::getCounterLastSignalAge(channelNr());
}


} // __IsoAgLib
