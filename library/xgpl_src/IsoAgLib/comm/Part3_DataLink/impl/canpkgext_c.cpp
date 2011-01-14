/*
  canpkgext_c.cpp: header for extended CanPkg_c object, which calls
    data flag converting functions on assign operations

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>

#include <IsoAgLib/util/iassert.h>

#if DEBUG_CAN
//  #include <IsoAgLib/util/impl/util_funcs.h>
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/irs232io_c.h>
  #endif
//  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

bool CanPkgExt_c::msb_runFlag2String = true;


/** caveat: in embedded systems, a static object may not get a clean constructure call,
    so do NOT RELY ON CLEAN INIT BY CONSTRUCTOR
  */
AddressResolveResults_c::AddressResolveResults_c( Ident_c& arc_ident, uint8_t aui8_position )
  : mpc_isoName( NULL ), mpc_monitorItem( NULL ), mrc_ident(arc_ident), mui8_position( aui8_position )
{
  mpc_isoName = new IsoName_c(IsoName_c::IsoNameUnspecified());
  mpc_monitorItem = NULL;
}

AddressResolveResults_c::~AddressResolveResults_c()
{
  delete mpc_isoName;
  mpc_isoName = NULL;
  mpc_monitorItem = NULL;
}


/** variable which holds the results for a resolved source address */
AddressResolveResults_c* CanPkgExt_c::mpc_addrResolveResSA = NULL;

/** variable which holds the results for a resolved destination address */
AddressResolveResults_c* CanPkgExt_c::mpc_addrResolveResDA = NULL;




/** default constructor, which has nothing to do */
CanPkgExt_c::CanPkgExt_c( int ai_multitonInst )
  : CanPkg_c( ai_multitonInst )
{
  static bool sb_addrResolveInitialized = false;
  if ( ! sb_addrResolveInitialized )
  { // addrResolveResSA and addrResolveResDA has to be initialized
    mpc_addrResolveResSA = new AddressResolveResults_c( msc_ident, 0 );
    mpc_addrResolveResDA = new AddressResolveResults_c( msc_ident, 1 );
    sb_addrResolveInitialized = true;
  }
}

/** virtual default destructor, which has nothing to do */
CanPkgExt_c::~CanPkgExt_c()
{
}

/**
  ==> OBSOLETE, because now all can-pkg-data is STATIC!
  ==> REACTIVATE if some NON-STATIC member vars will be added!
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  assign operator to insert informations from one CanPkg_c into another
  @see __IsoAgLib::FilterBox_c::operator>>
  @see CanPkgExt_c::operator=
  @see CanPkgExt_c::getData
  @param acrc_right reference to the source CanPkg_c on the right
  @return reference to the source CanPkg_c to enable assign chains like
      "pkg1 = pkg2 = pkg3 = pkg4;"
const CanPkg_c& CanPkgExt_c::operator=(const CanPkg_c& acrc_right)
{
  return CanPkg_c::operator =( acrc_right );
}
*/

/**
  set an uint8_t value at specified position in string
  @param ai8_pos position [0..7]
  @param aui8_val uint8_t value to set
*/
void CanPkgExt_c::setUint8Data( int8_t ai8_pos, uint8_t aui8_val)
{
  isoaglib_assert((ai8_pos >= 0) && (ai8_pos < 8));
  msc_data.setUint8Data(ai8_pos, aui8_val);
}

/**
  abstract function to transform the string data into flag values
  => derived class must implement suitable data conversion function

  needed for assigning informations from another CanPkg_c or CANPkgExt
  @see CanPkgExt_c::operator=
*/
void CanPkgExt_c::string2Flags()
{ // dummy body - normally NOT called
}

/**
  abstract transform flag values to data string
  => derived class must implement suitable data converting function

  needed for sending informations from this object via CanIo_c on CAN BUS,
  because CanIo_c doesn't know anything about the data format of this type of msg
  so that it can only use an unformated data string from CANPkg
  @see CanPkgExt_c::getData
  @see __IsoAgLib::CanIo_c::operator<<
*/
void CanPkgExt_c::flags2String()
{ // dummy body - normally NOT called
};

