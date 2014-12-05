/*
  tcclient_c.cpp: central managing instance for all Task Controller-Clients

  (C) Copyright 2009 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "tcclient_c.h"

#include "serverinstance_c.h"

#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include <IsoAgLib/util/iassert.h>

#include <algorithm>
#include <list>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif

namespace __IsoAgLib {

  TcClient_c&
  getTcClientInstance( uint8_t instance ) {
    MACRO_MULTITON_GET_INSTANCE_BODY( TcClient_c, PRT_INSTANCE_CNT, instance );
  }

  TcClient_c::TcClient_c()
    : m_handler( *this )
    , m_customer( *this )
    , m_stateHandler( NULL )
    , m_pdMessageHandler( NULL )
    , m_pdRemoteNodes()
    , m_connections()
  {
  }

        
  void
  TcClient_c::init() {
    isoaglib_assert ( !initialized() );

    __IsoAgLib::getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( m_handler );
#ifdef HAL_USE_SPECIFIC_FILTERS
    getIsoBusInstance4Comm().insertFilter( m_customer, IsoAgLib::iMaskFilter_c( ( 0x3FFFF00UL ), ( PROCESS_DATA_PGN | 0xFF ) << 8 ), 8 );
#else
    getIsoBusInstance4Comm().insertFilter( m_customer, IsoAgLib::iMaskFilter_c( ( 0x3FF0000UL ), PROCESS_DATA_PGN << 8 ), 8 );
#endif

    setInitialized();
  }


  void
  TcClient_c::close() {
    isoaglib_assert( initialized() );
    isoaglib_assert( m_connections.empty() );

    for (ItemToRemoteNodeMap_t::iterator iter = m_pdRemoteNodes.begin(); iter != m_pdRemoteNodes.end(); ++iter)
      delete iter->second;
    m_pdRemoteNodes.clear();

#ifdef HAL_USE_SPECIFIC_FILTERS
    getIsoBusInstance4Comm().deleteFilter( m_customer, IsoAgLib::iMaskFilter_c( ( 0x3FFFF00UL ), ( PROCESS_DATA_PGN | 0xFF ) << 8 ) );
#else
    getIsoBusInstance4Comm().deleteFilter( m_customer, IsoAgLib::iMaskFilter_c( ( 0x3FF0000UL ), PROCESS_DATA_PGN << 8 ) );
#endif

    getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( m_handler );

    setClosed();
  }


  void
  TcClient_c::setServerStateHandler( ServerStateHandler_c& hdl ) {
    isoaglib_assert ( m_stateHandler == NULL );

    m_stateHandler = &hdl;
  }


  void
  TcClient_c::clearServerStateHandler()
  {
    isoaglib_assert( m_stateHandler != NULL );

    m_stateHandler = NULL;
  }


  void
  TcClient_c::setPdMessageHandler( PdMessageHandler_c& hdl ) {
    isoaglib_assert ( m_pdMessageHandler == NULL );

    m_pdMessageHandler = &hdl;
  }


  void
  TcClient_c::clearPdMessageHandler()
  {
    isoaglib_assert( m_pdMessageHandler != NULL );

    m_pdMessageHandler = NULL;
  }


  TcClientConnection_c*
  TcClient_c::connect( const IdentItem_c& identItem, TcClientConnection_c::StateHandler_c& sh, const IsoItem_c& tcDlItem, const DevicePool_c& pool )
  {
    for (STL_NAMESPACE::list<PdConnection_c*>::const_iterator connection = m_connections.begin();
         connection != m_connections.end(); ++connection)
    {
      if ((&(*connection)->getIdentItem() == &identItem) && ((*connection)->getRemoteItem() == &tcDlItem))
      {
        isoaglib_assert( !"Double connect between one IdentItem_c and server detected!" );
        return NULL;
      }
    }
  
    ItemToRemoteNodeMap_t::iterator iter = m_pdRemoteNodes.find( &tcDlItem );
    isoaglib_assert( iter != m_pdRemoteNodes.end() );
    isoaglib_assert( iter->second->isServer() );

    TcClientConnection_c* connection = new TcClientConnection_c( identItem, sh, static_cast<ServerInstance_c&>(*(iter->second)), pool );
    m_connections.push_back( connection );

    return connection;
  }


  PdConnection_c*
  TcClient_c::connect( const IdentItem_c& identItem, const IsoItem_c& pdItem, const PdPool_c& pool )
  {
    for (STL_NAMESPACE::list<PdConnection_c*>::const_iterator connection = m_connections.begin();
         connection != m_connections.end(); ++connection)
    {
      if ((&(*connection)->getIdentItem() == &identItem) && ((*connection)->getRemoteItem() == &pdItem))
      {
        isoaglib_assert( !"Double connect between one IdentItem_c and pdServer/pdNode detected!" );
        return NULL;
      }
    }
  
    PdRemoteNode_c *remoteNode = NULL;
    
    ItemToRemoteNodeMap_t::iterator iter = m_pdRemoteNodes.find( &pdItem );
    if( iter == m_pdRemoteNodes.end() )
    {
      remoteNode = new PdRemoteNode_c( pdItem, false );
      m_pdRemoteNodes[ &pdItem ] = remoteNode;
    }
    else
      remoteNode = iter->second;

    isoaglib_assert( !remoteNode->isServer() );

    PdConnection_c* connection = new PdConnection_c( identItem, remoteNode, pool );
    m_connections.push_back( connection );

    return connection;
  }


  PdConnection_c*
  TcClient_c::connectBroadcast( const IdentItem_c& identItem, const PdPool_c& pool )
  {
    for (STL_NAMESPACE::list<PdConnection_c*>::const_iterator connection = m_connections.begin();
         connection != m_connections.end(); ++connection)
    {
      if ((&(*connection)->getIdentItem() == &identItem) && ((*connection)->getRemoteItem() == NULL))
      {
        isoaglib_assert( !"Double connect between one IdentItem_c and pdServer/pdNode detected!" );
        return NULL;
      }
    }

    PdConnection_c* connection = new PdConnection_c( identItem, NULL, pool );
    m_connections.push_back( connection );

    return connection;
  }


  void
  TcClient_c::disconnect( const IdentItem_c& identItem )
  {
    for (STL_NAMESPACE::list<PdConnection_c*>::iterator connection = m_connections.begin();
         connection != m_connections.end(); /*++connection*/)
    {
      if (&(*connection)->getIdentItem() == &identItem)
      {
        delete *connection;
        connection = m_connections.erase(connection);
      }
      else
        ++connection;
    }
  }

  void
  TcClient_c::disconnect( const PdConnection_c& connection)
  {
    STL_NAMESPACE::list<PdConnection_c*>::iterator iter = STL_NAMESPACE::find(m_connections.begin(),m_connections.end(), &connection);
    isoaglib_assert(iter != m_connections.end());
    delete (*iter);
    m_connections.erase(iter);
  }

  void
  TcClient_c::getAllServers( IsoAgLib::ProcData::ServerList& list_to_fill )
  {
    for (ItemToRemoteNodeMap_t::iterator iter = m_pdRemoteNodes.begin(); iter != m_pdRemoteNodes.end(); ++iter)
    {
      if( iter->second->isServer() )
      {
        ServerInstance_c *server = static_cast<ServerInstance_c*>(iter->second);
        if( server->isAlive() )
          list_to_fill.push_back( STL_NAMESPACE::make_pair(&iter->first->toConstIisoItem_c(), server->getEcuType()) );
      }
    }
  }


  void
  TcClient_c::processMsg( const CanPkg_c& data )
  {
    ProcessPkg_c pkg( data, getMultitonInst() );

    if( ! pkg.isValid() || ( pkg.getMonitorItemForSA() == NULL ) )
      return;

    // @todo to be removed later on!
    if (m_pdMessageHandler)
        m_pdMessageHandler->_eventPdMessageReceived(
            *pkg.getMonitorItemForSA(),
            pkg.getMonitorItemForDA(),
            pkg.men_command,
            pkg.mui16_element,
            pkg.mui16_DDI,
            pkg.mi32_pdValue);

    ItemToRemoteNodeMap_t::iterator iter = m_pdRemoteNodes.find( pkg.getMonitorItemForSA() );
    if( iter == m_pdRemoteNodes.end() )
      return;

    iter->second->processMsg( pkg );
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
        m_pdRemoteNodes[ &isoItem ] = new ServerInstance_c( isoItem, IsoAgLib::ProcData::RemoteTypeTaskController );
        break;

      case IsoName_c::ecuTypeDataLogger:
        m_pdRemoteNodes[ &isoItem ] = new ServerInstance_c( isoItem, IsoAgLib::ProcData::RemoteTypeDataLogger );
        break;
      }
      break;

    case ControlFunctionStateHandler_c::RemoveFromMonitorList:
      {
        ItemToRemoteNodeMap_t::iterator i = m_pdRemoteNodes.find( &isoItem );
        if( i != m_pdRemoteNodes.end() )
        {
          if( i->second->isServer() )
          {
            ServerInstance_c* server = static_cast<ServerInstance_c *>( i->second );
            if( server->isAlive() )
              notifyServerStatusChange( *server, false );
          }
          else
            notifyPdNodeDestruction( *(i->second) );

          delete i->second;
          m_pdRemoteNodes.erase( i );
        }
      }
      break;

    default:
      ;
    }
  }


  void
  TcClient_c::processChangeDesignator( const IdentItem_c& ident, uint16_t objID, const char* newDesig )
  {
    for( STL_NAMESPACE::list<PdConnection_c*>::const_iterator it = m_connections.begin();
         it != m_connections.end(); ++it )
    {
      if( ( &ident == &( ( *it )->getIdentItem() ) ) 
        && ( *it )->getRemoteNode()
        && ( *it )->getRemoteNode()->isServer() )
      {
        static_cast<TcClientConnection_c*>( *it )->sendCommandChangeDesignator( objID, newDesig, CNAMESPACE::strlen( newDesig ) );
      }
    }
  }


  void
  TcClient_c::notifyServerStatusChange(ServerInstance_c& server, bool new_status)
  {
    if (!new_status)
    {
      STL_NAMESPACE::list<TcClientConnection_c*> delete_connections;
      // first identify the PdConnection_c that are concerned
      for (STL_NAMESPACE::list<PdConnection_c*>::iterator connection = m_connections.begin();
          connection != m_connections.end(); /*++connection*/ )
      {
        if ((*connection)->getRemoteItem() == &server.getIsoItem())
        {
          delete_connections.push_back( static_cast<TcClientConnection_c*>(*connection) );
          connection = m_connections.erase(connection);
        }
        else
          ++connection;
      }
      // then report disconnected TcClientConnection_c to application
      // since we do not loop m_connections, the application can safely connect to an other Server
      for (STL_NAMESPACE::list<TcClientConnection_c*>::iterator connection = delete_connections.begin();
          connection != delete_connections.end(); ++connection)
      {
        if (m_stateHandler)
          m_stateHandler->_eventDisconnectedOnServerLoss(**connection);
        delete *connection;
      }
    }

    if (m_stateHandler)
      m_stateHandler->_eventServerAvailable( server.getIsoItem(), server.getEcuType(), new_status );
  }


  void
  TcClient_c::notifyPdNodeDestruction( PdRemoteNode_c& pdRemoteNode )
  {
    for (STL_NAMESPACE::list<PdConnection_c*>::iterator connection = m_connections.begin();
        connection != m_connections.end(); )
    {
      if ((*connection)->getRemoteNode() == &pdRemoteNode)
      {
        delete *connection;
        connection = m_connections.erase( connection );
      }
      else
        ++connection;
    }
  }

  void TcClient_c::sendPdMessage( const IsoItem_c& sa_item, const IsoItem_c* da_item, IsoAgLib::ProcData::CommandType_t command, uint16_t element, uint16_t ddi, int32_t value )
  {
    isoaglib_assert(command < IsoAgLib::ProcData::CommandUndefined);

    ProcessPkg_c pkg( command, element, ddi, value );

    pkg.setMonitorItemForDA( const_cast<IsoItem_c*>( da_item ));
    pkg.setMonitorItemForSA( const_cast<IsoItem_c*>( &sa_item ));

    getIsoBusInstance4Comm() << pkg;
  }

#ifdef HAL_USE_SPECIFIC_FILTERS
  void TcClient_c::receivePdMessage(const IsoItem_c& sa_item, const IsoItem_c* da_item, IsoAgLib::ProcData::CommandType_t command, uint16_t element, uint16_t ddi, int32_t value)
  {
    if (m_pdMessageHandler)
      m_pdMessageHandler->_eventPdMessageReceived(sa_item, da_item, command, element, ddi, value);
  }
#endif

} // __IsoAgLib
