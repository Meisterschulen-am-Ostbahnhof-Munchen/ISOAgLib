/*
  vtclient_c.cpp: central VT Client management

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "vtclient_c.h"

#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtclientconnection_c.h>
#include <IsoAgLib/util/iassert.h>
#include "../ivtclientobjectpool_c.h"

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {

/** C-style function, to get access to the unique multiton instance */
VtClient_c &getVtClientInstance( uint8_t instance )
{ // if > 1 singleton instance is used, no static reference can be used
  MACRO_MULTITON_GET_INSTANCE_BODY(VtClient_c, PRT_INSTANCE_CNT, instance);
}



VtClient_c::VtClient_c()
  : ml_vtServerInst()
  , m_vtConnections()
  , mt_handler( *this )
  , mt_customer( *this )
{
}


void
VtClient_c::init()
{
  isoaglib_assert (!initialized());

  getIsoMonitorInstance4Comm().registerControlFunctionStateHandler(mt_handler);

  getIsoBusInstance4Comm().insertFilter( mt_customer, IsoAgLib::iMaskFilterType_c( 0x3FFFF00UL, LANGUAGE_PGN<<8, Ident_c::ExtendedIdent ), 8 );
  getIsoBusInstance4Comm().insertFilter( mt_customer, IsoAgLib::iMaskFilterType_c( 0x3FF0000UL, VT_TO_ECU_PGN << 8, Ident_c::ExtendedIdent ), 8 );
  getIsoBusInstance4Comm().insertFilter( mt_customer, IsoAgLib::iMaskFilterType_c( 0x3FF0000UL, ACKNOWLEDGEMENT_PGN << 8, Ident_c::ExtendedIdent ), 8 );
#ifdef USE_VTOBJECT_auxiliaryfunction2
  getIsoBusInstance4Comm().insertFilter( mt_customer, IsoAgLib::iMaskFilterType_c( 0x3FF0000UL, ECU_TO_VT_PGN << 8, Ident_c::ExtendedIdent ), 8 );
#endif

  setInitialized();
}


void
VtClient_c::close()
{
  isoaglib_assert (initialized());
  isoaglib_assert (getClientCount() == 0);

  while (!ml_vtServerInst.empty())
  {
    delete ml_vtServerInst.back();
    ml_vtServerInst.pop_back();
  }

  getIsoBusInstance4Comm().deleteFilter(mt_customer, IsoAgLib::iMaskFilterType_c( 0x3FFFF00UL, LANGUAGE_PGN << 8, Ident_c::ExtendedIdent ) );
  getIsoBusInstance4Comm().deleteFilter(mt_customer, IsoAgLib::iMaskFilterType_c( 0x3FF0000UL, VT_TO_ECU_PGN << 8, Ident_c::ExtendedIdent  ) );
  getIsoBusInstance4Comm().deleteFilter(mt_customer, IsoAgLib::iMaskFilterType_c( 0x3FF0000UL, ACKNOWLEDGEMENT_PGN << 8, Ident_c::ExtendedIdent ) );
#ifdef USE_VTOBJECT_auxiliaryfunction2
  getIsoBusInstance4Comm().deleteFilter(mt_customer, IsoAgLib::iMaskFilterType_c( 0x3FF0000UL, ECU_TO_VT_PGN << 8, Ident_c::ExtendedIdent ) );
#endif

  getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler(mt_handler);

  setClosed();
}


VtClientConnection_c*
VtClient_c::initAndRegisterObjectPool(
  IdentItem_c& arc_identItem,
  IsoAgLib::iVtClientObjectPool_c& arc_pool, 
  const char* apc_versionLabel, 
  IsoAgLib::iVtClientDataStorage_c& apc_claimDataStorage, 
  IsoAgLib::iVtClientObjectPool_c::RegisterPoolMode_en aen_mode)
{
  switch (aen_mode)
  {
    case IsoAgLib::iVtClientObjectPool_c::RegisterPoolMode_MasterToPrimaryVt:
    case IsoAgLib::iVtClientObjectPool_c::RegisterPoolMode_MasterToAnyVt:
    case IsoAgLib::iVtClientObjectPool_c::RegisterPoolMode_MasterToSpecificVt:
      if (!arc_identItem.isMaster())
      {
        /// IdentItem must be a Master
        return NULL;
      }
      break;  
    case IsoAgLib::iVtClientObjectPool_c::RegisterPoolMode_Slave:
      break;
  }

  return initAndRegisterObjectPoolCommon(arc_identItem, arc_pool, apc_versionLabel, apc_claimDataStorage, aen_mode);
}