/**
  put data into given reference to BIOS related data structure with data, len
  @param rt_ident     reference where the ident is placed for send
  @param rui8_identType reference to the ident type val: 0==std, 1==ext
  @param rb_dlcTarget reference to the DLC field of the target
  @param pb_dataTarget pointer to the data string of the target
*/
void CanPkgExt_c::getData(uint32_t& rt_ident, uint8_t& rui8_identType,
                     uint8_t& rb_dlcTarget, uint8_t* pb_dataTarget)
{
  if ( ! msb_runFlag2String )
  { // data is already setup -> don't call flags2String - but reset it again to true for the next message
    msb_runFlag2String = true;
  }
  else
  {
    flags2String();
  }
  CanPkg_c::getData(rt_ident, rui8_identType, rb_dlcTarget, pb_dataTarget);
}


/**
  set the value of the ISO11783 ident field PGN
  @return parameter group number
*/
void CanPkgExt_c::setIsoPgn(uint32_t aui32_val)
{
  const uint16_t cui16_dpPf = static_cast<uint16_t>((aui32_val >> 8) & 0x03FF); // now extended to 2 bits (added R/EDP)
  const uint8_t cui8_ge = static_cast<uint8_t>(aui32_val);

  /// set DP+PF
  setIsoDpPf (cui16_dpPf);

  /// set PS (if GE)
  // the PGN parameter contains PF at the second lowest byte ==> PF >= 0xF0 correlates to PGN >= 0xF000
  if ( (cui16_dpPf&0xFF) >= 0xF0 )
  { // broadcasted message --> set Byte2 (index 1) from the PGN
    setIsoPs (cui8_ge); // --> PS is used as GroupExtension (GE)
  }
  /// don't set PS (if DA) - has to be done by application itself!
}


/** resolve a given address and set isoName and monitoritem if possible
    @param  arc_addressResolveResults  address to resolve
    @return true -> address could be resolved
  */
bool CanPkgExt_c::resolveAddress( AddressResolveResults_c& arc_addressResolveResults )
{
  if (   ( arc_addressResolveResults.getAddress() <= 0xFD )
      && ( getIsoMonitorInstance4Comm().existIsoMemberNr( arc_addressResolveResults.getAddress() ) )
     )
  { // there exists an item with the given address
    arc_addressResolveResults.mpc_monitorItem = &(getIsoMonitorInstance4Comm().isoMemberNr( arc_addressResolveResults.getAddress() ));

    if ( arc_addressResolveResults.mpc_monitorItem->itemState( IState_c::ClaimedAddress ) )
    { // the existing item has already claimed its address - is fully functional on the BUS
      *arc_addressResolveResults.mpc_isoName = arc_addressResolveResults.mpc_monitorItem->isoName();

      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Member is known with given address." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return true;
    }
  }
  // when we reach this position, the address could not resolve to an already claimed item
  arc_addressResolveResults.mpc_monitorItem = NULL;
  arc_addressResolveResults.mpc_isoName->setUnspecified();
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Member is not known with given address. Set monitorItem and isoName to NULL/unspecified." << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  return false;
}

/** check if source and destination address are valid
    @see     FilterBox_c::processMsg()
    @pre     we want to process a message
    @return  Valid -> both addresses are valid
             Invalid -> one or both addresses are invalid
             OnlyNetworkMgmt -> one or both addresses are only useable for network management
  */
