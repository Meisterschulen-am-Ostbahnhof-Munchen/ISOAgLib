/***************************************************************************
                          dinitem_c.cpp - element class for monitor
                                           lists of members
                             -------------------
    begin                : Thu Aug 19 2000
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
//#include <cstdio>
#include "dinitem_c.h"
#include "dinmonitor_c.h"
#include "dinsystempkg_c.h"
#include "../../impl/systemmgmt_c.h"
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/hal/system.h>

#if defined( __TSW_CPP__ ) && !defined( isprint ) && !defined( __W )
	#define	__W	0x01
	#define	__C	0x02
	#define	__S	0x04
	#define	__U	0x08
	#define	__L	0x10
	#define	__N	0x20
	#define	__P	0x40
	#define	__X	0x80
	#define	isprint(_c)	((_c)&(__P|__U|__L|__N|__S))
#else
	#include <cctype>
#endif

namespace __IsoAgLib {


/**
  constructor which can set all ident informations
  @see ServictIState::DINServiceItem_c
  @param ri32_time start time of this instance which is set to actual time on default
  @param rc_gtp GETY_POS of this member item (default: <empty> value)
  @param rui8_nr member number of this member item (default: <empty> value)
  @param rb_status state of this ident (off, claimed address, ...) (default: off)
  @param rui16_adrVect ADRESSBELEGVECTO information used by this item (important for remote ident)
  @param rpb_name uint8_t string with the name of the ident (max. 7 uint8_t; default empty)
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
DINItem_c::DINItem_c(int32_t ri32_time, GetyPos_c rc_gtp, uint8_t rui8_nr, IState_c::itemState_t rb_status,
            uint16_t rui16_adrVect, const uint8_t* rpb_name, int ri_singletonVecKey)
: DINServiceItem_c(ri32_time, rc_gtp, rui8_nr, rb_status, rui16_adrVect, ri_singletonVecKey )
{
  if (rpb_name != NULL)
  {
    setName(rpb_name);
  }
  else
  {
    for (int16_t i = 0; i < 8; i++)cpName[i] = '\0';
  }
}

/**
  copy constructor which gets its informations from another instance
  @param rrefc_src source instance
*/
DINItem_c::DINItem_c(const DINItem_c& rrefc_src) : DINServiceItem_c(rrefc_src)
{
  #ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  c_requestedState = rrefc_src.c_requestedState;
  #endif
  setName(rrefc_src.name());
}

/**
  copy constructor which gets its informations from another instance
  @param rrefc_src source instance
*/
DINItem_c& DINItem_c::operator=(const DINItem_c& rrefc_src){
  DINServiceItem_c::operator=(rrefc_src);
  #ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  c_requestedState = rrefc_src.c_requestedState;
  #endif
  setName(rrefc_src.name());
  return *this;
}

/** default destructor which has nothing to do */
DINItem_c::~DINItem_c(){
}

