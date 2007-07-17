// ----------------------------------------------------------------------
// File    : SystemManagement/elementbase_c.cc
// Class   : SystemManagement::ElementBase_c
// Project : xmi2code
// Author  : Achim Spangler (a.spangler@osb-ag.de)
// Created : Thu Apr 3 14:28:42 2003
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
// $Id: elementbase_c.cpp 838 2004-11-05 21:30:12Z spangler $
//
// $Log$
//
// ----------------------------------------------------------------------


#if defined(DEBUG)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif


#include "elementbase_c.h"

#define DEF_CONSOLIDATION_LIMIT_FOR_DELAY_1MSEC 3

namespace __IsoAgLib {

// ////////////////////////////// Operation 1398 : ElementBase_c

ElementBase_c::ElementBase_c()
: ui16_earlierInterval(75) //default 3/4 of timeperiod
, ui16_latestInterval(50)   //default 1/2 of timeperiod
, i_executionTimeHealth( 0 )
, i32_nextRetriggerTime( 0 )
, ui32_callCnt( 0 )
, ui32_sumTime( 0 )
#ifdef DEBUG_SCHEDULER
, i32_sumTimingAccuracy( 0 )
, i16_minTimingAccuracy( 32767 )
, i16_maxTimingAccuracy( -3276 )
, ui16_minTime( 0xFFFF )
, ui16_maxTime( 0 )
#endif
, ui16_approxExecTime( 0 )
, ui16_timePeriod( 100 )   //Long value for TimePeriod as default
{
}




// ////////////////////////////// Operation 1402 : getTimeToNextTrigger
//!  deliver the time [msec] to the next awaited retrigger of this task. The central scheduler can use the parameter to deduce the possible time
//!  interval, which is important, if some tasks have time problems due to high load (data processing, IsoAgLib).
//!  @return <0 -> too late, =0 -> trigger now, >0 -> wait
//! Parameter:
//! @param t_retriggerType: Bit-OR combination of [earliest|standard|latest]
int32_t
ElementBase_c::getTimeToNextTrigger(retriggerType_t t_retriggerType) const
{

  int32_t i32_now = System_c::getTime();
  int32_t i32_earlierTime = 0,
          i32_resultTime = 0;

  // first check for earliest setting
  if ( ( t_retriggerType & EarliestRetrigger ) != 0 )
  { // allow  shorter time between two retriggers (default 75 %)
    // in relation to standard period
    // avoid umber overflow -> restrict to max limits of int16_t
    i32_earlierTime = i32_resultTime = ( ( i32_nextRetriggerTime - int32_t(ui16_earlierInterval) ) - i32_now );

    // if earliest retrigger time results in immediate execution (0)
    // or idle time (>0) -> return with this setting
    if ( i32_resultTime >= 0 ) return i32_resultTime;
  }
  if ( ( t_retriggerType & StandardRetrigger ) != 0 )
  { // standard request -> answer normal timing
    // avoid umber overflow -> restrict to max limits of int16_t
    i32_resultTime = ( i32_nextRetriggerTime - i32_now );
    // if i16_earliestTime < 0 (too late) and i16_standardTime > 0 (too early)
    // -> actual time is inbetween -> return 0 to execute immediate
    if ( ( i32_earlierTime < 0 ) && ( i32_resultTime > 0 ) ) return 0;

    // if standard retrigger time results in immediate execution (0)
    // or idle time (>0) -> return with this setting
    if ( i32_resultTime >= 0 ) return i32_resultTime;
    // store standard time as earlier time for check in LatestRetrigger
    else i32_earlierTime = i32_resultTime;
  }
  if ( ( t_retriggerType & LatestRetrigger ) != 0 )
  { // allow (default 50 %) longer time between two retriggers
    i32_resultTime = ( i32_nextRetriggerTime + int32_t( ui16_latestInterval ) - i32_now );
    // if i16_standardTime < 0 (too late) and i16_latestTime > 0 (too early)
    // -> actual time is inbetween -> return 0 to execute immediate
    if ( ( i32_earlierTime < 0 ) && ( i32_resultTime > 0 ) ) return 0;
  }
  return i32_resultTime;
}



//!  This function is called by the schedulerEntry_C to update timestamps
//!  The parameter tells the task the available time for execution.
//! @param ri32_demandedExecEnd: available execution time. timeEvent() MUST be finished before the time, to avoid scheduling problems.
//!                              default value -1 == unrestricted time for execution.
void
ElementBase_c::timeEventPre(int32_t ri32_demandedExecEnd)
{
  /// store the demanded exec end

  i32_demandedExecEnd = ri32_demandedExecEnd;
  i32_retriggerTime = System_c::getTime();
  // derived classes should call base function at start
  // after update of i32_demandedExecEnd, the derived class specific
  // actions can be performed
}


//!  This function is called from the timeEventExec function
//!  if timeEvent() has all periodic activities performed
///! (-> DO NOT call  if earlier return was forced).
//!  Update the average execution time ui16_approxExecTime
void
ElementBase_c::timeEventPostUpdateStatistics()
{

  const int32_t ci32_now = System_c::getTime();
  ui16_approxExecTime = uint16_t(ci32_now - i32_retriggerTime);
  // update DBUG time values
  ui32_callCnt++;
  if ( ui32_callCnt != 0 )
  {
    ui32_sumTime += ui16_approxExecTime;
    #ifdef DEBUG_SCHEDULER
    if ( ui16_approxExecTime > ui16_maxTime ) ui16_maxTime = ui16_approxExecTime;
    if ( ui16_approxExecTime < ui16_minTime ) ui16_minTime = ui16_approxExecTime;
    const int32_t i32_tempTriggerTimeDelta = ( i32_nextRetriggerTime - i32_retriggerTime );
    i32_sumTimingAccuracy += i32_tempTriggerTimeDelta;
    if ( i32_tempTriggerTimeDelta > i16_maxTimingAccuracy ) i16_maxTimingAccuracy = i32_tempTriggerTimeDelta;
    if ( i32_tempTriggerTimeDelta < i16_minTimingAccuracy ) i16_minTimingAccuracy = i32_tempTriggerTimeDelta;
    #endif
  }
  else
  { /// ui32_callCnt overflow -> reset values to get valid AVG,...
    ui32_sumTime = 0;
    #ifdef DEBUG_SCHEDULER
    ui16_maxTime = 0;
    ui16_minTime = 0xFFFF;
    i32_sumTimingAccuracy = 0;
    i16_maxTimingAccuracy = -32766;
    i16_minTimingAccuracy = 32767;
    #endif
  }


  /// update i32_nextRetriggerTime
  // -> constant add the time period, so that independ from
  // a earlier or later call, the basic distance ( amount of calls in complete time )
  // is constant
  // otherwise a task coul be called always a earlier than planned. so that the
  // amount of calls is very high
  i32_nextRetriggerTime += ui16_timePeriod;

  if ( ci32_now >= i32_nextRetriggerTime )
  { //the last execution was too much delayed, so that the next execution timestamp would be in future (calculated from the "planned" time!!)
    i32_nextRetriggerTime = i32_retriggerTime + ui16_timePeriod;
    if ( ci32_now >= i32_nextRetriggerTime )
    { // now try with the REAL time the task was started and not with the PLANNED time!
      // --> replace next retrigger time with now+period
      i32_nextRetriggerTime = ci32_now + ui16_timePeriod;
    }
  }
}

// ////////////////////////////// Operation 2727 : getAvailableExecTime
//!  Deliver the available execution time for actual timeEvent() run.
//!  As the variables for decision on result of this function are static, this function can be also static.
//!  Therefore sub-elements of the actual working
//!  Sub-System can simply get informaiton
//!  on available time.
//!  @return 0 == immediate return is forced, <0 == unrestricted time, else available time in [msec]
int16_t
ElementBase_c::getAvailableExecTime()
{
  if ( i32_demandedExecEnd == 0 ) return 0;
  if ( i32_demandedExecEnd == -1 ) return -1;

  int32_t i32_now = System_c::getTime();

  if ( i32_now >= i32_demandedExecEnd ) return 0;
  else return ( i32_demandedExecEnd - i32_now );
}

