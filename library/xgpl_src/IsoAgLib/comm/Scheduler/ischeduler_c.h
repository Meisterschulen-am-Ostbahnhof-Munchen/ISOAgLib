/***************************************************************************
                          ischeduler_c.h  -  header for central hardware independent
                                    object which manages all functional
                                    IsoAgLib member objects
                             -------------------
    begin                : Thu Jul 29 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
#ifndef ISCHEDULER_H
#define ISCHEDULER_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/scheduler_c.h"

/// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  Central class for all other IsoAgLib classes.
  Only define one instance of iScheduler_c per
  thread, and the other classes are
  instanziated as members of this iScheduler_c class.
  The member objects represent the different scopes of functions of IsoAgLib.
  @author Dipl.-Inform. Achim Spangler
  @short central manager object for all hardware independent IsoAgLib objects.
*/
class iScheduler_c : private __IsoAgLib::Scheduler_c {
public:
  /** initialisation for the central IsoAgLib object */
  void init() { Scheduler_c::init(); }

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close() { Scheduler_c::close(); }

  /** simply close communicating clients */
  void closeCommunication() { Scheduler_c::closeCommunication(); }

  void startSystem() { Scheduler_c::startSystem(); }


/** Return values of the timeEvent function */
  typedef enum TimeEventRetValue_en {
      resourceLocked = -100,
      jobNotFinished = -1
    } TimeEventRetValue_t;

  /**
    call the timeEvent for CanIo_c and all communication classes (derived from Scheduler_Task_c) which
    registered within iScheduler_c for periodic timeEvent.
    Define common trigger timestamp, so that distributed activities can be performed with
    common time base.
    @param ai32_demandedExecEndScheduler optional timestamp, where timeEvent shall return execution to calling function
           -> allow tight integration of IsoAgLib into application specific scheduler, as In-Time execution is
           guaranteed (default -1 -> no execution stop defined)
  @return idleTime for main application (> 0 wait for next call; == 0 call function again)
          idleTime == -1 One Client could not finish his Job
          idleTime == -100 only in case of USE_MUTUAL_EXCLUSION, it means that the Mutex was already locked and the resource cannot
          be acquired.
  */
  int32_t timeEvent( int32_t ai32_demandedExecEndScheduler = -1) {
            #ifdef USE_MUTUAL_EXCLUSION
            /** Try to acquire the Resource */
              int i_ret =  Scheduler_c::tryAcquireResource();
              if ( i_ret != 0 ) return resourceLocked; //! the resource is already locked, return.

              int32_t i32_idleTimeSpread = Scheduler_c::timeEvent( ai32_demandedExecEndScheduler );
              Scheduler_c::releaseResource();
              return i32_idleTimeSpread;
            #else
              return Scheduler_c::timeEvent( ai32_demandedExecEndScheduler );
            #endif
              };


#ifdef USE_MUTUAL_EXCLUSION
/**
    Lock the resource TimeEvent and call it for CanIo_c
    @param i32_demandedExecEndScheduler optional timestamp, where timeEvent shall return execution to calling function
           -> allow tight integration of IsoAgLib into application specific scheduler, as In-Time execution is
           guaranteed (default -1 -> no execution stop defined)
  @return idleTime for main application (> 0 wait for next call; == 0 call function again)
          idleTime == -1 One Client could not finish his Job
  */
  int32_t timeEventWithWaitMutex(int32_t ai32_demandedExecEndScheduler = -1)
      {
        Scheduler_c::waitAcquireResource();
        int32_t i32_idleTimeSpread = Scheduler_c::timeEvent(ai32_demandedExecEndScheduler);
        Scheduler_c::releaseResource();
        return i32_idleTimeSpread;
      }




  int releaseResource() { return Scheduler_c::releaseResource();}

  int tryAcquireResource() {return Scheduler_c::tryAcquireResource();}

  int waitAcquireResource(){return Scheduler_c::waitAcquireResource(); }

#endif



/**
    * deliver the average execution time for timeEvent calls -> allows scheduler to
    * refine time schedule within execution
    * @return average execution time in [msec] (off complete performed runs)
    */
  uint16_t getExecTime( void ) const { return Scheduler_c::getExecTime();}

  /**
    * if a very imprtant IRQ event forces stop of iScheduler_c::timeEvent AS SOON AS POSSIBLE
    * the IRQ handler can call iScheduler_c::forceExecStop().
    * Therefore the only smallest set of atomic activities is performed, even if timeEvent
    * is WITHIN execution in the main task. This way, the IsoAgLib is leaved by iScheduler_c::timeEvent()
    * in a guaranteed WELL DEFINED and VALID state.
    */
  void forceExecStop( void ) {Scheduler_c::forceExecStop();}

 private:
  /** allow getISchedulerInstance() access to shielded base class.
      otherwise __IsoAgLib::getSchedulerInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iScheduler_c& getISchedulerInstance( uint8_t aui8_instance );
  #else
  friend iScheduler_c& getISchedulerInstance( void );
  #endif
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique iScheduler_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iScheduler_c& getISchedulerInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iScheduler_c&>(__IsoAgLib::getSchedulerInstance( aui8_instance ) );}
#else
  /** C-style function, to get access to the unique iScheduler_c singleton instance */
  inline iScheduler_c& getISchedulerInstance( void ) { return static_cast<iScheduler_c&>(__IsoAgLib::getSchedulerInstance() );}

#endif

}

#endif
