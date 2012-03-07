/*
  schedulertask_c.h: header for base class for all IsoAgLib member
    objects which stores the pointer to the IsoAgLib instance, and
    delivers some access features to other components of the system

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SCHEDULERTASK_C_H
#define SCHEDULERTASK_C_H

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/util/impl/util_funcs.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** Base class for member classes of IsoAgLib
  * which have a timeEvent() function, that
  * shall be called by Scheduler_c::timeEvent()
  *@author Dipl.-Inform. Achim Spangler
  */

  ///retriggerType_t for scheduling Scheduler_Task_c Objects in Scheduler_c
  typedef enum retriggerType_en { StandardRetrigger = 1, EarliestRetrigger = 2, LatestRetrigger = 4 } retriggerType_t;


// All Scheduler-Tasks are subsystems, because of
// the not-possible multiple-inheritance right now
class Scheduler_Task_c : public Subsystem_c
{
 public:
  Scheduler_Task_c();

  /** this function is used by IsoAgLib components
    * to perform periodic timed tasks.
    * The central function Scheduler_c::timeEvent()
    * calls the timeEvent() function of all
    * IsoAgLib subsystem classes which are derived
    * from Scheduler_Task_c and which registered
    * in the list of Scheduler_c during init
    * Call is wrapped by SchedulerEntry_c for Update of timestamps
    * @return true -> all planned activities performed in available time
    */
  virtual bool timeEvent() = 0;



  /** Start Integrate Implementation for Timescheduling */
  /** ****************************** */

  //  Operation: forceExecStop
  //!  This static function can be called by the central Task-Scheduler or by RegisterInterrupts_c to force rapid stop of activities,
  //!  so that a very important other task can be started.
  //!  This function sets the msi32_demandedExecEnd to 0 which commands the derived class timeEvent function to return activity as soon as possible
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
  virtual int32_t getTimeToNextTrigger(retriggerType_t t_retriggerType= StandardRetrigger) const {
    return getTimeToNextTriggerDefault(t_retriggerType);
  };


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

#if DEBUG_SCHEDULER
  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const { return NULL; }
#endif

  //  Operation: getAvgExecTime
  //!  this function is used for debug tests to get the AVG of exec time. This function uses the debug vars mui32_sumTime, mui32_callCnt
  //!  @return AVG of exec time
  inline uint16_t getAvgExecTime() const;

  //! virtual function which allows a scheduler client to define
  //! a minimum execution time, that should be saved after this item in the
  //! scheduler loop - some tasks might not be able to finish any sensible
  //! work in the default min exec time of 5msec
  virtual uint16_t getForcedMinExecTime() const {
    return getForcedMinExecTimeDefault();
  }

  // Default implementation for method getForcedMinExecTime.
  uint16_t getForcedMinExecTimeDefault() const;

#if DEBUG_SCHEDULER
  //  Operation: getMaxExecTime
  //!  deliver the max exec time of this task (used only for debug based on mui16_maxTime)
  inline uint16_t getMaxExecTime() const;

  //  Operation: getMinExecTime
  //!  deliver the min exec time of this task (used only for debug based on mui16_minTime)
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

   //  Operation: setDemandedExecEnd
  //!  Sets the registered exec end timestamp.
  static inline void setDemandedExecEnd(int32_t ai32_demandenExecTime) {  msi32_demandedExecEnd = ai32_demandenExecTime;};

  //  Operation: getLastRetriggerTime
  static inline int32_t getLastRetriggerTime();

  //! delay the next execution time by given period
  void delayNextTriggerTime( unsigned int ui_delay ) { mi32_nextRetriggerTime += ui_delay;}

  //! delay the next retrigger time to the given timestamp
  void delayNextTriggerTimeToTime( int32_t ai32_newNextRetrigger ) { mi32_nextRetriggerTime = ai32_newNextRetrigger;}


  //!  Virtual Destructor - just to avoid compiler warnings
  virtual ~Scheduler_Task_c();

  // Default implementation for method getTimeToNextTrigger
  int32_t getTimeToNextTriggerDefault(retriggerType_t t_retriggerType = StandardRetrigger) const;

  // Default implementation for method updateEarlierAndLatestInterval
  void updateEarlierAndLatestIntervalDefault();

  /** End Integrate Publics Implementation for Timescheduling */

  /** Start Integrate Protected Implementation for Timescheduling */
protected:

  //  Operation: setTimePeriod
  //!  Set client specific time period between calls of timeEvent.
  //!  Each from Scheduler_Task_c derived class must set at its INIT
  //!  or can use it to set other Time Period in timeEvent()
  //!  Do NOT call from outside (e.g. processMsg)
  //! Parameter:
  //! @param aui16_timePeriod: needed time between calls of timeEvent in [msec]
  void setTimePeriod(uint16_t aui16_timePeriod);

  //! Deliver the max Jitter in ms that uses the
  //! Scheduler for earlier oder later call of a task
  inline uint16_t getMaxRetriggerJitter() const;

