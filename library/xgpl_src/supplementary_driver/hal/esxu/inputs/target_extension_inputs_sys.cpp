/*
  inputs_target_extensions.cpp:
    some extensions to the used BIOS; for adaptations to the needed
    BIOS functions which can't be implemented in inline functions in
    bios_namespace_xx.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/isoaglib_config.h>
#include "inputs_target_extensions.h"

using namespace std; // simple version to avoid problems with using CNAMESPACE


namespace __HAL {

#define RPM_PRESCALER_MIN	RPM_PRESCALER_8
#define RPM_PRESCALER_MAX	RPM_PRESCALER_1024

/** prescaler value for digin input channels DIN9 & DIN10 */
static uint8_t _b_prescale_1_Index = RPM_PRESCALER_MIN;

/**
  array of pointers to uint32_t arrays with counter
  values; avoid memory waste, if only some channels are
  used for counting
*/
#define SENSOR_ARRAY_SIZE	10
static uint32_t _pulDiginCounter[SENSOR_ARRAY_SIZE];
static uint16_t _prevCounter[SENSOR_ARRAY_SIZE];

#if 0
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
	static t_triggerNode _pt_diginTriggerTime[SENSOR_ARRAY_SIZE];

	/**
	  pointer to triger node of diginput which caused interrupt
	*/
	/**
	  configured timebase for counter channels;
	  if time between two events is longer than given
	  timebase, getCounterPeriod answers 0;
	  important for timebases which are not supported by standard BIOS
	*/
	static uint16_t _puiDiginTimebase[SENSOR_ARRAY_SIZE];

	typedef void (*counterIrqFunction)();


	void counterIrqFlex(uint8_t ab_channel)
	{
	  _pulDiginCounter[ab_channel]++;
	  // the config function of input must guarantee, that
	  // vectors memory are allocated
	  t_triggerNode *_put_temp = &_pt_diginTriggerTime[ab_channel];
	  uint16_t _wIrqTime = (get_time() & 0xFFFF);
	  if (_wIrqTime > _put_temp->uiAct) _put_temp->uiPeriod = _wIrqTime - _put_temp->uiAct;
	  else
	  {
	    if (_wIrqTime == _put_temp->uiAct) _put_temp->uiPeriod = 1;
	    else _put_temp->uiPeriod = _wIrqTime - (0xFFFF - _put_temp->uiAct);
	  }
	  _put_temp->uiAct = _wIrqTime;
	}

	void counterIrq_0(){counterIrqFlex(0);}
	void counterIrq_1(){counterIrqFlex(1);}
	void counterIrq_2(){counterIrqFlex(2);}
	void counterIrq_3(){counterIrqFlex(3);}
	void counterIrq_4(){counterIrqFlex(4);}
	void counterIrq_5(){counterIrqFlex(5);}
	void counterIrq_6(){counterIrqFlex(6);}
	void counterIrq_7(){counterIrqFlex(7);}
	void counterIrq_8(){counterIrqFlex(8);}
	void counterIrq_9(){counterIrqFlex(9);}

	static counterIrqFunction irqFuncArr[SENSOR_ARRAY_SIZE] =
		{
		  &counterIrq_0
		, &counterIrq_1
		, &counterIrq_2
		, &counterIrq_3
		, &counterIrq_4
		, &counterIrq_5
		, &counterIrq_6
		, &counterIrq_7
		, &counterIrq_8
		, &counterIrq_9
		};

