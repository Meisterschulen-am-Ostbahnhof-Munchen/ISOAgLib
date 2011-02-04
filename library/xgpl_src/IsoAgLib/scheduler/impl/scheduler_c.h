/*
  scheduler_c.h: header for central hardware independent object which
    manages all functional IsoAgLib member objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SCHEDULER_H
#define SCHEDULER_H


namespace IsoAgLib
{ // forward declarations
  class iErrorObserver_c;
}

// IsoAgLib
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/hal/hal_system.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/scheduler/impl/schedulerentry_c.h>

// STL
#include <list>


//#define DEBUG_SCHEDULER 1
//#define DEBUG_SCHEDULER_EXTREME 1


/// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  Central class for all other IsoAgLib classes.
  Only define one instance of Scheduler_c per
  thread, and the other classes are
  instanziated as members of this Scheduler_c class.
  The member objects represent the different scopes of functions of IsoAgLib.
  @author Dipl.-Inform. Achim Spangler
  @short central manager object for all hardware independent IsoAgLib objects.
*/
class Scheduler_c : public Subsystem_c
{
  MACRO_SINGLETON_CONTRIBUTION();
public:

  /** Initialisation for the central IsoAgLib object
      @param apc_observer Optional pointer to an error-handler
      @return true if Scheduler/System could be initialized/opened
   */
  bool init( IsoAgLib::iErrorObserver_c *apc_observer = NULL );

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close();

  ~Scheduler_c() {}


  /**
    call the timeEvent for CanIo_c and all communication classes (derived from Scheduler_Task_c) which
    registered within Scheduler_c for periodic timeEvent.
    Define common trigger timestamp, so that distributed activities can be performed with
    common time base.
    @param ai32_demandedExecEndScheduler optional timestamp, where timeEvent shall return execution to calling function
           -> allow tight integration of IsoAgLib into application specific scheduler, as In-Time execution is
           guaranteed (default -1 -> no execution stop defined)
  @return idleTime for main application (> 0 wait for next call; == 0 call function again)
          idleTime == -1 One Client could not finish his Job
  */
  int32_t timeEvent( int32_t ai32_demandedExecEndScheduler = -1);

  /** wait until specified timeout or until next CAN message receive
   *  @return true -> there are CAN messages waiting for process. else: return due to timeout
   */
  bool waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval )
  { return CanIo_c::waitUntilCanReceiveOrTimeout( aui16_timeoutInterval ); }


  /** deliver the global execution time event for the central IsoAgLib Scheduler_c.
      This end time is defined by the application which calls Scheduler_c::timeEvent().
      This is different from the individual end time, which has to be regarded by each
      individual task!
    */
  int32_t getCentralSchedulerExecEndTime() { return mi32_demandedExecEndScheduler;}
  int16_t getCentralSchedulerAvailableExecTime();


  /**
    * deliver the average execution time for timeEvent calls -> allows scheduler to
    * refine time schedule within execution
    * @return average execution time in [msec] (off complete performed runs)
    */
  int32_t getExecTime( void ) const { return mi32_averageExecTime;}

  /**
    * if a very imprtant IRQ event forces stop of Scheduler_c::timeEvent AS SOON AS POSSIBLE
    * the IRQ handler can call Scheduler_c::forceExecStop().
    * Therefore the only smallest set of atomic activities is performed, even if timeEvent
    * is WITHIN execution in the main task. This way, the IsoAgLib is leaved by Scheduler_c::timeEvent()
    * in a guaranteed WELL DEFINED and VALID state.
    */
  void forceExecStop( void ) { mb_execStopForced = true; mi32_demandedExecEndScheduler = 0; }

#ifdef USE_MUTUAL_EXCLUSION
  int releaseResource() {
                          #if DEBUG_SCHEDULER
                          INTERNAL_DEBUG_DEVICE << "Released " << INTERNAL_DEBUG_DEVICE_ENDL;
                          #endif
                          return mc_protectAccess.releaseAccess();}

  int tryAcquireResource() {
                #if DEBUG_SCHEDULER
                INTERNAL_DEBUG_DEVICE << "TRY to acquire timeEvent "  << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return mc_protectAccess.tryAcquireAccess();}

  //! Lock the resource to prevent other threads to use it. If the resource is already locked,
  //! the calling threads is in blocking state until the unlock.
  int waitAcquireResource(){
                             forceExecStop();
                             #if DEBUG_SCHEDULER
                              INTERNAL_DEBUG_DEVICE << "Wait Lock timeEvent  " << INTERNAL_DEBUG_DEVICE_ENDL;
                             #endif
                             return mc_protectAccess.waitAcquireAccess();}
