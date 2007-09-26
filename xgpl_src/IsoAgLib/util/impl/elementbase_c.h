/***************************************************************************
                          elementbase_c.h - header for base class for all
                                               IsoAgLib member objects which
                                               stores the pointer to the IsoAgLib
                                               instance, and delivers some
                                               access features to other
                                               components of the system
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                 *
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

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#ifndef ELEMENT_BASE_H
#define ELEMENT_BASE_H


/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
//#include <Application_Config/isoaglib_config.h>
#include <IsoAgLib/hal/config.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/saclaimhandler_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** Base class for member classes of IsoAgLib
  * which have a timeEvent() function, that
  * shall be called by Scheduler_c::timeEvent()
  *@author Dipl.-Inform. Achim Spangler
  */

  ///retriggerType_t for scheduling ElementBase_c Objects in Scheduler_c
  typedef enum { StandardRetrigger = 1, EarliestRetrigger = 2, LatestRetrigger = 4 } retriggerType_t;


class ElementBase_c : public SaClaimHandler_c {
 public:

  //Constructor
  ElementBase_c();

  /** this function is used by IsoAgLib components
    * to perform periodic timed tasks.
    * The central function Scheduler_c::timeEvent()
    * calls the timeEvent() function of all
    * IsoAgLib subsystem classes which are derived
    * from ElementBase_c and which registered
    * in the list of Scheduler_c during init
    * Call is wrapped by SchedulerEntry_c for Update of timestamps
    * @return true -> all planned activities performed in available time
    */
  virtual bool timeEvent( void ) = 0;


  /** this function is used by IsoAgLib components
    * to close all their ressources.
    * this enables a coordinated shutdown by Scheduler_c
    */
  virtual void close( void ) = 0;

  /** clear b_alreadyClosed so that close() can be called one time */
  void clearAlreadyClosed( void ) { b_alreadyClosed = false; }

  /** set b_alreadyClosed so that close() can't be called another time */
  void setAlreadyClosed( void ) { b_alreadyClosed = true; }

  /** check b_alreadyClosed to decide if close() can be called */
  bool checkAlreadyClosed( void ) const { return b_alreadyClosed; }


  /** Start Integrate Implementation for Timescheduling */
  /** ****************************** */

  //  Operation: forceExecStop
  //!  This static function can be called by the central Task-Scheduler or by RegisterInterrupts_c to force rapid stop of activities,
  //!  so that a very important other task can be started.
  //!  This function sets the i32_demandedExecEnd to 0 which commands the derived class timeEvent function to return activity as soon as possible
  //!  (without comprising the well defined internal state of the Sub-System).
  //!  By static accessibility the caller doesn't need to know, which derived ElementBase instance is working at the moment,
  //!  as all derived classes shares the same
  //!  static timestamps.
  static inline void forceExecStop();

  //  Operation: getExecTime
  //!  deliver the execution time for the last timeEvent call -> allows scheduler to refine time schedule within execution
  //!  @return last execution time in [msec] (off complete performed runs)
  inline uint16_t getExecTime() const;

  //  Operation: getTimeToNextTrigger
  //!  deliver the time [msec] to the next awaited retrigger of this task. The central scheduler can use the parameter to deduce the possible time
  //!  interval, which is important, if some tasks have time problems due to high load (data processing, IsoAgLib).
  //!  @return <0 -> too late, =0 -> trigger now, >0 -> wait
  //! Parameter:
  //! @param t_retriggerType: Bit-OR combination of [earliest|standard|latest]
  virtual int32_t getTimeToNextTrigger(retriggerType_t t_retriggerType= StandardRetrigger) const;

  //  Operation: getTimePeriod
  inline uint16_t getTimePeriod() const;

  //  Operation: startTaskTiming
  //!  this function is called at the end of system init, to set the trigger times to a suitable and realizable
  //!  start state, so that the scheduler can find
  //!  a correct calling order.
  //!  this is needed cause of long initial tests (esp.
  //!  single device power switch test with 5
  //!  devices with 1 second per device)
  //! Parameter:
  //! @param rint32_StartTaskTime: individual time offset, to avoid concurring tasks (if starting at same time with same period, the scheduler has every round a time problem)
  void startTaskTiming(int32_t rint32_StartTaskTime);

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const = 0;

