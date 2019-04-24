/*
  pdconnection_c.cpp: base class for managing a PD-based connection

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "pdconnection_c.h"

#include <IsoAgLib/util/iassert.h>
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#ifdef HAL_USE_SPECIFIC_FILTERS
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#endif
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/pdremotenode_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepool_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measureprog_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {

  PdConnection_c::PdConnection_c()
    : m_identItem( NULL )
    , m_pdRemoteNode( NULL )
    , m_pool( NULL )
    , m_connectedPds()
    , m_nackHandler( NULL )
  {
  }

  
  PdConnection_c::PdConnection_c(
    const IdentItem_c& identItem,
    PdRemoteNode_c* pdRemoteNode,
    PdPool_c &pool )
    : m_identItem( NULL )
    , m_pdRemoteNode( NULL )
    , m_pool( NULL )
    , m_connectedPds()
    , m_nackHandler( NULL )
  {
    init( identItem, pdRemoteNode );
    start( pool );
  }


  PdConnection_c::~PdConnection_c()
  {
    stop();
    close();
  }

  void PdConnection_c::init( const IdentItem_c &identItem, PdRemoteNode_c *remoteNode )
  {
    m_identItem = &identItem;
    m_pdRemoteNode = remoteNode;

    if( m_pdRemoteNode )
    {
      m_pdRemoteNode->addConnection( *this );

#ifdef HAL_USE_SPECIFIC_FILTERS
      getIsoFilterManagerInstance4Comm().insertIsoFilter(
        IsoFilter_s(
          *this, IsoAgLib::iMaskFilter_c( 0x3FFFFFFUL, ( PROCESS_DATA_PGN << 8 ) ),
          &getIdentItem().isoName(), &m_pdRemoteNode->getIsoItem().isoName(), 8 ) );
#endif
    }
  }

  void PdConnection_c::start( PdPool_c &pool )
  {
    m_pool = &pool;

    createMeasureProgs();
  }


  void PdConnection_c::stop()
  {
    if( !m_pool )
      return;

    destroyMeasureProgs();

    m_pool = NULL;
  }


  void PdConnection_c::close()
  {
    if( !m_identItem )
      return;

    if( m_pdRemoteNode )
    {
#ifdef HAL_USE_SPECIFIC_FILTERS
      getIsoFilterManagerInstance4Comm().removeIsoFilter(
        IsoFilter_s(
          *this, IsoAgLib::iMaskFilter_c( 0x3FFFFFFUL, ( PROCESS_DATA_PGN << 8 ) ),
          &getIdentItem().isoName(), &m_pdRemoteNode->getIsoItem().isoName(), 8 ) );
#endif

      m_pdRemoteNode->removeConnection( *this );
    }

    m_identItem = NULL;
    m_pdRemoteNode = NULL;
  }

  void PdConnection_c::createMeasureProgs()
  {
    for( PdPool_c::PdBases_t::const_iterator i = m_pool->getPdList().begin(); i != m_pool->getPdList().end(); ++i )
    {
      PdBase_c* pd = ( *i );
      const uint32_t key = getMapKey( pd->DDI(), pd->element());
      isoaglib_assert( m_connectedPds.find( key ) == m_connectedPds.end() );

      ConnectedPd_c* cPd = &( pd->createConnectedPd( *this ) );
      m_connectedPds[ key ] = cPd;
    }
  }


  void PdConnection_c::destroyMeasureProgs()
  {
    for( ConnectedPdMap_t::iterator i = m_connectedPds.begin(); i != m_connectedPds.end(); ++i )
      delete i->second;
    
    m_connectedPds.clear();
  }


#ifdef HAL_USE_SPECIFIC_FILTERS
  void
  PdConnection_c::processMsg( const CanPkg_c& data )
  {
    // NOTE: Converting to ProcessPkg_c does resolving with CanPkgExt_c
    // which would be not necessary, because of the specific SA/DA filters!
    ProcessPkg_c pkg( data, getMultitonInst() );
    // only PROCESS_DATA_PGN with SA/DA from IsoFilterManager, no need to check anything!
    processProcMsg( pkg );
  }
#endif


  const IsoItem_c*
  PdConnection_c::getRemoteItem() const
  {
    return( m_pdRemoteNode )
      ? &( m_pdRemoteNode->getIsoItem() )
      : NULL;
  }
      

  void
  PdConnection_c::processProcMsg( const ProcessPkg_c& pkg )
  {
    if( pkg.getMonitorItemForDA() )
    {
      switch (pkg.men_command)
      {
        case IsoAgLib::ProcData::RequestValue:
          if( pkg.mui16_DDI == IsoAgLib::ProcData::DefaultDataLoggingDDI )
            processRequestDefaultDataLogging();
          else
            processRequestMsg( pkg );
          break;

        case IsoAgLib::ProcData::Value:
          processSetMsg( pkg );
          break;

        case IsoAgLib::ProcData::MeasurementTimeValueStart:
        case IsoAgLib::ProcData::MeasurementDistanceValueStart:
        case IsoAgLib::ProcData::MeasurementMinimumThresholdValueStart:
        case IsoAgLib::ProcData::MeasurementMaximumThresholdValueStart:
        case IsoAgLib::ProcData::MeasurementChangeThresholdValueStart:
          processMeasurementMsg( pkg );
          break;

        case IsoAgLib::ProcData::ProcessDataAcknowledge:
          processPdAck(pkg);
          break;

        case IsoAgLib::ProcData::TechnicalData:
        case IsoAgLib::ProcData::DeviceDescriptor:
        case IsoAgLib::ProcData::ControlAssignment:
        case IsoAgLib::ProcData::SetValueAndAcknowledge:
        case IsoAgLib::ProcData::ReservedB:
        case IsoAgLib::ProcData::ReservedC:
        case IsoAgLib::ProcData::TaskControllerStatus:
        case IsoAgLib::ProcData::ClientTaskOrNotApplicable:
          // not handled HERE when destination-specific
          break;
      }
    }
    else
    {
      switch (pkg.men_command)
      {
        case IsoAgLib::ProcData::ProcessDataAcknowledge:
          processPdAck( pkg );
          break;

        case IsoAgLib::ProcData::TechnicalData:
        case IsoAgLib::ProcData::DeviceDescriptor:
        case IsoAgLib::ProcData::RequestValue:
        case IsoAgLib::ProcData::MeasurementTimeValueStart:
        case IsoAgLib::ProcData::MeasurementDistanceValueStart:
        case IsoAgLib::ProcData::MeasurementMinimumThresholdValueStart:
        case IsoAgLib::ProcData::MeasurementMaximumThresholdValueStart:
        case IsoAgLib::ProcData::MeasurementChangeThresholdValueStart:
        case IsoAgLib::ProcData::ControlAssignment:
        case IsoAgLib::ProcData::SetValueAndAcknowledge:
        case IsoAgLib::ProcData::ReservedB:
        case IsoAgLib::ProcData::ReservedC:
        case IsoAgLib::ProcData::TaskControllerStatus:
        case IsoAgLib::ProcData::ClientTaskOrNotApplicable:
          // not handled when global
          break;

        case IsoAgLib::ProcData::Value:
          processSetMsg( pkg );
          break;
      }
    }
  }

  
  void
  PdConnection_c::sendNackNotFound( int16_t ddi, int16_t element, IsoAgLib::ProcData::CommandType_t cmdType, bool wasBroadcast ) const
  {
    // Note: element without DPD will not be processed properly.
    // Response will be NackInvalidElementNumber instead of NackDDINoSupportedByElement
    IsoAgLib::ProcData::AckResponse_t reason = IsoAgLib::ProcData::NackProcessDataCommandNotSupported;
    if( m_pool )
    {
      reason = IsoAgLib::ProcData::NackInvalidElementNumber;
      for( PdPool_c::PdBases_t::const_iterator i = m_pool->getPdList().begin(); i != m_pool->getPdList().end(); ++i )
      {
        if ( ( *i )->element() == element )
        {
          reason = IsoAgLib::ProcData::NackDDINotSupportedByElement;
          break;
        }
      }
    }

    sendPdAck( ddi, element, cmdType, reason, wasBroadcast );
  }



  void PdConnection_c::processRequestMsg( const ProcessPkg_c& data ) {
    const uint32_t key = getMapKey( data.mui16_DDI, data.mui16_element );
    ConnectedPdMap_t::iterator iter = m_connectedPds.find(key);

    const bool wasBroadcast = ( data.getMonitorItemForDA() == NULL );

    if ( iter != m_connectedPds.end())
      iter->second->handleRequest();
    else
      sendNackNotFound( data.mui16_DDI, data.mui16_element, IsoAgLib::ProcData::RequestValue, wasBroadcast );
  }


  void PdConnection_c::processSetMsg( const ProcessPkg_c& data )
  {
    const uint32_t key = getMapKey( data.mui16_DDI, data.mui16_element );
    ConnectedPdMap_t::iterator iter = m_connectedPds.find(key);

    const bool wasBroadcast = ( data.getMonitorItemForDA() == NULL );

    if( iter != m_connectedPds.end() )
      iter->second->handleIncoming( data.mi32_pdValue, wasBroadcast );
    else
      sendNackNotFound( data.mui16_DDI, data.mui16_element, IsoAgLib::ProcData::Value, wasBroadcast );
  }


  void PdConnection_c::processMeasurementMsg( const ProcessPkg_c& data ) {
    const uint32_t key = getMapKey( data.mui16_DDI, data.mui16_element );
    ConnectedPdMap_t::iterator iter = m_connectedPds.find(key);

    const bool wasBroadcast = ( data.getMonitorItemForDA() == NULL );

    if( iter != m_connectedPds.end() )
    {
      ConnectedPd_c &cPd = *( iter->second );

      // measurementCommand_t and CommandType_t are unified for all measurement types
      const bool measurementAccepted = cPd.startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t( data.men_command ), data.mi32_pdValue );
      if( measurementAccepted )
      {
        if (m_capsConnection.versionNr >= 4)
        {
          // TODO Version 4 Send out current value! to be clarified, within threshold it's already sent in "cPd.startMeasurement(..)" above.
          // ... can be sent out also if version <4, doesn't hurt and is currently done so already, partly (within threshold)
          sendPdAck(cPd.pdBase().DDI(), cPd.pdBase().element(), data.men_command, IsoAgLib::ProcData::AckNoErrors, wasBroadcast);
          // PdAck could be sent out also for version < 4, but not sure how the TCs would react on it, so better only do it version 4+.
        }
      }
      else
      {
        sendPdAck(cPd.pdBase().DDI(), cPd.pdBase().element(), data.men_command, IsoAgLib::ProcData::NackTriggerMethodNotSupported, wasBroadcast);
      }
    }
    else
      sendNackNotFound( data.mui16_DDI, data.mui16_element, data.men_command, wasBroadcast );
  }


  void
  PdConnection_c::sendProcMsg( IsoAgLib::ProcData::CommandType_t cmd, uint16_t ddi, uint16_t element, int32_t pdValue ) const
  {
    ProcessPkg_c pkg( cmd, element, ddi, pdValue );

    pkg.setMonitorItemForDA( const_cast<IsoItem_c*>( getRemoteItem() ) );
    pkg.setMonitorItemForSA( m_identItem->getIsoItem() );

    getIsoBusInstance4Comm() << pkg;
  }


  void
  PdConnection_c::sendProcMsgPeer( IsoAgLib::ProcData::CommandType_t cmd, uint16_t ddi, uint16_t element, int32_t pdValue, const IsoName_c &peer ) const
  {
    ProcessPkg_c pkg( cmd, element, ddi, pdValue );

    pkg.setISONameForDA( peer );
    pkg.setMonitorItemForSA( m_identItem->getIsoItem() );

    getIsoBusInstance4Comm() << pkg;
  }


  void
  PdConnection_c::sendPdAck( int16_t ddi, int16_t element, IsoAgLib::ProcData::CommandType_t pdCmd, IsoAgLib::ProcData::AckResponse_t errorcodes, bool wasBroadcast ) const
  {
    if (wasBroadcast)
      return;

    if (m_capsConnection.versionNr < 4)
      pdCmd = IsoAgLib::ProcData::ClientTaskOrNotApplicable; // NOT APPLICABLE

    sendProcMsg( IsoAgLib::ProcData::ProcessDataAcknowledge, ddi, element, int32_t( 0xfffff000UL | ( uint32_t( pdCmd ) << 8 ) | uint32_t( errorcodes ) ) );
  }

  void
  PdConnection_c::processPdAck(const ProcessPkg_c& pkg) const
  {
    const uint8_t errCodes = (pkg.mi32_pdValue & 0xFF);

    if( (errCodes != 0) && m_nackHandler )
    {
      m_nackHandler->handleNack(pkg.mui16_DDI, pkg.mui16_element, errCodes, (pkg.getMonitorItemForDA() != NULL), this);
    }
  }

};
