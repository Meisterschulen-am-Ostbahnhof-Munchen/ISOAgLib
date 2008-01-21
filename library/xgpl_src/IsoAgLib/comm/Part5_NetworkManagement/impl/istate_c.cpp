/***************************************************************************
                          istate_c.cpp - object which manages state of item
                                      in monitor list
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
 * protocol details. By providing simple function calls for jobs like      *
 * jobs like starting a measuring program for a process data value on a    *
 * the mAin program has not to deal with Single CAN telegram formatting.   *
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
#include "istate_c.h"

/* ************************************** */
/* import some namespaces for easy access */
/* ************************************** */
//using namespace __IsoAgLib;  // using direct specification "__IsoAgLib::xyz" 
                               // so that it will be correctly referenced in 
                               // the generated documantation (doxygen)

/**
  constructor of IState_c which can set the state to given initial value
  @param ren_itemState optional wanted state information (default \<empty\> value)
*/
__IsoAgLib::IState_c::IState_c(itemState_t ren_itemState, int ai_singletonVecKey)
	: ClientBase( ai_singletonVecKey )
{
  en_itemState = ren_itemState;
}

/**
  constructor of IState_c which can set the state to given initial value
  @param ren_itemState optional wanted state information (default \<empty\> value)
*/
__IsoAgLib::IState_c::IState_c(uint8_t ab_state, int ai_singletonVecKey )
	: ClientBase( ai_singletonVecKey )
{
  en_itemState = itemState_t(ab_state);
}

/**
  copy constructor for IState_c
  @param arc_src source for this new instance
*/
__IsoAgLib::IState_c::IState_c(const __IsoAgLib::IState_c& arc_src)
	: ClientBase( arc_src )
{
  en_itemState = arc_src.en_itemState;
}



/**
  set the state of an monitor list item and
  return the resulting state value

  set state to Active, and reset Off and OffUnable on
  Active, PreAddressClaim, AddressClaim, ClaimedAddress;

  set: PreAddressClaim, AddressClaim and ClaimedAddress exclusive

  set: Off, OffUnable, Active exclusive
  @param ren_itemState state information
  @param ab_clearOld optional clear old value for complete new set (default no clear)
  @return resulting state information
*/
__IsoAgLib::IState_c::itemState_t __IsoAgLib::IState_c::setItemState(itemState_t ren_itemState, bool ab_clearOld)
{
  if (ab_clearOld) en_itemState = ren_itemState;
  // set state to Active, and reset Off and OffUnable on
  // Active, PreAddressClaim, AddressClaim, ClaimedAddress
  if ((ren_itemState & (Active|PreAddressClaim|AddressClaim|ClaimedAddress|AddressLost)) > 0)
  { // one of these states set -> clear Off and OffUnable
    clearItemState(itemState_t(OffExplicitly|OffUnable));
    // set PreAddressClaim, AddressClaim, ClaimedAddress and AddressLost exclusive
    if ((ren_itemState & (PreAddressClaim|AddressClaim|ClaimedAddress|AddressLost)) > 0)
    { // one of PreAddressClaim, AddressClaim, ClaimedAddress and AddressLost exclusive
      // clear before set
      clearItemState(itemState_t(PreAddressClaim|AddressClaim|ClaimedAddress|AddressLost|PossiblyOffline));
    }
    // now set always additionally the Active flag
    en_itemState = itemState_t(en_itemState | Active);
  }

  // set: Off, Standby, Active exclusive
  if ((ren_itemState & (OffExplicitly|OffUnable)) > 0)
  { // one of Off, OffUnable, Active (handled above)
    // clear: Off, OffUnable, Active
    clearItemState(itemState_t(OffExplicitly|OffUnable|Active));
  }

  // now simple set the new value
  en_itemState = itemState_t(en_itemState | ren_itemState);

  return en_itemState;
};



/* ********************* */
/* methods of IStateExt_c */
/* ********************* */

/**
  constructor of IStateExt_c which can set the state to given initial value
  @param ren_itemState optional wanted state information (default \<empty\> value)
*/
__IsoAgLib::IStateExt_c::IStateExt_c(itemState_t ren_itemState, int ai_singletonVecKey)  : __IsoAgLib::IState_c(ren_itemState, ai_singletonVecKey)
{
  counter.b_addressClaimCnt = counter.b_causedConflictCnt = counter.b_affectedConflictCnt = 0;
  mi16_lastCausedConflictTime = mi16_lastAffectedConflictTime = 0;
};

