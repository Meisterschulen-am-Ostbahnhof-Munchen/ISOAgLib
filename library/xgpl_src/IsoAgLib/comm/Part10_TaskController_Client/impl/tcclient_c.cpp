/*
  tcclient_c.cpp: central managing instance for all Task Controller-Clients

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
#include "tcclient_c.h"

#include "serverinstance_c.h"

#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>
#include <IsoAgLib/util/iassert.h>

#include <algorithm>
#include <list>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif

namespace __IsoAgLib {

  TcClient_c&
  getTcClientInstance( unsigned instance ) {
    MACRO_MULTITON_GET_INSTANCE_BODY( TcClient_c, PRT_INSTANCE_CNT, instance );
  }

  TcClient_c::TcClient_c()
    :
#if defined(USE_DIRECT_PD_HANDLING)
      m_pdMessageHandler( NULL ),
#endif
      m_provider( NULL )
    , m_handler( *this )
    , m_customer( *this )
    , m_clientInfo()
    , m_peerConnections()
    , m_pdRemoteNodes()
  {
  }


  void
  TcClient_c::init() {
    isoaglib_assert ( !initialized() );

    __IsoAgLib::getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( m_handler );
#if defined(HAL_USE_SPECIFIC_FILTERS) && !defined(USE_DIRECT_PD_HANDLING)
    getIsoBusInstance4Comm().insertFilter( m_customer, IsoAgLib::iMaskFilter_c( ( 0x3FFFF00UL ), ( PROCESS_DATA_PGN | 0xFF ) << 8 ), 8 );
#else
    getIsoBusInstance4Comm().insertFilter( m_customer, IsoAgLib::iMaskFilter_c( ( 0x3FF0000UL ), PROCESS_DATA_PGN << 8 ), 8 );
#endif

    setInitialized();
  }


  void
  TcClient_c::close() {
    isoaglib_assert( initialized() );
    isoaglib_assert( m_clientInfo.empty() );
    isoaglib_assert( m_peerConnections.empty() );

    for (ItemToRemoteNodeMap_t::iterator iter = m_pdRemoteNodes.begin(); iter != m_pdRemoteNodes.end(); ++iter)
      delete *iter;
    m_pdRemoteNodes.clear();

#if defined(HAL_USE_SPECIFIC_FILTERS) && !defined(USE_DIRECT_PD_HANDLING)
    getIsoBusInstance4Comm().deleteFilter( m_customer, IsoAgLib::iMaskFilter_c( ( 0x3FFFF00UL ), ( PROCESS_DATA_PGN | 0xFF ) << 8 ) );
#else
    getIsoBusInstance4Comm().deleteFilter( m_customer, IsoAgLib::iMaskFilter_c( ( 0x3FF0000UL ), PROCESS_DATA_PGN << 8 ) );
#endif

    getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( m_handler );

    setClosed();
  }


  bool
  TcClient_c::registerClient( IdentItem_c& ident, const IsoAgLib::ProcData::ClientCapabilities_s &caps, TcClientConnection_c::StateHandler_c &hdl )
  {
    switch( caps.versionNr )
    {
    case 3:
    case 4:
      break; // supported versions
    default:
      return false; // unsupported versions
    }

    if( m_clientInfo.find( &ident ) != m_clientInfo.end() )
      return false;

    if( caps.hasTcBas )
      ident.getDiagnosticFunctionalities().addFunctionalitiesTaskControllerBasic(
        true, caps.versionNr, TaskControllerBasicOptionsBitMask_t() );

    if( caps.hasTcGeo )
      ident.getDiagnosticFunctionalities().addFunctionalitiesTaskControllerGeo(
        true, caps.versionNr, caps.numBin, TaskControllerGeoOptionsBitMask_t() );

    if( caps.hasTcSc )
      ident.getDiagnosticFunctionalities().addFunctionalitiesTaskControllerSectionControl(
        true, caps.versionNr, caps.numBoom, caps.numSection);

    ClientInfo_s newClientInfo;
    newClientInfo.capabilities = caps;
    newClientInfo.stateHandler = &hdl;

    m_clientInfo.insert( IdentToClientInfoMap_t::value_type( &ident, newClientInfo ) );
    return true;
  }


  bool
  TcClient_c::deregisterClient( IdentItem_c& ident )
  {
    IdentToClientInfoMap_t::iterator iter = m_clientInfo.find( &ident );
    if( iter == m_clientInfo.end() )
      return false;

    if( iter->second.capabilities.hasTcBas )
      iter->first->getDiagnosticFunctionalities().remFunctionalities( TaskControllerBasic );

    if( iter->second.capabilities.hasTcGeo )
      iter->first->getDiagnosticFunctionalities().remFunctionalities( TaskControllerGeo );

    if( iter->second.capabilities.hasTcSc )
      iter->first->getDiagnosticFunctionalities().remFunctionalities( TaskControllerSectionControl );

    for( int i=0; i<IsoAgLib::ProcData::ServerTypes; ++i )
      if( iter->second.m_serverConnections[ i ].connected() )
        iter->second.m_serverConnections[ i ].disconnect();

    m_clientInfo.erase( iter );
    return true;
  }


  TcClientConnection_c*
  TcClient_c::doConnect( const IdentItem_c& ident, const ServerInstance_c& server, DevicePool_c& pool )
  {
    IdentToClientInfoMap_t::iterator iter = m_clientInfo.find( const_cast<IdentItem_c *>(&ident) );
    if( iter == m_clientInfo.end() )
      return NULL;

    return( iter->second.m_serverConnections[ server.getEcuType() ].fullConnect( server, pool )
        ? &iter->second.m_serverConnections[ server.getEcuType() ]
        : NULL );
  }


  void
  TcClient_c::dontConnect( const IdentItem_c& ident, const ServerInstance_c& server )
  {
    IdentToClientInfoMap_t::iterator clientIter = m_clientInfo.find( const_cast<IdentItem_c *>(&ident) );
    if( clientIter == m_clientInfo.end() )
      return;

    TcClientConnection_c &connection = clientIter->second.m_serverConnections[ server.getEcuType() ];

    connection.disconnect();
    ServerInstance_c *nextServer = findNextServerOfSameType( server );
    if( nextServer )
      connection.preConnect( *(clientIter->first), *nextServer, *clientIter->second.stateHandler, clientIter->second.capabilities );
  }


  PdConnection_c*
  TcClient_c::connectPeer( const IdentItem_c& identItem, const IsoItem_c& pdItem, PdPool_c& pool )
  {
    switch( pdItem.isoName().getEcuType() )
    {
    case IsoName_c::ecuTypeTaskControl:
    case IsoName_c::ecuTypeDataLogger:
      isoaglib_assert( !"Peer-connect to a TC/DL attempted!" );
      return NULL;
    
    default:
      ;
    }

    for (STL_NAMESPACE::list<PdConnection_c*>::const_iterator connection = m_peerConnections.begin();
         connection != m_peerConnections.end(); ++connection)
    {
      if ((&(*connection)->getIdentItem() == &identItem) && ((*connection)->getRemoteItem() == &pdItem))
      {
        isoaglib_assert( !"Double connect between one IdentItem_c and pdServer/pdNode detected!" );
        return NULL;
      }
    }
  
    PdRemoteNode_c *remoteNode = findRemoteNode( pdItem );
    if( remoteNode == NULL )
    {
      remoteNode = new PdRemoteNode_c( pdItem, false );
      m_pdRemoteNodes.push_back( remoteNode );
    }

    isoaglib_assert( !remoteNode->isServer() );

    PdConnection_c* connection = new PdConnection_c( identItem, remoteNode, pool );
    m_peerConnections.push_back( connection );

    return connection;
  }


  PdConnection_c*
  TcClient_c::connectPeerBroadcast( const IdentItem_c& identItem, PdPool_c& pool )
  {
    for (STL_NAMESPACE::list<PdConnection_c*>::const_iterator connection = m_peerConnections.begin();
         connection != m_peerConnections.end(); ++connection)
    {
      if ((&(*connection)->getIdentItem() == &identItem) && ((*connection)->getRemoteItem() == NULL))
      {
        isoaglib_assert( !"Double connect between one IdentItem_c and pdServer/pdNode detected!" );
        return NULL;
      }
    }

    PdConnection_c* connection = new PdConnection_c( identItem, NULL, pool );
    m_peerConnections.push_back( connection );

    return connection;
  }


  void
  TcClient_c::disconnectPeer( const IdentItem_c& identItem )
  {
    for (STL_NAMESPACE::list<PdConnection_c*>::iterator connection = m_peerConnections.begin();
         connection != m_peerConnections.end(); /*++connection*/)
    {
      if (&(*connection)->getIdentItem() == &identItem)
      {
        delete *connection;
        connection = m_peerConnections.erase(connection);
      }
      else
        ++connection;
    }
  }

  void
  TcClient_c::disconnectPeer( const PdConnection_c& connection)
  {
    STL_NAMESPACE::list<PdConnection_c*>::iterator iter = STL_NAMESPACE::find(m_peerConnections.begin(),m_peerConnections.end(), &connection);
    isoaglib_assert(iter != m_peerConnections.end());
    delete (*iter);
    m_peerConnections.erase(iter);
  }


  void
  TcClient_c::processMsg( const CanPkg_c& data )
  {
    ProcessPkg_c pkg( data, getMultitonInst() );

    if( ! pkg.isValid() || ( pkg.getMonitorItemForSA() == NULL ) )
      return;

#if defined(USE_DIRECT_PD_HANDLING)
    if (m_pdMessageHandler)
        m_pdMessageHandler->_eventPdMessageReceived(
            *pkg.getMonitorItemForSA(),
            pkg.getMonitorItemForDA(),
            pkg.men_command,
            pkg.mui16_element,
            pkg.mui16_DDI,
            pkg.mi32_pdValue);
#endif

    PdRemoteNode_c *node = findRemoteNode( *pkg.getMonitorItemForSA() );
    if( node )
      node->processMsg( pkg );
  }


  void
  TcClient_c::reactOnIsoItemModification( ControlFunctionStateHandler_c::iIsoItemAction_e action, IsoItem_c const& isoItem )
  {
    if( isoItem.itemState( IState_c::Local ) )
      return;

    switch( action )
    {
    case ControlFunctionStateHandler_c::AddToMonitorList:
      switch( isoItem.isoName().getEcuType() )
      {
      case IsoName_c::ecuTypeTaskControl:
        addServer( isoItem, IsoAgLib::ProcData::ServerTypeTaskController );
        break;

      case IsoName_c::ecuTypeDataLogger:
        addServer( isoItem, IsoAgLib::ProcData::ServerTypeDataLogger );
        break;

      default:
        ;
      }
      break;

    case ControlFunctionStateHandler_c::RemoveFromMonitorList:
      removeRemotePd( isoItem );
      break;

    default:
      ;
    }
  }

  PdRemoteNode_c *
  TcClient_c::findRemoteNode( const IsoItem_c &item ) const
  {
    for( ItemToRemoteNodeMap_t::const_iterator iter = m_pdRemoteNodes.begin(); iter != m_pdRemoteNodes.end(); ++iter )
    {
      if( &(*iter)->getIsoItem() == &item )
        return *iter;
    }

    return NULL;
  }


  void
  TcClient_c::proprietaryServer( const IsoItem_c &isoItem, bool available )
  {
    if( available )
      addServer( isoItem, IsoAgLib::ProcData::ServerTypeProprietary );
    else
    {
      isoaglib_assert( findRemoteNode( isoItem ) );
      isoaglib_assert( findRemoteNode( isoItem )->isServer() );

      PdRemoteNode_c* pdRemoteNode = findRemoteNode( isoItem );
      if( pdRemoteNode && pdRemoteNode->isServer() )
      {
        removeRemotePd( isoItem );
      }
      // else: do not let other nodes get remove accidentally with this function!
    }
  }


  void
  TcClient_c::addServer( const IsoItem_c& isoItem, IsoAgLib::ProcData::ServerType_t type )
  {
    isoaglib_assert( !findRemoteNode( isoItem )  );
    isoaglib_assert( !isoItem.itemState( IState_c::Local ) );

    m_pdRemoteNodes.push_back( new ServerInstance_c( isoItem, type ) );
  }


  PdRemoteNode_c &
  TcClient_c::addConnectionToControlSource( PdConnection_c& connection, const IsoItem_c& isoItem )
  {
    PdRemoteNode_c* remoteNode = findRemoteNode( isoItem );

    if( remoteNode == NULL )
    {
      remoteNode = new PdRemoteNode_c( isoItem, false );
      m_pdRemoteNodes.push_back( remoteNode );
    }

    remoteNode->addConnection( connection );

    return *remoteNode;
  }


  void
  TcClient_c::removeConnectionFromControlSource( PdConnection_c& connection, const IsoItem_c& isoItem )
  {
    PdRemoteNode_c* remoteNode = findRemoteNode( isoItem );
    isoaglib_assert( remoteNode );

    remoteNode->removeConnection( connection );

    if( !remoteNode->hasConnections() )
    {
      for( ItemToRemoteNodeMap_t::iterator iter = m_pdRemoteNodes.begin(); iter != m_pdRemoteNodes.end(); ++iter )
      {
        if( &(*iter)->getIsoItem() == &isoItem )
        {
          m_pdRemoteNodes.erase( iter );
          break;
        }
      }

      delete remoteNode;
    }
  }


  void
  TcClient_c::removeRemotePd( const IsoItem_c& isoItem )
  {
    for( ItemToRemoteNodeMap_t::iterator i = m_pdRemoteNodes.begin(); i != m_pdRemoteNodes.end(); ++i )
    {
      if( &(*i)->getIsoItem() != &isoItem )
        continue;

      if( (*i)->isServer() )
      {
        ServerInstance_c* server = static_cast<ServerInstance_c *>( *i );
        if( server->isAlive() )
        {
          server->notifyOnDropOff(); // to avoid immediate reconnect in the "Online->Offline transition below!
          notifyServerStatusChange( *server, false );
        }
      }
      else
        notifyPeerAndControlSourceDestruction( **i );

      delete *i;
      m_pdRemoteNodes.erase( i );
      break;
    }
  }

