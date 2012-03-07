/*
  inputs_target_extensions.h: header for C2C specific extensions for
    the HAL for input

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file hal/c2c/inputs/inputs_target_extensions.h
 * A module targetExtensions should be used
 * for all methods, which can't be simply 
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
 * ********************************************************** */

#ifndef _HAL_C2C_INPUTS_TARGET_EXTENSIONS_H_
#define _HAL_C2C_INPUTS_TARGET_EXTENSIONS_H_

#include <IsoAgLib/hal/c2c/typedef.h>
#include <IsoAgLib/hal/c2c/errcodes.h>


namespace __HAL {

/* ********************************************* */
/** \name specific BIOS Extenders                */
/*@{*/
/**
  init counter for trigger events on digital input;
  rising edges are counted;
  @param aui16_timebase timebase to calculate periods, frequency
                     should be at least longer than longest
                     awaited signal period [msec.]
  @param ab_risingEdge true -> counter triggers on rising edge; else on falling edge
  @return C_NO_ERR if no error occured
*/
int16_t init_counter(uint16_t aui16_timebase, bool ab_risingEdge);
/**
  get counter value of an digital counter input
  @return counter events since init or last reset
*/
uint32_t getCounter();
/**
  reset the given counter
  @return C_NO_ERR ; C_RANGE if counter if it isn�t configured properly
*/
int16_t resetCounter();
/**
  get period of counter channel
  @return time between last two signals or 0xFFFF if time is longer than initially
           given timebase
*/
uint16_t getCounterPeriod();
/**
  get frequency of counter channel
  @return frequency calculated from time between last two signals
          or 0 if time is longer than initially given timebase
*/
uint32_t getCounterFrequency();
/**
 get time since last signal and reset according trigger timers
 if timebase is exceeded -> avoid overflow problems if timer floated around 0xFFFF
 @return time since last signal [msec.]
*/
uint16_t getCounterLastSignalAge();
/*@}*/
} // end of namespace __HAL
#endif