  //! Function set mui16_earlierInterval and
  //! ui16_laterInterval that will be used by
  //! getTimeToNextTrigger(retriggerType_t)
  //! can be overloaded by Childclass for special condition
  virtual void updateEarlierAndLatestInterval() {
    updateEarlierAndLatestIntervalDefault();
  }

  //! Operation: setEarlierInterval
  //! Set lowest time interval that can be handled by the application. The time period of a task is within the range
  //! of ( timePeriod - earlierInterval ) <= timePeriod <= (timePeriod + latestInterval )
  //! Parameter:
  //! @param uint16_t aui16_interval: smallest possible interval between two time Events (timePeriod - aui16_interval) [msec]
  void setEarlierInterval( uint16_t aui16_interval ) { mui16_earlierInterval = aui16_interval; }

  //! Operation: setLatestInterval
  //! Set latest time interval that can be handled by the application. The time period of a task is within the range
  //! of ( timePeriod - earlierInterval ) <= timePeriod <= (timePeriod + latestInterval )
  //! Parameter:
  //! @param uint16_t aui16_interval: biggest possible interval between two time Events (timePeriod + aui16_interval) [msec]
  void setLatestInterval( uint16_t aui16_interval ) { mui16_latestInterval = aui16_interval; }

  //! contains timeSpread for that a Task can be called EARLIER
  uint16_t mui16_earlierInterval;

  //! contains timeSpread for that a Task can be called LATER
  uint16_t mui16_latestInterval;

  /** End Integrate Protected Implementation for Timescheduling */

 private:

   //for Call of timeEventPre, timeEventPostUpdateStatistics
   friend class SchedulerEntry_c;

  //  Operation: timeEventPre
  //!  This function is called by the schedulerEntry_C to update timestamps
  //!  The parameter tells the task the available time for execution.
  //! @param ai32_demandedExecEnd: available execution time. timeEvent() MUST be finished before the time, to avoid scheduling problems.
  //!                              default value -1 == unrestricted time for execution.
  void timeEventPre(int32_t ai32_demandedExecEnd= -1) ;

  //  Operation: timeEventPostUpdateStatistics
  //!  This function is called from the timeEvent function if all periodic activities were performed (-> don't call  if earlier return was forced).
  //!  Update the average execution time mui16_approxExecTime
  void timeEventPostUpdateStatistics();

  //  Operation: changeNextTriggerTime
  //!  change timestamp of next planned retrigger (timeEvent() call)
  //!  ONLY for use by Scheduler_c to move task in TaskQueue
  //!  @param ai32_nextRetriggerTime new timeStamp for retrigger
  inline void changeNextTriggerTime(int32_t ai32_nextRetriggerTime) ;

  /** Start Integrate private Implementation for Timescheduling */
  //! counter to detect amount of timeEvent() calls, where the next task in the Scheduler_c
  //! queue had a too early/tight latest retrigger time
  int mi_executionTimeHealth;

  //!  at the end of timeEvent() the function timeEventPostUpdateStatistics() stores the next retrigger timestamp
  int32_t mi32_nextRetriggerTime;

  //  Attribute: msi32_demandedExecEnd
  static int32_t msi32_demandedExecEnd;

  //  Attribute: msi32_retriggerTime
  static int32_t msi32_retriggerTime;

  //  Attribute: mui32_callCnt
  //!  amount of task executions (only needed for debug tests to build AVG of execution time)
  uint32_t mui32_callCnt;

  //  Attribute: mui32_sumTime
  uint32_t mui32_sumTime;
#if DEBUG_SCHEDULER
  //  Attribute: mi32_sumTimingAccuracy
  //!  SUM of time between actual and next planned execution (to calc AVG)
  //!  (> 0 -> better than planned, < 0 -> worse than planned)
  //!  (only needed for debug tests)
  int32_t mi32_sumTimingAccuracy;

  //  Attribute: mi16_minTimingAccuracy
  //!  min time between actual and next planned execution
  //!  (> 0 -> better than planned, < 0 -> worse than planned)
  //!  (only needed for debug tests)
  int16_t mi16_minTimingAccuracy;

  //  Attribute: mi16_maxTimingAccuracy
  //!  max time between actual and next planned execution
  //!  (> 0 -> better than planned, < 0 -> worse than planned)
  //!  (only needed for debug tests)
  int16_t mi16_maxTimingAccuracy;

  //  Attribute: mui16_minTime
  //!  minimum execution time (only needed for debug tests)
  uint16_t mui16_minTime;

  //  Attribute: mui16_maxTime
  //!  maximum execution time (only needed for debug tests)
  uint16_t mui16_maxTime;
#endif
  //  Attribute: mui16_approxExecTime
  //!  approximate runtim of the timeEvent() function
  uint16_t mui16_approxExecTime;

  //  Attribute: mui16_timePeriod
  //!  standard period in [msec] when timeEvent() should be called
  uint16_t mui16_timePeriod;

  /** End Integrate private Implementation for Timescheduling */

};

