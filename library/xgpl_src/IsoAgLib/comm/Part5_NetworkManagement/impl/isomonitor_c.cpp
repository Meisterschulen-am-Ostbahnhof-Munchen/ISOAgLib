/*
  isomonitor_c.cpp - object for monitoring members (list of IsoItem_c)

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "isomonitor_c.h"
#include "isosystempkg_c.h"
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>
#include "isorequestpgn_c.h"
#include <IsoAgLib/driver/system/impl/system_c.h>

#ifdef USE_PROCESS
  #include <IsoAgLib/comm/Part7_ProcessData/impl/process_c.h>
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

/** constructor for IsoMonitor_c which can store optional pointer to central Scheduler_c instance
  <!--@param apc_lb optional pointer to central Scheduler_c instance (default NULL -> the later set is needed)-->
*/
IsoMonitor_c::IsoMonitor_c() :
  mvec_isoMember(),
  mc_serviceTool( IsoName_c::IsoNameUnspecified() ),
  mt_handler(*this),
  mt_customer(*this),
  CONTAINER_CLIENT1_CTOR_INITIALIZER_LIST
{
  // functionality moved OUT of the constructor, as the constructor is NOT called in embedded systems for static class instances.
}


void
IsoMonitor_c::init( void )
{
  if (checkAlreadyClosed())
  {
    clearAlreadyClosed();
    mc_data.setMultitonInst( getMultitonInst() );

    #if DEBUG_HEAP_USEAGE
    sui16_isoItemTotal -= mvec_isoMember.size();
    #endif
    isoaglib_assert (mvec_isoMember.empty());
    mpc_isoMemberCache = mvec_isoMember.end();
    mi32_lastSaRequest = -1; // not yet requested. Do NOT use 0, as the first "setLastRequest()" could (and does randomly) occur at time0 as it's called at init() time.
    mc_tempIsoMemberItem.set( 0, IsoName_c::IsoNameUnspecified(), 0xFE, IState_c::Active, getMultitonInst() );

    // register no-service mode
    mc_serviceTool.setUnspecified();

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

    if( getIsoBusInstance4Comm().insertStandardIsoFilter(mt_customer, ((ADDRESS_CLAIM_PGN)+0xFF), false))
      b_configure = true;
#ifdef USE_WORKING_SET
    if (getIsoBusInstance4Comm().insertStandardIsoFilter(mt_customer, (WORKING_SET_MASTER_PGN), false))
      b_configure = true;
    if (getIsoBusInstance4Comm().insertStandardIsoFilter(mt_customer, (WORKING_SET_MEMBER_PGN), false))
      b_configure = true;
#endif

    if (b_configure) {
      getIsoBusInstance4Comm().reconfigureMsgObj();
    }
  }
}


/** every subsystem of IsoAgLib has explicit function for controlled shutdown */
void IsoMonitor_c::close( void )
{
  if ( ! checkAlreadyClosed() )
  {
    // avoid another call
    setAlreadyClosed();

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

    getIsoBusInstance4Comm().deleteFilter( mt_customer, 0x3FFFF00UL, ((ADDRESS_CLAIM_PGN+0xFF) << 8));
#ifdef USE_WORKING_SET
    getIsoBusInstance4Comm().deleteFilter( mt_customer, 0x3FFFF00UL, ((WORKING_SET_MASTER_PGN) << 8));
    getIsoBusInstance4Comm().deleteFilter( mt_customer, 0x3FFFF00UL, ((WORKING_SET_MEMBER_PGN) << 8));
#endif
  }
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



/** deliver reference to data pkg as reference to CanPkgExt_c
  to implement the base virtual function correct
*/
CanPkgExt_c& IsoMonitor_c::dataBase()
{
  return mc_data;
}

/** performs periodically actions,
  possible errors:
  partial error caused by one of the memberItems
  @return true -> all planned activities performed in allowed time
*/
bool IsoMonitor_c::timeEvent( void )
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
      if (i32_checkPeriod > 150) i32_checkPeriod = 150;
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
  if ( getAvailableExecTime() == 0 ) return false;

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
      sendRequestForClaimedAddress( true );
    }
  } // if
  #endif

  return true;
}

