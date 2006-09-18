/***************************************************************************
                          canpkgext_c.cpp - header for extended CANPkg_c object,
                                           which calls data flag converting
                                           functions on assign operations
                             -------------------
    begin                : Tue Jan 11 2000
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#include <IsoAgLib/util/impl/devkey_c.h>
#include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>
#include "canpkgext_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

bool CANPkgExt_c::b_runFlag2String = true;

/** default constructor, which has nothing to do */
CANPkgExt_c::CANPkgExt_c( int ri_singletonVecKey )
  : CANPkg_c( ri_singletonVecKey )
{
  addrResolveResSA.p_devKey =  new DevKey_c(DevKey_c::DevKeyUnspecified);
  addrResolveResDA.p_devKey =  new DevKey_c(DevKey_c::DevKeyUnspecified);
  addrResolveResSA.pc_monitorItem = NULL;
  addrResolveResDA.pc_monitorItem = NULL;
  addrResolveResSA.pui8_address = &identRef(0);
  addrResolveResDA.pui8_address = &identRef(1);
}

/** virtual default destructor, which has nothing to do */
CANPkgExt_c::~CANPkgExt_c()
{
  delete addrResolveResSA.p_devKey;
  delete addrResolveResDA.p_devKey;
}

/**
  ==> OBSOLETE, because now all can-pkg-data is STATIC!
  ==> REACTIVATE if some NON-STATIC member vars will be added!
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  assign operator to insert informations from one CANPkg_c into another
  @see __IsoAgLib::FilterBox_c::operator>>
  @see CANPkgExt_c::operator=
  @see CANPkgExt_c::getData
  @param rrefc_right reference to the source CANPkg_c on the right
  @return reference to the source CANPkg_c to enable assign chains like
      "pkg1 = pkg2 = pkg3 = pkg4;"
const CANPkg_c& CANPkgExt_c::operator=(const CANPkg_c& rrefc_right)
{
  return CANPkg_c::operator =( rrefc_right );
}
*/


/**
  abstract function to transform the string data into flag values
  => derived class must implement suitable data conversion function

  needed for assigning informations from another CANPkg_c or CANPkgExt
  @see CANPkgExt_c::operator=
*/
void CANPkgExt_c::string2Flags()
{ // dummy body - normally NOT called
}

/**
  abstract transform flag values to data string
  => derived class must implement suitable data converting function

  needed for sending informations from this object via CANIO_c on CAN BUS,
  because CANIO_c doesn't know anything about the data format of this type of msg
  so that it can only use an unformated data string from CANPkg
  @see CANPkgExt_c::getData
  @see __IsoAgLib::CANIO_c::operator<<
*/
void CANPkgExt_c::flags2String()
{ // dummy body - normally NOT called
};

/**
  put data into given reference to BIOS related data structure with data, len
  @param reft_ident     reference where the ident is placed for send
  @param refui8_identType reference to the ident type val: 0==std, 1==ext
  @param refb_dlcTarget reference to the DLC field of the target
  @param pb_dataTarget pointer to the data string of the target
*/
void CANPkgExt_c::getData(uint32_t& reft_ident, uint8_t& refui8_identType,
                     uint8_t& refb_dlcTarget, uint8_t* pb_dataTarget)
{
  if ( ! b_runFlag2String )
  { // data is already setup -> don't call flags2String - but reset it again to true for the next message
    b_runFlag2String = true;
  }
  else
  {
    flags2String();
  }
  CANPkg_c::getData(reft_ident, refui8_identType, refb_dlcTarget, pb_dataTarget);
}


