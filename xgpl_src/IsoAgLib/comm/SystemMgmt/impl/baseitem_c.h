/***************************************************************************
                          baseitem_c.h  - base class for member lists;
                                    stores access time
                                    and pointer to root Scheduler_c object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
#ifndef BASE_ITEM_H
#define BASE_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include "istate_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  base class for member lists; stores access time
  and pointer to root Scheduler_c object
  Derive from CloientBase to get management of IsoAgLib instance, in case more than one
  CAN BUS is used for IsoAgLib.
  @author Dipl.-Inform. Achim Spangler
*/
class BaseItem_c  : public IStateExt_c {
public:
  /**
    constructor which takes optional the pointer to the containing Scheduler_c instance
    and the actual time as parameter to initialise all own values
    @param ri32_time optional timestamp to store as last update
    @param rb_status state of this ident (off, claimed address, ...) (default: off)
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  BaseItem_c( int32_t ri32_time = 0, IState_c::itemState_t rb_status = IState_c::IstateNull, int ri_singletonVecKey = 0);

  /**
    copy constructor which takes it initial values from another BaseItem_c instance
    @param rrefc_baseItem reference to the source BaseItem_c instance
  */
  BaseItem_c(const BaseItem_c& rrefc_baseItem);

  /** destructor which sets the update timestamp to 0 */
  ~BaseItem_c();

  /**
    operator= which defines src as const to avoid
    compile warnings with the automatic generated version
    @param rrefc_src reference to source BaseItem_c to copy values from
    @return reference to the source BaseItem_c instance for assign chains like "base1 = base2 = base3;"
  */
  BaseItem_c& operator=(const BaseItem_c& src);

  /**
    set pointer to containing Scheduler_c instance and update timestamp of object
    @param ri32_time optional timestamp to set as update time
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void set(int32_t ri32_time = -1, int ri_singletonVecKey = 0);

  /**
    set pointer to containing Scheduler_c instance and update timestamp of object
    @param ri32_time optional timestamp to set as update time
    @param rb_status state of this ident (off, claimed address, ...) (default: off)
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void set(int32_t ri32_time, IState_c::itemState_t rb_status, int ri_singletonVecKey = 0);

  /**
    calculates time between now and last set of i32_lastTime;
    if called with time parameter, the difference to this is calculated,
    otherwise the system time is retreived and used
    @return lasted time between last update and the compare time [msec.]
  */
  int32_t lastedTime( void ) const;

  /**
    deliver the timestamp of the last update as int32_t [msec]
    @return last update [msec]
  */
  int32_t lastTime() const {return i32_lastTime;}

  /**
    deliver actual timestamp
    @return actual val of i32_lastTime
  */
  int32_t actualTimestamp() const {return i32_lastTime;}

  /**
    updates i32_lastTime to ri32_time or actual
    system time if no time is given
    @param ri32_time optional time to store as last update time (default retreive actual time from central SystemMgmt_c instance)
  */
  void updateTime( int32_t ri32_time = -1 )
    {if ( ri32_time < 0 ) i32_lastTime = ElementBase_c::getLastRetriggerTime();
     else i32_lastTime = ri32_time;
    }

  /**
    check if given time intervall is lasted
    @param rui16_timeInterval time intervall in msec
    @param ri32_time optional timestamp in [msec]
    @return true -> time last timestamp older than intervall
  */
  bool checkTime(uint16_t rui16_timeInterval) const;

  /**
    check if given time intervall is lasted;
    if time intervall is lasted - update time
    @param rui16_timeInterval time intervall in msec
    @return true -> time last timestamp older than intervall
  */
  bool checkUpdateTime(uint16_t rui16_timeInterval);

protected:
private:
// Private methods

// Private attributes
  /** last system time of access or alive or received message in [250ms]*/
  int32_t i32_lastTime;
};


}
#endif
