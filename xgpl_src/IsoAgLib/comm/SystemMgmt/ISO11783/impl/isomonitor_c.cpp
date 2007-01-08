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
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include "isorequestpgn_c.h"
#include <IsoAgLib/driver/system/impl/system_c.h>

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
  }
#else
  /** C-style function, to get access to the unique ISOMonitor_c singleton instance */
  ISOMonitor_c& getIsoMonitorInstance( void )
  {
    static ISOMonitor_c& c_isoMonitor = ISOMonitor_c::instance();
    return c_isoMonitor;
  }
#endif

/** constructor for ISOMonitor_c which can store optional pointer to central Scheduler_c instance
  @param rpc_lb optional pointer to central Scheduler_c instance (default NULL -> the later set is needed)
*/
ISOMonitor_c::ISOMonitor_c()
  : SingletonISOMonitor_c(), vec_isoMember()
{
  // functionality moved OUT of the constructor, as the constructor is NOT called in embedded systems for static class instances.
}

/** initialize directly after the singleton instance is created.
  this is called from singleton.h and should NOT be called from the user again.
  users please use init(...) instead.
*/
void ISOMonitor_c::singletonInit()
{
  setAlreadyClosed(); // so init() will init ;-) (but only once!)
  // "init();" moved to systemStartup() in Scheduler_c to avoid circular dependencies
}


/** initialisation for ISOMonitor_c which can store optional pointer to central Scheduler_c instance */
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
    c_tempIsoMemberItem.set( 0, ISOName_c::ISONameUnspecified, 0xFE, IState_c::Active, getSingletonVecKey() );

    // clear state of b_alreadyClosed, so that close() is called one time AND no more init()s are performed!
    clearAlreadyClosed();

    /// Set Period for Scheduler_c Start Period is 125
    /// timeEvent will change to longer Period after Start
    setTimePeriod( (uint16_t) 125   );
    // register in Scheduler_c to be triggered fopr timeEvent
    getSchedulerInstance4Comm().registerClient( this );

    pc_activeLocalMember = NULL;

    bool b_configure = false;

    // add filter REQUEST_PGN_MSG_PGN via ISORequestPGN_c
    getIsoRequestPgnInstance4Comm().registerPGN (*this, ADRESS_CLAIM_PGN);
#ifdef USE_WORKING_SET
    getIsoRequestPgnInstance4Comm().registerPGN (*this, WORKING_SET_MASTER_PGN);
    getIsoRequestPgnInstance4Comm().registerPGN (*this, WORKING_SET_MEMBER_PGN);
#endif

    if (getCanInstance4Comm().insertFilter( *this, (0x3FFFF00UL), MASK_TYPE(static_cast<MASK_TYPE>((ADRESS_CLAIM_PGN)+0xFF) << 8), false, Ident_c::ExtendedIdent))
      b_configure = true;
#ifdef USE_WORKING_SET
    FilterBox_c* pc_filterBoxWsMaster
      = getCanInstance4Comm().insertFilter( *this, (0x3FFFF00UL), MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MASTER_PGN) << 8), false, Ident_c::ExtendedIdent);
    if (pc_filterBoxWsMaster)
      b_configure = true;
    if (getCanInstance4Comm().insertFilter( *this, (0x3FFFF00UL), MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MEMBER_PGN) << 8), false, Ident_c::ExtendedIdent), pc_filterBoxWsMaster)
      b_configure = true;
#endif

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

/** every subsystem of IsoAgLib has explicit function for controlled shutdown */
void ISOMonitor_c::close( void )
{
  if ( ! checkAlreadyClosed() )
  {
    // avoid another call
    setAlreadyClosed();

    while ( c_arrClientC1.size() > 0 )
    {
      (*c_arrClientC1.begin())->close();
    }
    getSchedulerInstance4Comm().unregisterClient( this );

    getIsoRequestPgnInstance4Comm().unregisterPGN (*this, ADRESS_CLAIM_PGN);
#ifdef USE_WORKING_SET
    getIsoRequestPgnInstance4Comm().unregisterPGN (*this, WORKING_SET_MASTER_PGN);
    getIsoRequestPgnInstance4Comm().unregisterPGN (*this, WORKING_SET_MEMBER_PGN);
#endif

    getCanInstance4Comm().deleteFilter( *this, 0x3FFFF00UL, MASK_TYPE(static_cast<MASK_TYPE>((ADRESS_CLAIM_PGN)+0xFF) << 8), Ident_c::ExtendedIdent);
#ifdef USE_WORKING_SET
    getCanInstance4Comm().deleteFilter( *this, 0x3FFFF00UL, MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MASTER_PGN) << 8), Ident_c::ExtendedIdent);
    getCanInstance4Comm().deleteFilter( *this, 0x3FFFF00UL, MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MEMBER_PGN) << 8), Ident_c::ExtendedIdent);
#endif
  }
}

/** deliver reference to data pkg as reference to CANPkgExt_c
  to implement the base virtual function correct
*/
CANPkgExt_c& ISOMonitor_c::dataBase()
{
  return c_data;
}

