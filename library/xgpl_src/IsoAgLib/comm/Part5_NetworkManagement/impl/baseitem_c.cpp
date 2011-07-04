/*
  baseitem_c.cpp - base class for member lists; stores access time and
    pointer to root Scheduler_c object

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "baseitem_c.h"
#include <IsoAgLib/scheduler/impl/scheduler_c.h>

namespace __IsoAgLib {

/**
  constructor which takes optional the pointer to the containing Scheduler_c instance
  and the actual time as parameter to initialise all own values
  @param ai32_time optional timestamp to store as last update
  @param ab_status state of this ident (off, claimed address, ...) (default: off)
  @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
*/
BaseItem_c::BaseItem_c( int32_t ai32_time, IState_c::itemState_t ab_status, int ai_multitonInst)
  : IStateExt_c(ab_status, ai_multitonInst), mi32_lastTime(ai32_time)
{}

/**
  copy constructor which takes it initial values from another BaseItem_c instance
  @param acrc_baseItem reference to the source BaseItem_c instance
*/
BaseItem_c::BaseItem_c(const BaseItem_c& acrc_baseItem)
: IStateExt_c(acrc_baseItem), mi32_lastTime(acrc_baseItem.mi32_lastTime)
{}

/** destructor which sets the update timestamp to 0 */
BaseItem_c::~BaseItem_c(){
  mi32_lastTime = 0;
}

/**
  set pointer to containing Scheduler_c instance and update timestamp of object
  @param ai32_time optional timestamp to set as update time
  @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
*/
void BaseItem_c::set(int32_t ai32_time, int ai_multitonInst)
{
  if (ai32_time >= 0) mi32_lastTime = ai32_time;
  /** ai_multitonInst==-1 is special value to indicate, that the ai_multitonInst should not be changed */
  if (ai_multitonInst != -1) ClientBase::setMultitonInst(ai_multitonInst);
}

/**
  set pointer to containing Scheduler_c instance and update timestamp of object
  @param ai32_time optional timestamp to set as update time
  @param ab_status state of this ident (off, claimed address, ...) (default: off)
  @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
*/
void BaseItem_c::set(int32_t ai32_time, IState_c::itemState_t ab_status, int ai_multitonInst)
{
  set(  ai32_time,ai_multitonInst);
  // force clear of old item state
  setItemState( ab_status, true );
}

/** operator= which defines src as const to avoid
compile warnings with the automatic generated version */
BaseItem_c& BaseItem_c::operator=(const BaseItem_c& src){
  mi32_lastTime = src.mi32_lastTime;
  return *this;
}

/**
  calculates time between now and last set of mi32_lastTime;
  if called with time parameter, the difference to this is calculated,
  otherwise the system time is retreived and used
  @return lasted time between last update and the compare time [msec.]
*/
int32_t BaseItem_c::lastedTime( void ) const {
  return ( Scheduler_Task_c::getLastRetriggerTime() - mi32_lastTime );
}

/**
  check if given time intervall is lasted
  INFO: Interval is only an uint16. That's because of SPEED reasons
        for 16-bit platforms and of the not existing need to check
        for more than a minute and 5 seconds (65535 msec)
  @param aui16_timeInterval time intervall in msec
  @return true -> time last timestamp older than intervall
*/
bool BaseItem_c::checkTime(uint16_t aui16_timeInterval) const  {
  return ( lastedTime() >= aui16_timeInterval );
}

/**
  check if given time intervall is lasted;
  if time intervall is lasted - update time
  INFO: Interval is only an uint16. That's because of SPEED reasons
        for 16-bit platforms and of the not existing need to check
        for more than a minute and 5 seconds (65535 msec)
  @param aui16_timeInterval time intervall in msec
  @return true -> time last timestamp older than intervall
*/
bool BaseItem_c::checkUpdateTime(uint16_t aui16_timeInterval) {
  if ( checkTime( aui16_timeInterval ) ) {
    // enable constant time intervalls without growing time drift
	// -> simply add aui16_timeIntervall, if current time
	//    has max 10% deviation from correct timing
	const uint16_t cui16_maxDeviation = aui16_timeInterval / 10;
	if ( Scheduler_Task_c::getLastRetriggerTime() <= ( mi32_lastTime + aui16_timeInterval + cui16_maxDeviation ) ) {
	  // time correctness is close enough to increment last timestamp by exact
	  // aui16_timeIntervall -> avoid growing time drift if %e.g. each alive msg
	  // is triggered 5 msec too late
	  mi32_lastTime += aui16_timeInterval;
	}
	else {
	  // time difference is too big -> allow reset of timestamp
      mi32_lastTime = Scheduler_Task_c::getLastRetriggerTime();
	}
	return true;

  }
  else {
    return false;
  }
}

} // end of namespace __IsoAgLib
