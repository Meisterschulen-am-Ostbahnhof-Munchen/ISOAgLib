/***************************************************************************
                          isomonitor_c.cpp - object for monitoring members
                                              (list of ISOItem_c)
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
#include "isomonitor_c.h"
#include "isosystempkg_c.h"
#include <IsoAgLib/util/liberr_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>
#include "isorequestpgn_c.h"

#ifdef USE_PROCESS
  #include <IsoAgLib/comm/Process/impl/process_c.h>
#endif

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <IsoAgLib/util/impl/util_funcs.h>
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

/** this define controls the time interval between regular SA requests on the bus
 *  (set to 0 to stop any periodic SA requests)
 */
#define SA_REQUEST_PERIOD_MSEC 60000


#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_isoItemTotal = 0;
#endif

namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique ISOMonitor_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  ISOMonitor_c& getIsoMonitorInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return ISOMonitor_c::instance( rui8_instance );
  };
#else
  /** C-style function, to get access to the unique ISOMonitor_c singleton instance */
  ISOMonitor_c& getIsoMonitorInstance( void )
  {
    static ISOMonitor_c& c_isoMonitor = ISOMonitor_c::instance();
    return c_isoMonitor;
  };
#endif



/**
  constructor for ISOMonitor_c which can store optional pointer to central Scheduler_c instance
  @param rpc_lb optional pointer to central Scheduler_c instance (default NULL -> the later set is needed)
*/
ISOMonitor_c::ISOMonitor_c()
  : SingletonISOMonitor_c(), vec_isoMember()
{
  // functionality moved OUT of the constructor, as the constructor is NOT called in embedded systems for static class instances.
}



/**
  initialize directly after the singleton instance is created.
  this is called from singleton.h and should NOT be called from the user again.
  users please use init(...) instead.
*/
void
ISOMonitor_c::singletonInit()
{
  setAlreadyClosed(); // so init() will init ;-) (but only once!)
  init();
};



/** initialisation for ISOMonitor_c which can store optional pointer to central Scheduler_c instance
*/
void ISOMonitor_c::init( void )
{
  // only init if closed (constructor "closes" it so it gets init'ed initially!
  if (checkAlreadyClosed())
  {
    c_data.setSingletonKey( getSingletonVecKey() );
    #ifdef DEBUG_HEAP_USEAGE
    sui16_isoItemTotal -= vec_isoMember.size();
    #endif
    vec_isoMember.clear();
    pc_isoMemberCache = vec_isoMember.end();
    i32_lastSaRequest = -1; // not yet requested. Do NOT use 0, as the first "setLastRequest()" could (and does randomly) occur at time0 as it's called at init() time.
    c_tempIsoMemberItem.set( 0, DevKey_c::DevKeyUnspecified, 0xFE, IState_c::Active,
            0xFFFF, getSingletonVecKey() );

    // clear state of b_alreadyClosed, so that close() is called one time AND no more init()s are performed!
    clearAlreadyClosed();
    // register in Scheduler_c to be triggered fopr timeEvent
    getSchedulerInstance4Comm().registerClient( this );

    bool b_configure = false;

    // add filter REQUEST_PGN_MSG_PGN via ISORequestPGN_c
    getIsoRequestPgnInstance4Comm().registerPGN (*this, ADRESS_CLAIM_PGN); // request for adress claim

    if (getCanInstance4Comm().insertFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>((ADRESS_CLAIM_PGN)+0xFF) << 8), false, Ident_c::ExtendedIdent))
      b_configure = true;
    FilterBox_c* pc_filterBoxWsMaster
      = getCanInstance4Comm().insertFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MASTER_PGN) << 8), false, Ident_c::ExtendedIdent);
    if (pc_filterBoxWsMaster)
      b_configure = true;
    if (getCanInstance4Comm().insertFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MEMBER_PGN) << 8), false, Ident_c::ExtendedIdent), pc_filterBoxWsMaster)
      b_configure = true;

    if (b_configure) {
      getCanInstance4Comm().reconfigureMsgObj();
    }
  }
}


/** default destructor which has nothing to do */
ISOMonitor_c::~ISOMonitor_c()
{
  close();
}

/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void ISOMonitor_c::close( void ) {
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    getSchedulerInstance4Comm().unregisterClient( this );

    getIsoRequestPgnInstance4Comm().unregisterPGN (*this, ADRESS_CLAIM_PGN); // request for adress claim

    getCanInstance4Comm().deleteFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>((ADRESS_CLAIM_PGN)+0xFF) << 8), Ident_c::ExtendedIdent);
    getCanInstance4Comm().deleteFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MASTER_PGN) << 8), Ident_c::ExtendedIdent);
    getCanInstance4Comm().deleteFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MEMBER_PGN) << 8), Ident_c::ExtendedIdent);
  }
};


/**
  deliver reference to data pkg as reference to CANPkgExt_c
  to implement the base virtual function correct
*/
CANPkgExt_c& ISOMonitor_c::dataBase()
{
  return c_data;
}