/** performs periodically actions,
  possible errors:
  partial error caused by one of the memberItems
  @return true -> all planned activities performed in allowed time
*/
bool ISOMonitor_c::timeEvent( void )
{
  if ( getAvailableExecTime() == 0 ) return false;

  int32_t i32_checkPeriod = 3000;
  for ( STL_NAMESPACE::vector<__IsoAgLib::IdentItem_c*>::iterator pc_iter = c_arrClientC1.begin(); ( pc_iter != c_arrClientC1.end() ); pc_iter++ )
  { // call timeEvent for each registered client -> if timeEvent of item returns false
    // it had to return BEFORE its planned activities were performed (because of the registered end time)
    if ( !(*pc_iter)->timeEvent() ) return false;
    switch( (*pc_iter)->itemState() & 0x7C )
    {
    case IState_c::AddressClaim | IState_c::Active:
      if (i32_checkPeriod > 150) i32_checkPeriod = 150;
      break;

    case IState_c::ClaimedAddress | IState_c::Active:
      #ifdef USE_WORKING_SET
      if (!(*pc_iter)->getIsoItem()->isClaimedAndWsAnnounced())
      { // we need 100ms for WS-Announce Sequence!
        if (i32_checkPeriod > 100) i32_checkPeriod = 100;
        break;
      }
      #endif
      if (i32_checkPeriod > 1000) i32_checkPeriod = 1000;
      break;

    // do not change period
    case IState_c::PreAddressClaim | IState_c::Active: // shouldn't happen. after timeEvent we can not be any longer PreAddressClaim
    case IState_c::Off:
    case IState_c::Standby:
    default:
      // nothing to todo stay on 3000 ms timePeriod
      break;
    }

  }
  // new TimePeriod is necessary - change it
  if (i32_checkPeriod != getTimePeriod()) setTimePeriod(i32_checkPeriod);

  /// We have now: (HT=HardTiming, ST=SoftTiming)
  /// At least one IdentItem
  /// - that has state AddressClaim: 150ms HT
  /// - ClaimedAddress, WS claiming: 100ms HT
  /// - ClaimedAddress, ws claimed: 1000ms ST
  /// otherwise idling around with: 3000ms ST
  /// @todo improve later to have the IdentItems give back fix timestamps. If you, use Hard-Timing and wait for exactly this timestamp
  ///       if not so, use soft-timing and idle around...


  if ( getAvailableExecTime() == 0 ) return false;

#if CONFIG_ISO_ITEM_MAX_AGE > 0
  if ( existActiveLocalIsoMember() )
  { // use the SA of the already active node
    data().setIsoSa(getActiveLocalIsoMember().nr());
  } /** @todo check if we really want to have the SA set in ANY case, even if we're not sending afterwards.. */

  if ( lastIsoSaRequest() == -1) return true;
  else if ( existActiveLocalIsoMember() )
  { // we could send the next SA request
    const int32_t ci32_timePeriod = SA_REQUEST_PERIOD_MSEC
        + ( ( getActiveLocalIsoMember().nr() % 0x80 ) * 1000 );
    // the request interval takes the number of the SA into account, so that nodes with higher
    // SA should receive the request of this node before they decide to send a request on their own
    // ==> MIN INTERVAL is SA_REQUEST_PERIOD_MSEC
    // ==> MAX INTERVAL is (SA_REQUEST_PERIOD_MSEC + (0xFF % 0x80) ) == ( SA_REQUEST_PERIOD_MSEC + 0x7F )
    if ( ( HAL::getTime() - lastIsoSaRequest() ) > ci32_timePeriod )
    { // it's time for the next SA request in case we have already one
      sendRequestForClaimedAddress( true );
    }
  }

  int32_t i32_now = getLastRetriggerTime();
  const int32_t ci32_timeSinceLastAdrClaimRequest = (i32_now - lastIsoSaRequest());
  bool b_requestAdrClaim = false;
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
            << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          pc_iter = vec_isoMember.erase(pc_iterDelete); // erase returns iterator to next element after the erased one
        }
        else
        { // give it another chance
          pc_iter->setItemState( IState_c::PossiblyOffline );
          b_requestAdrClaim = true;
        }
      } else  {
        pc_iter++;
      }
    } // for
    if ( b_requestAdrClaim )
    { // at least one node needs an additional adr claim
      sendRequestForClaimedAddress( true );
    }
  } // if
  #endif

  return true;
}

