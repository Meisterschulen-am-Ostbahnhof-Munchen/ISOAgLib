/*
  scheduler_c.cpp: implementation for central hardware independent
    object which manages all functional IsoAgLib member objects


  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "scheduler_c.h"
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/driver/eeprom/impl/eepromio_c.h>
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/iassert.h>
#ifdef USE_ACTOR
  #include <supplementary_driver/driver/actor/impl/actoro_c.h>
#endif
#ifdef USE_SENSOR
  #include <supplementary_driver/driver/sensor/impl/sensori_c.h>
#endif

#if defined(USE_CAN_EEPROM_EDITOR) || defined( USE_RS232_EEPROM_EDITOR )
  #include <IsoAgLib/hal/hal_eeprom.h>
#endif

#if DEBUG_SCHEDULER || DEBUG_HEAP_USEAGE || defined(TEST_TIMING) || DEBUG_ELEMENTBASE	|| DEBUG_TIME_EVENTS || DEBUG_TASKS_QUEUE
  #include <IsoAgLib/util/impl/util_funcs.h>
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif

#if defined(TEST_TIMING) && defined(SYSTEM_PC)
  #include <ctime>
#endif


#if DEBUG_HEAP_USEAGE
static uint16_t volatile sui16_clientPointerTotal = 0;

#ifdef MASSERT
extern unsigned int AllocateHeapMalloc;
extern unsigned int DeallocateHeapMalloc;
#endif
#endif

/* ************************************** */
/* import some namespaces for easy access */
/* ************************************** */
namespace __IsoAgLib {
  /** C-style function, to get access to the unique Scheduler_c singleton instance */
  Scheduler_c& getSchedulerInstance( void )
  {
    static Scheduler_c& c_scheduler = Scheduler_c::instance();
    return c_scheduler;
  }


Scheduler_c::Scheduler_c()
  : mpc_registeredErrorObserver( NULL )
  , pc_currentlyExecutedTask( NULL )
  , mi32_lastTimeEventTime( 0 )
  , mi32_demandedExecEndScheduler( 0 )
  , mi32_averageExecTime( 0 )
  , mi16_canExecTime( 0 )
  , mb_execStopForced( false )
  , mc_taskQueue()
  , mt_clientCnt( 0 )
  , mc_spareQueue()
#ifdef USE_MUTUAL_EXCLUSION
  , mc_protectAccess()
#endif
  , mb_systemStarted( false )
{
  // nop
}


void
Scheduler_c::init( IsoAgLib::iErrorObserver_c *apc_observer )
{
  isoaglib_assert (!mb_systemStarted);
  if (!mb_systemStarted)
  {
    mb_systemStarted = true;

    getILibErrInstance().init();
    if (apc_observer != NULL)
    {
      const bool cb_observerRegistered
        = getILibErrInstance().registerObserver (*apc_observer);

      if (cb_observerRegistered)
        mpc_registeredErrorObserver = apc_observer;
    }

    getSystemInstance().init();

    #ifdef USE_EEPROM_IO
      getEepromInstance().init();
    #endif

    #ifdef USE_ACTOR
      getActorInstance().init();
    #endif

    #ifdef USE_SENSOR
      getSensorInstance().init();
    #endif
  }
  // else: system already started, so don't restart.
}


void
Scheduler_c::close( void )
{
  isoaglib_assert (mb_systemStarted);
  if (mb_systemStarted)
  {
    mb_systemStarted = false;

    #ifdef USE_SENSOR
      getSensorInstance().close();
    #endif

    #ifdef USE_ACTOR
      getActorInstance().close();
    #endif

    #ifdef USE_EEPROM_IO
      getEepromInstance().close();
    #endif

    getSystemInstance().close();

    if (mpc_registeredErrorObserver != NULL)
    {
      getILibErrInstance().deregisterObserver( *mpc_registeredErrorObserver );
      mpc_registeredErrorObserver = NULL;
    }

    getILibErrInstance().close();

    // Reset all member variables to initial state.
    pc_currentlyExecutedTask = NULL;
    mi32_lastTimeEventTime = 0;
    mi32_demandedExecEndScheduler = 0;
    mi32_averageExecTime = 0;
    mi16_canExecTime = 0;
    mb_execStopForced = false;

    isoaglib_assert (mt_clientCnt == 0);
    isoaglib_assert (mc_taskQueue.empty());
    isoaglib_assert (mc_spareQueue.empty());
    // mc_protectAccess() // nothing to reset here.
  }
  // else: system wasn't started, so nothing to close.
}


void
Scheduler_c::registerAccessFlt( void )
{
  getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Scheduler );
}


/** register pointer to a new client
  * this function is called within construction of new client instance
  */
bool Scheduler_c::registerClient( Scheduler_Task_c* apc_client)
{
  // first check whether this client is already registered
  for ( STL_NAMESPACE::list<SchedulerEntry_c>::const_iterator iter = mc_taskQueue.begin(); iter != mc_taskQueue.end(); iter++ )
  {
    if (iter->isTask( apc_client ) ) return false;
  }

  /// add 2ms to startTime of new Client to avoid crossing timestamps
  static int32_t si32_taskStartTime = 0;
  if ( si32_taskStartTime == 0 ) si32_taskStartTime = System_c::getTime() + 50;
  else
  {
    if ( ! mc_taskQueue.empty() ) si32_taskStartTime -= apc_client->getForcedMinExecTime();
    else si32_taskStartTime -= 2;
  }
  //For Client that registers at later timepoint
  if ( System_c::getTime() > si32_taskStartTime ) si32_taskStartTime = System_c::getTime();;

# if 0 // Dont't call virtual method getTaskName here, otherwise it
       // will be called indirectly by iSchedulerTask_c's construcor
       // when the vtable is not yet complete.
  #if DEBUG_SCHEDULER
  INTERNAL_DEBUG_DEVICE << "RegisteredClient: "<<  apc_client->getTaskName() <<  INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
# endif

  apc_client->startTaskTiming(si32_taskStartTime);
  /// put client in taskQueue
  mc_taskQueue.push_front( SchedulerEntry_c( apc_client ) );
  setCntClient( mc_taskQueue.size() );
  #if DEBUG_HEAP_USEAGE
  sui16_clientPointerTotal++;
  #endif

  return true;
}