/**
  performs periodically actions,

  possible errors:
    * pertial error caused by one of the memberItems

  @return true -> all planned activities performed in allowed time
*/
bool ISOMonitor_c::timeEvent( void ){
  #if CONFIG_ISO_ITEM_MAX_AGE > 0
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

  if ( getSystemMgmtInstance4Comm().existActiveLocalMember() )
  { // use the SA of the already active node
    data().setIsoSa(getSystemMgmtInstance4Comm().getActiveLocalMember().nr());
  }


  if ( lastIsoSaRequest() == -1) return true;
  else if ( getSystemMgmtInstance4Comm().existActiveLocalMember() )
  { // we could send the next SA request
    const int32_t ci32_timePeriod = SA_REQUEST_PERIOD_MSEC
        + ( ( getSystemMgmtInstance4Comm().getActiveLocalMember().nr() % 0x80 ) * 1000 );
    // the request interval takes the number of the SA into account, so that nodes with higher
    // SA should receive the request of this node before they decide to send a request on their own
    // ==> MIN INTERVAL is SA_REQUEST_PERIOD_MSEC
    // ==> MAX INTERVAL is (SA_REQUEST_PERIOD_MSEC + (0xFF % 0x80) ) == ( SA_REQUEST_PERIOD_MSEC + 0x7F )
    if ( ( HAL::getTime() - lastIsoSaRequest() ) > ci32_timePeriod )
    { // it's time for the next SA request in case we have already one
      sendRequestForClaimedAddress( true );
    }
  }

  int32_t i32_now = Scheduler_c::getLastTimeEventTrigger();
  const int32_t ci32_timeSinceLastAdrClaimRequest = (i32_now - lastIsoSaRequest());
  bool b_reqeustAdrClaim = false;
  if ( ci32_timeSinceLastAdrClaimRequest > CONFIG_ISO_ITEM_MAX_AGE )
  { // the last request is more than CONFIG_ISO_ITEM_MAX_AGE ago
    // --> each client MUST have answered until now if it's still alive
    for(Vec_ISOIterator pc_iter = vec_isoMember.begin(); pc_iter != vec_isoMember.end();)
    { // delete item, if it didn't answer longer than CONFIG_ISO_ITEM_MAX_AGE since last adress claim request
      if ( ( (pc_iter->lastTime()+CONFIG_ISO_ITEM_MAX_AGE) < lastIsoSaRequest() )
        && ( !(pc_iter->itemState(IState_c::Local))   ) )
      { // its last AdrClaim is too old - it didn't react on the last request
        // was it too late for the first time??
        // special case: when the rate of ReqForAdrClaimed is at about CONFIG_ISO_ITEM_MAX_AGE
        // a node migth answer just in time to the _previous_ request, which is in turn _before_ the last
        // detected request on BUS
        // -->> regard the client only as stale, when the last AdrClaim was longer than CONFIG_ISO_ITEM_MAX_AGE
        //      before the newest ReqForAdrClaimed on BUS
        if ( pc_iter->itemState( IState_c::PossiblyOffline) )
        { // it's too late the second time -> remove it
          Vec_ISOIterator pc_iterDelete = pc_iter;
          #ifdef DEBUG_HEAP_USEAGE
          sui16_isoItemTotal--;

          getRs232Instance()
            << sui16_isoItemTotal << " x ISOItem_c: Mal-Alloc: "
            <<  sizeSlistTWithMalloc( sizeof(ISOItem_c), sui16_isoItemTotal )
            << "/" << sizeSlistTWithMalloc( sizeof(ISOItem_c), 1 )
            << ", Chunk-Alloc: "
            << sizeSlistTWithChunk( sizeof(ISOItem_c), sui16_isoItemTotal )
            << "\r\n\r\n";
          #endif
          pc_iter = vec_isoMember.erase(pc_iterDelete); // erase returns iterator to next element after the erased one
        }
        else
        { // give it another chance
          pc_iter->setItemState( IState_c::PossiblyOffline );
          b_reqeustAdrClaim = true;
        }
      } else  {
        pc_iter++;
      }
    } // for
    if ( b_reqeustAdrClaim )
    { // at least one node needs an additional adr claim
      sendRequestForClaimedAddress( true );
    }
  } // if
  #endif
  return true;
}