VtClientConnection_c*
VtClient_c::initAndRegisterObjectPoolCommon (IdentItem_c& rc_identItem, IsoAgLib::iVtClientObjectPool_c& arc_pool, const char* apc_versionLabel, IsoAgLib::iVtClientDataStorage_c& apc_claimDataStorage, IsoAgLib::iVtClientObjectPool_c::RegisterPoolMode_en aen_mode)
{
  uint8_t ui8_index = 0;
  // add new instance of VtClientServerCommunication
  for (; ui8_index < m_vtConnections.size(); ui8_index++)
  {
    if (m_vtConnections[ui8_index] == NULL)
    { // found one emtpy entry
      break;
    }
    else
    {
      if (&m_vtConnections[ui8_index]->getIdentItem() == &rc_identItem)
      { // this IdentItem has already one pool registered - use multiple
        // IdentItems if you want to use multiple pools!
        return NULL;
      }
    }
  }
  // create new instance
  VtClientConnection_c* pc_vtCSC = new VtClientConnection_c(
    rc_identItem, *this, arc_pool, apc_versionLabel, apc_claimDataStorage, ui8_index, aen_mode );

  if (pc_vtCSC->men_objectPoolState == VtClientConnection_c::OPCannotBeUploaded) // meaning here is: OPCannotBeInitialized (due to versionLabel problems)
  { // most likely due to wrong version label
    /// Error already registered in the VtClientConnection_c(..) constructor!
    delete pc_vtCSC;
    return NULL;
  }

  // add new instance to vector
  if (ui8_index < m_vtConnections.size())
    m_vtConnections[ui8_index] = pc_vtCSC;
  else
    m_vtConnections.push_back(pc_vtCSC);

  return pc_vtCSC;
}


bool
VtClient_c::deregisterObjectPool (IdentItem_c& r_identItem)
{
  /* what states the IdentItem could have we have to interrupt???
  * - IState_c::ClaimedAddress -> that item is Active and Member on ISOBUS
  * - !UploadType::UploadIdle -> interrupt any upload
  */
  uint8_t ui8_index = 0;
  for (; ui8_index < m_vtConnections.size(); ui8_index++)
  {
    if (m_vtConnections[ui8_index])
    {
      if (&r_identItem == &m_vtConnections[ui8_index]->getIdentItem())
      {
        delete m_vtConnections[ui8_index];
        m_vtConnections[ui8_index] = NULL;
        break;
      }
    }
  }

  if (ui8_index == m_vtConnections.size())
    return false; // appropriate IdentItem could not be found, so nothing was deleted
  else
    return true; // IdentItem was found and deleted
}


VtServerInstance_c* VtClient_c::getFirstActiveVtServer( bool mustBePrimary ) const
{
  STL_NAMESPACE::vector<VtServerInstance_c*>::const_iterator lit_vtServerInst;
  if( mustBePrimary )
  {
    for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); lit_vtServerInst++)
      if ( (*lit_vtServerInst)->isVtActive() && (*lit_vtServerInst)->isPrimaryVt() )
        return (*lit_vtServerInst);
  }
  else
  {
    for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); lit_vtServerInst++)
      if ( (*lit_vtServerInst)->isVtActive() )
        return (*lit_vtServerInst);
  }
  return NULL;
}


VtServerInstance_c* VtClient_c::getPreferredVtServer(const IsoName_c& aref_prefferedVTIsoName) const
{
  STL_NAMESPACE::vector<VtServerInstance_c*>::const_iterator lit_vtServerInst;
  for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); lit_vtServerInst++)
  {
    if ((*lit_vtServerInst)->isVtActive() && ((*lit_vtServerInst)->getIsoName() == aref_prefferedVTIsoName))
      return (*lit_vtServerInst);
  }
  return NULL;
}


VtServerInstance_c* VtClient_c::getSpecificVtServer(const IsoAgLib::iVtClientObjectPool_c& arc_pool) const
{
  STL_NAMESPACE::vector<VtServerInstance_c*>::const_iterator lit_vtServerInst;
  for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); lit_vtServerInst++)
  {
    if ((*lit_vtServerInst)->isVtActive() && arc_pool.selectVtServer((*lit_vtServerInst)->getIsoName().toConstIisoName_c()))
      return (*lit_vtServerInst);
  }
  return NULL;
}


uint16_t
VtClient_c::getClientCount() const
{
  uint16_t ui16_count = 0;
  for (uint8_t ui8_index = 0; ui8_index < m_vtConnections.size(); ++ui8_index)
  {
    if (m_vtConnections[ui8_index])
      ++ui16_count;
  }
  return ui16_count;
}


void 
VtClient_c::processMsg( const CanPkg_c& arc_data )
{
  CanPkgExt_c c_data( arc_data, getMultitonInst() );
  if( ( ! c_data.isValid() ) || ( c_data.getMonitorItemForSA() == NULL ) )
    return;

  if( c_data.getMonitorItemForDA() != NULL ) {
    processMsgNonGlobal( c_data );
  } else {
    processMsgGlobal( c_data );
  }
}

