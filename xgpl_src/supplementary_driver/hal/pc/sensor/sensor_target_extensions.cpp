/***************************************************************************
                          targetExtensions.cc - source for PC specific
                                                 extensions for the HAL
                                                 for sensor
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
    email                : A.Spangler@osb-ag:de
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
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

/* ********************************************************** */
/** \file supplementary_driver/implementation/__HAL/biosPc/HwSensor/biosExtender.c
 * A module biosExtenders should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in map_bios_namespace.h .
 * ********************************************************** */

#include "sensor_target_extensions.h"
#include "sensor.h"

#include <IsoAgLib/hal/pc/system/system.h>
#include <stdlib.h>
#include <cstring>
#include <cstdio>

namespace __HAL {
static FILE* sensorAnalogInput[ANALOG_INPUT_MAX+1];
static FILE* sensorDigitalInput[DIGITAL_INPUT_MAX+1];
static bool sensorAnalogInputOpen[] = { false,false,false,false,false,false,false,false };
static bool sensorDigitalInputOpen[] = { false,false,false,false,false,false,false,false,
                                            false,false,false,false,false,false,false,false };

static int32_t lastSensorAnalogInputTime[ANALOG_INPUT_MAX+1];
static int32_t lastSensorDigitalInputTime[DIGITAL_INPUT_MAX+1];
static int32_t lastSensorAnalogInputVal[ANALOG_INPUT_MAX+1];
static int32_t lastSensorDigitalInputVal[DIGITAL_INPUT_MAX+1];

static int32_t next_sensorAnalogInputTime[ANALOG_INPUT_MAX+1];
static int32_t next_sensorDigitalInputTime[DIGITAL_INPUT_MAX+1];
static int32_t next_sensorAnalogInputVal[ANALOG_INPUT_MAX+1];
static int32_t next_sensorDigitalInputVal[DIGITAL_INPUT_MAX+1];

/* *************************************** */
/* ****** Sensor_c I BIOS functions  ******* */
/* *************************************** */
typedef void (*counterIrqFunction)(...);
/**
  array of pointers to uint32_t arrays with counter
  values; avoid memory waste, if only some channels are
  used for counting
*/
static uint32_t *_pulDiginCounter[4];
/**
  structure to store last and actual trigger timestamp
  (with MOD(0xFFFF) )
*/
typedef struct
{
  uint16_t uiPeriod;
  uint16_t uiAct;
} t_triggerNode;

/**
  if the given timebase is longer than the timebase powered
  by standard BIOS, the timestamp of last two trigger events
  should be stored for calculating of longer periods
*/
static t_triggerNode *_pt_diginTriggerTime[4];
/**
  pointer to triger node of diginput which caused interrupt
*/
t_triggerNode *_put_temp;
static uint16_t _wIrqTime;

void counterIrqFlex(uint8_t rb_row, uint8_t rb_col)
{
  (_pulDiginCounter[rb_row][rb_col])++;
  // the config function of input must guarantee, that
  // vectors memory are allocated
  if (_pt_diginTriggerTime[rb_row] != NULL)
  {
    _put_temp = &(_pt_diginTriggerTime[rb_row][rb_col]);
    _wIrqTime = (getTime() & 0xFFFF);
    if (_wIrqTime > _put_temp->uiAct) _put_temp->uiPeriod = _wIrqTime - _put_temp->uiAct;
    else
    {
      if (_wIrqTime == _put_temp->uiAct) _put_temp->uiPeriod = 1;
      else _put_temp->uiPeriod = _wIrqTime - (0xFFFF - _put_temp->uiAct);
    }
    _put_temp->uiAct = _wIrqTime;
  }
}


void counterIrq_0(...){counterIrqFlex(0,0);}
void counterIrq_1(...){counterIrqFlex(0,1);}
void counterIrq_2(...){counterIrqFlex(0,2);}
void counterIrq_3(...){counterIrqFlex(0,3);}
void counterIrq_4(...){counterIrqFlex(1,0);}
void counterIrq_5(...){counterIrqFlex(1,1);}
void counterIrq_6(...){counterIrqFlex(1,2);}
void counterIrq_7(...){counterIrqFlex(1,3);}
void counterIrq_8(...){counterIrqFlex(2,0);}
void counterIrq_9(...){counterIrqFlex(2,1);}
void counterIrq_10(...){counterIrqFlex(2,2);}
void counterIrq_11(...){counterIrqFlex(2,3);}
void counterIrq_12(...){counterIrqFlex(3,0);}
void counterIrq_13(...){counterIrqFlex(3,1);}
void counterIrq_14(...){counterIrqFlex(3,2);}
void counterIrq_15(...){counterIrqFlex(3,3);}




static counterIrqFunction irqFuncArr[16] =
{&counterIrq_0, &counterIrq_1, &counterIrq_2, &counterIrq_3, &counterIrq_4, &counterIrq_5,
 &counterIrq_6, &counterIrq_7, &counterIrq_8, &counterIrq_9, &counterIrq_10, &counterIrq_11,
 &counterIrq_12, &counterIrq_13, &counterIrq_14, &counterIrq_15};




int16_t  init_digin(uint8_t bInput,uint8_t bMode,uint8_t bAktivhighlow,void (*pfFunctionName)(...)){
  printf("init_digin mit Kanal %i, Modus %i, HiLo %i aufgerufen\n", bInput,bMode, bAktivhighlow);
  #if !defined(SYSTEM_PC_VC)
  irqFuncArr[bInput] = pfFunctionName;
  #endif
  if ( sensorDigitalInputOpen[bInput] ) fclose(sensorDigitalInput[ bInput]);
  char name[100], zeile[100];
  sprintf(name, "../../../simulated_io/digitalInput_%hu", bInput );
  sensorDigitalInput[ bInput] = fopen(name, "r");
  sensorDigitalInputOpen[bInput] = true;
  fgets(zeile, 99, sensorDigitalInput[bInput]);
  sscanf(zeile, "%u %u\n", &(lastSensorDigitalInputTime[bInput]), &(lastSensorDigitalInputVal[bInput]));
  if ( feof(sensorDigitalInput[bInput]) == 0 )
  { // read next line
    fgets(zeile, 99, sensorDigitalInput[bInput]);
    sscanf(zeile, "%u %u\n", &(next_sensorDigitalInputTime[bInput]), &(next_sensorDigitalInputVal[bInput]));
  }

  return HAL_NO_ERR;
}

int16_t  getDiginOnoff(uint8_t bInputNumber){
  // printf("getDiginOnoff(%i)\n", bInputNumber);
  if ( getTime() >= next_sensorDigitalInputTime[bInputNumber] )
  { // save next_XX to last_XX
    lastSensorDigitalInputTime[bInputNumber] = next_sensorDigitalInputTime[bInputNumber];
    lastSensorDigitalInputVal[bInputNumber] = next_sensorDigitalInputVal[bInputNumber];
    if ( feof(sensorDigitalInput[bInputNumber]) == 0 )
    { // read next line
      char zeile[100];
      fgets(zeile, 99, sensorDigitalInput[bInputNumber]);
      sscanf(zeile, "%u %u\n", &(next_sensorDigitalInputTime[bInputNumber]), &(next_sensorDigitalInputVal[bInputNumber]));
    }
  }
  return (lastSensorDigitalInputVal[bInputNumber] > 0)?true:false;
}
int16_t  getDiginOnoffStatic(uint8_t bInputNumber){
  // printf("getDiginOnoffStatic(%i)\n", bInputNumber);
  if ( getTime() >= next_sensorDigitalInputTime[bInputNumber] )
  { // save next_XX to last_XX
    lastSensorDigitalInputTime[bInputNumber] = next_sensorDigitalInputTime[bInputNumber];
    lastSensorDigitalInputVal[bInputNumber] = next_sensorDigitalInputVal[bInputNumber];
    if ( feof(sensorDigitalInput[bInputNumber]) == 0 )
    { // read next line
      char zeile[100];
      fgets(zeile, 99, sensorDigitalInput[bInputNumber]);
      sscanf(zeile, "%u %u\n", &(next_sensorDigitalInputTime[bInputNumber]), &(next_sensorDigitalInputVal[bInputNumber]));
    }
  }
  return (lastSensorDigitalInputVal[bInputNumber] > 0)?true:false;
}
int16_t  setDiginPrescaler(uint8_t bGroup, uint8_t bMode)
{printf("setDiginPrescaler fuer Gruppe %d mit Prescale Modus %d aufgerufen\n",
 bGroup, bMode);return HAL_NO_ERR;};

int16_t  getDiginPeriod(uint8_t bInput, uint16_t *pwPeriod, uint16_t *pwImpulse)
{printf("getDiginPeriod fuer Kanal %d aufgerufen\n", bInput);
  *pwPeriod = ((getTime()%10000 +bInput)) / 3;
  *pwImpulse = ((getTime()%1000 +bInput)) / 3;
  return HAL_NO_ERR;
}
int16_t  getDiginFreq(uint8_t bInput, uint16_t *pwFrequency)
{ printf("getDiginFreq fuer Kanal %d aufgerufen\n", bInput);
  *pwFrequency = ((getTime()%1000 +bInput)) / 3;
  return HAL_NO_ERR;
}

/* evaluation of analog channels */
int16_t  getAdc(uint8_t bKanalnummer){
  // printf("getAdc(%i)\n", bKanalnummer);
  if ( bKanalnummer > DIGITAL_INPUT_MAX ) return 1000;
  else if ( getTime() >= next_sensorAnalogInputTime[bKanalnummer] )
  { // save next_XX to last_XX
    lastSensorAnalogInputTime[bKanalnummer] = next_sensorAnalogInputTime[bKanalnummer];
    lastSensorAnalogInputVal[bKanalnummer] = next_sensorAnalogInputVal[bKanalnummer];
    if ( feof(sensorAnalogInput[bKanalnummer]) == 0 )
    { // read next line
      char zeile[100];
      fgets(zeile, 99, sensorAnalogInput[bKanalnummer]);
      sscanf(zeile, "%u %u\n", &(next_sensorAnalogInputTime[bKanalnummer]), &(next_sensorAnalogInputVal[bKanalnummer]));
    }
  }
  return lastSensorAnalogInputVal[bKanalnummer];
}
/* initialisation of analog inputs */
int16_t  init_analogin(uint8_t bNumber, uint8_t bType){
  printf("init_analogin fuer Kanal %i, Typ %i aufgerufen\n", bNumber, bType);
  if ( sensorAnalogInputOpen[bNumber] ) fclose(sensorAnalogInput[bNumber]);
  char name[100], zeile[100];
  sprintf(name, "../../../simulated_io/analogInput_%hu", bNumber );
  sensorAnalogInput[ bNumber] = fopen(name, "r");
  sensorAnalogInputOpen[bNumber] = true;
  fgets(zeile, 99, sensorAnalogInput[bNumber]);
  sscanf(zeile, "%u %u\n", &(lastSensorAnalogInputTime[bNumber]), &(lastSensorAnalogInputVal[bNumber]));
  if ( feof(sensorAnalogInput[bNumber]) == 0 )
  { // read next line
    fgets(zeile, 99, sensorAnalogInput[bNumber]);
    sscanf(zeile, "%u %u\n", &(next_sensorAnalogInputTime[bNumber]), &(next_sensorAnalogInputVal[bNumber]));
  }
  return HAL_NO_ERR;
}

/* switching between fast and slow input sampling */
void setFastAnalogin(boolean bMode){
  printf("setFastAnalogin mit Modus %i aufgerufen\n", bMode);
}

/* evaluation of the mean value of an analog input */
int16_t  getAnaloginMean(uint8_t bInput){
 // printf("getAnaloginMean(%i)\n", bInput);
  if ( getTime() >= next_sensorAnalogInputTime[bInput] )
  { // save next_XX to last_XX
    lastSensorAnalogInputTime[bInput] = next_sensorAnalogInputTime[bInput];
    lastSensorAnalogInputVal[bInput] = next_sensorAnalogInputVal[bInput];
    if ( feof(sensorAnalogInput[bInput]) == 0 )
    { // read next line
      char zeile[100];
      fgets(zeile, 99, sensorAnalogInput[bInput]);
      sscanf(zeile, "%u %u\n", &(next_sensorAnalogInputTime[bInput]), &(next_sensorAnalogInputVal[bInput]));
    }
  }
  return lastSensorAnalogInputVal[bInput];
}
/* *************************************** */
/* *** Special Extension for Counters  *** */
/* *************************************** */


/** prescaler value for digin input channels 1_4 */
static uint8_t _b_prescale_1_4Index;
/** prescaler value for digin input channels 5_16 */
static uint8_t _b_prescale_5_16Index;


/**
  configured timebase for counter channels;
  if time between two events is longer than given
  timebase, getCounterPeriod answers 0;
  important for timebases which are not supported by standard BIOS
*/
static uint16_t *_puiDiginTimebase[4];



/**
  init counter for trigger events on digital inoput;
  rising edges are counted;
  @param rb_channel input channel to use [0..15]
  @param rui16_timebase timebase to calculate periods, frequency
                     should be at least longer than longest
                     awaited signal period [msec.]
  @param rb_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
  @param rb_risingEdge true -> counter triggers on rising edge; else on falling edge
  @return HAL_NO_ERR if no error occured
*/
int16_t init_counter(uint8_t rb_channel, uint16_t rui16_timebase, boolean rb_activHigh, boolean rb_risingEdge)
{
  int32_t i32_prescale = rui16_timebase;
  uint8_t b_codeActiv = (rb_activHigh)?HIGH_ACTIV:LOW_ACTIV;
  uint8_t b_codeEdge = (rb_risingEdge)?RISING_EDGE:FALLING_EDGE;

//  int32_t ui16_prescale = ((rui16_timebase * getCpuFreq() * 1000) / 65534);
  uint8_t ui8_prescaleIndex, b_pow;
  int16_t i16_errorState;
  i32_prescale *= (getCpuFreq() * 1000);
  i32_prescale /= 65534;
  /* check if rb_channel is allowed and exit function with RANGE error if not correct */
  if (rb_channel > 15) return HAL_RANGE_ERR;
  /* configure init channel */
  init_digin(rb_channel, b_codeEdge, b_codeActiv, irqFuncArr[rb_channel]);
  if (rb_channel < 5)
  { /* standard BIOS supports two prescaler parts */
    ui8_prescaleIndex = _b_prescale_1_4Index;
    for (b_pow = 9; b_pow > 1; b_pow--)
    { /* the prescaler must be configured by (2 << pow) values */
      if ((i32_prescale > (2 << b_pow)) || (b_pow == 2))
      { /* only change prescaler if longer than before */
        if (_b_prescale_1_4Index < (b_pow - 2))
          _b_prescale_1_4Index = (b_pow - 2);
        break;
      }
    }
    /* set prescaler */
    i16_errorState = setDiginPrescaler(RPM_IN_1_4, _b_prescale_1_4Index);
  }
  else
  { /* same for other prescaler part */
    ui8_prescaleIndex = _b_prescale_5_16Index;
    for (b_pow = 9; b_pow > 1; b_pow--)
    {
      if ((i32_prescale > (2 << b_pow)) || (b_pow == 2))
      {
        if (_b_prescale_5_16Index < (b_pow - 2))
          _b_prescale_5_16Index = (b_pow - 2);
        break;
      }
    }
    i16_errorState = setDiginPrescaler(RPM_IN_5_16, _b_prescale_5_16Index);
  }

  /* create var for counter value -> this vars are managed in 4-groups
   *  of int32_t -> avoid memory waste
   */
  if (_pulDiginCounter[(rb_channel / 4)] == NULL)
  { /* according 4-group of uint32_t isn´t created -> allocate */
    _pulDiginCounter[(rb_channel / 4)] = (uint32_t*) malloc(4*sizeof(uint32_t));
    /* check if allocated properly and init */
    if (_pulDiginCounter[(rb_channel / 4)] == NULL) i16_errorState |= HAL_OVERFLOW_ERR;
    else memset(_pulDiginCounter[(rb_channel / 4)], 0, sizeof(uint32_t)*4);
  }

  /* store given timebase in according 4-group */
  if (_puiDiginTimebase[(rb_channel / 4)] == NULL)
  {
    _puiDiginTimebase[(rb_channel / 4)] = (uint16_t*) malloc(4*sizeof(uint16_t));
    /* check if allocated properly and init */
    if (_puiDiginTimebase[(rb_channel / 4)] == NULL) i16_errorState |= HAL_OVERFLOW_ERR;
    else
    {
      memset(_puiDiginTimebase[(rb_channel / 4)], 0, sizeof(uint16_t) * 4);
      _puiDiginTimebase[(rb_channel / 4)][(rb_channel % 4)] = rui16_timebase;
    }
  }

  if (i32_prescale > 1024)
  { /* standard BIOS frequency and period methods doesn´t fir for
     * the wanted timebase -> use extension functions -> allocate needed vars
     */
    if (_pt_diginTriggerTime[(rb_channel / 4)] == NULL)
    {  /* according 4-group of t_triggerNode isn´t created -> allocate */
      _pt_diginTriggerTime[(rb_channel / 4)] = (t_triggerNode*) malloc(4*sizeof(t_triggerNode));
      if (_pt_diginTriggerTime[(rb_channel / 4)] == NULL) i16_errorState |= HAL_OVERFLOW_ERR;
      else memset(_pt_diginTriggerTime[(rb_channel / 4)], 0, sizeof(t_triggerNode) * 4);
    }
  }

  return i16_errorState;
}
/**
  get counter value of an digital counter input
  @param rb_channel channel of counter
  @return counter events since init or last reset
*/
uint32_t getCounter(uint8_t rb_channel)
{
  /* check if rb_channel is allowed and var array is allocated */
  if ((rb_channel < 16) && (_pulDiginCounter[(rb_channel / 4)] != NULL))
  {
    return _pulDiginCounter[(rb_channel / 4)][(rb_channel % 4)];
  }
  else return 0;
}
/**
  reset the given counter
  @param rb_channel channel of counter
  @return HAL_NO_ERR ; HAL_RANGE_ERR if counter for rb_channel isn´t configured properly
*/
int16_t resetCounter(uint8_t rb_channel)
{
  /* check if rb_channel is allowed and var array is allocated */
  if ((rb_channel < 16) && (_pulDiginCounter[(rb_channel / 4)] != NULL))
  {
    _pulDiginCounter[(rb_channel / 4)][(rb_channel % 4)] = 0;
    return HAL_NO_ERR;
  }
  else return HAL_RANGE_ERR;
}
/**
  get period of counter channel
  @param rb_channel channel of counter
  @return time between last two signals or 0xFFFF if time is longer than initially
           given timebase [msec.]
*/
uint16_t getCounterPeriod(uint8_t rb_channel)
{
  uint16_t ui16_timebase, ui16_result = 0xFFFF, ui16_counter;
  /* check if rb_channel is allowed and var array is allocated */
  if (rb_channel > 15) return 0xFFFF;
  if (_puiDiginTimebase[(rb_channel / 4)] != NULL)
  {
    ui16_timebase = _puiDiginTimebase[(rb_channel / 4)][(rb_channel % 4)];
    if (ui16_timebase == 0) ui16_result = 0xFFFF;
    else if (ui16_timebase < (1024 * 65534 / (getCpuFreq() * 1000)))
    { /* use standard BIOS method because timebase is short enough */
      getDiginPeriod(rb_channel, &ui16_result, &ui16_counter);
      if (rb_channel < 5)
      {
        ui16_result = (((2 << (_b_prescale_1_4Index + 2)) * ui16_result )/ (getCpuFreq() * 1000));
      }
      else
      {
        ui16_result = (((2 << (_b_prescale_5_16Index + 2)) * ui16_result )/ (getCpuFreq() * 1000));
      }
    }
    else
    { /* use extension method */
      if (_pt_diginTriggerTime[(rb_channel / 4)] != NULL)
      { /* vars are accessible */
        if (getCounterLastSignalAge(rb_channel) < ui16_timebase)
        { // handle overflow between uiLast and uiAct
          ui16_result = _pt_diginTriggerTime[(rb_channel / 4)][(rb_channel % 4)].uiPeriod;
          if (ui16_result == 0) ui16_result = 1;
        }
      }
    }
  }
  return ui16_result;
}
/**
  get frequency of counter channel
  @param rb_channel channel of counter
  @return frequency calculated from time between last two signals
          or 0 if time is longer than initially given timebase [100mHz]
*/
uint16_t getCounterFrequency(uint8_t rb_channel)
{
  uint16_t ui16_timebase, ui16_result = 0;
  uint16_t ui16_lastSignalAge = 0;
  uint16_t ui16_lastPeriod;
  /* check if rb_channel is allowed and var array is allocated */
  if ((rb_channel < 16) && (_puiDiginTimebase[(rb_channel / 4)] != NULL))
  {
    ui16_timebase = _puiDiginTimebase[(rb_channel / 4)][(rb_channel % 4)];
    if (ui16_timebase == 0) ui16_result = 0;
    else if (ui16_timebase < (1024 * 65534 / (getCpuFreq() * 1000)))
    { /* use standard BIOS method because timebase is short enough */
      getDiginFreq(rb_channel, &ui16_result);
    }
    else
    { /* use extension method */
      if (_pt_diginTriggerTime[(rb_channel / 4)] != NULL)
      { /* vars are accessible */
        ui16_lastSignalAge = getCounterLastSignalAge(rb_channel);
        if (ui16_lastSignalAge < ui16_timebase)
        { // handle overflow between uiLast and uiAct
          ui16_lastPeriod = _pt_diginTriggerTime[(rb_channel / 4)][(rb_channel % 4)].uiPeriod;
          if (ui16_lastPeriod == 0) ui16_result = 0;
          else
          {
            if ((ui16_lastPeriod * 2) < ui16_lastSignalAge) ui16_lastPeriod = ((int32_t)ui16_lastPeriod + (int32_t)ui16_lastSignalAge)/(int32_t)2;
            ui16_result = (10000 / ui16_lastPeriod);
          }
        }
      }
    }
  }
  return ui16_result;
}

/**
 get time since last signal and reset according trigger timers
 if timebase is exceeded -> avoid overflow problems if timer floated around 0xFFFF
 @param rb_channel channel of counter
 @return time since last signal [msec.]
*/
uint16_t getCounterLastSignalAge(uint8_t rb_channel)
{
  uint16_t uiResult = 0xFFFF, uiTime = (getTime() & 0xFFFF);
  uint16_t ui16_period, ui16_actTime;
  /* check if rb_channel is allowed and var array is allocated */
  if ((rb_channel < 16) && (_pt_diginTriggerTime[(rb_channel / 4)] != NULL))
  {
    ui16_period = _pt_diginTriggerTime[(rb_channel / 4)][(rb_channel % 4)].uiPeriod;
    ui16_actTime = _pt_diginTriggerTime[(rb_channel / 4)][(rb_channel % 4)].uiAct;
    if (ui16_period <= 0xFFFE)
    { // both values are valid and not resetted
      uiResult = (uiTime >= ui16_actTime)?(uiTime - ui16_actTime): (uiTime + (0xFFFF - ui16_actTime));
      // if timebase is exceeded -> reset *puiAct and *puiLast
      if (_puiDiginTimebase[(rb_channel / 4)][(rb_channel % 4)] < uiResult)
        _pt_diginTriggerTime[(rb_channel / 4)][(rb_channel % 4)].uiPeriod = 0xFFFF; // if both are equal answers in future time age 0xFFFF
    }
  }
  return uiResult;
}

} // end namespace __HAL
