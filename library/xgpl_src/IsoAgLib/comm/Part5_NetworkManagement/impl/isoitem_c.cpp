/*
  isoitem_c.cpp: object which represents an item in a service monitor
    list

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "isoitem_c.h"
// necessary for convert operators
#include "../iisoitem_c.h"
#include "isosystempkg_c.h"
#include "isomonitor_c.h"
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>


#if DEBUG_ADDRESS_CLAIM
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif

namespace __IsoAgLib {

/** default constructor - only used in creation of IsoMonitor_c's temp member variable.
 * all data has to be initialized with a call to "set(..)"
 * afterwards the IsoItem is being inserted to the list using the copy constructor!
 * --> so these constructors are very dumb now!
 */
IsoItem_c::IsoItem_c()
  : BaseItem_c(0, IState_c::IstateNull, 0)
  #ifdef USE_WORKING_SET
  , mpvec_slaveIsoNames (NULL)
  , mi8_slavesToClaimAddress (0) // idle around
  , mi32_timeLastCompletedAnnounceStarted (-1)
  , mi32_timeCurrentAnnounceStarted (-1)
  , mi32_timeAnnounceForRemoteItem (-1)
  , mb_repeatAnnounce (false)
  #endif
  , mui8_nr(0xFE)
  , mb_repeatClaim (false) // wouldn't be needed to be set here as it's set when entering AddressClaim
  , mpc_identItem (NULL) // per default not a local item which has a back-reference to an IdentItem
  , mc_isoName (IsoName_c::IsoNameUnspecified())
{
}

/** copy constructor for IsoItem
  All members are simply copied.
  This constructor is only used at construction-time.
  No IsoItems are copied later on as they're in (s)lists and not vectors..
  @param acrc_src source IsoItem_c instance
*/
IsoItem_c::IsoItem_c(const IsoItem_c& acrc_src)
  : BaseItem_c (acrc_src)
#ifdef USE_WORKING_SET
  //, mpvec_slaveIsoNames (...) // handled in code below!
  , mi8_slavesToClaimAddress (acrc_src.mi8_slavesToClaimAddress)
  , mi32_timeLastCompletedAnnounceStarted (acrc_src.mi32_timeLastCompletedAnnounceStarted)
  , mi32_timeCurrentAnnounceStarted (acrc_src.mi32_timeCurrentAnnounceStarted)
  , mb_repeatAnnounce (acrc_src.mb_repeatAnnounce)
#endif
  , mui8_nr (acrc_src.mui8_nr)
  , mb_repeatClaim (acrc_src.mb_repeatClaim)
  , mpc_identItem (acrc_src.mpc_identItem)
  , mc_isoName (acrc_src.mc_isoName)

{
  #ifdef USE_WORKING_SET
  if ( acrc_src.mpvec_slaveIsoNames == NULL)
  { // source is not a master, so simply copy the NULL
    mpvec_slaveIsoNames = NULL;
  }
  else
  { // source is a master, so create a copy of the pointed list (stl::vector)
    mpvec_slaveIsoNames = new STL_NAMESPACE::vector<IsoName_c> (*(acrc_src.mpvec_slaveIsoNames));
  }
  #endif
}


/** assignment operator for IsoItem_c
  @param acrc_src source IsoItem_c object
*/
IsoItem_c& IsoItem_c::operator=(const IsoItem_c& acrc_src)
{
  BaseItem_c::operator= (acrc_src);
  mc_isoName = acrc_src.mc_isoName;
  mpc_identItem = acrc_src.mpc_identItem;
  mui8_nr = acrc_src.mui8_nr;
  mb_repeatClaim = acrc_src.mb_repeatClaim;

  #ifdef USE_WORKING_SET
  if ( acrc_src.mpvec_slaveIsoNames == NULL)
  { // source is not a master, so simply copy the NULL
    mpvec_slaveIsoNames = NULL;
  }
  else
  { // source is a master, so create a copy of the pointed list (stl::vector)
    mpvec_slaveIsoNames = new STL_NAMESPACE::vector<IsoName_c> (*(acrc_src.mpvec_slaveIsoNames));
  }
  mi8_slavesToClaimAddress = acrc_src.mi8_slavesToClaimAddress;
  mi32_timeLastCompletedAnnounceStarted = acrc_src.mi32_timeLastCompletedAnnounceStarted;
  mi32_timeCurrentAnnounceStarted = acrc_src.mi32_timeCurrentAnnounceStarted;
  mb_repeatAnnounce = acrc_src.mb_repeatAnnounce;
  #endif
  return *this;
}


