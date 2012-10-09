/***************************************************************************
                          sensor.h - definition of Hardware Abstraction
                                     Layer for sensor functions for IMI
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
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

/* ************************************************************ */
/** \file supplementary_driver/hal/imi/sensor/sensor.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adopted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the IMI version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */

#ifndef _HAL_IMI_SENSOR_H_
#define _HAL_IMI_SENSOR_H_

#include <IsoAgLib/hal/imi/config.h>
#include <IsoAgLib/hal/imi/typedef.h>
#include <IsoAgLib/hal/imi/errcodes.h>
#include "sensor_target_extensions.h"

/* ******************************************************** */
/**
 * \name Basic BIOS limit constants
 */
/*@{*/
#define ANALOG_INPUT_MIN 0
// only uncomment following line if  ANALOG_INPUT_MIN > 0
//#define ANALOG_INPUT_MIN_GREATER_ZERO
#define ANALOG_INPUT_MAX 7
#define DIGITAL_INPUT_MIN 0
// only uncomment following line if  DIGITAL_INPUT_MIN > 0
//#define DIGITAL_INPUT_MIN_GREATER_ZERO
#define DIGITAL_INPUT_MAX 15

#define COUNTER_INPUT_MIN 0
// only uncomment following line if  COUNTER_INPUT_MIN > 0
//#define COUNTER_INPUT_MIN_GREATER_ZERO
#define COUNTER_INPUT_MAX 15

/// set maximal voltage input for analog input set to voltage in [mV]
#define MAX_ANALOG_MV 8500
///set max digital BIOS function return value corresponding to MAX_ANALOG_MV
#define MAX_ANALOG_MV_DIGIT 907
/// set maximal current input for analog input set [mA]
#define MAX_ANALOG_MA 20
///set max digital BIOS function return value corresponding to MAX_ANALOG_MA
#define MAX_ANALOG_MA_DIGIT 833

/** define the scale factor to get [mV] from the return value of get_analogin_mean */
/** @todo verify this with STW, as there are differing informations on this */
#define ADC2MILLIVOLT 16

/** define the scale factor to get [mA] from the return value of get_analogin_mean */
/** @todo verify this with STW, as there are differing informations on this */
#define ADC2MILLIAMPERE 20
/*@}*/

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_imi/adis10.h>
  }
}

/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaption to new platform
 */
namespace HAL
{
  /* ********************************* */
  /** \name Sensor_c Input BIOS functions */
/*@{*/

   #define  VOLTAGE_IN               0       /* Define to configure the input as voltage input */
   #define  CURRENT_IN               1       /* Define to configure the input as current input */

  /**
    initialize one of the [0..7] analog input channels to VOLTAGE input
    @param bNumber number of the analog input channel
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t  init_analoginVolt(uint8_t bNumber)
    {__HAL::init_input(bNumber, VOLTAGE_IN);return C_NO_ERR;};
  /**
    initialize one of the [0..7] analog input channels to CURRENT input
    @param bNumber number of the analog input channel
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t  init_analoginCurrent(uint8_t bNumber)
    {__HAL::init_input(bNumber, CURRENT_IN);return C_NO_ERR;};

  /**
    initialize one of the (16 [DIN1..DIN16]) digital input channels
    -> the IMI uses two different functions for digin init:
    a) init_digin set specific input
    b) init_input activates PullUp, PullDown resistors
    --> match HIGH_ACTIV==1 of IMI to PULL_DOWN==2; LOW_ACTIV==0 of IMI to PULL_UP==1
    @param bInput number of the digital input channel  [0..7]
    @param bMode input mode {DIGIN, RISING_EDGE, FALLING_EDGE, BOTH_EDGE,
           HIGH_ACTIV, LOW_ACTIV}
    @param pfFunctionName adress of function which is called on input events (NULL -> none)
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t  init_digin(uint8_t bInput,uint8_t bMode,uint8_t bAktivhighlow,void (*pfFunctionName)())
    {int16_t i16_res = __HAL::init_digin(bInput, bMode, pfFunctionName);
         i16_res |=  __HAL::init_input(bInput, (bMode == 1)?(PULL_DOWN):(PULL_UP));
         return i16_res;};

  /**
    init counter for trigger events on digital inoput;
    rising edges are counted;
    @param rb_channel input channel to use [0..7]
    @param rui16_timebase timebase to calculate periods, frequency
                       should be at least longer than longest
                       awaited signal period [msec.]
    @param rb_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
    @param rb_risingEdge true -> counter triggers on rising edge; else on falling edge
    @return C_NO_ERR if no error occured
  */
  inline int16_t init_counter(uint8_t rb_channel, uint16_t rui16_timebase, bool rb_activHigh, bool rb_risingEdge)
  {return __HAL::init_counter(rb_channel, rui16_timebase, rb_activHigh, rb_risingEdge);};
  /**
    get counter value of an digital counter input
    @param rb_channel channel of counter [0..7]
    @return counter events since init or last reset
  */
  inline uint32_t getCounter(uint8_t rb_channel)
  {return __HAL::getCounter(rb_channel);};
  /**
    reset the given counter
    @param rb_channel channel of counter [0..7]
    @return C_NO_ERR ; C_RANGE if counter for rb_channel isn´t configured properly
  */
  inline int16_t resetCounter(uint8_t rb_channel)
  {return __HAL::resetCounter(rb_channel);};
  /**
    get period of counter channel
    @param rb_channel channel of counter [0..7]
    @return time between last two signals or 0xFFFF if time is longer than initially
             given timebase
  */
  inline uint16_t getCounterPeriod(uint8_t rb_channel)
  {return __HAL::getCounterPeriod(rb_channel);};
  /**
    get frequency of counter channel
    @param rb_channel channel of counter [0..7]
    @return frequency calculated from time between last two signals
            or 0 if time is longer than initially given timebase
  */
  inline uint16_t getCounterFrequency(uint8_t rb_channel)
  {return __HAL::getCounterFrequency(rb_channel);};
  /**
   get time since last signal
   @param rb_channel channel of counter
   @return time since last signal [msec.]
  */
  inline uint32_t getCounterLastSignalAge(uint8_t rb_channel)
  {return __HAL::getCounterLastSignalAge(rb_channel);};
  /**
    set fast ADC mode ON or OFF
    !!! the DIOS-IMI (ADIS) doesn't provide setFastAnalogin !!
    @param bMode set fast ADC to ON or OFF
  */
  inline void setFastAnalogin(bool bMode)
    {return;};

