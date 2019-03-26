#ifndef THREAD_WRAPPER_H
#define THREAD_WRAPPER_H

/* ThreadWrapper
 
  Helper base class to start and to force a thread stop
 
  usage:
  - The derived class has to implement the Exec() method
  - In Start() the static method ThreadRouter() is used as thread entry function
    This thread entry function calls ExecThread() which finally calls Exec() in the derived class
    and stores the result of Exec()
  - In StopAndJoin(), the flag m_request_to_stop and the thread is joined.
    The derived Exec() method should regularily check via GetRequestToStop() and terminate, if needed
  - The exit code of Exec() can be checked via GetExitCode()
  - Any Start() call must have a corresponding StopAndJoin() to release the thread resources,
    independently whether the thread has already finished or not.

  (C) Copyright 2009 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/isoaglib_config.h>

#ifdef USE_MUTUAL_EXCLUSION

#include "mutex_pthread.h"

namespace HAL
{
    class ThreadWrapper
    {
        public:
            ThreadWrapper();
            virtual ~ThreadWrapper();

            bool Start();
            // must be called to release thread resources (also if thread is already finished)
            bool StopAndJoin();
            // use separate methods for IsoAgLibWorkerThread
            void Stop();
            bool Join();

            virtual int Exec() = 0;

            int GetExitCode() const
            {
                m_exclusive_access.waitAcquireAccess();
                const int exit_code = m_exit_code;
                m_exclusive_access.releaseAccess();
                return exit_code;
            }

        protected:

            bool GetRequestToStop() const
            {
                return m_request_to_stop;
            }

        private:

#ifdef WINCE
            static DWORD WINAPI ThreadRouter(LPVOID thread_param);
#else
            static void* ThreadRouter(void* arg);
#endif
            
            void ExecThread();

            mutable ExclusiveAccess_c m_exclusive_access;

            int m_exit_code;
            bool m_request_to_stop;
            bool m_thread_handle_valid;
#ifdef WINCE
            HANDLE mthread_core;
#else
            pthread_t m_thread_handle;
#endif
    };
};
#endif

#endif