  //  Operation: getAvgExecTime
  //!  this function is used for debug tests to get the AVG of exec time. This function uses the debug vars ui32_sumTime, ui32_callCnt
  //!  @return AVG of exec time
  inline uint16_t getAvgExecTime() const;

  //! virtual function which allows a scheduler client to define
  //! a minimum execution time, that should be saved after this item in the
  //! scheduler loop - some tasks might not be able to finish any sensible
  //! work in the default min exec time of 5msec
  virtual uint16_t getForcedMinExecTime() const;

#ifdef DEBUG_SCHEDULER
  //  Operation: getMaxExecTime
  //!  deliver the max exec time of this task (used only for debug based on ui16_maxTime)
  inline uint16_t getMaxExecTime() const;

  //  Operation: getMinExecTime
  //!  deliver the min exec time of this task (used only for debug based on ui16_minTime)
  inline uint16_t getMinExecTime() const;

  //  Operation: getAvgTimingAccuracy
  inline int16_t getAvgTimingAccuracy() const;

  //  Operation: getMinTimingAccuracy
  //!  deliver MIN time between actual and next planned execution
  //!  (> 0 -> better than planned, < 0 -> worse than planned)
  //!  (only needed for debug tests)
  inline int16_t getMinTimingAccuracy() const;

  //  Operation: getMaxTimingAccuracy
  //!  deliver MAX time between actual and next planned execution
  //!  (> 0 -> better than planned, < 0 -> worse than planned)
  //!  (only needed for debug tests)
  inline int16_t getMaxTimingAccuracy() const;
#endif
  //  Operation: getStdTimeToNextTrigger
  //!  deliver standard time till next retrigger (used for comparisong operators in priority queue of SystemManagement_c -> must be very quick as very often called)
  int32_t getStdTimeToNextTrigger() const;

  //  Operation: getNextTriggerTime
  //!  deliver timestamp of next planned retrigger (timeEvent() call)
  inline int32_t getNextTriggerTime() const;

  //  Operation: getEarlierInterval
  inline uint16_t getEarlierInterval() const;

  //  Operation: getLaterInterval
  inline uint16_t getLatestInterval() const ;

  //  Operation: getAvailableExecTime
  //!  Deliver the available execution time for actual timeEvent() run.
  //!  As the variables for decision on result of this function are static, this function can be also static.
  //!  Therefore sub-elements of the actual working
  //!  Sub-System can simply get informaiton
  //!  on available time.
  //!  @return 0 == immediate return is forced, <0 == unrestricted time, else available time in [msec]
  static int16_t getAvailableExecTime();

  //  Operation: getDemandedExecEnd
  //!  Deliver the registered exec end timestamp.
  static inline int32_t getDemandedExecEnd();

  //  Operation: getLastRetriggerTime
  static inline int32_t getLastRetriggerTime();

  //! delay the next execution time by given period
  void delayNextTriggerTime( unsigned int ui_delay ) { i32_nextRetriggerTime += ui_delay;}

  //! delay the next retrigger time to the given timestamp
  void delayNextTriggerTimeToTime( int32_t ri32_newNextRetrigger ) { i32_nextRetriggerTime = ri32_newNextRetrigger;}


  //!  Virtual Destructor - just to avoid compiler warnings
  virtual ~ElementBase_c();



  /** End Integrate Publics Implementation for Timescheduling */

  /** Start Integrate Protected Implementation for Timescheduling */
protected:

  //  Operation: setTimePeriod
  //!  Set client specific time period between calls of timeEvent.
  //!  Each from ElementBase_c derived class must set at its INIT
  //!  or can use it to set other Time Period in timeEvent()
  //!  Do NOT call from outside (e.g. processMsg)
  //! Parameter:
  //! @param rui16_timePeriod: needed time between calls of timeEvent in [msec]
  void setTimePeriod(uint16_t rui16_timePeriod);

  //! Deliver the max Jitter in ms that uses the
  //! Scheduler for earlier oder later call of a task
  inline uint16_t getMaxRetriggerJitter() const;

