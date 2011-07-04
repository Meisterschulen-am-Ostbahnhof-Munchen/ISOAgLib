/*
  schedulerentry_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SYSTEMMANAGEMENT_SCHEDULERENTRY_C_H
#define SYSTEMMANAGEMENT_SCHEDULERENTRY_C_H


#include <IsoAgLib/scheduler/impl/schedulertask_c.h>


/// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

//  +X2C Class 2690 : SchedulerEntry_c
//!  Wrapper class for poitners to already existing singleton-Scheduler_Task_c objects.
//!  This class provides the needed comparison operators for management of scheduler lists.
class SchedulerEntry_c
{

public:

  //  Operation: SchedulerEntry_c
  //!  Constructor which takes a pointer to the managed from Scheduler_Task_c derived class.
  //! Parameter:
  //! @param apc_entry: Pointer to the instance of the represented Scheduler_Task_c derived instance.
  inline SchedulerEntry_c(Scheduler_Task_c* apc_entry= NULL);

  //  Operation: operator<=
  //!  Compare retriger time of this item by operator<= with parameter
  //!  @return true -> this item should be triggered first (retrigger times can ba equal)
  //! Parameter:
  //! @param acrc_rightSide: Reference to the right side for the lower-comparison
  inline bool operator<=(const SchedulerEntry_c& acrc_rightSide) const;

  //  Operation: operator==
  //! Parameter:
  //! @param acrc_rightSide: Reference to the compared instance
  inline bool operator==(const SchedulerEntry_c& acrc_rightSide) const;

  //  Operation: isTask
  //!  this function compares the stored task object pointer to a given pointer.
  //!  This is used for scheduler queue find operations
  //!  (e.g. erase item with pointer to specific Task)
  //! Parameter:
  //! @param apc_cmpTask: pointer to task object, which should be compared
  inline bool isTask(const Scheduler_Task_c* apc_cmpTask) const;

  //  Operation: getTimeToNextTrigger
  //!  deliver the time [msec] to the next awaited retrigger of this task. The central scheduler can use the parameter to deduce the possible time
  //!  intervall, which is important, if some tasks have time problems due to high load (data processing, IsoAgLib).
  //! Parameter:
  //! @param t_retriggerType: Bit-OR combination of [earliest|standard|latest]
  inline int32_t getTimeToNextTrigger(retriggerType_t t_retriggerType= StandardRetrigger) const;

  //  Operation: getExecTime
  //!  deliver the average execution time for timeEvent calls -> allows scheduler to refine time schedule within execution
  //!  @return average execution time in [msec] (off complete performed runs)
  inline uint16_t getExecTime() const;

  //  Operation: timeEventExec
  //!  This function is called by the central scheduler to stimulate periodic activities for each task.
  //!  The parameter tells the task the available time for execution.
  //! @param ai32_demandedExecEnd available execution time. functon timeEvent() of Scheduler_Task_c Childs
  //!                              MUST be finished before the time, to avoid scheduling problems.
  //!                              default value -1 == unrestricted time for execution.
  //! @return true if client could finish his job else false
  bool timeEventExec(int32_t ai32_demandedExecEnd= -1);

  //  Operation: operator=
  //!  Assign pointer to task from source item to this item.
  //! Parameter:
  //! @param acrc_rightSide: Reference to the right side for the lower-comparison
  inline SchedulerEntry_c& operator=(const SchedulerEntry_c& acrc_rightSide);

  //  Operation: operator<
  //! Compare retriger time of this item by operator< with parameter
  //! @return true -> this item must be triggered first  
  //! Parameter:
  //! @param acrc_rightSide: Reference to the right side for the lower-comparison
  inline bool operator<(const SchedulerEntry_c& acrc_rightSide) const;

#if DEBUG_SCHEDULER
  inline const char* getTaskName() const;
#endif

  //  Operation: getTimePeriod
  inline uint16_t getTimePeriod() const;

  //  Operation: getAvgExecTime
  //!  this function is used for debug tests to get the AVG of exec time. This function uses the debug vars mui32_sumTime, mui32_callCnt
  //!  @return AVG of exec time
  inline uint16_t getAvgExecTime() const;

  //! virtual function which allows a scheduler client to define
  //! a minimum execution time, that should be saved after this item in the
  //! scheduler loop - some tasks might not be able to finish any sensible
  //! work in the default min exec time of 5msec
  inline uint16_t getForcedMinExecTime() const;

#if DEBUG_SCHEDULER
  //  Operation: getAvgTimingAccuracy
  inline int16_t getAvgTimingAccuracy() const;

  //  Operation: getMaxExecTime
  //!  deliver the max exec time of this task (used only for debug based on mui16_maxTime)
  inline uint16_t getMaxExecTime() const;

  //  Operation: getMaxTimingAccuracy
  //!  deliver MAX time between actual and next planned execution
  //!  (> 0 -> better than planned, < 0 -> worse than planned)
  //!  (only needed for debug tests)
  inline int16_t getMaxTimingAccuracy() const;

  //  Operation: getMinExecTime
  //!  deliver the min exec time of this task (used only for debug based on mui16_minTime)
  inline uint16_t getMinExecTime() const;

  //  Operation: getMinTimingAccuracy
  //!  deliver MIN time between actual and next planned execution
  //!  (> 0 -> better than planned, < 0 -> worse than planned)
  //!  (only needed for debug tests)
  inline int16_t getMinTimingAccuracy() const;
#endif
  //  Operation: getStdTimeToNextTrigger
  //!  deliver standard time till next retrigger (used for comparisong operators in priority queue of SystemManagement_c -> must be very quick as very often called)
  inline int32_t getStdTimeToNextTrigger() const;

  //  Operation: getNextTriggerTime
  //!  deliver timestamp of next planned retrigger (timeEvent() call)
  inline int32_t getNextTriggerTime() const;

  //!  Allows Scheduler_c to set new TimePeriod
  inline bool setTimePeriod(uint16_t aui16_timePeriod) const;

  //!  Allows Scheduler_c to change next retriggerTime
  inline bool changeNextTriggerTime(int32_t ai32_nextRetriggerTime);

protected:

private:

  //  Attribute: mpc_taskInstance
  Scheduler_Task_c* mpc_taskInstance;

};

// //////////////////////////////// +X2C Operation 2692 : SchedulerEntry_c
//!  Constructor which takes a pointer to the managed from Scheduler_Task_c derived class.
//! Parameter:
//! @param apc_entry: Pointer to the instance of the represented Scheduler_Task_c derived instance.
inline

SchedulerEntry_c::SchedulerEntry_c(Scheduler_Task_c* apc_entry)
: mpc_taskInstance( apc_entry )
{
}

//!  Compare retriger time of this item by operator<= with parameter
//!  @return true -> this item should be triggered first (retrigger times can ba equal)
//! Parameter:
//! @param acrc_rightSide: Reference to the right side for the lower-comparison
inline
bool
SchedulerEntry_c::operator<=(const SchedulerEntry_c& acrc_rightSide) const
{
  return ( mpc_taskInstance->getNextTriggerTime() <= acrc_rightSide.mpc_taskInstance->getNextTriggerTime() );
}

//! Parameter:
//! @param acrc_rightSide: Reference to the compared instance
inline
bool
SchedulerEntry_c::operator==(const SchedulerEntry_c& acrc_rightSide) const
{
  return ( mpc_taskInstance->getNextTriggerTime() == acrc_rightSide.mpc_taskInstance->getNextTriggerTime() );
}


//!  this function compares the stored task object pointer to a given pointer.
//!  This is used for scheduler queue find operations
//!  (e.g. erase item with pointer to specific Task)
//! Parameter:
//! @param apc_cmpTask: pointer to task object, which should be compared
inline
bool
SchedulerEntry_c::isTask(const Scheduler_Task_c* apc_cmpTask) const
{
  return ( mpc_taskInstance == apc_cmpTask );
}

//!  deliver the time [msec] to the next awaited retrigger of this task. The central scheduler can use the parameter to deduce the possible time
//!  intervall, which is important, if some tasks have time problems due to high load (data processing, IsoAgLib).
//! Parameter:
//! @param t_retriggerType: Bit-OR combination of [earliest|standard|latest]
inline
int32_t
SchedulerEntry_c::getTimeToNextTrigger(retriggerType_t t_retriggerType) const
{
  if ( mpc_taskInstance == NULL ) return 0xFFFF;
  else return mpc_taskInstance->getTimeToNextTrigger( t_retriggerType );
}

//!  deliver the average execution time for timeEvent calls -> allows scheduler to refine time schedule within execution
//!  @return average execution time in [msec] (off complete performed runs)
inline
uint16_t
SchedulerEntry_c::getExecTime() const
{
  if ( mpc_taskInstance == NULL ) return 0xFFFF;
  else return mpc_taskInstance->getExecTime();
}

//!  Assign pointer to task from source item to this item.
//! Parameter:
//! @param acrc_rightSide: Reference to the right side for the lower-comparison
inline
SchedulerEntry_c&
SchedulerEntry_c::operator=(const SchedulerEntry_c& acrc_rightSide)
{
  mpc_taskInstance = acrc_rightSide.mpc_taskInstance;
  return *this;
}

//! Compare retriger time of this item by operator< with parameter
//! @return true -> this item must be triggered first
//! @param acrc_rightSide Reference to the right side for the lower-comparison
inline
bool
SchedulerEntry_c::operator<(const SchedulerEntry_c& acrc_rightSide) const
{
  return ( mpc_taskInstance->getNextTriggerTime() < acrc_rightSide.mpc_taskInstance->getNextTriggerTime() );
}


#if DEBUG_SCHEDULER
inline
const char*
SchedulerEntry_c::getTaskName() const
{
  if ( mpc_taskInstance == NULL ) return "NULL";
  else return mpc_taskInstance->getTaskName();
}
#endif


inline
uint16_t
SchedulerEntry_c::getTimePeriod() const
{
  if ( mpc_taskInstance == NULL ) return 0xFFFF;
  else return mpc_taskInstance->getTimePeriod();
}

//!  this function is used for debug tests to get the AVG of exec time. This function uses the debug vars mui32_sumTime, mui32_callCnt
//!  @return AVG of exec time
inline
uint16_t
SchedulerEntry_c::getAvgExecTime() const
{
  if ( mpc_taskInstance == NULL ) return 0;
  else return mpc_taskInstance->getAvgExecTime();
}

//! virtual function which allows a scheduler client to define
//! a minimum execution time, that should be saved after this item in the
//! scheduler loop - some tasks might not be able to finish any sensible
//! work in the default min exec time of 5msec
inline uint16_t SchedulerEntry_c::getForcedMinExecTime() const
{
  if ( mpc_taskInstance == NULL ) return 0;
  else return mpc_taskInstance->getForcedMinExecTime();
}

#if DEBUG_SCHEDULER
inline
int16_t
SchedulerEntry_c::getAvgTimingAccuracy() const
{
  if ( mpc_taskInstance == NULL ) return 0;
  else return mpc_taskInstance->getAvgTimingAccuracy();
}

//!  deliver the max exec time of this task (used only for debug based on mui16_maxTime)
inline
uint16_t
SchedulerEntry_c::getMaxExecTime() const
{
  if ( mpc_taskInstance == NULL ) return 0;
  else return mpc_taskInstance->getMaxExecTime();
}

// //////////////////////////////// +X2C Operation 4145 : getMaxTimingAccuracy
//!  deliver MAX time between actual and next planned execution
//!  (> 0 -> better than planned, < 0 -> worse than planned)
//!  (only needed for debug tests)
inline
int16_t
SchedulerEntry_c::getMaxTimingAccuracy() const
{
  if ( mpc_taskInstance == NULL ) return 0;
  else return mpc_taskInstance->getMaxTimingAccuracy();
}

// //////////////////////////////// +X2C Operation 4146 : getMinExecTime
//!  deliver the min exec time of this task (used only for debug based on mui16_minTime)
inline
uint16_t
SchedulerEntry_c::getMinExecTime() const
{
  if ( mpc_taskInstance == NULL ) return 0;
  else return mpc_taskInstance->getMinExecTime();
}

// //////////////////////////////// +X2C Operation 4147 : getMinTimingAccuracy
//!  deliver MIN time between actual and next planned execution
//!  (> 0 -> better than planned, < 0 -> worse than planned)
//!  (only needed for debug tests)
inline
int16_t
SchedulerEntry_c::getMinTimingAccuracy() const
{
  if ( mpc_taskInstance == NULL ) return 0;
  else return mpc_taskInstance->getMinTimingAccuracy();
}
#endif
// //////////////////////////////// +X2C Operation 4184 : getStdTimeToNextTrigger
//!  deliver standard time till next retrigger (used for comparisong operators in priority queue of SystemManagement_c -> must be very quick as very often called)
inline
int32_t
SchedulerEntry_c::getStdTimeToNextTrigger() const
{
  if ( mpc_taskInstance == NULL ) return 0;
  else return mpc_taskInstance->getStdTimeToNextTrigger();
}

//!  deliver timestamp of next planned retrigger (timeEvent() call)
inline
int32_t
SchedulerEntry_c::getNextTriggerTime() const
{
  if ( mpc_taskInstance == NULL ) return 0;
  else return mpc_taskInstance->getNextTriggerTime();
}

//! Allows Scheduler_c to set new TimePeriod
//! @param aui16_timePeriod
inline
bool
SchedulerEntry_c::setTimePeriod(uint16_t aui16_timePeriod) const
{
  if ( mpc_taskInstance == NULL ) return false;
  mpc_taskInstance->setTimePeriod(aui16_timePeriod);
  return true;
}

//!  Allows Scheduler_c to change next retriggerTime
//! @param ai32_nextRetriggerTime
//!  @return bool -> change was done
inline
bool
SchedulerEntry_c::changeNextTriggerTime(int32_t ai32_nextRetriggerTime)
{
  if ( mpc_taskInstance == NULL ) return false;
  mpc_taskInstance->changeNextTriggerTime(ai32_nextRetriggerTime);
  return true;
}


} /// End Namespace __IsoAgLib

#endif
