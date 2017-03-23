#include <assert.h>

#include "ThreadWrapper_pthread.h"

#ifdef USE_MUTUAL_EXCLUSION

#ifdef API_WINCE
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace HAL
{

    ThreadWrapper::ThreadWrapper()
        : m_exit_code(0)
        , m_request_to_stop(false)
        , m_thread_handle_valid(false)
#ifdef WINCE
#else
        , m_thread_handle() // use default constructor (for Linux: long unsigned int default constructor does nothing
          // => not yet initialized but not used anyway
#endif
    {}

    ThreadWrapper::~ThreadWrapper() {}

    bool ThreadWrapper::Start()
    {
        m_request_to_stop = false;
        m_exit_code = 0;

#ifdef WINCE
        mthread_core = CreateThread(NULL, 0, ThreadRouter, this, 0, NULL);
        assert(NULL != mthread_core);
#else
        
        pthread_attr_t thread_attr;
        pthread_attr_init(&thread_attr);

        // attribute PTHREAD_CREATE_JOINABLE is set
        // => thread must be joined to release resources
        // => StopAndJoin() must be called by parent class
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
        int create_ret_val = pthread_create(&m_thread_handle, &thread_attr, ThreadRouter, reinterpret_cast<void*>(this));
        assert(create_ret_val == 0);
#endif
          
		m_thread_handle_valid = true;

#ifdef WINCE
		return (NULL != mthread_core);
#else
		return (create_ret_val == 0);
#endif
    }

    bool ThreadWrapper::StopAndJoin()
    {
        Stop();
        return Join();
    }

    void ThreadWrapper::Stop()
    {
        m_request_to_stop = true;
    }

    bool ThreadWrapper::Join()
    {
        if(!m_thread_handle_valid)
        {
            // never join a not yet started thread,
            // m_thread is not yet a valid thread handle
            return true;
        }

#ifdef WINCE
        const bool result = (WAIT_OBJECT_0 == WaitForSingleObject(mthread_core, INFINITE));
        CloseHandle(mthread_core);
#else
        const bool result (0 == pthread_join(m_thread_handle, NULL));
#endif

        m_thread_handle_valid = false;

        return result;
    }

#ifdef WINCE

    DWORD WINAPI ThreadWrapper::ThreadRouter(LPVOID thread_param)
    {
        reinterpret_cast<ThreadWrapper*>(thread_param)->ExecThread();

        ExitThread(0);

        return NULL;
    }

#else

    void* ThreadWrapper::ThreadRouter(void* arg)
    {
        reinterpret_cast<ThreadWrapper*>(arg)->ExecThread();

        pthread_exit(0);

        return NULL;
    }

#endif


    void ThreadWrapper::ExecThread()
    {
        m_exit_code = Exec();
    }

};

#endif