/** deliver the count of members in the Monitor-List with given ECU-Type (which is an own IsoAgLib-definition!)
  which optional (!!) match the condition of address claim state
  @param a_ecuType searched ECU-Type code
  @param ab_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return count of members in Monitor-List with ECU-Type == a_ecuType
*/
uint8_t IsoMonitor_c::isoMemberEcuTypeCnt (IsoName_c::ecuType_t a_ecuType, bool ab_forceClaimedAddress)
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

/** deliver one of the members with specific ECU_Type (which is an own IsoAgLib-definition!)
  which optional (!!) match the condition of address claim state
  check first with isoMemberEcuTypeCnt if enough members with wanted ECU-Type and
  optional (!!) property are registered in Monitor-List
  @see isoMemberEcuTypeCnt
  possible errors:
    * Err_c::range there exist less than aui8_ind members with ECU-Type aui8_ecuType
  @param a_ecuType searched ECU-Type code
  @param aui8_ind position of the wanted member in the
                sublist of member with given ECU-Type (first item has aui8_ind == 0 !!)
  @param ab_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return reference to searched element
*/
IsoItem_c& IsoMonitor_c::isoMemberEcuTypeInd (IsoName_c::ecuType_t a_ecuType, uint8_t aui8_ind, bool ab_forceClaimedAddress)
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
  // check if aui8_ind was in correct range
  if (aui8_ind != c_cnt)
  { // wrong range of aui8_ind
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::System );
  }
  return *mpc_isoMemberCache;
}

  /** deliver the count of members in the Monitor-List with given FUNCTION
    and device classe which optional (!!) match the condition of address claim state
    @param aui8_function searched FUNCTION code
    @param aui8_devClass searched DEVCLASS code
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with FUNCTION == aui8_function and DEVCLASS == aui8_devClass
  */
  uint8_t IsoMonitor_c::isoMemberDevClassFuncCnt(uint8_t aui8_devClass, uint8_t aui8_function, bool ab_forceClaimedAddress)
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

/** deliver the count of members in the Monitor-List with given DEVCLASS
  which optional (!!) match the condition of address claim state
  @param aui8_devClass searched DEVCLASS code
  @param ab_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return count of members in Monitor-List with DEVCLASS == aui8_devClass
*/
uint8_t IsoMonitor_c::isoMemberDevClassCnt(uint8_t aui8_devClass, bool ab_forceClaimedAddress)
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

/** deliver one of the members with specific DEVCLASS
  which optional (!!) match the condition of address claim state
  check first with isoMemberDevClassCnt if enough members with wanted DEVCLASS and
  optional (!!) property are registered in Monitor-List
  @see isoMemberDevClassCnt
  possible errors:
    * range there exist less than aui8_ind members with DEVCLASS aui8_devClass
 @param aui8_devClass searched DEVCLASS
 @param aui8_ind position of the wanted member in the
               sublist of member with given DEVCLASS (first item has aui8_ind == 0 !!)
 @param ab_forceClaimedAddress true -> only members with claimed address are used
       (optional, default false)
 @return reference to searched element
*/
IsoItem_c& IsoMonitor_c::isoMemberDevClassInd(uint8_t aui8_devClass, uint8_t aui8_ind, bool ab_forceClaimedAddress)
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
  // check if aui8_ind was in correct range
  if (aui8_ind != c_cnt)
  { // wrong range of aui8_ind
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::System );
  }
  return *mpc_isoMemberCache;
}

/** check if a memberItem with given ISOName exist
  which optional (!!) match the condition of address claim state
  and update local mpc_isoMemberCache
  @param acrc_isoName searched ISOName
  @param ab_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return true -> searched member exist
*/
bool IsoMonitor_c::existIsoMemberISOName(const IsoName_c& acrc_isoName, bool ab_forceClaimedAddress)
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

/** check if a member with given number exist
  which optional (!!) match the condition of address claim state
  and update local mpc_isoMemberCache
  @param aui8_nr searched member number
  @return true -> item found
*/
bool IsoMonitor_c::existIsoMemberNr(uint8_t aui8_nr)
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

