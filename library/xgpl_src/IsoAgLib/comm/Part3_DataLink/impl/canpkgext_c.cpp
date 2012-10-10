/*
  canpkgext_c.cpp: header for an ISO-supported CanPkg_c object

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

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
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/irs232io_c.h>
  #endif
#endif

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {




AddressResolveResults_c::AddressResolveResults_c( Ident_c& arc_ident, uint8_t aui8_position )
  : mc_isoName()
  , mpc_monitorItem( NULL )
  , mrc_ident(arc_ident)
  , mui8_position( aui8_position )
{}

AddressResolveResults_c::~AddressResolveResults_c()
{}

AddressResolveResults_c::AddressResolveResults_c(const AddressResolveResults_c& acrc_src)
  : mc_isoName( acrc_src.mc_isoName ),
    mpc_monitorItem( acrc_src.mpc_monitorItem ),
    mrc_ident( acrc_src.mrc_ident ),
    mui8_position( acrc_src.mui8_position )
{}


CanPkgExt_c::CanPkgExt_c()
  : CanPkg_c(),
    mc_addrResolveResSA( mc_ident, 0 ),
    mc_addrResolveResDA( mc_ident, 1 ),
    mt_msgState( MessageValid )
{
  CanPkg_c::setIdentType(Ident_c::ExtendedIdent);
}


CanPkgExt_c::~CanPkgExt_c()
{}


CanPkgExt_c::CanPkgExt_c( const CanPkgExt_c& arc_src )
  : CanPkg_c( arc_src ),
    mc_addrResolveResSA( arc_src.mc_addrResolveResSA ),
    mc_addrResolveResDA( arc_src.mc_addrResolveResDA ),
    mt_msgState( arc_src.mt_msgState )
{
}


CanPkgExt_c::CanPkgExt_c( const CanPkg_c& arc_src, int ai_multitonInst )
  : CanPkg_c( arc_src ),
    mc_addrResolveResSA( mc_ident, 0 ),
    mc_addrResolveResDA( mc_ident, 1 ),
    mt_msgState( MessageValid ) // note that this is done here in the body afterwards
{
  mt_msgState = resolveReceivingInformation( ai_multitonInst );
  isoaglib_assert( arc_src.identType() == Ident_c::ExtendedIdent ); 
}


void
CanPkgExt_c::setIsoPgn(uint32_t aui32_val)
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


bool
CanPkgExt_c::resolveAddress( AddressResolveResults_c& result, int ai_multitonInstance )
{
  // TODO: possible optimization: remove isoname setting in CAN rx
  result.mpc_monitorItem = getIsoMonitorInstance( ai_multitonInstance ).isoMemberNrFast( result.getAddress() );

  if( result.mpc_monitorItem && result.mpc_monitorItem->itemState( IState_c::ClaimedAddress ) ) {
    result.mc_isoName = result.mpc_monitorItem->isoName();
    return true;
  } else {
    result.mc_isoName.setUnspecified();
    return false;
  }
}


MessageState_t
CanPkgExt_c::resolveReceivingInformation( int ai_multitonInstance )
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
  MessageState_t messageStateSA = address2IdentRemoteSa( ai_multitonInstance );
  MessageState_t messageStateDA;

  // for receive, the local item is the addressee --> DA is interpreted
  if ( isoPf() >= 0xF0 )
  { // PDU2 format -> no destination address
    // isoName and monitoritem are unspecified
    // we have no explicit address, but PDU2 implies GLOBAL (0xFF)
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We have PDU2 format -> no destination address." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    mc_addrResolveResDA.mc_isoName.setUnspecified();
    mc_addrResolveResDA.mpc_monitorItem = NULL;

    messageStateDA = DaValid;
  }
  else
  { // for receive, the remote item is the sender --> SA is interpreted
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We have PDU1 format -> destination address." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    messageStateDA = address2IdentLocalDa( ai_multitonInstance );
  }
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Return value is " << (messageStateSA | messageStateDA) << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "(0) Valid, (1) OnlyNetworkMgmt, (3) Invalid. " << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  //only valid if both messageStates are valid
  return static_cast<MessageState_t>(messageStateSA | messageStateDA);
}


MessageState_t
CanPkgExt_c::address2IdentLocalDa( int ai_multitonInstance )
{
  //we are shure that we have PDU1 format and therefore we have a destination address
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Scope local(DA) with ps-field = " << int(mc_addrResolveResDA.getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  // now try to resolve the address
  const bool cb_addressBelongsToKnownItem = resolveAddress( mc_addrResolveResDA, ai_multitonInstance );

  if ( cb_addressBelongsToKnownItem )
  { // only problem might be: when we receive a message sent to a remote node
    if ( mc_addrResolveResDA.mpc_monitorItem->itemState(IState_c::Local) )
    { // everything is fine
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "We reached a VALID state. Either the target is known." << INTERNAL_DEBUG_DEVICE_ENDL;
        INTERNAL_DEBUG_DEVICE << "address =  " << int(mc_addrResolveResDA.getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return DaValid;
    }
    else
    { // this is ONLY interesting for BUS-SNOOPING classes like TcClient_c or handling
      // of Working-Set-Slaves which have to snoop messages to their Working-Set-Master
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "We reached an ONLYNETWORKMGTM state. Destination is a remote node." << INTERNAL_DEBUG_DEVICE_ENDL;
        INTERNAL_DEBUG_DEVICE << "address =  " << int(mc_addrResolveResDA.getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return static_cast<MessageState_t>(DaInvalidRemote | AdrInvalid);
    }
  }
  else if ( mc_addrResolveResDA.getAddress() == 0xFF )
  { // we received a broadcasted message
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached a VALID state. Target address is 0xFF (broadcast)." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return DaValid;
  }
  else
  { // the receiver is not known OR is 0xFE (which is not a valid receiver address) -> don't process this message
    IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::MonitorInvalidDa, ai_multitonInstance );
    if ( mc_addrResolveResDA.getAddress() == 0xFE ) {
      #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an INVALID state. Receiver is 0xFE which is NOT possible." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return static_cast<MessageState_t>(DaInvalidAnonymous | AdrInvalid);
    } else {
      #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an INVALID state. Receiver is " << int(mc_addrResolveResDA.getAddress()) << " which is NOT known." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return static_cast<MessageState_t>(DaInvalidUnknown | AdrInvalid);
    }
  }
}


MessageState_t
CanPkgExt_c::address2IdentRemoteSa( int ai_multitonInstance )
{
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Scope remote(SA) with sa-field = " << int(mc_addrResolveResSA.getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  // now try to resolve the address
  const bool cb_addressBelongsToKnownItem = resolveAddress( mc_addrResolveResSA, ai_multitonInstance );

  if ( cb_addressBelongsToKnownItem )
  { // only problem might be: when we receive a message with SA of a local ident
    if ( mc_addrResolveResSA.mpc_monitorItem->itemState(IState_c::Local) )
    {
      IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::MonitorSaRxConflict, ai_multitonInstance ); 
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Someone sends with one of our SA's." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return static_cast<MessageState_t>(SaInvalidLocal | AdrInvalid);
    }
    else
    { // everything is fine
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "We reached a valid state." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return SaValid;
    }
  }
  else if ( mc_addrResolveResSA.getAddress() == 0xFF )
  { // a SA with 0xFF is never allowed
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an INVALID state with address = 0xFF." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif

    IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::MonitorInvalidSa, ai_multitonInstance );
    return static_cast<MessageState_t>(SaInvalidGlobal | AdrInvalid);
  }
  else if ( mc_addrResolveResSA.getAddress() == 0xFE )
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
    return static_cast<MessageState_t>(SaInvalidUnknown | AdrInvalid);
  }
}


bool
CanPkgExt_c::resolveMonitorItem( AddressResolveResults_c& arc_addressResolveResults, int ai_multitonInstance  )
{
  //check if monitoritem exist
  if ( arc_addressResolveResults.mpc_monitorItem == NULL )
  {
    if( arc_addressResolveResults.mc_isoName.isUnspecified() )
        { // leave CAN-Identifier as is
      // nothing more to be done
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Leave CAN-Identifier as is. Nothing more to be done." << INTERNAL_DEBUG_DEVICE_ENDL;
        INTERNAL_DEBUG_DEVICE << "MonitorItem == NULL. ISOName unspecified." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return true;
    }
    else // ( mc_isoName.isSpecified() )
    { // get mpc_monitorItem if exist in systemmgmt_c
      if ( !getIsoMonitorInstance( ai_multitonInstance ).existIsoMemberISOName( arc_addressResolveResults.mc_isoName, false ) )
      {
        #if DEBUG_CAN
          INTERNAL_DEBUG_DEVICE << "ISOName specified and item does NOT exists in systemmgmt." << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        return false;
      }
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "ISOName specified and item exists in systemmgmt. Get monitoritem." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif

      arc_addressResolveResults.mpc_monitorItem = &getIsoMonitorInstance( ai_multitonInstance ).isoMemberISOName( arc_addressResolveResults.mc_isoName, false );
    }
  }
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "We have a Monitoritem." << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // now: verify if the retrieved item is already claimed or if we are preparing a network mgmt message send
  if ( arc_addressResolveResults.mpc_monitorItem->itemState(IState_c::ClaimedAddress) )
  { // if claimed address, sending is allowed under all conditions
    arc_addressResolveResults.setAddress(arc_addressResolveResults.mpc_monitorItem->nr());
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "ClaimedAddress state: " << arc_addressResolveResults.mpc_monitorItem->itemState(IState_c::ClaimedAddress) << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "AddressClaim state:   " << arc_addressResolveResults.mpc_monitorItem->itemState(IState_c::AddressClaim) << INTERNAL_DEBUG_DEVICE_ENDL;
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


bool
CanPkgExt_c::resolveSendingInformation( int ai_multitonInst )
{
  #if DEBUG_CAN
  INTERNAL_DEBUG_DEVICE << "*-*-*-* SEND MESSAGE *-*-*-*" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // handle SA
  if ( !resolveMonitorItem(mc_addrResolveResSA, ai_multitonInst ) )
  { // stop any further interpretation, as sending is not valid
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an INVALID state. SA could not be resolved." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return false;
  }
  else if ( mc_addrResolveResSA.mpc_monitorItem != NULL )
  { // resolving was performed
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "SA could be resolved and monitorItem != NULL." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    if ( !mc_addrResolveResSA.mpc_monitorItem->itemState(IState_c::Local ) )
    { // resolved MonitorItem is no local item -> no valid send possible
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Sending is not possible because item is not known local." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return false;
    }
  }
  #if DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Sending is possible. Item is known local." << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "SA = " << int(mc_addrResolveResSA.getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // set the SA in the IDENT
  // - when the SA has been directly set by call to setIsoSa(), the requested SA is already
  //   stored in mc_addrResolveResSA.address
  // ==> we can set least significant byte of the CAN ident in all cases from mc_addrResolveResSA.address
//  setIdent(*mc_addrResolveResSA.mpui8_address,0, Ident_c::ExtendedIdent);

  // handle DA for PF -> PDU1
  if ( isoPf() < 0xF0 )
  { // targeted message -> retrieve DA
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "PDU1 format -> existing DA." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    if ( !resolveMonitorItem(mc_addrResolveResDA, ai_multitonInst ) )
    { // stop any further interpretation, as sending is not valid
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Sending not valid. DA could not be resolved." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return false;
    }
    else if ( mc_addrResolveResDA.mpc_monitorItem != NULL )
    { // resolving was performed
      #if DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Sending valid. DA could be resolved. MonitorItem != NULL." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      if ( (mc_addrResolveResDA.mpc_monitorItem)->itemState(IState_c::Local ) )
      { // resolved MonitorItem is no remote item -> no valid send possible
        #if DEBUG_CAN
          INTERNAL_DEBUG_DEVICE << "Sending is not possible because item is known local." << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        return false;
      }
    }
    #if DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "Sending is possible. Item is known remote." << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "DA = " << int(mc_addrResolveResDA.getAddress()) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  }
  // set the PS in the IDENT
  // - when the PS has been directly set by call to setIsoPs()/setIsoPgn(), the requested PS is already
  //   stored in mc_addrResolveResDA.address
  // ==> we can set second least significant byte of the CAN ident in all cases from mc_addrResolveResDA.address
//  setIdent(*mc_addrResolveResDA.mpui8_address, 1, Ident_c::ExtendedIdent);
  #if DEBUG_CAN
  else
  {
    INTERNAL_DEBUG_DEVICE << "PDU2 format -> PS == GroupExtension." << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
  return true;
}


void
CanPkgExt_c::setIsoPs(uint8_t aui8_val)
{
  mc_addrResolveResDA.setAddress(aui8_val);
  mc_addrResolveResDA.mc_isoName.setUnspecified();
  mc_addrResolveResDA.mpc_monitorItem = NULL;
}


void
CanPkgExt_c::setIsoSa(uint8_t aui8_val)
{
  mc_addrResolveResSA.setAddress(aui8_val);
  mc_addrResolveResSA.mc_isoName.setUnspecified();
  mc_addrResolveResSA.mpc_monitorItem = NULL;
}


void
CanPkgExt_c::setMonitorItemForSA( IsoItem_c* apc_monitorItem )
{
  mc_addrResolveResSA.mpc_monitorItem = apc_monitorItem;
  // mc_isoName will not be needed -> set to unspecified
  mc_addrResolveResSA.mc_isoName.setUnspecified();
  mc_addrResolveResSA.setAddress(0xFF);
}


void
CanPkgExt_c::setISONameForSA( const IsoName_c& acrc_isoName )
{
  mc_addrResolveResSA.mc_isoName = acrc_isoName;
  // mpc_monitorItem will be set over mc_isoName -> reset mpc_monitorItem
  mc_addrResolveResSA.mpc_monitorItem = NULL;
  mc_addrResolveResSA.setAddress(0xFF);
}


void
CanPkgExt_c::setMonitorItemForDA( IsoItem_c* apc_monitorItem )
{
  mc_addrResolveResDA.mpc_monitorItem = apc_monitorItem;
  // mc_isoName will not be needed -> set to unspecified
  mc_addrResolveResDA.mc_isoName.setUnspecified();
  mc_addrResolveResDA.setAddress(0xFF);
}


void
CanPkgExt_c::setISONameForDA( const IsoName_c& acrc_isoName )
{
  mc_addrResolveResDA.mc_isoName = acrc_isoName;
  // mpc_monitorItem will be set over mc_isoName -> reset mpc_monitorItem
  mc_addrResolveResDA.mpc_monitorItem = NULL;
  mc_addrResolveResDA.setAddress(0xFF);
}


uint8_t
CanPkgExt_c::isoPs() const
{
  // destination address is valid
  return mc_addrResolveResDA.getAddress();
}


uint8_t
CanPkgExt_c::isoSa() const
{
  // source address is valid
  return mc_addrResolveResSA.getAddress();
}

} // end of namespace __IsoAgLib
