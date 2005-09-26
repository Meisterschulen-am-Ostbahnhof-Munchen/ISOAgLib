/***************************************************************************
                          dinmonitor.cpp - object for monitoring members
                                              (list of DINItem_c)
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
#include "dinmonitor_c.h"

#include "dinsystempkg_c.h"
#ifdef USE_DIN_SERVICEMONITOR
  #include "dinservicemonitor_c.h"
#endif
#include "../../impl/systemmgmt_c.h"
#include "../../impl/istate_c.h"
#include <IsoAgLib/util/liberr_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_dinItemTotal = 0;
#endif

namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique DINMonitor_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  DINMonitor_c& getDinMonitorInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return DINMonitor_c::instance( rui8_instance );
  };
#else
  /** C-style function, to get access to the unique DINMonitor_c singleton instance */
  DINMonitor_c& getDinMonitorInstance( void )
  {
    static DINMonitor_c& c_din_monitor = DINMonitor_c::instance();
    return c_din_monitor;
  };
#endif


void
DINMonitor_c::singletonInit()
{
  init();
  /** @todo maybe optimize to avoid double initialization? */
}

/**
  initialisation for DINMonitor_c
*/
void DINMonitor_c::init( void )
{
  c_tempDinMemberItem.set(0, DevKey_c::DevKeyUnspecified, 0xFF, IState_c::Active, 0, NULL, getSingletonVecKey() );
  c_data.setSingletonKey( getSingletonVecKey() );
  #ifdef DEBUG_HEAP_USEAGE
  sui16_dinItemTotal -= vec_dinMember.size();
  #endif
  vec_dinMember.clear();
  pc_dinMemberCache = vec_dinMember.end();
  i32_lastTrusted = 0;

  // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
  // register in Scheduler_c to be triggered fopr timeEvent
  getSchedulerInstance4Comm().registerClient( this );

  if (!getCanInstance4Comm().existFilter( *this, (uint16_t)(7 << 8),(uint16_t)0, Ident_c::StandardIdent))
  { // set filter with PRI = 000
    getCanInstance4Comm().insertFilter( *this, (7 << 8),0, true);
  }

  // set filter with PRI = 000

  b_lastNameRequest = 0;
  #ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  b_globalSystemState = true;
  #endif
}
/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void DINMonitor_c::close( void )
{
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    #ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
    b_globalSystemState = false;
    #endif
    getSchedulerInstance4Comm().unregisterClient( this );
  }
};

/**
  deliver reference to data pkg as reference to CANPkgExt_c
  to implement the base virtual function correct
*/
CANPkgExt_c& DINMonitor_c::dataBase()
{
  return c_data;
}
/**
  performs periodically actions,
  here: update trusted adrVect and resort array if needed every 3sec;

  possible errors:
    * pertial error caused by one of the memberItems

  @return true -> all planned activities performed in allowed time
*/
bool DINMonitor_c::timeEvent( void ){
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

  Vec_MemberIterator pc_iterItem;
  int32_t i32_now = Scheduler_c::getLastTimeEventTrigger();

  #ifndef SYSTEM_MITRON167
  // this causes Mitron167 to freeze after sending the name request
  // - just before the own name is sent
  // - not shure if watchdog problem, or if something else
  //  ( probably watchdog, as freeze is not always active -> timing could change like that )
  // check every 5 sec. if there's still a member with claimed address without name
  uint8_t b_now = ((i32_now/1000)%5);

  if ((b_lastNameRequest != b_now) && (b_now == 1))
  { // it's time to check for names
    b_lastNameRequest = b_now;
    bool b_namesComplete = true;
    uint8_t ui8_cntMember = dinMemberCnt(true);
    for (uint8_t ui8_ind = 0; ui8_ind < ui8_cntMember; ui8_ind++)
    {
      if (dinMemberInd(ui8_ind, true).isEmptyName())
      { // name is empty
        b_namesComplete = false;
      }
    }
    if ((!b_namesComplete) && (getSystemMgmtInstance4Comm().existActiveLocalDinMember()))
    { // there are members with claimed address with empty name
      requestDinMemberNames();
    }
  }
  else
  {
    if (b_now != 1)b_lastNameRequest = 0;
  }
  #endif

  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

  if (i32_now - i32_lastTrusted >= 3000)
  { // last resort is 3sec. ago -> sort array (quick if no sort is needed)
    System_c::triggerWd();
    vec_dinMember.sort();
    System_c::triggerWd();

    // trusted adrvVect is +3 sec. old -> build new
    // first collect number of registered
    AdrVect_c collectAdrvect, allOrVect, result;
    bool b_repeat=true;
    while (b_repeat && (!vec_dinMember.empty()))
    {
      b_repeat=false;
      for (pc_iterItem = vec_dinMember.begin();
          pc_iterItem != vec_dinMember.end(); pc_iterItem++)
      {
        if (pc_iterItem->itemState(IState_c::ClaimedAddress))
        {  // actual element has claimed address -> store its number in adrVect
          collectAdrvect.setUsedAdr(pc_iterItem->nr());
          allOrVect |= pc_iterItem->adrvect();
        }

        #if CONFIG_DIN_ITEM_MAX_AGE > 0
        // clean every 3sec. inactive member items from list
        if ((pc_iterItem->lastedTime() > CONFIG_DIN_ITEM_MAX_AGE) && (!(pc_iterItem->itemState(IState_c::Local))))
        {
          Vec_MemberIterator pc_iterDelete = pc_iterItem;
          #ifdef USE_ISO_11783
          // delete item with same DEVKEY also from ISO list
          if (getIsoMonitorInstance4Comm().existIsoMemberDevKey(pc_iterDelete->devKey()))
          { // item with same DEVKEY in ISOMonitor -> delete it
            getIsoMonitorInstance4Comm().deleteIsoMemberDevKey(pc_iterDelete->devKey());
          }
          #endif
          //erase delivers iterator to item after erased item
          pc_iterItem = vec_dinMember.erase(pc_iterDelete);
          #ifdef DEBUG_HEAP_USEAGE
          sui16_dinItemTotal--;

          getRs232Instance()
            << sui16_dinItemTotal << " x DINItem_c: Mal-Alloc: "
            <<  sizeSlistTWithMalloc( sizeof(DINItem_c), sui16_dinItemTotal )
            << "/" << sizeSlistTWithMalloc( sizeof(DINItem_c), 1 )
            << ", Chunk-Alloc: "
            << sizeSlistTWithChunk( sizeof(DINItem_c), sui16_dinItemTotal )
            << "\r\n\r\n";
          #endif
          b_repeat = true;
          break;
  // Old version:
  //        return true;        // return from this function, as the iterator might get problem after erase
        }
        #endif
      }
    }
    System_c::triggerWd();

    // now compare the two versions
    if (collectAdrvect == allOrVect)
    {  // use collectAdrvect as new trusted vector
      result = collectAdrvect;
    }
    else
    { // they are not the same -> use version which most members use
      uint8_t sumOr = 0, all = 0;
      for (pc_iterItem = vec_dinMember.begin();
            pc_iterItem != vec_dinMember.end(); pc_iterItem++)
      {
        if (pc_iterItem->itemState(IState_c::ClaimedAddress))
        { // use only members with claimed address for voting the correct AdrVect
          if (allOrVect == pc_iterItem->adrvect()) sumOr++;
          all++;
        }
      }
      System_c::triggerWd();
      // if allOrVect is used by >= 50% of items which claimed correctly an address use it as trusted
      if (sumOr >= (all/2))
      {
        result = allOrVect;
      }
      else
      { // use collectAdrvect as base
        result = collectAdrvect;

        // check for every reported number in allOrVect,
        // if >50% of all member report it as used
        for (int16_t nr = 0; nr < 15; nr++)
        {
          if ((allOrVect.isAdrUsed(nr)) && (!result.isAdrUsed(nr)))
          { // number i is reported as used in at least one vector, but its not reported in temp result
            // check how many vectors report it
            sumOr = 0;
            for (pc_iterItem = vec_dinMember.begin();
                  pc_iterItem != vec_dinMember.end(); pc_iterItem++)
            {
              // only use regular members with claimed address for voting
              if (pc_iterItem->itemState(IState_c::ClaimedAddress))
              { // increment sumOr if this item reports nr as claimed/used
                if (pc_iterItem->adrvect().isAdrUsed(nr)) sumOr++;
              }
            }
            // if sumOr >=50% set nr used in result
            if (sumOr >= (all/2)) result.setUsedAdr(nr);
          }
        }
        System_c::triggerWd();
        // now all possible numbers are checked -> result is new trusted vector
      }
    }
    // result is now the new trusted vector
    // check if there are changes to old trusted vector
    for (int16_t nr = 0; nr < 15; nr++)
    {
      if ((result.isAdrUsed(nr)) && (!isAdrUsedTrusted(nr)))
      { // nr only in new reported -> set member with nr as active
        if (existDinMemberNr(nr))
        { // if majority of other members report this item as having claimed an address
          // set the state to having correctly claimed address
          dinMemberNr(nr).setItemState(IState_c::ClaimedAddress);
        }
      }
    }
    System_c::triggerWd();

    // set result to trusted
    c_adrVectTrusted.setTrusted(result);
    // store the bits of trusted AdrVect_c additional with Bit-OR in std. AdrVect
    c_adrVectTrusted |= adrvect();
    // adrVect is now actual -> update time
    i32_lastTrusted = i32_now;
  }

  #ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  // perform stop command send
  // send STOP commands for all actual stop settings
  sendPeriodicStop();
  #endif

  return true;
}