void VtClient_c::processMsgNonGlobal( const CanPkgExt_c& arc_data ) {

  isoaglib_assert( ( arc_data.isoPurePgn() == VT_TO_ECU_PGN ) || ( arc_data.isoPurePgn() == ACKNOWLEDGEMENT_PGN ) );

  for ( STL_NAMESPACE::vector<VtClientConnection_c*>::iterator it = m_vtConnections.begin(); it != m_vtConnections.end(); ++it ) {
    if( (*it)->connectedToVtServer() &&
        ( arc_data.getMonitorItemForDA() == (*it)->getIdentItem().getIsoItem() ) &&
        ( arc_data.getMonitorItemForSA() == &(*it)->getVtServerInst().getIsoItem() ) )
      switch( arc_data.isoPurePgn() ) {
        case ACKNOWLEDGEMENT_PGN:
          (*it)->processMsgAck( arc_data );
          break;
        case VT_TO_ECU_PGN:
          (*it)->processMsgVtToEcu( arc_data );
          break;
      }
  }
}


void VtClient_c::processMsgGlobal( const CanPkgExt_c& arc_data ) {
  // VT_TO_GLOBAL is the only PGN we accept without VT being active, because it marks the VT active!!
  STL_NAMESPACE::vector<VtServerInstance_c*>::iterator lit_vtServerInst;
  uint8_t ui8_index;

  /// -->VT_TO_GLOBAL_PGN<-- ///
  if (arc_data.isoPgn() == VT_TO_GLOBAL_PGN)
  { // iterate through all registered VtServerInstances and process msg if vtSourceAddress == isoSa
    uint8_t const cui8_cmdByte = arc_data.getUint8Data (1-1);

    if (cui8_cmdByte == 0xFE) // Command: "Status", Parameter: "VT Status Message"
    {
      for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); lit_vtServerInst++)
      {
        if (&(*lit_vtServerInst)->getIsoItem() == arc_data.getMonitorItemForSA())
        {
          (*lit_vtServerInst)->setLatestVtStatusData( arc_data );

          // iterate through all registered VtClientServerCommunication and notify their pools with "eventVtStatusMsg"
          for (ui8_index = 0; ui8_index < m_vtConnections.size(); ui8_index++)
          {
            if (m_vtConnections[ui8_index])
            {
              if (m_vtConnections[ui8_index]->getVtServerInstPtr() == (*lit_vtServerInst))
              { // this vtClientServerComm is connected to this VT, so notify the objectpool!!
                m_vtConnections[ui8_index]->notifyOnVtStatusMessage();
              }
            }
          }
          return;
        }
      }
    }
    else if (cui8_cmdByte == 0x21) // Command: "Auxiliary Control", Parameter: "Auxiliary Input Status"
    { // iterate through all registered VtClientServerCommunication and notify them, maybe they have functions that need that input status!
      for (ui8_index = 0; ui8_index < m_vtConnections.size(); ui8_index++)
      {
        if (m_vtConnections[ui8_index])
        {
          m_vtConnections[ui8_index]->notifyOnAuxInputStatus( arc_data );
        }
      }
    }
    else if (cui8_cmdByte == 0x26) // Command: "Auxiliary Control Type 2", Parameter: "Auxiliary Input Status"
    { 
      // iterate through all registered VtClientServerCommunication and notify them, maybe they have functions that need that input status!
      notifyAllConnectionsOnAux2InputStatus( arc_data );

    }
    return;
  }

  if ((arc_data.isoPgn() & 0x3FFFFLU) == ECU_TO_GLOBAL_PGN)
  {
    uint8_t const cui8_cmdByte = arc_data.getUint8Data (1-1);
    if (cui8_cmdByte == 0x23) // Command: "Auxiliary Control", Parameter: "input maintenance message"
    { // iterate through all registered VtClientServerCommunication and notify them
      notifyAllConnectionsOnAux2InputMaintenance( arc_data );
    }
  }


  /// -->LANGUAGE_PGN<-- ///
  if (arc_data.isoPgn() == LANGUAGE_PGN)
  {
    VtServerInstance_c* pc_server = NULL;
    // first process LANGUAGE_PGN for all VtServerInstances BEFORE processing for the VtClientServerCommunications
    for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); lit_vtServerInst++)
    {
      if (&(*lit_vtServerInst)->getIsoItem() == arc_data.getMonitorItemForSA())
      {
        pc_server = *lit_vtServerInst;
        break;
      }
    }

    if (pc_server != NULL)
    {
      pc_server->setLocalSettings( arc_data );

      // notify all connected vtCSCs
      for (ui8_index = 0; ui8_index < m_vtConnections.size(); ui8_index++)
      {
        if ( m_vtConnections[ui8_index] &&
            (m_vtConnections[ui8_index]->getVtServerInstPtr() == pc_server) )
          m_vtConnections[ui8_index]->notifyOnVtsLanguagePgn();
      }
    }
    // else: Language PGN from non-VtServerInstance - ignore
  }
}


