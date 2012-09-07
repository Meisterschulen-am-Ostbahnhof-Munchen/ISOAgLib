/*
  iwatchdogtriggertask_c.h: scheduler task to trigger watchdog

  (C) Copyright 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/scheduler/ischedulertask_c.h>

namespace IsoAgLib {

  class iWatchdogTriggerTask_c : public iSchedulerTask_c {
    public:
      iWatchdogTriggerTask_c( int32_t period ) : iSchedulerTask_c( 0, period, true ) {}
    private:
      virtual void timeEvent();
  };


  void iWatchdogTriggerTask_c::timeEvent() {
    iSystem_c::triggerWd();
  }

}

//eof