MessageState_t CanPkgExt_c::resolveReceivingInformation()
{
  #if DEBUG_CAN
  INTERNAL_DEBUG_DEVICE <<
    "*-*-*-* PROCESS MESSAGE *-*-*-*--> " <<
#ifdef SYSTEM_PC
    STL_NAMESPACE::hex <<
#endif
      ident() <<
#ifdef SYSTEM_PC
    STL_NAMESPACE::dec <<
#endif
    uint16_t(getLen()) << " " <<
#ifdef SYSTEM_PC
    STL_NAMESPACE::hex <<
#endif
    " " << uint16_t(getUint8Data(0)) <<
    " " << uint16_t(getUint8Data(1)) <<
    " " << uint16_t(getUint8Data(2)) <<
    " " << uint16_t(getUint8Data(3)) <<
    " " << uint16_t(getUint8Data(4)) <<
    " " << uint16_t(getUint8Data(5)) <<
    " " << uint16_t(getUint8Data(6)) <<
    " " << uint16_t(getUint8Data(7)) <<
    INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  // resolve source address
  // in context of receiving SA is remote
  MessageState_t messageStateSA = address2IdentRemoteSa();
  MessageState_t messageStateDA;

  // for receive, the local item is the addressee --> DA is interpreted
  if ( isoPf() >= 0xF0 )
  { // PDU2 format -> no destination address
    // isoName and monitoritem are unspecified
    // we have no explicit address, but PDU2 implies GLOBAL (0xFF)
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We have PDU2 format -> no destination address." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    mpc_addrResolveResDA->mpc_isoName->setUnspecified();
    mpc_addrResolveResDA->mpc_monitorItem = NULL;

    messageStateDA = DaValid;
  }
  else
  { // for receive, the remote item is the sender --> SA is interpreted
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We have PDU1 format -> destination address." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    messageStateDA = address2IdentLocalDa();
  }
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Return value is " << (messageStateSA | messageStateDA) << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "(0) Valid, (1) OnlyNetworkMgmt, (3) Invalid. " << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  //only valid if both messageStates are valid
  return static_cast<MessageState_t>(messageStateSA | messageStateDA);
}


/** report if the combination of address and scope is valid in context of message processing
    @return  true -> address, scope combination is valid
  */
MessageState_t CanPkgExt_c::address2IdentLocalDa()
{
  //we are shure that we have PDU1 format and therefore we have a destination address
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Scope local(DA) with ps-field = " << int(mpc_addrResolveResDA->getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  // now try to resolve the address
  const bool cb_addressBelongsToKnownItem = resolveAddress( *mpc_addrResolveResDA );

  if ( cb_addressBelongsToKnownItem )
  { // only problem might be: when we receive a message sent to a remote node
    if ( mpc_addrResolveResDA->mpc_monitorItem->itemState(IState_c::Local) )
    { // everything is fine
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "We reached a VALID state. Either the target is known." << INTERNAL_DEBUG_DEVICE_ENDL;
        INTERNAL_DEBUG_DEVICE << "address =  " << int(mpc_addrResolveResDA->getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return DaValid;
    }
    else
    { // this is ONLY interesting for BUS-SNOOPING classes like Process_c or handling
      // of Working-Set-Slaves which have to snoop messages to their Working-Set-Master
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "We reached an ONLYNETWORKMGTM state. Destination is a remote node." << INTERNAL_DEBUG_DEVICE_ENDL;
        INTERNAL_DEBUG_DEVICE << "address =  " << int(mpc_addrResolveResDA->getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return static_cast<MessageState_t>(DaInvalidRemote | AdrOnlyNetworkMgmt);
    }
  }
  else if ( mpc_addrResolveResDA->getAddress() == 0xFF )
  { // we received a broadcasted message
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached a VALID state. Target address is 0xFF (broadcast)." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return DaValid;
  }
  else
  { // the receiver is not known OR is 0xFE (which is not a valid receiver address) -> don't process this message
    getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::Can );
    if ( mpc_addrResolveResDA->getAddress() == 0xFE ) {
      #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an INVALID state. Receiver is 0xFE which is NOT possible." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return static_cast<MessageState_t>(DaInvalidAnonymous | AdrInvalid);
    } else {
      #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an INVALID state. Receiver is " << int(mpc_addrResolveResDA->getAddress()) << " which is NOT known." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return static_cast<MessageState_t>(DaInvalidUnknown | AdrInvalid);
    }
  }
}


/** report if the combination of address and scope is valid in context of message processing
    @return  true -> address, scope combination is valid
  */
MessageState_t CanPkgExt_c::address2IdentRemoteSa()
{
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Scope remote(SA) with sa-field = " << int(mpc_addrResolveResSA->getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  // now try to resolve the address
  const bool cb_addressBelongsToKnownItem = resolveAddress( *mpc_addrResolveResSA );

  if ( cb_addressBelongsToKnownItem )
  { // only problem might be: when we receive a message with SA of a local ident
    if ( mpc_addrResolveResSA->mpc_monitorItem->itemState(IState_c::Local) )
    {
      /** @todo SOON-105: isoItem is not const anymore, so we could inform the item of the conflict.
                      Call for this problem case a function of IsoItem_c to inform it about a remote ECU which sends with our SA;
                      arc_addressResolveResults.mpc_monitorItem->affectedConflictCnt( IStateExt_c::Incr, time() );
        */
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Someone sends with one of our SA's." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return static_cast<MessageState_t>(SaInvalidLocal | AdrOnlyNetworkMgmt);
    }
    else
    { // everything is fine
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "We reached a valid state." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return SaValid;
    }
  }
  else if ( mpc_addrResolveResSA->getAddress() == 0xFF )
  { // a SA with 0xFF is never allowed
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an INVALID state with address = 0xFF." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif

    getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::Can );
    return static_cast<MessageState_t>(SaInvalidGlobal | AdrInvalid); // mark as invalid
  }
  else if ( mpc_addrResolveResSA->getAddress() == 0xFE )
  { // each RequestForXY message (not only ReqForAdrClaim) is allowed to be sent with SA == 0xFE
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached a VALID state with address = OxFE." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return SaValid;
  }
  else
  { // normal address, which is not yet known to monitor lists (possible during first SA claim)
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an ONLYNETWORKMGTM state." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return static_cast<MessageState_t>(SaInvalidUnknown | AdrOnlyNetworkMgmt);
  }
}