/**
  sends system message, which requests all active
  members to send their names (send own name too, as protocol demands)

  possible errors:
    * lbsSysNoActiveLocalMember on missing own active ident
  @return true -> member name command successful sent
*/
bool DINMonitor_c::requestDinMemberNames(){
  bool b_result = false;
  SystemMgmt_c& c_lbsSystem = getSystemMgmtInstance4Comm();
  /* ******************************** */
  /* perform some precondition checks */
  /* ******************************** */
  if (!c_lbsSystem.existActiveLocalDinMember())
  { // no active local ident
    getLbsErrInstance().registerError( LibErr_c::LbsSysNoActiveLocalMember, LibErr_c::LbsSystem );
    return false;
  }

  /* ************************** */
  /* build and send request msg */
  /* ************************** */

  // an local ident is active which can send answers
  // set the data for the request pgk
  MonitorItem_c& senderItem = c_lbsSystem.getActiveLocalDinMember();

  data().setDevKey(senderItem.devKey());
  data().setVerw(8); // verw code for name request
  data().setSend(senderItem.nr());
  // now send the telegram
  CANIO_c& c_can = getCanInstance4Comm();
  getLbsErrInstance().clear( LibErr_c::Can );
  c_can << data();
  // set return val dependent on CAN state after send
  b_result = (getLbsErrInstance().getErrCnt() == 0 )?true:false;

  // send the names of the own identities with a claimed address
  for (Vec_MemberIterator pc_iter = vec_dinMember.begin() ; pc_iter != vec_dinMember.end(); pc_iter++)
  {
    if (pc_iter->itemState(IState_c::itemState_t(IState_c::ClaimedAddress | IState_c::Local))) pc_iter->sendName();
  }
  return b_result;
}