#endif

  /**
    * informative function for all IsoAgLib subsystems which are triggered by Scheduler_c::timeEvent to
    * detect, if another task forced immediated stop of timeEvent
    * @return true -> immediate stop is forced
    */
  bool isExecStopForced( void ) { return mb_execStopForced; }

  /** get last timestamp of Scheduler_c::timeEvent trigger */
  int32_t getLastTimeEventTrigger( void ) { return mi32_lastTimeEventTime;}

  /** handler function for access to undefined client.
    * the base Singleton calls this function, if it detects an error
    */
  void registerAccessFlt( void );

  /** register pointer to a new client
    * this function is called within construction of new client instance
    */
  bool registerClient( Scheduler_Task_c* apc_client);
  size_t cntClient() const { return mt_clientCnt;}
  /** unregister pointer to a already registered client
    * this function is called within destruction of new client instance
    */
  void unregisterClient( Scheduler_Task_c* apc_client);

  //!  Calculate Delta from TimePeriod of a Client
  //!  Scheduler_c set (NOW + TimePeriod) as New Retrigger for Client
  //!  and sort Task to the right Position in the TaskQueue
  //! @param apc_client -> Client in Scheduler_c TaskQueue
  //! @param aui16_newTimePeriod -> New Period will set for the Client by Scheduler_c
  bool changeTimePeriodAndResortTask(Scheduler_Task_c * apc_client  , uint16_t aui16_newTimePeriod );


  //!  Uses Delta from TimePeriod of a Client
  //!  to put a Task to the right Position in the TaskQueue
  //!  ATTENTION parameter nextRetriggerTime will exactly used from Scheduler_c
  //!  for call of timevent.-> so add e.g. an TimePeriod for an later call
  //! @param apc_client -> Client in Scheduler_c TaskQueue
  //! @param i32_newRetriggerTime -> New i32_nextRetriggerTime set for Client by Scheduler_c
  //! @param ai16_newTimePeriod otpional -> New Period will set for the Client by Scheduler_c
  bool changeRetriggerTimeAndResort(Scheduler_Task_c * apc_client  , int32_t i32_newRetriggerTime, int16_t ai16_newTimePeriod = -1);

  //!  Uses Delta from TimePeriod of a Client
  //!  to put a Task to the right Position in the TaskQueue
  //!  ATTENTION parameter nextRetriggerTime will exactly used from Scheduler_c
  //!  for call of timevent.-> so add e.g. an TimePeriod for an later call
  //! @param ac_client -> Client in Scheduler_c TaskQueue
  //! @param i32_newRetriggerTime -> New i32_nextRetriggerTime set for Client by Scheduler_c
  //! @param ai16_newTimePeriod otpional -> New Period will set for the Client by Scheduler_c
  bool  changeRetriggerTimeAndResort(SchedulerEntry_c ac_client  , int32_t i32_newRetriggerTime, int16_t ai16_newTimePeriod = -1);


  /** The function checks whether the input Scheduler_task_c is in the timeEvent,
     in this case return true */
  bool isTaskExecuted(const Scheduler_Task_c* apc_client) const;
private: //Private methods
  /** constructor for the central IsoAgLib object */
  Scheduler_c();



#if DEBUG_SCHEDULER
  void TraceAndAbort();
