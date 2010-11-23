/*
  identitem_c.cpp: object for managing identity/ies of the ECU

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

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

#ifdef USE_EEPROM_IO
#include <IsoAgLib/driver/eeprom/impl/eepromio_c.h>
#endif

#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/util/iliberr_c.h>


namespace __IsoAgLib {


IdentItem_c::~IdentItem_c()
{
  isoaglib_assert (mpc_diagnosticPgnHandler);
  delete mpc_diagnosticPgnHandler;
}


IdentItem_c::IdentItem_c (uint16_t aui16_eepromAdr)
  : BaseItem_c (System_c::getTime(), IState_c::IstateNull, -1) // using an INVALID SingletonVecKey as it will be initialized later!
  , mpc_isoItem (NULL)
  , mui16_eepromAdr (aui16_eepromAdr)
  , mui8_globalRunState (GlobalRunStateNeverClaimed)
  , mpc_diagnosticPgnHandler (NULL)
#ifdef USE_WORKING_SET
  , mpvec_slaveIsoNames (NULL)
#endif
  , i32_lastIsoSaRequestForThisItem(-1)
  , mb_readyForActivation( false )

{
  mpc_diagnosticPgnHandler = new DiagnosticPgnHandler_c(*this);

  init (NULL, 0xFF, aui16_eepromAdr
#ifdef USE_WORKING_SET
        ,-1, NULL // -1 indicates we're no working-set!
#endif
      );
}


IdentItem_c::IdentItem_c(
  uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst,
  uint8_t ab_func, uint16_t aui16_manufCode, uint32_t aui32_serNo, uint8_t aui8_preferredSa, uint16_t aui16_eepromAdr,
  uint8_t ab_funcInst, uint8_t ab_ecuInst, bool ab_selfConf
  #ifdef USE_WORKING_SET
  ,int8_t ai8_slaveCount, const IsoName_c* apc_slaveIsoNameList
  #endif
)
  : BaseItem_c (System_c::getTime(), IState_c::IstateNull, -1) // using an INVALID SingletonVecKey as it will be initialized later!
  , mpc_isoItem (NULL)
  , mui16_eepromAdr (0xFFFF)
  , mui8_globalRunState (GlobalRunStateNeverClaimed)
  , mpc_diagnosticPgnHandler(NULL)
#ifdef USE_WORKING_SET
  , mpvec_slaveIsoNames (NULL)
#endif
  , i32_lastIsoSaRequestForThisItem(-1)
  , mb_readyForActivation( false )
{
  mpc_diagnosticPgnHandler = new DiagnosticPgnHandler_c(*this);

  init (aui8_indGroup, aui8_devClass, aui8_devClassInst, ab_func, aui16_manufCode, aui32_serNo,
        aui8_preferredSa, aui16_eepromAdr, ab_funcInst, ab_ecuInst, ab_selfConf
        #ifdef USE_WORKING_SET
        ,ai8_slaveCount, apc_slaveIsoNameList
        #endif
       );
}


// private
void
IdentItem_c::init (IsoName_c* apc_isoNameParam, uint8_t aui8_preferredSa, uint16_t aui16_eepromAdr
  #ifdef USE_WORKING_SET
  ,int8_t ai8_slaveCount, const IsoName_c* apc_slaveIsoNameList
  #endif
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

  // set all other member variables depending on the EEPROM-Address parameter/member variable
  mui16_eepromAdr = aui16_eepromAdr;

  /// Default to true for now. This'll be just set FALSE when initialization is postponed by the user!
  mb_readyForActivation = true;

  bool b_useParameters;
  if (aui16_eepromAdr == 0xFFFF)
  { // Not using EEPROM
    b_useParameters = true;
  }
  else
  { // Using EEPROM
   #ifdef USE_EEPROM_IO
    /// FIRST, default to EEPROM values
    EepromIo_c& rc_eeprom = getEepromInstance();
    uint8_t p8ui8_isoNameEeprom [8];
    rc_eeprom.setg (mui16_eepromAdr);
    rc_eeprom >> mui8_globalRunState >> mui8_preferredSa;
    rc_eeprom.readString (p8ui8_isoNameEeprom, 8);
    mc_isoName = IsoName_c (p8ui8_isoNameEeprom);

#if DEBUG_NETWORK_MANAGEMENT
      INTERNAL_DEBUG_DEVICE << "Read global run state " << (int)mui8_globalRunState << " from EEPROM.";
#endif

    // use fallback to free definition, when the EEPROM has only invalid SA
    if ( mui8_preferredSa == 0xFF ) mui8_preferredSa = 0xFE;

    if (apc_isoNameParam == NULL)
    { // no parameter given, so don't even try to figure out if we'd want to use them!
      b_useParameters = false;
    }
    else
    { /// SECOND, decide on the GlobalRunState and validity of the EEPROM values
      if (mui8_globalRunState == GlobalRunStateNeverClaimed)
      { // FIRST ECU power-up, try with given program parameters (eeprom is only for storage of claimed iso-name!)
        b_useParameters = true;

#if DEBUG_NETWORK_MANAGEMENT
      INTERNAL_DEBUG_DEVICE << " GlobalRunStateNeverClaimed" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      }
      else if (mui8_globalRunState == GlobalRunStateAlreadyClaimed)
      { // FURTHER ECU power-up, use claimed name stored in EEPROM
        // but only if not a new firmware was flashed with new ISO-Name and the EEPROM was NOT reset!

#if DEBUG_NETWORK_MANAGEMENT
      INTERNAL_DEBUG_DEVICE << " GlobalRunStateAlreadyClaimed" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

        if (mc_isoName.isEqualRegardingNonInstFields (*apc_isoNameParam))
        { // no firmware change, so go ahead!
          b_useParameters=false; // use EEPROM values - fine...
        }
        else
        { // firmware changed, we have different parameter-IsoName as in EEPROM, so use the parameters instead!
          b_useParameters=true; // override with most likely newer firmware(=parameter)
        }
      }
      else
      { // Illegal value in EEPROM
        b_useParameters=true;

#if DEBUG_NETWORK_MANAGEMENT
      INTERNAL_DEBUG_DEVICE << " Illegal value in EEPROM." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      }
    }
   #else
    // ERROR: Using EEPROM Address but IsoAgLib is NOT compiled with USE_EEPROM_IO !!!!
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Eeprom );
    #if DEBUG_NETWORK_MANAGEMENT && defined(SYSTEM_PC)
    INTERNAL_DEBUG_DEVICE << "ERROR: Using EEPROM Address in IdentItem_c() construction but IsoAgLib is NOT compiled with USE_EEPROM_IO !!!!" << INTERNAL_DEBUG_DEVICE_ENDL;
    MACRO_ISOAGLIB_ABORT();
    #else
    b_useParameters = true; // fallback because we can't read out the eeprom!
    #endif
   #endif
  }

  /// THIRD, use parameters if SECOND told us to do OR we were running without EEPROM anyway
  if (b_useParameters)
  {
    if (apc_isoNameParam == NULL)
    { /// NO Parameter-IsoName is given AND NO EEPROM address given, so initialize this IdentItem empty to be initialized later with "init"
      mc_isoName.setUnspecified();
      mb_readyForActivation = false; // only case where we don't start the address-claim procedure!
    }
    else
    { /// Parameter-IsoName is given and should be used: use it!
      mc_isoName = *apc_isoNameParam;
    }
    mui8_preferredSa = aui8_preferredSa; // 0xFF in case of "mc_isoName = IsoName_c::IsoNameUnspecified;"
    mui8_globalRunState = GlobalRunStateNeverClaimed; // 0
  }

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
#endif
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
    isoaglib_assert (mpc_diagnosticPgnHandler);
    mpc_diagnosticPgnHandler->init();
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
IdentItem_c::init(
  uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst, uint8_t ab_func, uint16_t aui16_manufCode,
  uint32_t aui32_serNo, uint8_t aui8_preferredSa, uint16_t aui16_eepromAdr, uint8_t ab_funcInst, uint8_t ab_ecuInst, bool ab_selfConf
  #ifdef USE_WORKING_SET
  ,int8_t ai8_slaveCount, const IsoName_c* apc_slaveIsoNameList
  #endif
  )
{
  // temporary to assemble the single parameters to one IsoName so it gets better handled in the following function call
  IsoName_c c_isoNameParam (ab_selfConf, aui8_indGroup, aui8_devClass, aui8_devClassInst,
                            ab_func, aui16_manufCode, aui32_serNo, ab_funcInst, ab_ecuInst);

  init (&c_isoNameParam, aui8_preferredSa, aui16_eepromAdr
        #ifdef USE_WORKING_SET
        ,ai8_slaveCount, apc_slaveIsoNameList
        #endif
       );
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
  bool const cb_sent = getIsoMonitorInstance4Comm().sendRequestForClaimedAddress();
  if (cb_sent)
    updateLastIsoSaRequestForThisItem();

  bool b_isoNameSuccessfulUnified = false;
  // check if isoName is unique and change if needed (to avoid adress conflict on Scheduler_c BUS) and allowed!

  // fixIsoName=true: no unifying possible, calling "unifyIsoISOName" only to see if the isoname exists in the monitor yet or not...
  const bool cb_fixIsoName = (mui8_globalRunState == GlobalRunStateAlreadyClaimed); // If already claimed with this IsoName, we can't change away!
  b_isoNameSuccessfulUnified = getIsoMonitorInstance4Comm().unifyIsoISOName (mc_isoName, cb_fixIsoName);

  if (b_isoNameSuccessfulUnified)
  {
    // insert element in list
    mpc_isoItem = getIsoMonitorInstance4Comm().insertIsoMember (isoName(), mui8_preferredSa,
      IState_c::itemState_t(IState_c::Local | IState_c::PreAddressClaim), this, false);

    if (mpc_isoItem != NULL)
    {
      // register the new item for ISORequestPGN
      getIsoRequestPgnInstance4Comm().registerLocalDevice( isoName() );

      #ifdef USE_WORKING_SET
      mpc_isoItem->setMasterSlaves (mpvec_slaveIsoNames);
      #endif

      mpc_isoItem->timeEvent();
    }
  }

  // set ident_item state to claim address
  if (b_isoNameSuccessfulUnified) setItemState(IState_c::AddressClaim);
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
    bool b_oldAddressClaimState = mpc_isoItem->itemState(IState_c::ClaimedAddress);

    mpc_isoItem->timeEvent();
    // check if IsoItem_c reports now to have finished address claim and store it in Ident_Item
    if ( (mpc_isoItem->itemState(IState_c::ClaimedAddress))
      && (!b_oldAddressClaimState) )
    { // item changed from address claim to claimed address state
      // -> create local filter for processs data
      setItemState(IState_c::ClaimedAddress);

      // set global run state because address claimed already
      mui8_globalRunState = GlobalRunStateAlreadyClaimed;

#if DEBUG_NETWORK_MANAGEMENT
      INTERNAL_DEBUG_DEVICE << "Write global run state " << (int)mui8_globalRunState << " to EEPROM." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

      if (mui16_eepromAdr != 0xFFFF)
      {
        #ifdef USE_EEPROM_IO
        const uint8_t* pcui8_isoName = mc_isoName.outputString();
        EepromIo_c& rc_eeprom = getEepromInstance();
        rc_eeprom.setp (mui16_eepromAdr);
        rc_eeprom << mui8_globalRunState << mui8_preferredSa;
        rc_eeprom.writeString (pcui8_isoName, 8);

        #else
        // ERROR: Using EEPROM Address in IdentItem_c()'s timeEventActive but IsoAgLib is NOT compiled with USE_EEPROM_IO !!!!" << INTERNAL_DEBUG_DEVICE_ENDL;
        getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Eeprom );
        #if DEBUG_NETWORK_MANAGEMENT && defined(SYSTEM_PC)
        INTERNAL_DEBUG_DEVICE << "ERROR: Using EEPROM Address in IdentItem_c()'s timeEventActive but IsoAgLib is NOT compiled with USE_EEPROM_IO !!!!" << INTERNAL_DEBUG_DEVICE_ENDL;
        MACRO_ISOAGLIB_ABORT();
        #endif
        #endif
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
      mpc_isoItem =  rc_isoMonitor.insertIsoMember(isoName(), mui8_preferredSa,
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
