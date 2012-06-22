/*
  isomonitor_c.cpp - object for monitoring members (list of IsoItem_c)

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "isomonitor_c.h"
#include "isorequestpgn_c.h"
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/iassert.h>

#ifdef USE_ISO_TASKCONTROLLER_CLIENT
  #include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#endif

#if DEBUG_ISOMONITOR || DEBUG_HEAP_USEAGE
  #include <IsoAgLib/util/impl/util_funcs.h>
  #ifdef SYSTEM_PC
    #include <iostream>
    #include <iomanip>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif

/** this define controls the time interval between regular SA requests on the bus
 *  (set to 0 to stop any periodic SA requests)
 */
#define SA_REQUEST_PERIOD_MSEC 60000

#if DEBUG_HEAP_USEAGE
static uint16_t sui16_isoItemTotal = 0;
#endif

namespace __IsoAgLib {
/** C-style function, to get access to the unique IsoMonitor_c singleton instance
 * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
 */
IsoMonitor_c &getIsoMonitorInstance( uint8_t aui8_instance )
{ // if > 1 singleton instance is used, no static reference can be used
  MACRO_MULTITON_GET_INSTANCE_BODY(IsoMonitor_c, PRT_INSTANCE_CNT, aui8_instance);
}


IsoMonitor_c::IsoMonitor_c() :
  mvec_isoMember(),
  mt_handler(*this),
  mt_customer(*this),
  CONTAINER_CLIENT1_CTOR_INITIALIZER_LIST
{
  // functionality moved OUT of the constructor, as the constructor is NOT called in embedded systems for static class instances.
}


void
IsoMonitor_c::init()
{
  isoaglib_assert (!initialized());

  #if DEBUG_HEAP_USEAGE
  sui16_isoItemTotal -= mvec_isoMember.size();
  #endif
  isoaglib_assert (mvec_isoMember.empty());
  mpc_isoMemberCache = mvec_isoMember.end();
  mi32_lastSaRequest = -1; // not yet requested. Do NOT use 0, as the first "setLastRequest()" could (and does randomly) occur at time0 as it's called at init() time.
  mc_tempIsoMemberItem.set( 0, IsoName_c::IsoNameUnspecified(), 0xFE, IState_c::Active, getMultitonInst() );

  /// Set Period for Scheduler_c Start Period is 125
  /// timeEvent will change to longer Period after Start
  setTimePeriod( (uint16_t) 125   );
  // register in Scheduler_c to be triggered fopr timeEvent
  getSchedulerInstance().registerClient( this );

  mpc_activeLocalMember = NULL;

  bool b_configure = false;

  // add filter REQUEST_PGN_MSG_PGN via IsoRequestPgn_c
  getIsoRequestPgnInstance4Comm().registerPGN (mt_handler, ADDRESS_CLAIM_PGN);
#ifdef USE_WORKING_SET
  getIsoRequestPgnInstance4Comm().registerPGN (mt_handler, WORKING_SET_MASTER_PGN);
  getIsoRequestPgnInstance4Comm().registerPGN (mt_handler, WORKING_SET_MEMBER_PGN);
#endif

  if( getIsoBusInstance4Comm().insertFilter( mt_customer, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, ((ADDRESS_CLAIM_PGN)+0xFF)<<8 ), 8, false))
    b_configure = true;
#ifdef USE_WORKING_SET
  if (getIsoBusInstance4Comm().insertFilter( mt_customer, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, (WORKING_SET_MASTER_PGN<<8) ), 8, false))
    b_configure = true;
  if (getIsoBusInstance4Comm().insertFilter( mt_customer, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, (WORKING_SET_MEMBER_PGN<<8) ), 8, false))
    b_configure = true;
#endif

  if (b_configure) {
    getIsoBusInstance4Comm().reconfigureMsgObj();
  }

  setInitialized();
}


void
IsoMonitor_c::close()
{
  isoaglib_assert (initialized());

  // Every (i)IdentItem_c must have close()d before the IsoMonitor_c is close()d.
  isoaglib_assert( c_arrClientC1.empty() );
  // Every SaClaimHandler must have deregistered properly already.
  isoaglib_assert( mvec_saClaimHandler.empty() );

  getSchedulerInstance().unregisterClient( this );

  // We can clear the list of remote nodes.
  /// NOTE: We do currently NOT call "internalIsoItemErase",
  ///       because the list of SaClaimHandlers is empty anyway.
  ///       But if the erase does some more stuff, it may be needed
  ///       to call "internalIsoItemErase" for each item instead
  ///       of just clearing the container of isoMembers.
  mvec_isoMember.clear();

  getIsoRequestPgnInstance4Comm().unregisterPGN (mt_handler, ADDRESS_CLAIM_PGN);
#ifdef USE_WORKING_SET
  getIsoRequestPgnInstance4Comm().unregisterPGN (mt_handler, WORKING_SET_MASTER_PGN);
  getIsoRequestPgnInstance4Comm().unregisterPGN (mt_handler, WORKING_SET_MEMBER_PGN);
#endif

  getIsoBusInstance4Comm().deleteFilter( mt_customer, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, ((ADDRESS_CLAIM_PGN+0xFF) << 8) ) );
#ifdef USE_WORKING_SET
  getIsoBusInstance4Comm().deleteFilter( mt_customer, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, ((WORKING_SET_MASTER_PGN) << 8) ) );
  getIsoBusInstance4Comm().deleteFilter( mt_customer, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, ((WORKING_SET_MEMBER_PGN) << 8) ) );
#endif

  setClosed();
}


bool
IsoMonitor_c::registerIdentItem( IdentItem_c& arc_item )
{
  const bool cb_activationSuccess
    = arc_item.activate( getMultitonInst() );

  if (cb_activationSuccess)
  { // Could activate it, so register it!
    /// IsoMonitor_c.timeEvent() should be called from Scheduler_c in 50 ms
    changeRetriggerTime();
    (void) registerC1 (&arc_item);
    return true;
  }
  else
  { // Couldn't activate it, so we don't register it
    return false;
  }
}


void
IsoMonitor_c::deregisterIdentItem( IdentItem_c& arc_item )
{
  arc_item.deactivate();
  mpc_activeLocalMember = NULL;
  unregisterC1 (&arc_item);
}