/** default destructor */
IsoItem_c::~IsoItem_c()
{
#ifdef USE_WORKING_SET
  if (mpvec_slaveIsoNames)
    delete mpvec_slaveIsoNames;
#endif
}


/**
 * General Change Address function. Will set the Address and then call the necessary broadcast.
 */
void
IsoItem_c::changeAddressAndBroadcast (uint8_t aui8_newAddress)
{
  uint8_t ui8_adrBefore = nr();
  bool b_adrBefore = (nr() < 0xFE);
  setNr (aui8_newAddress);
  bool b_adrAfter = (nr() < 0xFE);
  if (b_adrBefore && !b_adrAfter)
  { // address lost
    setItemState (IState_c::AddressLost);
    getIsoMonitorInstance4Comm().broadcastIsoItemModification2Clients (ControlFunctionStateHandler_c::LostAddress, *this);
  }
  else if (!b_adrBefore && b_adrAfter)
  { // reclaimed address
    setItemState (IState_c::ClaimedAddress);
    getIsoMonitorInstance4Comm().broadcastIsoItemModification2Clients (ControlFunctionStateHandler_c::ReclaimedAddress, *this);
  }
  else if (b_adrBefore && b_adrAfter)
  { // changed address
    if (ui8_adrBefore != nr())
    { // only act on a real Change
      getIsoMonitorInstance4Comm().broadcastIsoItemModification2Clients (ControlFunctionStateHandler_c::ChangedAddress, *this);
    }
  }
  // else /*if*/ (!b_adrBefore && !b_adrAfter)
  // {
  //   // nothing to do in this case
  // }
}


/**
  lower comparison between left ISOName uint8_t and right MonitorItem
  @param acrc_left ISOName uint8_t left parameter
  @param acrc_right rigth ServiceItem_c parameter
 */
bool operator<(const IsoName_c& acrc_left, const IsoItem_c& acrc_right)
{
  return (acrc_left < acrc_right.isoName());
}


/** deliver name
  @return pointer to the name uint8_t string (8byte)
*/
const uint8_t* IsoItem_c::name() const
{
  return isoName().outputString();
}


/** deliver name as pure ASCII string
  @param pc_asciiName string where ASCII string is inserted
  @param aui8_maxLen max length for name
*/
void IsoItem_c::getPureAsciiName(int8_t *pc_asciiName, uint8_t aui8_maxLen)
{
  char c_temp[30];
  const uint8_t* pb_src = name();
  sprintf(c_temp, "0x%02x%02x%02x%02x%02x%02x%02x%02x", pb_src[0],pb_src[1],pb_src[2],pb_src[3],
      pb_src[4],pb_src[5],pb_src[6],pb_src[7]);

  uint8_t ui8_len = CNAMESPACE::strlen(c_temp);
  if (aui8_maxLen < ui8_len) ui8_len = aui8_maxLen;
  CNAMESPACE::memcpy(pc_asciiName, c_temp, ui8_len );
  pc_asciiName[ui8_len-1] = '\0';
}


/** set all element data with one call
  @param ai32_time creation time of this item instance
  @param acrc_isoName ISOName code of this item ((deviceClass << 3) | devClInst )
  @param aui8_nr number of this item
  @param aren_status state of this ident (off, claimed address, ...)
  @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
*/
void IsoItem_c::set(int32_t ai32_time, const IsoName_c& acrc_isoName, uint8_t aui8_nr,
        itemState_t aren_status, int ai_multitonInst )
{
  BaseItem_c::set( ai32_time, aren_status, ai_multitonInst );
  setISOName(acrc_isoName);
  setNr(aui8_nr);
}