/** deliver the count of members in the Monitor-List with given ECU-Type (which is an own IsoAgLib-definition!)
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
    if ( ((pc_iter->isoName(). getEcuType() == r_ecuType))
      && (!rb_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      b_result++;
      pc_isoMemberCache = pc_iter; // set member cache to member  with searched devClass
    }
  }
  return b_result;
}

/** deliver one of the members with specific ECU_Type (which is an own IsoAgLib-definition!)
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
    if ( ((pc_iter->isoName(). getEcuType()) == r_ecuType)
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
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::System );
  }
  return *pc_isoMemberCache;
}

/** deliver the count of members in the Monitor-List with given DEVCLASS (variable POS)
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
//    cerr << "Tested Member DevClass: " << int( pc_iter->isoName().devClass() ) << endl;
    if ( ( ((pc_iter->isoName().devClass()) == rui8_devClass) || (rui8_devClass == 0xFF))
      && (!rb_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      b_result++;
      pc_isoMemberCache = pc_iter; // set member cache to member  with searched devClass
    }
  }
  return b_result;
}

/** deliver one of the members with specific DEVCLASS
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
    if ( ( ((pc_iter->isoName().devClass()) == rui8_devClass) || (rui8_devClass == 0xFF))
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
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::System );
  }
  return *pc_isoMemberCache;
}

/** check if a memberItem with given ISOName exist
  which optional (!!) match the condition of address claim state
  and update local pc_isoMemberCache
  @param rc_isoName searched ISOName
  @param rb_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return true -> searched member exist
*/
bool ISOMonitor_c::existIsoMemberISOName(const ISOName_c& rc_isoName, bool rb_forceClaimedAddress)
{
  if (!vec_isoMember.empty() && (pc_isoMemberCache != vec_isoMember.end()))
  {
    if ( (pc_isoMemberCache->isoName() == rc_isoName )
      && (!rb_forceClaimedAddress || pc_isoMemberCache->itemState(IState_c::ClaimedAddress))
        )  return true;
  }
  for (pc_isoMemberCache = vec_isoMember.begin();
       pc_isoMemberCache != vec_isoMember.end();
       pc_isoMemberCache++)
  {
    if ( (pc_isoMemberCache->isoName() == rc_isoName )
      && (!rb_forceClaimedAddress || pc_isoMemberCache->itemState(IState_c::ClaimedAddress))
        )  return true;
  }
  // if reaching here -> nothing found
  return false;
}

/** check if a member with given number exist
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
}

/** check if member is in member list with wanted ISOName,
  adapt instance if member with claimed address with other device class inst exist
  @param refc_isoName ISOName to search (-> it's updated if member with claimed address with other dev class inst is found)
  @return true -> member with claimed address with given DEVCLASS found (and refc_isoName has now its ISOName)
*/
bool ISOMonitor_c::isoDevClass2ISONameClaimedAddress(ISOName_c &refc_isoName)
{
  if (existIsoMemberISOName(refc_isoName, true))
  { // there exists a device with exact NAME in claimed address state
    return true;
  }
  else
  { // no item with ISOName found -> adapt POS
    // search for member with claimed address with same DEVCLASS
    if (isoMemberDevClassCnt(refc_isoName.devClass(), true) > 0)
    { // member with wanted device class exists -> store the ISOName
      refc_isoName = isoMemberDevClassInd(refc_isoName.devClass(), 0, true).isoName();
      return true;
    }
    else if (isoMemberDevClassCnt(refc_isoName.devClass(), false) > 0)
    { // member with wanted device class exists -> store the ISOName
      refc_isoName = isoMemberDevClassInd(refc_isoName.devClass(), 0, false).isoName();
      // even if a device with wanted ISOName exist - it is not claimed
      return false;
    }
    else
    {
      return false;
    }
  }
}

/** insert a new ISOItem_c in the list; with unset rui8_nr the member is initiated as
  address claim state; otherwise the given state can be given or state Active is used
  possible errors:
    * badAlloc not enough memory to insert new ISOItem_c isntance
    * busy another member with same ident exists already in the list
  @param rc_isoName ISOName of the member
  @param rui8_nr member number
  @param ren_status wanted status
  @return pointer to new ISOItem_c or NULL if not succeeded
*/
ISOItem_c* ISOMonitor_c::insertIsoMember(const ISOName_c& rc_isoName,
      uint8_t rui8_nr, IState_c::itemState_t ren_state)
{
  ISOItem_c* pc_result = NULL;

  // check if another ISOItem_c with same ISOName already exist
  if (existIsoMemberISOName(rc_isoName))
  { // another member with same ISOName found
    getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::System );
    return NULL; // don't insert
  }

  // FROM NOW ON WE DECIDE TO (TRY TO) CREATE A NEW ISOItem_c
  // prepare temp item with wanted data
  c_tempIsoMemberItem.set (System_c::getTime(), // Actually this value/time can be anything. The time is NOT used in PreAddressClaim and when entering AddressClaim it is being set correctly!
    rc_isoName, rui8_nr, IState_c::itemState_t(ren_state | IState_c::Member | IState_c::Active), getSingletonVecKey() );

  // now insert element
  const uint8_t b_oldSize = vec_isoMember.size();
  vec_isoMember.push_front(c_tempIsoMemberItem);
  pc_isoMemberCache = vec_isoMember.begin();
  if (vec_isoMember.size() <= b_oldSize)
  { // array didn't grow
    getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::System );
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
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  }
  vec_isoMember.sort(); // resort the list

  return pc_result;
}

/** deliver the amount of local members which matches the searched proto types
    @return amount of registered local din members
 */
uint8_t ISOMonitor_c::localIsoMemberCnt()
{
  uint8_t b_count = 0;
  for ( pc_searchCacheC1 = c_arrClientC1.begin(); ( pc_searchCacheC1 != c_arrClientC1.end() ); pc_searchCacheC1++ )
  {  // increase reult count if local ident is already registered in MemberList
    if ( existIsoMemberISOName( (*pc_searchCacheC1)->isoName(), false ) ) b_count++;
  }
  return b_count;
}

/** deliver reference to local member by index
    @see localMemberCnt
    @param rui8_ind index of wanted member (first item == 0)
    @return reference to wanted local member MonitorItem
      (MonitorItem_c is base class of ISOItem_c which serves
      adress, isoName, itemState)
    @return pointer to wanted local iso member (NULL if no suitable IsoItem_c found)
 */