bool
VtClient_c::sendCommandForDEBUG(IsoAgLib::iIdentItem_c& mrc_wsMasterIdentItem, uint8_t* apui8_buffer, uint32_t ui32_size)
{
  for (uint8_t ui8_index = 0; ui8_index < m_vtConnections.size(); ui8_index++)
  {
    if (&static_cast<__IsoAgLib::IdentItem_c&>(mrc_wsMasterIdentItem) == &m_vtConnections[ui8_index]->getIdentItem())
      return m_vtConnections[ui8_index]->sendCommand(apui8_buffer, ui32_size);
  }
  return false;
}


void
VtClient_c::reactOnIsoItemModification(
  ControlFunctionStateHandler_c::iIsoItemAction_e action,
  IsoItem_c const& isoItem )
{
  // we only care for the VTs
  if( isoItem.isoName().getEcuType() != IsoName_c::ecuTypeVirtualTerminal )
    return;

  switch( action )
  {
    case ControlFunctionStateHandler_c::AddToMonitorList:
      { ///! Attention: This function is also called from "init()", not only from ISOMonitor!
        for( STL_NAMESPACE::vector<VtServerInstance_c*>::iterator iter = ml_vtServerInst.begin();
             iter != ml_vtServerInst.end(); ++iter )
        { // check if newly added VtServerInstance is already in our list
          if (&isoItem == &(*iter)->getIsoItem())
            return;
        }

        ml_vtServerInst.push_back(
          new VtServerInstance_c( isoItem, *this ) );
      } break;

    case ControlFunctionStateHandler_c::RemoveFromMonitorList:
      for( STL_NAMESPACE::vector<VtServerInstance_c*>::iterator iter = ml_vtServerInst.begin();
           iter != ml_vtServerInst.end(); ++iter )
      { // check if lost VtServerInstance is in our list
        if (&isoItem == &(*iter)->getIsoItem())
        { // the VtServerInstance is already known and in our list,
          // delete it and notify all clients on early loss of that VtServerInstance
          for (unsigned index = 0; index < m_vtConnections.size(); ++index)
          {
            if( m_vtConnections[ index ] )
              m_vtConnections[ index ]->notifyOnVtServerInstanceLoss( *(*iter) );
          }

          delete *iter;
          ( void )ml_vtServerInst.erase( iter );
          break;
        }
      }
      break;

    default:
      break;
  }
}


void
VtClient_c::notifyAllConnectionsOnAux2InputStatus( const CanPkgExt_c& refc_data ) const
{
  for( unsigned index = 0; index < m_vtConnections.size(); ++index )
  {
    if( m_vtConnections[ index ] )
      m_vtConnections[ index ]->notifyOnAux2InputStatus( refc_data );
  }
}

void
VtClient_c::notifyAllConnectionsOnAux2InputMaintenance( const CanPkgExt_c& refc_data ) const
{
  for( unsigned index = 0; index < m_vtConnections.size(); ++index )
  {
    if( m_vtConnections[ index ] )
      m_vtConnections[ index ]->notifyOnAux2InputMaintenance( refc_data );
  }
}


#ifdef USE_IOP_GENERATOR_FAKE_VT_PROPERTIES
void
VtClient_c::fakeVtProperties (uint16_t aui16_dimension, uint16_t aui16_skWidth, uint16_t aui16_skHeight, uint8_t aui16_colorDepth, uint16_t aui16_fontSizes)
{
  const IsoItem_c c_dummyIsoItem;
  // casting NULL to a reference is okay here, as the reference isn't used for any FAKE_VT case (iop_generator, etc.)
  ml_vtServerInst.push_back (VtServerInstance_c (c_dummyIsoItem, IsoName_c::IsoNameUnspecified(), (*this) MULTITON_INST_WITH_COMMA));
  VtServerInstance_c& r_vtServerInst = ml_vtServerInst.back();
  r_vtServerInst.fakeVtProperties (aui16_dimension, aui16_skWidth, aui16_skHeight, aui16_colorDepth, aui16_fontSizes);
    // ... and notify all vtClientServerComm instances
  for (uint8_t ui8_index = 0; ui8_index < m_vtConnections.size(); ui8_index++)
  {
    if (m_vtConnections[ui8_index])
      m_vtConnections[ui8_index]->notifyOnNewVtServerInstance(r_vtServerInst);
  }
}
#endif

} // __IsoAgLib