/// @param ab_fromConflict false => Initial Address-Claim, so we need to go to "AddressClaim"-phase!
///                        true => go to "ClaimedAddress" state, no 250ms wait (for now) as we changed SA!
/// @todo SOON-240 Do we really need this parameter? Should we wait 250ms on change of SA also??? (clarification says: NO!)
/// @todo SOON-240 Merge with sendSaClaim - create an enum for the three cases!
void IsoItem_c::sendAddressClaim (bool ab_fromConflict)
{
  IsoBus_c& c_isobus = getIsoBusInstance4Comm();
  IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
  IsoSystemPkg_c& c_pkg = c_isoMonitor.data();

  if (nr() == 254)
  { // no success -> send NACK and switch to off | error state
    setItemState(IState_c::itemState_t(IState_c::OffUnable | IState_c::Error));
    clearItemState(IState_c::PreAddressClaim);
    /** @todo SOON-240 Do we need to notify the IdentItem of the OFF state? Or will it check that regularly?
              Somehow this IsoItem needs to be thrown out of the list! (New: We don't remove IsoItems anymore ;-) */
  }
  else
  { // success -> start address claim mode
    if (ab_fromConflict)
    { // we had an SA conflict and changed our SA, so let's just get another and keep "ClaimedAddress"
      setItemState(IState_c::ClaimedAddress);
    }
    else
    { // we're initially claiming an address, so we need to wait 250ms...
      setItemState(IState_c::AddressClaim);
    }
  }

  // now nr() has now suitable value
  c_pkg.setIdentType(Ident_c::ExtendedIdent);
  c_pkg.setIsoPri(6);
  c_pkg.setIsoPgn(ADDRESS_CLAIM_PGN);
  c_pkg.setIsoPs(255); // global information
  c_pkg.setMonitorItemForSA( this ); // free SA or NACK flag
  // set NAME to CANPkg
  c_pkg.setDataUnion( outputNameUnion() );
  // now IsoSystemPkg_c has right data -> send
  c_isobus << c_pkg;
  // update timestamp
  updateTime();

  mb_repeatClaim = false;
}


/** periodically time evented actions:
    * find free SA or check if last SA is available
    * send adress claim
  possible errors:
    * dependant error in CanIo_c during send
  @return true -> all planned time event activitie performed
*/
bool IsoItem_c::timeEvent( void )
{
  IsoBus_c& c_isobus = getIsoBusInstance4Comm();
  IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();

  int32_t i32_time = Scheduler_Task_c::getLastRetriggerTime();

  if (itemState(IState_c::PreAddressClaim))
  { // this item is in prepare address claim state -> wait for sending first adress claim
    int32_t i32_lastAdrRequestTime = c_isoMonitor.lastIsoSaRequest();

    // for local instance (mpc_identItem is set): use timestamp from ident item and not the global one from ISO monitor
    if (mpc_identItem)
      i32_lastAdrRequestTime = mpc_identItem->getLastIsoSaRequestForThisItem();

    if (i32_lastAdrRequestTime != -1)
    {
      int32_t i32_wait = 1250 + calc_randomWait();
      if ((i32_time - i32_lastAdrRequestTime) > i32_wait)
      { // last iso adress claim request is still valid and should have been answered till now
        // check if this item is self conf
        // unifyIsoSa delivers actual SA of this item if free
        // if actual SA isn't free and item is NOT-self-conf -> NACK flag 254 is answered
        // if actual SA is not free for self-conf item
        // -> a free SA is searched and answered
        // - if no free SA is found, 254 is answered for NACK
        setNr(c_isoMonitor.unifyIsoSa (this, false)); // false: We're NOT resolving a conflict here.

        sendAddressClaim (false); // false: Initial Address-Claim, so we need to go to "AddressClaim"-phase!
        // we may be "Off" here now but can't delete ourself,
        // so we check IsoItems for Off after we called their timeEvent.
        // => This way we can remove it immediately after detecting that we couldn't exist (anymore)
      }
    }
    else
    { // no adress claim request sent till now
      getIsoMonitorInstance4Comm().sendRequestForClaimedAddress( true );

      if (mpc_identItem)
        mpc_identItem->updateLastIsoSaRequestForThisItem();
    }
  }
  else if (itemState(IState_c::AddressClaim))
  { // item in address claim mode (time between send of claim and
    // check if there is a CAN send conflict during send of adress claim
    // final acceptance of adr claim (wait for possible contention)
    if (c_isobus.stopSendRetryOnErr())
    { // item was in address claim state and send of claim caused error
      setItemState(IState_c::PreAddressClaim);
    }
    else
    {
      if (checkTime(250))
      {
        // no conflict since sent of adress claim since 250ms
        setItemState(IState_c::ClaimedAddress);
        // if there has been a REQUEST_PGN:ADDRESS_CLAIMED_PGN while being in the 250ms-AddressClaim-phase, answer it NOW
        if (mb_repeatClaim)
        { // note: this HAS to be done AFTER setting to ClaimedAddress!
          sendSaClaim();
          mb_repeatClaim = false;
        }
        // now inform the ISO monitor list change clients on NEW client use
        getIsoMonitorInstance4Comm().broadcastIsoItemModification2Clients (ControlFunctionStateHandler_c::AddToMonitorList, *this);
      }
    }
  }
  #ifdef USE_WORKING_SET
  else if ( itemState(IState_c::ClaimedAddress) )
  { // do workingset-stuff if completely announced only
    if (isMaster())
    { // We're master, so check if something has to be done..
      if ( mi8_slavesToClaimAddress == 0 )
      { // 0 means successfully sent out the ws-announce.
        // So Wait. Nothing to be done here...
      }
      else
      { // <0 or >0
        bool b_sendOutWsMessage=true;
        IsoSystemPkg_c& c_pkg = c_isoMonitor.data();
        if ( mi8_slavesToClaimAddress < 0 ) // should be -1, but simply catch all <0 for ws-master sending
        { // Announce WS-Master
          mi8_slavesToClaimAddress = mpvec_slaveIsoNames->size();

          c_pkg.setIsoPgn (WORKING_SET_MASTER_PGN);
          c_pkg.setUint8Data (0, (mi8_slavesToClaimAddress+1));
          c_pkg.setLen8FillUpWithReserved (1);
        }
        else // it must be > 0 here now, so omit: if (mi8_slavesToClaimAddress > 0)
        { // Slave announcing needs 100ms interspace!
          if (!checkTime(100))
            b_sendOutWsMessage = false;
          else
          { // Announce WS-Slave(s)
            c_pkg.setIsoPgn (WORKING_SET_MEMBER_PGN);
            c_pkg.setDataUnion ((*mpvec_slaveIsoNames) [mpvec_slaveIsoNames->size()-mi8_slavesToClaimAddress].outputUnion());
            mi8_slavesToClaimAddress--; // claimed address for one...
          }
        }
        if (b_sendOutWsMessage)
        { // Really send it out on the bus now!
          c_pkg.setIdentType(Ident_c::ExtendedIdent);
          c_pkg.setIsoPri (7);
          c_pkg.setMonitorItemForSA (this);
          c_isobus << c_pkg;
          updateTime();

          // did we send the last message of the announce sequence?
          if (mi8_slavesToClaimAddress == 0)
          { // yes, announcing finished!
            mi32_timeLastCompletedAnnounceStarted = mi32_timeCurrentAnnounceStarted;
            mi32_timeCurrentAnnounceStarted = -1; // no announce running right now.
            // now see if we have to repeat the sequence because it was requested while it was sent...
            if (mb_repeatAnnounce)
            { // repeat the announce-sequence
              mb_repeatAnnounce = false;
              (void) startWsAnnounce();
            }
          }
        }
      }
    }
  }
  #endif

  // nothing to be done
  return true;
}


