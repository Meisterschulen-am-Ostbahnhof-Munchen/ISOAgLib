// ----------------------------------------------------------------------
// File    : SystemManagement/schedulerentry_c.h

/***************************************************************************
                          schedulerentry_c  -  header for Wrapper Class for
                      schedulertask_c objects that are managed in Scheduler_c
                             -------------------
    begin                : Thu Jul 29 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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


#ifndef SYSTEMMANAGEMENT_SCHEDULERENTRY_C_H
#define SYSTEMMANAGEMENT_SCHEDULERENTRY_C_H


#include <IsoAgLib/comm/Scheduler/impl/schedulertask_c.h>


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
  //! @param arc_rightSide: Reference to the right side for the lower-comparison
  inline bool operator<=(const SchedulerEntry_c& arc_rightSide) const;

  //  Operation: operator==
  //! Parameter:
  //! @param arc_rightSide: Reference to the compared instance
  inline bool operator==(const SchedulerEntry_c& arc_rightSide) const;

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
  //! @param ai32_demandedExecEnd: available execution time. functon timeEvent() of Scheduler_Task_c Childs
  //!                              MUST be finished before the time, to avoid scheduling problems.
  //!                              default value -1 == unrestricted time for execution.
  //! @return true if client could finish his job else false
  bool timeEventExec(int32_t ai32_demandedExecEnd= -1);

  //  Operation: operator=
  //!  Assign pointer to task from source item to this item.
  //! Parameter:
  //! @param arc_rightSide: Reference to the right side for the lower-comparison
  inline SchedulerEntry_c& operator=(const SchedulerEntry_c& arc_rightSide);

  //  Operation: operator<
  //!  Compare retriger time of this item by operator< with parameter
  //!  @return true -> this item must be triggered first  //! @param ai32_demandedExecEnd: available execution time. timeEvent() MUST be finished before the time, to avoid scheduling problems.
  //!                              default value -1 == unrestricted time for execution.
  //! Parameter:
  //! @param arc_rightSide: Reference to the right side for the lower-comparison
  inline bool operator<(const SchedulerEntry_c& arc_rightSide) const;

  //  Operation: getTaskName
  inline const char* getTaskName() const;

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

#ifdef DEBUG_SCHEDULER
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

  void close() { if (NULL != mpc_taskInstance) mpc_taskInstance->close();}

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
//! @param arc_rightSide: Reference to the right side for the lower-comparison
inline
bool
SchedulerEntry_c::operator<=(const SchedulerEntry_c& arc_rightSide) const
{
  return ( mpc_taskInstance->getNextTriggerTime() <= arc_rightSide.mpc_taskInstance->getNextTriggerTime() )?true:false;
}

//! Parameter:
//! @param arc_rightSide: Reference to the compared instance
inline
bool
SchedulerEntry_c::operator==(const SchedulerEntry_c& arc_rightSide) const
{
  return ( mpc_taskInstance->getNextTriggerTime() == arc_rightSide.mpc_taskInstance->getNextTriggerTime() )?true:false;
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
  return ( mpc_taskInstance == apc_cmpTask )?true:false;
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
//! @param arc_rightSide: Reference to the right side for the lower-comparison
inline
SchedulerEntry_c&
SchedulerEntry_c::operator=(const SchedulerEntry_c& arc_rightSide)
{
  mpc_taskInstance = arc_rightSide.mpc_taskInstance;
  return *this;
}

//!  Compare retriger time of this item by operator< with parameter
//!  @return true -> this item must be triggered first
//! Parameter:
//! @param arc_rightSide: Reference to the right side for the lower-comparison
inline
bool
SchedulerEntry_c::operator<(const SchedulerEntry_c& arc_rightSide) const
{
  return ( mpc_taskInstance->getNextTriggerTime() < arc_rightSide.mpc_taskInstance->getNextTriggerTime() )?true:false;
}

//!  Return ClassName for Scheduler_c
//!  @return true -> this item must be triggered first
inline
const char*
SchedulerEntry_c::getTaskName() const
{
  if ( mpc_taskInstance == NULL ) return "NULL";
  else return mpc_taskInstance->getTaskName();
}



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

#ifdef DEBUG_SCHEDULER
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

//!  Allows Scheduler_c to set new TimePeriod
//! @param uint16_t aui16_timePeriod
inline
bool
SchedulerEntry_c::setTimePeriod(uint16_t aui16_timePeriod) const
{
  if ( mpc_taskInstance == NULL ) return false;
  mpc_taskInstance->setTimePeriod(aui16_timePeriod);
  return true;
}

//!  Allows Scheduler_c to change next retriggerTime
//! @param int32_t ai32_nextRetriggerTime
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
