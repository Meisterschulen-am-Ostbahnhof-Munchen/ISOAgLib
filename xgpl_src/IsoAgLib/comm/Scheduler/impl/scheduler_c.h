/***************************************************************************
                          scheduler_c.h  -  header for central hardware independent
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
#ifndef SCHEDULER_H
#define SCHEDULER_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/elementbase_c.h>

#include <vector>

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
class Scheduler_c : public SINGLETON_CLIENT1( Scheduler_c, ElementBase_c, uint8_t ) {
public:

  /** initialisation for the central IsoAgLib object */
  void init( void );

  /** destructor for Scheduler_c */
  ~Scheduler_c() { close();}

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void );
  /** simply close communicating clients */
  void closeCommunication( void );

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
  bool timeEvent( int32_t ri32_demandedExecEnd = -1);

  /**
    * deliver the average execution time for timeEvent calls -> allows scheduler to
    * refine time schedule within execution
    * @return average execution time in [msec] (off complete performed runs)
    */
  uint16_t getExecTime( void ) const { return i32_averageExecTime;}

  /**
    * if a very imprtant IRQ event forces stop of Scheduler_c::timeEvent AS SOON AS POSSIBLE
    * the IRQ handler can call Scheduler_c::forceExecStop().
    * Therefore the only smallest set of atomic activities is performed, even if timeEvent
    * is WITHIN execution in the main task. This way, the IsoAgLib is leaved by Scheduler_c::timeEvent()
    * in a guaranteed WELL DEFINED and VALID state.
    */
  static void forceExecStop( void ) {b_execStopForced = true;i32_demandedExecEnd = 0;}

  /**
    * informative function for all IsoAgLib subsystems which are triggered by Scheduler_c::timeEvent to
    * detect, if another task forced immediated stop of timeEvent
    * @return true -> immediate stop is forced
    */
  static bool isExecStopForced( void ) { return b_execStopForced;}

  /** get last timestamp of Scheduler_c::timeEvent trigger */
  static int32_t getLastTimeEventTrigger( void ) { return i32_lastTimeEventTime;}

  /** deliver available time for time event
    * @param ri16_awaitedExecTime optional awaited execution time of planned next step
             ==> answer of this function tells, if planned step will fit into time frame
    */
  static int16_t getAvailableExecTime( int16_t ri16_awaitedExecTime = 0 );

  /** handler function for access to undefined client.
    * the base Singleton calls this function, if it detects an error
    */
  void registerAccessFlt( void );

  /** register pointer to a new client
    * this function is called within construction of new client instance
    */
  bool registerClient( ElementBase_c* pc_client);

  /** unregister pointer to a already registered client
    * this function is called within destruction of new client instance
    */
  void unregisterClient( ElementBase_c* pc_client);

private: //Private methods
  friend class SINGLETON( Scheduler_c );
  /** constructor for the central IsoAgLib object */
  Scheduler_c() {};

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit() { init(); };

private: // Private attributes
	#ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  /** vector of execution times for all registered timeEvent clients */
  STL_NAMESPACE::vector<int16_t,STL_NAMESPACE::__malloc_alloc_template<0> > arrExecTime;

  /** iterator to continue each timeEvent after end of last timeEvent call
      -> cache in execution time vector
    */
  STL_NAMESPACE::vector<int16_t,STL_NAMESPACE::__malloc_alloc_template<0> >::iterator pc_timeEventTimeIter;

  /** iterator to continue each timeEvent after end of last timeEvent call
      -> cache in client vector
    */
  STL_NAMESPACE::vector<ElementBase_c*,STL_NAMESPACE::__malloc_alloc_template<0> >::iterator pc_timeEventClientIter;
	#else

  /** vector of execution times for all registered timeEvent clients */
  STL_NAMESPACE::vector<int16_t> arrExecTime;

  /** iterator to continue each timeEvent after end of last timeEvent call
      -> cache in execution time vector
    */
  STL_NAMESPACE::vector<int16_t>::iterator pc_timeEventTimeIter;

  /** iterator to continue each timeEvent after end of last timeEvent call
      -> cache in client vector
    */
  STL_NAMESPACE::vector<ElementBase_c*>::iterator pc_timeEventClientIter;
	#endif


  /** timestamp where last timeEvent was called -> can be used to synchronise distributed timeEvent activities */
  static int32_t i32_lastTimeEventTime;

  /** commanded timestamp, where Scheduler_c::timeEvent MUST return action to caller */
  static int32_t i32_demandedExecEnd;

  /** average execution time for Scheduler_c::timeEvent */
  int32_t i32_averageExecTime;

  /** execution time of last call of CANIO_c::timeEvent() */
  int16_t i16_canExecTime;

  /** flag to detect, if other interrupting task forced immediated stop of Scheduler_c::timeEvent() */
  static bool b_execStopForced;

};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Scheduler_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  Scheduler_c& getSchedulerInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique Scheduler_c singleton instance */
  Scheduler_c& getSchedulerInstance( void );
#endif
}

#endif