void
IsoItem_c::processAddressClaimed(
  int32_t ai32_pkgTime,
  uint8_t aui8_pkgSa)
{
  int32_t i32_now = Scheduler_Task_c::getLastRetriggerTime();
  /// @todo SOON-240 what is the sense of the check below?
  if ((i32_now - ai32_pkgTime) > 100) updateTime(i32_now);
  else updateTime(ai32_pkgTime);

  // check if this item is local
  if ( itemState(IState_c::Local ) )
  { // IsoItem_c::processMsg() is only called for local item, when
    // this item has higher PRIO, so that we shall reject the SA steal
    // by resending OUR SA CLAIM
    IsoSystemPkg_c& c_pkg = getIsoMonitorInstance4Comm().data();
    c_pkg.setIdentType(Ident_c::ExtendedIdent);
    c_pkg.setMonitorItemForSA( this );
    c_pkg.setIsoPri(6);
    c_pkg.setIsoPgn(ADDRESS_CLAIM_PGN);
    c_pkg.setIsoPs(255); // global information
      // set NAME to CANPkg
    c_pkg.setDataUnion(outputNameUnion());
      // now IsoSystemPkg_c has right data -> send
    getIsoBusInstance4Comm() << c_pkg;
  }
  else
  { // remote item
    changeAddressAndBroadcast (aui8_pkgSa);
  }
}


/** send an SA claim message
  * - needed to respond on request for claimed SA fomr other nodes
  * - also needed when a local ident triggers a periodic request for SA
  * @return true -> this item has already a claimed SA -> it sent its SA; false -> didn't send SA, as not yet claimed or not local
  */