bool
IsoMonitor_c::timeEvent()
{
  int32_t i32_checkPeriod = 3000;
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  for ( STL_NAMESPACE::vector<__IsoAgLib::IdentItem_c*,MALLOC_TEMPLATE(__IsoAgLib::IdentItem_c*)>::iterator pc_iter = c_arrClientC1.begin(); ( pc_iter != c_arrClientC1.end() ); pc_iter++ )
  #else
  for ( STL_NAMESPACE::vector<__IsoAgLib::IdentItem_c*>::iterator pc_iter = c_arrClientC1.begin(); ( pc_iter != c_arrClientC1.end() ); pc_iter++ )
  #endif
  { // call timeEvent for each registered client -> if timeEvent of item returns false
    // it had to return BEFORE its planned activities were performed (because of the registered end time)
    if ( !(*pc_iter)->timeEvent() ) return false;
    /// @todo SOON-240 Adapt the check on itemState. Check if 0x7C is correct...
    switch( (*pc_iter)->itemState() & 0x7C )
    {
    case IState_c::AddressClaim | IState_c::Active:
      if (i32_checkPeriod > 10) i32_checkPeriod = 10;
      break;

    case IState_c::ClaimedAddress | IState_c::Active:
      #ifdef USE_WORKING_SET
      if ((*pc_iter)->getIsoItem()->isWsAnnouncing())
      { // we need 100ms for WS-Announce Sequence!
        if (i32_checkPeriod > 100) i32_checkPeriod = 100;
        break;
      }
      #endif
      if (i32_checkPeriod > 1000) i32_checkPeriod = 1000;
      break;

    // do not change period
    case IState_c::PreAddressClaim | IState_c::Active: // shouldn't happen. after timeEvent we cannot be any longer PreAddressClaim
    case IState_c::OffExplicitly:
    case IState_c::OffUnable:
    default:
      // nothing to to do stay on 3000 ms timePeriod
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
  /// @todo ON REQUEST: improve later to have the IdentItems give back fix timestamps. If you, use Hard-Timing and wait for exactly this timestamp
  ///       if not so, use soft-timing and idle around...

#if CONFIG_ISO_ITEM_MAX_AGE > 0
  // the following activities are optional for cleanup
  // --> do NOT execute them, if execution time is limited
  if ( getAvailableExecTime() == 0 )
	return false;

  if ( lastIsoSaRequest() == -1)
	return true;
  
  IsoItem_c *someActiveLocalMember = NULL;
  if ( existActiveLocalIsoMember() )
  { // store some active local member for later use below...
    someActiveLocalMember = &getActiveLocalIsoMember();
  }

  if (someActiveLocalMember)
  { // we could send the next SA request
    const int32_t ci32_timePeriod = SA_REQUEST_PERIOD_MSEC
        + ( ( getActiveLocalIsoMember().nr() % 0x80 ) * 1000 );
    // the request interval takes the number of the SA into account, so that nodes with higher
    // SA should receive the request of this node before they decide to send a request on their own
    // ==> MIN INTERVAL is SA_REQUEST_PERIOD_MSEC
    // ==> MAX INTERVAL is (SA_REQUEST_PERIOD_MSEC + (0xFF % 0x80) ) == ( SA_REQUEST_PERIOD_MSEC + 0x7F )
    if ( ( HAL::getTime() - lastIsoSaRequest() ) > ci32_timePeriod )
    { // it's time for the next SA request in case we have already one
      sendRequestForClaimedAddress( true, someActiveLocalMember );
    }
  }

  int32_t i32_now = getLastRetriggerTime();
  const int32_t ci32_timeSinceLastAdrClaimRequest = (i32_now - lastIsoSaRequest());
  bool b_requestAdrClaim = false;
  if ( ci32_timeSinceLastAdrClaimRequest > CONFIG_ISO_ITEM_MAX_AGE )
  { // the last request is more than CONFIG_ISO_ITEM_MAX_AGE ago
    // --> each client MUST have answered until now if it's still alive
    for(Vec_ISOIterator pc_iter = mvec_isoMember.begin(); pc_iter != mvec_isoMember.end();)
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
          #if DEBUG_HEAP_USEAGE
          sui16_isoItemTotal--;

          INTERNAL_DEBUG_DEVICE
            << sui16_isoItemTotal << " x IsoItem_c: Mal-Alloc: "
            <<  sizeSlistTWithMalloc( sizeof(IsoItem_c), sui16_isoItemTotal )
            << "/" << sizeSlistTWithMalloc( sizeof(IsoItem_c), 1 )
            << ", Chunk-Alloc: "
            << sizeSlistTWithChunk( sizeof(IsoItem_c), sui16_isoItemTotal )
            << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          pc_iter = internalIsoItemErase (pc_iterDelete); // erase returns iterator to next element after the erased one
          // immediately reset cache, because it may have gotten invalid due to the erase!!
          mpc_isoMemberCache = mvec_isoMember.begin();
        }
        else
        { // give it another chance
          pc_iter->setItemState( IState_c::PossiblyOffline );
          pc_iter++;
          b_requestAdrClaim = true;
        }
      } else {
        pc_iter++;
      }
    } // for
    if ( b_requestAdrClaim )
    { // at least one node needs an additional adr claim
      sendRequestForClaimedAddress( true, someActiveLocalMember );
    }
  } // if
  #endif

  return true;
}


