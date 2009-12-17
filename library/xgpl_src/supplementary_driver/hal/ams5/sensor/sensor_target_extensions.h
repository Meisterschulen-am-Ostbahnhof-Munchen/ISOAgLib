/*
  sensor_target_extensions.h: header for AMS5 specific extensions

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_AMS5_SENSOR_TARGET_EXTENSIONS_H_
#define _HAL_AMS5_SENSOR_TARGET_EXTENSIONS_H_
   
#include "..\..\..\..\IsoAgLib\hal\ams5\typedef.h"
#include "..\..\..\..\IsoAgLib\hal\ams5\errcodes.h"
#include "..\..\..\..\IsoAgLib\hal\ams5\config.h"

namespace __HAL
{
   /* ********************************* */
   /** \name Sensor_c Input BIOS functions */
   /*@{*/

   /**
      init counter inputs

      Use symbolic counter input channels form "config.h"!
      For "event counter mode" set aui16_timebase to "0".

      @param ab_channel input channel to use 
      @param aui16_timebase timebase to calculate periods, frequency
                            should be at least longer than longest
                            awaited signal period [msec.]
      @param ab_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
      @param ab_risingEdge true -> counter triggers on rising edge; else on falling edge
      @return HAL_NO_ERR if no error occured
   */
   int16_t init_counter(uint8_t ab_channel, uint16_t aui16_timebase, bool ab_activHigh, bool ab_risingEdge);

   /**
      get counter value of an digital counter input - "event counter mode"

      Use symbolic counter input channels form "config.h"!

      @param ab_channel channel of counter
      @return counter events since init or last reset
   */
   uint32_t getCounter(uint8_t ab_channel);

   /**
      reset the given counter - "event counter mode"

      Use symbolic counter input channels form "config.h"!

      @param ab_channel channel of counter
      @return C_NO_ERR ; HAL_RANGE_ERR if counter for ab_channel isn´t configured properly
   */
   int16_t resetCounter(uint8_t ab_channel);

   /**
      get period of counter channel - frequency/period measuring mode

      Use symbolic counter input channels form "config.h"!

      @param ab_channel channel of counter
      @return time between last two signals or 0xFFFF if time is longer than initially
              given timebase
   */
   uint16_t getCounterPeriod(uint8_t ab_channel);

   /**
      get frequency of counter channel - frequency/period measuring mode

      Use symbolic counter input channels form "config.h"!

      @param ab_channel channel of counter
      @return frequency calculated from time between last two signals
              or 0 if time is longer than initially given timebase
   */
   uint16_t getCounterFrequency(uint8_t ab_channel);

   /**
      get time since last signal - frequency/period measuring mode

      Also availabel for interrupt input channels: cntX19_5,  cntX19_6,  cntX21_5.
      See "__HAL::init_counter() and "AMSBIOS::io_initIrqInputs()" for details.   

      !!! this is a "counter function" -> use symbolic counter input channels only !!!

      @param ab_channel channel of counter
      @return time since last signal [msec.]
   */
   uint32_t getCounterLastSignalAge(uint8_t ab_channel);

   /**
      get the measured voltage value of a channel in [mV]

      0...1023 = 0...2500mV
      !!! remove jumper corresponding to channel !!!     

      @param ab_channel measured channel
      @return voltage [0..2500] [mV] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t getAdcVoltage(uint8_t ab_channel);

   /**
      get the MEDIUM of measured voltage value of a channel in [mV]

      0...1023 = 0...2500mV
      !!! remove jumper corresponding to channel !!!     

      @param ab_channel measured channel
      @return voltage [0..2500] [mV] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t getAdcMeanVoltage(uint8_t ab_channel);

   /**
      get the measured current value of a channel in [uA]

      0...957 = 0...20mA
      !!! set jumper corresponding to channel in position 2-3 !!!     

      @param ab_channel measured channel
      @return current [0..20000] [uA] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t  getAdcCurrent(uint8_t ab_channel);

   /**
      get the MEDIUM of measured current value of a channel in [uA]

      0...957 = 0...20mA
      !!! set jumper corresponding to channel in position 2-3 !!!     

      @param ab_channel measured channel
      @return current [0..20000] [uA] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t  getAdcMeanCurrent(uint8_t ab_channel);

   /**
      deliver state of symbolic digital input channels

      - static digital inputs (X7/X8)
      - analog inputs used as digital inputs (X5/X6)
      - counter and interrupt inputs used as digital inputs (X19/X20)
      - DIP switches (SW1)

      !!! use symbolic input channels only !!!
      !!! analog input: set jumper corresponding to channel in position 1-2 !!!     

      @param bInputNumber input channel number
      @return ON, OFF or HAL_RANGE_ERR
   */
   int16_t get_digin_onoff(uint8_t bInputNumber);

   /**
      deliver debounced state of symbolic digital input channels

      - static digital inputs (X7/X8)
      - analog inputs used as digital inputs (X5/X6)
      - counter and interrupt inputs used as digital inputs (X19/X20)
      - DIP switches (SW1)

      !!! use symbolic input channels only !!!
      !!! analog input: set jumper corresponding to channel in position 1-2 !!!     

      @param bInputNumber input channel number
      @return ON, OFF or HAL_RANGE_ERR
   */
   int16_t get_digin_onoff_static(uint8_t bInutNumber);

   /*@}*/
} 
#endif