/**
  set all identity data with one call
  @param ri32_time starting timestamp (default actual time)
  @param rc_gtp GETY_POS of this ident (default <empty> value)
  @param rui8_nr number of this member (default <empty> value)
  @param rb_status status information of this ident (off, claimed address, ...) (default <empty> value)
  @param rui16_adrVect ADRESSBELEGVECTO information used by this item (important for remote ident)
  @param rpb_name uint8_t string with the name of the ident (max. 7 uint8_t; default empty)
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
void DINItem_c::set(int32_t ri32_time, GetyPos_c rc_gtp, uint8_t rui8_nr,
        itemState_t ren_status, uint16_t rui16_adrVect, const uint8_t* rpb_name, int ri_singletonVecKey)
{
  DINServiceItem_c::set(ri32_time, rc_gtp, rui8_nr, ren_status, rui16_adrVect, ri_singletonVecKey);
  if (rpb_name != NULL)
  {
    setName(rpb_name);
  }
  else
  {
    for (int16_t i = 0; i < 8; i++)cpName[i] = '\0';
  }
};

/**
  deliver name as pure ASCII string
  @param pc_asciiName string where ASCII string is inserted
  @param rui8_maxLen max length for name
*/
void DINItem_c::getPureAsciiName(int8_t *pc_asciiName, uint8_t rui8_maxLen)
{
  char temp[6];
  char tempName[20];
  uint8_t ui8_resInd = 0;
  if ( CNAMESPACE::memcmp(cpName, "\0\0\0", 3) == 0)
  { // name in binary Fieldstar format
    uint32_t ui32_temp = 0;
    for (uint8_t ui8_ind = 0; ui8_ind < 4; ui8_ind++)
    {
      ui32_temp += ( ((int32_t)(cpName[ui8_ind+3])) << ((3 - ui8_ind)*8));
    }
    #ifndef SYSTEM_PC_VC
    CNAMESPACE::sprintf(tempName, "0x%ulx", ui32_temp);
    #else
    sprintf(tempName, "0x%ulx", ui32_temp);
    #endif
  }
  else
  {
    for (uint8_t ui8_ind = 0; ui8_ind < 7; ui8_ind++)
    {
      if (!(isprint(cpName[ui8_ind])))
      {
        #ifndef SYSTEM_PC_VC
        CNAMESPACE::sprintf(temp, "%hd", (int16_t)cpName[ui8_ind]);
        #else
        sprintf(temp, "%hd", (int16_t)cpName[ui8_ind]);
        #endif
        tempName[ui8_resInd] = '+';
        ui8_resInd++;
        for (uint8_t ui8_tempInd = 0; ((ui8_tempInd < 5) && (temp[ui8_tempInd] != '\0')); ui8_tempInd++)
        {
          tempName[ui8_resInd] = temp[ui8_tempInd];
          ui8_resInd++;
        }
        tempName[ui8_resInd] = '+';
        ui8_resInd++;
      }
      else
      {
        tempName[ui8_resInd] = cpName[ui8_ind];
        ui8_resInd++;
      }
    }
    tempName[ui8_resInd] = '\0';
  }
  CNAMESPACE::memmove(pc_asciiName, tempName, rui8_maxLen - 1);
  pc_asciiName[rui8_maxLen-1] = '\0';
}

/**
  deliver name
  @return pointer to the name uint8_t string (7byte)
*/
const uint8_t* DINItem_c::name() const {
  return cpName;
}

/**
  check if the name field is empty (no name received)
  @return true -> no name received
*/
bool DINItem_c::isEmptyName() const
{ // as there are some silly devices out, which
  // have a name with several '\0' in the beginning,
  // a name is only empty, if ALL 7 chars are '\0'
  uint8_t ui8_nullCnt = 0;
  for (uint8_t ui8_ind = 0; ui8_ind < 8; ui8_ind++)
  {
    if (cpName[ui8_ind] == '\0') ui8_nullCnt++;
  }
  return ( ui8_nullCnt >= 7 )?true:false;
}