  //! Function set ui16_earlierInterval and
  //! ui16_laterInterval that will be used by
  //! getTimeToNextTrigger(retriggerType_t)
  //! can be overloaded by Childclass for special condition
  virtual void updateEarlierAndLatestInterval();

  //! contains timeSpread for that a Task can be called EARLIER
  uint16_t ui16_earlierInterval;

  //! contains timeSpread for that a Task can be called LATER
  uint16_t ui16_latestInterval;

  /** End Integrate Protected Implementation for Timescheduling */

 private:

   //for Call of timeEventPre, timeEventPostUpdateStatistics
   friend class SchedulerEntry_c;

  //  Operation: timeEventPre
  //!  This function is called by the schedulerEntry_C to update timestamps
  //!  The parameter tells the task the available time for execution.
  //! @param ri32_demandedExecEnd: available execution time. timeEvent() MUST be finished before the time, to avoid scheduling problems.
  //!                              default value -1 == unrestricted time for execution.
  void timeEventPre(int32_t ri32_demandedExecEnd= -1) ;

  //  Operation: timeEventPostUpdateStatistics
  //!  This function is called from the timeEvent function if all periodic activities were performed (-> don't call  if earlier return was forced).
  //!  Update the average execution time ui16_approxExecTime
  void timeEventPostUpdateStatistics();

  //  Operation: changeNextTriggerTime
  //!  change timestamp of next planned retrigger (timeEvent() call)
  //!  ONLY for use by Scheduler_c to move task in TaskQueue
  //!  @param ri32_nextRetriggerTime new timeStamp for retrigger
  inline void changeNextTriggerTime(int32_t ri32_nextRetriggerTime) ;

  //  Attribute: b_alreadyClosed
  /** store already called close() after last call of init() */
  bool b_alreadyClosed;

  /** Start Integrate private Implementation for Timescheduling */
  //! counter to detect amount of timeEvent() calls, where the next task in the Scheduler_c
  //! queue had a too early/tight latest retrigger time
  int i_executionTimeHealth;

  //!  at the end of timeEvent() the function timeEventPostUpdateStatistics() stores the next retrigger timestamp
  int32_t i32_nextRetriggerTime;

  //  Attribute: i32_demandedExecEnd
  static int32_t i32_demandedExecEnd;

  //  Attribute: i32_retriggerTime
  static int32_t i32_retriggerTime;

  //  Attribute: ui32_callCnt
  //!  amount of task executions (only needed for debug tests to build AVG of execution time)
  uint32_t ui32_callCnt;

  //  Attribute: ui32_sumTime
  uint32_t ui32_sumTime;
#ifdef DEBUG_SCHEDULER
  //  Attribute: i32_sumTimingAccuracy
  //!  SUM of time between actual and next planned execution (to calc AVG)
  //!  (> 0 -> better than planned, < 0 -> worse than planned)
  //!  (only needed for debug tests)
  int32_t i32_sumTimingAccuracy;

  //  Attribute: i16_minTimingAccuracy
  //!  min time between actual and next planned execution
  //!  (> 0 -> better than planned, < 0 -> worse than planned)
  //!  (only needed for debug tests)
  int16_t i16_minTimingAccuracy;

  //  Attribute: i16_maxTimingAccuracy
  //!  max time between actual and next planned execution
  //!  (> 0 -> better than planned, < 0 -> worse than planned)
  //!  (only needed for debug tests)
  int16_t i16_maxTimingAccuracy;

  //  Attribute: ui16_minTime
  //!  minimum execution time (only needed for debug tests)
  uint16_t ui16_minTime;

  //  Attribute: ui16_maxTime
  //!  maximum execution time (only needed for debug tests)
  uint16_t ui16_maxTime;
#endif
  //  Attribute: ui16_approxExecTime
  //!  approximate runtim of the timeEvent() function
  uint16_t ui16_approxExecTime;

  //  Attribute: ui16_timePeriod
  //!  standard period in [msec] when timeEvent() should be called
  uint16_t ui16_timePeriod;

  /** End Integrate private Implementation for Timescheduling */

};

