/*
  inputs_target_extensions.h:
    header for ESXu specific extensions for the HAL for input

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_ESXu_INPUTS_TARGET_EXTENSIONS_H_
#define _HAL_ESXu_INPUTS_TARGET_EXTENSIONS_H_

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_esxu/mos10osy.h>
  }
}

#include <IsoAgLib/hal/esxu/config.h>
#include <IsoAgLib/hal/esxu/typedef.h>
#include <IsoAgLib/hal/esxu/errcodes.h>

             
namespace __HAL {

/* ********************************************* */
/** \name specific BIOS Extenders                */
/*@{*/
/**
  init counter for trigger events on digital input;
  rising edges are counted;
  @param ab_channel input channel to use [0..15]
  @param aui16_timebase timebase to calculate periods, frequency
                     should be at least longer than longest
                     awaited signal period [msec.]
  @param ab_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
  @param ab_risingEdge true -> counter triggers on rising edge; else on falling edge
  @return C_NO_ERR if no error occured
*/
int16_t init_counter(uint8_t ab_channel, uint16_t aui16_timebase, bool ab_activHigh, bool ab_risingEdge);
/**
  get counter value of an digital counter input
  @param ab_channel channel of counter
  @return counter events since init or last reset
*/
uint32_t getCounter(uint8_t ab_channel);
/**
  reset the given counter
  @param ab_channel channel of counter
  @return C_NO_ERR ; C_RANGE if counter for ab_channel isn�t configured properly
*/
int16_t resetCounter(uint8_t ab_channel);
/**
  get period of counter channel
  @param ab_channel channel of counter
  @return time between last two signals or 0xFFFF if time is longer than initially
           given timebase
*/
uint16_t getCounterPeriod(uint8_t ab_channel);
/**
  get frequency of counter channel
  @param ab_channel channel of counter
  @return frequency calculated from time between last two signals
          or 0 if time is longer than initially given timebase
*/
uint32_t getCounterFrequency(uint8_t ab_channel);
/**
 get time since last signal and reset according trigger timers
 if timebase is exceeded -> avoid overflow problems if timer floated around 0xFFFF
 @param ab_channel channel of counter
 @return time since last signal [msec.]
*/
uint16_t getCounterLastSignalAge(uint8_t ab_channel);
/*@}*/
} // end of namespace __HAL
#endif
