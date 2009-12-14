/***************************************************************************
    sensor_target_extensions.cpp - AMS5 specific extensions

    -------------------
    date                 : 09.09.2008
    copyright            : (c) 2007/2008 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Module
 ***************************************************************************/
/*   History:
              18.06.2007 V1.00  - first release

              04.08.2008 V1.01  - correction of function "initCounter" and
                                  "getCounterLastSignalAge":
                                  wrong distinction between the interrupt input channels

              09.09.2008 V1.02  - revision of function "get_digin_onoff" and "get_digin_onoff":
                                  AMS5 with decentral periphery (ZMDL/LW)
*/
// ---------------------------------------------------------------------------------------------


#include "sensor_target_extensions.h"
#include <..\commercial_BIOS\bios_ams5\ams_bios.h>

//#include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
//char str_outputrs[20];

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
      For "event counter mode" set rui16_timebase to "0".

      @param rb_channel input channel to use 
      @param rui16_timebase timebase to calculate periods, frequency
                            should be at least longer than longest
                            awaited signal period [msec.]
      @param rb_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
      @param rb_risingEdge true -> counter triggers on rising edge; else on falling edge
      @return HAL_NO_ERR if no error occured
   */
   int16_t init_counter(uint8_t rb_channel, uint16_t rui16_timebase, bool rb_activHigh, bool rb_risingEdge)
   {
      if (rb_channel < 5)
      {
         // init counter 0 to 4 (cntX19_2, cntX19_3, cntX19_4, cntX20_3, cntX20_4)
         return AMSBIOS::io_initCounter(rb_channel, rui16_timebase, rb_risingEdge);
      }
      else if (rb_channel < 8)
      {
         // init interrupt input channels cntX19_5,  cntX19_6,  cntX21_5  
         rb_channel -= 5;
         int16_t retVal = AMSBIOS::io_initIrqInputs(rb_channel, g_IrqFunction[rb_channel], rb_risingEdge);
         // enable interrupt
         if (retVal == HAL_NO_ERR)
           return AMSBIOS::io_setIrqInputs(rb_channel, 1);
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

      @param rb_channel channel of counter
      @return counter events since init or last reset
   */
   uint32_t getCounter(uint8_t rb_channel)
   {
      ioCounterValues_t CounterValues;

      if (AMSBIOS::io_getCounterValues(rb_channel, &CounterValues) == HAL_NO_ERR)
         return CounterValues.Counts;
      else
         return 0;
   }

   /**
      reset the given counter - "event counter mode"

      Use symbolic counter input channels form "config.h"!

      @param rb_channel channel of counter
      @return C_NO_ERR ; HAL_RANGE_ERR if counter for rb_channel isn´t configured properly
   */
   int16_t resetCounter(uint8_t rb_channel)
   {
     return AMSBIOS::io_resetCounter(rb_channel);
   }

   /**
      get period of counter channel - frequency/period measuring mode

      Use symbolic counter input channels form "config.h"!

      @param rb_channel channel of counter
      @return time between last two signals or 0xFFFF if time is longer than initially
              given timebase
   */
   uint16_t getCounterPeriod(uint8_t rb_channel)
   {
      ioCounterValues_t CounterValues;

      if (AMSBIOS::io_getCounterValues(rb_channel, &CounterValues) == HAL_NO_ERR)
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

      @param rb_channel channel of counter
      @return frequency calculated from time between last two signals
              or 0 if time is longer than initially given timebase
   */
   uint16_t getCounterFrequency(uint8_t rb_channel)
   {
      ioCounterValues_t CounterValues;

      if (AMSBIOS::io_getCounterValues(rb_channel, &CounterValues) == HAL_NO_ERR)
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

      @param rb_channel channel of counter
      @return time since last signal [msec.]
   */
   uint32_t getCounterLastSignalAge(uint8_t rb_channel)
   {
      uint32_t i32_now = AMSBIOS::sys_getSystemTimer();

      ioCounterValues_t CounterValues;
      uint32_t          LastTimestamp;
      int32_t           i32_retValue;

      if (rb_channel < 5)
      {
          // counter 0 to 4 (cntX19_2, cntX19_3, cntX19_4, cntX20_3, cntX20_4)
         if (AMSBIOS::io_getCounterValues(rb_channel, &CounterValues) == HAL_NO_ERR)
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
      else if (rb_channel < 8)
      {
         // interrupt input channels cntX19_5,  cntX19_6,  cntX21_5  
         rb_channel -= 5;
         // disable interrupt
         i32_retValue = AMSBIOS::io_setIrqInputs(rb_channel, 0);

         LastTimestamp = gw_LastSignalIrq[rb_channel];

         if (i32_retValue == HAL_NO_ERR)
         {
            // enable interrupt
            AMSBIOS::io_setIrqInputs(rb_channel, 1);
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

      @param rb_channel measured channel
      @return voltage [0..2500] [mV] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t getAdcVoltage(uint8_t rb_channel)
   {
      if (rb_channel>7) return HAL_RANGE_ERR;

      int32_t i32_temp = AMSBIOS::io_getADC(rb_channel);
      if ( i32_temp == (int32_t)HAL_RANGE_ERR ) return HAL_RANGE_ERR;
      else return ( (int16_t)( i32_temp * 2500l/1023l ) );
   }

   /**
      get the MEDIUM of measured voltage value of a channel in [mV]

      0...1023 = 0...2500mV
      !!! remove jumper corresponding to channel !!!     

      @param rb_channel measured channel
      @return voltage [0..2500] [mV] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t getAdcMeanVoltage(uint8_t rb_channel)
   {
      if (rb_channel>7) return HAL_RANGE_ERR;

      int32_t i32_temp = AMSBIOS::io_getADCMean(rb_channel);
      if ( i32_temp == (int32_t)HAL_RANGE_ERR ) return HAL_RANGE_ERR;
      else return ( (int16_t)( i32_temp * 2500l/1023l ) );
   }

   /**
      get the measured current value of a channel in [uA]

      0...957 = 0...20mA
      !!! set jumper corresponding to channel in position 2-3 !!!     

      @param rb_channel measured channel
      @return current [0..20000] [uA] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t  getAdcCurrent(uint8_t rb_channel)
   {
      if (rb_channel>7) return HAL_RANGE_ERR;

      int32_t i32_temp = AMSBIOS::io_getADC(rb_channel);
      if ( i32_temp == (int32_t)HAL_RANGE_ERR ) return HAL_RANGE_ERR;
      else return ( (int16_t)( i32_temp * 20000l/957l ) );
   }

   /**
      get the MEDIUM of measured current value of a channel in [uA]

      0...957 = 0...20mA
      !!! set jumper corresponding to channel in position 2-3 !!!     

      @param rb_channel measured channel
      @return current [0..20000] [uA] or HAL_RANGE_ERR on wrong input channel number
   */
   int16_t  getAdcMeanCurrent(uint8_t rb_channel)
   {
      if (rb_channel>7) return HAL_RANGE_ERR;

      int32_t i32_temp = AMSBIOS::io_getADCMean(rb_channel);
      if ( i32_temp == (int32_t)HAL_RANGE_ERR ) return HAL_RANGE_ERR;
      else return ( (int16_t)( i32_temp * 20000l/957l ) );
   }

   /**
      deliver state of symbolic digital input channels

      - static digital inputs (X7/X8)
      - analog inputs used as digital inputs (X5/X6)
      - counter and interrupt inputs used as digital inputs (X19/X20)
      - DIP switches (SW1)
      - static digital inputs from decentral periphery (ZMDL/LW)
      
      !!! use symbolic input channels only !!!
      !!! analog input: set jumper corresponding to channel in position 1-2 !!!     

      @param rb_channelNumber input channel number
      @return ON, OFF or HAL_RANGE_ERR
   */
   int16_t get_digin_onoff(uint8_t rb_channelNumber)
   {
      if (rb_channelNumber < 40)
      {     
        return AMSBIOS::io_getDigin(rb_channelNumber);
      }
      else if (rb_channelNumber < 144)
      {
         // input channel number: 0..12
         // Node number:          1..8
         uint8_t NodeNr    = (rb_channelNumber-40)/13+1;
         uint8_t ChannelNr = (rb_channelNumber-40)%13;
         return AMSBIOS::ioExt_getDigin(NodeNr, ChannelNr);
      }
      else
      {
         return HAL_RANGE_ERR;
      }
   }

   /**
      deliver debounced state of symbolic digital input channels

      - static digital inputs (X7/X8)
      - analog inputs used as digital inputs (X5/X6)
      - counter and interrupt inputs used as digital inputs (X19/X20)
      - DIP switches (SW1)
      - static digital inputs from decentral periphery (ZMDL/LW)

      !!! use symbolic input channels only !!!
      !!! analog input: set jumper corresponding to channel in position 1-2 !!!     

      @param rb_channelNumber input channel number
      @return ON, OFF or HAL_RANGE_ERR
   */
   int16_t get_digin_onoff_static(uint8_t rb_channelNumber)
   {
      if (rb_channelNumber < 40)
      {     
         return AMSBIOS::io_getDiginStatic(rb_channelNumber);
      }
      else if (rb_channelNumber < 144)
      {
         // input channel number: 0..12
         // Node number:          1..8
         uint8_t NodeNr    = (rb_channelNumber-40)/13+1;
         uint8_t ChannelNr = (rb_channelNumber-40)%13;
         return AMSBIOS::ioExt_getDigin(NodeNr, ChannelNr);
      }
      else
      {
         return HAL_RANGE_ERR;
      }
   }

   /*@}*/
} 