/** Start Integrate inline functions Implementation for Timescheduling */

//!  deliver the average execution time for timeEvent calls -> allows scheduler to refine time schedule within execution
//!  @return average execution time in [msec] (off complete performed runs)
inline
uint16_t
ElementBase_c::getExecTime() const
{
  return ui16_approxExecTime;
}



inline
uint16_t
ElementBase_c::getTimePeriod() const
{
  return ui16_timePeriod;
}


//!  this function is used for debug tests to get the AVG of exec time. This function uses the debug vars ui32_sumTime, ui32_callCnt
//!  @return AVG of exec time
inline
uint16_t
ElementBase_c::getAvgExecTime() const
{
  return ( ui32_callCnt != 0 )?( ui32_sumTime / ui32_callCnt ):0;
}
#ifdef DEBUG_SCHEDULER
//!  deliver the max exec time of this task (used only for debug based on ui16_maxTime)
inline
uint16_t
ElementBase_c::getMaxExecTime() const
{
  return ui16_maxTime;
}

//!  deliver the min exec time of this task (used only for debug based on ui16_minTime)
inline
uint16_t
ElementBase_c::getMinExecTime() const
{
  return ui16_minTime;
}
inline
int16_t
ElementBase_c::getAvgTimingAccuracy() const
{
  return ( ui32_callCnt != 0 )?( i32_sumTimingAccuracy / ui32_callCnt ):0;
}

//!  deliver MIN time between actual and next planned execution
//!  (> 0 -> better than planned, < 0 -> worse than planned)
//!  (only needed for debug tests)
inline
int16_t
ElementBase_c::getMinTimingAccuracy() const
{
  return i16_minTimingAccuracy;
}

//!  deliver MAX time between actual and next planned execution
//!  (> 0 -> better than planned, < 0 -> worse than planned)
//!  (only needed for debug tests)
inline
int16_t
ElementBase_c::getMaxTimingAccuracy() const
{
  return i16_maxTimingAccuracy;
}
#endif
//!  This static function can be called by the central Task-Scheduler or by RegisterInterrupts_c to force rapid stop of activities,
//!  so that a very important other task can be started.
//!  This function sets the i32_demandedExecEnd to 0 which commands the derived class timeEvent function to return activity as soon as possible
//!  (without comprising the well defined internal state of the Sub-System).
//!  By static accessibility the caller doesn't need to know, which derived ElementBase instance is working at the moment,
//!  as all derived classes shares the same
//!  static timestamps.
inline
void
ElementBase_c::forceExecStop()
{
  // the value 0 is telling timeEvent() and all called functions that an immediated stop is needed
  i32_demandedExecEnd = 0;
}

inline
int32_t
ElementBase_c::getLastRetriggerTime()
{
  return i32_retriggerTime;
}

//!  Deliver the registered exec end timestamp.
inline
int32_t
ElementBase_c::getDemandedExecEnd()
{
  return i32_demandedExecEnd;
}

//! Deliver the max Jitter in ms that uses the
//! Scheduler for earlier oder later call of a task
inline
uint16_t
ElementBase_c::getMaxRetriggerJitter() const
{
  return (ui16_earlierInterval + ui16_latestInterval) ;
}



//!  deliver timestamp of next planned retrigger (timeEvent() call)
inline
int32_t
ElementBase_c::getNextTriggerTime() const
{
  return i32_nextRetriggerTime;
}

//! get Interval for earliest call of Task
inline
uint16_t
ElementBase_c::getEarlierInterval() const
{
 return ui16_earlierInterval;
}

//! get Interval for latest call of Task
inline
uint16_t
ElementBase_c::getLatestInterval() const
{
 return ui16_latestInterval;
}

//!  change timestamp of next planned retrigger (timeEvent() call)
//!  ONLY for use by Scheduler_c to move task in TaskQueue
//!  @param ri32_nextRetriggerTime new timeStamp for retrigger
inline
void
ElementBase_c::changeNextTriggerTime(int32_t ri32_nextRetriggerTime){
  i32_nextRetriggerTime = ri32_nextRetriggerTime;
}

} // end namespace __IsoAgLib
#endif
