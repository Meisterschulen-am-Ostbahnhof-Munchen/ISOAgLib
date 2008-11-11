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
#include <IsoAgLib/scheduler/impl/scheduler_c.h>

namespace __IsoAgLib {

/**
  constructor which takes optional the pointer to the containing Scheduler_c instance
  and the actual time as parameter to initialise all own values
  @param ai32_time optional timestamp to store as last update
  @param ab_status state of this ident (off, claimed address, ...) (default: off)
  @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
BaseItem_c::BaseItem_c( int32_t ai32_time, IState_c::itemState_t ab_status, int ai_singletonVecKey)
  : IStateExt_c(ab_status, ai_singletonVecKey), mi32_lastTime(ai32_time)
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
  @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
void BaseItem_c::set(int32_t ai32_time, int ai_singletonVecKey)
{
  if (ai32_time >= 0) mi32_lastTime = ai32_time;
  /** ai_singletonVecKey==-1 is special value to indicate, that the ai_singletonVecKey should not be changed */
  if (ai_singletonVecKey != -1) ClientBase::setSingletonKey(ai_singletonVecKey);
}

/**
  set pointer to containing Scheduler_c instance and update timestamp of object
  @param ai32_time optional timestamp to set as update time
  @param ab_status state of this ident (off, claimed address, ...) (default: off)
  @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
void BaseItem_c::set(int32_t ai32_time, IState_c::itemState_t ab_status, int ai_singletonVecKey)
{
  set(  ai32_time,ai_singletonVecKey);
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
  return ( lastedTime() >= aui16_timeInterval )?true:false;
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
