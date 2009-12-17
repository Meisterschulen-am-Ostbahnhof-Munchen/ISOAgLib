/*
  ischeduler_c.h: header for central hardware independent object which
    manages all functional IsoAgLib member objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISCHEDULER_H
#define ISCHEDULER_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/scheduler_c.h"
#include <IsoAgLib/scheduler/ischedulertask_c.h>

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>
#ifdef USE_PROCESS
  #include <IsoAgLib/comm/Part7_ProcessData/impl/process_c.h>
#endif
#ifdef USE_TRACTOR_GENERAL
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracgeneral_c.h>
#endif
#ifdef USE_TRACTOR_MOVE
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.h>
#endif
#ifdef USE_TRACTOR_PTO
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracpto_c.h>
#endif
#ifdef USE_TRACTOR_FACILITIES
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracfacilities_c.h>
#endif
#ifdef USE_TRACTOR_LIGHT
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/traclight_c.h>
#endif
#ifdef USE_TRACTOR_AUX
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracaux_c.h>
#endif
#ifdef USE_TIME_GPS
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/timeposgps_c.h>
#endif
#ifdef USE_ISO_TERMINAL
  #include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/isoterminal_c.h>
#endif
#ifdef DEF_Stream_IMPL
  #include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>
  #include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#endif

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
  int32_t getExecTime( void ) const { return Scheduler_c::getExecTime();}

  /**
    * if a very imprtant IRQ event forces stop of iScheduler_c::timeEvent AS SOON AS POSSIBLE
    * the IRQ handler can call iScheduler_c::forceExecStop().
    * Therefore the only smallest set of atomic activities is performed, even if timeEvent
    * is WITHIN execution in the main task. This way, the IsoAgLib is leaved by iScheduler_c::timeEvent()
    * in a guaranteed WELL DEFINED and VALID state.
    */
  void forceExecStop( void ) {Scheduler_c::forceExecStop();}

  /** register pointer to a new client
    * this function is called within construction of new client instance
  */
  bool registerClient( iSchedulerTask_c* apc_client)
    { return Scheduler_c::registerClient(apc_client); }

  /** unregister pointer to a already registered client
    * this function is called within destruction of new client instance
  */
  void unregisterClient( iSchedulerTask_c* apc_client)
    {  return Scheduler_c::unregisterClient(apc_client); }


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
