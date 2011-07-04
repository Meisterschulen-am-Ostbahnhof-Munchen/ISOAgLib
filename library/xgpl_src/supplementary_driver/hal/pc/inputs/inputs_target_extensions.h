/*
  inputs_target_extensions.h:
    header for PC specific extensions for the HAL for inputs

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _PC_INPUTS_TARGET_EXTENSIONS_H_
#define _PC_INPUTS_TARGET_EXTENSIONS_H_

#include <IsoAgLib/hal/pc/config.h>
#include <IsoAgLib/hal/pc/typedef.h>


namespace __HAL {

/* ******************************************************** */
/**
 * \name Specific Functions
 */
/* ******************************************************** */
/*@{*/
#define DIGIN                  0   /* = 0; digital inputg*/
#define RISING_EDGE            1   /* = 1; interrupt on rising edge*/
#define FALLING_EDGE           2   /* = 2; interrupt on falling edge*/
#define BOTH_EDGE              3   /* = 3; interrupt on both edges*/
#define HIGH_ACTIV             1   /* = 1; input for aktiv high signals */
#define LOW_ACTIV              0   /* = 0; input for aktiv low signals */
#define  GET_U_C               35        /* UC (Boardspannung)   */
#define  GET_U_EXT_8_5_V       15        /* U 8,5 V EXT */
#define RPM_IN_1_4             0   /* = 0; group of inputs 1 to 4 */
#define RPM_IN_5_16            1   /* = 1; group of inputs 5 to 16 */
#define  VOLTAGE_IN               0       /* Define to configure the input as voltage input */
#define  CURRENT_IN               1       /* Define to configure the input as current input */
#define  GET_M_VIRT            16        /* Virtuelle Masse */
#define  GET_D_T               17        /* Temperaturaenderung DT */
#define  GET_U_2_3_V           80        /* U 2,3 V   */
#define  GET_U_THRESHOLD       80        /* U 2,3 V   */
#define  GET_U_PIC             32        /* Spannungsversg. PIC, 5V   */
#define  GET_SUM_I             1         /* Messung Summenstrom       */

int16_t  init_digin(uint8_t bInput,uint8_t bMode,uint8_t bAktivhighlow,void (*pfFunctionName)(...));
int16_t  getDiginOnoff(uint8_t bInputNumber);
int16_t  getDiginOnoffStatic(uint8_t bInputNumber);
int16_t  setDiginPrescaler(uint8_t bGroup, uint8_t bMode);
int16_t  getDiginPeriod(uint8_t bInput, uint16_t *pwPeriod, uint16_t *pwImpulse);
int16_t  getDiginFreq(uint8_t bInput, uint16_t *pwFrequency);


int16_t  getAdc(uint8_t bKanalnummer);              /* evaluation of analog channels */
int16_t  init_analogin(uint8_t bNumber, uint8_t bType); /* initialisation of analog inputs */
void setFastAnalogin(boolean bMode);        /* switching between fast and slow input sampling */
int16_t  getAnaloginMean(uint8_t bInput);          /* evaluation of the mean value of an analog input */
/**
  deliver channel number for checking/requesting of analog input
  for call of __HAL:: functions (differences mostly caused
  by multiplexers)
  @param channel number from interval [0..maxNo] == [0..7]
  @return according channel number for __HAL call
*/
inline uint8_t getAnaloginCheckNr(uint8_t ab_channel)
  {return (ab_channel);} // for simulation, directly map the channels
/**
  deliver channel number for checking/requesting of
  ADC value at digital input
  for call of __HAL:: functions (differences mostly caused
  by multiplexers)
  @param channel number from interval [0..maxNo]
  @return according channel number for __HAL call
*/
inline uint8_t getDiginAdcCheckNr(uint8_t ab_channel)
  {return (ab_channel);} // for simulation, directly map the channels


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
int16_t init_counter(uint8_t ab_channel, uint16_t aui16_timebase, boolean ab_activHigh, boolean ab_risingEdge);
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
uint16_t getCounterFrequency(uint8_t ab_channel);
/**
 get time since last signal and reset according trigger timers
 if timebase is exceeded -> avoid overflow problems if timer floated around 0xFFFF
 @param ab_channel channel of counter
 @return time since last signal [msec.]
*/
uint16_t getCounterLastSignalAge(uint8_t ab_channel);
/*@}*/

} //end namespace __HAL
#endif