/** check if member is in member list with wanted ISOName,
  adapt instance if member with claimed address with other device class inst exist
  @param rc_isoName ISOName to search (-> it's updated if member with claimed address with other dev class inst is found)
  @return true -> member with claimed address with given DEVCLASS found (and rc_isoName has now its ISOName)
*/
bool IsoMonitor_c::isoDevClass2ISONameClaimedAddress(IsoName_c &rc_isoName)
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

/** insert a new IsoItem_c in the list; with unset aui8_nr the member is initiated as
  address claim state; otherwise the given state can be given or state Active is used
  possible errors:
    * badAlloc not enough memory to insert new IsoItem_c isntance
    * busy another member with same ident exists already in the list
  @param acrc_isoName ISOName of the member
  @param aui8_nr member number
  @param ren_state wanted status
  @param apc_identItemForLocalItems 
  @param ab_announceAddition
  @return pointer to new IsoItem_c or NULL if not succeeded
*/
IsoItem_c* IsoMonitor_c::insertIsoMember(const IsoName_c& acrc_isoName,
      uint8_t aui8_nr, IState_c::itemState_t ren_state, IdentItem_c* apc_identItemForLocalItems, bool ab_announceAddition)
{
  IsoItem_c* pc_result = NULL;

  // check if another IsoItem_c with same ISOName already exist
  if (existIsoMemberISOName(acrc_isoName))
  { // another member with same ISOName found
    getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::System );
    return NULL; // don't insert
  }

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
    getIsoMonitorInstance4Comm().broadcastIsoItemModification2Clients (ControlFunctionStateHandler_c::AddToMonitorList, *pc_result);
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

/** deliver the amount of local members which matches the searched proto types
    @return amount of registered local din members
 */
uint8_t IsoMonitor_c::localIsoMemberCnt()
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
    @param aui8_ind index of wanted member (first item == 0)
    @return reference to wanted local member MonitorItem
      (MonitorItem_c is base class of IsoItem_c which serves
      adress, isoName, itemState)
    @return pointer to wanted local iso member (NULL if no suitable IsoItem_c found)
 */
IsoItem_c& IsoMonitor_c::localIsoMemberInd(uint8_t aui8_ind)
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

  if ( pc_result == NULL )
  { // index exceeds array size
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::System );
  }

  return *pc_result;
}

/** check if one of the own local members is active with claimed address at ISO11783
    @return true -> at least one of the own din identities is active with claimed address at ISO11783
    @see IsoMonitor_c::getActiveLocalMember
 */
bool IsoMonitor_c::existActiveLocalIsoMember()
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

/** delivers reference to the first active local member;
    send of ISO11783 system msg demands telling a sender ident member no
    -> using the number of the first active member serves as default (f.e. for requesting other member names)
    can throw an preconditionViolation error, if none of the own identities is active/claimed address yet
    possible errors:
 * SysNoActiveLocalMember on missing own active ident
    @return reference to the IsoItem_c of the first active local member
    @exception preconditionViolation
 */