#endif
/**
  init counter for trigger events on digital input;
  rising edges are counted;
  @param aui16_timebase timebase to calculate periods, frequency
                     should be at least longer than longest
                     awaited signal period [msec.]
  @param ab_risingEdge true -> counter triggers on rising edge; else on falling edge
  @return C_NO_ERR if no error occured
*/
int16_t init_counter(uint8_t ab_channel, uint16_t aui16_timebase, bool ab_activHigh, bool ab_risingEdge)
{
  int16_t i16_errorState;

  uint8_t b_codeActiv = (ab_activHigh)?HIGH_ACTIVE:LOW_ACTIVE;
  uint8_t b_codeEdge = (ab_risingEdge)?RISING_EDGE:FALLING_EDGE;

  uint32_t ui32_prescale = ( ((uint32_t)aui16_timebase) * get_cpu_freq() * 1000) / 65534;
  /* configure init channel */
  int16_t ret = init_digin(ab_channel, b_codeEdge, b_codeActiv, NULL);

#if DEBUG_HAL
  INTERNAL_DEBUG_DEVICE <<
    __HAL::get_time() << " ms - " <<
    "init_digin( " << (uint16_t)ab_channel << ", " <<
    (uint16_t)b_codeEdge << ", " <<
    (uint16_t)b_codeActiv <<
    ", NULL ) returns " << ret << "\r";
#endif

// NOTE: On ESXu, The prescaler applies to both RPM inputs DIN9 and DIN10
// So, try to find the best prescaler for both inputs based on the maximum time per pulse in ms given by aui16_timebase
// The best (highest precision) prescaler is RPM_PRESCALER_MIN
  for( uint8_t ui8_pow = RPM_PRESCALER_MAX; ; ui8_pow-- )
  { /* the prescaler must be configured by (2 << pow) values */
    if ((ui32_prescale > (1 << (ui8_pow+2))) || (ui8_pow == RPM_PRESCALER_MIN))
    { /* only change prescaler if longer than before */
      if (_b_prescale_1_Index < ui8_pow )
        _b_prescale_1_Index = ui8_pow;
      break;
    }
  }
  /* set prescaler */
  i16_errorState = set_digin_prescaler(_b_prescale_1_Index);

#if DEBUG_HAL
  INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " <<
    "set_digin_prescaler( " << (uint16_t)_b_prescale_1_Index <<
    " ) returns " << i16_errorState << "\r";
#endif

  /* clear counter value
   */
  _pulDiginCounter[ab_channel] = 0;

#if 0
  /* clear given timebase */
  _puiDiginTimebase[ab_channel] = 0;

  if (ui32_prescale > 1024)
  { /* standard BIOS frequency and period methods doesn�t fit for
     * the wanted timebase -> use extension functions -> allocate needed vars
     */
    CNAMESPACE::memset(&_pt_diginTriggerTime[ab_channel], 0, sizeof(t_triggerNode));
  }
#endif

// Really need to call this for each input when the prescaler changes
uint16_t wTime = (uint32_t)aui16_timebase * 1000 * get_cpu_freq() >> (_b_prescale_1_Index+3);
uint16_t numPulsesToAvg = 1;
int16_t configretval = config_digin_freq( ab_channel, wTime, numPulsesToAvg );

#if DEBUG_HAL
  INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " <<
    "config_digin_freq( " <<
    (uint16_t) ab_channel << ", " <<
    (uint16_t) wTime << ", " <<
    (uint16_t) numPulsesToAvg <<
    " ) returns  " << configretval << "\r";
#endif

  return i16_errorState;
}
/**
  get counter value of an digital counter input
  @return counter events since init or last reset
*/
uint32_t getCounter(uint8_t ab_channel)
{
	uint16_t ui16_result = 0xFFFF, ui16_counter;

	int16_t retval = get_digin_period(ab_channel, &ui16_result, &ui16_counter);

#if DEBUG_HAL
  INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " <<
    "get_digin_period( " << unsigned(ab_channel) << ", " <<
    unsigned(ui16_result) << ", " <<
    unsigned(ui16_counter) <<
    " ) returns " << unsigned(retval) << "\r";
#endif

	_pulDiginCounter[ab_channel] += ( ui16_counter - _prevCounter[ab_channel] );
	_prevCounter[ab_channel] = ui16_counter;

	return _pulDiginCounter[ab_channel];
}
/**
  reset the given counter
  @return C_NO_ERR ; C_RANGE if counter if it isn�t configured properly
*/
int16_t resetCounter(uint8_t ab_channel)
{
	uint16_t ui16_result = 0xFFFF, ui16_counter;
	int16_t retval = get_digin_period(ab_channel, &ui16_result, &ui16_counter);

#if DEBUG_HAL
  INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " <<
    "get_digin_period( " << unsigned(ab_channel) << ", " <<
    unsigned(ui16_result) << ", " <<
    unsigned(ui16_counter) <<
    " ) returns " << unsigned(retval) << "\r";
#endif

	_pulDiginCounter[ab_channel] = 0;
	_prevCounter[ab_channel] = ui16_counter;

	return C_NO_ERR;
}
/**
  get period of counter channel
  @return time between last two signals or 0xFFFF if time is longer than initially
           given timebase [msec.]
*/
uint16_t getCounterPeriod(uint8_t ab_channel)
{
	uint16_t ui16_result = 0xFFFF, ui16_counter;

	int16_t retval = get_digin_period(ab_channel, &ui16_result, &ui16_counter);

#if DEBUG_HAL
  INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " <<
    "get_digin_period( " << unsigned(ab_channel) << ", " <<
    unsigned(ui16_result) << ", " <<
    unsigned(ui16_counter) <<
    " ) returns " << unsigned(retval) << "\r";
#endif

	_pulDiginCounter[ab_channel] += ( ui16_counter - _prevCounter[ab_channel] );
	_prevCounter[ab_channel] = ui16_counter;
    ui16_result = (((2 << (_b_prescale_1_Index + 2)) * ui16_result )/ (get_cpu_freq() * 1000));

#if 0
  uint16_t ui16_timebase = _puiDiginTimebase[ab_channel];
  if (ui16_timebase == 0) ui16_result = 0xFFFF;
  else if (ui16_timebase < (1024 * 65534 / (get_cpu_freq() * 1000)))
  { /* use standard BIOS method because timebase is short enough */
    //get_digin_period(ab_channel, &ui16_result, &ui16_counter);
    ui16_result = (((2 << (_b_prescale_1_Index + 2)) * ui16_result )/ (get_cpu_freq() * 1000));
  }
  else
  { /* use extension method */
    /* vars are accessible */
    if (getCounterLastSignalAge(ab_channel) < ui16_timebase)
    { // handle overflow between uiLast and uiAct
      ui16_result = _pt_diginTriggerTime[ab_channel].uiPeriod;
      if (ui16_result == 0) ui16_result = 1;
    }
  }
#endif

  return ui16_result;
}
/**
  get frequency of counter channel
  @return frequency calculated from time between last two signals
          or 0 if time is longer than initially given timebase [100mHz]
*/
uint32_t getCounterFrequency(uint8_t ab_channel)
{
	word ui16_result = 0;

#if 0
  uint16_t ui16_timebase = _puiDiginTimebase[ab_channel];
#endif

    int16_t retval = get_digin_freq((byte)ab_channel, &ui16_result);

#if DEBUG_HAL
  INTERNAL_DEBUG_DEVICE << __HAL::get_time() << " ms - " <<
    "get_digin_freq( " << unsigned(ab_channel) << ", " <<
    unsigned(ui16_result) << ", " <<
    " ) returns " << unsigned(retval) << "\r";
#endif

#if 0
  if (ui16_timebase == 0) ui16_result = 0;
  else if (ui16_timebase < (1024 * 65534L / (get_cpu_freq() * 1000)))
  { /* use standard BIOS method because timebase is short enough */
    get_digin_freq(ab_channel, &ui16_result);
  }
  else
  { /* use extension method */
    /* vars are accessible */
    uint16_t ui16_lastSignalAge = getCounterLastSignalAge(ab_channel);
    if (ui16_lastSignalAge < ui16_timebase)
    { // handle overflow between uiLast and uiAct
      uint16_t ui16_lastPeriod = _pt_diginTriggerTime[ab_channel].uiPeriod;
      if (ui16_lastPeriod == 0)
        ui16_result = 0;
      else
      {
        if ((ui16_lastPeriod * 2) < ui16_lastSignalAge) ui16_lastPeriod = ((int32_t)ui16_lastPeriod + (int32_t)ui16_lastSignalAge)/(int32_t)2;
        ui16_result = (10000 / ui16_lastPeriod);
      }
    }
  }
#endif
  return ui16_result;
}

/**
 get time since last signal and reset according trigger timers
 if timebase is exceeded -> avoid overflow problems if timer floated around 0xFFFF
 @return time since last signal [msec.]
*/
uint16_t getCounterLastSignalAge(uint8_t ab_channel)
{
#if 0
  uint16_t uiResult = 0xFFFF, uiTime = (get_time() & 0xFFFF);
  uint16_t ui16_period, ui16_actTime;
  ui16_period = _pt_diginTriggerTime[ab_channel].uiPeriod;
  ui16_actTime = _pt_diginTriggerTime[ab_channel].uiAct;
  if (ui16_period <= 0xFFFE)
  { // both values are valid and not resetted
    uiResult = (uiTime >= ui16_actTime)?(uiTime - ui16_actTime): (uiTime + (0xFFFF - ui16_actTime));
    // if timebase is exceeded -> reset *puiAct and *puiLast
    if (_puiDiginTimebase[ab_channel] < uiResult)
      _pt_diginTriggerTime[ab_channel].uiPeriod = 0xFFFF; // if both are equal answers in future time age 0xFFFF
  }
  return uiResult;
#endif
return 0;
}

} // end of namespace __HAL
