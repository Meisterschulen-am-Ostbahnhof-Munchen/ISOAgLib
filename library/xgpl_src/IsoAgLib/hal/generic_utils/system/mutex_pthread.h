/*
  mutex_pthread.h:
    definition of Hardware Abstraction Layer for central system
    functions for debug/simulation system on PC

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_MUTEX_PTHREAD_H_
#define _HAL_MUTEX_PTHREAD_H_


#ifdef USE_MUTUAL_EXCLUSION
#include <pthread.h>


namespace HAL
{


/*
*   pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    * int pthread_mutex_init (pthread_mutex_t *mutex, pthread_mutexattr_t *attr);
    * int pthread_mutex_destroy (pthread_mutex_t *mutex);
    * int pthread_mutex_lock (pthread_mutex_t *mutex); - Used to lock a mutex. This function will block until the mutex is unlocked, if it is locked. Do not try to lock the mutex if the same thread already has locked the mutex.
    * int pthread_mutex_trylock (pthread_mutex_t *mutex); - Returns an error code if the mutex is already locked by another process. This function will not block.
    * int pthread_mutex_unlock (pthread_mutex_t *mutex);
*/
/**
 * Class wrapping for a MUTEX
 * @short Class wrapping a MUTEX.
*/

class ExclusiveAccess_c {
public:

  ExclusiveAccess_c() {// init the Mutex
                        int i_retV = pthread_mutex_init(&m_exclusiveAccess,NULL);
                        if(i_retV != 0)
                        {
                            #if DEBUG_MUTEX
                             INTERNAL_DEBUG_DEVICE << "Error on Mutex initialization" << INTERNAL_DEBUG_DEVICE_ENDL;
                            #endif
                        }
                      }

  ~ExclusiveAccess_c() {// destroy the Mutex
                            pthread_mutex_destroy(&m_exclusiveAccess); }

  int  waitAcquireAccess() { return pthread_mutex_lock(&m_exclusiveAccess);}

  int  tryAcquireAccess(){ return pthread_mutex_trylock(&m_exclusiveAccess);}

  int releaseAccess(){ return pthread_mutex_unlock(&m_exclusiveAccess);}


private:

   pthread_mutex_t m_exclusiveAccess;

};//end class ExclusiveAccess_c


}
#endif


#endif