/** unregister pointer to a already registered client
  * this function is called within destruction of new client instance
  */
void Scheduler_c::unregisterClient( Scheduler_Task_c* apc_client)
{
  // delete from Queue if not empty
  if(!mc_taskQueue.empty()){
    STL_NAMESPACE::list<SchedulerEntry_c>::iterator itc_task;
    for(itc_task = mc_taskQueue.begin(); itc_task != mc_taskQueue.end();){

      if(itc_task->isTask(apc_client)){
        #if DEBUG_SCHEDULER || DEBUG_TASKS_QUEUE
        INTERNAL_DEBUG_DEVICE << "Scheduler_cunregisterClient() Delete from TaskList:"
        << itc_task->getTaskName() << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        itc_task = mc_taskQueue.erase(itc_task);
        setCntClient(mc_taskQueue.size() );
      }
      else
      {
        itc_task++;
      }
    }//end for
  }


  // output client array in sync
  if ( ! mc_taskQueue.empty() )
  { // delete last element
    #if DEBUG_HEAP_USEAGE
    sui16_clientPointerTotal--;

    #endif

  }

}

/** deliver available time for time event
  * @param ai16_awaitedExecTime optional awaited execution time of planned next step
           ==> answer of this function tells, if planned step will fit into time frame
  */
int16_t Scheduler_c::getAvailableExecTime( int16_t ai16_awaitedExecTime )
{ // check if mb_execStopForced is set
  //if ( mb_execStopForced ) return 0;
  if ( mb_execStopForced ) return 0 ;
  /// values < 0 for mi32_demandedExecEndScheduler means unrestricted execution time
  if ( mi32_demandedExecEndScheduler >= 0 )
  {
    const int32_t ci32_now = System_c::getTime();
    const int32_t ci32_result = mi32_demandedExecEndScheduler - ci32_now;
    if ( ci32_result < 0 ) return 0; ///< we are too late
    else if ( ci32_result < ai16_awaitedExecTime ) return 0; ///< indicate that awaited time for next step is too long
    else if ( ci32_result > 0x7FFF ) return 0x7FFF; ///< limit to the biggest int16_t time
    else return ci32_result;
  }
  return -1;
}

int16_t
Scheduler_c::getCentralSchedulerAvailableExecTime()
{
  if ( mi32_demandedExecEndScheduler == 0 ) return 0;
  if ( mi32_demandedExecEndScheduler == -1 ) return -1;

  const int32_t i32_now = System_c::getTime();

  if ( i32_now >= mi32_demandedExecEndScheduler ) return 0;
  else return ( mi32_demandedExecEndScheduler - i32_now );
}



