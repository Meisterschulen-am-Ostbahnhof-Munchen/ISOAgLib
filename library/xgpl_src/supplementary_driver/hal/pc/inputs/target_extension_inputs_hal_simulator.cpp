/*
  inputs_target_extensions_hal_simulator.cpp:
    source for HAL simulator for inputs. This file was based on
    inputs_target_extensions.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
/** \file
 * 
 * A module biosExtenders should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in map_bios_namespace.h .
 * ********************************************************** */

#include "inputs_target_extensions.h"

#include <stdlib.h>
#include <cstring>
#include <cstdio>

#include <IsoAgLib/hal/hal_system.h>


namespace __HAL {


/* *************************************** */
/* ****** Inputs_c BIOS functions  ******* */
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

void counterIrqFlex(uint8_t ab_row, uint8_t ab_col)
{
  (_pulDiginCounter[ab_row][ab_col])++;
  // the config function of input must guarantee, that
  // vectors memory are allocated
  if (_pt_diginTriggerTime[ab_row] != NULL)
  {
    _put_temp = &(_pt_diginTriggerTime[ab_row][ab_col]);
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




int16_t
init_digin(uint8_t bInput,uint8_t bMode,uint8_t bAktivhighlow,void (*pfFunctionName)(...))
{
  halSimulator().init_digin( bInput, bMode, bAktivhighlow, pfFunctionName );
  return HAL_NO_ERR;
}


int16_t
getDiginOnoff(uint8_t bInputNumber)
{
  return halSimulator().getDiginOnoff( bInputNumber );
}


int16_t
getDiginOnoffStatic(uint8_t bInputNumber)
{
  return getDiginOnoff(bInputNumber);
}


int16_t
setDiginPrescaler(uint8_t bGroup, uint8_t bMode)
{
  return HAL_NO_ERR;
}


int16_t
getDiginPeriod(uint8_t bInput, uint16_t *pwPeriod, uint16_t *pwImpulse)
{
  halSimulator().getDiginPeriod( bInput, pwPeriod, pwImpulse );
  return HAL_NO_ERR;
}


int16_t
getDiginFreq(uint8_t bInput, uint32_t *pwFrequency)
{
  halSimulator().getDiginFreq( bInput, pwFrequency );
  return HAL_NO_ERR;
}


int16_t
getAdc(uint8_t bKanalnummer)
{
  return halSimulator().getAdc( bKanalnummer );
}


int16_t
init_analogin(uint8_t bNumber, uint8_t bType)
{
  halSimulator().init_analogin( bNumber, bType );
  return HAL_NO_ERR;
}


void
setFastAnalogin(boolean bMode)
{
}


int16_t
getAnaloginMean(uint8_t bInput)
{
  return getAdc( bInput );
}


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


int16_t
init_counter(uint8_t ab_channel, uint16_t aui16_timebase, boolean ab_activHigh, boolean ab_risingEdge)
{
  int32_t i32_prescale = aui16_timebase;
  uint8_t b_codeActiv = (ab_activHigh)?HIGH_ACTIV:LOW_ACTIV;
  uint8_t b_codeEdge = (ab_risingEdge)?RISING_EDGE:FALLING_EDGE;

//  int32_t ui16_prescale = ((aui16_timebase * getCpuFreq() * 1000) / 65534);
  uint8_t ui8_prescaleIndex, b_pow;
  int16_t i16_errorState;
  i32_prescale *= (getCpuFreq() * 1000);
  i32_prescale /= 65534;
  /* check if ab_channel is allowed and exit function with RANGE error if not correct */
  if (ab_channel > 15) return HAL_RANGE_ERR;
  /* configure init channel */
  init_digin(ab_channel, b_codeEdge, b_codeActiv, irqFuncArr[ab_channel]);
  if (ab_channel < 5)
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
  if (_pulDiginCounter[(ab_channel / 4)] == NULL)
  { /* according 4-group of uint32_t isn�t created -> allocate */
    _pulDiginCounter[(ab_channel / 4)] = (uint32_t*) malloc(4*sizeof(uint32_t));
    /* check if allocated properly and init */
    if (_pulDiginCounter[(ab_channel / 4)] == NULL) i16_errorState |= HAL_OVERFLOW_ERR;
    else memset(_pulDiginCounter[(ab_channel / 4)], 0, sizeof(uint32_t)*4);
  }

  /* store given timebase in according 4-group */
  if (_puiDiginTimebase[(ab_channel / 4)] == NULL)
  {
    _puiDiginTimebase[(ab_channel / 4)] = (uint16_t*) malloc(4*sizeof(uint16_t));
    /* check if allocated properly and init */
    if (_puiDiginTimebase[(ab_channel / 4)] == NULL) i16_errorState |= HAL_OVERFLOW_ERR;
    else
    {
      memset(_puiDiginTimebase[(ab_channel / 4)], 0, sizeof(uint16_t) * 4);
      _puiDiginTimebase[(ab_channel / 4)][(ab_channel % 4)] = aui16_timebase;
    }
  }

  if (i32_prescale > 1024)
  { /* standard BIOS frequency and period methods doesn�t fir for
     * the wanted timebase -> use extension functions -> allocate needed vars
     */
    if (_pt_diginTriggerTime[(ab_channel / 4)] == NULL)
    {  /* according 4-group of t_triggerNode isn�t created -> allocate */
      _pt_diginTriggerTime[(ab_channel / 4)] = (t_triggerNode*) malloc(4*sizeof(t_triggerNode));
      if (_pt_diginTriggerTime[(ab_channel / 4)] == NULL) i16_errorState |= HAL_OVERFLOW_ERR;
      else memset(_pt_diginTriggerTime[(ab_channel / 4)], 0, sizeof(t_triggerNode) * 4);
    }
  }

  return i16_errorState;
}


uint32_t
getCounter(uint8_t ab_channel)
{
  /* check if ab_channel is allowed and var array is allocated */
  return halSimulator().getCounter( ab_channel );
}


int16_t
resetCounter(uint8_t ab_channel)
{
  return halSimulator().resetCounter( ab_channel );
}


uint16_t
getCounterPeriod(uint8_t ab_channel)
{
  uint16_t pwPeriod;
  uint16_t pwImpulse;

  getDiginPeriod(ab_channel, &pwPeriod, &pwImpulse);
  return pwPeriod;
}


uint32_t
getCounterFrequency(uint8_t ab_channel)
{
  uint32_t pwFrequency;

  getDiginFreq(ab_channel, &pwFrequency);
  return pwFrequency;
}


uint16_t
getCounterLastSignalAge(uint8_t ab_channel)
{
  return halSimulator().getCounterLastSignalAge( ab_channel );
}


} // __HAL
