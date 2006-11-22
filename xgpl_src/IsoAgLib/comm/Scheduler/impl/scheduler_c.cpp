/***************************************************************************
                          scheduler_c.cpp - implementation for central hardware
                                   independent object which manages all
                                   functional IsoAgLib member objects
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

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#include "scheduler_c.h"
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/util/liberr_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>

#ifdef USE_PROCESS
  #include <IsoAgLib/comm/Process/impl/process_c.h>
#endif
#ifdef USE_TRACTOR_GENERAL
  #include <IsoAgLib/comm/Base/impl/tracgeneral_c.h>
#endif
#ifdef USE_TRACTOR_MOVE
  #include <IsoAgLib/comm/Base/impl/tracmove_c.h>
#endif
#ifdef USE_TRACTOR_PTO
  #include <IsoAgLib/comm/Base/impl/tracpto_c.h>
#endif
#ifdef USE_TRACTOR_LIGHT
  #include <IsoAgLib/comm/Base/ext/impl/traclight_c.h>
#endif
#ifdef USE_TRACTOR_AUX
  #include <IsoAgLib/comm/Base/ext/impl/tracaux_c.h>
#endif
#ifdef USE_TIME_GPS
  #include <IsoAgLib/comm/Base/impl/timeposgps_c.h>
#endif
#ifdef USE_ISO_TERMINAL
  #include <IsoAgLib/comm/ISO_Terminal/impl/isoterminal_c.h>
#endif
#ifdef DEF_Stream_IMPL
  #include <IsoAgLib/comm/Multipacket/impl/multireceive_c.h>
  #include <IsoAgLib/comm/Multipacket/impl/multisend_c.h>
#endif

#if defined(USE_CAN_EEPROM_EDITOR) || defined( USE_RS232_EEPROM_EDITOR )
  #include <IsoAgLib/hal/eeprom.h>
#endif

#if defined(DEBUG_SCHEDULER) || defined(DEBUG_HEAP_USEAGE)
  #include <IsoAgLib/util/impl/util_funcs.h>
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif


#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_clientPointerTotal = 0;
static uint16_t sui16_clientTimeTotal = 0;

#ifdef MASSERT
extern unsigned int AllocateHeapMalloc;
extern unsigned int DeallocateHeapMalloc;
#endif
#endif

/* ************************************** */
/* import some namespaces for easy access */
/* ************************************** */
namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Scheduler_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  Scheduler_c& getSchedulerInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return Scheduler_c::instance( rui8_instance );
  }
#else
  /** C-style function, to get access to the unique Scheduler_c singleton instance */
  Scheduler_c& getSchedulerInstance( void )
  {
     static Scheduler_c& c_lbs = Scheduler_c::instance();
    return c_lbs;
  }
#endif

/** timestamp where last timeEvent was called -> can be used to synchronise distributed timeEvent activities */
int32_t Scheduler_c::i32_lastTimeEventTime = 0;

/** commanded timestamp, where Scheduler_c::timeEvent MUST return action to caller */
int32_t Scheduler_c::i32_demandedExecEndScheduler = 0;

/** flag to detect, if other interrupting task forced immediated stop of Scheduler_c::timeEvent() */
bool Scheduler_c::b_execStopForced = false;


/** initialisation for the central IsoAgLib object */
void Scheduler_c::init( void )
{ // set attributes to valid initial state
  b_execStopForced = false;
  i16_canExecTime = 0;
  i32_averageExecTime = 0;
  i32_demandedExecEndScheduler = 0;
  i32_lastTimeEventTime = 0;


  // clear the scheduler queues
  while ( !c_taskQueue.empty() ) c_taskQueue.pop_front();

}

/** simply close communicating clients */
void Scheduler_c::closeCommunication( void ) {
  // as soon as all communicating IsoAgLib clients are closed, CANIO_c can be closed
  getCanInstance4Comm().close();
  #if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  for ( uint8_t ind = 1; ind < CAN_INSTANCE_CNT; ind++ )
  { // process msg of other BUS ( other CAN is always at position 1 (independent from CAN BUS at controller!!)
    getCanInstance( ind ).close();
  }
  #endif
  for ( std::list<SchedulerEntry_c>::iterator iter = c_taskQueue.begin(); ! c_taskQueue.empty(); iter = c_taskQueue.begin())
  { // call close for each registered client
    iter->close();
    c_taskQueue.erase(iter);
  }
}


/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void Scheduler_c::close( void )
{ // call close for each registered client system
  closeCommunication();
  // as soon as all communicating IsoAgLib clients are closed, CANIO_c can be closed
  getCanInstance4Comm().close();
  #if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  for ( uint8_t ind = 1; ind < CAN_INSTANCE_CNT; ind++ )
  { // process msg of other BUS ( other CAN is always at position 1 (independent from CAN BUS at controller!!)
    getCanInstance( ind ).close();
  }
  #endif
  // last but not least close System
  getSystemInstance().close();
}