/**
  set the value of the ISO11783 ident field PGN
  @return parameter group number
*/
void CANPkgExt_c::setIsoPgn(uint32_t rui32_val)
{
  uint16_t ui16_val = rui32_val & 0x1FFFF;

  // the PGN parameter contains PF at the second lowest byte
  // ==> PF >= 0xF0 correlates to PGN >= 0xF000
  if ( ui16_val >= 0xF000 )
  { // broadcasted message --> set Byte2 (index 1) from the PGN --> it is used as GroupExtension (GE)
    setIsoPs( (ui16_val & 0xFF) );
    //setIdent( (ui16_val & 0xFF), 1, Ident_c::ExtendedIdent);
  }
  // ELSE: do NOT set Byte2 (index 1) here, as this might be already set with call to setIsoPs() !!!!

  setIdent( (ui16_val >> 8), 2, Ident_c::ExtendedIdent);
  ui16_val = (rui32_val >> 16) & 0x1;
  ui16_val |= (ident(3) & 0x1E);
  setIdent( uint8_t(ui16_val & 0xFF), 3, Ident_c::ExtendedIdent);
}

/** resolve a given address and set devKey and monitoritem if possible
    @param  addressResolveResults  address to resolve
    @return true -> address could be resolved
  */
bool resolveAddress( AddressResolveResults& addressResolveResults )
{
  if (   ( *addressResolveResults.pui8_address <= 0xFD )
      && ( getIsoMonitorInstance4Comm().existIsoMemberNr( *addressResolveResults.pui8_address ) )
     )
  { // there exists an item with the given address
    addressResolveResults.pc_monitorItem = &(getIsoMonitorInstance4Comm().isoMemberNr( *addressResolveResults.pui8_address ));

    if ( addressResolveResults.pc_monitorItem->itemState( IState_c::ClaimedAddress ) )
    { // the existing item has already claimed its address - is fully functional on the BUS
      *addressResolveResults.p_devKey = addressResolveResults.pc_monitorItem->devKey();

      #ifdef DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Member is known with given address." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return true;
    }
  }
  // when we reach this position, the address could not resolve to an already claimed item
  addressResolveResults.pc_monitorItem = NULL;
  addressResolveResults.p_devKey->setUnspecified();
  #ifdef DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Member is not known with given address. Set monitorItem and devKey to NULL/unspecified." << INTERNAL_DEBUG_DEVICE_ENDL;
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
MessageState_t CANPkgExt_c::resolveReceivingInformation()
{
  #ifdef DEBUG_CAN
  INTERNAL_DEBUG_DEVICE << "*-*-*-* PROCESS MESSAGE *-*-*-*" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

//  *addrResolveResSA.pui8_address = ident() & 0xFF;
//  *addrResolveResDA.pui8_address = (ident() >> 8) & 0xFF;

  // resolve source address
  // in context of receiving SA is remote
  MessageState_t messageStateSA = address2IdentRemoteSa();
  MessageState_t messageStateDA = Valid;

  // for receive, the local item is the addressee --> DA is interpreted
  if ( isoPf() >= 0xF0 )
  { // PDU2 format -> no destination address
    // devKey and monitoritem are unspecified
    // we have no explicit address, but PDU2 implies GLOBAL (0xFF)
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We have PDU2 format -> no destination address." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    addrResolveResDA.p_devKey->setUnspecified();
    addrResolveResDA.pc_monitorItem = NULL;
  }
  else
  { // for receive, the remote item is the sender --> SA is interpreted
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We have PDU1 format -> destination address." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    messageStateDA = address2IdentLocalDa();
  }
  #ifdef DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Return value is " << (messageStateSA | messageStateDA) << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "(0) Valid, (1) OnlyNetworkMgmt, (3) Invalid. " << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  //only valid if both messageStates are valid
  return static_cast<MessageState_t>(messageStateSA | messageStateDA);
}


/** report if the combination of address and scope is valid in context of message processing
    @return  true -> address, scope combination is valid
  */
MessageState_t CANPkgExt_c::address2IdentLocalDa()
{
  //we are shure that we have PDU1 format and therefore we have a destination address
  #ifdef DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Scope local(DA) with ps-field = " << int(*addrResolveResDA.pui8_address ) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  // now try to resolve the address
  const bool cb_addressBelongsToKnownItem = resolveAddress( addrResolveResDA );

  if ( ( cb_addressBelongsToKnownItem ) || ( *addrResolveResDA.pui8_address == 0xFF ) )
  { // fine - the target adr is known
    // OR
    // we received a broadcasted message
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached a VALID state. Either the target is known or address = 0xFF." << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "address =  " << int( *addrResolveResDA.pui8_address ) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return Valid;
  }
  else
  { // the receiver is not known -> don't process this message
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an INVALID state. Receiver not known." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return Invalid;
  }
}


/** report if the combination of address and scope is valid in context of message processing
    @return  true -> address, scope combination is valid
  */
MessageState_t CANPkgExt_c::address2IdentRemoteSa()
{
  #ifdef DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Scope remote(SA) with sa-field = " << int( *addrResolveResSA.pui8_address ) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  // now try to resolve the address
  const bool cb_addressBelongsToKnownItem = resolveAddress( addrResolveResSA );

  if ( cb_addressBelongsToKnownItem )
  { // only problem might be: when we receive a message with SA of a local ident
    if ( addrResolveResSA.pc_monitorItem->itemState(IState_c::Local) )
    {
      /** @todo monitorItem is const, we cannot inform the item of the conflict, but it will be done anyway from identItem
                addressResolveResults.pc_monitorItem->affectedConflictCnt( IStateExt_c::Incr, time() );
        */
      #ifdef DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Someone sends with one of our SA's." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return OnlyNetworkMgmt;
    }
    else
    { // everything is fine
      #ifdef DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "We reached a valid state." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return Valid;
    }
  }
  else if ( *addrResolveResSA.pui8_address == 0xFF )
  { // a SA with 0xFF is never allowed
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an INVALID state with address = 0xFF." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return Invalid; // mark as invalid
  }
  else if ( *addrResolveResSA.pui8_address == 0xFE )
  { // each RequestForXY message (not only ReqForAdrClaim) is allowed to be sent with SA == 0xFE
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached a VALID state with address = OxFE." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return Valid;
  }
  else
  { // normal address, which is not yet known to monitor lists (possible during first SA claim)
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an ONLYNETWORKMGTM state." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return OnlyNetworkMgmt;
  }
}


/** resolve a given monitoritem and get address if possible
    @param  addressResolveResults  address to resolve
    @return true -> monitoritem could be resolved
            false -> nothing more to be done
  */
bool CANPkgExt_c::resolveMonitorItem( AddressResolveResults& addressResolveResults )
{
  //check if monitoritem exist
  if ( addressResolveResults.pc_monitorItem == NULL )
  {
    if( addressResolveResults.p_devKey->isUnspecified() )
    { // leave CAN-Identifier as is
      // nothing more to be done
      #ifdef DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Leave CAN-Identifier as is. Nothing more to be done." << INTERNAL_DEBUG_DEVICE_ENDL;
        INTERNAL_DEBUG_DEVICE << "MonitorItem == NULL. DevKey unspecified." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return true;
    }
    else // ( p_devKey.isSpecified() )
    { // get pc_monitorItem if exist in systemmgmt_c
      if ( !getIsoMonitorInstance4Comm().existIsoMemberDevKey( *addressResolveResults.p_devKey, false ) )
      {
        #ifdef DEBUG_CAN
          INTERNAL_DEBUG_DEVICE << "DevKey specified and item does NOT exists in systemmgmt." << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        return false;
      }
      #ifdef DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "DevKey specified and item exists in systemmgmt. Get monitoritem." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif

      addressResolveResults.pc_monitorItem = &getIsoMonitorInstance4Comm().isoMemberDevKey( *addressResolveResults.p_devKey, false );
    }
  }
  #ifdef DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "We have a Monitoritem." << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // now: verify if the retrieved item is already claimed or if we are preparing a network mgmt message send
  if (   addressResolveResults.pc_monitorItem->itemState(IState_c::ClaimedAddress)
      || ( isNetworkMgmt() )
     )
  { // if claimed address, sending is allowed under all conditions
    // if address claim and isNetworkMgmt(), sending in AddressClaim state is allowed for network mgmt
    *addressResolveResults.pui8_address = addressResolveResults.pc_monitorItem->nr();
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "ClaimedAddress state: " << addressResolveResults.pc_monitorItem->itemState(IState_c::ClaimedAddress) << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "AddressClaim state:   " << addressResolveResults.pc_monitorItem->itemState(IState_c::AddressClaim) << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "isNetworkMgmt() =     " << isNetworkMgmt() << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "address =             " << int( *addressResolveResults.pui8_address ) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return true;
  }
  else
  { // sending is not valid
  #ifdef DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "ItemState is neither ClaimedAddress nor AddressClaim." << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
    return false;
  }
}

/** check if source and destination address are valid
    @see     CANPkgExt_c::operator<<
    @pre     we want to send a message
    @return  Valid -> both addresses are valid
             Invalid -> one or both addresses are invalid
             OnlyNetworkMgmt -> one or both addresses are only useable for network management
  */
bool CANPkgExt_c::resolveSendingInformation()
{
  #ifdef DEBUG_CAN
  INTERNAL_DEBUG_DEVICE << "*-*-*-* SEND MESSAGE *-*-*-*" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // handle SA
  if ( !resolveMonitorItem(addrResolveResSA) )
  { // stop any further interpretation, as sending is not valid
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "We reached an INVALID state. SA could not be resolved." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return false;
  }
  else if ( addrResolveResSA.pc_monitorItem != NULL )
  { // resolving was performed
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "SA could be resolved and monitorItem != NULL." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    if ( !addrResolveResSA.pc_monitorItem->itemState(IState_c::Local ) )
    { // resolved MonitorItem is no local item -> no valid send possible
      #ifdef DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Sending is not possible because item is not known local." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return false;
    }
  }
  #ifdef DEBUG_CAN
    INTERNAL_DEBUG_DEVICE << "Sending is possible. Item is known local." << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "SA = " << int( *addrResolveResSA.pui8_address ) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // set the SA in the IDENT
  // - when the SA has been directly set by call to setIsoSa(), the requested SA is already
  //   stored in addrResolveResSA.address
  // ==> we can set least significant byte of the CAN ident in all cases from addrResolveResSA.address
//  setIdent(*addrResolveResSA.pui8_address,0, Ident_c::ExtendedIdent);

  // handle DA for PF -> PDU1
  if ( isoPf() < 0xF0 )
  { // targeted message -> retrieve DA
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "PDU1 format -> existing DA." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    if ( !resolveMonitorItem(addrResolveResDA) )
    { // stop any further interpretation, as sending is not valid
      #ifdef DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Sending not valid. DA could not be resolved." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return false;
    }
    else if ( addrResolveResDA.pc_monitorItem != NULL )
    { // resolving was performed
      #ifdef DEBUG_CAN
        INTERNAL_DEBUG_DEVICE << "Sending valid. DA could be resolved. MonitorItem != NULL." << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      if ( addrResolveResDA.pc_monitorItem->itemState(IState_c::Local ) )
      { // resolved MonitorItem is no remote item -> no valid send possible
        #ifdef DEBUG_CAN
          INTERNAL_DEBUG_DEVICE << "Sending is not possible because item is known local." << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        return false;
      }
    }
    #ifdef DEBUG_CAN
      INTERNAL_DEBUG_DEVICE << "Sending is possible. Item is known remote." << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "DA = " << int( *addrResolveResDA.pui8_address ) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  }
  // set the PS in the IDENT
  // - when the PS has been directly set by call to setIsoPs()/setIsoPgn(), the requested PS is already
  //   stored in addrResolveResDA.address
  // ==> we can set second least significant byte of the CAN ident in all cases from addrResolveResDA.address
//  setIdent(*addrResolveResDA.pui8_address, 1, Ident_c::ExtendedIdent);
  #ifdef DEBUG_CAN
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
void CANPkgExt_c::setIsoPs(uint8_t rui8_val)
{
  *addrResolveResDA.pui8_address = rui8_val;
  addrResolveResDA.p_devKey->setUnspecified();
  addrResolveResDA.pc_monitorItem = NULL;
}


/**
    set the value of the ISO11783 ident field SA
    @return source adress
  */
void CANPkgExt_c::setIsoSa(uint8_t rui8_val)
{
  *addrResolveResSA.pui8_address = rui8_val;
  addrResolveResSA.p_devKey->setUnspecified();
  addrResolveResSA.pc_monitorItem = NULL;
}


/** set the structure for resolve results DA
    @param pc_monitorItem  needed monitoritem
  */
void CANPkgExt_c::setMonitorItemForSA( const ISOItem_c* pc_monitorItem )
{
  addrResolveResSA.pc_monitorItem = pc_monitorItem;
  // p_devKey will not be needed -> set to unspecified
  addrResolveResSA.p_devKey->setUnspecified();
  *addrResolveResSA.pui8_address = 0xFF;
}


/** set the devKey for resolve SA
    @param p_devKey  needed devKey
  */
void CANPkgExt_c::setDevKeyForSA( const DevKey_c& p_devKey )
{
  *addrResolveResSA.p_devKey = p_devKey;
  // pc_monitorItem will be set over p_devKey -> reset pc_monitorItem
  addrResolveResSA.pc_monitorItem = NULL;
  *addrResolveResSA.pui8_address = 0xFF;
}


/** set the structure for resolve results DA
    @param pc_monitorItem  needed monitoritem
  */
void CANPkgExt_c::setMonitorItemForDA( const ISOItem_c* pc_monitorItem )
{
  addrResolveResDA.pc_monitorItem = pc_monitorItem;
  // p_devKey will not be needed -> set to unspecified
  addrResolveResDA.p_devKey->setUnspecified();
  *addrResolveResDA.pui8_address = 0xFF;
}


/** set the devKey for resolve DA
    @param p_devKey  needed devKey
  */
void CANPkgExt_c::setDevKeyForDA( const DevKey_c& p_devKey )
{
  *addrResolveResDA.p_devKey = p_devKey;
  // pc_monitorItem will be set over p_devKey -> reset pc_monitorItem
  addrResolveResDA.pc_monitorItem = NULL;
  *addrResolveResDA.pui8_address = 0xFF;
}


/** check if an adddress could be resolved with monitorItem and devKey
    @param  addressResolveResults  address to resolve
*/
uint8_t checkMonitorItemDevKey( const AddressResolveResults& addressResolveResults )
{
  // check if monitoritem exist and if not resolve it with devKey
  if ( addressResolveResults.pc_monitorItem == NULL )
  { // get pc_monitorItem if exist in systemmgmt_c
    if (     addressResolveResults.p_devKey->isUnspecified()
        || !getIsoMonitorInstance4Comm().existIsoMemberDevKey( *addressResolveResults.p_devKey, false )
       )
    { // there exist no iso member with given devKey
      return *addressResolveResults.pui8_address;
    }
    return getIsoMonitorInstance4Comm().isoMemberDevKey( *addressResolveResults.p_devKey, false ).nr();
  }
  // know we can be shure that an monitorItem exists
  return addressResolveResults.pc_monitorItem->nr();
}


/**
  get the value of the ISO11783 ident field PS
  @return PDU Specific
*/
uint8_t CANPkgExt_c::isoPs() const
{
  // destination address is already valid
  if (*addrResolveResDA.pui8_address != 0xFF ) return *addrResolveResDA.pui8_address;

  // check if destination address is willingly 0xFF or if it can be resolved
  return checkMonitorItemDevKey( addrResolveResDA );
}


/**
  get the value of the ISO11783 ident field SA
  @return source adress
*/
uint8_t CANPkgExt_c::isoSa() const
{
  // source address is already valid
  if (*addrResolveResSA.pui8_address != 0xFF ) return *addrResolveResSA.pui8_address;

  // check if source address is willingly 0xFF or if it can be resolved
  return checkMonitorItemDevKey( addrResolveResSA );
}

} // end of namespace __IsoAgLib