/** resolve a given monitoritem and get address if possible
    @param arc_addressResolveResults address to resolve
    @return true -> monitoritem could be resolved
            false -> nothing more to be done
  */
bool CanPkgExt_c::resolveMonitorItem( AddressResolveResults_c& arc_addressResolveResults )
{
  //check if monitoritem exist
  if ( arc_addressResolveResults.mpc_monitorItem == NULL )
  {
    if( arc_addressResolveResults.mpc_isoName->isUnspecified() )
        { // leave CAN-Identifier as is
      // nothing more to be done
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Leave CAN-Identifier as is. Nothing more to be done." << INTERNAL_DEBUG_DEVICE_ENDL;
        INTERNAL_DEBUG_DEVICE << "MonitorItem == NULL. ISOName unspecified." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return true;
    }
    else // ( mpc_isoName.isSpecified() )
    { // get mpc_monitorItem if exist in systemmgmt_c
      if ( !getIsoMonitorInstance4Comm().existIsoMemberISOName( *arc_addressResolveResults.mpc_isoName, false ) )
      {
        #if DEBUG_CAN
          INTERNAL_DEBUG_DEVICE << "ISOName specified and item does NOT exists in systemmgmt." << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        return false;
      }
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "ISOName specified and item exists in systemmgmt. Get monitoritem." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif

      arc_addressResolveResults.mpc_monitorItem = &getIsoMonitorInstance4Comm().isoMemberISOName( *arc_addressResolveResults.mpc_isoName, false );
    }
  }
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "We have a Monitoritem." << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // now: verify if the retrieved item is already claimed or if we are preparing a network mgmt message send
  if (   arc_addressResolveResults.mpc_monitorItem->itemState(IState_c::ClaimedAddress)
      || ( isNetworkMgmt() )
     )
  { // if claimed address, sending is allowed under all conditions
    // if address claim and isNetworkMgmt(), sending in AddressClaim state is allowed for network mgmt
    arc_addressResolveResults.setAddress(arc_addressResolveResults.mpc_monitorItem->nr());
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "ClaimedAddress state: " << arc_addressResolveResults.mpc_monitorItem->itemState(IState_c::ClaimedAddress) << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "AddressClaim state:   " << arc_addressResolveResults.mpc_monitorItem->itemState(IState_c::AddressClaim) << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "isNetworkMgmt() =     " << isNetworkMgmt() << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "address =             " << int(arc_addressResolveResults.getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return true;
  }
  else
  { // sending is not valid
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "ItemState is neither ClaimedAddress nor AddressClaim." << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
    return false;
  }
}

/** check if source and destination address are valid
    @see     CanPkgExt_c::operator<<
    @pre     we want to send a message
    @return  Valid -> both addresses are valid
             Invalid -> one or both addresses are invalid
             OnlyNetworkMgmt -> one or both addresses are only useable for network management
  */
bool CanPkgExt_c::resolveSendingInformation()
{
  #if DEBUG_CAN
  INTERNAL_DEBUG_DEVICE << "*-*-*-* SEND MESSAGE *-*-*-*" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // handle SA
  if ( !resolveMonitorItem(*mpc_addrResolveResSA) )
  { // stop any further interpretation, as sending is not valid
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an INVALID state. SA could not be resolved." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return false;
  }
  else if ( mpc_addrResolveResSA->mpc_monitorItem != NULL )
  { // resolving was performed
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "SA could be resolved and monitorItem != NULL." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    if ( !mpc_addrResolveResSA->mpc_monitorItem->itemState(IState_c::Local ) )
    { // resolved MonitorItem is no local item -> no valid send possible
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Sending is not possible because item is not known local." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return false;
    }
  }
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Sending is possible. Item is known local." << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "SA = " << int(mpc_addrResolveResSA->getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // set the SA in the IDENT
  // - when the SA has been directly set by call to setIsoSa(), the requested SA is already
  //   stored in mpc_addrResolveResSA->address
  // ==> we can set least significant byte of the CAN ident in all cases from mpc_addrResolveResSA->address
//  setIdent(*mpc_addrResolveResSA->mpui8_address,0, Ident_c::ExtendedIdent);

  // handle DA for PF -> PDU1
  if ( isoPf() < 0xF0 )
  { // targeted message -> retrieve DA
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "PDU1 format -> existing DA." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    if ( !resolveMonitorItem(*mpc_addrResolveResDA) )
    { // stop any further interpretation, as sending is not valid
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Sending not valid. DA could not be resolved." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return false;
    }
    else if ( mpc_addrResolveResDA->mpc_monitorItem != NULL )
    { // resolving was performed
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Sending valid. DA could be resolved. MonitorItem != NULL." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      if ( mpc_addrResolveResDA->mpc_monitorItem->itemState(IState_c::Local ) )
      { // resolved MonitorItem is no remote item -> no valid send possible
        #if DEBUG_CAN
          INTERNAL_DEBUG_DEVICE << "Sending is not possible because item is known local." << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        return false;
      }
    }
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "Sending is possible. Item is known remote." << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "DA = " << int(mpc_addrResolveResDA->getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  }
  // set the PS in the IDENT
  // - when the PS has been directly set by call to setIsoPs()/setIsoPgn(), the requested PS is already
  //   stored in mpc_addrResolveResDA->address
  // ==> we can set second least significant byte of the CAN ident in all cases from mpc_addrResolveResDA->address
//  setIdent(*mpc_addrResolveResDA->mpui8_address, 1, Ident_c::ExtendedIdent);
  #if DEBUG_CAN
  else
  {
    INTERNAL_DEBUG_DEVICE << "PDU2 format -> PS == GroupExtension." << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
  return true;
}


/** set the value of the ISO11783 ident field PS
    @return PDU Specific
  */
void CanPkgExt_c::setIsoPs(uint8_t aui8_val)
{
  mpc_addrResolveResDA->setAddress(aui8_val);
  mpc_addrResolveResDA->mpc_isoName->setUnspecified();
  mpc_addrResolveResDA->mpc_monitorItem = NULL;
}


/**
    set the value of the ISO11783 ident field SA
    @return source adress
  */
void CanPkgExt_c::setIsoSa(uint8_t aui8_val)
{
  mpc_addrResolveResSA->setAddress(aui8_val);
  mpc_addrResolveResSA->mpc_isoName->setUnspecified();
  mpc_addrResolveResSA->mpc_monitorItem = NULL;
}


/** set the structure for resolve results DA
    @param apc_monitorItem  needed monitoritem
  */
void CanPkgExt_c::setMonitorItemForSA( IsoItem_c* apc_monitorItem )
{
  mpc_addrResolveResSA->mpc_monitorItem = apc_monitorItem;
  // mpc_isoName will not be needed -> set to unspecified
  mpc_addrResolveResSA->mpc_isoName->setUnspecified();
  mpc_addrResolveResSA->setAddress(0xFF);
}


/** set the isoName for resolve SA
    @param acrc_isoName  needed isoName
  */
void CanPkgExt_c::setISONameForSA( const IsoName_c& acrc_isoName )
{
  *mpc_addrResolveResSA->mpc_isoName = acrc_isoName;
  // mpc_monitorItem will be set over mpc_isoName -> reset mpc_monitorItem
  mpc_addrResolveResSA->mpc_monitorItem = NULL;
  mpc_addrResolveResSA->setAddress(0xFF);
}


/** set the structure for resolve results DA
    @param apc_monitorItem  needed monitoritem
  */
void CanPkgExt_c::setMonitorItemForDA( IsoItem_c* apc_monitorItem )
{
  mpc_addrResolveResDA->mpc_monitorItem = apc_monitorItem;
  // mpc_isoName will not be needed -> set to unspecified
  mpc_addrResolveResDA->mpc_isoName->setUnspecified();
  mpc_addrResolveResDA->setAddress(0xFF);
}


/** set the isoName for resolve DA
    @param acrc_isoName  needed isoName
  */
void CanPkgExt_c::setISONameForDA( const IsoName_c& acrc_isoName )
{
  *mpc_addrResolveResDA->mpc_isoName = acrc_isoName;
  // mpc_monitorItem will be set over mpc_isoName -> reset mpc_monitorItem
  mpc_addrResolveResDA->mpc_monitorItem = NULL;
  mpc_addrResolveResDA->setAddress(0xFF);
}


/** check if an adddress could be resolved with monitorItem and isoName
    @param  acrc_addressResolveResults  address to resolve
*/
uint8_t CanPkgExt_c::checkMonitorItemISOName( const AddressResolveResults_c& acrc_addressResolveResults ) const
{
  // check if monitoritem exist and if not resolve it with isoName
  if ( acrc_addressResolveResults.mpc_monitorItem == NULL )
  { // get mpc_monitorItem if exist in systemmgmt_c
    if (     acrc_addressResolveResults.mpc_isoName->isUnspecified()
        || !getIsoMonitorInstance4Comm().existIsoMemberISOName( *acrc_addressResolveResults.mpc_isoName, false )
       )
    { // there exist no iso member with given isoName
      return (acrc_addressResolveResults.getAddress());
    }
    return getIsoMonitorInstance4Comm().isoMemberISOName( *acrc_addressResolveResults.mpc_isoName, false ).nr();
  }
  // know we can be shure that an monitorItem exists
  return acrc_addressResolveResults.mpc_monitorItem->nr();
}


/**
  get the value of the ISO11783 ident field PS
  @return PDU Specific
*/
uint8_t CanPkgExt_c::isoPs() const
{
  // destination address is already valid
  if (mpc_addrResolveResDA->getAddress() != 0xFF ) return mpc_addrResolveResDA->getAddress();

  // check if destination address is willingly 0xFF or if it can be resolved
  return checkMonitorItemISOName( *mpc_addrResolveResDA );
}


/**
  get the value of the ISO11783 ident field SA
  @return source adress
*/
uint8_t CanPkgExt_c::isoSa() const
{
  // source address is already valid
  if (mpc_addrResolveResSA->getAddress() != 0xFF ) return mpc_addrResolveResSA->getAddress();

  // check if source address is willingly 0xFF or if it can be resolved
  return checkMonitorItemISOName( *mpc_addrResolveResSA );
}

} // end of namespace __IsoAgLib