/**
  call the timeEvent for CanIo_c and all communication classes (derived from Scheduler_Task_c) which
  registered within Scheduler_c for periodic timeEvent.
  Define common trigger timestamp, so that distributed activities can be performed with
  common time base.
  @param ai32_demandedExecEndScheduler optional timestamp, where timeEvent shall return execution to calling function
         -> allow tight integration of IsoAgLib into application specific scheduler, as In-Time execution is
         guaranteed (default -1 -> no execution stop defined)
  @return idleTime for main application (> 0 wait for next call; == 0 call function again)
          idleTime < 0  One Client could not finish his Job
*/
int32_t Scheduler_c::timeEvent( int32_t ai32_demandedExecEndScheduler )
{ // first check if demanded exec time allows execution
  // update last trigger time
  #if defined(TEST_TIMING) && defined(SYSTEM_PC)
  static int32_t si32_globalTimeStart = 0;
  static int32_t si32_cpuTimeStart = 0;
  si32_globalTimeStart = HAL::getTime();
  si32_cpuTimeStart = (STL_NAMESPACE::clock() / ( CLOCKS_PER_SEC / 1000 ));
  #endif

  int32_t i32_stepStartTime = mi32_lastTimeEventTime = System_c::getTime();

  mi32_demandedExecEndScheduler = ai32_demandedExecEndScheduler;
  #ifdef CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME
  if ( ai32_demandedExecEndScheduler < 0 )
  { // limit execution time, even if no limit was defined by caller - avoid deadlock due to overload
    mi32_demandedExecEndScheduler = i32_stepStartTime + CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME;
  }
  #endif
  // trigger the watchdog
  System_c::triggerWd();

  int32_t i32_endCanProcessing = mc_taskQueue.front().getTimeToNextTrigger( retriggerType_t(LatestRetrigger) );
  if ( i32_endCanProcessing < ci32_minCanProcessingTime ) i32_endCanProcessing = HAL::getTime() +  ci32_minCanProcessingTime;
  else i32_endCanProcessing = HAL::getTime() + i32_endCanProcessing;
  Scheduler_Task_c::setDemandedExecEnd( (i32_endCanProcessing <ai32_demandedExecEndScheduler)? i32_endCanProcessing : ai32_demandedExecEndScheduler   );

  // process CAN messages
  if ( getCanInstance4Comm().timeEvent() )
  { // all CAN_IO activities ready -> update statistic for CAN_IO
    mi16_canExecTime = System_c::getTime() - i32_stepStartTime;
  }
  System_c::triggerWd();
  /* call EEEditor Process */
  #if defined(USE_CAN_EEPROM_EDITOR)
    // check if immediate return is needed
    HAL::ProcessCANEEEditorMsg();
  #elif defined(USE_RS232_EEPROM_EDITOR)
    // check if immediate return is needed
    HAL::ProcessRS232EEEditorMsg();
  #endif

  #if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  for ( uint8_t ind = 1; ind < CAN_INSTANCE_CNT; ind++ )
  { // process msg of other BUS ( other CAN is always at position 1 (independent from CAN BUS at controller!!)
    getCanInstance( ind ).timeEvent();
    System_c::triggerWd();
  }
  #else
  System_c::triggerWd();
  #endif


  /// Call timeEvent for next Client in TaskQueue as long as
  /// one Client returns idleTime=0 and Scheduler_c has still time;
  /// (i32_idleTime > 0)-> ends loop -> triggers still proccessmsg()
  int32_t  i32_idleTime;
  for ( i32_idleTime = selectCallTaskAndUpdateQueue();
        ((i32_idleTime  == 0) && (System_c::getTime() < mi32_demandedExecEndScheduler));
        i32_idleTime = selectCallTaskAndUpdateQueue() )
  {
    System_c::triggerWd();
  }

  i32_endCanProcessing = mc_taskQueue.front().getTimeToNextTrigger( retriggerType_t(LatestRetrigger) );
  if ( i32_endCanProcessing < ci32_minCanProcessingTime ) i32_endCanProcessing = HAL::getTime() +  ci32_minCanProcessingTime;
  else i32_endCanProcessing = HAL::getTime() + i32_endCanProcessing;
  Scheduler_Task_c::setDemandedExecEnd( ( i32_endCanProcessing < ai32_demandedExecEndScheduler)? i32_endCanProcessing : ai32_demandedExecEndScheduler   );

  /// @todo SOON-126 Removing the below extra-CAN-processing is subject to be done..
  // check if all tasks are called
  if ( i32_idleTime > 0 )
  {
#if 0
/// see above
    // as we are in time, call CanIo_c::processMsg() if last CanIo_c::timeEvent
    System_c::triggerWd();
    // was used to process received messages
    // ==> allows to cope wiht high BUS loads
    if ( getAvailableExecTime() != 0 )
    { // simply call processMsg and don't update statistic
      getCanInstance4Comm().processMsg();
    }
    #if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
    for ( uint8_t ind = 1; ind < CAN_INSTANCE_CNT; ind++ )
    {
      if ( getAvailableExecTime() != 0 )
      { // simply call processMsg and don't update statistic
        // process msg of other BUS ( other CAN is always at position 1 (independent from CAN BUS at controller!!)
        getCanInstance( ind ).processMsg();
      }
    }
    #endif
#endif
    // timeEvent of all registered clients called -> update overall
    // time statistic
    if ( mi32_averageExecTime != 0 )
      // use pseudo average time: take middle value between last and actual value
      mi32_averageExecTime = ( mi32_averageExecTime + ( System_c::getTime() - mi32_lastTimeEventTime ) ) / 2;
    else
      mi32_averageExecTime = ( System_c::getTime() - mi32_lastTimeEventTime );

  }
   // trigger the watchdog
    System_c::triggerWd();


    #if defined(TEST_TIMING) && defined(SYSTEM_PC)
    const int32_t ci32_globalTimeEnd = HAL::getTime();
    const int32_t ci32_cpuTimeEnd = (STL_NAMESPACE::clock() / ( CLOCKS_PER_SEC / 1000 ));

    const int32_t ci32_deltaGlobal = ci32_globalTimeEnd - si32_globalTimeStart;
    const int32_t ci32_deltaCpu = ci32_cpuTimeEnd - si32_cpuTimeStart;

    if ( ( ci32_deltaCpu > 0) && (ci32_deltaGlobal > 0))
    {
      const float cf_deltaPercent = 100.0 * float(ci32_deltaCpu) / float(ci32_deltaGlobal);

      if ( cf_deltaPercent < TEST_TIMING )
        INTERNAL_DEBUG_DEVICE << "\n\n#########\nProblem as CPU had only " << cf_deltaPercent
          << " Percent usage of CPU during this IsoAgLib Scheduler run"
          << ", delta Global: " << ci32_deltaGlobal
          << ", delta GPU: " << ci32_deltaCpu
          << "#########\n\n\n" << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    #endif


    ///return i32_idleTime to inform mainapplication when recall is needed;
    return i32_idleTime;
}

//!  resort from start of task list by swapping neighbour elements.
//!  Stop execution, if compared elements are in correct order.
//!  Avoid complex complete sort of list, if only the previously executed task must be placed in the correct position again - the rest of the list is still correct sorted.
//!  @param apc_sort ptr to currently executed SchedulerEntry_c
void
Scheduler_c::resortTaskList(const SchedulerEntry_c* apc_sort)
{
  if(cntClient() <= 1) return ; //nothing to sort

  STL_NAMESPACE::list<SchedulerEntry_c>::iterator iterExecuted = mc_taskQueue.begin();

  for ( ; iterExecuted != mc_taskQueue.end(); iterExecuted++ )
  {
    if (apc_sort == &(*iterExecuted) ) break;
  }
  if ( iterExecuted == mc_taskQueue.end() ) return;

  STL_NAMESPACE::list<SchedulerEntry_c>::iterator iterCompare = iterExecuted;

  // compare with the next item in list
  ++iterCompare;

  // Safety test
  if (iterCompare == mc_taskQueue.end()) return;

  if ( *iterExecuted <= *iterCompare )
  { // the current item is still having lower retrigger time -> no resort needed
    return;
  }

  // now in any case the current item
  // (i.e. the one that has previously been executed and should now be re-entered
  //  in right place) has to be removed
  // move current item from mc_taskQueue to the spare list - without any deletion and
  // re-allocation of a task-node -> should be faster
  mc_spareQueue.splice( mc_spareQueue.end(), mc_taskQueue, iterExecuted );
  for ( iterCompare = mc_taskQueue.begin();
        iterCompare != mc_taskQueue.end();
        ++iterCompare )
  {
    if ( mc_spareQueue.front() <= *iterCompare )
    { // move first/single item from spare list in front of iterator iterCompare
      mc_taskQueue.splice( iterCompare, mc_spareQueue, mc_spareQueue.begin() );
      break;
    }
  }
  if ( iterCompare == mc_taskQueue.end() )
  { // no other item in queue had later next retrigger time -> place c_resortedFrontItem at end
    // move first item from spare list in front of end() --> add it as last item to list
    mc_taskQueue.splice( mc_taskQueue.end(), mc_spareQueue, mc_spareQueue.begin() );
  }


  #if DEBUG_SCHEDULER
  static int32_t si32_lastTaskListTimesDebug = 0;
  /// for better debugging use > 100 or > 0 here
  if ( ( System_c::getTime() - si32_lastTaskListTimesDebug ) > 1000 ) {
      si32_lastTaskListTimesDebug = System_c::getTime();

    INTERNAL_DEBUG_DEVICE << "\n\nTaskList: Time to next Call after resort: ";
    printTaskList();
  }
  #endif
}

//!  This function  selects the next task(Scheduler_Task_c Client), executes it and updates the task-list.
//!  If no task can be immediately started with EarliestRetrigger-option,
//!  the time to earliest triggerable task
//!  is given to setDebugIdleInformation.
//!  Each In-Time call of a task is
//!  given to setDebugTimeAccuracy,
//!  to debug the time behaviour of the system.
//!  @return idle_time till next executable task
//!  ( 0 == this function should be called immediately again; -1 one Client was running out of time )
int32_t
Scheduler_c::selectCallTaskAndUpdateQueue()
{ // ~X2C
  System_c::triggerWd();
  #if DEBUG_SCHEDULER
  static bool b_firstCallSelectCallTastAndUpdateQueue = true;
  if ( b_firstCallSelectCallTastAndUpdateQueue ) {
    b_firstCallSelectCallTastAndUpdateQueue = false;
    INTERNAL_DEBUG_DEVICE << "Scheduler_c::selectCallTaskAndUpdateQueue()\n";
  }
  #endif
  // declare local timestamp vars as static to avoid repeating allocation
  static int32_t i32_idleTime, i32_endTime;

  i32_idleTime = mc_taskQueue.front().getTimeToNextTrigger( retriggerType_t(EarliestRetrigger|StandardRetrigger) );
  i32_endTime = System_c::getTime();


  if ( i32_idleTime <= 0 )
  { // first task in queue can be executed
    STL_NAMESPACE::list<SchedulerEntry_c>::iterator pc_execIter = mc_taskQueue.begin();
    #if DEBUG_SCHEDULER
    setDebugTimeAccuracy( *pc_execIter );
    #endif

    // get max time to execution of next entry waiting
    STL_NAMESPACE::list<SchedulerEntry_c>::const_iterator pc_nextCallIter = pc_execIter;
    pc_nextCallIter++; /// go one further

    ///get Last Execution Time of next in Queue and ADD to actualTime
    int32_t i32_nextTaskTriggerTimeSpread = CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME;
    if ( pc_nextCallIter != mc_taskQueue.end() )
      i32_nextTaskTriggerTimeSpread = pc_nextCallIter->getTimeToNextTrigger( LatestRetrigger );
    if(  i32_nextTaskTriggerTimeSpread   <  pc_execIter->getForcedMinExecTime() ){
    #if DEBUG_SCHEDULER
    INTERNAL_DEBUG_DEVICE << "i32_endTime to small for " <<  pc_execIter->getTaskName() << "endTime "
      << (int) i32_nextTaskTriggerTimeSpread  << " Set to " << pc_execIter->getTaskName() << " msec " << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
      i32_endTime += pc_execIter->getForcedMinExecTime() ;  //add getForcedMinExecTime()
    }
    else {
      //regular condition
      i32_endTime += i32_nextTaskTriggerTimeSpread;
    }

    /// Decrease (Time to next Task) if remaining Time of Scheduler is smaller
    if ( ( getCentralSchedulerExecEndTime() >= 0 )
    && ( i32_endTime > getCentralSchedulerExecEndTime() ) )
    { // the calling app specified an end time which is lower than the calculated end time
      i32_endTime = getCentralSchedulerExecEndTime();
    }

    System_c::triggerWd();
    #if DEBUG_SCHEDULER_EXTREME
    INTERNAL_DEBUG_DEVICE << "Call timeevent of " <<  pc_execIter->getTaskName() << "endTime "
      << (int) i32_endTime << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    ///Call Clients timeEvent() via SchedulerEntry_c::timeEventExec()
    /// IF Client returns with false -> return i32_idleTime = -1
    /// because last Client could not finish in available TimeSpan
    SchedulerEntry_c* pc_execute = &(*pc_execIter);

    /// set the current Task as currently executed task
    pc_currentlyExecutedTask = pc_execute;

    const bool b_result = pc_execIter->timeEventExec( i32_endTime );

    ///reset the currently executed task.
    pc_currentlyExecutedTask = NULL;

    if ( !b_result && (pc_nextCallIter != mc_taskQueue.end() ) )
    { // time was not enough and more than one client is managed - check whether the next item
      // needs also to be executed
      if ( pc_nextCallIter->getTimeToNextTrigger( LatestRetrigger ) <= 0 )
      { // the executed task had not enough time and the next task needs to be called immediately
        // --> reschedule this task _after_ the following task
        int16_t i16_avgTime = (pc_nextCallIter->getAvgExecTime() > pc_nextCallIter->getForcedMinExecTime())?pc_nextCallIter->getAvgExecTime():pc_nextCallIter->getForcedMinExecTime();
        if ( pc_nextCallIter->getExecTime() > i16_avgTime ) i16_avgTime = pc_nextCallIter->getExecTime();
        #if DEBUG_SCHEDULER
        INTERNAL_DEBUG_DEVICE
          << "+++++++++++++++++++++\nretrigger tie breaked task: " << pc_execIter->getTaskName() << " for time: "
          << (i16_avgTime+pc_nextCallIter->getNextTriggerTime()) << " so that task "
          << pc_nextCallIter->getTaskName()
          << " with AVG exect time of: " << pc_nextCallIter->getAvgExecTime()
          << " can start as planned\n" << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        changeRetriggerTimeAndResort( pc_execIter, i16_avgTime+pc_nextCallIter->getNextTriggerTime() );
      }
    }
    else if (b_result)
    { // executed task finished its work -> reschedule it by resort of task list
      resortTaskList(pc_execute);
    }
    System_c::triggerWd();

    if(!b_result) return -1 ;
    // update idle time: first item is probably changed by sort
    i32_idleTime = mc_taskQueue.begin()->getTimeToNextTrigger( retriggerType_t(EarliestRetrigger|StandardRetrigger) );
  }


  if ( i32_idleTime <= 0 ) return 0;
  #if DEBUG_SCHEDULER
  setDebugIdleInformation( i32_idleTime );
  #endif

  return i32_idleTime;
}

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
void
Scheduler_c::setDebugTimeAccuracy(SchedulerEntry_c& rc_selectedTask)
{ // ~X2C
  static uint32_t sui32_lastDebugOutput = 0;
  static int16_t si16_maxDeviation = -32767,
                 si16_minDeviation = +32767;

  int16_t i16_timeDeviation = rc_selectedTask.getStdTimeToNextTrigger();
  bool b_correctTimeManagement = ( rc_selectedTask.getTimeToNextTrigger( LatestRetrigger ) >= 0  );
  int32_t i32_now = System_c::getTime();
  // update extreme times
  if ( i16_timeDeviation > si16_maxDeviation ) si16_maxDeviation = i16_timeDeviation;
  if ( i16_timeDeviation < si16_minDeviation ) si16_minDeviation = i16_timeDeviation;

  // approximately every 30 second with apporx 50msex period -> modulo 1001
  if ( ( i32_now - sui32_lastDebugOutput ) >= 30000 )
  { // send debug data on RS232
    sui32_lastDebugOutput = i32_now;
    INTERNAL_DEBUG_DEVICE << "setDebugTimeAccuracy()\n"
      << "Actual time: " << i32_now
      << " for Task: " << rc_selectedTask.getTaskName()
      << ",\tMax earlier execution: " << si16_maxDeviation
      << ",\tMin earlier execution: " << si16_minDeviation << ",\tCorrect Time Management: "
    << "idle time of FRONT: " << mc_taskQueue.front().getNextTriggerTime()
//    << "idle time of BACK: " << SystemManagement_c::instance().mc_taskQueue.back().getNextTriggerTime()
      << b_correctTimeManagement << INTERNAL_DEBUG_DEVICE_ENDL;
    if ( i16_timeDeviation >= 0 )
    { // everything is good
      INTERNAL_DEBUG_DEVICE << "GOOD System state with task called "
        << i16_timeDeviation << "[ms] before the standard retrigger time (still not earlier than allowed)"
        << ", with typical exec time: " << rc_selectedTask.getExecTime()
        << ", and period: " << rc_selectedTask.getTimePeriod() << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    else if ( b_correctTimeManagement )
    { // real deviation -> a task is executed after the standard retrigger time
      // but before the latest allowed time
      INTERNAL_DEBUG_DEVICE << "Critical Scheduling state, as a task is called "
        << -1*i16_timeDeviation << "[ms] after the standard retrigger time (bot not later than allowed)."
        << ", with typical exec time: " << rc_selectedTask.getExecTime()
        << ", and period: " << rc_selectedTask.getTimePeriod() << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    else
    { // bad state
      INTERNAL_DEBUG_DEVICE << "BAD System state, as  "
        << i16_timeDeviation << "[ms] after the standard and latest allowed retrigger time, with typical exec time: "
        << rc_selectedTask.getExecTime()
        << ", and period: " << rc_selectedTask.getTimePeriod() << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    // print timing fingerprint of actual task
    INTERNAL_DEBUG_DEVICE
      << "AVG exec time: " << rc_selectedTask.getAvgExecTime()
      << ", MIN exec time: " << rc_selectedTask.getMinExecTime()
      << ", MAX exec time: " << rc_selectedTask.getMaxExecTime()
      << "\nAVG timing accuracy: " << rc_selectedTask.getAvgTimingAccuracy()
      << ", MIN timing accuracy: " << rc_selectedTask.getMinTimingAccuracy()
      << ", MAX timing accuracy: " << rc_selectedTask.getMaxTimingAccuracy()
      << INTERNAL_DEBUG_DEVICE_ENDL;

    // reset extreme values, to compensate short max idle times (e.g. if a lot of debug messages are sent)
    si16_maxDeviation = -32767;
    si16_minDeviation = +32767;
  }
}


//!  Send debug messages with information on the idletime
//!  This is important to check whether the time
//!  restrictions are fullfilled.
//!  Use shortest period (now 25ms) of mainapplication for test
//!  Parameter:
//! @param aui16_idleTime:
void Scheduler_c::setDebugIdleInformation(uint16_t aui16_idleTime){

  static int32_t si32_lastRs232Send = 0;
  static uint16_t sui16_maxIdle = 0,
                  sui16_minIdle = 0xFFFF;

  int32_t i32_now = System_c::getTime();
  // update extreme times
  if ( aui16_idleTime > sui16_maxIdle ) sui16_maxIdle = aui16_idleTime;
  if ( aui16_idleTime < sui16_minIdle ) sui16_minIdle = aui16_idleTime;

  // debug msg every 30sec. (30.000 msec)
  if ( ( i32_now - si32_lastRs232Send ) > 30000 )
  { // ist's time to send debug data on RS232
    si32_lastRs232Send = i32_now;
    // send debug data on configured Device for Output
    INTERNAL_DEBUG_DEVICE << "setDebugIdleInformation()\n"
      << "Actual time: " << i32_now << ",\tMax IDLE time: " << sui16_maxIdle
      << ",\tMin IDLE time: " << sui16_minIdle
      << ",\tactual IDLE time: " << aui16_idleTime
      << "idle time of FRONT: " << mc_taskQueue.front().getNextTriggerTime()
      //<< "idle time of BACK: " << mc_taskQueue.back().getNextTriggerTime()
      << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  /// send debug msg if idle time is higher than allowed (longer than shortest period)
  if ( aui16_idleTime > 1001 )
  { // problem with scheduling
    INTERNAL_DEBUG_DEVICE << "setDebugIdleInformation() with too long idle of " << aui16_idleTime
      << ", TOP Task: " << mc_taskQueue.front().getTaskName()
      << ", Now: " << System_c::getTime()
      << ", Retrigger Time: " << mc_taskQueue.front().getNextTriggerTime()
      << INTERNAL_DEBUG_DEVICE_ENDL;

  }
}


void Scheduler_c::printTaskList()
{
  INTERNAL_DEBUG_DEVICE << "\n=========================\nPrintTaskList: ";
  for ( STL_NAMESPACE::list<SchedulerEntry_c>::iterator pc_test = mc_taskQueue.begin();
        pc_test != mc_taskQueue.end();
        pc_test++ )
  {
    if ( pc_test != mc_taskQueue.begin() ) INTERNAL_DEBUG_DEVICE << ", ";
    INTERNAL_DEBUG_DEVICE << pc_test->getTimeToNextTrigger(  retriggerType_t(StandardRetrigger) );
    if (pc_test->getTaskName() == NULL)
      INTERNAL_DEBUG_DEVICE << " unnamed task.";
    else
      INTERNAL_DEBUG_DEVICE << " name:" << pc_test->getTaskName();
  } //end for
  INTERNAL_DEBUG_DEVICE << "\n" << INTERNAL_DEBUG_DEVICE_ENDL;
}

#endif


/// Calculate Delta from TimePeriod of a Client
/// Scheduler_c set (NOW + delta of TimePeriod) as New Retrigger for Client
/// and sort Task to the right Position in the TaskQueue
/// @param apc_client -> Client in Scheduler_c TaskQueue
/// @param aui16_newTimePeriod -> New Period will set for the Client by Scheduler_c
bool Scheduler_c::changeTimePeriodAndResortTask(Scheduler_Task_c * apc_client  , uint16_t aui16_newTimePeriod ){

/** the task involved in the operation is being executing now , the operation cannot be performed */
  if(isTaskExecuted(apc_client))
  {
   #if DEBUG_SCHEDULER
    INTERNAL_DEBUG_DEVICE << " Method Scheduler_c::changeTimePeriodAndResortTask" << INTERNAL_DEBUG_DEVICE_ENDL;

    TraceAndAbort();
   #endif
    return false;
  }


  //Now calculate Delta and nextTriggerTime for Client
  int16_t i_deltaTime = aui16_newTimePeriod - apc_client->getTimePeriod()  ;
  int32_t i32_newTriggerTime = apc_client->getNextTriggerTime() + i_deltaTime;
#if DEBUG_SCHEDULER
    INTERNAL_DEBUG_DEVICE << "New TimePeriod:" << (int) aui16_newTimePeriod
    << " Old TimePeriod: "<<  apc_client->getTimePeriod();
    if (apc_client->getTaskName() == NULL)
      INTERNAL_DEBUG_DEVICE <<" Name" << apc_client->getTaskName() << INTERNAL_DEBUG_DEVICE_ENDL;
    else
      INTERNAL_DEBUG_DEVICE <<" unnamed Task" << apc_client->getTaskName() << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  ///Now lets do the resort  and update new TimePeriod in client
  return changeRetriggerTimeAndResort(apc_client,i32_newTriggerTime ,aui16_newTimePeriod);

}



//!  Uses Delta from TimePeriod of a Client
//!  to put a Task to the right Position in the TaskQueue
//!  ATTENTION parameter nextRetriggerTime will exactly used from Scheduler_c
//!  for call of timevent.-> so add e.g. an TimePeriod for an later call
//! @param ac_client -> Client in Scheduler_c TaskQueue
//! @param ai32_newRetriggerTime -> New ai32_nextRetriggerTime set for Client by Scheduler_c
//! @param ai16_newTimePeriod otpional -> New Period will set for the Client by Scheduler_c
bool  Scheduler_c::changeRetriggerTimeAndResort(SchedulerEntry_c ac_client  , int32_t ai32_newRetriggerTime, int16_t ai16_newTimePeriod)
{

  /** the task involved in the operation is being executing now , the operation cannot be performed */
  if(pc_currentlyExecutedTask == &ac_client)
  {
    #if DEBUG_SCHEDULER
    INTERNAL_DEBUG_DEVICE << " Method Scheduler_c::changeRetriggerTimeAndResort" << INTERNAL_DEBUG_DEVICE_ENDL;

    TraceAndAbort();
    #endif
    return false;
  }

  if ( mc_taskQueue.empty() ) return false;
  else if (cntClient() == 1) return true;
  else
  { // search iterator for the client of change
    STL_NAMESPACE::list<SchedulerEntry_c>::iterator itc_task;
    for(itc_task = mc_taskQueue.begin(); itc_task != mc_taskQueue.end(); itc_task++)
    {
      if (*itc_task == ac_client)
        return changeRetriggerTimeAndResort(itc_task, ai32_newRetriggerTime, ai16_newTimePeriod);
    }
    // not found
    return false;
  }
}

/// Uses Delta from ai32_nextRetriggerTime of a Client
/// to put a Task to the right Position in the TaskQueue
/// ATTENTION parameter nextRetriggerTime will be exactly used from Scheduler_c
/// for call of timevent.-> so add e.g. an TimePeriod for an later call
/// @param apc_client -> Client in Scheduler_c TaskQueue
/// @param ai32_newRetriggerTime -> New ai32_nextRetriggerTime set for Client by Scheduler_c
/// @param ai16_newTimePeriod optional -> New Period will set for the Client by Scheduler_c
bool Scheduler_c::changeRetriggerTimeAndResort(Scheduler_Task_c * apc_client, int32_t ai32_newRetriggerTime, int16_t ai16_newTimePeriod)
{

  /** the task involved in the operation is being executing now , the operation cannot be performed */
  if(isTaskExecuted(apc_client))
  {
    #if DEBUG_SCHEDULER
    INTERNAL_DEBUG_DEVICE << " Method Scheduler_c::changeRetriggerTimeAndResort" << INTERNAL_DEBUG_DEVICE_ENDL;

    TraceAndAbort();
    #endif
    return false;
  }

  if ( mc_taskQueue.empty() ) return false;
  else if (cntClient() == 1) return true;
  else
  { // search iterator for the client of change
    STL_NAMESPACE::list<SchedulerEntry_c>::iterator itc_task;
    for(itc_task = mc_taskQueue.begin(); itc_task != mc_taskQueue.end(); itc_task++)
    {
      if(itc_task->isTask(apc_client))
        return changeRetriggerTimeAndResort(itc_task, ai32_newRetriggerTime, ai16_newTimePeriod);
    }
    // not found
    return false;
  }
}

/// Uses Delta from ai32_nextRetriggerTime of a Client
/// to put a Task to the right Position in the TaskQueue
/// ATTENTION parameter nextRetriggerTime will be exactly used from Scheduler_c
/// for call of timevent.-> so add e.g. an TimePeriod for an later call
/// @param itc_task -> iterator to the task that should be changed
/// @param ai32_newRetriggerTime -> New ai32_nextRetriggerTime set for Client by Scheduler_c
/// @param  ai16_newTimePeriod optional -> New Period will set for the Client by Scheduler_c
bool Scheduler_c::changeRetriggerTimeAndResort(STL_NAMESPACE::list<SchedulerEntry_c>::iterator itc_task, int32_t ai32_newRetriggerTime, int16_t ai16_newTimePeriod)
{
  #if DEBUG_SCHEDULER
  printTaskList();
  #endif

#if DEBUG_SCHEDULER
  if (itc_task->getTaskName() == NULL)
    INTERNAL_DEBUG_DEVICE << "Scheduler_c notifyed by unnamed Client." << INTERNAL_DEBUG_DEVICE_ENDL;
  else
    INTERNAL_DEBUG_DEVICE << "Scheduler_c notifyed by Client: " << itc_task->getTaskName() << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  int32_t i32_deltaRetrigger = ai32_newRetriggerTime - itc_task->getNextTriggerTime();

  //set New TimePeriod
  if(ai16_newTimePeriod > -1 ) itc_task->setTimePeriod(ai16_newTimePeriod);


  /// increase of RetriggerTime task should be called LATER
  if (i32_deltaRetrigger > 0) {
    #if DEBUG_SCHEDULER
    INTERNAL_DEBUG_DEVICE << "task should be called LATER for ms: " << i32_deltaRetrigger
      << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    itc_task->changeNextTriggerTime( ai32_newRetriggerTime );
    //remove to LATER Position
    STL_NAMESPACE::list<SchedulerEntry_c>::iterator itc_greater = itc_task;
    ++itc_greater;
    if ( itc_greater == mc_taskQueue.end() )
    {
      #if DEBUG_SCHEDULER
      printTaskList();
      #endif
      return true; ///< was already last element
    }
    else if ( *itc_task <= *itc_greater )
    { // there is no need to resort, as following item is still later
      #if DEBUG_SCHEDULER
      printTaskList();
      #endif
      return true;
    }
    // from here on: the list has to be resorted
    for ( ++itc_greater; itc_greater != mc_taskQueue.end(); ++itc_greater )
    {
      if ( *itc_task <= *itc_greater )
      { // the item at itc_greater has late enough retrigger time, so that
        // itc_task can be inserted before

        // move itc_task from mc_taskQueue to the mc_spareQueue
        mc_spareQueue.splice( mc_spareQueue.end(), mc_taskQueue, itc_task );
        // and move itc_task back from mc_spareQueue in front of itc_greater in mc_taskQueue
        mc_taskQueue.splice( itc_greater, mc_spareQueue, mc_spareQueue.begin() );
        #if DEBUG_SCHEDULER
        printTaskList();
        #endif
        return true;
      }
    }
    // we reach only here, when no other task in queue afterwards has later retrigger time
    // --> place it at end

    // move itc_task from mc_taskQueue to the mc_spareQueue
    mc_spareQueue.splice( mc_spareQueue.end(), mc_taskQueue, itc_task );
    // and move itc_task back from mc_spareQueue in front of mc_taskQueue.end() - i.e. at the end of the list
    mc_taskQueue.splice( mc_taskQueue.end(), mc_spareQueue, mc_spareQueue.begin() );
    #if DEBUG_SCHEDULER
    printTaskList();
    #endif
  }
  /// decrease of TimePeriod task should be called EARLIER
  else if (i32_deltaRetrigger < 0){
    #if DEBUG_SCHEDULER
    INTERNAL_DEBUG_DEVICE << "task should be called EARLIER for ms: " << i32_deltaRetrigger << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    ///set new NextTriggerTime to now to avoid delay of following tasks
    if((ai32_newRetriggerTime < System_c::getTime())   ) ai32_newRetriggerTime = System_c::getTime();

    ///set new NextTriggerTime
    itc_task->changeNextTriggerTime( ai32_newRetriggerTime );
    //remove to EARLIER Position
    if ( itc_task == mc_taskQueue.begin() )
    {
      #if DEBUG_SCHEDULER
      printTaskList();
      #endif
      return true; ///< the changed task is already at start of queue
    }
    STL_NAMESPACE::list<SchedulerEntry_c>::iterator itc_smaller = itc_task;
    --itc_smaller;
    if ( *itc_smaller <= *itc_task )
    {
      #if DEBUG_SCHEDULER
      printTaskList();
      #endif
      return true; ///< the changed task is still later scheduled than the item before it
    }
    // now we really have to resort
    for ( ; itc_smaller != mc_taskQueue.begin(); --itc_smaller )
    {
      if ( *itc_smaller <= *itc_task )
      { // the item at itc_smaller has early enough retrigger time, so that
        // itc_task can be inserted afterwards
        ++itc_smaller; ///< the insertion can only take place _before_ an iterator

        // move itc_task from mc_taskQueue to the mc_spareQueue
        mc_spareQueue.splice( mc_spareQueue.end(), mc_taskQueue, itc_task );
        // and move itc_task back from mc_spareQueue in front of itc_smaller
        mc_taskQueue.splice( itc_smaller, mc_spareQueue, mc_spareQueue.begin() );
        #if DEBUG_SCHEDULER
        printTaskList();
        #endif
        return true;
      }
    }
    if ( ( itc_smaller == mc_taskQueue.begin() ) && (*itc_smaller <= *itc_task))
    { // the first item in list has earlier trigger
      ++itc_smaller; ///< the insertion can only take place _before_ an iterator

      // move itc_task from mc_taskQueue to the mc_spareQueue
      mc_spareQueue.splice( mc_spareQueue.end(), mc_taskQueue, itc_task );
      // and move itc_task back from mc_spareQueue in front of itc_smaller
      mc_taskQueue.splice( itc_smaller, mc_spareQueue, mc_spareQueue.begin() );
      #if DEBUG_SCHEDULER
      printTaskList();
      #endif
      return true;
    }
    // we reach only here, when no other item before changed task has earlier retrigger time
    // -> place it at front of list

    // move itc_task from mc_taskQueue to the mc_spareQueue
    mc_spareQueue.splice( mc_spareQueue.end(), mc_taskQueue, itc_task );
    // and move itc_task back from mc_spareQueue in front (mc_taskQueue.begin())++ - i.e. at begin of list
    mc_taskQueue.splice( mc_taskQueue.begin(), mc_spareQueue, mc_spareQueue.begin() );
  } // end if shift forward
  #if DEBUG_SCHEDULER
  printTaskList();
  #endif
  return  true ;
}

 /** The function checks whether the input Scheduler_task_c is in the timeEvent,
     in this case return true */
bool Scheduler_c::isTaskExecuted(const Scheduler_Task_c* apc_client) const
  {
      if(pc_currentlyExecutedTask == NULL)
      {
        return false;
      }

      if(pc_currentlyExecutedTask->isTask( apc_client))
      {
        return true;
      }

      return false;
   };

#if DEBUG_SCHEDULER

/** Tracing function for debugging aim, the function performs an abort in case of SYSTEM_PC target */

void Scheduler_c::TraceAndAbort(){

        INTERNAL_DEBUG_DEVICE << " The Task is currently on the timeEvent."
        << "It is not allowed to call the methods changeTimePeriodAndResortTask,changeRetriggerTimeAndResort, changeRetriggerTimeAndResort "
        << "Please call the method setTimePeriod instead " << INTERNAL_DEBUG_DEVICE_ENDL;

        #ifdef SYSTEM_PC
        MACRO_ISOAGLIB_ABORT();
        #endif

   }
#endif

} // end of namespace __IsoAgLib