void Scheduler_c::startSystem()
{
  if (!b_systemStarted)
  {
    b_systemStarted = true;
    // NOW INIT ONCE the core singleton classes that correspond to the compile time
    // configured features of the IsoAgLib
#ifdef USE_PROCESS
    getProcessInstance4Comm().init();
#endif
#ifdef USE_TRACTOR_GENERAL
    getTracGeneralInstance4Comm().init();
#endif
#ifdef USE_TRACTOR_MOVE
    getTracMoveInstance4Comm().init();
#endif
#ifdef USE_TRACTOR_PTO
    getTracPtoInstance4Comm().init();
#endif
#ifdef USE_TRACTOR_LIGHT
    getTracLightInstance4Comm().init();
#endif
#ifdef USE_TRACTOR_AUX
    getTracAuxInstance4Comm().init();
#endif
#ifdef USE_TIME_GPS
    getTimePosGpsInstance4Comm().init();
#endif
#ifdef USE_ISO_TERMINAL
    getIsoTerminalInstance().init();
#endif
#ifdef DEF_Stream_IMPL
    getMultiReceiveInstance4Comm().init();
    getMultiSendInstance4Comm().init();
#endif
  }
}


/** handler function for access to undefined client.
  * the base Singleton calls this function, if it detects an error
  */
void Scheduler_c::registerAccessFlt( void )
{
  getLibErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::Scheduler );
}


/** register pointer to a new client
  * this function is called within construction of new client instance
  */
bool Scheduler_c::registerClient( ElementBase_c* pc_client)
{
  // first check whether this client is already registered
  for ( std::list<SchedulerEntry_c>::const_iterator iter = c_taskQueue.begin(); iter != c_taskQueue.end(); iter++ )
  {
    if (iter->isTask( pc_client ) ) return false;
  }

  /// add 2ms to startTime of new Client to avoid crossing timestamps
  static int32_t si32_taskStartTime = 0;
  if ( si32_taskStartTime == 0 ) si32_taskStartTime = System_c::getTime();
  else si32_taskStartTime += 2;
  //For Client that registers at later timepoint
  if ( System_c::getTime() > si32_taskStartTime ) si32_taskStartTime = System_c::getTime();;

  #ifdef DEBUG_SCHEDULER
  INTERNAL_DEBUG_DEVICE << "RegisteredClient: "<<  pc_client->getTaskName() <<  INTERNAL_DEBUG_DEVICE_ENDL;
  #endif


  pc_client->startTaskTiming(si32_taskStartTime);
  /// put client in taskQueue
  const uint16_t ui16_oldSize = c_taskQueue.size();
  c_taskQueue.push_front( SchedulerEntry_c( pc_client ) );
  // check whether the task list grew as awaited
  if ( c_taskQueue.size() <= ui16_oldSize ) return false;
  #ifdef DEBUG_HEAP_USEAGE
  sui16_clientPointerTotal++;

  getRs232Instance()
    << sui16_clientPointerTotal
    << "(" << c_arrClientC1.capacity()
    << ") x Scheduler_c Clients: Mal-Alloc: "
    << sizeVectorTWithMalloc( sizeof(void*), c_taskQueue.capacity() )
    << "/" << sizeof(void*)
    << ", Chunk-Alloc: "
    << sizeVectorTWithChunk( sizeof(void*), c_arrClientC1.capacity() )
    << INTERNAL_DEBUG_DEVICE_ENDL
    << sui16_clientTimeTotal
    #ifdef MASSERT
    << "\r\n__mall tot:" << AllocateHeapMalloc
    << ", _mall deal tot: " << DeallocateHeapMalloc
    << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    #else
    << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  #endif

  return true;
}


/** unregister pointer to a already registered client
  * this function is called within destruction of new client instance
  */
