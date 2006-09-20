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

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
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
int32_t Scheduler_c::i32_demandedExecEnd = 0;

/** flag to detect, if other interrupting task forced immediated stop of Scheduler_c::timeEvent() */
bool Scheduler_c::b_execStopForced = false;


/** initialisation for the central IsoAgLib object */
void Scheduler_c::init( void )
{ // set attributes to valid initial state
  b_execStopForced = false;
  i16_canExecTime = 0;
  i32_averageExecTime = 0;
  i32_demandedExecEnd = 0;
  i32_lastTimeEventTime = 0;

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
  while ( ! c_arrClientC1.empty() )
  { // call close for each registered client
    pc_searchCacheC1 = c_arrClientC1.begin();
    if ( *pc_searchCacheC1 != NULL )
      (*pc_searchCacheC1)->close();
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
    getTracGeneralInstance4Comm().init(NULL, IsoAgLib::IdentModeImplement);
#endif
#ifdef USE_TRACTOR_MOVE
    getTracMoveInstance4Comm().init( NULL, IsoAgLib::IdentModeImplement );
#endif
#ifdef USE_TRACTOR_PTO
    getTracPtoInstance4Comm().init( NULL, IsoAgLib::IdentModeImplement );
#endif
#ifdef USE_TRACTOR_LIGHT
    getTracLightInstance4Comm().init(NULL, IsoAgLib::IdentModeImplement);
#endif
#ifdef USE_TRACTOR_AUX
    getTracAuxInstance4Comm().init(NULL, IsoAgLib::IdentModeImplement);
#endif
#ifdef USE_TIME_GPS
    getTimePosGpsInstance4Comm().init(NULL, IsoAgLib::IdentModeImplement);
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
  getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::Lbs );
}


/** register pointer to a new client
  * this function is called within construction of new client instance
  */
bool Scheduler_c::registerClient( ElementBase_c* pc_client)
{
  if ( registerC1( pc_client ) )
  { // new entry registered
    #ifdef DEBUG_HEAP_USEAGE
    sui16_clientPointerTotal++;
    sui16_clientTimeTotal++;

    getRs232Instance()
      << sui16_clientPointerTotal
      << "(" << c_arrClientC1.capacity()
      << ") x Scheduler_c Clients: Mal-Alloc: "
      << sizeVectorTWithMalloc( sizeof(void*), c_arrClientC1.capacity() )
      << "/" << sizeof(void*)
      << ", Chunk-Alloc: "
      << sizeVectorTWithChunk( sizeof(void*), c_arrClientC1.capacity() )
      << INTERNAL_DEBUG_DEVICE_ENDL
      << sui16_clientTimeTotal
      << "(" << arrExecTime.capacity()
      << ") x Execution Times: Mal-Alloc: "
      << sizeVectorTWithMalloc( sizeof(uint16_t), arrExecTime.capacity() )
      << "/" << sizeof(uint16_t)
      << ", Chunk-Alloc: "
      << sizeVectorTWithChunk( sizeof(uint16_t), arrExecTime.capacity() )
      #ifdef MASSERT
      << "\r\n__mall tot:" << AllocateHeapMalloc
      << ", _mall deal tot: " << DeallocateHeapMalloc
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
      #else
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
    #endif
    // bring time array and client array in sync
    while ( c_arrClientC1.size() > arrExecTime.size() )
    { // delete last element
      arrExecTime.push_back( 0 );
    }
    for ( unsigned int index = 0; index < arrExecTime.size() ; index++ )
    { // reset all entrys to 0 after set of clients is changed
      arrExecTime[index] = 0;
    }

    // reset timeEvent iterators
    pc_timeEventTimeIter = arrExecTime.begin();
    pc_timeEventClientIter = c_arrClientC1.begin();
    return true;
  }
  else return false;
}


/** unregister pointer to a already registered client
  * this function is called within destruction of new client instance
  */
void Scheduler_c::unregisterClient( ElementBase_c* pc_client)
{
  // reset timeEvent iterators
  unregisterC1( pc_client );

  // bring time array and client array in sync
  while ( c_arrClientC1.size() < arrExecTime.size() )
  { // delete last element
    #ifdef DEBUG_HEAP_USEAGE
    sui16_clientPointerTotal--;
    sui16_clientTimeTotal--;

    getRs232Instance()
      << sui16_clientPointerTotal
      << "(" << c_arrClientC1.capacity()
      << ") x Scheduler_c Clients: Mal-Alloc: "
      << sizeVectorTWithMalloc( sizeof(void*), c_arrClientC1.capacity() )
      << "/" << sizeof(void*)
      << ", Chunk-Alloc: "
      << sizeVectorTWithChunk( sizeof(void*), c_arrClientC1.capacity() )
      << INTERNAL_DEBUG_DEVICE_ENDL
      << sui16_clientTimeTotal
      << "(" << arrExecTime.capacity()
      << ") x Execution Times: Mal-Alloc: "
      << sizeVectorTWithMalloc( sizeof(uint16_t), arrExecTime.capacity() )
      << "/" << sizeof(uint16_t)
      << ", Chunk-Alloc: "
      << sizeVectorTWithChunk( sizeof(uint16_t), arrExecTime.capacity() )
      #ifdef MASSERT
      << "\r\n__mall tot:" << AllocateHeapMalloc
      << ", _mall deal tot: " << DeallocateHeapMalloc
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
      #else
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
    #endif

    arrExecTime.pop_back();
  }
  for ( unsigned int index = 0; index < arrExecTime.size() ; index++ )
  { // reset all entrys to 0 after set of clients is changed
    arrExecTime[index] = 0;
  }
  pc_timeEventTimeIter = arrExecTime.begin();
  pc_timeEventClientIter = c_arrClientC1.begin();
}

