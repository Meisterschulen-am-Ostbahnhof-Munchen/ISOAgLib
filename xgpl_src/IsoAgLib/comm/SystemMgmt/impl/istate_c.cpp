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
 * protocol details. By proviDing simple function calls for jobs like      *
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
using namespace __IsoAgLib;

/**
  constructor of IState_c which can set the state to given initial value
  @param ren_itemState optional wanted state information (default <empty> value)
*/
IState_c::IState_c(itemState_t ren_itemState, int ri_singletonVecKey)
	: ClientBase( ri_singletonVecKey )
{
  en_itemState = ren_itemState;
}

/**
  constructor of IState_c which can set the state to given initial value
  @param ren_itemState optional wanted state information (default <empty> value)
*/
IState_c::IState_c(uint8_t rb_state, int ri_singletonVecKey )
	: ClientBase( ri_singletonVecKey )
{
  en_itemState = itemState_t(rb_state);
}

/**
  copy constructor for IState_c
  @param rrefc_src source for this new instance
*/
IState_c::IState_c(const IState_c& rrefc_src)
	: ClientBase( rrefc_src )
{
  en_itemState = rrefc_src.en_itemState;
}



/**
  set the state of an monitor list item and
  return the resulting state value

  set state to Active, and reset Off and Standby on
  Active, PreAddressClaim, AddressClaim, ClaimedAddress, FalseAlive;

  set: PreAddressClaim, AddressClaim and ClaimedAddress exclusive

  set: Off, Standby, Active exclusive
  @param ren_itemState state information
  @param rb_clearOld optional clear old value for complete new set (default no clear)
  @return resulting state information
*/
IState_c::itemState_t IState_c::setItemState(itemState_t ren_itemState, bool rb_clearOld)
{
  if (rb_clearOld) en_itemState = ren_itemState;
  // set state to Active, and reset Off andStandby on
  // Active, PreAddressClaim, AddressClaim, ClaimedAddress, FalseAlive
  if ((ren_itemState & (Active|PreAddressClaim|AddressClaim|ClaimedAddress|FalseAlive)) > 0)
  { // one of these states set -> clear Off and Standby
    clearItemState(itemState_t(Off|Standby));
    // set PreAddressClaim, AddressClaim and ClaimedAddress exclusive
    if ((ren_itemState & (PreAddressClaim|AddressClaim|ClaimedAddress|FalseAlive)) > 0)
    { // one of PreAddressClaim, AddressClaim and ClaimedAddress exclusive
      // clear before set
      clearItemState(itemState_t(PreAddressClaim|AddressClaim|ClaimedAddress|FalseAlive));
    }
    // now set always additionally the Active flag
    en_itemState = itemState_t(en_itemState | Active);
  }

  // set: Off, Standby, Active exclusive
  if ((ren_itemState & (Off|Standby)) > 0)
  { // one of Off, Standby, Active (handled above)
    // clear: Off, Standby, Active
    clearItemState(itemState_t(Off|Standby|Active));
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
  @param ren_itemState optional wanted state information (default <empty> value)
*/
IStateExt_c::IStateExt_c(itemState_t ren_itemState, int ri_singletonVecKey)  : IState_c(ren_itemState, ri_singletonVecKey)
{
  counter.b_addressClaimCnt = counter.b_falseAliveCnt = counter.b_causedConflictCnt = counter.b_affectedConflictCnt = 0;
  i16_lastCausedConflictTime = i16_lastAffectedConflictTime = 0;
};

/**
  constructor of IState_c which can set the state to given initial value
  @param ren_itemState optional wanted state information (default <empty> value)
*/
IStateExt_c::IStateExt_c(uint8_t rb_state, int ri_singletonVecKey) : IState_c(rb_state, ri_singletonVecKey)
{
  counter.b_addressClaimCnt = counter.b_falseAliveCnt = counter.b_causedConflictCnt = counter.b_affectedConflictCnt = 0;
  i16_lastCausedConflictTime = i16_lastAffectedConflictTime = 0;
}

/**
  copy constructor for IState_c
  @param rrefc_src source for this new instance
*/
IStateExt_c::IStateExt_c(const IStateExt_c& rrefc_src) : IState_c(rrefc_src)
{
  counter.b_addressClaimCnt = rrefc_src.counter.b_addressClaimCnt;
  counter.b_falseAliveCnt = rrefc_src.counter.b_falseAliveCnt;
  counter.b_causedConflictCnt = rrefc_src.counter.b_causedConflictCnt;
  counter.b_affectedConflictCnt = rrefc_src.counter.b_affectedConflictCnt;
  i16_lastCausedConflictTime = rrefc_src.i16_lastCausedConflictTime;
  i16_lastAffectedConflictTime = rrefc_src.i16_lastAffectedConflictTime;
}

/**
  set and/or retreive the AddressClaim counter
  @param rc_cnt optional new AddressClaim counter  (default only Request)
  @return actual or resulting AddressClaim cnt
*/
uint8_t IStateExt_c::addressClaimCnt(int8_t rc_cnt)
{
  if (rc_cnt > Request)
  {
    setItemState(IState_c::AddressClaim);
    counter.b_addressClaimCnt = rc_cnt;
  }
  return (itemState(IState_c::AddressClaim))?counter.b_addressClaimCnt:0;
};

/**
  set and/or retreive the counter of false alive msgs
  @param rc_cnt optional new false alive counter  (default only Request)
  @return actual or resulting false alive cnt
*/
uint8_t IStateExt_c::falseAliveCnt(int8_t rc_cnt)
{
  switch (rc_cnt)
  {
    case Incr: // -2 --> correct timed alive in FalseAlive state -> Increment
      if (counter.b_falseAliveCnt < 0xF) counter.b_falseAliveCnt++;
      break;
    case Decr: // -3 --> wrong timed alive in FalseAlive state -> Decrement
      if (counter.b_falseAliveCnt > 0) counter.b_falseAliveCnt--;
      break;
    case Request: // -1 --> only answer cnt
      break;
    default: // new dircet given value >= 0
      if ((rc_cnt >= 0) && (rc_cnt < 0xF)) counter.b_falseAliveCnt = rc_cnt;
      break;
  }

  return (itemState(IState_c::FalseAlive))?counter.b_falseAliveCnt:0;
};

/**
  set and/or retreive the counter of false alive msgs
  @param rc_cnt optional new false alive counter  (default only Request)
  @return actual or resulting false alive cnt
*/
uint8_t IStateExt_c::causedConflictCnt(int8_t rc_cnt, int32_t ri32_time)
{
  switch (rc_cnt)
  {
    case Incr: // -2 --> new conflict
      i16_lastCausedConflictTime = (ri32_time / 1000);
      if (counter.b_causedConflictCnt < 0xF) counter.b_causedConflictCnt++;
      break;
    case Decr: // -3 --> timeEvent without conflict -> test if cnt can be reduced
       if (counter.b_causedConflictCnt > 0)
       {
        if ( (ri32_time / 1000) != i16_lastCausedConflictTime)
        { // last conflict over 1 sec. away
          counter.b_causedConflictCnt--;
        }
      }
      break;
    case Request: // -1 --> only answer cnt
      break;
    default: // new dircet given value >= 0
      if (rc_cnt > 0) setItemState(IState_c::CausedConflict);
      else clearItemState(IState_c::CausedConflict);
      if ((rc_cnt >= 0) && (rc_cnt < 0xF)) counter.b_causedConflictCnt = rc_cnt;
      break;
  }

  return (itemState(IState_c::CausedConflict))?counter.b_causedConflictCnt:0;
};

/**
  set and/or retreive the counter of false alive msgs
  @param rc_cnt optional new false alive counter  (default only Request)
  @return actual or resulting false alive cnt
*/
uint8_t IStateExt_c::affectedConflictCnt(int8_t rc_cnt, int32_t ri32_time)
{
  switch (rc_cnt)
  {
    case Incr: // -2 --> new conflict
      i16_lastAffectedConflictTime = (ri32_time / 1000);
      if (counter.b_affectedConflictCnt < 0xF) counter.b_affectedConflictCnt++;
      break;
    case Decr: // -3 --> timeEvent without conflict -> test if cnt can be reduced
       if (counter.b_affectedConflictCnt > 0)
       {
        if ( (ri32_time / 1000) != i16_lastAffectedConflictTime)
        { // last conflict over 1 sec. away
          counter.b_affectedConflictCnt--;
        }
      }
      break;
    case Request: // -1 --> only answer cnt
      break;
    default: // new dircet given value >= 0
      if (rc_cnt > 0) setItemState(IState_c::AffectedConflict);
      else clearItemState(IState_c::AffectedConflict);
      if ((rc_cnt >= 0) && (rc_cnt < 0xF)) counter.b_affectedConflictCnt = rc_cnt;
      break;
  }

  return (itemState(IState_c::AffectedConflict))?counter.b_affectedConflictCnt:0;
};
