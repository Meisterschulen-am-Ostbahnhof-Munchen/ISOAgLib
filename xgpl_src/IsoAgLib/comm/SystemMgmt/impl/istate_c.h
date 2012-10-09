/***************************************************************************
                          istate_c.h - object which manages state of item
                                     in monitor list
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
#ifndef ISTATE_H
#define ISTATE_H

#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/singleton.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  object which manages state of item in monitor list
  @author Dipl.-Inform. Achim Spangler
*/
class IState_c : public ClientBase {
public:
  /* *************************************** */
  /* ************* typedefs **************** */
  /* *************************************** */
  /** enum type for state of a monitor list item */
  enum itemState_t { IstateNull = 0x0,
         // states for distinguishing of DIN and ISO Members
         Din = 0x1, Iso = 0x2,
         // only one states of following Line possible
         Off = 0x4, Standby = 0xC, Active = 0x8,
         // only one states of following Line possible
         PreAddressClaim = 0x10, AddressClaim = 0x20, ClaimedAddress = 0x40, FalseAlive = 0x80,
         Stop = 0x100,
         // states of following Line independent parallel possible
         CausedConflict = 0x200,
         AffectedConflict = 0x400,
         Error = 0x800,
         Member = 0x1000,  ///< This identifies a normal member
         Service = 0x2000, ///< This flag identifies DIN 9684 services (could be used for ISO also ???)
         Local = 0x4000,   ///< This flag identifies local items
         PossiblyOffline = 0x8000 ///< This flag indicates, that the node missed to anwer at least one AdrClaim request -> upon next miss it should be deleted from monitor list
  };

  /**
    enum for function parameters selcting Single Member type or type order;
    the bits are so defined, that (protoOrder_t & itemState_t & 0x3) != 0
    only if an IState_c itemState and a searched proto protoOrder_t
    are fIffing for at least one protocol type
  */
  enum protoOrder_t { DinOnly = 0x1, IsoOnly = 0x2, DinIso = 0x13, IsoDin = 0x23 };

  /**
    constructor of IState_c which can set the state to given initial value
    @param ren_itemState optional wanted state information (default Active value)
  */
  IState_c(itemState_t ren_itemState = Active, int ri_singletonVecKey = 0);
  /**
    constructor of IState_c which can set the state to given initial value
    @param ren_itemState optional wanted state information (default <empty> value)
  */
  IState_c(uint8_t rb_state, int ri_singletonVecKey = 0);

  /**
    copy constructor for IState_c
    @param rrefc_src source for this new instance
  */
  IState_c(const IState_c& rrefc_src);

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
  itemState_t setItemState(itemState_t ren_itemState, bool rb_clearOld = false);

  /**
    check if specific state is exactly set
    @param ren_itemState state information to check
    @return true -> the given state is set
  */
  bool itemState(itemState_t ren_itemState) const
    {return ((en_itemState & ren_itemState) == ren_itemState)?true:false;};

  /**
  check if specific state is partially set
  @param ren_itemState combined state information to check
  @return true -> one of the given combined states is set
   */
  bool itemStatePartialMatch(itemState_t ren_itemState) const
  {return ((en_itemState & ren_itemState) != 0)?true:false;};

  /**
    deliver the state information
    @return state information of type itemState_t (with state informations coded by OR in enum)
  */
  const itemState_t itemState() const
    {return en_itemState;};

  /**
    clear whole state or some specific flags
    @param ren_itemState optional flags to clear (default clear all)
  */
  void clearItemState(itemState_t ren_itemState = itemState_t(~IstateNull))
    {en_itemState = itemState_t(en_itemState & ~ren_itemState);};
  /**
    check if an IState_c matches the searched protocols defined by protoOrder_t
    @param ren_searchedProto
    @return true -> at least one of the searched proto (DIN or ISO) is set for this item
  */
  bool matchSearchedProto(protoOrder_t ren_searchedProto) const
    {return (((uint8_t)en_itemState & (uint8_t)ren_searchedProto & 0x3) != 0)?true:false;};

private:
  /** state of this monitor item */
  itemState_t en_itemState;

};

/**
  extended state managing object, which stores additional information for Members
  @author Dipl.-Inform. Achim Spangler
*/
class IStateExt_c : public IState_c {
public:
  /** enum type for special command arguments */
  enum specialParameter_t { Request = -1, Incr = -2, Decr = -3};

  /**
    constructor of IStateExt_c which can set the state to given initial value
    @param ren_itemState optional wanted state information (default Off value)
  */
  IStateExt_c(itemState_t ren_itemState = Off, int ri_singletonVecKey = 0);

  /**
    constructor of IState_c which can set the state to given initial value
    @param ren_itemState optional wanted state information (default <empty> value)
  */
  IStateExt_c(uint8_t rb_state, int ri_singletonVecKey = 0);

  /**
    copy constructor for IState_c
    @param rrefc_src source for this new instance
  */
  IStateExt_c(const IStateExt_c& rrefc_src);

  /**
    set and/or retreive the AddressClaim counter
    @param rc_cnt optional new AddressClaim counter  (default only Request)
    @return actual or resulting AddressClaim cnt
  */
  uint8_t addressClaimCnt(int8_t rc_cnt = Request);

  /**
    retreive the counter of false alive msgs
    @return actual false alive cnt
  */
  uint8_t falseAliveCnt() const {return counter.b_falseAliveCnt;};

  /**
    set the counter of false alive msgs
    @param rc_cnt new false alive counter  (default only Request)
    @return actual or resulting false alive cnt
  */
  uint8_t falseAliveCnt(int8_t rc_cnt);

  /**
    retreive the counter of caused conflicts
    @return actual count of caused conflict
  */
  uint8_t causedConflictCnt() const {return counter.b_causedConflictCnt;};

  /**
    set the counter of false alive msgs
    @param rc_cnt new false alive counter  (default only Request)
    @return actual or resulting false alive cnt
  */
  uint8_t causedConflictCnt(int8_t rc_cnt, int32_t ri32_time = -1);

  /**
    retreive the counter of Affected conflicts
    @return actual count of Affected conflict
  */
  uint8_t affectedConflictCnt() const {return counter.b_affectedConflictCnt;};

  /**
    set the counter of false alive msgs
    @param rc_cnt new false alive counter  (default only Request)
    @return actual or resulting false alive cnt
  */
  uint8_t affectedConflictCnt(int8_t rc_cnt, int32_t ri32_time = -1);

private:
  /** last timestamp (in seconds) of caused conflict */
  int16_t i16_lastCausedConflictTime;
  /** last timestamp (in seconds) of Affected conflict */
  int16_t i16_lastAffectedConflictTime;
  struct {
    uint16_t b_causedConflictCnt : 4;
    uint16_t b_affectedConflictCnt : 4;
    uint16_t b_addressClaimCnt : 4;
    uint16_t b_falseAliveCnt : 4;
  } counter;
};

}

#endif