IsoItem_c& IsoMonitor_c::getActiveLocalIsoMember()
{
  if  (existActiveLocalIsoMember() )
  { // return reference to the pointed ident element
    return *mpc_activeLocalMember;
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
    @param aui8_nr member no to search for
    @return true -> one of the own din identities has the wanted member no
 */
bool IsoMonitor_c::existLocalIsoMemberNr(uint8_t aui8_nr)
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

/** check for own din ident with given ISOName
    @param acrc_isoName ISOName to search for
    @return true -> one of the own din identities has the wanted ISOName
 */
bool IsoMonitor_c::existLocalIsoMemberISOName (const IsoName_c& acrc_isoName, bool ab_forceClaimedAddress)
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


/** register an ControlFunctionStateHandler_c */
void IsoMonitor_c::registerControlFunctionStateHandler( ControlFunctionStateHandler_c & arc_client )
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


/** deregister an ControlFunctionStateHandler */
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


/** this function is used to broadcast an ISO monitor list change to all registered clients */
void IsoMonitor_c::broadcastIsoItemModification2Clients( ControlFunctionStateHandler_c::IsoItemModification_t at_isoItemModification, IsoItem_c const& acrc_isoItem ) const
{
  for ( ControlFunctionStateHandlerVectorConstIterator_t iter = mvec_saClaimHandler.begin(); iter != mvec_saClaimHandler.end(); iter++ )
  { // call the handler function of the client
    (*iter)->reactOnIsoItemModification (at_isoItemModification, acrc_isoItem);
  }
}


/**
  deliver member item with given isoName
  (check with existIsoMemberISOName before access to not defined item)
  possible errors:
    * elNonexistent on failed search
  @param acrc_isoName searched ISOName
  @return reference to searched ISOItem
  @exception containerElementNonexistant
*/
IsoItem_c& IsoMonitor_c::isoMemberISOName(const IsoName_c& acrc_isoName, bool ab_forceClaimedAddress)
{
  if (existIsoMemberISOName(acrc_isoName, ab_forceClaimedAddress))
  { // no error
    return static_cast<IsoItem_c&>(*mpc_isoMemberCache);
  }
  else
  { // wanted element not found
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::System );

    // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    //return reference to first element as fallback
    return mvec_isoMember.front();
  }
}

/** deliver member item with given nr
  (check with existIsoMemberNr before access to not defined item)
  possible errors:
    * elNonexistent on failed search
  @param aui8_nr searched number
  @return reference to searched ISOItem
  @exception containerElementNonexistant
*/
IsoItem_c& IsoMonitor_c::isoMemberNr(uint8_t aui8_nr)
{
  if (existIsoMemberNr(aui8_nr))
  { // no error
    return static_cast<IsoItem_c&>(*mpc_isoMemberCache);
  }
  else
  { // wanted element not found
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::System );

    // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    //return reference to first element as fallback
    return mvec_isoMember.front();
  }
}

/** deliver member item with given ISOName, set pointed bool var to true on success
  and set a Member Array Iterator to the result
  @param acrc_isoName searched ISOName
  @param pb_success bool pointer to store the success (true on success)
  @param pbc_iter optional member array iterator which points to searched IsoItem_c on success
  @return reference to the searched item
*/
IsoItem_c& IsoMonitor_c::isoMemberISOName(const IsoName_c& acrc_isoName, bool *const pb_success, bool ab_forceClaimedAddress, Vec_ISOIterator *const pbc_iter)
{
  *pb_success = (existIsoMemberISOName(acrc_isoName, ab_forceClaimedAddress));

  if (pbc_iter != NULL)
  {
    *pbc_iter = mpc_isoMemberCache;
  }
  return static_cast<IsoItem_c&>(*mpc_isoMemberCache);
}

/**
  delete item with specified isoName
  possible errors:
    * elNonexistent no member with given ISOName exists
  @param acrc_isoName ISOName of to be deleted member
*/
bool IsoMonitor_c::deleteIsoMemberISOName(const IsoName_c& acrc_isoName)
{
  if (existIsoMemberISOName(acrc_isoName))
  { // set correct state
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
  else
  { // to be deleted member ISOName does not exist
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::System );
    return false;
  }
}

