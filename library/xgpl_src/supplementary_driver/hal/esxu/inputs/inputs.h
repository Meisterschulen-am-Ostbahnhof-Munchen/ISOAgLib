/*
  inputs.h: definition of Hardware Abstraction Layer for input
    functions for ESXu

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_ESXu_INPUTS_H_
#define _HAL_ESXu_INPUTS_H_

#include <IsoAgLib/isoaglib_config.h>
#include "inputs_target_extensions.h"

/* ******************************************************** */
/**
 * \name Basic BIOS limit constants
 */
/*@{*/
#define ANALOG_INPUT_MIN AIN1
// only uncomment following line if  ANALOG_INPUT_MIN > 0
#if (ANALOG_INPUT_MIN > 0)
#  define ANALOG_INPUT_MIN_GREATER_ZERO
#endif
#define ANALOG_INPUT_MAX AIN8

#define DIGITAL_INPUT_MIN DIN1
// only uncomment following line if  DIGITAL_INPUT_MIN > 0
#if (DIGITAL_INPUT_MIN > 0)
#  define DIGITAL_INPUT_MIN_GREATER_ZERO
#endif
#define DIGITAL_INPUT_MAX DIN10

#define COUNTER_INPUT_MIN DIN1
// only uncomment following line if  COUNTER_INPUT_MIN > 0
#if (COUNTER_INPUT_MIN > 0)
#  define COUNTER_INPUT_MIN_GREATER_ZERO
#endif
#define COUNTER_INPUT_MAX DIN10

/// set maximal voltage input for analog input set to voltage in [mV]
#define MAX_ANALOG_MV 30000
///set max digital BIOS function return value corresponding to MAX_ANALOG_MV
#define MAX_ANALOG_MV_DIGIT 1000
/// set maximal current input for analog input set [mA]
#define MAX_ANALOG_MA 25
///set max digital BIOS function return value corresponding to MAX_ANALOG_MA
#define MAX_ANALOG_MA_DIGIT 1000
/*@}*/

namespace __HAL {
  /**
    deliver channel number for checking/requesting of analog input
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @param channel number from interval [0..maxNo] == [0..7]
    @return according channel number for __HAL call
  */
  inline uint8_t getAnaloginCheckNr(uint8_t channel)
    {return (GET_A_IN_1 - channel);}

  /**
    deliver channel number for checking/requesting of
    ADC value at digital input
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @param ab_channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getDiginAdcCheckNr(uint8_t ab_channel)
    {return getAnaloginCheckNr( ab_channel );}
}

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
    {
    int16_t retval = __HAL::init_analogin(bNumber, VOLTAGE_IN);

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " << "init_analogin( "
<< (uint16_t)bNumber << ", "
<< (uint16_t)VOLTAGE_IN << ") returns " << retval << "\r";
#endif

    return retval;}

  /**
    initialize one of the [0..7] analog input channels to CURRENT input
    @param bNumber number of the analog input channel
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t  init_analoginCurrent(uint8_t bNumber)
    {
    int16_t retval = __HAL::init_analogin(bNumber, CURRENT_IN);

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " << "init_analogin( "
<< (uint16_t)bNumber << ", "
<< (uint16_t)CURRENT_IN << ") returns " << retval << "\r";
#endif

    return retval;}

  /**
    initialize one of the [0..7] digital input channels
    @param ab_channel number of the digital input channel
    @param bMode input mode {DIGIN, RISING_EDGE, FALLING_EDGE, BOTH_EDGE}
    @param bAktivhighlow {HIGH_ACTIV, LOW_ACTIV}
    @param pfFunctionName adress of function which is called on input events (NULL -> none)
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t  init_digin(uint8_t ab_channel,uint8_t bMode,uint8_t bAktivhighlow,void (*pfFunctionName)())
    {
  	int16_t retval = __HAL::init_digin(ab_channel, bMode, bAktivhighlow, pfFunctionName);

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " << "init_digin( "
<< (uint16_t)ab_channel << ", "
<< (uint16_t)bMode << ", "
<< (uint16_t)bAktivhighlow << ", "
<< (pfFunctionName?"pfFunctionName":"NULL") << ") returns " << retval << "\r";
#endif

    return retval;}

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
  {return __HAL::init_counter(ab_channel, aui16_timebase, ab_activHigh, ab_risingEdge);}

  /**
    get counter value of an digital counter input
   @param ab_channel channel of counter [0..15]
    @return counter events since init or last reset
  */
  inline uint32_t getCounter(uint8_t ab_channel)
  {return __HAL::getCounter(ab_channel);}

  /**
    reset the given counter
   @param ab_channel channel of counter [0..15]
    @return C_NO_ERR ; C_RANGE if counter for ab_channel isn�t configured properly
  */
  inline int16_t resetCounter(uint8_t ab_channel)
  {return __HAL::resetCounter(ab_channel);}

  /**
    get period of counter channel
    @param ab_channel channel of counter [0..15]
    @return time between last two signals or 0xFFFF if time is longer than initially
             given timebase
  */
  inline uint16_t getCounterPeriod(uint8_t ab_channel)
  {return __HAL::getCounterPeriod(ab_channel);}

