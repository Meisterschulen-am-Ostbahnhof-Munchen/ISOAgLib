/*
  pdconnection_c.cpp: base class for managing a PD-based connection

  (C) Copyright 2009 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
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
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclient_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/pdremotenode_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepool_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measureprog_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {


  PdConnection_c::PdConnection_c(
    const IdentItem_c& identItem,
    PdRemoteNode_c* pdRemoteNode,
    const PdPool_c &pool )
    : m_identItem( identItem )
    , m_pdRemoteNode( pdRemoteNode )
    , m_pool( pool )
    , m_connectedPds()
  {
    createMeasureProgs();

    if( m_pdRemoteNode )
    {
      m_pdRemoteNode->addConnection( *this );

#ifdef HAL_USE_SPECIFIC_FILTERS
      getIsoFilterManagerInstance4Comm().insertIsoFilter(
        IsoFilter_s(
          *this, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, ( PROCESS_DATA_PGN << 8 ) ),
          &getIdentItem().isoName(), &m_pdRemoteNode->getIsoItem().isoName(), 8 ) );
#endif
    }
  }


  PdConnection_c::~PdConnection_c()
  {
    if( m_pdRemoteNode )
    {
#ifdef HAL_USE_SPECIFIC_FILTERS
      getIsoFilterManagerInstance4Comm().removeIsoFilter(
        IsoFilter_s(
          *this, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, ( PROCESS_DATA_PGN << 8 ) ),
          &getIdentItem().isoName(), &m_pdRemoteNode->getIsoItem().isoName(), 8 ) );
#endif

      m_pdRemoteNode->removeConnection( *this );
    }

    destroyMeasureProgs();
  }


  void PdConnection_c::createMeasureProgs()
  {
    for( PdPool_c::PdList_t::const_iterator i = m_pool.getPdList().begin(); i != m_pool.getPdList().end(); ++i )
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

    // @todo to be removed later on, this was just the quick-hack!! Also remove from TcClient_c
    // also remove #include above then!
    getTcClientInstance4Comm().receivePdMessage(
            *pkg.getMonitorItemForSA(),
            pkg.getMonitorItemForDA(),
            pkg.men_command,
            pkg.mui16_element,
            pkg.mui16_DDI,      
            pkg.mi32_pdValue);
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
        case IsoAgLib::ProcData::requestConfiguration:
        case IsoAgLib::ProcData::configurationResponse:
        case IsoAgLib::ProcData::nack:
          processMsgTc( pkg );
          break;

        case IsoAgLib::ProcData::requestValue:
          if( pkg.mui16_DDI == IsoAgLib::ProcData::DefaultDataLoggingDDI )
            processRequestDefaultDataLogging();
          else
            processRequestMsg( pkg );
          break;

        case IsoAgLib::ProcData::setValue:
          processSetMsg( pkg );
          break;

        case IsoAgLib::ProcData::measurementTimeValueStart:
        case IsoAgLib::ProcData::measurementDistanceValueStart:
        case IsoAgLib::ProcData::measurementMinimumThresholdValueStart:
        case IsoAgLib::ProcData::measurementMaximumThresholdValueStart:
        case IsoAgLib::ProcData::measurementChangeThresholdValueStart:
          processMeasurementMsg( pkg );
          break;

        case IsoAgLib::ProcData::commandReserved1:
        case IsoAgLib::ProcData::commandReserved2:
        case IsoAgLib::ProcData::commandReserved3:
        case IsoAgLib::ProcData::commandReserved4:
        case IsoAgLib::ProcData::taskControllerStatus:
        case IsoAgLib::ProcData::workingsetMasterMaintenance:
        case IsoAgLib::ProcData::CommandUndefined:
          // not handled when destination-specific
          break;
      }
    }
    else
    {
      switch (pkg.men_command)
      {
        case IsoAgLib::ProcData::requestConfiguration:
        case IsoAgLib::ProcData::configurationResponse:
        case IsoAgLib::ProcData::nack:
        case IsoAgLib::ProcData::requestValue:
        case IsoAgLib::ProcData::measurementTimeValueStart:
        case IsoAgLib::ProcData::measurementDistanceValueStart:
        case IsoAgLib::ProcData::measurementMinimumThresholdValueStart:
        case IsoAgLib::ProcData::measurementMaximumThresholdValueStart:
        case IsoAgLib::ProcData::measurementChangeThresholdValueStart:
        case IsoAgLib::ProcData::commandReserved1:
        case IsoAgLib::ProcData::commandReserved2:
        case IsoAgLib::ProcData::commandReserved3:
        case IsoAgLib::ProcData::commandReserved4:
        case IsoAgLib::ProcData::taskControllerStatus:
        case IsoAgLib::ProcData::workingsetMasterMaintenance:
        case IsoAgLib::ProcData::CommandUndefined:
          // not handled when global
          break;

        case IsoAgLib::ProcData::setValue:
          processSetMsg( pkg );
          break;
      }
    }
  }

  
  void
  PdConnection_c::sendNackNotFound( int16_t ddi, int16_t element ) const
  {
    // Note: element without DPD will not be processed properly.
    // Response will be NackInvalidElementNumber instead of NackDDINoSupportedByElement
    NackResponse_t reason = NackInvalidElementNumber;
    for( PdPool_c::PdList_t::const_iterator i = m_pool.getPdList().begin(); i != m_pool.getPdList().end(); ++i )
    {
      if ( ( *i )->element() == element )
      {
        reason = NackDDINotSupportedByElement;
        break;
      }
    }

    sendNack( ddi, element, reason );
  }



  void PdConnection_c::processRequestMsg( const ProcessPkg_c& data ) {
    const uint32_t key = getMapKey( data.mui16_DDI, data.mui16_element );
    ConnectedPdMap_t::iterator iter = m_connectedPds.find(key);

    if ( iter != m_connectedPds.end())
      iter->second->handleRequest();
    else
      sendNackNotFound( data.mui16_DDI, data.mui16_element );
  }


  void PdConnection_c::processSetMsg( const ProcessPkg_c& data )
  {
    const uint32_t key = getMapKey( data.mui16_DDI, data.mui16_element );
    ConnectedPdMap_t::iterator iter = m_connectedPds.find(key);

    if( iter != m_connectedPds.end() )
      iter->second->handleIncoming( data.mi32_pdValue );
    else
      sendNackNotFound( data.mui16_DDI, data.mui16_element );
  }


  void PdConnection_c::processMeasurementMsg( const ProcessPkg_c& data ) {
    const uint32_t key = getMapKey( data.mui16_DDI, data.mui16_element );
    ConnectedPdMap_t::iterator iter = m_connectedPds.find(key);

    if( iter != m_connectedPds.end() )
    {
      switch ( data.men_command ) {
        case IsoAgLib::ProcData::measurementDistanceValueStart:
        case IsoAgLib::ProcData::measurementTimeValueStart:
        case IsoAgLib::ProcData::measurementChangeThresholdValueStart:
        case IsoAgLib::ProcData::measurementMinimumThresholdValueStart:
        case IsoAgLib::ProcData::measurementMaximumThresholdValueStart:
          {
            ConnectedPd_c &cPd = *( iter->second );

            // measurementCommand_t and CommandType_t are unified for all measurement types
            if( !cPd.startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t( data.men_command ), data.mi32_pdValue ) )
              sendNack( cPd.pdBase().DDI(), cPd.pdBase().element(), NackTriggerMethodNotSupported );
          }
          break;

        default:
          isoaglib_assert( !"Method shall not be called for this Process command" );
          break;
      }
    }
    else
      sendNackNotFound( data.mui16_DDI, data.mui16_element );
  }


  void
  PdConnection_c::sendProcMsg( IsoAgLib::ProcData::CommandType_t cmd, uint16_t ddi, uint16_t element, int32_t pdValue ) const
  {
    ProcessPkg_c pkg( cmd, element, ddi, pdValue );

    pkg.setMonitorItemForDA( const_cast<IsoItem_c*>( getRemoteItem() ) );
    pkg.setMonitorItemForSA( m_identItem.getIsoItem() );

    getIsoBusInstance4Comm() << pkg;
  }


  void
  PdConnection_c::sendNack( int16_t ddi, int16_t element, NackResponse_t errorcodes ) const
  {
    isoaglib_assert( errorcodes != NackReserved1 );
    isoaglib_assert( errorcodes != NackReserved2 );
    isoaglib_assert( errorcodes != NackUndefined );

    sendProcMsg( IsoAgLib::ProcData::nack, ddi, element, int32_t( 0xffffff00UL | uint32_t( errorcodes ) ) );
  }
  

};