void Scheduler_c::unregisterClient( ElementBase_c* pc_client)
{
  // delete from Queue if not empty
  if(!c_taskQueue.empty()){
    STL_NAMESPACE::list<SchedulerEntry_c>::iterator itc_task;
    for(itc_task = c_taskQueue.begin(); itc_task != c_taskQueue.end(); itc_task++){

      if(itc_task->isTask(pc_client)){
        #ifdef DEBUG_SCHEDULER
        INTERNAL_DEBUG_DEVICE << "Scheduler_cunregisterClient() Delete from TaskList:"
        << itc_task->getTaskName() << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        c_taskQueue.erase(itc_task) ;
      }
    }//end for
  }


  // output client array in sync
  if ( c_taskQueue.size() > 0 )
  { // delete last element
    #ifdef DEBUG_HEAP_USEAGE
    sui16_clientPointerTotal--;

    getRs232Instance()
      << sui16_clientPointerTotal
      << "(" << c_arrClientC1.capacity()
      << ") x Scheduler_c Clients: Mal-Alloc: "
      << sizeVectorTWithMalloc( sizeof(void*), c_taskQueue.capacity() )
      << "/" << sizeof(void*)
      << ", Chunk-Alloc: "
      << sizeVectorTWithChunk( sizeof(void*), c_taskQueue.capacity() )
      << INTERNAL_DEBUG_DEVICE_ENDL
      #ifdef MASSERT
      << "\r\n__mall tot:" << AllocateHeapMalloc
      << ", _mall deal tot: " << DeallocateHeapMalloc
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
      #else
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
    #endif

  }

}

/** deliver available time for time event
  * @param ri16_awaitedExecTime optional awaited execution time of planned next step
           ==> answer of this function tells, if planned step will fit into time frame
  */
int16_t Scheduler_c::getAvailableExecTime( int16_t ri16_awaitedExecTime )
{ // check if b_execStopForced is set
  //if ( b_execStopForced ) return 0;
  if ( b_execStopForced ) return 0 ;
  /// values < 0 for i32_demandedExecEndScheduler means unrestricted execution time
  if ( i32_demandedExecEndScheduler >= 0 )
  {
    const int32_t ci32_now = System_c::getTime();
    const int32_t ci32_result = i32_demandedExecEndScheduler - ci32_now;
    if ( ci32_result < 0 ) return 0; ///< we are too late
    else if ( ci32_result < ri16_awaitedExecTime ) return 0; ///< indicate that awaited time for next step is too long
    else if ( ci32_result > 0x7FFF ) return 0x7FFF; ///< limit to the biggest int16_t time
    else return ci32_result;
  }
  return -1;
}


/**
  call the timeEvent for CANIO_c and all communication classes (derived from ElementBase_c) which
  registered within Scheduler_c for periodic timeEvent.
  Define common trigger timestamp, so that distributed activities can be performed with
  common time base.
  @param ri32_demandedExecEndScheduler optional timestamp, where timeEvent shall return execution to calling function
         -> allow tight integration of IsoAgLib into application specific scheduler, as In-Time execution is
         guaranteed (default -1 -> no execution stop defined)
  @return idleTime for main application (> 0 wait for next call; == 0 call function again)
          idleTime < 0  One Client could not finish his Job
*/
int32_t Scheduler_c::timeEvent( int32_t ri32_demandedExecEndScheduler )
{ // first check if demanded exec time allows execution
  // update last trigger time
  int32_t i32_now = System_c::getTime();
  int32_t i32_stepStartTime = i32_lastTimeEventTime = i32_now;

  i32_demandedExecEndScheduler = ri32_demandedExecEndScheduler;
  #ifdef CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME
  if ( ri32_demandedExecEndScheduler < 0 )
  { // limit execution time, even if no limit was defined by caller - avoid deadlock due to overload
    i32_demandedExecEndScheduler = i32_stepStartTime + CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME;
    //i32_demandedExecEndScheduler = i32_stepStartTime + 250;

  }
  #endif
  // trigger the watchdog
  System_c::triggerWd();

  // check if immediate return is needed
  if ( getAvailableExecTime( i16_canExecTime ) == 0 )
  {
     // reset awaited can processing execution time to prevent dead lock
     i16_canExecTime = 0;
     return -1; //Client could not do his Job
  }

  // process CAN messages
  if ( getCanInstance4Comm().timeEvent() )
  { // all CAN_IO activities ready -> update statistic for CAN_IO
    i16_canExecTime = System_c::getTime() - i32_stepStartTime;
  }
  System_c::triggerWd();


  /* call EEEditor Process */
  #if defined(USE_CAN_EEPROM_EDITOR)
    // check if immediate return is needed
    if ( getAvailableExecTime() == 0 ) return -1; //Client could not do his Job
    HAL::ProcessCANEEEditorMsg();
  #elif defined(USE_RS232_EEPROM_EDITOR)
    // check if immediate return is needed
    if ( getAvailableExecTime() == 0 ) return -1; //Client could not do his Job
    HAL::ProcessRS232EEEditorMsg();
  #endif

  #if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  for ( uint8_t ind = 1; ind < CAN_INSTANCE_CNT; ind++ )
  { // process msg of other BUS ( other CAN is always at position 1 (independent from CAN BUS at controller!!)
    getCanInstance( ind ).timeEvent();
  }
  #endif
  System_c::triggerWd();
  /// Call timeEvent for next Client in TaskQueue as long as
  /// one Client returns idleTime=0 and Scheduler_c has still time;
  /// (i32_idleTime > 0)-> ends loop -> triggers still proccessmsg()
  int32_t  i32_idleTime;
  for ( i32_idleTime = selectCallTaskAndUpdateQueue();
        ((i32_idleTime  == 0) && (System_c::getTime() < i32_demandedExecEndScheduler));
        i32_idleTime = selectCallTaskAndUpdateQueue() )
  {
            System_c::triggerWd();
  }

  // check if all tasks are called
  if ( i32_idleTime > 0 )
  { // as we are in time, call CANIO_c::processMsg() if last CANIO_c::timeEvent
    System_c::triggerWd();
    // was used to process received messages
    // ==> allows to cope wiht high BUS loads
    if ( getAvailableExecTime() != 0 )
    { // simply call processMsg and don't update statistic
      getCanInstance4Comm().processMsg();
    }
    #if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
    if ( getAvailableExecTime() != 0 )
    { // simply call processMsg and don't update statistic
      // process msg of other BUS ( other CAN is always at position 1 (independent from CAN BUS at controller!!)
      getCanInstance( 1 ).processMsg();
    }
    #endif

    // timeEvent of all registered clients called -> update overall
    // time statistic
    if ( i32_averageExecTime != 0 )
      // use pseudo average time: take middle value between last and actual value
      i32_averageExecTime = ( i32_averageExecTime + ( System_c::getTime() - i32_lastTimeEventTime ) ) / 2;
    else
      i32_averageExecTime = ( System_c::getTime() - i32_lastTimeEventTime );

  }
   // trigger the watchdog
    System_c::triggerWd();

    ///return i32_idleTime to inform mainapplication when recall is needed;
    return i32_idleTime;
}