bool IsoItem_c::sendSaClaim()
{
  // If we're still in AddressClaim, delay the saSending until we're ClaimedAddress
  if (   itemState(IState_c::AddressClaim) ) { mb_repeatClaim = true; return false; }
  if ( ! itemState(IState_c::ClaimedAddress) ) return false; ///< send no answer, if not yet ready claimed
  if ( ! itemState(IState_c::Local) ) return false;
  // now this ISOItem should/can send a SA claim
  #if DEBUG_ADDRESS_CLAIM
  INTERNAL_DEBUG_DEVICE << "Send SA claim (sendSaClaim())" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  IsoSystemPkg_c& c_pkg = getIsoMonitorInstance4Comm().data();
  c_pkg.setIdentType(Ident_c::ExtendedIdent);
  c_pkg.setIsoPri(6);
  c_pkg.setIsoPgn(ADDRESS_CLAIM_PGN);
  c_pkg.setIsoPs(255); // global information
  c_pkg.setMonitorItemForSA( this );
  // set NAME to CANPkg
  c_pkg.setDataUnion(outputNameUnion());
  // now IsoSystemPkg_c has right data -> send
  getIsoBusInstance4Comm() << c_pkg;
  return true;
}


/** calculate random wait time between 0 and 153msec. from NAME and time
  @return wait offset in msec. [0..153]
*/
uint8_t IsoItem_c::calc_randomWait()
{ // perform some calculation from NAME
  uint16_t ui16_result = outputNameUnion()->getUint8Data(0) * outputNameUnion()->getUint8Data(1);
  if ( ( (outputNameUnion()->getUint8Data(2) +1) != 0 )
    && ( System_c::getTime() != 0 )
    && ( (System_c::getTime() / (outputNameUnion()->getUint8Data(2) +1)) != 0 ) )
    ui16_result /= (System_c::getTime() / (outputNameUnion()->getUint8Data(2) +1));
  ui16_result += outputNameUnion()->getUint8Data(3);
  ui16_result %= (outputNameUnion()->getUint8Data(4) + 1);
  ui16_result -= outputNameUnion()->getUint8Data(5);
  ui16_result *= ((outputNameUnion()->getUint8Data(6) + 1) / (outputNameUnion()->getUint8Data(7) + 1));

  // divide by last uint8_t of name till offset in limit
  uint8_t ui8_divisor = outputNameUnion()->getUint8Data(7) + 1;

  for (; ui16_result > 153; ui16_result /= ui8_divisor)
  {
    // Get around potential div0 errors
    ui8_divisor = outputNameUnion()->getUint8Data(7) + 1;

    // Get around potential div1 eternal loop
    if (ui8_divisor == 1)
    {
      ui8_divisor++;
    }
  }

  return static_cast<uint8_t>(ui16_result); // we can cast, as it's assured above that the ui16_result is <= 153.
}


/**
  lower comparison between left IsoItem_c and right ISOName uint8_t
  @param acrc_left left ServiceItem_c parameter
  @param acrc_right ISOName uint8_t right parameter
*/
bool lessThan(const IsoItem_c& acrc_left, const IsoName_c& acrc_right)
{
  return (acrc_left.isoName() < acrc_right);
}


#ifdef USE_WORKING_SET
/// This is for IdentItem's setting of WS-master/slave
void
IsoItem_c::setMasterSlaves (STL_NAMESPACE::vector<IsoName_c>* apvec_slaveIsoNames)
{
  if (mpvec_slaveIsoNames)
  { // already registered as working-set master.
    /** @todo SOON-240 How to handle changing Workingset-State when it's already set? Maybe that's simply not allowed at all... */
  }
  else
  { // create a copy of the vector. (if it's set...)
    if (apvec_slaveIsoNames)
      mpvec_slaveIsoNames = new STL_NAMESPACE::vector<IsoName_c> (*apvec_slaveIsoNames);
    else
      mpvec_slaveIsoNames = NULL;
  }
}


/// This is for incoming WORKING_SET_MASTER_PGN
void
IsoItem_c::setMaster (uint8_t aui8_slaveCount)
{
  if (mpvec_slaveIsoNames)
  { // already registered as working-set master.
    /** @todo SOON-240 How to handle an UPDATE of the working-set definition? First keep it parallel until it finishes successful or fails? Maybe also check if this is a remote item?? */
  }
  else
  { // by creating the vector and setting the pointer, we're master now and have "aui8_slaveCount" slaves.
    // in case an entry is IsoNameUnspecified, it's definition has not yet arrived...
    mpvec_slaveIsoNames = new STL_NAMESPACE::vector<IsoName_c> (aui8_slaveCount, IsoName_c::IsoNameUnspecified());
  }

  mi32_timeAnnounceForRemoteItem = getIsoMonitorInstance4Comm().data().time();
}