/** delete item with specified member number
  possible errors:
    * elNonexistent no member with given ISOName exists
  @param aui8_nr SA of to be deleted member
*/
bool IsoMonitor_c::deleteIsoMemberNr(uint8_t aui8_nr)
{
  if (existIsoMemberNr(aui8_nr))
  { // use deleteIsoMemberISOName
    return deleteIsoMemberISOName(mpc_isoMemberCache->isoName());
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
  @param rc_isoName reference to ISOName var (is changed directly if needed!!)
  @param ab_dontUnify don't touch the IsoName because it most likely has already (at least once) claimed
                      an SA with this IsoName, so it can't change away, it has to keep its identity
  @return true -> referenced ISOName is now unique
*/
bool IsoMonitor_c::unifyIsoISOName (IsoName_c& rc_isoName, bool ab_dontUnify)
{
  bool b_result = true;
  if (existIsoMemberISOName (rc_isoName))
  {
    if (ab_dontUnify)
    { // if we shouldn't unify, we're lost
      b_result = false;
    }
    else
    { // we can try to unify, so search now with changed instances if one is available
      IsoName_c c_tempISOName = rc_isoName;

      // store the pos part of given isoName
      int16_t tempPos = (rc_isoName.devClassInst()),
              diff = 1;

      for (; diff < 16; diff++)
      {
        c_tempISOName.setDevClassInst( (tempPos + diff) & 0x0F ); // modulo through all 16 instances
        if (!(existIsoMemberISOName(c_tempISOName)))
        { // new instance can't be found in list -> it is unique
          rc_isoName.setDevClassInst( (tempPos + diff) & 0x0F );
          break;
        }
      }
      if (diff == 16)
      { // we tried all 16, but none was available!
        b_result = false;
      }
    } // else: if we can't unify, we're lost here as such an IsoName is already claimed on the bus
  } // else: IsoName doesn't exist --> fine!

  if (!b_result) getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::System );
  return b_result;
}

/** check if SA of an announcing IsoItem_c is unique and deliver
  another free SA if not yet unique (else deliver its actual SA if unique yet)
  @param apc_isoItem pointer to announcing IsoItem_c
  @param ab_resolveConflict true => don't use current SA because someone else
                                    claimed it with a higher prior isoname,
                                    so we have to change our SA. This is needed
                                    as the new member is not yet in the list,
                                    so the algorithm would still take the current SA.
                                    We can't insert the new item, as we don't want
                                    a state where two items have the same SA.
                            false => no conflict to resolve, so we can take the current
                                      source address if it's available!
  @return free unique SA (if possible the actual SA of the pointed item)
    (if wanted SA is not free for NOT-self-conf item or if no free SA is available
      254 is answered -> special flag for NACK)
*/
uint8_t IsoMonitor_c::unifyIsoSa(const IsoItem_c* apc_isoItem, bool ab_resolveConflict)
{
  uint8_t ui8_wishSa = apc_isoItem->nr();
  const bool cb_selfConf = apc_isoItem->selfConf();

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

  /// @todo SOON-240 Maybe wrap around to SA 0 if we couldn't allocate one up to 253!?

  // while we have addresses to try, try!
  while (ui8_wishSa < 254)
  { // try the current ui8_wishSa
    bool b_free = true;
    for (Vec_ISOIterator pc_iterItem = mvec_isoMember.begin();
          pc_iterItem != mvec_isoMember.end(); pc_iterItem++)
    {
      if ((pc_iterItem->nr() == ui8_wishSa)
           && (ab_resolveConflict || (&(*pc_iterItem) != apc_isoItem))
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
  @pre IsoBus is properly initialized.
  @param ab_force false -> send request only if no request was detected until now
  @return true -> request was sent
  */
bool IsoMonitor_c::sendRequestForClaimedAddress( bool ab_force )
{ // trigger an initial request for claimed address
  // ( only if no request was detected )
  if ( ( lastIsoSaRequest() != -1 ) && ( ! ab_force ) )
  { // at least one request was already detected
    return false;
  }
  bool b_sendOwnSa = false;
  // now it's needed to send
  const int32_t i32_time = HAL::getTime();

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
  data().setUint32Data( 0, ADDRESS_CLAIM_PGN );
  data().setLen(3);
  // now IsoSystemPkg_c has right data -> send
  getIsoBusInstance4Comm() << data();
  // store adress claim request time
  setLastIsoSaRequest(i32_time);

  // now send own SA in case at least one local ident has yet claimed adress
  if (b_sendOwnSa)
  {
    #if DEBUG_ISOMONITOR
    INTERNAL_DEBUG_DEVICE << "Send checking SA request (sendRequestForClaimedAddress())" << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    const uint8_t cui8_localCnt = localIsoMemberCnt();
    for ( uint8_t ui8_ind = 0; ui8_ind < cui8_localCnt; ui8_ind++ )
    { // the function IsoItem_c::sendSaClaim() checks if this item is local and has already claimed a SA
      localIsoMemberInd( ui8_ind ).sendSaClaim();
    }
  }
  return true;
}

/** process system msg with informations which are
  important for managing of members
  @return true -> message processed by IsoMonitor_c; false -> process msg by ServiceMonitor
*/
bool IsoMonitor_c::processMsg()
{
  bool b_processed = false;

  IsoItem_c *pc_itemSameSa = NULL,
            *pc_itemSameISOName = NULL;

#if DEBUG_ISOMONITOR
  INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL << "IsoMonitor_c::processMsg()-BEGIN" << INTERNAL_DEBUG_DEVICE_ENDL;
  debugPrintNameTable();
#endif

  // decide whether the message should be processed
  if ( mc_serviceTool.isSpecified() )
  { // we are in diagnostic mode --> check if the message sender is the diagnostic tool
    IsoName_c const& rcc_isoNameSa = data().getISONameForSA();
    if (rcc_isoNameSa.isSpecified())
    {
      if ( rcc_isoNameSa != mc_serviceTool ) return false;
    }
    else if ( data().isoName() != mc_serviceTool ) return false;
    // if we reach here, the received message has to be processed, as the sender is the
    // diagnostic tool
  }


  // Handle DESTINATION PGNs
  switch ((data().isoPgn() & 0x3FF00))
  {
    case ADDRESS_CLAIM_PGN:
    { // address claim
      b_processed = true;
      const IsoName_c cc_dataIsoName (data().isoName());
      const uint8_t cui8_sa = data().isoSa();
      const int32_t ci32_time = data().time();

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
          /// WE GOT A PROBLEM! SOMEONE IS SENDING WITH OUR ISONAME!
          /// @todo SOON-240 Handle the case when a remote message claims an address with a local running IsoName!
          // for now, we shut down our own Ident...
          if (pc_itemSameISOName->getIdentItem())
          { // as it should be! as it's local!
            if ( (pc_itemSameISOName->itemState(IState_c::PreAddressClaim)              )
              && (pc_itemSameISOName->getIdentItem()->itemState(IState_c::AddressClaim) ) )
            { // special state:
              // A) the IdentItem_c started already the SA-Claim phase
              // BUT
              // B) the IsoItem_c has not yet sent any SA-Claim
              // ===>> thus we can restart the SA-Claim phase of IdentItem_c with removal of IsoItem_c and setting
              //       item state of IdentItem_c back to PreAddressClaim
              pc_itemSameISOName->getIdentItem()->restartWithPreAddressClaim();
            }
            else
            { // the IsoItem_c of the local ident has already sent a first SA-Claim, so that no clean restart is allowed
              pc_itemSameISOName->getIdentItem()->goOffline(false); // false: we couldn't get a new address for this item!
            }
            // now create a new node for the remote SA claim
            insertIsoMember (cc_dataIsoName, cui8_sa, IState_c::ClaimedAddress, NULL, true);
          }
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
    } break;

#ifdef USE_PROCESS
    case PROCESS_DATA_PGN:
      static_cast<__IsoAgLib::CanPkg_c&>(getProcessInstance4Comm().data())
        = static_cast<__IsoAgLib::CanPkg_c&>(data());
      getProcessInstance4Comm().data().string2Flags();
      return getProcessInstance4Comm().processMsg();
#endif
  } // end switch for DESTINATION pgn

  // Handle NON-DESTINATION PGNs
  switch ((data().isoPgn() /* & 0x3FFFF */ )) // isoPgn is already "& 0x3FFFF" !
  {
    #ifdef USE_WORKING_SET
    case WORKING_SET_MASTER_PGN:
    { // working set master
      b_processed = true;
      // get and check sender
      IsoItem_c* pc_masterItem = data().getMonitorItemForSA();
      // we have to check here because we also receive network-management messages here (i.e. "virtual bool isNetworkMgmt() const { return true; }").
      if (pc_masterItem)
      { // in Record Byte 1 (i.e. offset-byte 0) stands the number of TOTAL MEMBERS of this Working-Set.
        // Note that this includes the Master, too - So we need to substract the master
        pc_masterItem->setMaster (data().getUint8Data (1-1)-1);
        /** @todo SOON-240: WE HAVE TO BE SURE THAT ALL THOSE x MEMBER DEFINITIONS REALLY ARRIVED!!
         * for now the slaves' isonames are just initialized with IsoNameUnspecified until the WORKING_SET_SLAVE message arrives...
         * AND: Check what happens if the WS-sequence arrives a further time?
         * --> for timings see Iso11783-Part1
         */
      }
      else
      { // shouldn't happen normally (but could be a theoretical message on the bus,
        // so we needed the check. But we don't have to do anything...
        // ! Someone with an unknown SA sent a WORKING_SET_MASTER_PGN message...
        /// @todo SOON-240 Should we register such errors at all? Should we register such errors as "(iLibErr_c::Inconsistency, iLibErr_c::System)" ?
        //getILibErrInstance().registerError( iLibErr_c::Inconsistency, iLibErr_c::System );
      }
    } break;

    case WORKING_SET_MEMBER_PGN:
    { // working set member
      b_processed = true;
      // get and check sender
      IsoItem_c* pc_masterItem = data().getMonitorItemForSA();
      // we have to check here because we also receive network-management messages here (i.e. "virtual bool isNetworkMgmt() const { return true; }").
      if (pc_masterItem)
      {
        // the working set master places the NAME field of each children
        // in the data part of this message type
        pc_masterItem->addSlave (data().isoName());
      }
      else
      { // shouldn't happen normally (but could be a theoretical message on the bus,
        // so we needed the check. But we don't have to do anything...
        // ! Someone with an unknown SA sent a WORKING_SET_MASTER_PGN message...
        /// @todo SOON-240 Should we register such errors at all? Should we register such errors as "(iLibErr_c::Inconsistency, iLibErr_c::System)" ?
        //getILibErrInstance().registerError( iLibErr_c::Inconsistency, iLibErr_c::System );
      }
    } break;
    #endif

    default:
      break;
  } // end switch for NON-DESTINATION pgn

#if DEBUG_ISOMONITOR
  INTERNAL_DEBUG_DEVICE << "IsoMonitor_c::processMsg()-END" << INTERNAL_DEBUG_DEVICE_ENDL;
  debugPrintNameTable();
#endif

  return b_processed; // return if msg was processed by IsoMonitor_c
}

bool
IsoMonitor_c::processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* apc_isoItemReceiver)
{
  switch (aui32_pgn)
  {
    case ADDRESS_CLAIM_PGN:
      if (apc_isoItemReceiver == NULL)
      { // No specific destination so it's broadcast: Let all local item answer!
        // update time of last GLOBAL adress claim request to detect dead nodes
        setLastIsoSaRequest (getIsoRequestPgnInstance4Comm().getTimeOfLastRequest()); // Now using CAN-Pkg-Times, see header for "setLastIsoSaRequest" for more information!

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

#ifdef USE_WORKING_SET
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
#endif

    default:
      // shouldn't happen as we only registered for the above handled PGNs
      return false;
  }
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


//! Function set ui16_earlierInterval and
//! ui16_laterInterval that will be used by
//! getTimeToNextTrigger(retriggerType_t)
//! can be overloaded by Childclass for special condition
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



/** command switching to and from special service / diagnostic mode.
    setting the flag mc_serviceTool controls appropriate handling
  */
void IsoMonitor_c::setDiagnosticMode( const IsoName_c& acrc_serviceTool)
{
  mc_serviceTool = acrc_serviceTool;
  if ( mc_serviceTool.isUnspecified() )
  { // back to normal operation --> trigger send of Req4SaClaim
    sendRequestForClaimedAddress( true );
  }
  else
  { // switch from normal operation to diagnostic mode
    Vec_ISOIterator pc_iter = mvec_isoMember.begin();
    bool b_updateCacheIterator = false;
    while ( pc_iter != mvec_isoMember.end() )
    {
      if ( ( pc_iter->isoName() == mc_serviceTool ) || ( pc_iter->itemState(IState_c::Local) ) )
      { // current item is service tool or is local
        // --> increment iterator to next item
        pc_iter++;
      }
      else
      { // remove the item from the monitor list
        pc_iter = internalIsoItemErase( pc_iter );
        b_updateCacheIterator = true;
      }
    }
    if (b_updateCacheIterator)
    { // due to erasing an element, the cache may get invalid, so we better reset it here!
      mpc_isoMemberCache = mvec_isoMember.begin();
    }
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