//!  resort from start of task list by swapping neighbour elements.
//!  Stop execution, if compared elements are in correct order.
//!  Avoid complex complete sort of list, if only the previously executed task must be placed in the correct position again - the rest of the list is still correct sorted.
void
Scheduler_c::resortTaskList()
{
  if(c_taskQueue.size() <= 1) return ; //nothing to sort

  std::list<SchedulerEntry_c>::iterator iterCompare = c_taskQueue.begin();
  // compare with the second item in list
  ++iterCompare;
  if ( c_taskQueue.front() <= *iterCompare )
  { // the first item is still having lower retrigger time -> no resort needed
    return;
  }

  // now in any case the current front item
  // (i.e. the one that has previously been executed and should now be re-entered
  //  in right place) has to be removed from front
  SchedulerEntry_c c_resortedFrontItem = c_taskQueue.front();
  c_taskQueue.pop_front();
  for ( iterCompare = c_taskQueue.begin();
        iterCompare != c_taskQueue.end();
        ++iterCompare )
  {
    if ( c_resortedFrontItem <= *iterCompare )
    {
      c_taskQueue.insert(iterCompare, c_resortedFrontItem);
      break;
    }
  }
  if ( iterCompare == c_taskQueue.end() )
  { // no other item in queue had later next retrigger time -> place c_resortedFrontItem at end
    c_taskQueue.push_back( c_resortedFrontItem );
  }


  #ifdef DEBUG_SCHEDULER
  static int32_t si32_lastTaskListTimesDebug = 0;
  /// for better debugging use > 100 or > 0 here
  if ( ( System_c::getTime() - si32_lastTaskListTimesDebug ) > 1000 ) {
      si32_lastTaskListTimesDebug = System_c::getTime();

    INTERNAL_DEBUG_DEVICE << "\n\nTaskList: Time to next Call after resort: ";
    printTaskList();
  }
  #endif
}