ISOItem_c& ISOMonitor_c::localIsoMemberInd(uint8_t rui8_ind)
{
  ISOItem_c* pc_result = NULL;
  uint8_t b_count = 0;
  for ( pc_searchCacheC1 = c_arrClientC1.begin(); ( pc_searchCacheC1 != c_arrClientC1.end() ); pc_searchCacheC1++ )
  {  // increase reult count if local ident is already registered in MemberList
    if ( existIsoMemberISOName( (*pc_searchCacheC1)->isoName(), false ) )
    {
      if (b_count == rui8_ind)
      { // wanted item found
        pc_result = &( isoMemberISOName( (*pc_searchCacheC1)->isoName(), false ) );
        break;
      }
      b_count++;
    }
  } // for

  if ( pc_result == NULL )
  { // index exceeds array size
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::System );
  }

  return *pc_result;
}

/** check if one of the own local members is active with claimed address at ISO11783
    @return true -> at least one of the own din identities is active with claimed address at ISO11783
    @see ISOMonitor_c::getActiveLocalMember
 */
bool ISOMonitor_c::existActiveLocalIsoMember()
{
  bool b_result = false; // set default to no success

  // check if actual cache pointer points to active ident
  if ((pc_activeLocalMember == NULL)
       || (!pc_activeLocalMember->itemState(IState_c::ClaimedAddress)))
  { // the actual cache pointer isn't correct -> search new one
    const ISOName_c *pc_useISOName = NULL;
    ISOItem_c* pc_monitorItem = NULL;

    for ( pc_searchCacheC1 = c_arrClientC1.begin(); ( pc_searchCacheC1 != c_arrClientC1.end() ); pc_searchCacheC1++ )
    {
      if ((*pc_searchCacheC1)->itemState(IState_c::ClaimedAddress))
      {
        if (existIsoMemberISOName((*pc_searchCacheC1)->isoName(), true ))
        {
          pc_monitorItem = &(isoMemberISOName((*pc_searchCacheC1)->isoName(), true));
          if (pc_monitorItem->itemState(IState_c::itemState_t(IState_c::ClaimedAddress | IState_c::Local)))
          {
            pc_useISOName = &((*pc_searchCacheC1)->isoName());
            break;
          } // end if claimed address and local
        } // end if existMemberISOName
      } // end if ident_item has claimed address
    } // searching for loop
    if (pc_useISOName != NULL)
    { // active own identity found
      b_result = true;
      pc_activeLocalMember = pc_monitorItem;
    }
    else
    { // no active ident found -> set cache to NULL
      pc_activeLocalMember = NULL;
    }
  }
  else
  { // cache pointer correct
    b_result = true;
  }
  return b_result;
}

/** delivers reference to the first active local member;
    send of ISO11783 system msg demands telling a sender ident member no
    -> using the number of the first active member serves as default (f.e. for requesting other member names)
    can throw an preconditionViolation error, if none of the own identities is active/claimed address yet
    possible errors:
 * lbsSysNoActiveLocalMember on missing own active ident
    @return reference to the MonitorItem_c of the first active local member
      (MonitorItem_c is bas class of both ISOItem_c or DINItem_c which serves
      adress, isoName, itemState)
    @exception preconditionViolation
 */
ISOItem_c& ISOMonitor_c::getActiveLocalIsoMember()
{
  if  (existActiveLocalIsoMember() )
  { // return reference to the pointed ident element
    return *pc_activeLocalMember;
  }
  else
  { // no active own identity found -> set error state
    getILibErrInstance().registerError( iLibErr_c::SysNoActiveLocalMember, iLibErr_c::System );

    // throw exception by constant -> if no exception configured no command is created
    THROW_PRECOND_VIOLATION

    //return reference to the first ident in IsoMonitor_c
    return isoMemberISOName(c_arrClientC1.front()->isoName(), true);
  }
}

/** check for own din ident with given member no
    @param rui8_nr member no to search for
    @return true -> one of the own din identities has the wanted member no
 */
bool ISOMonitor_c::existLocalIsoMemberNr(uint8_t rui8_nr)
{
  if ( !c_arrClientC1.empty()
        && ( pc_searchCacheC1 != c_arrClientC1.end() )
        && (*pc_searchCacheC1)->equalNr(rui8_nr )
     )
  {
    return true;
  }
  else
  {
    for (pc_searchCacheC1 = c_arrClientC1.begin(); pc_searchCacheC1 != c_arrClientC1.end(); pc_searchCacheC1++)
    {
      if ( (*pc_searchCacheC1)->equalNr(rui8_nr) )
        break;
    }
    return (pc_searchCacheC1 != c_arrClientC1.end())?true:false;
  }
}

/** check for own din ident with given ISOName
    @param rc_isoName ISOName to search for
    @return true -> one of the own din identities has the wanted ISOName
 */
