/*
  ischedulertask_c.h: header for central hardware independent object
    which manages all functional IsoAgLib member objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISCHEDULERTASK_H
#define ISCHEDULERTASK_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/schedulertask_c.h"
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/Part3_DataLink/icanpkgext_c.h>

/// Begin Namespace IsoAgLib
namespace IsoAgLib {


/** Abstract base class for tasks to be scheduled by IsoAgLib. Its
  * subclasses have to provide some methods including timeEvent, that
  * will be called periodically.
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


  /** This method has to be overridden so that IsoAgLib's scheduler
    * calls it periodically. (There's no need to register at the
    * scheduler, because this is done implicitly by iSchedulerTask_c's
    * constructor.)
    * 
    * \return Status.
    * \retval false The activities could not be performed in the available time.
    * \retval true The activities could be performed in the available time.
    */
  virtual bool timeEvent() = 0;


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

  /** clear mb_alreadyClosed so that close() can be called one time */
  void clearAlreadyClosed( void ) { Scheduler_Task_c::clearAlreadyClosed(); }

  /** set mb_alreadyClosed so that close() can't be called another time */
  void setAlreadyClosed( void ) { Scheduler_Task_c::setAlreadyClosed(); }

  /** check mb_alreadyClosed to decide if close() can be called */
  bool checkAlreadyClosed( void ) const { return Scheduler_Task_c::checkAlreadyClosed(); }

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

  /** return the protocol instance */

  int getSingletonVecKey() const { return mi_key;}

protected:

  /**  Operation: setTimePeriod
    * Set client specific time period between calls of timeEvent.
    * Each from Scheduler_Task_c derived class must set at its INIT
    * or can use it to set other Time Period in timeEvent()
    * Do NOT call from outside (e.g. processMsg)
    * Parameter:
    * @param aui16_timePeriod: needed time between calls of timeEvent in [msec]
    */
  void setTimePeriod (uint16_t aui16_timePeriod) { return Scheduler_Task_c::setTimePeriod( aui16_timePeriod ); }

  /** Operation: setEarlierInterval
   * Set lowest time interval that can be handled by the application. The execution time period of a task is within the range
   * of ( timePeriod - earlierInterval ) <= timePeriod <= (timePeriod + latestInterval )
   * Parameter:
   * @param uint16_t aui16_interval: smallest possible interval between two time Events (timePeriod - aui16_interval) [msec]
   */
  void setEarlierInterval( uint16_t aui16_interval ) { Scheduler_Task_c::setEarlierInterval( aui16_interval ); }

  /** Operation: setLatestInterval
   * Set latest time interval that can be handled by the application. The time period of a task is within the range
   * of ( timePeriod - earlierInterval ) <= timePeriod <= (timePeriod + latestInterval )
   * Parameter:
   * @param uint16_t aui16_interval: biggest possible interval between two time Events (timePeriod + aui16_interval) [msec]
   */
  void setLatestInterval( uint16_t aui16_interval ) { Scheduler_Task_c::setLatestInterval( aui16_interval ); }

  private:

  /**protocol instance */
  int mi_key;

  /** gives to iCanio_c the permission to look inside the container, useful for
   the casting between canCustomer_c and iSchedulerTask_c */
  friend class iCanIo_c;
  friend struct iIsoFilter_s;
  friend class iMultiReceive_c;
  friend class iScheduler_c;

};


}

#endif