/**
  constructor of IState_c which can set the state to given initial value
  @param ren_itemState optional wanted state information (default \<empty\> value)
*/
__IsoAgLib::IStateExt_c::IStateExt_c(uint8_t ab_state, int ai_singletonVecKey) : __IsoAgLib::IState_c(ab_state, ai_singletonVecKey)
{
  counter.b_addressClaimCnt = counter.b_causedConflictCnt = counter.b_affectedConflictCnt = 0;
  mi16_lastCausedConflictTime = mi16_lastAffectedConflictTime = 0;
}

/**
  copy constructor for IState_c
  @param arc_src source for this new instance
*/
__IsoAgLib::IStateExt_c::IStateExt_c(const IStateExt_c& arc_src) : __IsoAgLib::IState_c(arc_src)
{
  counter.b_addressClaimCnt = arc_src.counter.b_addressClaimCnt;
  counter.b_causedConflictCnt = arc_src.counter.b_causedConflictCnt;
  counter.b_affectedConflictCnt = arc_src.counter.b_affectedConflictCnt;
  mi16_lastCausedConflictTime = arc_src.mi16_lastCausedConflictTime;
  mi16_lastAffectedConflictTime = arc_src.mi16_lastAffectedConflictTime;
}

/**
  set and/or retreive the AddressClaim counter
  @param ac_cnt optional new AddressClaim counter  (default only Request)
  @return actual or resulting AddressClaim cnt
*/
uint8_t __IsoAgLib::IStateExt_c::addressClaimCnt(int8_t ac_cnt)
{
  if (ac_cnt > Request)
  {
    setItemState(__IsoAgLib::IState_c::AddressClaim);
    counter.b_addressClaimCnt = ac_cnt;
  }
  return (itemState(__IsoAgLib::IState_c::AddressClaim))?counter.b_addressClaimCnt:0;
};

/**
  set and/or retreive the counter of false alive msgs
  @param ac_cnt optional new false alive counter  (default only Request)
  @return actual or resulting false alive cnt
*/
uint8_t __IsoAgLib::IStateExt_c::causedConflictCnt(int8_t ac_cnt, int32_t ai32_time)
{
  switch (ac_cnt)
  {
    case Incr: // -2 --> new conflict
      mi16_lastCausedConflictTime = (ai32_time / 1000);
      if (counter.b_causedConflictCnt < 0xF) counter.b_causedConflictCnt++;
      break;
    case Decr: // -3 --> timeEvent without conflict -> test if cnt can be reduced
       if (counter.b_causedConflictCnt > 0)
       {
        if ( (ai32_time / 1000) != mi16_lastCausedConflictTime)
        { // last conflict over 1 sec. away
          counter.b_causedConflictCnt--;
        }
      }
      break;
    case Request: // -1 --> only answer cnt
      break;
    default: // new dircet given value >= 0
      if (ac_cnt > 0) setItemState(__IsoAgLib::IState_c::CausedConflict);
      else clearItemState(__IsoAgLib::IState_c::CausedConflict);
      if ((ac_cnt >= 0) && (ac_cnt < 0xF)) counter.b_causedConflictCnt = ac_cnt;
      break;
  }

  return (itemState(__IsoAgLib::IState_c::CausedConflict))?counter.b_causedConflictCnt:0;
};

/**
  set and/or retreive the counter of false alive msgs
  @param ac_cnt optional new false alive counter  (default only Request)
  @return actual or resulting false alive cnt
*/
uint8_t __IsoAgLib::IStateExt_c::affectedConflictCnt(int8_t ac_cnt, int32_t ai32_time)
{
  switch (ac_cnt)
  {
    case Incr: // -2 --> new conflict
      mi16_lastAffectedConflictTime = (ai32_time / 1000);
      if (counter.b_affectedConflictCnt < 0xF) counter.b_affectedConflictCnt++;
      break;
    case Decr: // -3 --> timeEvent without conflict -> test if cnt can be reduced
       if (counter.b_affectedConflictCnt > 0)
       {
        if ( (ai32_time / 1000) != mi16_lastAffectedConflictTime)
        { // last conflict over 1 sec. away
          counter.b_affectedConflictCnt--;
        }
      }
      break;
    case Request: // -1 --> only answer cnt
      break;
    default: // new dircet given value >= 0
      if (ac_cnt > 0) setItemState(__IsoAgLib::IState_c::AffectedConflict);
      else clearItemState(__IsoAgLib::IState_c::AffectedConflict);
      if ((ac_cnt >= 0) && (ac_cnt < 0xF)) counter.b_affectedConflictCnt = ac_cnt;
      break;
  }

  return (itemState(__IsoAgLib::IState_c::AffectedConflict))?counter.b_affectedConflictCnt:0;
};