bool ISOMonitor_c::existLocalIsoMemberISOName (const ISOName_c& rc_isoName, bool rb_forceClaimedAddress)
{
  if ( (!c_arrClientC1.empty()) && (pc_searchCacheC1 != c_arrClientC1.end()) )
  { // try to use current cache as it points to valid entry
    if ( ((*pc_searchCacheC1)->isoName() == rc_isoName )
            && (!rb_forceClaimedAddress || (*pc_searchCacheC1)->itemState(IState_c::ClaimedAddress))
       )  return true;
  }
  // if last cache is still vald the function is exited -> start new search
  for (pc_searchCacheC1 = c_arrClientC1.begin();
       pc_searchCacheC1 != c_arrClientC1.end(); pc_searchCacheC1++)
  {
    if ( ((*pc_searchCacheC1)->isoName() == rc_isoName )
            && (!rb_forceClaimedAddress || (*pc_searchCacheC1)->itemState(IState_c::ClaimedAddress))
       )  return true;
  }
  // if reaching here -> nothing found
  return false;
}

/** reset the Addres Claim state by:
  * + reset IdentItem::IStat_c to IState_c::PreAddressClaim
  * + remove pointed ISOItem_c and DINItem_c nodes and the respective pointer
  * @return true -> there was an item with given ISOName_c that has been resetted to IState_c::PreAddressClaim
 */
bool ISOMonitor_c::restartAddressClaim( const ISOName_c& rrefc_isoName )
{
  if ( existLocalIsoMemberISOName( rrefc_isoName, false ) )
  { // there exists a local IdentItem_c with the given ISOName_c
    // -> forward the function call
    (*pc_searchCacheC1)->restartAddressClaim();
    return true;
  }
  else
  { // no local item has same ISOName_c
    return false;
  }
}

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
void ISOMonitor_c::broadcastSaAdd2Clients( const ISOName_c& rc_isoName, const ISOItem_c* rpc_isoItem ) const
{
  for ( SaClaimHandlerVectorConstIterator_t iter = vec_saClaimHandler.begin(); iter != vec_saClaimHandler.end(); iter++ )
  { // call the handler function of the client
    (*iter)->reactOnMonitorListAdd( rc_isoName, rpc_isoItem );
  }
}

/** this function is used to broadcast a ISO monitor list change to all registered clients */
void ISOMonitor_c::broadcastSaRemove2Clients( const ISOName_c& rc_isoName, uint8_t rui8_oldSa ) const
{
  for ( SaClaimHandlerVectorConstIterator_t iter = vec_saClaimHandler.begin(); iter != vec_saClaimHandler.end(); iter++ )
  { // call the handler function of the client
    (*iter)->reactOnMonitorListRemove( rc_isoName, rui8_oldSa );
  }
}

/**
  deliver member item with given isoName
  (check with existIsoMemberISOName before access to not defined item)
  possible errors:
    * elNonexistent on failed search
  @param rc_isoName searched ISOName
  @return reference to searched ISOItem
  @exception containerElementNonexistant
*/
ISOItem_c& ISOMonitor_c::isoMemberISOName(const ISOName_c& rc_isoName, bool rb_forceClaimedAddress)
{
  if (existIsoMemberISOName(rc_isoName, rb_forceClaimedAddress))
  { // no error
    return static_cast<ISOItem_c&>(*pc_isoMemberCache);
  }
  else
  { // wanted element not found
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::System );

    // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    //return reference to first element as fallback
    return vec_isoMember.front();
  }
}

/** deliver member item with given nr
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
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::System );

    // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    //return reference to first element as fallback
    return vec_isoMember.front();
  }
}

/** deliver member item with given ISOName, set pointed bool var to true on success
  and set a Member Array Iterator to the result
  @param rc_isoName searched ISOName
  @param pb_success bool pointer to store the success (true on success)
  @param pbc_iter optional member array iterator which points to searched ISOItem_c on success
  @return reference to the searched item
*/
ISOItem_c& ISOMonitor_c::isoMemberISOName(const ISOName_c& rc_isoName, bool *const pb_success, bool rb_forceClaimedAddress, Vec_ISOIterator *const pbc_iter)
{
  *pb_success = (existIsoMemberISOName(rc_isoName, rb_forceClaimedAddress))?true:false;

  if (pbc_iter != NULL)
  {
    *pbc_iter = pc_isoMemberCache;
  }
  return static_cast<ISOItem_c&>(*pc_isoMemberCache);
}

/**
  delete item with specified isoName
  possible errors:
    * elNonexistent no member with given ISOName exists
  @param rc_isoName ISOName of to be deleted member
*/
bool ISOMonitor_c::deleteIsoMemberISOName(const ISOName_c& rc_isoName)
{
  if (existIsoMemberISOName(rc_isoName))
  { // set correct state

    #ifdef USE_WORKING_SET
    // Are we deleting a WorkingSetMaster?
    if (pc_isoMemberCache->isMaster())
    { // yes, so notify all slaves that they're now standalone!
      notifyOnWsMasterLoss (*pc_isoMemberCache);
    }
    #endif

    // erase it from list (existIsoMemberISOName sets pc_isoMemberCache to the wanted item)
    vec_isoMember.erase(pc_isoMemberCache);
    #ifdef DEBUG_HEAP_USEAGE
    sui16_isoItemTotal--;

    getRs232Instance()
      << sui16_isoItemTotal << " x ISOItem_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(ISOItem_c), sui16_isoItemTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(ISOItem_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(ISOItem_c), sui16_isoItemTotal )
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    pc_isoMemberCache = vec_isoMember.begin();
    return true;
  }
  else
  { // to be deleted member ISOName does not exist
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::System );
    return false;
  }
}