/**
  performs periodic actions for members
  actions in DINItem_c: announc, adress claim
  actions delegated to DINServiceItem_c: alive or error state information

  possible errors:
    * dependant error on send of alive in CANIO_c
  @see DINServiceItem_c::timeEvent
  @return true -> all planned time event activitie performed
*/
bool DINItem_c::timeEvent( void ){
  CANIO_c& c_can = getCanInstance4Comm();
  DINMonitor_c& c_din_monitor = getDinMonitorInstance4Comm();

  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  if (itemState(IState_c::ClaimedAddress))
  { // the timeEvent actions for items with claimed address are performed by DINServiceItem_c::timeEvent
    if (DINServiceItem_c::timeEvent())
    { // timed action was performed -> 1sec lasted
      // uppdate conflict state vars
      // -> if one second no conflict decrement conflict counter
      int i32_time = Scheduler_c::getLastTimeEventTrigger();
      affectedConflictCnt(IStateExt_c::Decr, i32_time);
      causedConflictCnt(IStateExt_c::Decr, i32_time);
    }
  }
  else
  { // element is in announcing state
    // -> check if it can claim an adress number
    if (checkUpdateTime(1000))
    { // prepare for announcing or adressclaim msg
      DINSystemPkg_c& c_pkg = c_din_monitor.data();
      // fill data in DINSystemPkg_c
      c_pkg.setGtp(gtp());

      uint8_t internAddressClaimCnt = addressClaimCnt();
      uint8_t freeAvailCode;
      bool b_can_claim = false;
      if (internAddressClaimCnt >= 3)
      {
        bool b_forceClear = ( internAddressClaimCnt >= 10 )?true:false;
				freeAvailCode = c_din_monitor.freeNrAvailable( b_forceClear );
        if (freeAvailCode == 2) return false;

        if ((c_din_monitor.canClaimNr(gtp())) && freeAvailCode == 1)
        {  // can claim an adress
          b_can_claim = true;
          // retreive free number and reserve it in AdrVect_c
          setNr(c_din_monitor.reserveFreeAdress());
          c_pkg.setVerw(0); // verw code for adress claim
          c_pkg.setSend(nr());
          c_adrvect = c_din_monitor.adrvect();
          c_pkg.adrvect() = c_adrvect;
          // set state to active and clear possible off or standby flags
          setItemState(IState_c::ClaimedAddress);
        }
      }
      if (b_can_claim == false)
      { // send normal address claim
        // build address claim msg
        c_pkg.set_a(1);

				if (internAddressClaimCnt < 3)
        { // increment address claim counter

          addressClaimCnt(internAddressClaimCnt + 1);
          internAddressClaimCnt++;
					c_pkg.setNr(internAddressClaimCnt);
        }
				else
				{ // use 3 as cnt for external usage and increase cnt until 10 is reached
					if ( internAddressClaimCnt < 10 ) addressClaimCnt(internAddressClaimCnt + 1);
					c_pkg.setNr( 3 );
				}

      }
      // start send
      c_can << c_pkg;
    }
  }
  return true;
}

/**
  process received CAN pkg to update data and
  react if needed
  reactions of DINItem_c: adress claim, alive, name request, name msg,
                            status command,
  reactions delegated to DINServiceItem_c: status information, nromal alive,
                                      error information
  @return this item reacted on the msg
*/
bool DINItem_c::processMsg()
{
  DINMonitor_c& c_din_monitor = getDinMonitorInstance4Comm();
  DINSystemPkg_c& c_pkg = c_din_monitor.data();
  bool result = false;

  if (c_pkg.a() == 1)
  { // address claim msg
    setItemState(IState_c::AddressClaim);
    addressClaimCnt(c_pkg.nr());
    updateTime(c_pkg.time());
  }
  else
  {
    switch (c_pkg.verw())
    {
      case 0: // number claim
        // store member nr claimed by SEND within this msg
        setNr(c_pkg.send());
        c_adrvect =  c_pkg.adrvect(); // set ADVECT
        updateTime(c_pkg.time()); // set alive timestamp
        // update AdrVect_c of SystemMgmt_c
        c_din_monitor.setUsedAdr(c_pkg.gtp(), c_pkg.send());
        // set this member only active, if trusted adrvect reports it as
        // correctly registered -> unallowed adress claims are blocked
        if (c_din_monitor.isAdrUsedTrusted(nr()))
        {
          setItemState(IState_c::ClaimedAddress);
        }
        else
        { // member has not parformed a correct address claim before this adress claim
          setItemState(IState_c::FalseAlive);
        }
        result = true;
        break;
      case 1:  // alive
        if (itemState(IState_c::FalseAlive))
        { // item is in false alive state
          if(!checkTime(3000))
          { // last falseAlive max 3sec old -> increment counter, so that
            // this item can be interpreted trusted with cnt==3 in the next update of trusted adrvect
            falseAliveCnt(IStateExt_c::Incr);
            if (falseAliveCnt() >= 3)
            { // accept false alive member as normal active one
              setItemState(IState_c::ClaimedAddress);
              falseAliveCnt(0); // reset falseAlive counter
            }
          }
          else
          { // non periodic alive msg of members in false alive state
            // shrink chance of being accepted as usual member
            if (falseAliveCnt() > 0) falseAliveCnt(IStateExt_c::Decr);
          }
          // take the received adrvect for this item
          c_adrvect = c_pkg.adrvect();
          updateTime(c_pkg.time());

          result = true;
        }
        else
        { // alive in normal state is processed by DINServiceItem_c
          result = DINServiceItem_c::processMsg();
        }
        break;
      case 8:
        // request for name
        // send own name as response
        sendName();
        result = true;
        break;
      case 9:
        // msg with name
        // load name of member
        setName(c_pkg.name());
        result = true;
        break;
      #ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
      case 12:
        if (c_pkg.m() == 0)
        {  // status command
          switch (c_pkg.sta())
          {
            case 0:
              setRequestedState(IState_c::Off);
              break;
            case 1:
              setRequestedState(IState_c::Standby);
              break;
            case 2:
              setRequestedState(IState_c::Active);
              break;
          }
          ui8_requesterNumber = c_pkg.send();
          result = true;
          break;
        }
        else
        { // status informations are handled by DINServiceItem_c
          result = DINServiceItem_c::processMsg();
          break;
        }
      #endif
      default:
        // let DINServiceItem_c process the msg
        result = DINServiceItem_c::processMsg();
        break;
    }
  }
  return result;
}



