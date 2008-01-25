/***************************************************************************
    sensor_target_extensions.cpp - AMS5 specific extensions

    -------------------
    date                 : 18.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Module
 ***************************************************************************/

#include "sensor_target_extensions.h"
#include <..\commercial_BIOS\bios_ams5\ams_bios.h>
   
/** time of last interrupt */
__near static uint32_t gw_LastSignalIrq[3] = { 0, 0, 0 };

/** ISR for interrupt input channel X19_5 */
void isr_X19_5 (void)
{
   gw_LastSignalIrq[0] = AMSBIOS::sys_getSystemTimer();
}

/** ISR for interrupt input channel X19_6 */
void isr_X19_6 (void)
{
   gw_LastSignalIrq[1] = AMSBIOS::sys_getSystemTimer();
}

/** ISR for interrupt input channel X21_5 (expansion port) */
void isr_X21_5 (void)
{
   gw_LastSignalIrq[2] = AMSBIOS::sys_getSystemTimer();
}

static const irqFunction_pt g_IrqFunction[3] = { isr_X19_5, isr_X19_6, isr_X21_5 }; 

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
   int16_t init_counter(uint8_t ab_channel, uint16_t aui16_timebase, bool ab_activHigh, bool ab_risingEdge)
   {
      if (ab_channel < 5)
      {
         // init counter 0 to 4 (cntX19_2, cntX19_3, cntX19_4, cntX20_3, cntX20_4)
         return AMSBIOS::io_initCounter(ab_channel, aui16_timebase, ab_risingEdge);
      }
      else if (ab_channel < 8)
      {
         // init interrupt input channel cntX19_5,  cntX19_6,  cntX21_5  
         ab_channel -= 4;
         int16_t retVal = AMSBIOS::io_initIrqInputs(ab_channel, g_IrqFunction[ab_channel], ab_risingEdge);
         // enable interrupt
         if (retVal == HAL_NO_ERR)
           return AMSBIOS::io_setIrqInputs(ab_channel, 1);
         else
            return retVal;
      }
      else
      {
         return HAL_RANGE_ERR;
      }
   }

   /**
      get counter value of an digital counter input - "event counter mode"

      Use symbolic counter input channels form "config.h"!

      @param ab_channel channel of counter
      @return counter events since init or last reset
   */
   uint32_t getCounter(uint8_t ab_channel)
   {
      ioCounterValues_t CounterValues;

      if (AMSBIOS::io_getCounterValues(ab_channel, &CounterValues) == HAL_NO_ERR)
         return CounterValues.Counts;
      else
         return 0;
   }

   /**
      reset the given counter - "event counter mode"

      Use symbolic counter input channels form "config.h"!

      @param ab_channel channel of counter
      @return C_NO_ERR ; HAL_RANGE_ERR if counter for ab_channel isn´t configured properly
   */
   int16_t resetCounter(uint8_t ab_channel)
   {
     return AMSBIOS::io_resetCounter(ab_channel);
   }

   /**
      get period of counter channel - frequency/period measuring mode

      Use symbolic counter input channels form "config.h"!

      @param ab_channel channel of counter
      @return time between last two signals or 0xFFFF if time is longer than initially
              given timebase
   */
   uint16_t getCounterPeriod(uint8_t ab_channel)
   {
      ioCounterValues_t CounterValues;

      if (AMSBIOS::io_getCounterValues(ab_channel, &CounterValues) == HAL_NO_ERR)
      {
         return CounterValues.Period;         
      }
      else
      {
         return 0xFFFF;
      }
   }

   /**
      get frequency of counter channel - frequency/period measuring mode

      Use symbolic counter input channels form "config.h"!

      @param ab_channel channel of counter
      @return frequency calculated from time between last two signals
              or 0 if time is longer than initially given timebase
   */
   uint16_t getCounterFrequency(uint8_t ab_channel)
   {
      ioCounterValues_t CounterValues;

      if (AMSBIOS::io_getCounterValues(ab_channel, &CounterValues) == HAL_NO_ERR)
      {
         return CounterValues.Frequency;         
      }
      else
      {
         return 0;
      }
   }

   /**
      get time since last signal - frequency/period measuring mode

      Also availabel for interrupt input channels: cntX19_5,  cntX19_6,  cntX21_5.
      See "__HAL::init_counter() and "AMSBIOS::io_initIrqInputs()" for details.   

      !!! this is a "counter function" -> use symbolic counter input channels only !!!

      @param ab_channel channel of counter
      @return time since last signal [msec.]
   */
   uint32_t getCounterLastSignalAge(uint8_t ab_channel)
   {
      uint32_t i32_now = AMSBIOS::sys_getSystemTimer();

      ioCounterValues_t CounterValues;
      uint32_t          LastTimestamp;
      int32_t           i32_retValue;

      if (ab_channel < 4)
      {
          // counter 0 to 4 (cntX19_2, cntX19_3, cntX19_4, cntX20_3, cntX20_4)
         if (AMSBIOS::io_getCounterValues(ab_channel, &CounterValues) == HAL_NO_ERR)
         {
            // calculate signal age
            if (i32_now > CounterValues.Timestamp)
               return i32_now-CounterValues.Timestamp;
            else
               return 0;
         }
         else
            return 0;
      }
      else if (ab_channel < 7)
      {
         // interrupt input channel cntX19_5,  cntX19_6,  cntX21_5  
         ab_channel -= 4;
         // disable interrupt
         i32_retValue = AMSBIOS::io_setIrqInputs(ab_channel, 0);

         LastTimestamp = gw_LastSignalIrq[ab_channel];

         if (i32_retValue == HAL_NO_ERR)
         {
            // enable interrupt
            AMSBIOS::io_setIrqInputs(ab_channel, 1);
         }
         else
         {
            // interrupt input is not initialized
            return 0;
         }
         
         // calculate signal age
         if (i32_now > LastTimestamp)
            return i32_now-LastTimestamp;
         else
            return 0;
      }
      else
      {
         return 0;
      }
   }

   /**
      get the measured voltage value of a channel in [mV]

      0...1023 = 0...2500mV
      !!! remove jumper corresponding to channel !!!     

      @param ab_channel measured channel
      @return voltage [0..2500] [mV] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t getAdcVoltage(uint8_t ab_channel)
   {
      if (ab_channel>7) return HAL_RANGE_ERR;

      int32_t i32_temp = AMSBIOS::io_getADC(ab_channel);
      if ( i32_temp == (int32_t)HAL_RANGE_ERR ) return HAL_RANGE_ERR;
      else return ( (int16_t)( i32_temp * 2500l/1023l ) );
   }

   /**
      get the MEDIUM of measured voltage value of a channel in [mV]

      0...1023 = 0...2500mV
      !!! remove jumper corresponding to channel !!!     

      @param ab_channel measured channel
      @return voltage [0..2500] [mV] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t getAdcMeanVoltage(uint8_t ab_channel)
   {
      if (ab_channel>7) return HAL_RANGE_ERR;

      int32_t i32_temp = AMSBIOS::io_getADCMean(ab_channel);
      if ( i32_temp == (int32_t)HAL_RANGE_ERR ) return HAL_RANGE_ERR;
      else return ( (int16_t)( i32_temp * 2500l/1023l ) );
   }

   /**
      get the measured current value of a channel in [uA]

      0...957 = 0...20mA
      !!! set jumper corresponding to channel in position 2-3 !!!     

      @param ab_channel measured channel
      @return current [0..20000] [uA] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t  getAdcCurrent(uint8_t ab_channel)
   {
      if (ab_channel>7) return HAL_RANGE_ERR;

      int32_t i32_temp = AMSBIOS::io_getADC(ab_channel);
      if ( i32_temp == (int32_t)HAL_RANGE_ERR ) return HAL_RANGE_ERR;
      else return ( (int16_t)( i32_temp * 20000l/957l ) );
   }

   /**
      get the MEDIUM of measured current value of a channel in [uA]

      0...957 = 0...20mA
      !!! set jumper corresponding to channel in position 2-3 !!!     

      @param ab_channel measured channel
      @return current [0..20000] [uA] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t  getAdcMeanCurrent(uint8_t ab_channel)
   {
      if (ab_channel>7) return HAL_RANGE_ERR;

      int32_t i32_temp = AMSBIOS::io_getADCMean(ab_channel);
      if ( i32_temp == (int32_t)HAL_RANGE_ERR ) return HAL_RANGE_ERR;
      else return ( (int16_t)( i32_temp * 20000l/957l ) );
   }

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
   int16_t get_digin_onoff(uint8_t bInputNumber)
   {
      if (bInputNumber > 39) return HAL_RANGE_ERR;
      
      return AMSBIOS::io_getDigin(bInputNumber);
   }

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
   int16_t get_digin_onoff_static(uint8_t bInputNumber)
   {
      if (bInputNumber > 39) return HAL_RANGE_ERR;

      return AMSBIOS::io_getDiginStatic(bInputNumber);
   }

   /*@}*/
} 

