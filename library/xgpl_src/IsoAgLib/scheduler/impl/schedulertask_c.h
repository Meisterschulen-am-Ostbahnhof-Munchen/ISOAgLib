/*
  schedulertask_c.h: header for base class for all IsoAgLib member
    objects which stores the pointer to the IsoAgLib instance, and
    delivers some access features to other components of the system

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SCHEDULERTASK_C_H
#define SCHEDULERTASK_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/driver/system/impl/system_c.h>



// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

  // All Scheduler-Tasks are subsystems, because of
  // the not-possible multiple-inheritance right now
  class SchedulerTask_c : public Subsystem_c {
    public:
      SchedulerTask_c( int32_t nextTriggerTime, int32_t period, bool hardTiming );
      virtual ~SchedulerTask_c() {}


      /** this function is used by IsoAgLib components
        * to perform periodic timed tasks.  */
      virtual void timeEvent() = 0;

      int32_t getPeriod() const {
        return m_period;
      }

      /** sets a new period for this task
       * Note, that the task get scheduled from now + period */
      void setPeriod( int32_t a_period );

      int32_t getTimeToNextTrigger() const {
        return getNextTriggerTime() - System_c::getTime();
      }

      int32_t getNextTriggerTime() const {
        return m_nextTriggerTime;
      }

      void setNextTriggerTime( int32_t time );

      bool isRegistered() const {
        return m_registered;
      }

    private:
      friend class Scheduler_c;
      void setRegistered( bool r );
      void timeEventPre();
      void timeEventPost();

      bool m_hardTiming;
      bool m_nextTriggerTimeSet;
      bool m_registered;
      int32_t m_nextTriggerTime;
      int32_t m_period;
  };


  /** Proxy for SchedulerTask_c.
    * Having such a proxy as component is an alternative to subclassing
    * SchedulerTask_c directly.
    */
  template < typename OWNER_T >
  class SchedulerTaskProxy_c : public SchedulerTask_c {
    public:
      typedef OWNER_T Owner_t;

      SchedulerTaskProxy_c( Owner_t &art_owner ) : mrt_owner( art_owner ) {}

      virtual ~SchedulerTaskProxy_c() {}

    private:
      virtual void timeEvent() {
        mrt_owner.timeEvent();
      }

      // SchedulerTaskProxy_c shall not be copyable. Otherwise the
      // reference to the containing object would become invalid.
      SchedulerTaskProxy_c( SchedulerTaskProxy_c const & );

      SchedulerTaskProxy_c &operator=( SchedulerTaskProxy_c const & );

      Owner_t &mrt_owner;
  };

} // end namespace __IsoAgLib
#endif