/**
  deliver the count of members in the Monitor-List with given DEVCLASS (variable POS)
  which optional (!!) match the condition of address claim state
  @param rui8_devClass searched DEVCLASS code
  @param rb_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return count of members in Monitor-List with DEVCLASS == rui8_devClass
*/
uint8_t DINMonitor_c::dinMemberDevClassCnt(uint8_t rui8_devClass, bool rb_forceClaimedAddress)
{
  uint8_t b_result = 0;
  for (Vec_MemberIterator pc_iter = vec_dinMember.begin() ; pc_iter != vec_dinMember.end(); pc_iter++)
  {
    if ( ( ((pc_iter->devKey().getDevClass()) == rui8_devClass) || (rui8_devClass == 0xFF))
      && (!rb_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      b_result++;
      pc_dinMemberCache = pc_iter; // set member cache to member  with searched devClass
    }
  }
  return b_result;
}

/**
  deliver one of the members with specific DEVCLASS
  which optional (!!) match the condition of address claim state
  check first with dinMemberDevClassCnt if enough members with wanted DEVCLASS and
  optional (!!) property are registered in Monitor-List
  @see dinMemberDevClassCnt

  possible errors:
    * range there exist less than rui8_ind members with DEVCLASS rui8_devClass
  @param rui8_devClass searched DEVCLASS
  @param rui8_ind position of the wanted member in the
               sublist of member with given DEVCLASS (first item has rui8_ind == 0 !!)
  @param rb_forceClaimedAddress true -> only members with claimed address are used
       (optional, default false)
  @return reference to searched element
*/
DINItem_c& DINMonitor_c::dinMemberDevClassInd(uint8_t rui8_devClass, uint8_t rui8_ind, bool rb_forceClaimedAddress)
{
  int8_t c_cnt = -1;
  for (Vec_MemberIterator pc_iter  = vec_dinMember.begin() ; pc_iter != vec_dinMember.end(); pc_iter++)
  {
    if ( ( ((pc_iter->devKey().getDevClass()) == rui8_devClass) || (rui8_devClass == 0xFF))
      && (!rb_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      c_cnt++;
      if (c_cnt == rui8_ind)
      {
        pc_dinMemberCache = pc_iter; // set member cache to member  with searched devClass
        break; //searched Item found (first element has 0)break; //searched Item found (first element has 0)
      }
    }
  }
  // check if rui8_ind was in correct range
  if (rui8_ind != c_cnt)
  { // wrong range of rui8_ind
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::LbsSystem );
  }
  return *pc_dinMemberCache;
}


/**
  check if a memberItem with given DEV_KEY exist
  which optional (!!) match the condition of address claim state
  and update local pc_dinMemberCache
  @param rc_devKey searched DEV_KEY
  @param rb_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return true -> searched member exist
*/
bool DINMonitor_c::existDinMemberDevKey(const DevKey_c& rc_devKey, bool rb_forceClaimedAddress)
{
  if (!vec_dinMember.empty() && (pc_dinMemberCache != vec_dinMember.end()))
  {
    if ( (pc_dinMemberCache->devKey() == rc_devKey )
      && (!rb_forceClaimedAddress || pc_dinMemberCache->itemState(IState_c::ClaimedAddress))
        )  return true;
  }
  for (pc_dinMemberCache = vec_dinMember.begin();
       pc_dinMemberCache != vec_dinMember.end();
       pc_dinMemberCache++)
  {
    if ( (pc_dinMemberCache->devKey() == rc_devKey )
      && (!rb_forceClaimedAddress || pc_dinMemberCache->itemState(IState_c::ClaimedAddress))
        )  return true;
  };
  // if reaching here -> nothing found
  return false;
};

/**
  check if a member with given number exist
  which optional (!!) match the condition of address claim state
  and update local pc_dinMemberCache
  @param rui8_nr searched member number
  @return true -> item found
*/
bool DINMonitor_c::existDinMemberNr(uint8_t rui8_nr )
{
  if (!vec_dinMember.empty() && (pc_dinMemberCache != vec_dinMember.end()))
  {
      if (pc_dinMemberCache->nr() == rui8_nr) return true;
  }
  for (pc_dinMemberCache = vec_dinMember.begin();
       pc_dinMemberCache != vec_dinMember.end(); pc_dinMemberCache++)
  {if (pc_dinMemberCache->equalNr(rui8_nr))return true;}
  return false;
};

/**
  check if member is in member list with wanted DEV_KEY,
  adopt instance if member with claimed address with other device class inst exist
  @param refc_devKey DEV_KEY to search (-> it's updated if member with claimed address with other dev class inst is found)
  @return true -> member with claimed address with given DEVCLASS found (and refc_devKey has now its DEV_KEY)
*/
bool DINMonitor_c::dinDevClass2DevKeyClaimedAddress(DevKey_c &refc_devKey)
{
  if (existDinMemberDevKey(refc_devKey, true))
  { // there exists a device with exact NAME in claimed address state
    return true;
  }
  else
  { // no item with DEV_KEY found -> adapt POS
    // search for member with claimed address with same DEVCLASS
    if (dinMemberDevClassCnt(refc_devKey.getDevClass(), true) > 0)
    { // member with wanted device class exists -> store the DEVKEY
      refc_devKey = dinMemberDevClassInd(refc_devKey.getDevClass(), 0, true).devKey();
      return true;
    }
    else if (dinMemberDevClassCnt(refc_devKey.getDevClass(), false) > 0)
    { // member with wanted device class exists -> store the DEVKEY
      refc_devKey = dinMemberDevClassInd(refc_devKey.getDevClass(), 0, false).devKey();
      // even if a device with wanted DEVKEY exist - it is not claimed
      return false;
    }
    else
    {
      return false;
    }
  }
}


/**
  insert a new DINItem_c in the list; with unset rui8_nr the member is initiated as
  address claim state; otherwise the given state can be given or state Active is used

  possible errors:
    * badAlloc not enough memory to insert new DINItem_c isntance
    * busy another member with same ident exists already in the list

  @param rc_devKey DEV_KEY of the member
  @param rpb_name pointer to 7 uint8_t name string
  @param rui8_nr member number
  @param rui16_adrvect AdrVect_c used by the member
  @param ren_status wanted status
  @return true -> the DINItem_c was inserted
*/
bool DINMonitor_c::insertDinMember(const DevKey_c& rc_devKey, const uint8_t* rpb_name, uint8_t rui8_nr, uint16_t rui16_adrvect, IState_c::itemState_t ren_state)
{
  bool b_result = true;

  // check if another DINItem_c with same DEV_KEY already exist
  if (existDinMemberDevKey(rc_devKey))
  { // another member with same DEV_KEY found
    getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::LbsSystem );
    b_result = false; // don't insert
  }

  if (b_result)
  { // the insert is allowed
    // prepare temp item with wanted data
    c_tempDinMemberItem.set(System_c::getTime(), rc_devKey, rui8_nr, IState_c::itemState_t(ren_state), rui16_adrvect, rpb_name, getSingletonVecKey() );
    c_tempDinMemberItem.setItemState(IState_c::Member);
    // if rNr < 15 == active member update adrVect
    if (rui8_nr < 15)
    { // defined number
      setUsedAdr(rc_devKey, rui8_nr);
      // check if trusted vector has accepted this member
      if (isAdrUsedTrusted(rui8_nr))
      { // number is accepted -> set new member to claimed address
        c_tempDinMemberItem.setItemState(IState_c::ClaimedAddress);
      }
      else
      { // the number isn't accepted as correctly claimed -> set falseAlive
        c_tempDinMemberItem.itemState(IState_c::FalseAlive);
      }
    }
    else
    { // new member is announcing
      c_tempDinMemberItem.addressClaimCnt(1);
    }

    // now insert element
    const uint8_t b_oldSize = vec_dinMember.size();
    vec_dinMember.push_front(c_tempDinMemberItem);
    if (vec_dinMember.size() <= b_oldSize)
    { // array didn't grow
      getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsSystem );
      b_result = false;
    }
    #ifdef DEBUG_HEAP_USEAGE
    else
    {
      sui16_dinItemTotal++;
      getRs232Instance()
        << sui16_dinItemTotal << " x DINItem_c: Mal-Alloc: "
        <<  sizeSlistTWithMalloc( sizeof(DINItem_c), sui16_dinItemTotal )
        << "/" << sizeSlistTWithMalloc( sizeof(DINItem_c), 1 )
        << ", Chunk-Alloc: "
        << sizeSlistTWithChunk( sizeof(DINItem_c), sui16_dinItemTotal )
        << "\r\n\r\n";
    }
    #endif
    vec_dinMember.sort(); // resort the list
  }
  return b_result;
};