/** delete item with specified member number
  possible errors:
    * elNonexistent no member with given ISOName exists
  @param rui8_nr SA of to be deleted member
*/
bool ISOMonitor_c::deleteIsoMemberNr(uint8_t rui8_nr)
{
  if (existIsoMemberNr(rui8_nr))
  { // use deleteIsoMemberISOName
    return deleteIsoMemberISOName(pc_isoMemberCache->isoName());
  }
  else
  { // to be deleted member number does not exist
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::System );
    return false;
  }
}


/**
  change isoName if actual isoName isn't unique
  (search possible free instance to given device class)
  possible errors:
    * busy no other device class inst code leads to unique ISOName code
  @param refc_isoName reference to ISOName var (is changed directly if needed!!)
  @param rb_dontUnify don't touch the IsoName because it most likely has already (at least once) claimed
                      an SA with this IsoName, so it can't change away, it has to keep its identity
  @return true -> referenced ISOName is now unique
*/
bool ISOMonitor_c::unifyIsoISOName (ISOName_c& refc_isoName, bool rb_dontUnify)
{
  bool b_result = true;
  if (existIsoMemberISOName (refc_isoName))
  { // isoName exist -> search new with changed POS
    // first, for now, we don't have it unified, we start with conflict.
    b_result = false;

    if (!rb_dontUnify)
    {
      /** @todo What algorithm to use for unifying the IsoName? Which isntance to increase? */
      ISOName_c c_tempISOName = refc_isoName;
      b_result = false; // refc_isoName isn't unique
      // store the pos part of given isoName
      int16_t tempPos = (refc_isoName.devClassInst()),
          diff = 1;
      for (; diff < 8; diff++)
      {
        if (tempPos + diff < 8)
        {  // (tempPos + diff) would be an allowed device class inst code
          c_tempISOName.setDevClassInst( tempPos + diff );
          if (!(existIsoMemberISOName(c_tempISOName)))
          {  // (tempPos + diff) can't be found in list -> it is unique
            refc_isoName.setDevClassInst( tempPos + diff );
            b_result = true;
            break;
          }
        }
        if (tempPos - diff >= 0)
        { // (tempPos - diff) would be an allowed device class inst code
          c_tempISOName.setDevClassInst( tempPos - diff );
          if (!(existIsoMemberISOName(c_tempISOName)))
          {  // (tempPos - diff) can't be found in list -> it is unique
            refc_isoName.setDevClassInst( tempPos - diff );
            b_result = true;
            break;
          }
        }
      }
    } // else: if we can't unify, we're lost here as such an IsoName is already claimed on the bus
  } // else: IsoName doesn't exist --> fine!

  if (!b_result) getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::System );
  return b_result;
}

