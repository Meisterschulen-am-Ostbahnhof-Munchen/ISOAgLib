/***************************************************************************
    sensor.h - definition of HAL namespace layer for AMS5

    -------------------
    date                 : 18.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Header
 ***************************************************************************/

#ifndef _HAL_AMS5_SENSOR_H_
#define _HAL_AMS5_SENSOR_H_
   
#include "sensor_target_extensions.h"

#ifndef DIGITAL_INPUT_MIN
  #define DIGITAL_INPUT_MIN 0
#endif

#ifndef ANALOG_INPUT_MIN
  #define ANALOG_INPUT_MIN 0
#endif

#ifndef COUNTER_INPUT_MIN
  #define COUNTER_INPUT_MIN 0
#endif

#ifndef DIGITAL_INPUT_MAX
  #define DIGITAL_INPUT_MAX 15
#endif

#ifndef ANALOG_INPUT_MAX
  #define ANALOG_INPUT_MAX 7
#endif

#ifndef COUNTER_INPUT_MAX
  #define COUNTER_INPUT_MAX 15
#endif

#ifndef DIGIN
  #define DIGIN 0
#endif

#ifndef BOTH_EDGE
  #define BOTH_EDGE 3
#endif


namespace HAL
{
   /* ********************************* */
   /** \name Sensor_c Input BIOS functions */
   /*@{*/

   /**
      initialize one of the analog input channels to VOLTAGE input

      dummy function - Not Supported

      !!! voltage input: remove jumper corresponding to analog input channel !!!     

      @param bNumber number of the analog input channel
      @return error state HAL_CONFIG_ERR
   */
   inline int16_t  init_analoginVolt(uint8_t bNumber)
   {
      return HAL_CONFIG_ERR;
   };

   /**
      initialize one of the analog input channels to CURRENT input

      dummy function - Not Supported

      !!! current input: set jumper corresponding to analog input channel in position 2-3 !!!     

      @param bNumber number of the analog input channel
      @return error state HAL_CONFIG_ERR
   */
   inline int16_t  init_analoginCurrent(uint8_t bNumber)
   {
      return HAL_CONFIG_ERR;
   };

   /**
      initialize one of the digital input channels

      dummy function - Not Supported

      All digital inputs are low active !
      If high activ inputs needed, please contact GESAS GmbH.

      Function call and input modes are availabel on interrupt input channels:
      irqX19_5,  irqX19_6,  irqX21_5.
      See "__HAL::init_counter() and "AMSBIOS::io_initIrqInputs()" for details.   

      @param rb_channel number of the digital input channel
      @param bMode input mode {DIGIN, RISING_EDGE, FALLING_EDGE, BOTH_EDGE}
      @param bAktivhighlow {HIGH_ACTIV, LOW_ACTIV}
      @param pfFunctionName adress of function which is called on input events (NULL -> none)
      @return error state HAL_CONFIG_ERR
   */
   inline int16_t init_digin(uint8_t rb_channel,uint8_t bMode,uint8_t bAktivhighlow,void (*pfFunctionName)(...))
   {
      return HAL_CONFIG_ERR;
   };

   /**
      init counter inputs

      Use symbolic counter input channels form "config.h"!
      For "event counter mode" set rui16_timebase to "0".

      @param rb_channel input channel to use 
      @param rui16_timebase timebase to calculate periods, frequency
                            should be at least longer than longest
                            awaited signal period [msec.]
      @param rb_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
      @param rb_risingEdge true -> counter triggers on rising edge; else on falling edge
      @return HAL_NO_ERR if no error occured
   */
   inline int16_t init_counter(uint8_t rb_channel, uint16_t rui16_timebase, bool rb_activHigh, bool rb_risingEdge)
   {
      return __HAL::init_counter(rb_channel, rui16_timebase, rb_activHigh, rb_risingEdge);
   };

   /**
      get counter value of an digital counter input - "event counter mode"

      Use symbolic counter input channels form "config.h"!

      @param rb_channel channel of counter
      @return counter events since init or last reset
   */
   inline uint32_t getCounter(uint8_t rb_channel)
   {
      return __HAL::getCounter(rb_channel);
   };

   /**
      reset the given counter - "event counter mode"

      Use symbolic counter input channels form "config.h"!

      @param rb_channel channel of counter
      @return C_NO_ERR ; HAL_RANGE_ERR if counter for rb_channel isn´t configured properly
   */
   inline int16_t resetCounter(uint8_t rb_channel)
   {
      return __HAL::resetCounter(rb_channel);
   };

   /**
      get period of counter channel - frequency/period measuring mode

      Use symbolic counter input channels form "config.h"!

      @param rb_channel channel of counter
      @return time between last two signals or 0xFFFF if time is longer than initially
              given timebase
   */
   inline uint16_t getCounterPeriod(uint8_t rb_channel)
   {
      return __HAL::getCounterPeriod(rb_channel);
   };

   /**
      get frequency of counter channel - frequency/period measuring mode

      Use symbolic counter input channels form "config.h"!

      @param rb_channel channel of counter
      @return frequency calculated from time between last two signals
              or 0 if time is longer than initially given timebase
   */
   inline uint16_t getCounterFrequency(uint8_t rb_channel)
   {
      return __HAL::getCounterFrequency(rb_channel);
   };