#if 0
// currently not supported
  void
  TcClient_c::processChangeDesignator( const IdentItem_c& ident, uint16_t objID, const char* newDesig )
  {
    for( STL_NAMESPACE::list<PdConnection_c*>::const_iterator it = m_peerConnections.begin();
         it != m_peerConnections.end(); ++it )
    {
      if( ( &ident == &( ( *it )->getIdentItem() ) ) 
        && ( *it )->getRemoteNode()
        && ( *it )->getRemoteNode()->isServer() )
      {
        static_cast<TcClientConnection_c*>( *it )->sendCommandChangeDesignator( objID, newDesig, CNAMESPACE::strlen( newDesig ) );
      }
    }
  }
#endif

  void
  TcClient_c::notifyServerStatusChange( ServerInstance_c& server, bool new_status )
  {
    for( IdentToClientInfoMap_t::iterator clientIter = m_clientInfo.begin(); clientIter != m_clientInfo.end(); ++clientIter )
    {
      TcClientConnection_c &connection = clientIter->second.m_serverConnections[ server.getEcuType() ];
       
      if( new_status )
      {
        if( !connection.connected() )
          connection.preConnect( *(clientIter->first), server, *clientIter->second.stateHandler, clientIter->second.capabilities );
      }
      else
      {
        if( connection.connected() == &server )
          notifyConnectionToBeEnded( connection );
      }
    }
  }


  void
  TcClient_c::notifyConnectionToBeEnded( TcClientConnection_c& connection )
  {
    isoaglib_assert( connection.connected() );

    ServerInstance_c *thisServer = connection.connected();
    const IdentItem_c &ident = connection.getIdentItem();
    TcClientConnection_c::StateHandler_c &sh = *connection.getStateHandler();
    IsoAgLib::ProcData::ClientCapabilities_s caps = connection.getClientCapabilities();

    connection.disconnect();

    ServerInstance_c *nextServer = findNextServerOfSameType( *thisServer );
    if( nextServer )
      connection.preConnect( ident, *nextServer, sh, caps );
    else if( thisServer->isAlive() )        
      // enable reconnect for same server
      connection.preConnect( ident, *thisServer, sh, caps );
  }


  ServerInstance_c *
  TcClient_c::findNextServerOfSameType( const ServerInstance_c &thisServer ) const
  {
    bool takeNextMatch=false;

    ServerInstance_c* p_firstMatchedServerInList = NULL;
    
    for( ItemToRemoteNodeMap_t::const_iterator iter = m_pdRemoteNodes.begin(); iter != m_pdRemoteNodes.end(); ++iter )
    {
      if( (*iter)->isServer() )
      {
        ServerInstance_c &nextServer = static_cast<ServerInstance_c&>(**iter);
        if( !takeNextMatch && ( &nextServer == &thisServer ) )
        {
          takeNextMatch = true;
          continue;
        }

        if( ( nextServer.getEcuType() == thisServer.getEcuType() ) && nextServer.isAlive() )
        {
          if( NULL == p_firstMatchedServerInList )
            p_firstMatchedServerInList = static_cast<ServerInstance_c*>(*iter);
          
          if( takeNextMatch )
            return &nextServer;
        }
      }
    }

    return p_firstMatchedServerInList;
  }


  void
  TcClient_c::notifyPeerAndControlSourceDestruction( PdRemoteNode_c& pdRemoteNode )
  {
    // 1/2: TC/DL Connection - check if Control Source was used!
    for( IdentToClientInfoMap_t::iterator clientIter = m_clientInfo.begin(); clientIter != m_clientInfo.end(); ++clientIter )
    {
      for( int i=0; i<IsoAgLib::ProcData::ServerTypes; ++i )
      {
        TcClientConnection_c &connection = clientIter->second.m_serverConnections[ i ];
        connection.notifyOnPeerDropOff( pdRemoteNode );
      }
    }

    // 2/2: Peer Connections
    for (STL_NAMESPACE::list<PdConnection_c*>::iterator connection = m_peerConnections.begin();
        connection != m_peerConnections.end(); )
    {
      if ((*connection)->getRemoteNode() == &pdRemoteNode)
      {
        delete *connection;
        connection = m_peerConnections.erase( connection );
      }
      else
        ++connection;
    }
  }


#if defined(USE_DIRECT_PD_HANDLING)
  void
  TcClient_c::setPdMessageHandler( PdMessageHandler_c& hdl )
  {
    isoaglib_assert ( m_pdMessageHandler == NULL );

    m_pdMessageHandler = &hdl;
  }


  void
  TcClient_c::clearPdMessageHandler()
  {
    isoaglib_assert( m_pdMessageHandler != NULL );

    m_pdMessageHandler = NULL;
  }


  void TcClient_c::sendPdMessage( const IsoItem_c& sa_item, const IsoItem_c* da_item, IsoAgLib::ProcData::CommandType_t command, uint16_t element, uint16_t ddi, int32_t value )
  {
    isoaglib_assert(command < IsoAgLib::ProcData::CommandUndefined);

    ProcessPkg_c pkg( command, element, ddi, value );

    pkg.setMonitorItemForDA( const_cast<IsoItem_c*>( da_item ));
    pkg.setMonitorItemForSA( const_cast<IsoItem_c*>( &sa_item ));

    getIsoBusInstance4Comm() << pkg;
  }
#endif


} // __IsoAgLib