//!  This function  selects the next task(ElementBase_c Client), executes it and updates the task-list.
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
  IsoAgLib::iSystem_c::triggerWd();
  #ifdef DEBUG_SCHEDULER
  static bool b_firstCallSelectCallTastAndUpdateQueue = true;
  if ( b_firstCallSelectCallTastAndUpdateQueue ) {
    b_firstCallSelectCallTastAndUpdateQueue = false;
    INTERNAL_DEBUG_DEVICE << "Scheduler_c::selectCallTaskAndUpdateQueue()\n";
  }
  #endif
  // declare local timestamp vars as static to avoid repeating allocation
  static int32_t i32_idleTime, i32_endTime;

  i32_idleTime = c_taskQueue.front().getTimeToNextTrigger( retriggerType_t(EarliestRetrigger|StandardRetrigger) );
  i32_endTime = System_c::getTime();


  if ( i32_idleTime <= 0 )
  { // first task in queue can be executed
    STL_NAMESPACE::list<SchedulerEntry_c>::iterator pc_execIter = c_taskQueue.begin();
    #ifdef DEBUG_SCHEDULER
    setDebugTimeAccuracy( *pc_execIter );
    #endif

    // get max time to execution of next entry waiting
    STL_NAMESPACE::list<SchedulerEntry_c>::const_iterator pc_nextCallIter = pc_execIter;
    pc_nextCallIter++; /// go one further

    ///get Last Execution Time of next in Queue and ADD to actualTime
    const int32_t i32_nextTaskTriggerTimeSpread = pc_nextCallIter->getTimeToNextTrigger( LatestRetrigger );
    if(  i32_nextTaskTriggerTimeSpread   < 5 ){
    #ifdef DEBUG_SCHEDULER
    INTERNAL_DEBUG_DEVICE << "i32_endTime to small for " <<  pc_execIter->getTaskName() << "endTime "
      << (int) i32_nextTaskTriggerTimeSpread  << " Set to 5ms" << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
      i32_endTime += 5 ;  //add 5ms
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

    IsoAgLib::iSystem_c::triggerWd();
    #ifdef DEBUG_SCHEDULER_EXTREME
    INTERNAL_DEBUG_DEVICE << "Call timeevent of " <<  pc_execIter->getTaskName() << "endTime "
      << (int) i32_endTime << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    ///Call Clients timeEvent() via SchedulerEntry_c::timeEventExec()
    /// IF Client returns with false -> return i32_idleTime = -1
    /// because last Client could not finish in available TimeSpan
    const bool b_result = pc_execIter->timeEventExec( i32_endTime );
    if ( !b_result && ElementBase_c::getDemandedExecEnd() != getCentralSchedulerExecEndTime() )
    { // the executed task had not enough time and the limit was NOT defined by the central
      // scheduler end time, that was defined by the APP
      // --> reschedule this task _after_ the following task
      int16_t i16_avgTime = (pc_nextCallIter->getAvgExecTime() > 2)?pc_nextCallIter->getAvgExecTime():2;
      if ( pc_nextCallIter->getExecTime() > i16_avgTime ) i16_avgTime = pc_nextCallIter->getExecTime();
      #ifdef DEBUG_SCHEDULER
      INTERNAL_DEBUG_DEVICE
        << "+++++++++++++++++++++\nretrigger tie breaked task: " << pc_execIter->getTaskName() << " for time: "
        << (i16_avgTime+pc_nextCallIter->getNextTriggerTime()) << " so that task "
        << pc_nextCallIter->getTaskName()
        << " with AVG exect time of: " << pc_nextCallIter->getAvgExecTime()
        << " can start as planned\n" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      changeRetriggerTimeAndResort( pc_execIter, i16_avgTime+pc_nextCallIter->getNextTriggerTime() );
    }
    else if (b_result)
    { // executed task finished its work -> reschedule it by resort of task list
      resortTaskList();
    }
    IsoAgLib::iSystem_c::triggerWd();

    if(!b_result) return -1 ;
    // update idle time: first item is probably changed by sort
    i32_idleTime = c_taskQueue.begin()->getTimeToNextTrigger( retriggerType_t(EarliestRetrigger|StandardRetrigger) );
  }


  if ( i32_idleTime <= 0 ) return 0;
  #ifdef DEBUG_SCHEDULER
  setDebugIdleInformation( i32_idleTime );
  #endif

  return i32_idleTime;
}

