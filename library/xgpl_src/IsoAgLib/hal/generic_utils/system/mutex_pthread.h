/*
  mutex_pthread.h: Wrapper class for MUTEXes.

  (C) Copyright 2009 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_MUTEX_PTHREAD_H_
#define _HAL_MUTEX_PTHREAD_H_

#include <IsoAgLib/isoaglib_config.h>


#ifdef USE_MUTUAL_EXCLUSION

#ifdef WINCE
  #include <Windows.h>
#else
  // Version >= Microsoft Visual Studio C++ 2015
  // fix broken pthread.h (that defines timespec on its own, which may result in double definition)
  #if _MSC_VER >= 1900
    #ifndef HAVE_STRUCT_TIMESPEC
      #define HAVE_STRUCT_TIMESPEC
    #endif
  #endif
  #include <pthread.h>
#endif

#include <assert.h>


namespace HAL
{


/**
 * Class wrapping for a MUTEX
 * @short Class wrapping a MUTEX.
*/
#ifdef WINCE
class ExclusiveAccess_c
{
    public:
        
        ExclusiveAccess_c(bool a_recursive_mutex = false)
        {
            // in Windows: "The thread that owns a mutex can specify the same mutex in repeated wait function calls without blocking its execution."
            // => a_recursive_mutex is not used
            (void)a_recursive_mutex;

            m_exclusiveAccess = CreateMutex(NULL,  // default security attributes
                false, // initially not owned
                NULL   // unnamed mutex
                );
            assert(NULL != m_exclusiveAccess);
        }

        ExclusiveAccess_c(const ExclusiveAccess_c& a_ref_exclusive_access)
        {
            // new instances needs a new mutex handle, otherwise CloseHandle would be called twice with the same HANDLE
            m_exclusiveAccess = CreateMutex(NULL,  // default security attributes
                false, // initially not owned
                NULL   // unnamed mutex
                );
            assert(NULL != m_exclusiveAccess);
        }

        ~ExclusiveAccess_c()
        {
            CloseHandle(m_exclusiveAccess);
        }

        int waitAcquireAccess() 
        { 
            if(WaitForSingleObject(m_exclusiveAccess, INFINITE) == WAIT_OBJECT_0)
                return 0;
            return -1;
        }

        int tryAcquireAccess()
        { 
            if(WaitForSingleObject(m_exclusiveAccess, 0) == WAIT_OBJECT_0)
                return 0;
            return -1;
        }
        
        int releaseAccess()
        {
            if(ReleaseMutex(m_exclusiveAccess))
                return 0;
            return -1;
        }
        
    private:
        
        // prevent assignment
        ExclusiveAccess_c& operator=(const ExclusiveAccess_c& /* ref_source */);
        
        HANDLE m_exclusiveAccess;    
};

#else


class ExclusiveAccess_c {
public:
  
  ExclusiveAccess_c(bool a_recursive_mutex = false)
  {
    pthread_mutexattr_t* p_attr = NULL;
    if(a_recursive_mutex)
    {
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
      p_attr = &attr;
    }
            
    const int i_retV = pthread_mutex_init( &m_exclusiveAccess, p_attr );
    ( void )i_retV; assert( i_retV == 0 );
  }

  
  ExclusiveAccess_c(const ExclusiveAccess_c& /* a_ref_exclusive_access */)
  {
    // new instances needs a new mutex handle, otherwise pthread_mutex_destroy would close the same HANDLE twice
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    // we don't get the recursive info from a_ref_exclusive_access
    // => the new mutex is not recursive
    // => might be a problem unter Linux but not under Windows (mutex is always recursive)
    // pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    const int i_retV = pthread_mutex_init( &m_exclusiveAccess, &attr );
    ( void )i_retV; assert( i_retV == 0 );
  }
  
  ~ExclusiveAccess_c()
  {
    pthread_mutex_destroy( &m_exclusiveAccess ); }
  
  int waitAcquireAccess() { return pthread_mutex_lock( &m_exclusiveAccess ); }
  int tryAcquireAccess() { return pthread_mutex_trylock( &m_exclusiveAccess ); }
  int releaseAccess() { return pthread_mutex_unlock( &m_exclusiveAccess ); }

private:
    
  // prevent assignment
  ExclusiveAccess_c& operator=(const ExclusiveAccess_c& /* ref_source */);
  
  pthread_mutex_t m_exclusiveAccess;
};

#endif

}

#endif

#endif