/** deliver available time for time event
  * @param ri16_awaitedExecTime optional awaited execution time of planned next step
           ==> answer of this function tells, if planned step will fit into time frame
  */
int16_t Scheduler_c::getAvailableExecTime( int16_t ri16_awaitedExecTime )
{ // check if b_execStopForced is set
  if ( b_execStopForced ) return 0;
  // values < 0 for i32_demandedExecEnd means unrestricted execution time
  if ( i32_demandedExecEnd >= 0 )
  {
    int16_t i16_result = int32_t( i32_demandedExecEnd - System_c::getTime() );
    if ( ( i32_demandedExecEnd > System_c::getTime() ) && ( i16_result < 0 ) )
    { // overflow as we have TOOO much time to execute -> set to biggest possible time
      i16_result = 0x7FFF;
    }
    // if awaited time for next planned step is longer than available time
    // answer 0 to indicate need for immediate return from timeEvent
    if ( i16_result < ri16_awaitedExecTime ) return 0;
    return i16_result;
  }
  return -1;
}


/**
  call the timeEvent for CANIO_c and all communication classes (derived from ElementBase_c) which
  registered within Scheduler_c for periodic timeEvent.
  Define common trigger timestamp, so that distributed activities can be performed with
  common time base.
  @param ri32_demandedExecEnd optional timestamp, where timeEvent shall return execution to calling function
         -> allow tight integration of IsoAgLib into application specific scheduler, as In-Time execution is
         guaranteed (default -1 -> no execution stop defined)
  @return true -> all planned executions performed
*/
bool Scheduler_c::timeEvent( int32_t ri32_demandedExecEnd )
{ // first check if demanded exec time allows execution
  // update last trigger time
  int32_t i32_now = System_c::getTime();
  int32_t i32_stepStartTime = i32_lastTimeEventTime = i32_now;

  i32_demandedExecEnd = ri32_demandedExecEnd;
  #ifdef CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME
  if ( ri32_demandedExecEnd < 0 )
  { // limit execution time, even if no limit was defined by caller - avoid deadlock due to overload
    i32_demandedExecEnd = i32_stepStartTime + CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME;
  }
  #endif
  // trigger the watchdog
  System_c::triggerWd();
  // define flag to detect amount of executed tasks
  uint8_t ui8_execCnt = 0;
  // check if immediate return is needed
  if ( getAvailableExecTime( i16_canExecTime ) == 0 )
  {
     // reset awaited can processing execution time to prevent dead lock
     i16_canExecTime = 0;
     return false;
  }
  // FIRST give the SystemMgmt_c class a chance to do periodic actions
  // like sending alive messages
  getIsoMonitorInstance4Comm().timeEvent();

  // process CAN messages
  if ( getCanInstance4Comm().timeEvent() )
  { // all CAN_IO activities ready -> update statistic for CAN_IO
    i16_canExecTime = System_c::getTime() - i32_stepStartTime;
  }
  System_c::triggerWd();

  /* call EEEditor Process */
  #if defined(USE_CAN_EEPROM_EDITOR)
    // check if immediate return is needed
    if ( getAvailableExecTime() == 0 ) return false;
    HAL::ProcessCANEEEditorMsg();
  #elif defined(USE_RS232_EEPROM_EDITOR)
    // check if immediate return is needed
    if ( getAvailableExecTime() == 0 ) return false;
    HAL::ProcessRS232EEEditorMsg();
  #endif

  #if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  for ( uint8_t ind = 1; ind < CAN_INSTANCE_CNT; ind++ )
  { // process msg of other BUS ( other CAN is always at position 1 (independent from CAN BUS at controller!!)
    getCanInstance( ind ).timeEvent();
  }
  #endif
  System_c::triggerWd();

  // run timeEvent for each registered clients as long as demanded exec end is not
  // reached
  #if 0
  // very sensitive variant - something with exec time calc seems to be wrong
  for ( uint8_t ui8_ind = 0;
        ( ( ui8_ind < c_arrClientC1.size() ) && ( getAvailableExecTime( *pc_timeEventTimeIter ) != 0 ) );
        ui8_ind++ )
  #else
  for ( uint8_t ui8_ind = 0;
        ( ( ui8_ind < c_arrClientC1.size() ) && ( getAvailableExecTime() != 0 ) );
        ui8_ind++ )
  #endif
  { // update step time for individual time statistic
    System_c::triggerWd();
    i32_stepStartTime = System_c::getTime();
    if ( (*pc_timeEventClientIter)->timeEvent() )
    { // timeEvent of actual client performed all activities in time
      // update individual time statistic
      *pc_timeEventTimeIter = ( System_c::getTime() - i32_stepStartTime );
      ui8_execCnt++;
    }
    // set iterators to next elem - check for end of vector to restart at begin
    pc_timeEventTimeIter++;
    if ( pc_timeEventTimeIter == arrExecTime.end() ) pc_timeEventTimeIter = arrExecTime.begin();
    pc_timeEventClientIter++;
    if ( pc_timeEventClientIter == c_arrClientC1.end() ) pc_timeEventClientIter = c_arrClientC1.begin();
  }

  // check if all tasks are called
  if ( ui8_execCnt == c_arrClientC1.size() )
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

    // trigger the watchdog
    System_c::triggerWd();
    // return true as indicator that all planned tasks are performed
    return true;
  }
  else
  { // trigger the watchdog
    System_c::triggerWd();
    return false;
  }
}

} // end of namespace __IsoAgLib
