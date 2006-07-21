/***************************************************************************
                          dinitem_c.h - element class for monitor
                                          lists of members
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
#ifndef DIN_ITEM_H
#define DIN_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "dinserviceitem_c.h"
#include "../../impl/istate_c.h"
#include <IsoAgLib/util/config.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  A DINItem_c stores and manages all status and identity informations
  of a DIN 9684 member. In addition to the name (up to 7 character) the lasted
  time since the last alive msg can be requested. Instances for local
  identities can manage the address claim procedure and send of alive triggered
  by periodic calls of timeEvent.
  @author Dipl.-Inform. Achim Spangler
  @short element class for monitor lists of members
*/
class DINItem_c : public DINServiceItem_c  {
private:
  // private typedef alias names
public:
  /**
    constructor which can set all ident informations
    @see ServictIState::ServiceItem
    @param ri32_time start time of this instance which is set to actual time on default
    @param rc_devKey DEV_KEY of this member item (default: <empty> value)
    @param rui8_nr member number of this member item (default: <empty> value)
    @param rb_status state of this ident (off, claimed address, ...) (default: off)
    @param rui16_adrVect ADRESSBELEGVECTO information used by this item (important for remote ident)
    @param rpb_name uint8_t string with the name of the ident (max. 7 uint8_t; default empty)
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  DINItem_c(int32_t ri32_time = 0, const DevKey_c& rc_devKey = DevKey_c::DevKeyUnspecified, uint8_t rui8_nr = 0xFF, IState_c::itemState_t rb_status = IState_c::IstateNull,
             uint16_t rui16_adrVect = 0, const uint8_t* rpb_name = NULL, int ri_singletonVecKey = 0);

  /**
    copy constructor which gets its informations from another instance
    @param rrefc_src source instance
  */
  DINItem_c(const DINItem_c& rrefc_src);

  /**
    copy operator which defines src as const
    @param rrefc_src source DINItem_c instance
  */
  DINItem_c& operator=(const DINItem_c& rrefc_src);

  /** default destructor which has nothing to do */
  virtual ~DINItem_c();

  /**
    set all identity data with one call
    @param ri32_time starting timestamp (default actual time)
    @param rc_devKey DEV_KEY of this ident (default <empty> value)
    @param rui8_nr number of this member (default <empty> value)
    @param ren_status status information of this ident (IState_c::Off, IState_c::Active, ...) (default: IState_c::Active)
    @param rui16_adrVect ADRESSBELEGVECTO information used by this item (important for remote ident)
    @param rpb_name uint8_t string with the name of the ident (max. 7 uint8_t; default empty)
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void set(int32_t ri32_time = 0, const DevKey_c& rc_devKey = DevKey_c::DevKeyUnspecified, uint8_t rui8_nr = 0xFF, itemState_t ren_status = IState_c::Active,
           uint16_t rui16_adrVect = 0, const uint8_t* rpb_name = NULL, int ri_singletonVecKey = 0 );

  /**
    performs periodic actions for members
    actions in DINItem_c: announc, adress claim
    actions delegated to DINServiceItem_c: alive or error state information

    possible errors:
      * dependant error on send of alive in CAN_IO
    @see ServiceIState::timeEvent
    @return true -> all planned activities performed
  */
  bool timeEvent( void );
  /**
    process received CAN pkg to update data and
    react if needed
    @return this item reacted on the msg
  */
  bool processMsg();
  /**
    start announcing
    @param rb_force true -> if item has already claimed address send adress release and start repeated address alcim
  */
  void startAddressClaim(bool rb_force = false);

  /**
    deliver name
    @return pointer to the name uint8_t string (7byte)
  */
  virtual const uint8_t* name() const;
  /**
    check if the name field is empty (no name received)
    @return true -> no name received
  */
  virtual bool isEmptyName() const;

  /**
    deliver name as pure ASCII string
    @param pc_name string where ASCII string is inserted
    @param rui8_maxLen max length for name
  */
  virtual void getPureAsciiName(int8_t *pc_asciiName, uint8_t rui8_maxLen);

  /**
    set name
    @param rpb_name pointer to 7 uint8_t string, which is used as name
  */
  void setName(const uint8_t* rpb_name){CNAMESPACE::memcpy(cpName,rpb_name,7);cpName[7] = '\0';};

  /**
    send the name of this monitor item

    possible errors:
      * Err_c::lbsSysNoActiveLocalDinMember this item hasn't claimed address
            --> sending is not allowed
      * dependant error in CANIO_c during send
  */
  void sendName();
  /**
    send an adress release telegram;
    should be called before deleting own member  instances

    possible errors:
      * Err_c::lbsSysNoActiveLocalDinMember this item hasn't claimed address
            --> sending is not allowed
      * dependant error in CANIO_c during send
  */
  void sendAdressRelease();
  #ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  /**
    set the state of a memberItem
    (overwrite base class method to implement reset of requested state and
      optional send of new state for IState_c::Off, IState_c::Standby, IState_c::Active)
    @param ren_itemState wante new state (independent old values aren't changed)
    @param rb_sendState optional send the new state for TRUE (default: don't send)
    @return resulting state
  */
  IState_c::itemState_t setItemState(itemState_t ren_itemState, bool rb_sendState = false);
  /**
    send the actual state as Scheduler_c message
    (check if member has claimed address correctly,
    and if this item is own ident)

    possible errors:
      * Err_c::lbsSysNoActiveLocalDinMember this item hasn't claimed address
            --> sending is not allowed
      * dependant error in CANIO_c during send
  */
  void sendState();
  /**
    send a state request/command for another member

    possible errors:
      * Err_c::lbsSysNoActiveLocalDinMember this item hasn't claimed address
            --> sending is not allowed
      * dependant error in CANIO_c during send
  */
  void sendStateRequest();

  /**
    deliver the requested state information
    @return state information of type itemState_t (with state informations coded by OR in enum)
  */
  itemState_t requestedState(){return c_requestedState.itemState();};
  /**
    check if specific state is requested
    @param ren_itemState state information to check
    @return true -> the given state is requested
  */
  bool requestedState(itemState_t ren_itemState) const
    {return c_requestedState.itemState(ren_itemState);};
  /**
    set the requested state of an monitor list item and
    return the resulting state value
    @param ren_itemState requested state information
    @param rb_clearOld optional clear old value for complete new set (default no clear)
    @return resulting state information
  */
  itemState_t setRequestedState(itemState_t ren_itemState, bool rb_clearOld = false)
    {return c_requestedState.setItemState(ren_itemState, rb_clearOld);};
  /**
    clear whole requested state or some specific flags
    @param ren_itemState optional flags to clear (default clear all)
  */
  void clearRequestedState(itemState_t ren_itemState = itemState_t(~IstateNull))
    { c_requestedState.clearItemState(ren_itemState);}
  #endif

private:
// Private attributes

  /** name of member */
  uint8_t cpName[8];
  #ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  /** number of state requester */
  uint8_t ui8_requesterNumber;
  /** state information for requested state */
  IState_c c_requestedState;
  #endif
};

}
#endif