/** Proxy for Scheduler_Task_c.
  * Having such a proxy as component is an alternative to subclassing
  * Scheduler_Task_c directly.
  */
template < typename OWNER_T >
class SchedulerTaskProxy_c : public Scheduler_Task_c {
public:
  typedef OWNER_T Owner_t;

  SchedulerTaskProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

  virtual ~SchedulerTaskProxy_c() {}

  using Scheduler_Task_c::setTimePeriod;

private:
  virtual bool timeEvent() {
    return mrt_owner.timeEvent();
  }

  virtual int32_t getTimeToNextTrigger(retriggerType_en e_retriggerType = StandardRetrigger) const {
    return mrt_owner.getTimeToNextTrigger(e_retriggerType);
  }

#if DEBUG_SCHEDULER
  virtual const char *getTaskName() const {
    return mrt_owner.getTaskName();
  }
#endif

  virtual void updateEarlierAndLatestInterval() {
    mrt_owner.updateEarlierAndLatestInterval();
  }

  virtual uint16_t getForcedMinExecTime() const {
    return mrt_owner.getForcedMinExecTime();
  }

  // SchedulerTaskProxy_c shall not be copyable. Otherwise the
  // reference to the containing object would become invalid.
  SchedulerTaskProxy_c(SchedulerTaskProxy_c const &);

  SchedulerTaskProxy_c &operator=(SchedulerTaskProxy_c const &);

  Owner_t &mrt_owner;
};


/** Start Integrate inline functions Implementation for Timescheduling */

//!  deliver the average execution time for timeEvent calls -> allows scheduler to refine time schedule within execution
//!  @return average execution time in [msec] (off complete performed runs)
inline
uint16_t
Scheduler_Task_c::getExecTime() const
{
  return mui16_approxExecTime;
}



inline
uint16_t
Scheduler_Task_c::getTimePeriod() const
{
  return mui16_timePeriod;
}


//!  this function is used for debug tests to get the AVG of exec time. This function uses the debug vars mui32_sumTime, mui32_callCnt
//!  @return AVG of exec time
inline
uint16_t
Scheduler_Task_c::getAvgExecTime() const
{
  return static_cast<uint16_t>( ( mui32_callCnt != 0 )?( mui32_sumTime / mui32_callCnt ):0 );
}
#if DEBUG_SCHEDULER
//!  deliver the max exec time of this task (used only for debug based on mui16_maxTime)
inline
uint16_t
Scheduler_Task_c::getMaxExecTime() const
{
  return mui16_maxTime;
}

//!  deliver the min exec time of this task (used only for debug based on mui16_minTime)
inline
uint16_t
Scheduler_Task_c::getMinExecTime() const
{
  return mui16_minTime;
}
inline
int16_t
Scheduler_Task_c::getAvgTimingAccuracy() const
{
  return ( mui32_callCnt != 0 )?( mi32_sumTimingAccuracy / mui32_callCnt ):0;
}

//!  deliver MIN time between actual and next planned execution
//!  (> 0 -> better than planned, < 0 -> worse than planned)
//!  (only needed for debug tests)
inline
int16_t
Scheduler_Task_c::getMinTimingAccuracy() const
{
  return mi16_minTimingAccuracy;
}

//!  deliver MAX time between actual and next planned execution
//!  (> 0 -> better than planned, < 0 -> worse than planned)
//!  (only needed for debug tests)
inline
int16_t
Scheduler_Task_c::getMaxTimingAccuracy() const
{
  return mi16_maxTimingAccuracy;
}
#endif

inline
int32_t
Scheduler_Task_c::getLastRetriggerTime()
{
  return msi32_retriggerTime;
}

//!  Deliver the registered exec end timestamp.
inline
int32_t
Scheduler_Task_c::getDemandedExecEnd()
{
  return msi32_demandedExecEnd;
}

//! Deliver the max Jitter in ms that uses the
//! Scheduler for earlier oder later call of a task
inline
uint16_t
Scheduler_Task_c::getMaxRetriggerJitter() const
{
  return (mui16_earlierInterval + mui16_latestInterval) ;
}



//!  deliver timestamp of next planned retrigger (timeEvent() call)
inline
int32_t
Scheduler_Task_c::getNextTriggerTime() const
{
  return mi32_nextRetriggerTime;
}

//! get Interval for earliest call of Task
inline
uint16_t
Scheduler_Task_c::getEarlierInterval() const
{
 return mui16_earlierInterval;
}

//! get Interval for latest call of Task
inline
uint16_t
Scheduler_Task_c::getLatestInterval() const
{
 return mui16_latestInterval;
}

//!  change timestamp of next planned retrigger (timeEvent() call)
//!  ONLY for use by Scheduler_c to move task in TaskQueue
//!  @param ai32_nextRetriggerTime new timeStamp for retrigger
inline
void
Scheduler_Task_c::changeNextTriggerTime(int32_t ai32_nextRetriggerTime){
  mi32_nextRetriggerTime = ai32_nextRetriggerTime;
}

} // end namespace __IsoAgLib
#endif
