/***************************************************************************
                          mutex_pthread.h - definition of Hardware Abstraction
                                     Layer for central system functions
                                     for debug/simulation
                                     system on PC
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
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
                            #if DEBUG
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

