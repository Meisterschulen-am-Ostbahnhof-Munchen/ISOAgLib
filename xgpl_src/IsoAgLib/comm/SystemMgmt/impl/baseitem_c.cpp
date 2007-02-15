/***************************************************************************
                          baseitem_c.cpp - base class for member lists;
                                    stores access time
                                    and pointer to root Scheduler_c object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#include "baseitem_c.h"
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>

namespace __IsoAgLib {

/**
  constructor which takes optional the pointer to the containing Scheduler_c instance
  and the actual time as parameter to initialise all own values
  @param ri32_time optional timestamp to store as last update
  @param rb_status state of this ident (off, claimed address, ...) (default: off)
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
BaseItem_c::BaseItem_c( int32_t ri32_time, IState_c::itemState_t rb_status, int ri_singletonVecKey)
  : IStateExt_c(rb_status, ri_singletonVecKey), i32_lastTime(ri32_time)
{}

/**
  copy constructor which takes it initial values from another BaseItem_c instance
  @param rrefc_baseItem reference to the source BaseItem_c instance
*/
BaseItem_c::BaseItem_c(const BaseItem_c& rrefc_baseItem)
: IStateExt_c(rrefc_baseItem), i32_lastTime(rrefc_baseItem.i32_lastTime)
{}

/** destructor which sets the update timestamp to 0 */
BaseItem_c::~BaseItem_c(){
  i32_lastTime = 0;
}

/**
  set pointer to containing Scheduler_c instance and update timestamp of object
  @param ri32_time optional timestamp to set as update time
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
void BaseItem_c::set(int32_t ri32_time, int ri_singletonVecKey)
{
  if (ri32_time >= 0) i32_lastTime = ri32_time;
  /** @todo Put this check into ClientBase itself??? */
  if (ri_singletonVecKey != -1) ClientBase::setSingletonKey(ri_singletonVecKey);
}

/**
  set pointer to containing Scheduler_c instance and update timestamp of object
  @param ri32_time optional timestamp to set as update time
  @param rb_status state of this ident (off, claimed address, ...) (default: off)
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
void BaseItem_c::set(int32_t ri32_time, IState_c::itemState_t rb_status, int ri_singletonVecKey)
{
  set(  ri32_time,ri_singletonVecKey);
  // force clear of old item state
  setItemState( rb_status, true );
}

/** operator= which defines src as const to avoid
compile warnings with the automatic generated version */
BaseItem_c& BaseItem_c::operator=(const BaseItem_c& src){
  i32_lastTime = src.i32_lastTime;
  return *this;
}

/**
  calculates time between now and last set of i32_lastTime;
  if called with time parameter, the difference to this is calculated,
  otherwise the system time is retreived and used
  @return lasted time between last update and the compare time [msec.]
*/
int32_t BaseItem_c::lastedTime( void ) const {
  return ( ElementBase_c::getLastRetriggerTime() - i32_lastTime );
}

/**
  check if given time intervall is lasted
  INFO: Interval is only an uint16. That's because of SPEED reasons
        for 16-bit platforms and of the not existing need to check
        for more than a minute and 5 seconds (65535 msec)
  @param rui16_timeInterval time intervall in msec
  @return true -> time last timestamp older than intervall
*/
bool BaseItem_c::checkTime(uint16_t rui16_timeInterval) const  {
  return ( lastedTime() >= rui16_timeInterval )?true:false;
}

/**
  check if given time intervall is lasted;
  if time intervall is lasted - update time
  INFO: Interval is only an uint16. That's because of SPEED reasons
        for 16-bit platforms and of the not existing need to check
        for more than a minute and 5 seconds (65535 msec)
  @param rui16_timeInterval time intervall in msec
  @return true -> time last timestamp older than intervall
*/
bool BaseItem_c::checkUpdateTime(uint16_t rui16_timeInterval) {
  if ( checkTime( rui16_timeInterval ) ) {
    // enable constant time intervalls without growing time drift
	// -> simply add rui16_timeIntervall, if current time
	//    has max 10% deviation from correct timing
	const uint16_t cui16_maxDeviation = rui16_timeInterval / 10;
	if ( ElementBase_c::getLastRetriggerTime() <= ( i32_lastTime + rui16_timeInterval + cui16_maxDeviation ) ) {
	  // time correctness is close enough to increment last timestamp by exact
	  // rui16_timeIntervall -> avoid growing time drift if %e.g. each alive msg
	  // is triggered 5 msec too late
	  i32_lastTime += rui16_timeInterval;
	}
	else {
	  // time difference is too big -> allow reset of timestamp
      i32_lastTime = ElementBase_c::getLastRetriggerTime();
	}
	return true;

  }
  else {
    return false;
  }
}

} // end of namespace __IsoAgLib