#endif

  /** deliver available time for time event of SCHEDULER only
    * @param ai16_awaitedExecTime optional awaited execution time of planned next step
             ==> answer of this function tells, if planned step will fit into time frame
  */
  int16_t getAvailableExecTime( int16_t ai16_awaitedExecTime = 0 );

  //!  This function implements the main scheduler work defined in UC15 Scheduler. It selects the next task, executes it and updates the task-list.
  //!  If no task can be immediately started with EarliestRetrigger-option,
  //!  the time to earliest triggerable task
  //!  is given to setDebugIdleInformation.
  //!  Each In-Time call of a task is
  //!  given to setDebugTimeAccuracy,
  //!  to debug the time behaviour of the system.
  //!  @return idle time till next executable task ( 0 == this function should be called immediately again )
  int32_t selectCallTaskAndUpdateQueue();

  //!  resort from start of task list by swapping neighbour elements.
  //!  Stop execution, if compared elements are in correct order.
  //!  Avoid complex complete sort of list, if only the previously executed task must be placed in the correct position again - the rest of the list is still correct sorted.
  //!  @param apc_sort ptr to currently executed SchedulerEntry_c
  void resortTaskList(const SchedulerEntry_c* apc_sort);


  //!  Uses Delta from TimePeriod of a Client
  //!  to put a Task to the right Position in the TaskQueue
  //!  ATTENTION parameter nextRetriggerTime will exactly used from Scheduler_c
  //!  for call of timevent.-> so add e.g. an TimePeriod for an later call
  //! @param itc_task -> iterator to the task that should be changed
  //! @param i32_newRetriggerTime -> New i32_nextRetriggerTime set for Client by Scheduler_c
  //! @param  ai16_newTimePeriod otpional -> New Period will set for the Client by Scheduler_c
  bool  changeRetriggerTimeAndResort(STL_NAMESPACE::list<SchedulerEntry_c>::iterator itc_task, int32_t i32_newRetriggerTime, int16_t ai16_newTimePeriod = -1);

  void setCntClient( size_t at_newSize ) { mt_clientCnt = at_newSize;}

#if DEBUG_SCHEDULER
  //!  Send debug messages with information on the
  //!  acfuracy of time behaviour.
  //!  Retrieve information about actual executed task from referenced SchedulerEntry_c.
  //!  As long as time to next standard retrigger >= 0,
  //!  everything is O.K.
  //!  As long as latest execution would allow some idle time,
  //!  the timing is acceptable.
  //! Parameter:
  //! @param rc_selectedTask: reference to the next executed task
  void setDebugTimeAccuracy(SchedulerEntry_c& rc_selectedTask);


  //!  Send debug messages with information on the idletime
  //!  This is important to check whether the time
  //!  restrictions are fullfilled.
  //! Parameter:
  //! @param aui16_idleTime:
  void setDebugIdleInformation(uint16_t aui16_idleTime);

  //! Debug function to print current list of tasks in the scheduler with
  //! the single retrigger times
  void printTaskList();
#endif



private: // Private attributes

  /** registered Error-Observer. Stored to be able to
      automatically deregister at close(). */
  IsoAgLib::iErrorObserver_c *mpc_registeredErrorObserver;

 /** pointer to the currently executed task */
  SchedulerEntry_c * pc_currentlyExecutedTask;

  /** timestamp where last timeEvent was called -> can be used to synchronise distributed timeEvent activities */
  int32_t mi32_lastTimeEventTime;

  /** commanded timestamp, where Scheduler_c::timeEvent MUST return action to caller */
  int32_t mi32_demandedExecEndScheduler;

  /** average execution time for Scheduler_c::timeEvent */
  int32_t mi32_averageExecTime;

  /** execution time of last call of CanIo_c::timeEvent() */
  int16_t mi16_canExecTime;

  /** flag to detect, if other interrupting task forced immediated stop of Scheduler_c::timeEvent() */
  bool mb_execStopForced;

  /** constant for minimum time for CAN processing */
  static const int ci32_minCanProcessingTime = 10;

  //  Attribute: mc_taskQueue
  //!  central priority queue for all tasks
  STL_NAMESPACE::list<SchedulerEntry_c> mc_taskQueue;
  size_t mt_clientCnt;
  STL_NAMESPACE::list<SchedulerEntry_c> mc_spareQueue;

#ifdef USE_MUTUAL_EXCLUSION
  /** Attribute for the exclusive access of the IsoAgLib for threads */
  HAL::ExclusiveAccess_c mc_protectAccess;
#endif

  friend Scheduler_c &getSchedulerInstance();
};

  /** C-style function, to get access to the unique Scheduler_c singleton instance */
  Scheduler_c &getSchedulerInstance();

}

#endif