/**
  start announcing
  @param rb_rb_force true -> if item has already claimed address send adress release and start repeated address alcim
*/
void DINItem_c::startAddressClaim(bool rb_force)
{
  if (itemState(IState_c::ClaimedAddress) && rb_force )
  {  // item has already claimed address, and repeated address claim is forced
    // send free adress message
    sendAdressRelease();
    clearItemState(IState_c::ClaimedAddress);
  }
  // now item isn't active -> send first address claim message
  DINSystemPkg_c& c_pkg = getDinMonitorInstance4Comm().data();
  // fill data in DINSystemPkg_c
  c_pkg.setGtp(gtp());
  c_pkg.set_a(1);
  c_pkg.setNr(1);
  addressClaimCnt(1);
  // start send
  getCanInstance4Comm() << c_pkg;
  // register new state
  clearItemState(IState_c::PreAddressClaim);
  setItemState(IState_c::AddressClaim);
}

/**
  send the name of this monitor item

  possible errors:
    * Err_c::lbsSysNoActiveLocalMember this item hasn't claimed address
          --> sending is not allowed
    * dependant error in CANIO_c during send
*/
void DINItem_c::sendName(){
  if (!isEmptyName())
   {
    if (itemState(IState_c::ClaimedAddress))
    {
      DINSystemPkg_c& c_pkg = getDinMonitorInstance4Comm().data();
      // fill data in DINSystemPkg_c
      c_pkg.setGtp(gtp());
      c_pkg.setVerw(9);
      c_pkg.setSend(nr());
      c_pkg.setName(name());
      // start send
      getCanInstance4Comm() << c_pkg;
    }
    else
    {  // this item hasn't claimed address and can't send msg
      getLbsErrInstance().registerError( LibErr_c::LbsSysNoActiveLocalMember, LibErr_c::LbsSystem );
    }
  }
}

/**
  send an adress release telegram;
  should be called before deleting own member  instances

  possible errors:
    * Err_c::lbsSysNoActiveLocalMember this item hasn't claimed address
          --> sending is not allowed
    * dependant error in CANIO_c during send
*/
void DINItem_c::sendAdressRelease(){
  if (itemState(IState_c::Local)) //send adress release only for own items
   {
    if (itemState(IState_c::ClaimedAddress))
    {
      DINSystemPkg_c& c_pkg = getDinMonitorInstance4Comm().data();
      // fill data in DINSystemPkg_c
      c_pkg.setGtp(gtp());
      c_pkg.setVerw(2);
      c_pkg.setSend(nr());
      // clear own number in AdrVect_c
      getDinMonitorInstance4Comm().clearUsedAdr(nr());
      c_adrvect = getDinMonitorInstance4Comm().adrvect();
      // get AdrVect_c where own number is cleared
      c_pkg.adrvect() = c_adrvect;
      // start send
      getCanInstance4Comm() << c_pkg;
    }
    else
    { // this item hasn't claimed address ->not allowed to send
      getLbsErrInstance().registerError( LibErr_c::LbsSysNoActiveLocalMember, LibErr_c::LbsSystem );
    }
  }
}

#ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
/**
  set the state of a memberItem
  (overwrite base class method to implement reset of requested state and
    optional send of new state for IState_c::Off, IState_c::Standby, IState_c::Active)
  @param ren_itemState wante new state (independent old values aren't changed)
  @param rb_sendState optional send the new state for TRUE (default: don't send)
  @return resulting state
*/
IState_c::itemState_t DINItem_c::setItemState(itemState_t ren_itemState, bool rb_sendState)
{
  // some new states need special additional action
  if ((ren_itemState & (IState_c::Active | IState_c::Off | IState_c::Standby)) > 0)
  { // reset the requested state
    clearRequestedState();
    if (rb_sendState) sendState();
  }
  // store the new state with base class method
  return DINServiceItem_c::setItemState(ren_itemState);
}

/**
  send the actual state as Scheduler_c message
  (check if member has claimed address correctly,
  and if this item is own ident)

  possible errors:
    * Err_c::lbsSysNoActiveLocalMember this item hasn't claimed address
          --> sending is not allowed
    * dependant error in CANIO_c during send
*/
void DINItem_c::sendState(){
  if (itemState(IState_c::Local))
   { //send state information only for local identities
    if (itemState(IState_c::ClaimedAddress))
    {
      DINSystemPkg_c& c_pkg = getDinMonitorInstance4Comm().data();
      // fill data in DINSystemPkg_c
      c_pkg.setGtp(gtp());
      c_pkg.setVerw(12);
      c_pkg.setSend(nr());
      // if this is an answer to state reuqest set empf to the requester number
      if (requestedState(IState_c::itemState_t(IState_c::Off | IState_c::Standby | IState_c::Active)))
        {c_pkg.setEmpf(ui8_requesterNumber);}
      else c_pkg.setEmpf(nr()); // else use own number
      c_pkg.set_m(1); // indicate state
      c_pkg.setSta(sta());
      // start send
      getCanInstance4Comm() << c_pkg;
    }
    else
    { // this item hasn't claimed address for send of msg
      getLbsErrInstance().registerError( LibErr_c::LbsSysNoActiveLocalMember, LibErr_c::LbsSystem );
    }
  }
}

/**
  send a state request/command for another member

  possible errors:
    * Err_c::lbsSysNoActiveLocalMember this item hasn't claimed address
          --> sending is not allowed
    * dependant error in CANIO_c during send
*/
void DINItem_c::sendStateRequest(){
  // send a state request only for remote members
  if ((!itemState(IState_c::Local)) && (getSystemMgmtInstance4Comm().existActiveLocalDinMember()))
   {
    if (itemState(IState_c::ClaimedAddress))
    {
      DINMonitor_c& c_din_monitor = getDinMonitorInstance4Comm();
      DINSystemPkg_c& c_pkg = c_din_monitor.data();
      // use number/gtp of first active identity
      DINItem_c& senderItem = getSystemMgmtInstance4Comm().getActiveLocalDinMember();

      // fill data in DINSystemPkg_c
      c_pkg.setGtp(senderItem.gtp());
      c_pkg.setVerw(12);
      c_pkg.setSend(senderItem.nr());


      c_pkg.setEmpf(nr());
      c_pkg.set_m(0); // request/command for state
      uint8_t b_sta = (requestedState(IState_c::Off))?0:
                    ((requestedState(IState_c::Standby))?1:2);
      c_pkg.setSta(b_sta);
      // start send of state request
      getCanInstance4Comm() << c_pkg;
    }
    else
    { // this item hasn't claimed address for send of msg
      getLbsErrInstance().registerError( LibErr_c::LbsSysNoActiveLocalMember, LibErr_c::LbsSystem );
    }
  }
}
#endif

} // end of namespace __IsoAgLib
