/*
  schedulertask_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "schedulertask_c.h"

#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/scheduler/impl/scheduler_c.h>

namespace __IsoAgLib {

  SchedulerTask_c::SchedulerTask_c( int32_t nextTriggerTime, int32_t period, bool hardTiming )
    : Subsystem_c(),
      m_hardTiming( hardTiming ),
      m_nextTriggerTime( nextTriggerTime ),
      m_nextTriggerTimeSet( false ),
      m_period( period ),
      m_registered( false ) {}


  void SchedulerTask_c::setPeriod( int32_t a_period ) {
    m_period = a_period;
    setNextTriggerTime( System_c::getTime() + getPeriod() );
  }


  void SchedulerTask_c::setNextTriggerTime( int32_t time ) {
    m_nextTriggerTime = time;
    if( isRegistered() ) {
      getSchedulerInstance().rescheduleTask( *this );
    }
    m_nextTriggerTimeSet = true;
  }


  void SchedulerTask_c::timeEventPre() {
    m_nextTriggerTimeSet = false;
  }


  void SchedulerTask_c::timeEventPost() {

    if( m_nextTriggerTimeSet ) {
      // next trigger time is set during time event call
      // so we don't have to adjust our next trigger time here and
      // we signal to the scheduler, that he don't has to schedule
      // us cause this already happend within the setNextTriggerTime
      // call
      m_nextTriggerTimeSet = false;
    } else {
      // relax timing if not set to hard timing: calculate
      // the next trigger time from now and not from the time
      // we would have been theoretically called
      if( ! m_hardTiming ) {
        m_nextTriggerTime = System_c::getTime();
      }

      m_nextTriggerTime += getPeriod();
      while( m_nextTriggerTime < System_c::getTime() ) {
        isoaglib_assert( ! "we or someone else spent too much time that we would miss one call" );
        m_nextTriggerTime += getPeriod();
      }

      if( isRegistered() ) {
        getSchedulerInstance().rescheduleTask( *this );
      }
    }
  }


  void SchedulerTask_c::setRegistered( bool r ) {
    m_registered = r;
  }

} /// end of namespace
