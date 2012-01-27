/*
  identitem_c.cpp: object for managing identity/ies of the ECU

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include <IsoAgLib/hal/hal_typedef.h>
#include "identitem_c.h"
#include "../impl/isomonitor_c.h"
#include "../impl/isorequestpgn_c.h"
#if !defined(IGNORE_VTSERVER_NACK) && defined(USE_ISO_TERMINAL)
  #include "../../Part6_VirtualTerminal_Client/impl/isoterminal_c.h"
#endif

#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/util/iliberr_c.h>

#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

namespace __IsoAgLib {


IdentItem_c::~IdentItem_c()
{
  delete mpc_diagnosticPgnHandler;
  if (mpc_diagnosticsServices) delete mpc_diagnosticsServices;
}


IdentItem_c::IdentItem_c ()
  : BaseItem_c (System_c::getTime(), IState_c::IstateNull, -1) // using an INVALID SingletonVecKey as it will be initialized later!
  , mpc_isoItem(NULL)
  , mpc_dataStorageHandler(NULL)
  , mui8_sa(0x0)
  , mpc_diagnosticPgnHandler (NULL)
  , mpc_diagnosticsServices (NULL)
#ifdef USE_WORKING_SET
  , mpvec_slaveIsoNames (NULL)
#endif
  , i32_lastIsoSaRequestForThisItem(-1)
  , mb_readyForActivation( false )

{
  mpc_diagnosticPgnHandler = new DiagnosticPgnHandler_c(*this);
}


void
IdentItem_c::init ( const IsoName_c& arc_isoNameParam,
                    IsoAgLib::iIdentDataStorage_c& apc_claimDataStorage,
                    int8_t ai8_slaveCount,
                    const IsoName_c* apc_slaveIsoNameList,
                    bool ab_enablediagnosticsServices
  )
{
  /// Check if Item was already properly initialized
  if (mb_readyForActivation)
  { // For init again, you'd first have to stop the identity - this feature is to come somewhen (when needed)...
    getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::System );
    #if DEBUG_NETWORK_MANAGEMENT && defined(SYSTEM_PC)
    INTERNAL_DEBUG_DEVICE << "ERROR: Double initialization of IdentItem_c detected!!!!" << INTERNAL_DEBUG_DEVICE_ENDL;
    MACRO_ISOAGLIB_ABORT();
    #else
    return;
    #endif
  }

  // store claim data Storage handler for further usage
  mpc_dataStorageHandler = &apc_claimDataStorage;
  // get SA from application
  mui8_sa = mpc_dataStorageHandler->loadSa();
  // store our configured iso identity
  mc_isoName = arc_isoNameParam;

#ifdef USE_WORKING_SET
  /// store SLAVE ISONAMEs
  if (mpvec_slaveIsoNames)
  { // first we need to remove the old ones
    delete mpvec_slaveIsoNames;
    mpvec_slaveIsoNames = NULL;
  }
  if (ai8_slaveCount >= 0)
  { // we're workingset-master!
    mpvec_slaveIsoNames = new STL_NAMESPACE::vector<IsoName_c>();
    // which is indicated by "mpvec_slaveIsoNames != NULL"
    if (ai8_slaveCount > 0)
    { // we have Slaves
      mpvec_slaveIsoNames->reserve (ai8_slaveCount);
      if (apc_slaveIsoNameList == NULL)
      { // Catch bad case and to avoid dereferencing NULL we set ai8_slaveCount to 0.
        getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::System );
        ai8_slaveCount = 0;
      }
      for (int i=0; i<ai8_slaveCount; i++)
      { // copy the given Slave-IsoNames to the member stl-vector variable
        mpvec_slaveIsoNames->push_back (apc_slaveIsoNameList [i]);
      }
    }
  }
#else
  // avoid warning
  (void) apc_slaveIsoNameList;
  (void) ai8_slaveCount;
  isoaglib_assert (ai8_slaveCount == -1);
  isoaglib_assert (apc_slaveIsoNameList == NULL);
#endif
  mb_readyForActivation = true;

  if (ab_enablediagnosticsServices)
  {
    mpc_diagnosticsServices = new DiagnosticsServices_c(*this);
    mpc_dataStorageHandler->loadDtcs(mpc_diagnosticsServices->getDtcContainer());
  }
}

IsoAgLib::iIdentDataStorage_c&
IdentItem_c::getIIdentDataStorage()
{
  isoaglib_assert(mpc_dataStorageHandler != NULL);
  return *mpc_dataStorageHandler;
}

bool
IdentItem_c::activate (int ai_multitonInst)
{
  if (mb_readyForActivation)
  { /// Set Item to ACTIVE
    BaseItem_c::set(
      System_c::getTime(),
      (IState_c::itemState_t (IState_c::Active |
                              IState_c::PreAddressClaim |
                              IState_c::Local)),
      ai_multitonInst);

    // The Diagnostics Handler needs to be created on Construction!
    mpc_diagnosticPgnHandler->init();
    if (mpc_diagnosticsServices) mpc_diagnosticsServices->init();
    return true;
  }
  else
  { /// Item wasn't properly initialized - can't activate
    return false;
  }
}


void
IdentItem_c::deactivate()
{ // delete the corresponding IsoItem_c instance in the respective monitoring list
  // and set state to Off (explicitly requested)
  goOffline (true);

  mpc_diagnosticPgnHandler->close();
  if (mpc_diagnosticsServices)
  {
    mpc_dataStorageHandler->storeDtcs(mpc_diagnosticsServices->getDtcContainer());
    mpc_diagnosticsServices->close();
  }

  // if we have a list of slaves, then
#ifdef USE_WORKING_SET
  if (mpvec_slaveIsoNames)
  { // delete list of slaves, too.
    delete mpvec_slaveIsoNames;
    mpvec_slaveIsoNames = NULL;
  }
#endif
}


void
IdentItem_c::restartWithPreAddressClaim( void )
{
  if (mpc_isoItem != NULL)
  { // item is online
    // ->delete item from memberList
    getIsoMonitorInstance4Comm().deleteIsoMemberISOName (isoName());
    mpc_isoItem = NULL;
  }
  setItemState( IState_c::PreAddressClaim );
}


void
IdentItem_c::goOffline (bool ab_explicitlyOffByUser)
{
  if (mpc_isoItem != NULL)
  { // item is online
    // -> send unregister cmd
    // ->delete item from memberList
    getIsoRequestPgnInstance4Comm().unregisterLocalDevice( isoName() );
    getIsoMonitorInstance4Comm().deleteIsoMemberISOName (isoName());
    mpc_isoItem = NULL;
  }
  clearItemState( IState_c::ClaimedAddress );
  if (ab_explicitlyOffByUser) {
    setItemState( IState_c::OffExplicitly );
  } else {
    setItemState( IState_c::OffUnable );
  }
}


bool
IdentItem_c::timeEvent( void )
{
  if (NULL != getIsoItem())
  { // If we have an IsoItem, see if it went off..
    if ( (getIsoItem()->itemState (IState_c::OffExplicitly))
      || (getIsoItem()->itemState (IState_c::OffUnable))
       )
    { // if the IsoItem dictates us to go offline, we're NOT offline by User of course...
      goOffline(false);
    }
  }
  if ( (itemState (IState_c::OffExplicitly))
    || (itemState (IState_c::OffUnable))
     )
  { // We're OFF! So nothing to do!
    return true;
  }
  else
  { // the system is not switched to off state
    // if state is prepare address claim check if isoName unique and insert item in list
    if (itemState(IState_c::PreAddressClaim))
      return timeEventPreAddressClaim();
    else
      return timeEventActive();
  }
}


bool
IdentItem_c::timeEventPreAddressClaim( void )
{
  if( ! getIsoMonitorInstance4Comm().existIsoMemberISOName( mc_isoName, true ) ) 
  {
    // insert element in list
    mpc_isoItem = getIsoMonitorInstance4Comm().insertIsoMember (isoName(), mui8_sa,
      IState_c::itemState_t(IState_c::Local | IState_c::PreAddressClaim), this, false);

    if (mpc_isoItem != NULL)
    {
      bool const cb_sent = getIsoMonitorInstance4Comm().sendRequestForClaimedAddress( false, NULL );
      if (cb_sent)
        updateLastIsoSaRequestForThisItem();

      // register the new item for ISORequestPGN
      getIsoRequestPgnInstance4Comm().registerLocalDevice( isoName() );

      #ifdef USE_WORKING_SET
      mpc_isoItem->setMasterSlaves (mpvec_slaveIsoNames);
      #endif

      mpc_isoItem->timeEvent();
    }
    setItemState(IState_c::AddressClaim);
  }

  return true;
}


bool
IdentItem_c::timeEventActive( void )
{
  #ifdef CHANGE_DEV_CLASS_INST_ON_CONFLICT
  // only change dev class inst and repeated address claim on adress conflicts, if following define
  // is set in masterHeader
  /** @todo SOON-240 USE THIS PRINCIPLE IN CASE WE RECEIVE MESSAGE WITH LOCAL SA -> detected in DataLinkLayer (CanPkgExt_c->resolving)
           --> we should change SA in such a case AND maybe send some sort of DIAGNOSE MESSAGE
    */
  // because of errors caused by terminals, which doesn't claim correctly an address
  // don't do conflict changing of POS
  if ((pc_memberItem->affectedConflictCnt() > 3) && (itemState(IState_c::Din)))
  { // repeated address claim needed
    // send an adress release for according number
    pc_memberItem->sendAdressRelease();
    // delete item from memberList
    getDinMonitorInstance4Comm().deleteDinMemberISOName(isoName());
    pc_memberItem = NULL;
    // restart with init
    init();
    // set time to individual wait time
    setIndividualWait();
  }
  else
  #endif

  /// If we're in Activetimeevent, we always do have a valid mpc_isoItem!
  if (mpc_isoItem->itemState(IState_c::Local))
  {
    bool b_storeAddress = false;
    bool b_oldAddressClaimState = mpc_isoItem->itemState(IState_c::ClaimedAddress);

    mpc_isoItem->timeEvent();
    // check if IsoItem_c reports now to have finished address claim and store it in Ident_Item
    if(mpc_isoItem->itemState(IState_c::ClaimedAddress)) {

      if(!b_oldAddressClaimState) {
        // item changed from address claim to claimed address state
        // -> create local filter for processs data
        setItemState(IState_c::ClaimedAddress);
        b_storeAddress = true;
      }

      // check for a changed SA - there's no state change cause of address conflict handling
      if((mpc_isoItem->nr() != mui8_sa)) {
        mui8_sa = mpc_isoItem->nr();
        b_storeAddress = true;
      }

      isoaglib_assert(mpc_dataStorageHandler);
      if(b_storeAddress) {
        mpc_dataStorageHandler->storeSa(mui8_sa);
      }
    }
  }
  else
  { // remote ISO item has overwritten local item
    /// @todo SOON-240 This case shouldn't happen anymore!!!!!!!! --> See conflict handling when some sends an ADDRESS_CLAIM with our ISONAME!
    // ->see if we can still live with our IsoName
    // ->if not, we'lost because we can't change our IsoName
    IsoMonitor_c& rc_isoMonitor = getIsoMonitorInstance4Comm();
    const bool cb_isoNameStillAvailable = !rc_isoMonitor.existIsoMemberISOName (mc_isoName);

    if (cb_isoNameStillAvailable)
    { // insert element in list
      mpc_isoItem =  rc_isoMonitor.insertIsoMember(isoName(), mui8_sa,
        IState_c::itemState_t(IState_c::Local | IState_c::PreAddressClaim), this, false);
      if ( NULL != mpc_isoItem )
      {
        // register the new item for ISORequestPGN
        getIsoRequestPgnInstance4Comm().registerLocalDevice( isoName() );

        #ifdef USE_WORKING_SET
        mpc_isoItem->setMasterSlaves (mpvec_slaveIsoNames);
        #endif

        mpc_isoItem->timeEvent();
      }
    }
    else
    { /// IsoName now already used on the bus - we can't claim an address now anymore!
      /** @todo SOON-240 insert new error-location/type for those cases! */
      getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::System );
      IsoItem_c& rc_foundIsoItemSameIsoName = rc_isoMonitor.isoMemberISOName (mc_isoName);
      if (rc_foundIsoItemSameIsoName.itemState (IState_c::Local))
      { // now the ISOName is used by some other member on the BUS
        mpc_isoItem = &rc_foundIsoItemSameIsoName; // seems to be our IsoItem although it's a case that shouldn't occur!
        #if DEBUG_NETWORK_MANAGEMENT && defined(SYSTEM_PC)
        INTERNAL_DEBUG_DEVICE << "ERROR: IsoName stolen by other local member, take this IsoItem then, although this shouldn't happen!" << INTERNAL_DEBUG_DEVICE_ENDL;
        MACRO_ISOAGLIB_ABORT();
        #endif
      }
      else
      { // now the ISOName is used by some other member on the BUS
        // ==> conflict
        setItemState (OffUnable); // withdraw from action
        #if DEBUG_NETWORK_MANAGEMENT && defined(SYSTEM_PC)
        INTERNAL_DEBUG_DEVICE << "WARNING: IsoName stolen by other member on the bus (remote), so we have to shut off forever!" << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
    }
  }

  return true;
}


bool
IdentItem_c::equalNr(uint8_t aui8_nr)
{
  bool b_result = false;

  if (mpc_isoItem != NULL) b_result = (mpc_isoItem->nr() == aui8_nr);

  return b_result;
}


} // end of namespace __IsoAgLib