/**
  deliver the count of members in the Monitor-List with given ECU-Type (which is an own IsoAgLib-definition!)
  which optional (!!) match the condition of address claim state
  @param rui8_ecuType searched ECU-Type code
  @param rb_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return count of members in Monitor-List with ECU-Type == rui8_ecuType
*/
uint8_t ISOMonitor_c::isoMemberEcuTypeCnt (ISOName_c::ecuType_t r_ecuType, bool rb_forceClaimedAddress)
{
  uint8_t b_result = 0;
  for (Vec_ISOIterator pc_iter = vec_isoMember.begin() ; pc_iter != vec_isoMember.end(); pc_iter++)
  {
    if ( ((pc_iter->devKey().getConstName().getEcuType() == r_ecuType))
      && (!rb_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      b_result++;
      pc_isoMemberCache = pc_iter; // set member cache to member  with searched devClass
    }
  }
  return b_result;
}

/**
  deliver one of the members with specific ECU_Type (which is an own IsoAgLib-definition!)
  which optional (!!) match the condition of address claim state
  check first with isoMemberEcuTypeCnt if enough members with wanted ECU-Type and
  optional (!!) property are registered in Monitor-List
  @see isoMemberEcuTypeCnt

  possible errors:
    * Err_c::range there exist less than rui8_ind members with ECU-Type rui8_ecuType
  @param rui8_ecuType searched ECU-Type code
  @param rui8_ind position of the wanted member in the
                sublist of member with given ECU-Type (first item has rui8_ind == 0 !!)
  @param rb_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return reference to searched element
*/
ISOItem_c& ISOMonitor_c::isoMemberEcuTypeInd (ISOName_c::ecuType_t r_ecuType, uint8_t rui8_ind, bool rb_forceClaimedAddress)
{
  int8_t c_cnt = -1;
  for (Vec_ISOIterator pc_iter  = vec_isoMember.begin() ; pc_iter != vec_isoMember.end(); pc_iter++)
  {
    if ( ((pc_iter->devKey().getConstName().getEcuType()) == r_ecuType)
      && (!rb_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      c_cnt++;
      if (c_cnt == rui8_ind)
      {
        pc_isoMemberCache = pc_iter; // set member cache to member  with searched devClass
        break; //searched Item found (first element has 0)break; //searched Item found (first element has 0)
      }
    }
  }
  // check if rui8_ind was in correct range
  if (rui8_ind != c_cnt)
  { // wrong range of rui8_ind
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::LbsSystem );
  }
  return *pc_isoMemberCache;
}



/**
  deliver the count of members in the Monitor-List with given DEVCLASS (variable POS)
  which optional (!!) match the condition of address claim state
  @param rui8_devClass searched DEVCLASS code
  @param rb_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return count of members in Monitor-List with DEVCLASS == rui8_devClass
*/
uint8_t ISOMonitor_c::isoMemberDevClassCnt(uint8_t rui8_devClass, bool rb_forceClaimedAddress)
{
  uint8_t b_result = 0;
  for (Vec_ISOIterator pc_iter = vec_isoMember.begin() ; pc_iter != vec_isoMember.end(); pc_iter++)
  {
//    cerr << "Tested Member DevClass: " << int( pc_iter->devKey().getDevClass() ) << endl;
    if ( ( ((pc_iter->devKey().getDevClass()) == rui8_devClass) || (rui8_devClass == 0xFF))
      && (!rb_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      b_result++;
      pc_isoMemberCache = pc_iter; // set member cache to member  with searched devClass
    }
  }
  return b_result;
}

/**
  deliver one of the members with specific DEVCLASS
  which optional (!!) match the condition of address claim state
  check first with isoMemberDevClassCnt if enough members with wanted DEVCLASS and
  optional (!!) property are registered in Monitor-List
  @see isoMemberDevClassCnt

  possible errors:
    * range there exist less than rui8_ind members with DEVCLASS rui8_devClass
 @param rui8_devClass searched DEVCLASS
 @param rui8_ind position of the wanted member in the
               sublist of member with given DEVCLASS (first item has rui8_ind == 0 !!)
 @param rb_forceClaimedAddress true -> only members with claimed address are used
       (optional, default false)
 @return reference to searched element
*/
ISOItem_c& ISOMonitor_c::isoMemberDevClassInd(uint8_t rui8_devClass, uint8_t rui8_ind, bool rb_forceClaimedAddress)
{
  int8_t c_cnt = -1;
  for (Vec_ISOIterator pc_iter  = vec_isoMember.begin() ; pc_iter != vec_isoMember.end(); pc_iter++)
  {
    if ( ( ((pc_iter->devKey().getDevClass()) == rui8_devClass) || (rui8_devClass == 0xFF))
      && (!rb_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      c_cnt++;
      if (c_cnt == rui8_ind)
      {
        pc_isoMemberCache = pc_iter; // set member cache to member  with searched devClass
        break; //searched Item found (first element has 0)break; //searched Item found (first element has 0)
      }
    }
  }
  // check if rui8_ind was in correct range
  if (rui8_ind != c_cnt)
  { // wrong range of rui8_ind
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::LbsSystem );
  }
  return *pc_isoMemberCache;
}

/**
  check if a memberItem with given DEV_KEY exist
  which optional (!!) match the condition of address claim state
  and update local pc_isoMemberCache
  @param rc_devKey searched DEV_KEY
  @param rb_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return true -> searched member exist
*/
bool ISOMonitor_c::existIsoMemberDevKey(const DevKey_c& rc_devKey, bool rb_forceClaimedAddress)
{
  if (!vec_isoMember.empty() && (pc_isoMemberCache != vec_isoMember.end()))
  {
    if ( (pc_isoMemberCache->devKey() == rc_devKey )
      && (!rb_forceClaimedAddress || pc_isoMemberCache->itemState(IState_c::ClaimedAddress))
        )  return true;
  }
  for (pc_isoMemberCache = vec_isoMember.begin();
       pc_isoMemberCache != vec_isoMember.end();
       pc_isoMemberCache++)
  {
    if ( (pc_isoMemberCache->devKey() == rc_devKey )
      && (!rb_forceClaimedAddress || pc_isoMemberCache->itemState(IState_c::ClaimedAddress))
        )  return true;
  };
  // if reaching here -> nothing found
  return false;
};

/**
  check if a member with given number exist
  which optional (!!) match the condition of address claim state
  and update local pc_isoMemberCache
  @param rui8_nr searched member number
  @return true -> item found
*/
bool ISOMonitor_c::existIsoMemberNr(uint8_t rui8_nr)
{
  if (!vec_isoMember.empty() && (pc_isoMemberCache != vec_isoMember.end()))
  {
    if ( pc_isoMemberCache->nr() == rui8_nr ) return true;
  }
  for (pc_isoMemberCache = vec_isoMember.begin();
       pc_isoMemberCache != vec_isoMember.end();
       pc_isoMemberCache++)
  {
    if (pc_isoMemberCache->equalNr(rui8_nr)) return true;
  }
  return false;
};


/**
  check if member is in member list with wanted DEV_KEY,
  adapt instance if member with claimed address with other device class inst exist
  @param refc_devKey DEV_KEY to search (-> it's updated if member with claimed address with other dev class inst is found)
  @return true -> member with claimed address with given DEVCLASS found (and refc_devKey has now its DEV_KEY)
*/
bool ISOMonitor_c::isoDevClass2DevKeyClaimedAddress(DevKey_c &refc_devKey)
{
  if (existIsoMemberDevKey(refc_devKey, true))
  { // there exists a device with exact NAME in claimed address state
    return true;
  }
  else
  { // no item with DEV_KEY found -> adapt POS
    // search for member with claimed address with same DEVCLASS
    if (isoMemberDevClassCnt(refc_devKey.getDevClass(), true) > 0)
    { // member with wanted device class exists -> store the DEVKEY
      refc_devKey = isoMemberDevClassInd(refc_devKey.getDevClass(), 0, true).devKey();
      return true;
    }
    else if (isoMemberDevClassCnt(refc_devKey.getDevClass(), false) > 0)
    { // member with wanted device class exists -> store the DEVKEY
      refc_devKey = isoMemberDevClassInd(refc_devKey.getDevClass(), 0, false).devKey();
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
  insert a new ISOItem_c in the list; with unset rui8_nr the member is initiated as
  address claim state; otherwise the given state can be given or state Active is used

  possible errors:
    * badAlloc not enough memory to insert new ISOItem_c isntance
    * busy another member with same ident exists already in the list

  @param rc_devKey DEV_KEY of the member
  @param rui8_nr member number
  @param rui16_saEepromAdr EEPROM adress to store actual SA -> next boot with same adr
  @param ren_status wanted status
  @return pointer to new ISOItem_c or NULL if not succeeded
*/
ISOItem_c* ISOMonitor_c::insertIsoMember(const DevKey_c& rc_devKey,
      uint8_t rui8_nr, IState_c::itemState_t ren_state, uint16_t rui16_saEepromAdr)
{
  ISOItem_c* pc_result = NULL;

  // check if another ISOItem_c with same DEV_KEY already exist
  if (existIsoMemberDevKey(rc_devKey))
  { // another member with same DEV_KEY found
    getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::LbsSystem );
    return NULL; // don't insert
  }

  // FROM NOW ON WE DECIDE TO (TRY TO) CREATE A NEW ISOItem_c
  // prepare temp item with wanted data
  c_tempIsoMemberItem.set(System_c::getTime(), rc_devKey, rui8_nr,
      IState_c::itemState_t(ren_state | IState_c::Member | IState_c::Iso | IState_c::Active),
      rui16_saEepromAdr, getSingletonVecKey() );

  // now insert element
  const uint8_t b_oldSize = vec_isoMember.size();
  vec_isoMember.push_front(c_tempIsoMemberItem);
  pc_isoMemberCache = vec_isoMember.begin();
  if (vec_isoMember.size() <= b_oldSize)
  { // array didn't grow
    getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsSystem );
  }
  else
  { // item was inserted
    pc_result = &(*pc_isoMemberCache);
    #ifdef DEBUG_HEAP_USEAGE
    sui16_isoItemTotal++;

    getRs232Instance()
      << sui16_isoItemTotal << " x ISOItem_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(ISOItem_c), sui16_isoItemTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(ISOItem_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(ISOItem_c), sui16_isoItemTotal )
      << "\r\n\r\n";
    #endif
  }
  vec_isoMember.sort(); // resort the list

  return pc_result;
};

/** register a SaClaimHandler_c */
bool ISOMonitor_c::registerSaClaimHandler( SaClaimHandler_c* rpc_client )
{
  for ( SaClaimHandlerVectorConstIterator_t iter = vec_saClaimHandler.begin(); iter != vec_saClaimHandler.end(); iter++ )
  { // check if it points to the same client
    if ( *iter == rpc_client ) return true; // already in multimap -> don't insert again
  }
  const unsigned int oldSize = vec_saClaimHandler.size();
  // if this position is reached, a new item must be inserted
  vec_saClaimHandler.push_back( rpc_client );

  return ( vec_saClaimHandler.size() > oldSize )?true:false;
}


/** deregister a SaClaimHandler */
bool
ISOMonitor_c::deregisterSaClaimHandler (SaClaimHandler_c* rpc_client)
{
  const unsigned int oldSize = vec_saClaimHandler.size();
  for ( SaClaimHandlerVectorIterator_t iter = vec_saClaimHandler.begin(); iter != vec_saClaimHandler.end(); iter++ )
  { // check if it points to the same client
    if ( *iter == rpc_client )
    {
      vec_saClaimHandler.erase (iter); // in multimap -> so delete it
      break;
    }
  }
  return (vec_saClaimHandler.size() > oldSize)?true:false;
}


/** this function is used to broadcast a ISO monitor list change to all registered clients */
void ISOMonitor_c::broadcastSaAdd2Clients( const DevKey_c& rc_devKey, const ISOItem_c* rpc_isoItem ) const
{
  for ( SaClaimHandlerVectorConstIterator_t iter = vec_saClaimHandler.begin(); iter != vec_saClaimHandler.end(); iter++ )
  { // call the handler function of the client
    (*iter)->reactOnMonitorListAdd( rc_devKey, rpc_isoItem );
  }
}
/** this function is used to broadcast a ISO monitor list change to all registered clients */
void ISOMonitor_c::broadcastSaRemove2Clients( const DevKey_c& rc_devKey, uint8_t rui8_oldSa ) const
{
  for ( SaClaimHandlerVectorConstIterator_t iter = vec_saClaimHandler.begin(); iter != vec_saClaimHandler.end(); iter++ )
  { // call the handler function of the client
    (*iter)->reactOnMonitorListRemove( rc_devKey, rui8_oldSa );
  }
}

/**
  deliver member item with given devKey
  (check with existIsoMemberDevKey before access to not defined item)

  possible errors:
    * elNonexistent on failed search

  @param rc_devKey searched DEV_KEY
  @return reference to searched ISOItem
  @exception containerElementNonexistant
*/
ISOItem_c& ISOMonitor_c::isoMemberDevKey(const DevKey_c& rc_devKey, bool rb_forceClaimedAddress)
{
  if (existIsoMemberDevKey(rc_devKey, rb_forceClaimedAddress))
  { // no error
    return static_cast<ISOItem_c&>(*pc_isoMemberCache);
  }
  else
  { // wanted element not found
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );

    // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    //return reference to first element as fallback
    return vec_isoMember.front();
  }
};

/**
  deliver member item with given nr
  (check with existIsoMemberNr before access to not defined item)

  possible errors:
    * elNonexistent on failed search

  @param rui8_nr searched number
  @return reference to searched ISOItem
  @exception containerElementNonexistant
*/
ISOItem_c& ISOMonitor_c::isoMemberNr(uint8_t rui8_nr)
{
  if (existIsoMemberNr(rui8_nr))
  { // no error
    return static_cast<ISOItem_c&>(*pc_isoMemberCache);
  }
  else
  { // wanted element not found
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );

    // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    //return reference to first element as fallback
    return vec_isoMember.front();
  }
};

/**
  deliver member item with given DEV_KEY, set pointed bool var to true on success
  and set a Member Array Iterator to the result
  @param rc_devKey searched DEV_KEY
  @param pb_success bool pointer to store the success (true on success)
  @param pbc_iter optional member array iterator which points to searched ISOItem_c on success
  @return reference to the searched item
*/
ISOItem_c& ISOMonitor_c::isoMemberDevKey(const DevKey_c& rc_devKey, bool *const pb_success, bool rb_forceClaimedAddress, Vec_ISOIterator *const pbc_iter)
{
  *pb_success = (existIsoMemberDevKey(rc_devKey, rb_forceClaimedAddress))?true:false;

  if (pbc_iter != NULL)
  {
    *pbc_iter = pc_isoMemberCache;
  }
  return static_cast<ISOItem_c&>(*pc_isoMemberCache);
};

/**
  delete item with specified devKey

  possible errors:
    * elNonexistent no member with given DEV_KEY exists

  @param rc_devKey DEV_KEY of to be deleted member
*/
bool ISOMonitor_c::deleteIsoMemberDevKey(const DevKey_c& rc_devKey)
{
  if (existIsoMemberDevKey(rc_devKey))
  { // set correct state

    #ifdef USE_WORKING_SET
    // Are we deleting a WorkingSetMaster?
    if (pc_isoMemberCache->isMaster())
    { // yes, so notify all slaves that they're now standalone!
      notifyOnWsMasterLoss (*pc_isoMemberCache);
    }
    #endif

    // erase it from list (existIsoMemberDevKey sets pc_isoMemberCache to the wanted item)
    vec_isoMember.erase(pc_isoMemberCache);
    #ifdef DEBUG_HEAP_USEAGE
    sui16_isoItemTotal--;

    getRs232Instance()
      << sui16_isoItemTotal << " x ISOItem_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(ISOItem_c), sui16_isoItemTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(ISOItem_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(ISOItem_c), sui16_isoItemTotal )
      << "\r\n\r\n";
    #endif
    pc_isoMemberCache = vec_isoMember.begin();
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

  @param rui8_nr SA of to be deleted member
*/
bool ISOMonitor_c::deleteIsoMemberNr(uint8_t rui8_nr)
{
  if (existIsoMemberNr(rui8_nr))
  { // use deleteIsoMemberDevKey
    return deleteIsoMemberDevKey(pc_isoMemberCache->devKey());
  }
  else
  { // to be deleted member number does not exist
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );
    return false;
  }
};


/**
  change devKey if actual devKey isn't unique
  (search possible free instance to given device class)

  possible errors:
    * busy no other device class inst code leads to unique DEV_KEY code

  @param refc_devKey reference to DEV_KEY var (is changed directly if needed!!)
  @return true -> referenced DEV_KEY is now unique
*/
bool ISOMonitor_c::unifyIsoDevKey(DevKey_c& refc_devKey){
  bool b_result = true;
  DevKey_c c_tempDevKey = refc_devKey;
  if (existIsoMemberDevKey(refc_devKey))
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
        if (!(existIsoMemberDevKey(c_tempDevKey)))
        {  // (tempPos + diff) can't be found in list -> it is unique
          refc_devKey.setDevClassInst( tempPos + diff );
          b_result = true;
          break;
        }
      }
      if (tempPos - diff >= 0)
      { // (tempPos - diff) would be an allowed device class inst code
        c_tempDevKey.setDevClassInst( tempPos - diff );
        if (!(existIsoMemberDevKey(c_tempDevKey)))
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
};
/**
  check if SA of an announcing ISOItem_c is unique and deliver
  another free SA if not yet unique (else deliver its actual
  SA if unique yet)
  @param rpc_isoItem pointer to announcing ISOItem_c
  @return free unique SA (if possible the actual SA of the pointed item)
    (if wanted SA is not free for NOT-self-conf item or if no free SA is available
     254 is answered -> special flag for NACK)
*/
uint8_t ISOMonitor_c::unifyIsoSa(const ISOItem_c* rpc_isoItem)
{
  uint8_t ui8_wishSa = rpc_isoItem->nr();
  bool b_free = true;

  if ((ui8_wishSa < 128) && (rpc_isoItem->selfConf() == 1))
  { // false SA adress -> correct
    ui8_wishSa = 128;
  }

  if (ui8_wishSa != 254)
  {
    for (Vec_ISOIterator pc_iterItem = vec_isoMember.begin();
          pc_iterItem != vec_isoMember.end(); pc_iterItem++)
    {
      if ((pc_iterItem->nr() == ui8_wishSa)
        &&(&(*pc_iterItem) != rpc_isoItem)
         )
      { // same SA, rpc_isoItem had special SA wish and actual
        // item is different from calling ISOItem
        b_free = false;
        break;
      }
    }
    if (b_free) return ui8_wishSa;
  }
  if (!(rpc_isoItem->selfConf()))
  { // wanted fixed SA not free -> signal this with 254
    return 254;
  }
  // now wished SA isn't free or no SA wished
  for (ui8_wishSa = 128; ui8_wishSa <= 207; ui8_wishSa++)
  { // try all possible self conf adresses
    b_free = true;
    for (Vec_ISOIterator pc_iterItem = vec_isoMember.begin();
          pc_iterItem != vec_isoMember.end(); pc_iterItem++)
    {
      if (pc_iterItem->nr() == ui8_wishSa)
      { // the tried SA is already used by this item
        // -> break this search loop and try another ui8_wishSa
        b_free = false;
        break;
      }
    }

    if (b_free)
    { // free SA found
      return ui8_wishSa;
    }
  }
  // no free SA found -> return 254 as signal
  return 254;
}

/**
  trigger a request for claimed addreses
  @param rb_force false -> send request only if no request was detected until now
  @return true -> request was sent
  */
bool ISOMonitor_c::sendRequestForClaimedAddress( bool rb_force )
{ // trigger an initial request for claimed address
  // send no request if CANIO_c is not yet ready for send
  if ( ! getCanInstance4Comm().isReady2Send() ) return false;
  // ( only if no request was detected )
  if ( ( lastIsoSaRequest() != -1 ) && ( ! rb_force ) )
  { // at least one request was already detected
    return false;
  }
  bool b_sendOwnSa = false;
  // now it's needed to send
//  int32_t i32_time = Scheduler_c::getLastTimeEventTrigger();
  const int32_t i32_time = System_c::getTime();

//  getRs232Instance() << "_time in sendReq4AdrCl: " << HAL::getTime() <<"_";

  data().setIsoPri(6);
  data().setIsoPgn(REQUEST_PGN_MSG_PGN);
  data().setIsoPs(255); // global request
  if ( getSystemMgmtInstance4Comm().existActiveLocalMember() )
  { // use the SA of the already active node
    data().setMonitorItemForSA( &getSystemMgmtInstance4Comm().getActiveLocalMember() );
    b_sendOwnSa = true;
  }
  else
  { // no local ident has claimed an adress so far
    data().setIsoSa(254); // special flag for "no SA yet"
  }
  // built request data string
  uint8_t pb_requestString[4];
  pb_requestString[0] = (ADRESS_CLAIM_PGN & 0xFF);
  pb_requestString[1] = ((ADRESS_CLAIM_PGN >> 8)& 0xFF);
  pb_requestString[2] = ((ADRESS_CLAIM_PGN >> 16)& 0xFF);
  data().setDataString(pb_requestString, 3);
  // now ISOSystemPkg_c has right data -> send
  getCanInstance4Comm() << data();
  // store adress claim request time
  setLastIsoSaRequest(i32_time);

  // now send own SA in case at least one local ident has yet claimed adress
  if (b_sendOwnSa)
  {
    #ifdef DEBUG
    EXTERNAL_DEBUG_DEVICE << "Send checking SA request (sendRequestForClaimedAddress())" << EXTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    const uint8_t cui8_localCnt = getSystemMgmtInstance4Comm().localIsoMemberCnt();
    for ( uint8_t ui8_ind = 0; ui8_ind < cui8_localCnt; ui8_ind++ )
    { // the function ISOItem_c::sendSaClaim() checks if this item is local and has already claimed a SA
      getSystemMgmtInstance4Comm().localIsoMemberInd( ui8_ind ).sendSaClaim();
    }
  }
  return true;
}


/**
  process system msg with informations which are
  important for managing of members
  @return true -> message processed by ISOMonitor_c; false -> process msg by ServiceMonitor
*/
bool ISOMonitor_c::processMsg()
{
  bool b_processed = false;

  ISOItem_c *pc_item = NULL,
            *pc_itemSameSa = NULL,
            *pc_itemSameDevKey = NULL;

  // Handle DESTINATION PGNs
  switch ((data().isoPgn() & 0x1FF00))
  {
    case ADRESS_CLAIM_PGN: // adress claim
      if ( existIsoMemberDevKey( data().devKey()) )
      {
        pc_itemSameDevKey = &(isoMemberDevKey(data().devKey()));
      }
      if ( existIsoMemberNr(data().isoSa()) )
      {
        pc_itemSameSa = &(isoMemberNr(data().isoSa()));
      }

      // FIRST REMOVE any REMOTE ISOItem_c with SAME SA but different DevKey_c
      if ( ( NULL != pc_itemSameSa                       )
        && ( ! pc_itemSameSa->itemState(IState_c::Local) )
        && ( pc_itemSameSa->devKey() != data().devKey()  ) )
      { // REMOVE the node that is pointed by pc_itemSameSa
        // - the destructor of the ISOItem_c informs all handlers about the loss
        deleteIsoMemberNr( data().isoSa() );
        pc_itemSameSa = NULL;
      }

      // SECOND: trigger total restart of ADR claim for any local ISOItem_c with
      //         SAME DevKey_c
      if ( ( NULL != pc_itemSameDevKey ) && ( pc_itemSameDevKey->itemState(IState_c::Local)) )
      { // in ANY case - when another node sends SA claim with same NAME we MUST restart
        // as nobody else on the network will detect the NAME clash
        // - this restart triggers also the REMOVAL of the item pc_itemSameDevKey
        if ( ( NULL != pc_itemSameSa ) && ( pc_itemSameSa->itemState( IState_c::Local ) ) )
        { // there is also a SA conflict with a local ISOItem_c
          if ( pc_itemSameSa != pc_itemSameDevKey )
          { // the received adr claim overlaps two different local ISOItem_c --> restart BOTH
            // (even if pc_itemSameSa would have higher prio - this is needed, as the new remote item would be in conflict by SA with
            //  the currently existing local pc_itemSameSa )
            getSystemMgmtInstance4Comm().restartAddressClaim( pc_itemSameSa->devKey() );
          }
          // in case of sort of SA conflict with a local ISOItem_c, we must avoid a later reaction on this SA conflict
          // => set the pointer to NULL to indicate the handled conflict
          pc_itemSameSa = NULL;
        }
        // the case of a conflict on SA with a remote ISOItem_c is handled elsewhere as this block is only directed to handle
        // conflicts with LOCAL ISOItem_c
        getSystemMgmtInstance4Comm().restartAddressClaim( pc_itemSameDevKey->devKey() );
        pc_itemSameDevKey = NULL;
        // DO NOT set b_processed to true as the SA CLAIM shall trigger creation of fresh remote ISOItem_c
      }

      // THIRD: handle LOCAL item that has same SA
      if ( ( NULL != pc_itemSameSa ) && ( pc_itemSameSa->itemState(IState_c::Local)) )
      { // there exists a LOCAL item with same SA
        // --> remove it when it has lower PRIO
        int8_t i8_higherPrio = pc_itemSameSa->devKey().getConstName().higherPriThanPar(data().name());
        if ( ( i8_higherPrio < 0                              )
               // the local ISOItem_c should be overwritten by the received adr claim, when the local
               // item is not yet sent any adr claim (e.g. Off state or PreAddressClaim)
               || ( ! pc_itemSameSa->itemStatePartialMatch(IState_c::itemState_t(IState_c::ClaimedAddress | IState_c::AddressClaim)) ) )
        { // the LOCAL item has lower PRIO or has not yet fully claimed --> remove it
          // the function SystemMgmt_c::restartAddressClaim() triggers removal of ISOItem_c
          // and registers the next address claim try afterwards
          getSystemMgmtInstance4Comm().restartAddressClaim( pc_itemSameSa->devKey() );
          pc_itemSameSa = NULL;
        }
        else
        { // let local ISOItem_c process the conflicting adr claim
          // --> the ISOItem_c::processMsg() will send an ADR CLAIM to indicate the higher prio
          pc_itemSameSa->processMsg();
          // set b_processed to TRUE, so that the next case FOUR is NOT active
          // ==>> in case the remote ISOItem_c of the CAN message sender (key NAME)
          //      exists already with _currently_ another SA in the ISOItem_c
          //      WE DECIDE TO IGNORE THE SA CLAIM which would lead to a conflicting SA
          //      ( in case our local ISOItem_c has higher PRIO )
          b_processed = true;
        }
      }

      // FOUR handle case where remote node has SAME DevKey_c (i.e. NAME)
      if ( ( NULL != pc_itemSameDevKey                       )
        && ( ! pc_itemSameDevKey->itemState(IState_c::Local) )
        && ( ! b_processed                                   ) ///< this message has not yet been processed
         )
      { // there exists a REMOTE item with same DevKey_c
        // --> simply let this item process
        if ( pc_itemSameDevKey->processMsg() ) b_processed = true;
      }


      // create NEW ISOItem_c in case no ISOItem_c with Pkg.DevKey_c exists
      // and the message has not yet been marked as processed
      if ( ( NULL == pc_itemSameDevKey ) && ( ! b_processed ) )
      { // create a suitable ISOItem_c and let it process
        ISOItem_c* pc_newItem = insertIsoMember(data().devKey(), data().isoSa(),
                              IState_c::itemState_t(IState_c::AddressClaim));
        if ( NULL != pc_newItem )
        { // new entry has been created
          if ( pc_newItem->processMsg() ) b_processed = true;
        }
      }

      break;
#ifdef USE_PROCESS
    case PROCESS_DATA_PGN:
      static_cast<__IsoAgLib::CANPkg_c&>(getProcessInstance4Comm().data())
        = static_cast<__IsoAgLib::CANPkg_c&>(data());
      getProcessInstance4Comm().data().string2Flags();
      return getProcessInstance4Comm().processMsg();
#endif
  } // end switch for DESTINATION pgn


  // Handle NON-DESTINATION PGNs
  switch ((data().isoPgn() /* & 0x1FFFF */ )) // isoPgn is already "& 0x1FFFF" !
  {
    #ifdef USE_WORKING_SET
    case WORKING_SET_MASTER_PGN: // working set master
      b_processed = true;
      if (existIsoMemberNr(data().isoSa()))
      { // ISOItem_c with same SA has to exist!
        ISOItem_c *pc_itemMaster = &(isoMemberNr(data().isoSa()));
        pc_itemMaster->setMaster (pc_itemMaster); // set item as master
        /** @todo IGNORE THAT WE GOT x MEMBERS FOR NOW
            LATER WE HAVE TO BE SURE THAT ALL THOSE x MEMBER DEFINITIONS REALLY ARRIVED!!
            for timings see Iso11783-Part1
          */
      }
      else
      {
        // shouldn't happen!
      }
      break;
    case WORKING_SET_MEMBER_PGN: // working set member
      if (existIsoMemberNr(data().isoSa()))
      { // ISOItem_c with same SA exists (THE SA IS THE MASTER!)
        ISOItem_c *pc_itemMaster = &(isoMemberNr(data().isoSa()));
        // the working set master places the NAME field of each children
        // in the data part of this message type
        pc_item = &(isoMemberDevKey(data().devKey()));
        pc_item->setMaster (pc_itemMaster); // set master on this isoItem
        b_processed = true;
      }
      else
      {
        // shouldn't happen!
      }
      break;
    #endif
    default:
      break;
  } // end switch for NON-DESTINATION pgn

  return b_processed; // return if msg was processed by ISOMonitor_c
}


bool
ISOMonitor_c::processMsgRequestPGN (uint32_t /*rui32_pgn*/, uint8_t /*rui8_sa*/, uint8_t rui8_da)
{
  // Only handling ADRESS_CLAIM_PGN here, no other RequestPGNs registered
  // if handling multiple PGNs, insert a switch statement
  // update time of last adress claim request
  setLastIsoSaRequest (getIsoRequestPgnInstance4Comm().getTimeOfLastRequest()); // Now using CAN-Pkg-Times, see header for "setLastIsoSaRequest" for more information!
  // don't break because default handling is true for
  // adress claim request, too
  // if isoPs is 255 let all local item answer
  if (rui8_da == 255)
  {
    bool b_processedRequestPGN = false;
    for (Vec_ISOIterator pc_iterItem = vec_isoMember.begin();
          pc_iterItem != vec_isoMember.end(); pc_iterItem++)
    { // let all local pc_iterItem process process this request
      if (pc_iterItem->itemState(IState_c::Local))
        b_processedRequestPGN |= pc_iterItem->sendSaClaim();
    }
    return b_processedRequestPGN; //return value doesn't matter, because the request was for GLOBAL (255), so it isn't NACKed anyway
  }
  else
  { // check if item with SA == isoPs exist and let it process
    // if local
    if (existIsoMemberNr(rui8_da))
    { // check if local
      if (isoMemberNr(rui8_da).itemState(IState_c::Local))
        return isoMemberNr(rui8_da).sendSaClaim();
    }
    return false; // this case can't be reached, as isorequestpgn_c won't call us, if it wouldn't knew (by us :-) that we have such a local member!
  }
};


#ifdef USE_WORKING_SET
uint8_t ISOMonitor_c::getSlaveCount (ISOItem_c* rpc_masterItem)
{
  uint8_t slaveCount;
  ISOItem_c* pc_iterMaster;
  // iterate through all items and check if it has the master set to us
  // and is not the master itself!
  slaveCount = 0;
  for(Vec_ISOIterator pc_iter = vec_isoMember.begin(); pc_iter != vec_isoMember.end(); pc_iter++)
  {
    pc_iterMaster = pc_iter->getMaster ();
    if ((& (*pc_iter) != rpc_masterItem) && (pc_iterMaster == rpc_masterItem)) {
      slaveCount++;
    }
  }
  return slaveCount;
}


ISOItem_c* ISOMonitor_c::getSlave (uint8_t index, ISOItem_c* rpc_masterItem)
{
  uint8_t slaveCount;
  ISOItem_c* pc_iterMaster;
  // iterate through all items and check if it the (desired) Xth item
  // if so, break and return it, else return NULL
  slaveCount = 0;
  for(Vec_ISOIterator pc_iter = vec_isoMember.begin(); pc_iter != vec_isoMember.end(); pc_iter++)
  {
    pc_iterMaster = pc_iter->getMaster ();
    if ((& (*pc_iter) != rpc_masterItem) && (pc_iterMaster == rpc_masterItem)) {
      if (slaveCount == index) return & (*pc_iter);
      slaveCount++;
    }
  }
  return NULL;
}

void
ISOMonitor_c::notifyOnWsMasterLoss (ISOItem_c& rrefc_masterItem)
{
  // iterate through all items and check if they are slaves to the given master
  for(Vec_ISOIterator pc_iter = vec_isoMember.begin(); pc_iter != vec_isoMember.end(); pc_iter++)
  { // ALSO notify the master, as this function is NOT ONLY called from ~ISOItem_c(), but also
    // intentionally from vtDocument_c to get the document NACKed!
    if (pc_iter->getMaster() == (&rrefc_masterItem))
    { // this ISOItem has the given MasterIsoItem as Master, so set it free now ;)
      pc_iter->setMaster (NULL); // for ALL WS-Members (Master AND Slave)
    }
  }
}
#endif

} // end of namespace __IsoAgLib