/**
  deliver member item with given devKey
  (check with existDinMemberDevKey before access to not defined item)

  possible errors:
    * elNonexistent on failed search

  @param rc_devKey searched DEV_KEY
  @return reference to searched MemberItem
  @exception containerElementNonexistant
*/
DINItem_c& DINMonitor_c::dinMemberDevKey(const DevKey_c& rc_devKey, bool rb_forceClaimedAddress)
{
  if (existDinMemberDevKey(rc_devKey, rb_forceClaimedAddress))
  { // no error
    return static_cast<DINItem_c&>(*pc_dinMemberCache);
  }
  else
  { // wanted element not found
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );

    // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    //return reference to first element as fallback
    return vec_dinMember.front();
  }
};

/**
  deliver member item with given nr
  (check with existDinMemberNr before access to not defined item)

  possible errors:
    * elNonexistent on failed search

  @param rui8_nr searched number
  @return reference to searched MemberItem
  @exception containerElementNonexistant
*/
DINItem_c& DINMonitor_c::dinMemberNr(uint8_t rui8_nr)
{
  if (existDinMemberNr(rui8_nr))
  { // no error
    return static_cast<DINItem_c&>(*pc_dinMemberCache);
  }
  else
  { // wanted element not found
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );

    // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    //return reference to first element as fallback
    return vec_dinMember.front();
  }
};

/**
  deliver member item with given DEV_KEY, set pointed bool var to true on success
  and set a Member Array Iterator to the result
  @param rc_devKey searched DEV_KEY
  @param pb_success bool pointer to store the success (true on success)
  @param pbc_iter optional member array iterator which points to searched DINItem_c on success
  @return reference to the searched item
*/
DINItem_c& DINMonitor_c::dinMemberDevKey(const DevKey_c& rc_devKey, bool *const pb_success, bool rb_forceClaimedAddress, Vec_MemberIterator *const pbc_iter)
{
  *pb_success = (existDinMemberDevKey(rc_devKey, rb_forceClaimedAddress))?true:false;

  if (pbc_iter != NULL)
  {
    *pbc_iter = pc_dinMemberCache;
  }
  return static_cast<DINItem_c&>(*pc_dinMemberCache);
};

/**
  delete item with specified devKey

  possible errors:
    * elNonexistent no member with given DEV_KEY exists

  @param rc_devKey DEV_KEY of to be deleted member
  @param rb_send-release true -> send adress release msg (optional, default = false)
*/
bool DINMonitor_c::deleteDinMemberDevKey(const DevKey_c& rc_devKey, bool rb_sendRelease)
{ // only delete local items, if send of adress release is requested - otherwise
  // this is not triggered by local software
  if ( ( existDinMemberDevKey(rc_devKey)                                                    )
    && ( ( rb_sendRelease ) || ( ! pc_dinMemberCache->itemState ( IState_c::Local ) ) ) )
  { // set correct state
    // check the number and delete it from AdrVect
    clearUsedAdr(pc_dinMemberCache->nr());

    // if release should be send - do it now
    if (rb_sendRelease && pc_dinMemberCache->itemState(IState_c::ClaimedAddress))
    { // fill data in SystemPkg
      data().setDevKey(rc_devKey);
      data().setVerw(2);
      data().setSend(pc_dinMemberCache->nr());
      data().adrvect() = adrvect();
      // start send
      getCanInstance4Comm() << data();
    }

    // erase it from list (existDinMemberDevKey sets pc_dinMemberCache to the wanted item)
    vec_dinMember.erase(pc_dinMemberCache);
    pc_dinMemberCache = vec_dinMember.begin();
    #ifdef DEBUG_HEAP_USEAGE
    sui16_dinItemTotal--;

    getRs232Instance()
      << sui16_dinItemTotal << " x DINItem_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(DINItem_c), sui16_dinItemTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(DINItem_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(DINItem_c), sui16_dinItemTotal )
      << "\r\n\r\n";
    #endif

    return true;
  }
  else
  { // to be deleted member DEV_KEY does not exist
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );
    return false;
  }
};

/**
  delete item with specified member number

  possible errors:
    * elNonexistent no member with given DEV_KEY exists

  @param rui8_nr number of to be deleted member
  @param rb_send-release true -> send adress release msg (optional, default = false)
*/
bool DINMonitor_c::deleteDinMemberNr(uint8_t rui8_nr, bool rb_sendRelease)
{
  if (existDinMemberNr(rui8_nr))
  { // use deleteDinMemberDevKey
    return deleteDinMemberDevKey(pc_dinMemberCache->devKey(), rb_sendRelease);
  }
  else
  { // to be deleted member number does not exist
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );
    return false;
  }
};

