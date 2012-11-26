/*
  inputs.h:
    definition of Hardware Abstraction Layer for input functions for
    debug/simulation system on PC

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file
 * 
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module biosExtenders can be used to implement
   the needed activity. The implementation of the PC version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */

#ifndef _HAL_PC_INPUTS_H_
#define _HAL_PC_INPUTS_H_

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/pc/typedef.h>
#include <IsoAgLib/hal/pc/errcodes.h>
#include "inputs_target_extensions.h"

#include <cstdio>

/* ******************************************************** */
/**
 * \name Basic BIOS limit constants
 */
/*@{*/
#define ANALOG_INPUT_MIN 0
#define ANALOG_INPUT_MAX 7
#define DIGITAL_INPUT_MIN 0
#define DIGITAL_INPUT_MAX 15

#define COUNTER_INPUT_MIN 0
#define COUNTER_INPUT_MAX 15

/// set maximal voltage input for analog input set to voltage in [mV]
#define MAX_ANALOG_MV 8500
///set max digital BIOS function return value corresponding to MAX_ANALOG_MV
#define MAX_ANALOG_MV_DIGIT 907
/// set maximal current input for analog input set [mA]
#define MAX_ANALOG_MA 20
///set max digital BIOS function return value corresponding to MAX_ANALOG_MA
#define MAX_ANALOG_MA_DIGIT 833
/*@}*/

/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaptation to new platform
 */
namespace HAL
{
  /* *************************** */
  /** \name Input BIOS functions */
  /*@{*/
  /**
    initialize one of the [0..7] analog input channels to VOLTAGE input
    @param bNumber number of the analog input channel
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t  init_analoginVolt(uint8_t bNumber)
    {return __HAL::init_analogin(bNumber, VOLTAGE_IN);};
  /**
    initialize one of the [0..7] analog input channels to CURRENT input
    @param bNumber number of the analog input channel
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t  init_analoginCurrent(uint8_t bNumber)
    {return __HAL::init_analogin(bNumber, CURRENT_IN);};


  /**
    initialize one of the (16 [DIN1..DIN16]) digital input channels
    @param bInput number of the digital input channel
    @param bMode input mode {DIGIN, RISING_EDGE, FALLING_EDGE, BOTH_EDGE,
           HIGH_ACTIV, LOW_ACTIV}
    @param pfFunctionName adress of function which is called on input events (NULL -> none)
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t  init_digin(uint8_t bInput,uint8_t bMode,uint8_t bAktivhighlow,void (*pfFunctionName)(...))
    {return __HAL::init_digin(bInput, bMode, bAktivhighlow, pfFunctionName);};

  /**
    init counter for trigger events on digital inoput;
    rising edges are counted;
    @param ab_channel input channel to use [0..15]
    @param aui16_timebase timebase to calculate periods, frequency
                       should be at least longer than longest
                       awaited signal period [msec.]
    @param ab_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
    @param ab_risingEdge true -> counter triggers on rising edge; else on falling edge
    @return C_NO_ERR if no error occured
  */
  inline int16_t init_counter(uint8_t ab_channel, uint16_t aui16_timebase, bool ab_activHigh, bool ab_risingEdge)
  {return __HAL::init_counter(ab_channel, aui16_timebase, ab_activHigh, ab_risingEdge);};
  /**
    get counter value of an digital counter input
    @param ab_channel channel of counter
    @return counter events since init or last reset
  */
  inline uint32_t getCounter(uint8_t ab_channel)
  {return __HAL::getCounter(ab_channel);};
  /**
    reset the given counter
    @param ab_channel channel of counter
    @return C_NO_ERR ; HAL_RANGE_ERR if counter for ab_channel isn�t configured properly
  */
  inline int16_t resetCounter(uint8_t ab_channel)
  {return __HAL::resetCounter(ab_channel);};
  /**
    get period of counter channel
    @return time between last two signals or 0xFFFF if time is longer than initially
             given timebase
  */
  inline uint16_t getCounterPeriod(uint8_t ab_channel)
  {return __HAL::getCounterPeriod(ab_channel);};
  /**
    get frequency of counter channel
    @param ab_channel channel of counter
    @return frequency calculated from time between last two signals
            or 0 if time is longer than initially given timebase
  */
  inline uint32_t getCounterFrequency(uint8_t ab_channel)
  {return __HAL::getCounterFrequency(ab_channel);};
  /**
   get time since last signal
   @param ab_channel channel of counter
   @return time since last signal [msec.]
  */
  inline uint32_t getCounterLastSignalAge(uint8_t ab_channel)
  {return __HAL::getCounterLastSignalAge(ab_channel);};

  /**
    set fast ADC mode ON or OFF
    (important: only the 8 analog inputs are measured during this time,
      --> the other system voltages aren't measured with active fast ADC)
    @param bMode set fast ADC to ON or OFF
  */
  inline void setFastAnalogin(bool bMode)
    {__HAL::setFastAnalogin(bMode);};