/** check if SA of an announcing ISOItem_c is unique and deliver
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
  const bool cb_selfConf = rpc_isoItem->selfConf();

  if (ui8_wishSa >= 254) // also include 255
  { // no preferred SA
    if (cb_selfConf)
    { // We're self-conf, so start at 128 (0x80)
      ui8_wishSa = 128;
    }
    else
    { // we're not self-conf, but request the address that's used as "take-any": that won't work!
      ui8_wishSa = 254; // couldn't find a suitable SA!
    }
  }

  // while we have addresses to try, try!
  while (ui8_wishSa < 254)
  { // try the current ui8_wishSa
    bool b_free = true;
    for (Vec_ISOIterator pc_iterItem = vec_isoMember.begin();
          pc_iterItem != vec_isoMember.end(); pc_iterItem++)
    {
      if ((pc_iterItem->nr() == ui8_wishSa)
           && (&(*pc_iterItem) != rpc_isoItem)
         )
      { // the tried SA is already used by this item
        // -> break this search loop and try another ui8_wishSa
        b_free = false;
        break;
      }
    }
    if (b_free)
    { // address is FREE
      break; // after breaking, ui8_wishSa is returned, so we're done unifying
    }
    else
    { // address is USED
      if (cb_selfConf)
      { // we're self-configurable, so let's try the next higher one
        ui8_wishSa++;
      }
      else
      { // we're not self-configurable, and we didn't get the requested SA --> report this with 254
        ui8_wishSa = 254;
        break;
      }
    }
  }
  return ui8_wishSa; // return the best we could get (may 254 if no addresses were free below.
}


/** trigger a request for claimed addreses
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
  const int32_t i32_time = ElementBase_c::getLastRetriggerTime();

//  getRs232Instance() << "_time in sendReq4AdrCl: " << HAL::getTime() <<"_";

  data().setIsoPri(6);
  data().setIsoPgn(REQUEST_PGN_MSG_PGN);
  data().setIsoPs(255); // global request
  if ( existActiveLocalIsoMember() )
  { // use the SA of the already active node
    data().setMonitorItemForSA( &getActiveLocalIsoMember() );
    b_sendOwnSa = true;
  }
  else
  { // no local ident has claimed an adress so far
    data().setIsoSa(254); // special flag for "no SA yet"
  }
  // built request data string
//  uint8_t pb_requestString[4];
  data().setUint32Data( 0, ADRESS_CLAIM_PGN );
  data().setLen(3);
  // now ISOSystemPkg_c has right data -> send
  getCanInstance4Comm() << data();
  // store adress claim request time
  setLastIsoSaRequest(i32_time);

  // now send own SA in case at least one local ident has yet claimed adress
  if (b_sendOwnSa)
  {
    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "Send checking SA request (sendRequestForClaimedAddress())" << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    const uint8_t cui8_localCnt = localIsoMemberCnt();
    for ( uint8_t ui8_ind = 0; ui8_ind < cui8_localCnt; ui8_ind++ )
    { // the function ISOItem_c::sendSaClaim() checks if this item is local and has already claimed a SA
      localIsoMemberInd( ui8_ind ).sendSaClaim();
    }
  }
  return true;
}

/** process system msg with informations which are
  important for managing of members
  @return true -> message processed by ISOMonitor_c; false -> process msg by ServiceMonitor
*/
bool ISOMonitor_c::processMsg()
{
  bool b_processed = false;

  ISOItem_c *pc_itemSameSa = NULL,
          #ifdef USE_WORKING_SET
            *pc_item = NULL,
          #endif
            *pc_itemSameISOName = NULL;


  // Handle DESTINATION PGNs
  switch ((data().isoPgn() & 0x3FF00))
  {
    case ADRESS_CLAIM_PGN: // adress claim
      if ( existIsoMemberISOName( data().isoName()) )
      {
        pc_itemSameISOName = &(isoMemberISOName(data().isoName()));
      }
      if ( existIsoMemberNr(data().isoSa()) )
      {
        pc_itemSameSa = &(isoMemberNr(data().isoSa()));
      }

      // FIRST REMOVE any REMOTE ISOItem_c with SAME SA but different ISOName_c
      if ( ( NULL != pc_itemSameSa                       )
        && ( ! pc_itemSameSa->itemState(IState_c::Local) )
        && ( pc_itemSameSa->isoName() != data().isoName()  ) )
      { // REMOVE the node that is pointed by pc_itemSameSa
        // - the destructor of the ISOItem_c informs all handlers about the loss
        deleteIsoMemberNr( data().isoSa() );
        pc_itemSameSa = NULL;
      }

      /** @todo Change handling of case where SAME ISONAME occurs. We shall not change our ISONAME while running in ISO! */
      // SECOND: trigger total restart of ADR claim for any local ISOItem_c with
      //         SAME ISOName_c
      if ( ( NULL != pc_itemSameISOName ) && ( pc_itemSameISOName->itemState(IState_c::Local)) )
      { // in ANY case - when another node sends SA claim with same NAME we MUST restart
        // as nobody else on the network will detect the NAME clash
        // - this restart triggers also the REMOVAL of the item pc_itemSameISOName
        if ( ( NULL != pc_itemSameSa ) && ( pc_itemSameSa->itemState( IState_c::Local ) ) )
        { // there is also a SA conflict with a local ISOItem_c
          if ( pc_itemSameSa != pc_itemSameISOName )
          { // the received adr claim overlaps two different local ISOItem_c --> restart BOTH
            // (even if pc_itemSameSa would have higher prio - this is needed, as the new remote item would be in conflict by SA with
            //  the currently existing local pc_itemSameSa )
            restartAddressClaim( pc_itemSameSa->isoName() );
          }
          // in case of sort of SA conflict with a local ISOItem_c, we must avoid a later reaction on this SA conflict
          // => set the pointer to NULL to indicate the handled conflict
          pc_itemSameSa = NULL;
        }
        // the case of a conflict on SA with a remote ISOItem_c is handled elsewhere as this block is only directed to handle
        // conflicts with LOCAL ISOItem_c
        restartAddressClaim( pc_itemSameISOName->isoName() );
        pc_itemSameISOName = NULL;
        // DO NOT set b_processed to true as the SA CLAIM shall trigger creation of fresh remote ISOItem_c
      }

      // THIRD: handle LOCAL item that has same SA
      if ( ( NULL != pc_itemSameSa ) && ( pc_itemSameSa->itemState(IState_c::Local)) )
      { // there exists a LOCAL item with same SA
        // --> remove it when it has lower PRIO
        int8_t i8_higherPrio = pc_itemSameSa->isoName(). higherPriThanPar(data().getDataUnionConst());
        if ( ( i8_higherPrio < 0                              )
               // the local ISOItem_c should be overwritten by the received adr claim, when the local
               // item is not yet sent any adr claim (e.g. Off state or PreAddressClaim)
               || ( ! pc_itemSameSa->itemStatePartialMatch(IState_c::itemState_t(IState_c::ClaimedAddress | IState_c::AddressClaim)) ) )
        { // the LOCAL item has lower PRIO or has not yet fully claimed --> remove it
          // the function SystemMgmt_c::restartAddressClaim() triggers removal of ISOItem_c
          // and registers the next address claim try afterwards
          restartAddressClaim( pc_itemSameSa->isoName() );
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

      // FOUR handle case where remote node has SAME ISOName_c (i.e. NAME)
      if ( ( NULL != pc_itemSameISOName                       )
        && ( ! pc_itemSameISOName->itemState(IState_c::Local) )
        && ( ! b_processed                                   ) ///< this message has not yet been processed
         )
      { // there exists a REMOTE item with same ISOName_c
        // --> simply let this item process
        if ( pc_itemSameISOName->processMsg() ) b_processed = true;
      }

      // create NEW ISOItem_c in case no ISOItem_c with Pkg.ISOName_c exists
      // and the message has not yet been marked as processed
      if ( ( NULL == pc_itemSameISOName ) && ( ! b_processed ) )
      { // create a suitable ISOItem_c and let it process
        ISOItem_c* pc_newItem = insertIsoMember(data().isoName(), data().isoSa(),
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
  switch ((data().isoPgn() /* & 0x3FFFF */ )) // isoPgn is already "& 0x3FFFF" !
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
        getILibErrInstance().registerError( iLibErr_c::Inconsistency, iLibErr_c::System );
      }
      break;
    case WORKING_SET_MEMBER_PGN: // working set member
      if (existIsoMemberNr(data().isoSa()))
      { // ISOItem_c with same SA exists (THE SA IS THE MASTER!)
        ISOItem_c *pc_itemMaster = &(isoMemberNr(data().isoSa()));
        // the working set master places the NAME field of each children
        // in the data part of this message type
        pc_item = &(isoMemberISOName(data().isoName()));
        pc_item->setMaster (pc_itemMaster); // set master on this isoItem
        b_processed = true;
      }
      else
      {
        // shouldn't happen!
        getILibErrInstance().registerError( iLibErr_c::Inconsistency, iLibErr_c::System );
      }
      break;
    #endif
    default:
      break;
  } // end switch for NON-DESTINATION pgn

  return b_processed; // return if msg was processed by ISOMonitor_c
}

bool
ISOMonitor_c::processMsgRequestPGN (uint32_t rui32_pgn, ISOItem_c* /*rpc_isoItemSender*/, ISOItem_c* rpc_isoItemReceiver)
{
  switch (rui32_pgn)
  {
    case ADRESS_CLAIM_PGN:
      // update time of last adress claim request
      setLastIsoSaRequest (getIsoRequestPgnInstance4Comm().getTimeOfLastRequest()); // Now using CAN-Pkg-Times, see header for "setLastIsoSaRequest" for more information!

      if (rpc_isoItemReceiver == NULL)
      { // No specific destination so it's broadcast: Let all local item answer!
        bool b_processedRequestPGN = false;
        for (Vec_ISOIterator pc_iterItem = vec_isoMember.begin();
              pc_iterItem != vec_isoMember.end(); pc_iterItem++)
        { // let all local pc_iterItem process this request
          if (pc_iterItem->itemState (IState_c::Local))
            b_processedRequestPGN |= pc_iterItem->sendSaClaim();
        }
        return b_processedRequestPGN; //return value doesn't matter, because the request was for GLOBAL (255), so it isn't NACKed anyway
      }
      else
      { // ISORequestPGN ensured that the Item exists and is local: Let it process!
        return rpc_isoItemReceiver->sendSaClaim();
      }
      break;

#ifdef USE_WORKING_SET
    case WORKING_SET_MASTER_PGN: // break intentionally left out - react on both PGNs with sending out the complete ws-announce sequence!
    case WORKING_SET_MEMBER_PGN:
      if (rpc_isoItemReceiver == NULL)
      { // Let ALL local item answer
        for (Vec_ISOIterator pc_iterItem = vec_isoMember.begin();
              pc_iterItem != vec_isoMember.end(); pc_iterItem++)
        { // let all local pc_iterItem process process this request
          if (pc_iterItem->itemState (IState_c::Local))
          {
            if (pc_iterItem->isMaster())
            { // is Master, so can it send out the ws-announce?
              if (!pc_iterItem->sendWsAnnounce())
              { // couldn't send right now, so answer with ACK-BUSY!
                __IsoAgLib::getIsoRequestPgnInstance4Comm().answerRequestPGNwithCannotRespondNow (*pc_iterItem);
              }
            }
            else
            { // no master, can't send out these PGNs
              // ==> NACK, but not when request went to GLOBAL
            }
          }
        }
        return true; //return value doesn't matter, because the request was for GLOBAL (255), so it isn't NACKed anyway
      }
      else
      { // Let the given local item answer
        if (rpc_isoItemReceiver->isMaster())
        { // is Master, so can it send out the ws-announce?
          if (!rpc_isoItemReceiver->sendWsAnnounce())
          { // couldn't send right now, so answer with ACK-BUSY!
            __IsoAgLib::getIsoRequestPgnInstance4Comm().answerRequestPGNwithCannotRespondNow(*rpc_isoItemReceiver);
          }
          return true;
        }
        else
        { // no master, can't send out these PGNs
          return false; // let it get NACKed
        }
      }
      break; // actually not needed, but for safety...
#endif

    default:
      // shouldn't happen as we only registered for the above handled PGNs
      return false;
  }
}

// Funktion for Debugging in Scheduler_c
const char*
ISOMonitor_c::getTaskName() const
{   return "ISOMonitor_c";}


//! Function set ui16_earlierInterval and
//! ui16_laterInterval that will be used by
//! getTimeToNextTrigger(retriggerType_t)
//! can be overloaded by Childclass for special condition
void
ISOMonitor_c::updateEarlierAndLatestInterval()
{
  if (getTimePeriod() <= 250)
  { // use HARD-Timing
    ui16_earlierInterval = 0;
    ui16_latestInterval  = (getTimePeriod() / 2);
  }
  else
  { // use SOFT-Timing (using jitter for earlier/after
    ui16_earlierInterval = ( (getTimePeriod() * 3) / 4);
    ui16_latestInterval  =   (getTimePeriod() / 2);
  }
}


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