  /**
    get frequency of counter channel
   @param ab_channel channel of counter [0..15]
    @return frequency calculated from time between last two signals
            or 0 if time is longer than initially given timebase
  */
  inline uint32_t getCounterFrequency(uint8_t ab_channel)
  {  return __HAL::getCounterFrequency(ab_channel); }

  /**
   get time since last signal
   @param ab_channel channel of counter
   @return time since last signal [msec.]
  */
  inline uint32_t getCounterLastSignalAge(uint8_t ab_channel)
  {  return __HAL::getCounterLastSignalAge(ab_channel); }

  /**
    set fast ADC mode ON or OFF
    !!! the ESXu doesn't provide setFastAnalogin !!
    @param bMode set fast ADC to ON or OFF
  */
  inline void setFastAnalogin(bool bMode)
    {return;}

  /**
    get the measured voltage value of a channel in [mV]
    @param ab_channel measured channel
    @return voltage [0..8500] [mV] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcVoltage(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::get_adc(__HAL::getAnaloginCheckNr(ab_channel));

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " << "get_adc( "
<< (uint16_t)__HAL::getAnaloginCheckNr(ab_channel) << ") returns " << i16_temp << "\r";
#endif

     if ( i16_temp == C_RANGE ) return C_RANGE;
     return (i16_temp * 10);}

  /**
    get the MEDIUM of measured voltage value of a channel in [mV]
    @param ab_channel measured channel
    @return voltage [0..8500] [mV] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcMeanVoltage(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::get_adc_mean(__HAL::getAnaloginCheckNr(ab_channel));

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " << "get_adc_mean( "
<< (uint16_t)__HAL::getAnaloginCheckNr(ab_channel) << ") returns " << i16_temp << "\r";
#endif

     if ( i16_temp == C_RANGE ) return C_RANGE;
     return (i16_temp * 10);}

  /**
    get the measured current value of a channel in [uA]
    @param ab_channel measured channel
    @return current [4000..20000] [uA] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcCurrent(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::get_adc(__HAL::getDiginAdcCheckNr(ab_channel));

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " << "get_adc( "
<< (uint16_t)__HAL::getDiginAdcCheckNr(ab_channel) << ") returns " << i16_temp << "\r";
#endif

     if ( i16_temp == C_RANGE ) return C_RANGE;
     return (i16_temp * 25);}

  /**
    get the MEDIUM of measured current value of a channel in [uA]
    @param ab_channel measured channel
    @return current [4000..20000] [uA] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcMeanCurrent(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::get_adc_mean(__HAL::getDiginAdcCheckNr(ab_channel));

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " << "get_adc_mean( "
<< (uint16_t)__HAL::getDiginAdcCheckNr(ab_channel) << ") returns " << i16_temp << "\r";
#endif

     if ( i16_temp == C_RANGE ) return C_RANGE;
     return (i16_temp * 25);}

  /**
    get the diagnose ADC value from specified DIGITAL INPUT channel
    @param ab_channel channel number [0..7]
    @return ADC diagnose voltage [ or C_RANGE on wrong input channel number
  */
  inline int16_t  getDiginDiagnoseAdc(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::get_adc(__HAL::getDiginAdcCheckNr(ab_channel));

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " << "get_adc( "
<< (uint16_t)__HAL::getDiginAdcCheckNr(ab_channel) << ") returns " << i16_temp << "\r";
#endif

     if ( i16_temp == C_RANGE ) return C_RANGE;
     return (i16_temp * 10);}

  /**
    get the temperature
    @return temperature in degree [-40..120 C]
  */
  inline int16_t  getAdcTemp( void )
    {
	__HAL::t_Sys_AnalogData t_Sys_AnalogData;
	__HAL::get_system_analogdata(&t_Sys_AnalogData);

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - "
<< "get_system_analogdata( &t_Sys_AnalogData ) " << ", iTemperatur = " << t_Sys_AnalogData.iTemperatur << "\r";
#endif

    return t_Sys_AnalogData.iTemperatur;
    }
   #if !defined(GET_U_THRESHOLD) && defined(GET_U_2_3_V)
     #define GET_U_THRESHOLD GET_U_2_3_V
   #endif

  /**
    deliver state of digital input based on Activ-High/Low setting
    (evalutation of input signals independent from switching type)
    @param ab_channelNumber input channel number [DIN1..DIN16]
    @return ON, OFF or C_RANGE
  */
  inline int16_t  getDiginOnoff(uint8_t ab_channelNumber)
    { int16_t retval = __HAL::get_digin_onoff(ab_channelNumber);

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " << "get_digin_onoff( "
<< (uint16_t)ab_channelNumber << ") returns " << retval << "\r";
#endif

  return retval;}

  /**
    deliver debounced state of digital input based on Activ-High/Low setting
    (evalutation of input signals independent from switching type)
    @param ab_channelNumber input channel number [DIN1..DIN16]
    @return ON, OFF or C_RANGE
  */
  inline int16_t  getDiginOnoffStatic(uint8_t ab_channelNumber)
    {
    int16_t retval = __HAL::get_digin_onoff_static(ab_channelNumber);

#if DEBUG_HAL
INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " << "get_digin_onoff_static( "
<< (uint16_t)ab_channelNumber << ") returns " << retval << "\r";
#endif

    return retval;
  }
}
#endif
