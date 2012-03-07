/*
  ischedulertask_c.cpp: header for central hardware independent object
    which manages all functional IsoAgLib member objects

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "ischedulertask_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>


/// Begin Namespace IsoAgLib
namespace IsoAgLib {


iSchedulerTask_c::iSchedulerTask_c()
{
  __IsoAgLib::getSchedulerInstance().registerClient(this);
}


iSchedulerTask_c:: ~iSchedulerTask_c()
{
  __IsoAgLib::getSchedulerInstance().unregisterClient(this);
}


//!  deliver the time [msec] to the next awaited retrigger of this task. The central scheduler can use the parameter to deduce the possible time
//!  interval, which is important, if some tasks have time problems due to high load (data processing, IsoAgLib).
//!  @return <0 -> too late, =0 -> trigger now, >0 -> wait
//! Parameter:
//! @param t_retriggerType: Bit-OR combination of [earliest|standard|latest]
int32_t iSchedulerTask_c::getTimeToNextTrigger(__IsoAgLib::retriggerType_t t_retriggerType) const
{
  return getTimeToNextTriggerDefault(t_retriggerType);
}


//! virtual function which allows a scheduler client to define
//! a minimum execution time, that should be saved after this item in the
//! scheduler loop - some tasks might not be able to finish any sensible
//! work in the default min exec time of 5msec
uint16_t iSchedulerTask_c::getForcedMinExecTime() const
{
  return getForcedMinExecTimeDefault();
}


//! Function set mui16_earlierInterval and
//! ui16_laterInterval that will be used by
//! getTimeToNextTrigger(retriggerType_t)
//! can be overloaded by Childclass for special condition
void iSchedulerTask_c::updateEarlierAndLatestInterval()
{
  return updateEarlierAndLatestIntervalDefault();
}


}
