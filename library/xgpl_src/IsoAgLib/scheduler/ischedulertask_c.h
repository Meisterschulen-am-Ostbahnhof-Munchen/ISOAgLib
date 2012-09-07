/*
  ischedulertask_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISCHEDULERTASK_H
#define ISCHEDULERTASK_H

#include "impl/schedulertask_c.h"

namespace IsoAgLib {

  class iSchedulerTask_c : private __IsoAgLib::SchedulerTask_c {
    public:
      iSchedulerTask_c( int32_t nextTriggerTime, int32_t period, bool hardTiming ) : SchedulerTask_c( nextTriggerTime, period, hardTiming ) {}
      virtual ~iSchedulerTask_c() {}

      /** This method has to be overridden so that IsoAgLib's scheduler
        * calls it periodically. (There's no need to register at the
        * scheduler, because this is done implicitly by iSchedulerTask_c's
        * constructor.)
        */
      virtual void timeEvent() = 0;


      int32_t getPeriod() const {
        return SchedulerTask_c::getPeriod();
      }

      /**  setTimePeriod
        * Set client specific time period between calls of timeEvent.
        * @param a_period: task period
        */
      void setPeriod ( int32_t a_period ) {
        SchedulerTask_c::setPeriod( a_period );
      }

      /**  deliver the time [msec] to the next awaited retrigger of this task.
        *  @return time in ms to the next retrigger
        */
      int32_t getTimeToNextTrigger() const {
        return SchedulerTask_c::getTimeToNextTrigger();
      }

      /** deliver the time [msec] the next awaited retrigger of this task.
        * @return time in ms when the next trigger will happen
        * */
      int32_t getNextTriggerTime() const {
        return SchedulerTask_c::getNextTriggerTime();
      }

      /** set abosolut time for next timeEvent call. This time is
       * take regardless of the current period.
       * */
      void setNextTriggerTime( int32_t time ) {
        SchedulerTask_c::setNextTriggerTime( time );
      }

    private:
      friend class iScheduler_c;
  };

}

#endif