  /**
    get the measured voltage value of a channel in [mV]
    @param rb_channel measured channel
    @return voltage [0..8500] [mV] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcVoltage(uint8_t rb_channel)
    {int16_t i16_temp = __HAL::get_analogin_mean(rb_channel);
     if ( i16_temp == C_RANGE ) return C_RANGE;
     return (i16_temp * ADC2MILLIVOLT);};
  /**
    get the MEDIUM of measured voltage value of a channel in [mV]
    @param rb_channel measured channel
    @return voltage [0..8500] [mV] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcMeanVoltage(uint8_t rb_channel)
    { return getAdcVoltage( rb_channel );};
  /**
    get the measured current value of a channel in [uA]
    @param rb_channel measured channel
    @return current [4000..20000] [uA] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcCurrent(uint8_t rb_channel)
    {int16_t i16_temp = __HAL::get_analogin_mean(rb_channel);
     if ( i16_temp == C_RANGE ) return C_RANGE;
     return (i16_temp * ADC2MILLIAMPERE);};
  /**
    get the MEDIUM of measured current value of a channel in [uA]
    @param rb_channel measured channel
    @return current [4000..20000] [uA] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcMeanCurrent(uint8_t rb_channel)
    { return getAdcCurrent(rb_channel);};
    /**
      deliver state of digital input based on Activ-High/Low setting
      (evalutation of sensor signals independent from switching type)
      @param bInputNumber input channel number [0..7]
      @return ON, OFF or C_RANGE
    */
    inline int16_t  getDiginOnoff(uint8_t bInputNumber)
      {return __HAL::get_digin_onoff(bInputNumber);};

    /**
      deliver debounced state of digital input based on Activ-High/Low setting
      (evalutation of sensor signals independent from switching type)
      @param bInputNumber input channel number [0..7]
      @return ON, OFF or C_RANGE
    */
    inline int16_t  getDiginOnoffStatic(uint8_t bInputNumber)
      {return __HAL::get_digin_onoff_static(bInputNumber);};

    /**
      deliver frequency of digital interrupt channel
      (this function uses default value for timebase for
       calculating frequency == 1,67sec. for 20MHz CPU)
      @param bInputNumber input channel number [DIN1..DIN16]
      @param b_useVirtual (default false) use virtual calculated
             value, if last trigger is lasted longer than last period
             -> virt_val uses time from last trigger to actual time
      @return frequency of triggered events [mHz] or BIOS_WARN on too less impulses
    */
    inline uint16_t getDiginFreq(uint8_t bInputNumber, bool b_useVirtual = false)
      {uint16_t ui16_result;
       return ((__HAL::get_digin_freq(bInputNumber, &ui16_result) == C_NO_ERR) || (b_useVirtual))?ui16_result:0;};

  /*@}*/
}
#endif
