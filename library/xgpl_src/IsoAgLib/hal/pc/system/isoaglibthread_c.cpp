/*
  isoaglibthreah_c.h: Singleton'd ISOAgLib Scheduler-main-loop thread
    This feature is only enabled if Mutual-Exclusion is enabled, too.

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifdef USE_MUTUAL_EXCLUSION

#include "isoaglibthread_c.h"

// ISOAgLib
#include <IsoAgLib/scheduler/ischeduler_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisomonitor_c.h>

// system (we know we're on PC!)
#include <assert.h>


namespace __IsoAgLib {


IsoAgLibThread_c::startResult
IsoAgLibThread_c::start (void *key, uint8_t aui8_busNr)
{
  // need to save that state before inserting the key!
  const bool cb_wasRunning = isRunning();

  const bool cb_wasNewlyInserted = mset_keys.insert (key).second;
  if (cb_wasNewlyInserted)
  { /// key was not yet registered, but is now
#ifdef DEBUG_THREAD
    std::cout << "IsoAgLibThread_c::start - Newly added key " << key << "." << std::endl;
#endif
    if (cb_wasRunning)
    { /// was already running before insertion
      if (aui8_busNr == __IsoAgLib::getCanInstance().getBusNumber())
      { // same setup
        return startSuccess;
      }
      else
      { // different setup
        return startFailAlreadyStartedWithDifferentSettings;
      }
    }
    else
    { /// was not yet running (before insertion)
      // If we do the actions before the thread is running,
      // we don't need to mutex :-)

#ifdef DEBUG_THREAD
      std::cout << "IsoAgLibThread_c::start - Init CAN & Start THREAD for key " << key << "." << std::endl;
#endif

      // @todo NOTE that a REstart is not yet possible, because
      //       close() does not really enable a REstart!
      IsoAgLib::getISchedulerInstance().startSystem();
      IsoAgLib::getIcanInstance().init (aui8_busNr, 250);

      mb_requestThreadToStop = false;
      pthread_create (&mthread_core, NULL, thread_core, (void *)this);

      return startSuccess;
    }
  }
  else
  { /// key already registered
    return startNoActionAlreadyRunningForThisKey;
  }
}


IsoAgLibThread_c::stopResult
IsoAgLibThread_c::stop (void *key)
{
  // erase and check if it was erased or not
  if (mset_keys.erase (key) == 0)
  { /// was not started for this key at all
    return stopFailNotStartedForThisKey;
  }
  else
  { /// was running for this key - key is now already erased
    if (!isRunning())
    { /// No more keys -> stop thread then of course, too!
#ifdef DEBUG_THREAD
      std::cout << "IsoAgLibThread_c::stop - Stop THREAD for key " << key << "." << std::endl;
#endif
      mb_requestThreadToStop = true;
      pthread_join (mthread_core, NULL);
      // Thread is not running now anymore...
      IsoAgLib::getISchedulerInstance().close();
      // Last one closed the door.
    }
    else
    { // still some other key(s) registered, keep thread running
#ifdef DEBUG_THREAD
      std::cout << "IsoAgLibThread_c::stop - Thread still running, only removed key " << key << "." << std::endl;
#endif
    }
    return stopSuccess;
  }
}


void *
IsoAgLibThread_c::thread_core (void* thread_param)
{
  IsoAgLibThread_c &thisThread = *(static_cast<IsoAgLibThread_c *>(thread_param));

  while (!thisThread.mb_requestThreadToStop)
  {
    int32_t i32_sleepTime = IsoAgLib::getISchedulerInstance().timeEventWithWaitMutex( (HAL::getTime()+3) );

    if (i32_sleepTime > 0)
    { // We can sleep...
      if (i32_sleepTime > 3)
      { /// @todo TIMING As long as the GUI thread won't wake up the "waitUntilCanReceiveOrTimeout()"-call below, lets simply limit the sleep time
        i32_sleepTime = 3;
      }

      // Now do sleep...
      IsoAgLib::iCanIo_c::waitUntilCanReceiveOrTimeout (i32_sleepTime);
    }
  }

  return NULL;
}

}

#endif