#ifdef DEBUG_SCHEDULER
//!  Send debug messages with information on the
//!  acfuracy of time behaviour.
//!  Retrieve information about actual executed task from referenced SchedulerEntry_c.
//!  As long as time to next standard retrigger >= 0,
//!  everything is O.K.
//!  As long as latest execution would allow some idle time,
//!  the timing is acceptable.
//! Parameter:
//! @param refc_selectedTask: reference to the next executed task
void
Scheduler_c::setDebugTimeAccuracy(SchedulerEntry_c& refc_selectedTask)
{ // ~X2C
  static uint32_t sui32_lastDebugOutput = 0;
  static int16_t si16_maxDeviation = -32767,
                 si16_minDeviation = +32767;

  int16_t i16_timeDeviation = refc_selectedTask.getStdTimeToNextTrigger();
  bool b_correctTimeManagement = ( refc_selectedTask.getTimeToNextTrigger( LatestRetrigger ) >= 0  )?true:false;
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
      << " for Task: " << refc_selectedTask.getTaskName() /// todo implente  Taskschilds
      << ",\tMax earlier execution: " << si16_maxDeviation
      << ",\tMin earlier execution: " << si16_minDeviation << ",\tCorrect Time Management: "
    << "idle time of FRONT: " << c_taskQueue.front().getNextTriggerTime()
//    << "idle time of BACK: " << SystemManagement_c::instance().c_taskQueue.back().getNextTriggerTime()
      << b_correctTimeManagement << INTERNAL_DEBUG_DEVICE_ENDL;
    if ( i16_timeDeviation >= 0 )
    { // everything is good
      INTERNAL_DEBUG_DEVICE << "GOOD System state with task called "
        << i16_timeDeviation << "[ms] before the standard retrigger time (still not earlier than allowed)"
        << ", with typical exec time: " << refc_selectedTask.getExecTime()
        << ", and period: " << refc_selectedTask.getTimePeriod() << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    else if ( b_correctTimeManagement )
    { // real deviation -> a task is executed after the standard retrigger time
      // but before the latest allowed time
      INTERNAL_DEBUG_DEVICE << "Critical Scheduling state, as a task is called "
        << -1*i16_timeDeviation << "[ms] after the standard retrigger time (bot not later than allowed)."
        << ", with typical exec time: " << refc_selectedTask.getExecTime()
        << ", and period: " << refc_selectedTask.getTimePeriod() << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    else
    { // bad state
      INTERNAL_DEBUG_DEVICE << "BAD System state, as  "
        << i16_timeDeviation << "[ms] after the standard and latest allowed retrigger time, with typical exec time: "
        << refc_selectedTask.getExecTime()
        << ", and period: " << refc_selectedTask.getTimePeriod() << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    // print timing fingerprint of actual task
    INTERNAL_DEBUG_DEVICE
      << "AVG exec time: " << refc_selectedTask.getAvgExecTime()
      << ", MIN exec time: " << refc_selectedTask.getMinExecTime()
      << ", MAX exec time: " << refc_selectedTask.getMaxExecTime()
      << "\nAVG timing accuracy: " << refc_selectedTask.getAvgTimingAccuracy()
      << ", MIN timing accuracy: " << refc_selectedTask.getMinTimingAccuracy()
      << ", MAX timing accuracy: " << refc_selectedTask.getMaxTimingAccuracy()
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
//! @param rui16_idleTime:
void Scheduler_c::setDebugIdleInformation(uint16_t rui16_idleTime){

  static int32_t si32_lastRs232Send = 0;
  static uint16_t sui16_maxIdle = 0,
                  sui16_minIdle = 0xFFFF;

  int32_t i32_now = System_c::getTime();
  // update extreme times
  if ( rui16_idleTime > sui16_maxIdle ) sui16_maxIdle = rui16_idleTime;
  if ( rui16_idleTime < sui16_minIdle ) sui16_minIdle = rui16_idleTime;

  // debug msg every 30sec. (30.000 msec)
  if ( ( i32_now - si32_lastRs232Send ) > 30000 )
  { // ist's time to send debug data on RS232
    si32_lastRs232Send = i32_now;
    // send debug data on configured Device for Output
    INTERNAL_DEBUG_DEVICE << "setDebugIdleInformation()\n"
      << "Actual time: " << i32_now << ",\tMax IDLE time: " << sui16_maxIdle
      << ",\tMin IDLE time: " << sui16_minIdle
      << ",\tactual IDLE time: " << rui16_idleTime
      << "idle time of FRONT: " << c_taskQueue.front().getNextTriggerTime()
      //<< "idle time of BACK: " << c_taskQueue.back().getNextTriggerTime()
      << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  /// send debug msg if idle time is higher than allowed (longer than shortest period)
  if ( rui16_idleTime > 1001 )
  { // problem with scheduling
    INTERNAL_DEBUG_DEVICE << "setDebugIdleInformation() with too long idle of " << rui16_idleTime
      << ", TOP Task: " << c_taskQueue.front().getTaskName()
      << ", Now: " << System_c::getTime()
      << ", Retrigger Time: " << c_taskQueue.front().getNextTriggerTime()
      << INTERNAL_DEBUG_DEVICE_ENDL;

  }
}


void Scheduler_c::printTaskList()
{
  INTERNAL_DEBUG_DEVICE << "\n=========================\nPrintTaskList: ";
  for ( STL_NAMESPACE::list<SchedulerEntry_c>::iterator pc_test = c_taskQueue.begin();
        pc_test != c_taskQueue.end();
        pc_test++ )
  {
    if ( pc_test != c_taskQueue.begin() ) INTERNAL_DEBUG_DEVICE << ", ";
    INTERNAL_DEBUG_DEVICE << pc_test->getTimeToNextTrigger(  retriggerType_t(StandardRetrigger) ) << " name:"
      << pc_test->getTaskName() ;
  } //end for
  INTERNAL_DEBUG_DEVICE << "\n" << INTERNAL_DEBUG_DEVICE_ENDL;
}

#endif


//!  Calculate Delta from TimePeriod of a Client
//!  Scheduler_c set (NOW + delta of TimePeriod) as New Retrigger for Client
//!  and sort Task to the right Position in the TaskQueue
//! @param p_client -> Client in Scheduler_c TaskQueue
//! @param ri16_newTimePeriod -> New Period will set for the Client by Scheduler_c
bool Scheduler_c::changeTimePeriodAndResortTask(ElementBase_c * pc_client  , uint16_t rui16_newTimePeriod ){

  //Now calculate Delta and nextTriggerTime for Client
  int16_t i_deltaTime = rui16_newTimePeriod - pc_client->getTimePeriod()  ;
  int32_t i32_newTriggerTime = pc_client->getNextTriggerTime() + i_deltaTime;
  #ifdef DEBUG_SCHEDULER
    INTERNAL_DEBUG_DEVICE << "New TimePeriod:" << (int) rui16_newTimePeriod
    << " Old TimePeriod: "<<  pc_client->getTimePeriod() <<" Name"
    << pc_client->getTaskName() << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  ///Now lets do the resort  and update new TimePeriod in client
  return changeRetriggerTimeAndResort(pc_client,i32_newTriggerTime ,rui16_newTimePeriod);

}



//!  Uses Delta from TimePeriod of a Client
//!  to put a Task to the right Position in the TaskQueue
//!  ATTENTION parameter nextRetriggerTime will exactly used from Scheduler_c
//!  for call of timevent.-> so add e.g. an TimePeriod for an later call
//! @param rc_client -> Client in Scheduler_c TaskQueue
//! @param i32_nextRetriggerTime -> New i32_nextRetriggerTime set for Client by Scheduler_c
//! @param  ri16_newTimePeriod otpional -> New Period will set for the Client by Scheduler_c
bool  Scheduler_c::changeRetriggerTimeAndResort(SchedulerEntry_c rc_client  , int32_t i32_newRetriggerTime, int16_t ri16_newTimePeriod)
{
  if ( c_taskQueue.empty() ) return false;
  else if (c_taskQueue.size() == 1) return true;
  else
  { // search iterator for the client of change
    std::list<SchedulerEntry_c>::iterator itc_task;
    for(itc_task = c_taskQueue.begin(); itc_task != c_taskQueue.end(); itc_task++)
    {
      if (*itc_task == rc_client)
        return changeRetriggerTimeAndResort(itc_task, i32_newRetriggerTime, ri16_newTimePeriod);
    }
    // not found
    return false;
  }
}

//!  Uses Delta from i32_nextRetriggerTime of a Client
//!  to put a Task to the right Position in the TaskQueue
//!  ATTENTION parameter nextRetriggerTime will be exactly used from Scheduler_c
//!  for call of timevent.-> so add e.g. an TimePeriod for an later call
//! @param p_client -> Client in Scheduler_c TaskQueue
//! @param i32_nextRetriggerTime -> New i32_nextRetriggerTime set for Client by Scheduler_c
//! @param  ri16_newTimePeriod optional -> New Period will set for the Client by Scheduler_c
bool  Scheduler_c::changeRetriggerTimeAndResort(ElementBase_c * pc_client  , int32_t i32_newRetriggerTime, int16_t ri16_newTimePeriod)
{
  if ( c_taskQueue.empty() ) return false;
  else if (c_taskQueue.size() == 1) return true;
  else
  { // search iterator for the client of change
    std::list<SchedulerEntry_c>::iterator itc_task;
    for(itc_task = c_taskQueue.begin(); itc_task != c_taskQueue.end(); itc_task++)
    {
      if(itc_task->isTask(pc_client))
        return changeRetriggerTimeAndResort(itc_task, i32_newRetriggerTime, ri16_newTimePeriod);
    }
    // not found
    return false;
  }
}

//!  Uses Delta from i32_nextRetriggerTime of a Client
//!  to put a Task to the right Position in the TaskQueue
//!  ATTENTION parameter nextRetriggerTime will be exactly used from Scheduler_c
//!  for call of timevent.-> so add e.g. an TimePeriod for an later call
//! @param itc_task -> iterator to the task that should be changed
//! @param i32_nextRetriggerTime -> New i32_nextRetriggerTime set for Client by Scheduler_c
//! @param  ri16_newTimePeriod optional -> New Period will set for the Client by Scheduler_c
bool  Scheduler_c::changeRetriggerTimeAndResort(std::list<SchedulerEntry_c>::iterator itc_task, int32_t i32_newRetriggerTime, int16_t ri16_newTimePeriod)
{
  #ifdef DEBUG_SCHEDULER
  printTaskList();
  #endif

  #ifdef DEBUG_SCHEDULER
  INTERNAL_DEBUG_DEVICE << "Scheduler_c notifyed by Client:" << itc_task->getTaskName()
  << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  int32_t i32_deltaRetrigger = i32_newRetriggerTime - itc_task->getNextTriggerTime();

  //set New TimePeriod
  if(ri16_newTimePeriod > -1 ) itc_task->setTimePeriod(ri16_newTimePeriod);


  /// increase of RetriggerTime task should be called LATER
  if (i32_deltaRetrigger > 0) {
    #ifdef DEBUG_SCHEDULER
    INTERNAL_DEBUG_DEVICE << "task should be called LATER for ms: " << i32_deltaRetrigger
      << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    itc_task->changeNextTriggerTime( i32_newRetriggerTime );
    //remove to LATER Position
    STL_NAMESPACE::list<SchedulerEntry_c>::iterator itc_greater = itc_task;
    ++itc_greater;
    if ( itc_greater == c_taskQueue.end() )
    {
      #ifdef DEBUG_SCHEDULER
      printTaskList();
      #endif
      return true; ///< was already last element
    }
    else if ( *itc_task <= *itc_greater )
    { // there is no need to resort, as following item is still later
      #ifdef DEBUG_SCHEDULER
      printTaskList();
      #endif
      return true;
    }
    // from here on: the list has to be resorted
    for ( ++itc_greater; itc_greater != c_taskQueue.end(); ++itc_greater )
    {
      if ( *itc_task <= *itc_greater )
      { // the item at itc_greater has late enough retrigger time, so that
        // itc_task can be inserted before
        const SchedulerEntry_c c_move(*itc_task);
        c_taskQueue.erase( itc_task );
        c_taskQueue.insert( itc_greater, c_move );
        #ifdef DEBUG_SCHEDULER
        printTaskList();
        #endif
        return true;
      }
    }
    // we reach only here, when no other task in queue afterwards has later retrigger time
    // --> place it at end
    const SchedulerEntry_c c_move(*itc_task);
    c_taskQueue.erase( itc_task );
    c_taskQueue.push_back( c_move );
    #ifdef DEBUG_SCHEDULER
    printTaskList();
    #endif
  }
  /// decrease of TimePeriod task should be called EARLIER
  else if (i32_deltaRetrigger < 0){
    #ifdef DEBUG_SCHEDULER
    INTERNAL_DEBUG_DEVICE << "task should be called EARLIER for ms: " << i32_deltaRetrigger
      << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    ///set new NextTriggerTime to now to avoid delay of following tasks
    if((i32_newRetriggerTime < System_c::getTime())   ) i32_newRetriggerTime = System_c::getTime();

    ///set new NextTriggerTime
    itc_task->changeNextTriggerTime( i32_newRetriggerTime );
    //remove to EARLIER Position
    if ( itc_task == c_taskQueue.begin() )
    {
      #ifdef DEBUG_SCHEDULER
      printTaskList();
      #endif
      return true; ///< the changed task is already at start of queue
    }
    STL_NAMESPACE::list<SchedulerEntry_c>::iterator itc_smaller = itc_task;
    --itc_smaller;
    if ( *itc_smaller <= *itc_task )
    {
      #ifdef DEBUG_SCHEDULER
      printTaskList();
      #endif
      return true; ///< the changed task is still later scheduled than the item before it
    }
    // now we really have to resort
    for ( ; itc_smaller != c_taskQueue.begin(); --itc_smaller )
    {
      if ( *itc_smaller <= *itc_task )
      { // the item at itc_smaller has early enough retrigger time, so that
        // itc_task can be inserted afterwards
        ++itc_smaller; ///< the insertion can only take place _before_ an iterator
        const SchedulerEntry_c c_move(*itc_task);
        c_taskQueue.erase( itc_task );
        c_taskQueue.insert( itc_smaller, c_move );
        #ifdef DEBUG_SCHEDULER
        printTaskList();
        #endif
        return true;
      }
    }
    if ( ( itc_smaller == c_taskQueue.begin() ) && (*itc_smaller <= *itc_task))
    { // the first item in list has earlier trigger
      ++itc_smaller; ///< the insertion can only take place _before_ an iterator
      const SchedulerEntry_c c_move(*itc_task);
      c_taskQueue.erase( itc_task );
      c_taskQueue.insert( itc_smaller, c_move );
      #ifdef DEBUG_SCHEDULER
      printTaskList();
      #endif
      return true;
    }
    // we reach only here, when no other item before changed task has earlier retrigger time
    // -> place it at front of list
    const SchedulerEntry_c c_move(*itc_task);
    c_taskQueue.erase( itc_task );
    c_taskQueue.push_front( c_move );
  } // end if shift forward
  #ifdef DEBUG_SCHEDULER
  printTaskList();
  #endif
  return  true ;
}

} // end of namespace __IsoAgLib