uint8_t
IsoMonitor_c::isoMemberEcuTypeCnt (IsoName_c::ecuType_t a_ecuType, bool ab_forceClaimedAddress)
{
  uint8_t b_result = 0;
  for (Vec_ISOIterator pc_iter = mvec_isoMember.begin() ; pc_iter != mvec_isoMember.end(); pc_iter++)
  {
    if ( ((pc_iter->isoName(). getEcuType() == a_ecuType))
      && (!ab_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      b_result++;
      mpc_isoMemberCache = pc_iter; // set member cache to member  with searched devClass
    }
  }
  return b_result;
}


IsoItem_c&
IsoMonitor_c::isoMemberEcuTypeInd (IsoName_c::ecuType_t a_ecuType, uint8_t aui8_ind, bool ab_forceClaimedAddress)
{
  int8_t c_cnt = -1;
  for (Vec_ISOIterator pc_iter  = mvec_isoMember.begin() ; pc_iter != mvec_isoMember.end(); pc_iter++)
  {
    if ( ((pc_iter->isoName(). getEcuType()) == a_ecuType)
      && (!ab_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      c_cnt++;
      if (c_cnt == aui8_ind)
      {
        mpc_isoMemberCache = pc_iter; // set member cache to member  with searched devClass
        break; //searched Item found (first element has 0)break; //searched Item found (first element has 0)
      }
    }
  }

  isoaglib_assert( aui8_ind != c_cnt );

  return *mpc_isoMemberCache;
}


uint8_t
IsoMonitor_c::isoMemberDevClassFuncCnt(uint8_t aui8_devClass, uint8_t aui8_function, bool ab_forceClaimedAddress)
{
  uint8_t b_result = 0;
  for (Vec_ISOIterator pc_iter = mvec_isoMember.begin() ; pc_iter != mvec_isoMember.end(); pc_iter++)
  {
    if ( ( (pc_iter->isoName().func()) == aui8_function)
      && ( ((pc_iter->isoName().devClass()) == aui8_devClass) )
      && (!ab_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      b_result++;
      mpc_isoMemberCache = pc_iter; // set member cache to member  with searched devClass
    }
  }
  return b_result;
}


uint8_t
IsoMonitor_c::isoMemberDevClassCnt(uint8_t aui8_devClass, bool ab_forceClaimedAddress)
{
  uint8_t b_result = 0;
  for (Vec_ISOIterator pc_iter = mvec_isoMember.begin() ; pc_iter != mvec_isoMember.end(); pc_iter++)
  {
    if ( ( ((pc_iter->isoName().devClass()) == aui8_devClass) || (aui8_devClass == 0xFF))
      && (!ab_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      b_result++;
      mpc_isoMemberCache = pc_iter; // set member cache to member  with searched devClass
    }
  }
  return b_result;
}


IsoItem_c&
IsoMonitor_c::isoMemberDevClassInd(uint8_t aui8_devClass, uint8_t aui8_ind, bool ab_forceClaimedAddress)
{
  int8_t c_cnt = -1;
  for (Vec_ISOIterator pc_iter  = mvec_isoMember.begin() ; pc_iter != mvec_isoMember.end(); pc_iter++)
  {
    if ( ( ((pc_iter->isoName().devClass()) == aui8_devClass) || (aui8_devClass == 0xFF))
      && (!ab_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      c_cnt++;
      if (c_cnt == aui8_ind)
      {
        mpc_isoMemberCache = pc_iter; // set member cache to member  with searched devClass
        break; //searched Item found (first element has 0)break; //searched Item found (first element has 0)
      }
    }
  }

  isoaglib_assert( aui8_ind != c_cnt );

  return *mpc_isoMemberCache;
}


bool
IsoMonitor_c::existIsoMemberISOName(const IsoName_c& acrc_isoName, bool ab_forceClaimedAddress)
{
  if (!mvec_isoMember.empty() && (mpc_isoMemberCache != mvec_isoMember.end()))
  {
    if ( (mpc_isoMemberCache->isoName() == acrc_isoName )
      && (!ab_forceClaimedAddress || mpc_isoMemberCache->itemState(IState_c::ClaimedAddress))
        )  return true;
  }
  for (mpc_isoMemberCache = mvec_isoMember.begin();
       mpc_isoMemberCache != mvec_isoMember.end();
       mpc_isoMemberCache++)
  {
    if ( (mpc_isoMemberCache->isoName() == acrc_isoName )
      && (!ab_forceClaimedAddress || mpc_isoMemberCache->itemState(IState_c::ClaimedAddress))
        )  return true;
  }
  // if reaching here -> nothing found
  return false;
}


bool
IsoMonitor_c::existIsoMemberNr(uint8_t aui8_nr)
{
  if (!mvec_isoMember.empty() && (mpc_isoMemberCache != mvec_isoMember.end()))
  {
    if ( mpc_isoMemberCache->nr() == aui8_nr ) return true;
  }
  for (mpc_isoMemberCache = mvec_isoMember.begin();
       mpc_isoMemberCache != mvec_isoMember.end();
       mpc_isoMemberCache++)
  {
    if (mpc_isoMemberCache->equalNr(aui8_nr)) return true;
  }
  return false;
}


bool
IsoMonitor_c::isoDevClass2ISONameClaimedAddress(IsoName_c &rc_isoName)
{
  if (existIsoMemberISOName(rc_isoName, true))
  { // there exists a device with exact NAME in claimed address state
    return true;
  }
  else
  { // no item with ISOName found -> adapt DevClassInd
    // search for member with claimed address with same DEVCLASS
    if (isoMemberDevClassCnt(rc_isoName.devClass(), true) > 0)
    { // member with wanted device class exists -> store the ISOName
      rc_isoName = isoMemberDevClassInd(rc_isoName.devClass(), 0, true).isoName();
      return true;
    }
    else if (isoMemberDevClassCnt(rc_isoName.devClass(), false) > 0)
    { // member with wanted device class exists -> store the ISOName
      rc_isoName = isoMemberDevClassInd(rc_isoName.devClass(), 0, false).isoName();
      // even if a device with wanted ISOName exist - it is not claimed
      return false;
    }
    else
    {
      return false;
    }
  }
}


IsoItem_c*
IsoMonitor_c::insertIsoMember(
  const IsoName_c& acrc_isoName,
  uint8_t aui8_nr,
  IState_c::itemState_t ren_state,
  IdentItem_c* apc_identItemForLocalItems,
  bool ab_announceAddition )
{
  IsoItem_c* pc_result = NULL;

  isoaglib_assert( ! existIsoMemberISOName(acrc_isoName) );

  // FROM NOW ON WE DECIDE TO (TRY TO) CREATE A NEW IsoItem_c
  // prepare temp item with wanted data
  mc_tempIsoMemberItem.set (System_c::getTime(), // Actually this value/time can be anything. The time is NOT used in PreAddressClaim and when entering AddressClaim it is being set correctly!
    acrc_isoName, aui8_nr, IState_c::itemState_t(ren_state | IState_c::Active), getMultitonInst() );
  // if it's a local item, we need to set the back-reference.
  if (apc_identItemForLocalItems)
    mc_tempIsoMemberItem.setIdentItem(*apc_identItemForLocalItems);

  // now insert element
  mvec_isoMember.push_front(mc_tempIsoMemberItem);
  mpc_isoMemberCache = mvec_isoMember.begin();
  // item was inserted
  pc_result = &(*mpc_isoMemberCache);
  if (ab_announceAddition)
  { // immediately announce addition.
    // only not do this if you insert a local isoitem that is in state "AddressClaim" - it will be done there if it changes its state to "ClaimedAddress".
    broadcastIsoItemModification2Clients (ControlFunctionStateHandler_c::AddToMonitorList, *pc_result);
  }
#if DEBUG_HEAP_USEAGE
  sui16_isoItemTotal++;

  INTERNAL_DEBUG_DEVICE
    << sui16_isoItemTotal << " x IsoItem_c: Mal-Alloc: "
    <<  sizeSlistTWithMalloc( sizeof(IsoItem_c), sui16_isoItemTotal )
    << "/" << sizeSlistTWithMalloc( sizeof(IsoItem_c), 1 )
    << ", Chunk-Alloc: "
    << sizeSlistTWithChunk( sizeof(IsoItem_c), sui16_isoItemTotal )
    << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  return pc_result;
}


uint8_t
IsoMonitor_c::localIsoMemberCnt()
{
  uint8_t b_count = 0;
  for ( pc_searchCacheC1 = c_arrClientC1.begin(); ( pc_searchCacheC1 != c_arrClientC1.end() ); pc_searchCacheC1++ )
  {  // increase reult count if local ident is already registered in MemberList
    if ( existIsoMemberISOName( (*pc_searchCacheC1)->isoName(), false ) ) b_count++;
  }
  return b_count;
}


IsoItem_c&
IsoMonitor_c::localIsoMemberInd(uint8_t aui8_ind)
{
  IsoItem_c* pc_result = NULL;
  uint8_t b_count = 0;
  for ( pc_searchCacheC1 = c_arrClientC1.begin(); ( pc_searchCacheC1 != c_arrClientC1.end() ); pc_searchCacheC1++ )
  {  // increase reult count if local ident is already registered in MemberList
    if ( existIsoMemberISOName( (*pc_searchCacheC1)->isoName(), false ) )
    {
      if (b_count == aui8_ind)
      { // wanted item found
        pc_result = &( isoMemberISOName( (*pc_searchCacheC1)->isoName(), false ) );
        break;
      }
      b_count++;
    }
  } // for


  isoaglib_assert( pc_result );

  return *pc_result;
}


bool
IsoMonitor_c::existActiveLocalIsoMember()
{
  bool b_result = false; // set default to no success

  // check if actual cache pointer points to active ident
  if ((mpc_activeLocalMember == NULL)
       || (!mpc_activeLocalMember->itemState(IState_c::ClaimedAddress)))
  { // the actual cache pointer isn't correct -> search new one
    const IsoName_c *pc_useISOName = NULL;
    IsoItem_c* pc_monitorItem = NULL;

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
      mpc_activeLocalMember = pc_monitorItem;
    }
    else
    { // no active ident found -> set cache to NULL
      mpc_activeLocalMember = NULL;
    }
  }
  else
  { // cache pointer correct
    b_result = true;
  }
  return b_result;
}


IsoItem_c&
IsoMonitor_c::getActiveLocalIsoMember()
{
  isoaglib_assert( existActiveLocalIsoMember() );
  return *mpc_activeLocalMember;
}


bool
IsoMonitor_c::existLocalIsoMemberNr(uint8_t aui8_nr)
{
  if ( !c_arrClientC1.empty()
        && ( pc_searchCacheC1 != c_arrClientC1.end() )
        && (*pc_searchCacheC1)->equalNr(aui8_nr )
     )
  {
    return true;
  }
  else
  {
    for (pc_searchCacheC1 = c_arrClientC1.begin(); pc_searchCacheC1 != c_arrClientC1.end(); pc_searchCacheC1++)
    {
      if ( (*pc_searchCacheC1)->equalNr(aui8_nr) )
        break;
    }
    return (pc_searchCacheC1 != c_arrClientC1.end());
  }
}


bool
IsoMonitor_c::existLocalIsoMemberISOName (const IsoName_c& acrc_isoName, bool ab_forceClaimedAddress)
{
  if ( (!c_arrClientC1.empty()) && (pc_searchCacheC1 != c_arrClientC1.end()) )
  { // try to use current cache as it points to valid entry
    if ( ((*pc_searchCacheC1)->isoName() == acrc_isoName )
            && (!ab_forceClaimedAddress || (*pc_searchCacheC1)->itemState(IState_c::ClaimedAddress))
       )  return true;
  }
  // if last cache is still vald the function is exited -> start new search
  for (pc_searchCacheC1 = c_arrClientC1.begin();
       pc_searchCacheC1 != c_arrClientC1.end(); pc_searchCacheC1++)
  {
    if ( ((*pc_searchCacheC1)->isoName() == acrc_isoName )
            && (!ab_forceClaimedAddress || (*pc_searchCacheC1)->itemState(IState_c::ClaimedAddress))
       )  return true;
  }
  // if reaching here -> nothing found
  return false;
}


void
IsoMonitor_c::registerControlFunctionStateHandler( ControlFunctionStateHandler_c & arc_client )
{
  for ( ControlFunctionStateHandlerVectorConstIterator_t iter = mvec_saClaimHandler.begin(); iter != mvec_saClaimHandler.end(); iter++ )
  { // check if it points to the same client
    if ( *iter == &arc_client ) return; // already in multimap -> don't insert again
  }
  // if this position is reached, a new item must be inserted
  mvec_saClaimHandler.push_back( &arc_client );

  // now: trigger suitable ControlFunctionStateHandler_c calls for all already known IsoNames in the list
  for ( Vec_ISOIteratorConst iter = mvec_isoMember.begin(); iter != mvec_isoMember.end(); iter++)
  { // inform this ControlFunctionStateHandler_c on existance of the ISONAME node at iter
    arc_client.reactOnIsoItemModification (ControlFunctionStateHandler_c::AddToMonitorList, *iter);
  }
}


void
IsoMonitor_c::deregisterControlFunctionStateHandler (ControlFunctionStateHandler_c & arc_client)
{
  for ( ControlFunctionStateHandlerVectorIterator_t iter = mvec_saClaimHandler.begin(); iter != mvec_saClaimHandler.end(); iter++ )
  { // check if it points to the same client
    if ( *iter == &arc_client )
    {
      mvec_saClaimHandler.erase (iter); // in multimap -> so delete it
      break;
    }
  }
}


void
IsoMonitor_c::broadcastIsoItemModification2Clients( ControlFunctionStateHandler_c::IsoItemModification_t at_isoItemModification, IsoItem_c const& acrc_isoItem ) const
{
  for ( ControlFunctionStateHandlerVectorConstIterator_t iter = mvec_saClaimHandler.begin(); iter != mvec_saClaimHandler.end(); iter++ )
  { // call the handler function of the client
    (*iter)->reactOnIsoItemModification (at_isoItemModification, acrc_isoItem);
  }
}


IsoItem_c&
IsoMonitor_c::isoMemberISOName(const IsoName_c& acrc_isoName, bool ab_forceClaimedAddress)
{
  isoaglib_assert( existIsoMemberISOName( acrc_isoName, ab_forceClaimedAddress ) );
  return static_cast<IsoItem_c&>(*mpc_isoMemberCache);
}


IsoItem_c&
IsoMonitor_c::isoMemberNr(uint8_t aui8_nr)
{
  isoaglib_assert( existIsoMemberNr( aui8_nr ) );
  return static_cast<IsoItem_c&>(*mpc_isoMemberCache);
}


#if 0
IsoItem_c&
IsoMonitor_c::isoMemberISOName(const IsoName_c& acrc_isoName, bool *const pb_success, bool ab_forceClaimedAddress, Vec_ISOIterator *const pbc_iter)
{
  *pb_success = (existIsoMemberISOName(acrc_isoName, ab_forceClaimedAddress));

  if (pbc_iter != NULL)
  {
    *pbc_iter = mpc_isoMemberCache;
  }
  return static_cast<IsoItem_c&>(*mpc_isoMemberCache);
}
#endif


bool
IsoMonitor_c::deleteIsoMemberISOName(const IsoName_c& acrc_isoName)
{
  isoaglib_assert( existIsoMemberISOName( acrc_isoName) );
  // check if "mpc_activeLocalMember" will be invalidated by this deletion
  if (mpc_activeLocalMember == &(*mpc_isoMemberCache))
  { // clear cached active local item - it points to the to be deleted one
    mpc_activeLocalMember = NULL;
  }
  // erase it from list (existIsoMemberISOName sets mpc_isoMemberCache to the wanted item)
  internalIsoItemErase (mpc_isoMemberCache);
  // immediately reset cache, because it may have gotten invalid due to the erase!!
  mpc_isoMemberCache = mvec_isoMember.begin();
  #if DEBUG_HEAP_USEAGE
  sui16_isoItemTotal--;

  INTERNAL_DEBUG_DEVICE
    << sui16_isoItemTotal << " x IsoItem_c: Mal-Alloc: "
    <<  sizeSlistTWithMalloc( sizeof(IsoItem_c), sui16_isoItemTotal )
    << "/" << sizeSlistTWithMalloc( sizeof(IsoItem_c), 1 )
    << ", Chunk-Alloc: "
    << sizeSlistTWithChunk( sizeof(IsoItem_c), sui16_isoItemTotal )
    << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  return true;
}


#if 0
bool
IsoMonitor_c::deleteIsoMemberNr(uint8_t aui8_nr)
{
  isoaglib_assert( existIsoMemberNr( aui8_nr ) )
  return deleteIsoMemberISOName( mpc_isoMemberCache->isoName() );
}
#endif

bool isAddressFree( const IsoItem_c* apc_isoItem, const STL_NAMESPACE::list<IsoItem_c>& vec_isoMember, uint8_t address, bool ab_resolveConflict )
{
  for (STL_NAMESPACE::list<IsoItem_c>::const_iterator pc_iterItem = vec_isoMember.begin();
        pc_iterItem != vec_isoMember.end(); pc_iterItem++)
  {
    if ((pc_iterItem->nr() == address)
         && (ab_resolveConflict || (&(*pc_iterItem) != apc_isoItem))
       )
    { // the tried SA is already used by this item
      return false;
    }
  }
  return true;
}

uint8_t
IsoMonitor_c::unifyIsoSa(const IsoItem_c* apc_isoItem, bool ab_resolveConflict)
{
  const uint8_t scui8_addressRangeLower = 0x80; // 128
  const uint8_t scui8_addressRangeHigher = 0xEE; // 238
  const uint8_t scui8_noAddressFound = 0xFE;

  uint8_t ui8_wishSa = apc_isoItem->nr();
  if ( (ui8_wishSa < scui8_noAddressFound) && (isAddressFree(apc_isoItem, mvec_isoMember, ui8_wishSa, ab_resolveConflict)) )
  { // address is FREE: use it
    return ui8_wishSa;
  }
  //else: address not available
  const bool cb_selfConf = apc_isoItem->selfConf();
  if (!cb_selfConf)
  { // not configurable -> no need to loop free address
    return scui8_noAddressFound;
  }
  else
  { // self-configurable
    if ( (ui8_wishSa >= scui8_addressRangeLower) &&
         (ui8_wishSa <= scui8_addressRangeHigher) )
    { // address already in dynamic address range,
      // search on from the wished address
    }
    else
    { // moved to dynamic address range
      ui8_wishSa = scui8_addressRangeLower;
    }

    const uint8_t stopSa = ui8_wishSa;
    do
    { // try the current ui8_wishSa
      if (isAddressFree(apc_isoItem, mvec_isoMember, ui8_wishSa, ab_resolveConflict))
      { // address is FREE: use it
        return ui8_wishSa;
      }
      else
      {
        ++ui8_wishSa;
        if (ui8_wishSa >= scui8_addressRangeHigher)
          ui8_wishSa = scui8_addressRangeLower;
      }
    } while (ui8_wishSa != stopSa);
    // completely looped one time, but no free address found
    return scui8_noAddressFound;
  }
}


bool
IsoMonitor_c::sendRequestForClaimedAddress( bool ab_force, IsoItem_c *sender )
{ // trigger an initial request for claimed address
  // ( only if no request was detected )
  if ( ( lastIsoSaRequest() != -1 ) && ( ! ab_force ) )
  { // at least one request was already detected
    return false;
  }
  CanPkgExt_c c_data;

  c_data.setIsoPri(6);
  c_data.setIsoPgn(REQUEST_PGN_MSG_PGN);
  c_data.setIsoPs(255); // global request
  if ( sender )
    c_data.setMonitorItemForSA( sender );
  else
    c_data.setIsoSa( 0xFE );
  c_data.setLen(3);
  c_data.setUint32Data( 0, ADDRESS_CLAIM_PGN );
  getIsoBusInstance4Comm() << c_data;

  setLastIsoSaRequest( HAL::getTime() );

  // now send own local SAs in case at least one local ident has claimed adress
  if ( existActiveLocalIsoMember() )
  {
    const uint8_t cui8_localCnt = localIsoMemberCnt();
    for ( uint8_t ui8_ind = 0; ui8_ind < cui8_localCnt; ++ui8_ind )
    { // the function IsoItem_c::sendSaClaim() checks if this item is local and has already claimed a SA
      localIsoMemberInd( ui8_ind ).sendSaClaim();
    }
  }
  return true;
}


bool
IsoMonitor_c::processMsg( const CanPkg_c& arc_data )
{
  bool b_processed = false;

  IsoItem_c *pc_itemSameSa = NULL,
            *pc_itemSameISOName = NULL;

#if DEBUG_ISOMONITOR
  INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL << "IsoMonitor_c::processMsg()-BEGIN" << INTERNAL_DEBUG_DEVICE_ENDL;
  debugPrintNameTable();
#endif

  CanPkgExt_c pkg( arc_data, getMultitonInst() );

  const IsoName_c cc_dataIsoName (pkg.getDataUnionConst());

  // Special NETWORK-MANAGEMENT Handling of ADDRESS_CLAIM_PGN
  // don't do the generic "valid-resolving" check here!
  if( (pkg.isoPgn() & 0x3FF00LU) == ADDRESS_CLAIM_PGN )
  {
    b_processed = true;
    const uint8_t cui8_sa = pkg.isoSa();
    const int32_t ci32_time = pkg.time();

    if ( existIsoMemberISOName (cc_dataIsoName) )
    {
      pc_itemSameISOName = &(isoMemberISOName (cc_dataIsoName));
      if (pc_itemSameISOName->itemState(IState_c::PreAddressClaim))
        // no need to check here for LostAddress, as it's only about the ISOName,
        // and that's correct in all other cases!
      { // this item is still in PreAddressClaim, so don't consider its
        // ISOName as final, it may be able to adapt it when switching to AddressClaim
        // Note: Only LOCAL Items can be in state PreAddressClaim
        pc_itemSameISOName = NULL;
      }
    }
    if ( existIsoMemberNr (cui8_sa) )
    {
      pc_itemSameSa = &(isoMemberNr (cui8_sa));
      if (pc_itemSameSa->itemState(IState_c::PreAddressClaim)
       || pc_itemSameSa->itemState(IState_c::AddressLost) )
      { // this item has no valid address, as it's not (anymore) active.
        // so don't consider it as item with the same SA as the received one.
        pc_itemSameSa = NULL;
      }
    }

    /// Receiving REMOTE Address-Claim
    /// ##############################

    if (NULL == pc_itemSameISOName)
    { // We have NO item with this IsoName
      /// Insert this new remote node (new isoname). Just check before if it steals a SA from someone
      if (NULL == pc_itemSameSa)
      { // New remote node took a fresh SA. The way it should be. Insert it to the list.
        insertIsoMember (cc_dataIsoName, cui8_sa, IState_c::ClaimedAddress, NULL, true);
      }
      else
      { // New remote node stole a SA. Check if it stole from local or remote.
        if (pc_itemSameSa->itemState(IState_c::Local))
        { /// New remote node steals SA from Local node!
          // --> change address if it has lower PRIO
          if (pc_itemSameSa->isoName() < cc_dataIsoName)
          { // the LOCAL item has lower PRIO
            if (pc_itemSameSa->itemState(IState_c::AddressClaim))
            { // the LOCAL item was still in AddressClaim (250ms) phase
              pc_itemSameSa->setNr (unifyIsoSa (pc_itemSameSa, true));
              // No need to broadcast anything, we didn't yet even call AddToMonitorList...
              pc_itemSameSa->sendAddressClaim (false); // false: Address-Claim due to SA-change on conflict **while 250ms-phase** , so we can skip the "AddressClaim"-phase!
            }
            else
            { // the LOCAL item is already up and running, so simply change the SA, claim again and go on
              pc_itemSameSa->changeAddressAndBroadcast (unifyIsoSa (pc_itemSameSa, true));
              pc_itemSameSa->sendAddressClaim (true); // true: Address-Claim due to SA-change on conflict **after 250ms-phase**, so we can skip the "AddressClaim"-phase!
            }

            if (pc_itemSameSa->nr() == 254)
            { // Couldn't get a new address -> remove the item and let IdentItem go to OffUnable!
              if (pc_itemSameSa->getIdentItem())
              { // as it should be! as it's local!
                pc_itemSameSa->getIdentItem()->goOffline(false); // false: we couldn't get a new address for this item!
              }
            }
            insertIsoMember (cc_dataIsoName, cui8_sa, IState_c::ClaimedAddress, NULL, true);
          }
          else
          { // let local IsoItem_c process the conflicting adr claim
            // --> the IsoItem_c::processMsg() will send an ADR CLAIM to indicate the higher prio
            pc_itemSameSa->processAddressClaimed (ci32_time, cui8_sa);
            insertIsoMember (cc_dataIsoName, 0xFE, IState_c::AddressLost, NULL, true);
            /// ATTENTION: We insert the IsoName WITHOUT a valid Address. (and even notify the registered clients about it!)
            /// But this may also happen anyway if you register your handler at a later time -
            /// then your handler will be called with "AddToMonitorList" for all yet known IsoItems -
            /// and those don't need to have a valid SA at this moment!!
          }
        }
        else
        { /// New remote node steals SA from Remote node!
          pc_itemSameSa->giveUpAddressAndBroadcast();
          insertIsoMember (cc_dataIsoName, cui8_sa, IState_c::ClaimedAddress, NULL, true);
        }
      }
    }
    else
    { // We already have an item with this IsoName
      if (pc_itemSameISOName->itemState(IState_c::Local))
      { // We have a local item with this IsoName
        /// @todo SOON-240 Handle the case when a remote message claims an address with a local running IsoName!
        // for now, we shut down our own Ident...
        isoaglib_assert( pc_itemSameISOName->getIdentItem() );
        pc_itemSameISOName->getIdentItem()->goOffline( false ); // false: we couldn't get a new address for this item!

        // now create a new node for the remote SA claim
        insertIsoMember (cc_dataIsoName, cui8_sa, IState_c::ClaimedAddress, NULL, true);
      }
      else
      { // We have a remote item with this IsoName
        /// Change SA of existing remote node. Just check before if it steals a SA from someone
        if (NULL == pc_itemSameSa)
        { // (A9) Existing remote node took a fresh SA. The way it should be. Just change its address.
          pc_itemSameISOName->processAddressClaimed (ci32_time, cui8_sa);
        }
        else
        { // Existing remote node took an already existing SA.
          if (pc_itemSameSa == pc_itemSameISOName)
          { // (A1) Existing remote node reclaimed its SA, so it's just a repeated address-claim.
            pc_itemSameISOName->processAddressClaimed (ci32_time, cui8_sa); // only call to update the timestamp basically
          }
          else if (pc_itemSameSa->itemState(IState_c::Local))
          { // (A5) Existing remote node steals SA from Local node!
            // --> change address if it has lower PRIO
            if (pc_itemSameSa->isoName() < cc_dataIsoName)
            { // the LOCAL item has lower PRIO
              if (pc_itemSameSa->itemState(IState_c::AddressClaim))
              { // the LOCAL item was still in AddressClaim (250ms) phase
                pc_itemSameSa->setNr (unifyIsoSa (pc_itemSameSa, true));
                // No need to broadcast anything, we didn't yet even call AddToMonitorList...
                pc_itemSameSa->sendAddressClaim (false); // false: Address-Claim due to SA-change on conflict **while 250ms-phase** , so we can skip the "AddressClaim"-phase!
              }
              else
              { // the LOCAL item is already up and running, so simply change the SA, claim again and go on
                pc_itemSameSa->changeAddressAndBroadcast (unifyIsoSa (pc_itemSameSa, true));
                pc_itemSameSa->sendAddressClaim (true); // true: Address-Claim due to SA-change on conflict **after 250ms-phase**, so we can skip the "AddressClaim"-phase!
              }

              if (pc_itemSameSa->nr() == 254)
              { // Couldn't get a new address -> remove the item and let IdentItem go to OffUnable!
                if (pc_itemSameSa->getIdentItem())
                { // as it should be! as it's local!
                  pc_itemSameSa->getIdentItem()->goOffline(false); // false: we couldn't get a new address for this item!
                }
              }
              pc_itemSameISOName->processAddressClaimed (ci32_time, cui8_sa);
            }
            else
            { // let local IsoItem_c process the conflicting adr claim
              // --> the IsoItem_c::processMsg() will send an ADR CLAIM to indicate the higher prio
              pc_itemSameSa->processAddressClaimed (ci32_time, cui8_sa);
              pc_itemSameISOName->giveUpAddressAndBroadcast();
            }
          }
          else
          { // (A3) Existing remote node steals other remote node's SA
            pc_itemSameSa->giveUpAddressAndBroadcast();
            pc_itemSameISOName->processAddressClaimed (ci32_time, cui8_sa); // will set the new SA and do broadcasting
          }
        }
      }
    }
  }
  else
  {
    // for all following modules, we do the "typical" "valid-resolving"-check!
    if( !pkg.isValid() || (pkg.getMonitorItemForSA() == NULL) )
      return true;

#ifdef USE_ISO_TASKCONTROLLER_CLIENT
    switch ((pkg.isoPgn() & 0x3FF00LU))
    {
      case PROCESS_DATA_PGN:
        // TODO copy arc data in procesMsg of process_c
        return getProcessInstance4Comm().processMsg( arc_data );
    }
#endif

#ifdef USE_WORKING_SET
    // Handle NON-DESTINATION PGNs
    switch ((pkg.isoPgn() /* & 0x3FFFF */ )) // isoPgn is already "& 0x3FFFF" !
    {
      case WORKING_SET_MASTER_PGN:
      { // working set master
        b_processed = true;
        // in Record Byte 1 (i.e. offset-byte 0) stands the number of TOTAL MEMBERS of this Working-Set.
        // Note that this includes the Master, too - So we need to substract the master
        pkg.getMonitorItemForSA()->setMaster (pkg.getUint8Data (1-1)-1, pkg.time() );
        /** @todo SOON-240: WE HAVE TO BE SURE THAT ALL THOSE x MEMBER DEFINITIONS REALLY ARRIVED!!
         * for now the slaves' isonames are just initialized with IsoNameUnspecified until the WORKING_SET_SLAVE message arrives...
         * AND: Check what happens if the WS-sequence arrives a further time?
         * --> for timings see Iso11783-Part1
         */
      } break;

      case WORKING_SET_MEMBER_PGN:
      { // working set member
        b_processed = true;
        // the working set master places the NAME field of each children
        // in the data part of this message type
        pkg.getMonitorItemForSA()->addSlave (cc_dataIsoName);
      } break;

      default:
        break;
    } // end switch for NON-DESTINATION pgn
#endif
  }

#if DEBUG_ISOMONITOR
  INTERNAL_DEBUG_DEVICE << "IsoMonitor_c::processMsg()-END" << INTERNAL_DEBUG_DEVICE_ENDL;
  debugPrintNameTable();
#endif

  return b_processed; // return if msg was processed by IsoMonitor_c
}


bool
IsoMonitor_c::processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver, int32_t ai_requestTimestamp )
{
  // ADDRESS_CLAIM_PGN is NETWORK-MANAGEMENT, so SA=0xFE is allowed
  if( aui32_pgn == ADDRESS_CLAIM_PGN )
  {
    if (apc_isoItemReceiver == NULL)
    { // No specific destination so it's broadcast: Let all local item answer!
      // update time of last GLOBAL adress claim request to detect dead nodes
      setLastIsoSaRequest (ai_requestTimestamp); // Now using CAN-Pkg-Times, see header for "setLastIsoSaRequest" for more information!

      bool b_processedRequestPGN = false;
      for (Vec_ISOIterator pc_iterItem = mvec_isoMember.begin();
        pc_iterItem != mvec_isoMember.end(); pc_iterItem++)
      { // let all local pc_iterItem process this request
        bool const cb_set = pc_iterItem->itemState (IState_c::Local) &&
        pc_iterItem->sendSaClaim();
        if (cb_set)
          b_processedRequestPGN = true;
      }
      return b_processedRequestPGN; //return value doesn't matter, because the request was for GLOBAL (255), so it isn't NACKed anyway
    }
    else
    { // ISORequestPGN ensured that the Item exists and is local: Let it process!
      return apc_isoItemReceiver->sendSaClaim();
    }
  }
  
  // for all other PGNs, SA=0xFE is not allowed, enforce a valid sender!
  
  // we're not Network Management, so don't answer requests from 0xFE
  if( apc_isoItemSender == NULL )
    return false;

#ifdef USE_WORKING_SET
  switch (aui32_pgn)
  {
    case WORKING_SET_MASTER_PGN: // break intentionally left out - react on both PGNs with sending out the complete ws-announce sequence!
    case WORKING_SET_MEMBER_PGN:
      if (apc_isoItemReceiver == NULL)
      { // No specific destination so it's broadcast: Let all local item answer!
        for (Vec_ISOIterator pc_iterItem = mvec_isoMember.begin();
              pc_iterItem != mvec_isoMember.end(); pc_iterItem++)
        { // let all local pc_iterItem process process this request
          if (pc_iterItem->itemState (IState_c::Local))
          {
            if (pc_iterItem->isMaster())
            { // is Master, so send out the ws-announce. If it's busy now, it will enqueue the request
              (void) pc_iterItem->startWsAnnounce(); // so it's definitely being sent out.
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
        if (apc_isoItemReceiver->isMaster())
        { // is Master, so can it send out the ws-announce?
          (void) apc_isoItemReceiver->startWsAnnounce();
          return true;
        }
        else
        { // no master, can't send out these PGNs
          return false; // let it get NACKed
        }
      }
	  break;
  }
#endif
  // shouldn't happen as we only registered for the above handled PGNs
  return false;
}


#if DEBUG_SCHEDULER
const char*
IsoMonitor_c::getTaskName() const
{ return "IsoMonitor_c"; }
#endif


IsoMonitor_c::Vec_ISOIterator
IsoMonitor_c::internalIsoItemErase( Vec_ISOIterator aiter_toErase )
{
  // first inform SA-Claim handlers on SA-Loss
  /// @todo SOON-240 We need to get sure that the IdentItem doesn't have a dangling reference to this IsoItem!
  broadcastIsoItemModification2Clients (ControlFunctionStateHandler_c::RemoveFromMonitorList, *aiter_toErase);

  return mvec_isoMember.erase( aiter_toErase );
}


void
IsoMonitor_c::updateEarlierAndLatestInterval()
{
  if (getTimePeriod() <= 250)
  { // use HARD-Timing
    mui16_earlierInterval = 0;
    mui16_latestInterval  = (getTimePeriod() / 2);
  }
  else
  { // use SOFT-Timing (using jitter for earlier/after
    mui16_earlierInterval = ( (getTimePeriod() * 3) / 4);
    mui16_latestInterval  =    getTimePeriod();
  }
}


#if DEBUG_ISOMONITOR
void
IsoMonitor_c::debugPrintNameTable()
{
  INTERNAL_DEBUG_DEVICE << "IsoMonitor-NAME/SA-Table - Time:" << HAL::getTime() << INTERNAL_DEBUG_DEVICE_ENDL;
  for (mpc_isoMemberCache = mvec_isoMember.begin();
       mpc_isoMemberCache != mvec_isoMember.end();
       mpc_isoMemberCache++)
  {
    INTERNAL_DEBUG_DEVICE << "   NAME (LE as on CAN): " 
#ifdef SYSTEM_PC
        << std::hex << std::setfill('0')
        << std::setw(2) << int(mpc_isoMemberCache->isoName().outputString()[0]) << " "
        << std::setw(2) << int(mpc_isoMemberCache->isoName().outputString()[1]) << " "
        << std::setw(2) << int(mpc_isoMemberCache->isoName().outputString()[2]) << " "
        << std::setw(2) << int(mpc_isoMemberCache->isoName().outputString()[3]) << " "
        << std::setw(2) << int(mpc_isoMemberCache->isoName().outputString()[4]) << " "
        << std::setw(2) << int(mpc_isoMemberCache->isoName().outputString()[5]) << " "
        << std::setw(2) << int(mpc_isoMemberCache->isoName().outputString()[6]) << " "
        << std::setw(2) << int(mpc_isoMemberCache->isoName().outputString()[7]) << " "
        << " --> SA: "
        << std::setw(2) << int (mpc_isoMemberCache->nr())
        << std::dec << std::endl;
#else
        << int(mpc_isoMemberCache->isoName().outputString()[0]) << " "
        << int(mpc_isoMemberCache->isoName().outputString()[1]) << " "
        << int(mpc_isoMemberCache->isoName().outputString()[2]) << " "
        << int(mpc_isoMemberCache->isoName().outputString()[3]) << " "
        << int(mpc_isoMemberCache->isoName().outputString()[4]) << " "
        << int(mpc_isoMemberCache->isoName().outputString()[5]) << " "
        << int(mpc_isoMemberCache->isoName().outputString()[6]) << " "
        << int(mpc_isoMemberCache->isoName().outputString()[7]) << " "
        << " --> SA: "
        << int (mpc_isoMemberCache->nr())
        << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  }
}
#endif


} // end of namespace __IsoAgLib