  /**
    get the measured voltage value of a channel in [mV]
    @param ab_channel measured channel
    @return voltage [0..8500] [mV] or HAL_RANGE_ERR on wrong input channel number
  */
  inline int16_t  getAdcVoltage(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::getAdc(__HAL::getAnaloginCheckNr(ab_channel));
     if ( i16_temp == HAL_RANGE_ERR ) return HAL_RANGE_ERR;
     // use 1:1 value mappgin for PC test
     // return ((i16_temp * 9) + ((i16_temp * 39)/100));};
     return i16_temp;};
  /**
    get the MEDIUM of measured voltage value of a channel in [mV]
    @param ab_channel measured channel
    @return voltage [0..8500] [mV] or HAL_RANGE_ERR on wrong input channel number
  */
  inline int16_t  getAdcMeanVoltage(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::getAnaloginMean(ab_channel);
     if ( i16_temp == HAL_RANGE_ERR ) return HAL_RANGE_ERR;
     // use 1:1 value mappgin for PC test
     // return ((i16_temp * 9) + ((i16_temp * 39)/100));};
     return i16_temp;};
  /**
    get the measured current value of a channel in [uA]
    @param ab_channel measured channel
    @return current [4000..20000] [uA] or HAL_RANGE_ERR on wrong input channel number
  */
  inline int16_t  getAdcCurrent(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::getAdc(__HAL::getAnaloginCheckNr(ab_channel));
     if ( i16_temp == HAL_RANGE_ERR ) return HAL_RANGE_ERR;
     // use 1:1 value mappgin for PC test
     // return ((i16_temp * 24) + ((i16_temp * 41)/100));};
     return i16_temp;};
  /**
    get the MEDIUM of measured current value of a channel in [uA]
    @param ab_channel measured channel
    @return current [4000..20000] [uA] or HAL_RANGE_ERR on wrong input channel number
  */
  inline int16_t  getAdcMeanCurrent(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::getAnaloginMean(ab_channel);
     if ( i16_temp == HAL_RANGE_ERR ) return HAL_RANGE_ERR;
     // use 1:1 value mappgin for PC test
     // return ((i16_temp * 24) + ((i16_temp * 41)/100));};
     return i16_temp;};
  /**
    get the diagnose ADC value from specified DIGITAL INPUT channel
    @param ab_channel channel number [0..15]
    @return ADC diagnose voltage [ or HAL_RANGE_ERR on wrong input channel number
  */
  inline int16_t  getDiginDiagnoseAdc(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::getAdc(__HAL::getDiginAdcCheckNr(ab_channel));
     if ( i16_temp == HAL_RANGE_ERR ) return HAL_RANGE_ERR;
     // use 1:1 value mappgin for PC test
     // return ((i16_temp * 8) + ((i16_temp * 43)/100));};
     return i16_temp;};
  /**
    get the connection state of case mass
    @return true == case is connected to ground
  */
  inline bool  getAdcVirtMass( void )
    {return (__HAL::getAdc(GET_M_VIRT) < 440 );};
  /**
    get the temperature
    @return temperature in degree [-40..85]
  */
  inline int16_t  getAdcTemp( void )
    {int16_t i16_temp = __HAL::getAdc(GET_D_T);
     return ((100*(i16_temp - 273))/(-83));};
  /**
    get the threshold value
    @return threshold voltage [mV]
  */
  inline int16_t  getAdcThreshold( void )
    {int16_t i16_temp = __HAL::getAdc(GET_U_THRESHOLD);
     return ((i16_temp * 4) + ((i16_temp * 94)/100));};
  /**
    get the voltage at the PIC
    @return PIC voltage [mV]
  */
  inline int16_t  getAdcPic( void )
    {int16_t i16_temp = __HAL::getAdc(GET_U_PIC);
     return ((i16_temp * 4) + ((i16_temp * 94)/100));};
  /**
    get the complete sum of all currents
    @return sum of all currents [mA]
  */
  inline int16_t  getAdcCurrentSum( void )
    {int16_t i16_temp = __HAL::getAdc(GET_SUM_I);
     return ((i16_temp * 24) + ((i16_temp * 41)/100));};


  /**
    deliver state of digital input based on Activ-High/Low setting
    (evalutation of input signals independent from switching type)
    @param bInputNumber input channel number [DIN1..DIN16]
    @return ON, OFF or HAL_RANGE_ERR
  */
  inline int16_t  getDiginOnoff(uint8_t bInputNumber)
    {return __HAL::getDiginOnoff(bInputNumber);};

  /**
    deliver debounced state of digital input based on Activ-High/Low setting
    (evalutation of input signals independent from switching type)
    @param bInputNumber input channel number [DIN1..DIN16]
    @return ON, OFF or HAL_RANGE_ERR
  */
  inline int16_t  getDiginOnoffStatic(uint8_t bInputNumber)
    {return __HAL::getDiginOnoffStatic(bInputNumber);};

  /**
    deliver channel number for checking/requesting of
    ADC value at UBAT
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @return according channel number for __HAL call
  */
  inline uint8_t getUbatAdcCheckNr(){return GET_U_C;};

  /**
    deliver channel number for checking/requesting of
    ADC value at EXT8,5V
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @return according channel number for __HAL call
  */
  inline uint8_t getU85AdcCheckNr(){return GET_U_EXT_8_5_V;};
  /*@}*/
}
#endif
