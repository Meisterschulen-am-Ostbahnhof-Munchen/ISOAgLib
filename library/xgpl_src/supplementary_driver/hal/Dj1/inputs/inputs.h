/*
  inputs.h: definition of Hardware Abstraction Layer for sensor functions for Dj Hardware

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_DJ1_INPUTS_H_
#define _HAL_DJ1_INPUTS_H_

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/Dj1/typedef.h>
#include <IsoAgLib/hal/Dj1/errcodes.h>


/* ******************************************************** */
/**
 * \name Basic BIOS limit constants
 */
#define ANALOG_INPUT_MIN  0
#define ANALOG_INPUT_MAX  15

#define DIGITAL_INPUT_MIN 0
#define DIGITAL_INPUT_MAX 17

#define COUNTER_INPUT_MIN 0
#define COUNTER_INPUT_MAX 17


namespace __HAL
{
  extern "C"
  {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_Dj1/DjBios1.h>
  }
}

namespace HAL
{
  /* ********************************* */
  /** \name Sensor_c Input BIOS functions */
/*@{*/
  #define DIGIN                  0   /* = 0; digital inputg*/
  #define RISING_EDGE            1   /* = 1; interrupt on rising edge*/
  #define FALLING_EDGE           2   /* = 2; interrupt on falling edge*/
  #define BOTH_EDGE              3   /* = 3; interrupt on both edges*/

  #define HIGH_ACTIV             1   /* = 1; input for aktiv high signals */
  #define LOW_ACTIV              0   /* = 0; input for aktiv low signals */

  inline int16_t init_analoginVolt ( uint8_t bNumber, bool bitPullDownResistor = false )
  {
    /* Selectable Pull down resistors are not supported */
    return ( (__HAL::DjBios_AnalogInit(bNumber) == __HAL::BIOS_ANALOG_NO_ERR) ? HAL_NO_ERR : HAL_RANGE_ERR );
  };


  inline int16_t  init_analoginCurrent ( uint8_t bNumber )
  {
    return HAL_CONFIG_ERR;      /* Not Supported */
  };


  inline int16_t init_digin ( uint8_t ab_channel, uint8_t bMode,
                              uint8_t bAktivhighlow, void (*pfFunctionName)() )
  {
    __HAL::enum_DigMode eMode;

    if ( bMode == RISING_EDGE )
    {
      eMode = __HAL::BIOS_DIG_INT_RISING;
    } /* end if() */
    else if ( bMode == FALLING_EDGE )
    {
      eMode = __HAL::BIOS_DIG_INT_FALLING;
    } /* end if() */
    else if ( bMode == BOTH_EDGE )
    {
      eMode = __HAL::BIOS_DIG_INT_BOTH;
    } /* end if() */
    else               // DIGIN
    {
      eMode = __HAL::BIOS_DIG_IO;
    } /* end else() */
    return ( (__HAL::DjBios_DigInit ( ab_channel, eMode,
           (bAktivhighlow == HIGH_ACTIV)? __HAL::BIOS_DIG_HIGH : __HAL::BIOS_DIG_LOW,
      pfFunctionName) == __HAL::BIOS_DIG_NO_ERR) ? HAL_NO_ERR : HAL_CONFIG_ERR );
  }


  inline int16_t init_counter ( uint8_t ab_channel, uint16_t aui16_timebase,
                                    bool ab_activHigh, bool ab_risingEdge )
  {
    return ( (__HAL::DjBios_CounterInit ( ab_channel, aui16_timebase,
                             ab_activHigh ? __HAL::BIOS_DIG_HIGH : __HAL::BIOS_DIG_LOW,
                             ab_risingEdge? __HAL::BIOS_DIG_HIGH : __HAL::BIOS_DIG_LOW) == __HAL::BIOS_DIG_NO_ERR) ? HAL_NO_ERR : HAL_CONFIG_ERR );
  };


  inline uint32_t getCounter ( uint8_t ab_channel )
  {
    return ( __HAL::DjBios_CounterGetValue(ab_channel) );
  };


  inline int16_t resetCounter ( uint8_t ab_channel )
  {
    return ( (__HAL::DjBios_CounterReset(ab_channel) == __HAL::BIOS_DIG_NO_ERR) ? HAL_NO_ERR : HAL_CONFIG_ERR );
  };


  inline uint32_t getCounterPeriod_us ( uint8_t ab_channel )
  {
    return ( __HAL::DjBios_CounterGetPeriod(ab_channel) * 1000 );
  };


  inline uint32_t getCounterFrequency ( uint8_t ab_channel )
  {
    return ( __HAL::DjBios_CounterGetFreq(ab_channel) );
  };


  inline uint32_t getCounterLastSignalAge(uint8_t ab_channel)
  {
    return ( __HAL::DjBios_CounterGetAge(ab_channel) );
  };


  inline void setFastAnalogin ( bool bMode )
  {
    return;  /* Not supported */
  };


  inline int16_t  getAdcVoltage ( uint8_t ab_channel )
  {
    int16_t Value;

    Value = __HAL::DjBios_AnalogGetVolts ( ab_channel );
    return ( Value < 0 ? HAL_RANGE_ERR : Value );
  };


  inline int16_t  getAdcMeanVoltage ( uint8_t ab_channel )
  {
    int16_t Value;

    Value = __HAL::DjBios_AnalogGetVoltsFilter ( ab_channel );
    return ( Value < 0 ? HAL_RANGE_ERR : Value );
  };


  inline int16_t getAdcCurrent ( uint8_t ab_channel )
  {
    return HAL_RANGE_ERR;  /* Not supported */
  };


  inline int16_t  getAdcMeanCurrent ( uint8_t ab_channel )
  {
    return HAL_RANGE_ERR;   /* Not supported */
  };


  inline int16_t  getDiginDiagnoseAdc ( uint8_t ab_channel )
  {
    return HAL_RANGE_ERR;   /* Not supported */
  };


  inline int16_t  getAdcTemp ( void )
  {
    return ( 0 );
  };


  inline int16_t getDiginOnoff ( uint8_t ab_channelNumber )
  {
    int16_t Value = __HAL::DjBios_DigGetState ( ab_channelNumber );
    if ( Value == __HAL::BIOS_DIG_HIGH )
    {
      return ( ON );
    } /* end if() */
    if ( Value == __HAL::BIOS_DIG_LOW )
    {
      return ( OFF );
    } /* end if() */
    else /* if ( Value == BIOS_DIG_BAD_CHAN ) */
    {
      return ( HAL_RANGE_ERR );
    } /* end if() */
  };


  inline int16_t getDiginOnoffStatic ( uint8_t ab_channelNumber )
  {
    int16_t Value = __HAL::DjBios_DigGetStateDebounce ( ab_channelNumber );
    if ( Value == __HAL::BIOS_DIG_HIGH )
    {
      return ( ON );
    } /* end if() */
    if ( Value == __HAL::BIOS_DIG_LOW )
    {
      return ( OFF );
    } /* end if() */
    else /* if ( Value == BIOS_DIG_BAD_CHAN ) */
    {
      return ( HAL_RANGE_ERR );
    } /* end if() */
  };

  /*@}*/
}

#endif