/**
  check if ECU with devKey is allowed to claim an adress number
  @param rc_devKey DEV_KEY of member which is tested
  @return true -> member is allowed to claim number
*/
bool DINMonitor_c::canClaimNr(const DevKey_c& rc_devKey)
{ DevKey_c min_devKey = rc_devKey;

  // search announcing list item with lowest devKey
  for (Vec_MemberIterator pc_iterItem = vec_dinMember.begin();
        pc_iterItem != vec_dinMember.end(); pc_iterItem++)
  {
    // if item pc_iterItem is announcing and its devKey is lower than actual min_devKey
    // update min_devKey
//    if ((pc_iterItem->itemState(IState_c::AddressClaim)) && (pc_iterItem->devKey() < min_devKey))
//      min_devKey =  pc_iterItem->devKey();
    if (pc_iterItem->itemState(IState_c::AddressClaim))
    {
      if (pc_iterItem->devKey() < min_devKey) min_devKey =  pc_iterItem->devKey();
    }
  }
  // if actual min_devKey is rc_devKey, the ECU can claim adress
  if (min_devKey == rc_devKey) return true;
  else return false;
}

/**
  check, if one free adre is available;
  if all numbers are used:
  a) numbers which are reported as used, but not found in monitor list are deleted
  b) deletable, because >3sec. inactive member, are searched and deleted
  @param rb_eraseInactiveItem optional command deletion of inactive item, if no numberis free (default false)
  @return true -> a member number is free
*/
bool DINMonitor_c::freeNrAvailable(bool rb_eraseInactiveItem)
{
  bool b_result = c_adrVectTrusted.freeNrAvailable();

  // if no number is free search for item with claimed address,
  // which didn't sent alive for min 3 sec
#if CONFIG_DIN_ITEM_MAX_AGE > 0
  if (true)
#else
  if (!b_result && rb_eraseInactiveItem)
#endif
  {
     bool foundDeletable = false;
     int32_t deletableAge = 3000, aliveAge;

    // TRY A)
    // check for bits set as used, for which no items can be found
    // -> free them first
     for (int16_t i = 0; i < 15; i++)
     {
       if (!existDinMemberNr(i))
       { // for number i in adrvect no member item can be found -> free it
         // only clear nr in AdrVect_c if no according item in vector is found
        clearUsedAdr(i);
        b_result = true; // now one adress is free
        break;
       }
#if CONFIG_DIN_ITEM_MAX_AGE > 0
      else
      {
        aliveAge = dinMemberNr(i).lastedTime();
        if (aliveAge > CONFIG_DIN_ITEM_MAX_AGE)
        { // this item isn't active any more -> possibly reuse this nr
          clearUsedAdr(i);
          b_result = true; // now one adress is free
          break;
        }
      }
#endif
     }

    // if A) without success TRY B)
    if (!b_result)
     {
      Vec_MemberIterator pc_iterItem, pc_deletableIter = NULL;
      // search for longest time (>= 3sec.) inactive member in vector
      for (pc_iterItem = vec_dinMember.begin();
           pc_iterItem != vec_dinMember.end(); pc_iterItem++)
       {
         if (pc_iterItem->itemState(IState_c::ClaimedAddress))
         {
           aliveAge = pc_iterItem->lastedTime();
           if (aliveAge > deletableAge)
           {
             foundDeletable = true;
             deletableAge =  aliveAge;
             pc_deletableIter = pc_iterItem;
           }
         }
       }
      if (foundDeletable)
       { // delete inactive member and force send of adress release
         deleteDinMemberDevKey(pc_deletableIter->devKey(), true);
        b_result = true;   // sign for now free but previously not -> exit timeEvent
      }
     }
   }
  return b_result;
};

/**
  change devKey if actual devKey isn't unique
  (search possible free instance to given device class)

  possible errors:
    * busy no other device class inst code leads to unique DEV_KEY code

  @param refc_devKey reference to DEV_KEY var (is changed directly if needed!!)
  @return true -> referenced DEV_KEY is now unique
*/
bool DINMonitor_c::unifyDinDevKey(DevKey_c& refc_devKey){
  bool b_result = true;
  DevKey_c c_tempDevKey = refc_devKey;
  if (existDinMemberDevKey(refc_devKey))
  { // devKey exist -> search new with changed POS
    b_result = false; // refc_devKey isn't unique
    // store the pos part of given devKey
    int16_t tempPos = (refc_devKey.getDevClassInst()),
        diff = 1;
    for (; diff < 8; diff++)
    {
      if (tempPos + diff < 8)
      {  // (tempPos + diff) would be an allowed device class inst code
        c_tempDevKey.setDevClassInst( tempPos + diff );
        if (!(existDinMemberDevKey(c_tempDevKey)))
        {  // (tempPos + diff) can't be found in list -> it is unique
          refc_devKey.setDevClassInst( tempPos + diff );
          b_result = true;
          break;
        }
      }
      if (tempPos - diff >= 0)
      { // (tempPos - diff) would be an allowed device class inst code
        c_tempDevKey.setDevClassInst( tempPos - diff );
        if (!(existDinMemberDevKey(c_tempDevKey)))
        {  // (tempPos - diff) can't be found in list -> it is unique
          refc_devKey.setDevClassInst( tempPos - diff );
          b_result = true;
          break;
        }
      }
    }
  }
  if (!b_result) getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::LbsSystem );
  return b_result;
}

/**
  sets given number as used at least in internal adrvect;
  if claim is allowed, the number is set used in trusted and the
  published version too

  possible errors:
    * busy another member with given nr is already registered in AdrVect

  @param rc_devKey DEV_KEY of the used setting member
  @param rui8_nr number to register as used for the given member
  @return true -> the wanted nr was registered successful in AdrVect
*/
bool DINMonitor_c::setUsedAdr(const DevKey_c& rc_devKey, uint8_t rui8_nr){
  bool b_result = false;
  if ((canClaimNr(rc_devKey)) || (System_c::getTime() < 3000))
  { // claim is allowed -> update trusted (it updates internal too)
    b_result = c_adrVectTrusted.setUsedAdrTrusted(rui8_nr);
  }
  else
  { // only update the internal, which is used too avoid using insecure or used numbers
    // for internal identities
    b_result = c_adrVectTrusted.setUsedAdr(rui8_nr);
  }
  #if 0
  // too sensible
  if (!b_result)
  { // number is already used in AdrVect_c -> set error
    getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::LbsSystem );
  }
  #endif
  return b_result;
}