/// This is for incoming WORKING_SET_SLAVE_PGN
void
IsoItem_c::addSlave (IsoName_c const& rcc_slaveName)
{
  /// @todo SOON-240 We currently IGNORE any subsequent working-set announces! If the list is full, it stays full and no nothing is overwritten or alike...
  if (mpvec_slaveIsoNames)
  {
    const STL_NAMESPACE::vector<IsoName_c>::iterator end = mpvec_slaveIsoNames->end();
    for (STL_NAMESPACE::vector<IsoName_c>::iterator iter = mpvec_slaveIsoNames->begin();
         iter != end;
         ++iter)
    {
      if (iter->isUnspecified())
      { // found a free slave-entry!
        (*iter) = rcc_slaveName;
        // added the slave, so exit function successfully.
        return; // don't run into possible error-handling below.
      }
      // else: skip as this slave entry is already set/specified.
    }
  }
  // else: someone sent a WORKING_SET_SLAVE_PGN seemingly without sending a WORKING_SET_MASTER_PGN message first..
  // possible error handling for that case here.
}


int32_t IsoItem_c::startWsAnnounce()
{
  int32_t const ci32_timeNow = HAL::getTime();

  if (mi32_timeCurrentAnnounceStarted < 0)
  { // no announce currently running, so start it!
    mi32_timeCurrentAnnounceStarted = ci32_timeNow;
    mi8_slavesToClaimAddress = -1; // start with announcing the master!
  }
  else
  { // we're right in announcing, so delay a second one till after this one ran through
    mb_repeatAnnounce = true;
  }

  // return the Announce-Key
  return ci32_timeNow; // which is the NOW-time!
}


/** get master of this IsoItem
    @return  this if master himself; get master if client; NULL if standalone
  */
IsoItem_c* IsoItem_c::getMaster()
{
  // check if called for a master isoItem_c
  if ( isMaster() ) return this;

  uint8_t ui8_numberOfMembers = getIsoMonitorInstance4Comm().isoMemberCnt(true);
  for ( uint8_t ui8_index = 0; ui8_index < ui8_numberOfMembers; ui8_index++)
  {
    IsoItem_c& refc_isoItem = getIsoMonitorInstance4Comm().isoMemberInd(ui8_index, true);
    if ( refc_isoItem.isMaster() )
    {
      STL_NAMESPACE::vector<IsoName_c>* mpvec_slaveIsoNamesTmp = refc_isoItem.getVectorOfClients();
      if (mpvec_slaveIsoNamesTmp == NULL) continue;

      for (IsoName_c* c_isoNameOfClient = &*mpvec_slaveIsoNamesTmp->begin(); c_isoNameOfClient != &*mpvec_slaveIsoNamesTmp->end(); c_isoNameOfClient++)
      {
        if ( *c_isoNameOfClient == isoName() )
        { // found master to this
          return &refc_isoItem;
        }
      }
    }
  }
  // master of client cannot be found -> standalone client
  return NULL;
}



/// For checking if the WS-Announce is completed use the "announce key" returned from "startWsAnnounce()".
/// Only check for valid announce keys (i.e. ai32_timeAnnounceStarted).
/// Init your announce key to -1, then you're always fine with calling this function!
bool
IsoItem_c::isWsAnnounced (int32_t ai32_timeAnnounceStarted)
{
  if (ai32_timeAnnounceStarted < 0)
    return false;
  else
    return (mi32_timeLastCompletedAnnounceStarted >= ai32_timeAnnounceStarted);
}
#endif

/** convert function - avoids lots of explicit static_casts */
IsoAgLib::iIsoItem_c& IsoItem_c::toIisoItem_c()
{
  // typically would be: static_cast<IsoAgLib::iIsoItem_c&>(*this);
  // but avoiding that due to problems with IAR compiler
  return (IsoAgLib::iIsoItem_c&)(*this);
}

/** convert function - avoids lots of explicit static_casts */
const IsoAgLib::iIsoItem_c& IsoItem_c::toConstIisoItem_c() const
{
  // typically would be: static_cast<const IsoAgLib::iIsoItem_c&>(*this);
  // but avoiding that due to problems with IAR compiler
  return (const IsoAgLib::iIsoItem_c&)(*this);
}

} // end of namespace __IsoAgLib