 /// static_attributes
int32_t ElementBase_c::i32_demandedExecEnd= -1;
int32_t ElementBase_c::i32_retriggerTime= 0;

//!  this function is called at the end of system init, to set the trigger times to a suitable and realizable
//!  start state, so that the scheduler can find
//!  a correct calling order.
//!  this is needed cause of long initial tests (esp.
//!  single device power switch test with 5
//!  devices with 1 second per device)
//! Parameter:
//! @param int32_StartTaskTime: individual time offset, to avoid concurring tasks (if starting at same time with same period, the scheduler has every round a time problem)
void ElementBase_c::startTaskTiming(int32_t rint32_StartTaskTime)
{
  i32_nextRetriggerTime = rint32_StartTaskTime;
}

//!  Each from ElementBase_c derived class must set at its init
//!  the needed time period between calls of timeEvent.
//! Parameter:
//! @param rui16_timePeriod: needed time between calls of timeEvent in [msec]
void
ElementBase_c::setTimePeriod
(uint16_t rui16_timePeriod)
{
  ui16_timePeriod = rui16_timePeriod;
  //call Function to calculate new intervals
  updateEarlierAndLatestInterval();
  #ifdef DEBUG_SCHEDULER
  INTERNAL_DEBUG_DEVICE
      << "ElementBase_c::setTimePeriod( " << rui16_timePeriod << ") zu Task "
      << getTaskName() << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
}

//!  deliver standard time till next retrigger (used for comparisong operators in priority queue of SystemManagement_c -> must be very quick as very often called)
int32_t
ElementBase_c::getStdTimeToNextTrigger() const
{
  int32_t i32_temp = ( i32_nextRetriggerTime - System_c::getTime() );
  if ( i32_temp < -32767 ) return -32767;
  else if ( i32_temp > 32767 ) return 32767;
  else return i32_temp;
}


//! Function set ui16_earlierInterval and
//! ui16_laterInterval that will be used by
//! getTimeToNextTrigger(retriggerType_t)
//! can be overloaded by Childclass for special condition
void
ElementBase_c::updateEarlierAndLatestInterval(){
  ui16_earlierInterval = ( ( getTimePeriod() * 3) / 4);
  ui16_latestInterval   =  ( getTimePeriod() / 2) ;
}

//!  Virtual Destructor - just to avoid compiler warnings
ElementBase_c::~ElementBase_c()
{
	///b_isInitialized = false;
}



//! virtual function which allows a scheduler client to define
//! a minimum execution time, that should be saved after this item in the
//! scheduler loop - some tasks might not be able to finish any sensible
//! work in the default min exec time of 5msec
uint16_t ElementBase_c::getForcedMinExecTime() const
{
  return 5;
}


} /// end of namespace