/**
  process system msg with informations which are
  important for managing of members
  @return true -> message processed by DINMonitor_c; false -> process msg by ServiceMonitor
*/
bool DINMonitor_c::processMsg(){
  bool b_result = false;

   // perform dependent on type of msg apropriate validation checks
  if (data().a() == 1)
  {
    processAddressClaimMsg();
    return true;
  }

  // normal DIN9684 system message
  if (!adressConflict())
  { // decide what to do dependent on system msg type
    bool allowedClaim = false;
    switch (data().verw())
    {
     case 9:
       // additionally register this msg in Scheduler_c (other reqction equal to VERW 1,3,10,11,12
       getSchedulerInstance4Comm().registerDinMemberNameReceive();
     case 1: case 3: case 10: case 11: case 12:
      // cehck if is valid active member
      // (there is member with this devKey/nr marked as active in list)
      // || (there is no itme with this nr && no with this devKey
      //       -> if SystemTime < 3sec accept as normal; else insert with
      //          status "falseAlive" --> is member send 3times false alive
      //                check if other member interprete it as normal member -> adapt)
      if (!existDinMemberDevKey(data().devKey()))
      {
        // insert memberItem
        insertDinMember(data().devKey(), NULL, data().send(), data().adrvect().adrvect());
        // register send in AdrVect
        setUsedAdr(data().devKey(), data().send());
        // set falseAlive state if local ECU is longer than 3sec. online
        if (System_c::getTime() > 3000)dinMemberDevKey(data().devKey()).setItemState(IState_c::FalseAlive);
        else dinMemberDevKey(data().devKey()).setItemState(IState_c::ClaimedAddress);
      }

      // suitable entry with DEV_KEY exist in list
      if ((data().verw() == 11) || ((data().verw() == 12) && (data().m() == 0)))
      { // this is a command from send to empf -> set the state in the stateRequest
        // of member with number empf
        if (existDinMemberNr(data().empf()))dinMemberNr(data().empf()).processMsg();
      }
      else
      {
        // let the item process the data of pkg
        dinMemberDevKey(data().devKey()).processMsg();
      }
      b_result = true;
      break;
     case 0: // address claim
      // ((member must have lowest devKey of all announcing members
      // && must have announed 3 times)
      // || (system time is lower than 3 sec
      //      && no announcing member with this devKey in list))
      // && no other active member with this devKey in list

      if (!existDinMemberNr(data().send()))
      { // message is valid -> process
        if( ( canClaimNr(data().devKey()) ) ||( System_c::getTime() < 3000 )) allowedClaim = true;
        if (!existDinMemberDevKey(data().devKey()))
        { // insert memberItem
          insertDinMember(data().devKey(), NULL, data().send(), data().adrvect().adrvect());
        }
        // let the item process the data of pkg
        dinMemberDevKey(data().devKey()).processMsg();
        // if claim was unallowed register it as false alive
        if (!allowedClaim) dinMemberDevKey(data().devKey()).itemState(IState_c::FalseAlive);
      }
      b_result = true;
      break;
     case 2: // adress free
      // there must be an item with same nr/devKey or according nr hasn't sent alive since > 3sec
      // delete item from the list
      // IMPORTANT: Varioterminal sends delete msg for members which are still active - but not used by Varioterminal
      // -> call delete only for outdated and not local members
      if (existDinMemberDevKey(data().devKey())) {
        // member exists
        if ( ( dinMemberDevKey(data().devKey()).lastedTime() > 10000            )
          && ( ! dinMemberDevKey(data().devKey()).itemState( IState_c::Local )  )
          && ( ! getSystemMgmtInstance4Comm().existLocalMemberDevKey( data().devKey()
                #ifdef USE_ISO_11783
                , IState_c::DinOnly
                #endif
                ) )
          ) {
          deleteDinMemberDevKey(data().devKey());
          b_result = true;
        }
      }
      else if ( ( c_adrVectTrusted.isAdrUsedTrusted( data().send() ) ) || ( c_adrVectTrusted.isAdrUsed( data().send() ) ) )
      { // given number is marked as used in AdrVect_c -> clear it there
        c_adrVectTrusted.clearUsedAdr( data().send() );
        b_result = true;
      }
      break;
     case 8: // claim for sending the own ECU name
      // simply send own name - it has no risk -> let all
      // local identities process the message
      for (Vec_MemberIterator pc_iter = vec_dinMember.begin() ; pc_iter != vec_dinMember.end(); pc_iter++)
      {
        if (pc_iter->itemState(IState_c::itemState_t(IState_c::ClaimedAddress | IState_c::Local))) pc_iter->processMsg();
      }
      b_result = true;
      break;
     #ifdef USE_DIN_SERVICEMONITOR
     case 4: case 7:
      b_result = getDinServiceMonitorInstance4Comm().processMsg();
      break;
      #endif
      #ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
     case 15: // system on/off
      // must be sent from virtual terminal -> devKey== 0, send==0
      if ((data().devKey() == 0) && (data().send() == 0))
      {
        if (data().nae() == 1) b_globalSystemState = true; // on
        else b_globalSystemState = false; // off
        b_result = true;
      }
      break;
      #endif
    } // end switch
  } // end no conflict
  return b_result; // return if msg was processed by DINMonitor_c
}