   /**
      get time since last signal - frequency/period measuring mode

      Also availabel for interrupt input channels: cntX19_5,  cntX19_6,  cntX21_5.
      See "__HAL::init_counter() and "AMSBIOS::io_initIrqInputs()" for details.   

      Use symbolic counter input channels form "config.h"!

      @param rb_channel channel of counter
      @return time since last signal [msec.]
   */
   inline uint32_t getCounterLastSignalAge(uint8_t rb_channel)
   {
      return __HAL::getCounterLastSignalAge(rb_channel);
   };

   /**
      set fast ADC mode ON or OFF

      dummy function - Not Supported

      All analog input channels will be updated by BIOS-Timer every 1ms.
      If higher rate is needed, please contact GESAS GmbH.

      @param bMode set fast ADC to ON or OFF
   */
   inline void setFastAnalogin(bool bMode)
   {
      return;
   };

   /**
      get the measured voltage value of a channel in [mV]

      0...1023 = 0...2500mV
      !!! remove jumper corresponding to channel !!!     

      @param rb_channel measured channel
      @return voltage [0..2500] [mV] or HAL_RANGE_ERR on wrong input channel number
   */
   inline int16_t getAdcVoltage(uint8_t rb_channel)
   {
      return __HAL::getAdcVoltage(rb_channel);
   };

   /**
      get the MEDIUM of measured voltage value of a channel in [mV]

      0...1023 = 0...2500mV
      !!! remove jumper corresponding to channel !!!     

      @param rb_channel measured channel
      @return voltage [0..2500] [mV] or HAL_RANGE_ERR on wrong input channel number
    */
   inline int16_t getAdcMeanVoltage(uint8_t rb_channel)
   {
      return __HAL::getAdcMeanVoltage(rb_channel);
   };

   /**
      get the measured current value of a channel in [uA]

      0...957 = 0...20mA
      !!! set jumper corresponding to channel in position 2-3 !!!     

      @param rb_channel measured channel
      @return current [0..20000] [uA] or HAL_RANGE_ERR on wrong input channel number
   */
   inline int16_t getAdcCurrent(uint8_t rb_channel)
   {
      return __HAL::getAdcCurrent(rb_channel);
   };

   /**
      get the MEDIUM of measured current value of a channel in [uA]

      0...957 = 0...20mA
      !!! set jumper corresponding to channel in position 2-3 !!!     

      @param rb_channel measured channel
      @return current [0..20000] [uA] or HAL_RANGE_ERR on wrong input channel number
   */
   inline int16_t  getAdcMeanCurrent(uint8_t rb_channel)
   {
      return __HAL::getAdcMeanCurrent(rb_channel);
   };

   /**
      get the diagnose ADC value from specified DIGITAL INPUT channel

      dummy function - Not Supported

      @param rb_channel channel number
      @return HAL_CONFIG_ERR
   */
   inline int16_t  getDiginDiagnoseAdc(uint8_t rb_channel)
   {
      return HAL_CONFIG_ERR;
   };

   /**
      get the connection state of case mass

      dummy function - Not Supported

      @return false
   */
   inline bool getAdcVirtMass(void)
   {
      return false;
   };

   /**
      get the temperature

      dummy function - Not Supported

      @return 0
   */
   inline int16_t getAdcTemp(void)
   {
      return 0;
   };

   /**
      get the threshold value

      dummy function - Not Supported

      @return threshold voltage [mV]
   */
   inline int16_t getAdcThreshold(void)
   {
      return 0;
   };

   /**
      get the voltage at the PIC

      dummy function - Not Supported

      @return PIC voltage [mV]
   */
   inline int16_t getAdcPic(void)
   {
      return 0;
   };

   /**
      get the complete sum of all currents

      dummy function - Not Supported

      @return 0
   */
   inline int16_t getAdcCurrentSum(void)
   {
      return 0;
   };

   /**
      deliver state of symbolic digital input channels

      - static digital inputs (X7/X8)
      - analog inputs used as digital inputs (X5/X6)
      - counter and interrupt inputs used as digital inputs (X19/X20)
      - DIP switches (SW1)

      !!! use symbolic input channels only !!!
      !!! analog input: set jumper corresponding to channel in position 1-2 !!!     

      @param rb_channelNumber input channel number
      @return ON, OFF or HAL_RANGE_ERR
   */
   inline int16_t getDiginOnoff(uint8_t rb_channelNumber)
   {
      return __HAL::get_digin_onoff(rb_channelNumber);
   };

   /**
      deliver debounced state of symbolic digital input channels

      - static digital inputs (X7/X8)
      - analog inputs used as digital inputs (X5/X6)
      - counter and interrupt inputs used as digital inputs (X19/X20)
      - DIP switches (SW1)

      !!! use symbolic input channels only !!!
      !!! analog input: set jumper corresponding to channel in position 1-2 !!!     

      @param rb_channelNumber input channel number
      @return ON, OFF or HAL_RANGE_ERR
   */
   inline int16_t getDiginOnoffStatic(uint8_t rb_channelNumber)
   {
      return __HAL::get_digin_onoff_static(rb_channelNumber);
   };

   /**
      deliver frequency of digital interrupt channel

      dummy function - Not Supported

      @param rb_channelNumber input channel number
      @param b_useVirtual
      @return frequency of triggered events [mHz] or BIOS_WARN on too less impulses
   */
   inline uint16_t getDiginFreq(uint8_t rb_channelNumber, bool b_useVirtual = false)
   {
      return 0;
   };
   /*@}*/
} 
#endif
