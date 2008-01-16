/***************************************************************************
                          ischedulertask_c.h  -  header for central hardware independent
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
#ifndef ISCHEDULERTASK_H
#define ISCHEDULERTASK_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/schedulertask_c.h"
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/Part3_DataLink/icanpkgext_c.h>

/// Begin Namespace IsoAgLib
namespace IsoAgLib {




/**
  Central class for all other IsoAgLib classes.
  Only define one instance of iSchedulerTask_c per
  thread, and the other classes are
  instanziated as members of this iSchedulerTask_c class.
  The member objects represent the different scopes of functions of IsoAgLib.
  @author Dipl.-Inform. Achim Spangler
  @short central manager object for all hardware independent IsoAgLib objects.
*/
class iSchedulerTask_c : private __IsoAgLib::Scheduler_Task_c {
public:

  /** Constructor */
  iSchedulerTask_c(int ai_key = 0);

  /** Destructor */
  ~iSchedulerTask_c();

  virtual void close( void ) = 0;

  //!  deliver the time [msec] to the next awaited retrigger of this task. The central scheduler can use the parameter to deduce the possible time
  //!  interval, which is important, if some tasks have time problems due to high load (data processing, IsoAgLib).
  //!  @return <0 -> too late, =0 -> trigger now, >0 -> wait
  //! Parameter:
  //! @param t_retriggerType: Bit-OR combination of [earliest|standard|latest]
  virtual int32_t getTimeToNextTrigger(__IsoAgLib::retriggerType_t t_retriggerType= __IsoAgLib::StandardRetrigger) const;



  /** this function is used by IsoAgLib components
    * to perform periodic timed tasks.
    * The central function Scheduler_c::timeEvent()
    * calls the timeEvent() function of all
    * IsoAgLib subsystem classes which are derived
    * from Scheduler_Task_c and which registered
    * in the list of Scheduler_c during init
    * Call is wrapped by SchedulerEntry_c for Update of timestamps
    * @return true -> all planned activities performed in available time
    */
  virtual bool timeEvent( void ) = 0;


 /** Method getTimePeriod */
  uint16_t getTimePeriod() const {return Scheduler_Task_c::getTimePeriod();}


  /** interface function to change the next trigger time
      Uses Delta from TimePeriod of a Client
      to put a Task to the right Position in the TaskQueue
      ATTENTION parameter nextRetriggerTime will exactly used from Scheduler_c
      for call of timevent.-> so add e.g. an TimePeriod for an later call
      @param i32_nextRetriggerTime -> New i32_nextRetriggerTime set for Client by Scheduler_c
      @param  ai16_newTimePeriod otpional -> New Period will set for the Client by Scheduler_c
  */
  bool  changeRetriggerTime(int32_t i32_nextRetriggerTime, int16_t ai16_newTimePeriod = -1)
    { return __IsoAgLib::getSchedulerInstance4Comm().changeRetriggerTimeAndResort( this, i32_nextRetriggerTime, ai16_newTimePeriod );}


   //  Operation: startTaskTiming
  //!  this function is called at the end of system init, to set the trigger times to a suitable and realizable
  //!  start state, so that the scheduler can find
  //!  a correct calling order.
  //!  this is needed cause of long initial tests (esp.
  //!  single device power switch test with 5
  //!  devices with 1 second per device)
  //! Parameter:
  //! @param aint32_StartTaskTime: individual time offset, to avoid concurring tasks (if starting at same time with same period, the scheduler has every round a time problem)

  void startTaskTiming(int32_t aint32_StartTaskTime){return Scheduler_Task_c::startTaskTiming(aint32_StartTaskTime);}

//! Used for Debugging Tasks in Scheduler_c
 virtual const char* getTaskName() const = 0;


  //! virtual function which allows a scheduler client to define
  //! a minimum execution time, that should be saved after this item in the
  //! scheduler loop - some tasks might not be able to finish any sensible
  //! work in the default min exec time of 5msec
 virtual uint16_t getForcedMinExecTime() const;


  //  Operation: getAvailableExecTime
  //!  Deliver the available execution time for actual timeEvent() run.
  //!  As the variables for decision on result of this function are static, this function can be also static.
  //!  Therefore sub-elements of the actual working
  //!  Sub-System can simply get informaiton
  //!  on available time.
  //!  @return 0 == immediate return is forced, <0 == unrestricted time, else available time in [msec]

  int16_t getAvailableExecTime() {return Scheduler_Task_c::getAvailableExecTime();}

 //!  Deliver the registered exec end timestamp.

 int32_t getDemandedExecEnd(){return Scheduler_Task_c::getDemandedExecEnd();}

//!  Sets the registered exec end timestamp.
 void setDemandedExecEnd(int32_t ai32_demandenExecTime) {return Scheduler_Task_c::setDemandedExecEnd(ai32_demandenExecTime); }

 //!  Operation: getLastRetriggerTime
  int32_t getLastRetriggerTime(){return Scheduler_Task_c::getLastRetriggerTime();}

  //! delay the next execution time by given period
  void delayNextTriggerTime( unsigned int aui_delay ) { return Scheduler_Task_c::delayNextTriggerTime(aui_delay);}

  //! delay the next retrigger time to the given timestamp
  void delayNextTriggerTimeToTime( int32_t ai32_newNextRetrigger ) { return Scheduler_Task_c::delayNextTriggerTimeToTime(ai32_newNextRetrigger);}

  //! Function set mui16_earlierInterval and
  //! ui16_laterInterval that will be used by
  //! getTimeToNextTrigger(retriggerType_t)
  //! can be overloaded by Childclass for special condition
  virtual void updateEarlierAndLatestInterval();

 /**
    virtual function which delivers a pointer to the iCANCustomer
    specific iCanPkgExt_c instance.
    Default implementation of this function "borrows" the CanPkgExt_c instance of the corresponding
    IsoMonitor_c singleton class.
    This function has only to be overloaded by a derived class, if the application wants to use a special
    derived version from iCanPkgExt_c for special sting2flags() and flags2string() functionality.
  */
  virtual iCanPkgExt_c& iDataBase();

  /**
    process a message -> the specialized/derived version of this virtual
    function is called during processing of received CAN telegrams in CanIo_c::processMsg
    @param apc_box pointer to the FilterBox_c instances which received the telegram (i.e. which has the telegram in its buffer)
    @see __IsoAgLib::CanIo_c::processMsg
  */
  virtual bool processMsg();

 /**
    process a message -> the specialized/derived version of this virtual
    function can be called during processing of received CAN telegrams in CanIo_c::processMsg
    @param apc_box pointer to the FilterBox_c instances which received the telegram (i.e. which has the telegram in its buffer)
    @see __IsoAgLib::CanIo_c::processMsg
  */
  virtual bool processInvalidMsg();

  virtual bool isNetworkMgmt() const;


  /** return the protocol instance */

  int getSingletonVecKey() const { return mi_key;}

  private:

  /**protocol instance */
  int mi_key;

  /** gives to iCanio_c the permission to look inside the container, useful for
   the casting between canCustomer_c and iSchedulerTask_c */
  friend class iCanIo_c;
  friend class iIsoFilter_s;
  friend class iMultiReceive_c;

};


}

#endif