/** process a message with an address claim information */
void DINMonitor_c::processAddressClaimMsg(){
  // check if sender used my devKey or number
  if ( (getSystemMgmtInstance4Comm().existLocalDinMemberDevKey(data().devKey()))
    && ( existDinMemberDevKey(data().devKey()) ) )
  { // notify my identity with this devKey about conflict
    dinMemberDevKey(data().devKey()).affectedConflictCnt(IStateExt_c::Incr, System_c::getTime());
  }
  else
  {
    bool valid = false;

    switch (data().nr())
    {
      case 1: // first address claim
        if (!existDinMemberDevKey(data().devKey())) valid = true;
        break;
      case 2: // second address claim
        if (
            (
             ( existDinMemberDevKey(data().devKey()) )
           &&( dinMemberDevKey(data().devKey()).addressClaimCnt() == 1 )
            )
          ||( System_c::getTime() < 3000 )
           ) valid = true;
        break;
      case 3: //third address claim
        if (
            (
             ( existDinMemberDevKey(data().devKey()) )
           &&( dinMemberDevKey(data().devKey()).addressClaimCnt() >= 2 )
            )
          ||( System_c::getTime() < 3000 )
           ) valid = true;
        break;
    }
    if (valid)
    { // process address claim msg
      if (!existDinMemberDevKey(data().devKey()))
      { // insert memberItem
        insertDinMember(data().devKey());
      }
      // let the item process the data of pkg
      dinMemberDevKey(data().devKey()).processMsg();
    }
  }
}

/**
  check if a received msg contains a adress conflict
  -> only interprete the msg, if no conflict is reported
  @return true -> the actual received conflict causes a conflict
*/
bool DINMonitor_c::adressConflict(){
  // ignore all adress conflict caused by name requests, because
  // of lots of interpretation problems of name request
  // additionally ignore conflicts caused by Scheduler_c services (DEVCLASS < 8)
  if ((data().verw() == 8) || (data().devKey().getDevClass() < 8)) return false; // report NO conflict

  // first calculate some standard elements
  Vec_MemberIterator senderIter;
  bool senderFound = false, senderDevKeyExist = false, senderNrExist = false;
  dinMemberDevKey(data().devKey(), &senderDevKeyExist, &senderIter);
  if ((senderDevKeyExist) && (senderIter->nr() == data().send()))
  {
    senderFound = true;
    senderNrExist = true;
  }
  else
  {
    if (existDinMemberNr(data().send())) senderNrExist = true;
  }

  // check if sender used my devKey or number
  // only interprete as conflict of affected local member is already in
  // monitor list
  if ( (senderDevKeyExist)
     &&(getSystemMgmtInstance4Comm().existLocalDinMemberDevKey(data().devKey()))
     )
  { // notify my identity with this devKey about conflict
     dinMemberDevKey(data().devKey()).affectedConflictCnt(IStateExt_c::Incr, System_c::getTime());
     return true; // report conflict
  }

  if ((senderNrExist)
    &&(getSystemMgmtInstance4Comm().existLocalDinMemberNr(data().send()))
      )
  { // notify my identity with this number about conflict
     if (existDinMemberNr(data().send()))
     { // only interprete if local member is already in monitor-list
       dinMemberNr(data().send()).affectedConflictCnt(IStateExt_c::Incr, System_c::getTime());
       return true; // report conflict
     }
  }

  // if both devKey and send are not found in list, no conflict
  if ((!senderDevKeyExist)&&(!senderNrExist)) return false;

  if (senderFound) // == senderNrExist && senderNrExist
  { // DEV_KEY and SEND correspond right according monitor list and
    // no conflict with local indents -> return reporting no conflict
    return false; // NO conflict
  }

  if ((senderDevKeyExist) && (!senderNrExist))
  { // notify the member with DEV_KEY of msg if it has other number
    if (dinMemberDevKey(data().devKey()).itemState(IState_c::ClaimedAddress))
    { // other member with same DEV_KEY is announcd with diferent number
      senderIter->causedConflictCnt(IStateExt_c::Incr, System_c::getTime());
      return true;
    }
    else
    { // this can be the first msg with number from previous not claimed address member
      return false;
    }
  }

  // last alternative: !senderDevKeyExist && senderNrExist
  if (dinMemberNr(data().send()).checkTime(3000))
  { // ignore conflict, because member seems to be inactive
    // delete old item
    deleteDinMemberNr(data().send());
    return false; //report NO conflict
  }
  else
  {
    dinMemberNr(data().send()).affectedConflictCnt(IStateExt_c::Incr, System_c::getTime());
  }
  return true; // report conflict detect
}

#ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
/**
  starts or release stop command for specific member;
  IMPORTANT: as protocol demands the stop command is repeated till explicit stop-release function call

  possible errors:
    * elNonexistent the commanded member DEV_KEY doesn't exist in member list
    * lbsSysNoActiveLocalMember on missing own active ident
  @param rc_devKeyTarget DEV_KEY of the member, which should be stopped
  @param rb_toStop true -> start sending STOP commands; false -> release STOP sending mode
  @return true -> stop command sent without errors
*/
bool DINMonitor_c::commandStop(const DevKey_c& rc_devKeyTarget, bool rb_toStop){
  /* ******************************** */
  /* perform some precondition checks */
  /* ******************************** */
  if (!existDinMemberDevKey(rc_devKeyTarget))
  { // commanded member DEV_KEY not existant
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );
    return false;
  }
  if (!getSystemMgmtInstance4Comm().existActiveLocalDinMember())
  { // no active local ident
    getLbsErrInstance().registerError( LbsSysNoActiveLocalMember, LibErr_c::LbsSystem );
    return false;
  }
  // commanded member exist and a local ident can send the command

  /* *************************** */
  /* build and send stop command */
  /* *************************** */
  uint8_t b_empfTarget = dinMemberDevKey(rc_devKeyTarget).nr();
  DINItem_c& c_sender = getSystemMgmtInstance4Comm().getActiveLocalDinMember();
  ArrStopIterator c_stopIter = vec_stop.begin();
  bool b_result = true;

  // check if a stop manager entry exists for the given target member
  for (; c_stopIter != vec_stop.end(); c_stopIter++)
  {
    if (c_stopIter->empf() == b_empfTarget)
    {
      break;
    }
  }

  // check if stop command or release for active stopManager entry is wanted
  if (rb_toStop || (c_stopIter != vec_stop.end()))
  {
    // now if there is already a stop command for empf its pos is in detected
    if (c_stopIter == vec_stop.end())
    { // insert stop in vec_stop
      uint8_t b_oldStopSize = vec_stop.size();
      vec_stop.push_front(DINStopManager_c(b_empfTarget, rb_toStop));

      // if new DINStopManager_c wasn't inserted successfully stop performing stop
      if (vec_stop.size() <= b_oldStopSize) b_result = false;
      else c_stopIter = (vec_stop.begin());
    }
    else
    { // update stop entry for existant entry
      c_stopIter->setStop(b_empfTarget, rb_toStop);
    }
    // if new DINStopManager_c entry wasn't inserted with success don't continue
    // -> continue only for b_result still true
    if (b_result)
    {
      // send first command - and update i32_lastStopSent
      if(sendStopIntern(c_sender.devKey(), 11, c_sender.nr(),
                        b_empfTarget, c_stopIter->command()))
      {
        // notify element of send
        c_stopIter->notifySend();
        // delete stop manager entry if no more active
        if (!c_stopIter->active()) vec_stop.erase(c_stopIter);
      }
      else
      { // send of stop command with errors
        b_result = false;
      }
    }
  }
  return b_result;
}

