/***************************************************************************
                          counter_i.cc  -  implementation file for CounterI_c,
                                           an object for digital sensor input
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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
#include "counteri_c.h"
#include "sensori_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  internal called constructor for a new digital input channel which performs configuration of hardware
  (uses BIOS function)
  
  possible errors:
      * LibErr_c::Range wrong input number
  @see SensorI_c::createCounter
  @param rb_channel default-argument for setting hardware channel for this input
  @param rui16_timebase default-argument for setting the timebase which should be
          greater than max time distance between signals and should be small
          enough to avoid overflow of signals in one timebase
  @param rb_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
  @param rb_risingEdge true -> counter triggers on rising edge; else on falling edge
*/
CounterI_c::CounterI_c(uint8_t rb_channel, uint16_t rui16_timebase, bool rb_activHigh, bool rb_risingEdge)
  : SensorBase_c(rb_channel, Sensor_c::counter){
  if ( rb_channel != 0xFF ) init(rb_channel, rui16_timebase, rb_activHigh, rb_risingEdge);
}
/**
  internal called constructor for a new digital input channel which performs configuration of hardware
  (uses BIOS function)

  possible errors:
      * LibErr_c::Range wrong input number
  @see SensorI_c::createCounter
  @param rb_channel default-argument for setting hardware channel for this input
  @param rui16_timebase default-argument for setting the timebase which should be
          greater than max time distance between signals and should be small
          enough to avoid overflow of signals in one timebase
  @param rb_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
  @param rb_risingEdge true -> counter triggers on rising edge; else on falling edge
*/
void CounterI_c::init(uint8_t rb_channel, uint16_t rui16_timebase, bool rb_activHigh, bool rb_risingEdge )
{
  SensorBase_c::init(rb_channel, Sensor_c::counter);
  // now init the digital input
  if (HAL::init_counter(channelNr(), rui16_timebase, rb_activHigh, rb_risingEdge) == HAL_RANGE_ERR)
  { // wrong input channel no
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Sensor );
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
  return (val() > 0)?true:false;
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
      * LibErr_c::Range wrong input number
*/
void CounterI_c::reset()
{
  if (HAL::resetCounter(channelNr()))
  { // wrong input channel no
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Sensor );
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
