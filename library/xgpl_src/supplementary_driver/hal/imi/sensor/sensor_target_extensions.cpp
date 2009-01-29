/***************************************************************************
                          sensor_target_extensions.cpp - some extensions to the used
                                             BIOS; for adaptations to the
                                             needed BIOS functions which
                                             can't be implemented in inline
                                             functions in bios_namespace_xx.h
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2009 Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                  *
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
/** \file IsoAgLib/hal/pc/system/SystemTargetExtensions.cc
 * A module targetExtensions should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
/* ********************************************************** */

#include "sensor_target_extensions.h"
#include "sensor.h"

#include <IsoAgLib/hal/imi/config.h>

#include <cstdlib>
#include <cstring>

namespace __HAL {
extern "C" {
  /** include the BIOS specific header with the part for CAN into __HAL */
  #include <commercial_BIOS/bios_imi/adis10.h>
}

using namespace std;

/** prescaler value for digin input channels 1_6 */
static uint8_t _b_prescale_1_6Index;
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
/**
  configured timebase for counter channels;
  if time between two events is longer than given
  timebase, getCounterPeriod answers 0;
  important for timebases which are not supported by standard BIOS
*/
static uint16_t *_puiDiginTimebase[4];

typedef void (*counterIrqFunction)();
static uint16_t _wIrqTime;

void counterIrqFlex(uint8_t ab_row, uint8_t ab_col)
{
  (_pulDiginCounter[ab_row][ab_col])++;
  // the config function of input must guarantee, that
  // vectors memory are allocated
  if (_pt_diginTriggerTime[ab_row] != NULL)
  {
    _put_temp = &(_pt_diginTriggerTime[ab_row][ab_col]);
    _wIrqTime = (get_time() & 0xFFFF);
    if (_wIrqTime > _put_temp->uiAct) _put_temp->uiPeriod = _wIrqTime - _put_temp->uiAct;
    else
    {
      if (_wIrqTime == _put_temp->uiAct) _put_temp->uiPeriod = 1;
      else _put_temp->uiPeriod = _wIrqTime - (0xFFFF - _put_temp->uiAct);
    }
    _put_temp->uiAct = _wIrqTime;
  }
}

void counterIrq_0(){counterIrqFlex(0,0);}
void counterIrq_1(){counterIrqFlex(0,1);}
void counterIrq_2(){counterIrqFlex(0,2);}
void counterIrq_3(){counterIrqFlex(0,3);}
void counterIrq_4(){counterIrqFlex(1,0);}
void counterIrq_5(){counterIrqFlex(1,1);}
void counterIrq_6(){counterIrqFlex(1,2);}
void counterIrq_7(){counterIrqFlex(1,3);}
void counterIrq_8(){counterIrqFlex(2,0);}
void counterIrq_9(){counterIrqFlex(2,1);}
void counterIrq_10(){counterIrqFlex(2,2);}
void counterIrq_11(){counterIrqFlex(2,3);}
void counterIrq_12(){counterIrqFlex(3,0);}
void counterIrq_13(){counterIrqFlex(3,1);}
void counterIrq_14(){counterIrqFlex(3,2);}
void counterIrq_15(){counterIrqFlex(3,3);}




static counterIrqFunction irqFuncArr[16] =
{&counterIrq_0, &counterIrq_1, &counterIrq_2, &counterIrq_3, &counterIrq_4, &counterIrq_5,
 &counterIrq_6, &counterIrq_7, &counterIrq_8, &counterIrq_9, &counterIrq_10, &counterIrq_11,
 &counterIrq_12, &counterIrq_13, &counterIrq_14, &counterIrq_15};

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
int16_t init_counter(uint8_t ab_channel, uint16_t aui16_timebase, boolean ab_activHigh, boolean ab_risingEdge)
{
  int32_t i32_prescale = aui16_timebase;
  uint8_t b_codeActiv = (ab_activHigh)?PULL_DOWN:PULL_UP;
  uint8_t b_codeEdge = (ab_risingEdge)?RISING_EDGE:FALLING_EDGE;

//  int32_t ui16_prescale = ((aui16_timebase * getCpuFreq() * 1000) / 65534);
  uint8_t ui8_prescaleIndex, b_pow;
  int16_t i16_errorState;
  i32_prescale *= (get_cpu_freq() * 1000);
  i32_prescale /= 65534;
  /* check if ab_channel is allowed and exit function with RANGE error if not correct */
  if (ab_channel > 15) return C_RANGE;
  /* configure init channel */
  init_digin(ab_channel, b_codeEdge, irqFuncArr[ab_channel]);
  init_input(ab_channel, b_codeActiv);
  /* ADIS/DIOS-IMI BIOS supports one prescaler part */
  ui8_prescaleIndex = _b_prescale_1_6Index;
  for (b_pow = 9; b_pow > 1; b_pow--)
  { /* the prescaler must be configured by (2 << pow) values */
    if ((i32_prescale > (2 << b_pow)) || (b_pow == 2))
    { /* only change prescaler if longer than before */
      if (_b_prescale_1_6Index < (b_pow - 2))
        _b_prescale_1_6Index = (b_pow - 2);
      break;
    }
  }
  /* set prescaler */
  i16_errorState = set_digin_prescaler(RPM_IN_1_6, _b_prescale_1_6Index);

  /* create var for counter value -> this vars are managed in 4-groups
   *  of int32_t -> avoid memory waste
   */
  if (_pulDiginCounter[(ab_channel / 4)] == NULL)
  { /* according 4-group of uint32_t isn´t created -> allocate */
    _pulDiginCounter[(ab_channel / 4)] = (uint32_t*)malloc(4*sizeof(uint32_t));
    /* check if allocated properly and init */
    if (_pulDiginCounter[(ab_channel / 4)] == NULL) i16_errorState |= C_OVERFLOW;
    else memset(_pulDiginCounter[(ab_channel / 4)], 0, sizeof(uint32_t)*4);
  }

  /* store given timebase in according 4-group */
  if (_puiDiginTimebase[(ab_channel / 4)] == NULL)
  {
    _puiDiginTimebase[(ab_channel / 4)] = (uint16_t*)malloc(4*sizeof(uint16_t));
    /* check if allocated properly and init */
    if (_puiDiginTimebase[(ab_channel / 4)] == NULL) i16_errorState |= C_OVERFLOW;
    else
    {
      memset(_puiDiginTimebase[(ab_channel / 4)], 0, sizeof(uint16_t) * 4);
      _puiDiginTimebase[(ab_channel / 4)][(ab_channel % 4)] = aui16_timebase;
    }
  }

  if (i32_prescale > 1024)
  { /* standard BIOS frequency and period methods doesn´t fir for
     * the wanted timebase -> use extension functions -> allocate needed vars
     */
    if (_pt_diginTriggerTime[(ab_channel / 4)] == NULL)
    {  /* according 4-group of t_triggerNode isn´t created -> allocate */
      _pt_diginTriggerTime[(ab_channel / 4)] = (t_triggerNode*)malloc(4*sizeof(t_triggerNode));
      if (_pt_diginTriggerTime[(ab_channel / 4)] == NULL) i16_errorState |= C_OVERFLOW;
      else memset(_pt_diginTriggerTime[(ab_channel / 4)], 0, sizeof(t_triggerNode) * 4);
    }
  }

  return i16_errorState;
}
/**
  get counter value of an digital counter input
  @param ab_channel channel of counter
  @return counter events since init or last reset
*/
uint32_t getCounter(uint8_t ab_channel)
{
  /* check if ab_channel is allowed and var array is allocated */
  if ((ab_channel < 16) && (_pulDiginCounter[(ab_channel / 4)] != NULL))
  {
    return _pulDiginCounter[(ab_channel / 4)][(ab_channel % 4)];
  }
  else return 0;
}
/**
  reset the given counter
  @param ab_channel channel of counter
  @return C_NO_ERR ; C_RANGE if counter for ab_channel isn´t configured properly
*/
int16_t resetCounter(uint8_t ab_channel)
{
  /* check if ab_channel is allowed and var array is allocated */
  if ((ab_channel < 16) && (_pulDiginCounter[(ab_channel / 4)] != NULL))
  {
    _pulDiginCounter[(ab_channel / 4)][(ab_channel % 4)] = 0;
    return C_NO_ERR;
  }
  else return C_RANGE;
}
/**
  get period of counter channel
  @param ab_channel channel of counter
  @return time between last two signals or 0xFFFF if time is longer than initially
           given timebase [msec.]
*/
uint16_t getCounterPeriod(uint8_t ab_channel)
{
  uint16_t ui16_timebase, ui16_result = 0xFFFF, ui16_counter;
  /* check if ab_channel is allowed and var array is allocated */
  if (ab_channel > 15) return 0xFFFF;
  if (_puiDiginTimebase[(ab_channel / 4)] != NULL)
  {
    ui16_timebase = _puiDiginTimebase[(ab_channel / 4)][(ab_channel % 4)];
    if (ui16_timebase == 0) ui16_result = 0xFFFF;
    else if (ui16_timebase < (1024 * 65534 / (get_cpu_freq() * 1000)))
    { /* use standard BIOS method because timebase is short enough */
      get_digin_period(ab_channel, &ui16_result, &ui16_counter);
      ui16_result = (((2 << (_b_prescale_1_6Index + 2)) * ui16_result )/ (get_cpu_freq() * 1000));
    }
    else
    { /* use extension method */
      if (_pt_diginTriggerTime[(ab_channel / 4)] != NULL)
      { /* vars are accessible */
        if (getCounterLastSignalAge(ab_channel) < ui16_timebase)
        { // handle overflow between uiLast and uiAct
          ui16_result = _pt_diginTriggerTime[(ab_channel / 4)][(ab_channel % 4)].uiPeriod;
          if (ui16_result == 0) ui16_result = 1;
        }
      }
    }
  }
  return ui16_result;
}
/**
  get frequency of counter channel
  @param ab_channel channel of counter
  @return frequency calculated from time between last two signals
          or 0 if time is longer than initially given timebase [100mHz]
*/
uint16_t getCounterFrequency(uint8_t ab_channel)
{
  uint16_t ui16_timebase, ui16_result = 0;
  uint16_t ui16_lastSignalAge = 0;
  uint16_t ui16_lastPeriod;
  /* check if ab_channel is allowed and var array is allocated */
  if ((ab_channel < 16) && (_puiDiginTimebase[(ab_channel / 4)] != NULL))
  {
    ui16_timebase = _puiDiginTimebase[(ab_channel / 4)][(ab_channel % 4)];
    if (ui16_timebase == 0) ui16_result = 0;
    else if (ui16_timebase < (1024 * 65534 / (get_cpu_freq() * 1000)))
    { /* use standard BIOS method because timebase is short enough */
      get_digin_freq(ab_channel, &ui16_result);
    }
    else
    { /* use extension method */
      if (_pt_diginTriggerTime[(ab_channel / 4)] != NULL)
      { /* vars are accessible */
        ui16_lastSignalAge = getCounterLastSignalAge(ab_channel);
        if (ui16_lastSignalAge < ui16_timebase)
        { // handle overflow between uiLast and uiAct
          ui16_lastPeriod = _pt_diginTriggerTime[(ab_channel / 4)][(ab_channel % 4)].uiPeriod;
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
 @param ab_channel channel of counter
 @return time since last signal [msec.]
*/
uint16_t getCounterLastSignalAge(uint8_t ab_channel)
{
  uint16_t uiResult = 0xFFFF, uiTime = (get_time() & 0xFFFF);
  uint16_t ui16_period, ui16_actTime;
  /* check if ab_channel is allowed and var array is allocated */
  if ((ab_channel < 16) && (_pt_diginTriggerTime[(ab_channel / 4)] != NULL))
  {
    ui16_period = _pt_diginTriggerTime[(ab_channel / 4)][(ab_channel % 4)].uiPeriod;
    ui16_actTime = _pt_diginTriggerTime[(ab_channel / 4)][(ab_channel % 4)].uiAct;
    if (ui16_period <= 0xFFFE)
    { // both values are valid and not resetted
      uiResult = (uiTime >= ui16_actTime)?(uiTime - ui16_actTime): (uiTime + (0xFFFF - ui16_actTime));
      // if timebase is exceeded -> reset *puiAct and *puiLast
      if (_puiDiginTimebase[(ab_channel / 4)][(ab_channel % 4)] < uiResult)
        _pt_diginTriggerTime[(ab_channel / 4)][(ab_channel % 4)].uiPeriod = 0xFFFF; // if both are equal answers in future time age 0xFFFF
    }
  }
  return uiResult;
}

} // end of namespace __HAL