/**
  send ISO11783 or DIN9684 system msg to command status request for another member to switch to given mode

  possible errors:
    * elNonexistent the commanded member DEV_KEY doesn't exist in member list
    * lbsSysNoActiveLocalMember on missing own active ident
  @param rc_devKey DEVCLASS_PSO of member, which switch state to OFF
  @param ren_itemState wanted state of item
  @return true -> stop command sent without errors
*/
bool DINMonitor_c::commandItemState(const DevKey_c& rc_devKey, IState_c::itemState_t ren_itemState)
{
  /* ******************************** */
  /* perform some precondition checks */
  /* ******************************** */
  if (!existDinMemberDevKey(rc_devKey))
  { // commanded member DEV_KEY not existant
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );
    return false;
  }
  if (!getSystemMgmtInstance4Comm().existActiveLocalDinMember())
  { // no active local ident
    getLbsErrInstance().registerError( LbsSysNoActiveLocalMember, LibErr_c::LbsSystem );
    return false;
  }
  // commanded member exist and a local ident can send the command

  /* ********************* */
  /* initiate state command */
  /* ********************* */
  dinMemberDevKey(rc_devKey).setRequestedState(ren_itemState);
  return true;
};

/**
  starts or release stop command for all system members;
  IMPORTANT: as protocol demands the stop command is repeated till explicit stop-release function call

  possible errors:
    * lbsSysNoActiveLocalMember on missing own active ident for sending the command
  @param rb_toStop true -> star sending STOP commands; false -> release STOP sending mode
  @return true -> one of the own identities was active with claimed address to send the global stop correctly
*/
bool DINMonitor_c::commandGlobalStop(bool rb_toStop){
  bool b_result = false;
  /* ******************************** */
  /* perform some precondition checks */
  /* ******************************** */
  if (!getSystemMgmtInstance4Comm().existActiveLocalDinMember())
  { // no active local ident
    getLbsErrInstance().registerError( LbsSysNoActiveLocalMember, LibErr_c::LbsSystem );
    return false;
  }

  DINItem_c& c_sender = getSystemMgmtInstance4Comm().getActiveLocalDinMember();

  c_globalStop.setStop(0xF, rb_toStop);
  // send first command - and update i32_lastStopSent
  if (sendStopIntern(c_sender.devKey(), 10, c_sender.nr(),
                    0xF, c_globalStop.command()))
  { // send performed without error
    // notify element of send
    c_globalStop.notifySend();
    b_result = true;
  }
  return b_result;
}
/**
  check if a periodic stop command repetition
  should be sent and do this

  possible errors:
    * lbsSysNoActiveLocalMember on missing own active ident for sending the command
  @return true -> send of stop commands performed without send errors
*/
bool DINMonitor_c::sendPeriodicStop( void){
  // use given time or retreive actual system time if default val
  int32_t i32_time = System_c::getTime();
  bool b_result = true;


  /* ******************************** */
  /* perform some precondition checks */
  /* ******************************** */
  if (!getSystemMgmtInstance4Comm().existActiveLocalDinMember())
  { // no active ident which could send msg
    return false;
  }

  /* ******************** */
  /* perform send command */
  /* ******************** */
  DINItem_c& c_sender = getSystemMgmtInstance4Comm().getActiveLocalDinMember();

  if ((c_globalStop.active() || vec_stop.size() > 0) &&
      ((i32_time - i32_lastStopSent) >= 100) && c_sender.itemState(IState_c::ClaimedAddress))
  { // stop command must be sent
    if (c_globalStop.active())
    {
      sendStopIntern(c_sender.devKey(), 10, c_sender.nr(),0xF, c_globalStop.command());
      c_globalStop.notifySend();
    }
    else
    {
      for (ArrStopIterator c_stopIter = vec_stop.begin();
          c_stopIter != vec_stop.end();
          c_stopIter++)
      {
        if (c_stopIter->active())
        {
          if (sendStopIntern(c_sender.devKey(), 11, c_sender.nr(),
                c_stopIter->empf(), c_stopIter->command()))
          {
            c_stopIter->notifySend();
          }
          else
          { // send caused an error
            b_result = false;
            break; // stop try for further send -> break for-loop
          }
        }
      }
    }
  }
  return b_result;
}

/**
  internal inline function to send stop command with given devKey,send,verw,empf,xxx
  @param rc_devKey DEV_KEY of sending member identity
  @param rb_verw VERW code of the system command
  @param rb_send SEND code of the system command
  @param rb_empf EMPF code of the system command (target member no)
  @param rb_xxxx XXXX code of the system command
  @return true -> send without errors
*/
bool DINMonitor_c::sendStopIntern(const DevKey_c& rc_devKey, const uint8_t& rb_verw, const uint8_t& rb_send,
    const uint8_t& rb_empf, const uint8_t& rb_xxxx)
{
  data().setDevKey(rc_devKey);
  data().setVerw(rb_verw); // stop for specific member
  data().setSend(rb_send);
  data().setEmpf(rb_empf);
  data().setXxxx(rb_xxxx);
  // send it
  CANIO_c& c_can = getCanInstance4Comm();
  getLbsErrInstance().clear( LibErr_c::Can );
  c_can << data();
   // update stop command timer
  i32_lastStopSent = System_c::getTime();

  // return dependent on state of CAN_IO
  // set return val dependent on CAN state after send
  return (getLbsErrInstance().getErrCnt() == 0 )?true:false;
};
#endif

} // namespace __IsoAgLib
