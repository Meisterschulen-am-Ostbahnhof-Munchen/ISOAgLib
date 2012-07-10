/*
  schedulerentry_c.cpp: body for central hardware independent object
    which manages all functional IsoAgLib member objects

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#if DEBUG_SCHEDULER
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif


#include <IsoAgLib/scheduler/impl/schedulerentry_c.h>
/// Begin Namespace __IsoAgLib

namespace __IsoAgLib {
//!  This function is called by the central scheduler to stimulate periodic activities for each task.
//!  The parameter tells the task the available time for execution.
//! @param ai32_demandedExecEnd: available execution time. timeEvent() MUST be finished before the time, to avoid scheduling problems.
//!                              default value -1 == unrestricted time for execution.
//!  @return  false-> Client could not finish job; true-> Client finish job correctly

//#define DEBUG_SCHEDULER

bool
SchedulerEntry_c::timeEventExec(int32_t ai32_demandedExecEnd)
{
  #if DEBUG_SCHEDULER
  if ( mpc_taskInstance == NULL )
  {
    #ifdef SYSTEM_PC
    INTERNAL_DEBUG_DEVICE << "SchedulerEntry_c::timeEvent() had no mpc_taskInstance."
    << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return true;
  }
  #endif
  mpc_taskInstance->timeEventPre(ai32_demandedExecEnd);
  const bool cb_result = mpc_taskInstance->timeEvent();
  /// call only if client returns true caused by  nextRetriggerTime
  /// should NOT be Updated in  timeEventPostUpdateStatistics()
  /// so Client keep old nextRetriggerTime and stay as FIRST in TaskQueue
  if (cb_result) mpc_taskInstance->timeEventPostUpdateStatistics();

#if DEBUG_SCHEDULER
  if (!cb_result) {
    if (mpc_taskInstance->getTaskName() == NULL)
      INTERNAL_DEBUG_DEVICE << "Unnamed SchedulerEntry's timeEvent() returned false." << INTERNAL_DEBUG_DEVICE_ENDL;
    else
      INTERNAL_DEBUG_DEVICE << mpc_taskInstance->getTaskName() << ".timeEvent() returned false." << INTERNAL_DEBUG_DEVICE_ENDL;
  }
#endif
  return cb_result;
}


}   /// End Namespace __IsoAgLib

