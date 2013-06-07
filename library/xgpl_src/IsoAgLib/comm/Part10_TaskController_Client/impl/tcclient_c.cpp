/*
  tcclient_c.cpp: central managing instance for all Task Controller-Clients

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "tcclient_c.h"

#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>
#ifdef HAL_USE_SPECIFIC_FILTERS
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#endif
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/idevicepool_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/itcclient_c.h>
#include <IsoAgLib/util/iassert.h>

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
    , m_customer( *this ) {
  }


  const TcClientConnection_c&
  TcClient_c::getTcClientConnection( const IdentItem_c& identItem ) const {
    STL_NAMESPACE::list<identData_t>::const_iterator i = m_identdata.begin();
    for( ; i != m_identdata.end(); ++i ) {
      if( i->ident == &identItem ) {
        break;
      }
    }
    isoaglib_assert( !"No matching TcClientConnection found!!!" );
    return *( i->connections.front() );
  }


  void
  TcClient_c::init( IsoAgLib::iTcDlStateHandler_c& hdl ) {
    isoaglib_assert ( !initialized() );

    m_stateHandler = &hdl;

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
    isoaglib_assert ( initialized() );

#ifdef HAL_USE_SPECIFIC_FILTERS
    getIsoBusInstance4Comm().deleteFilter( m_customer, IsoAgLib::iMaskFilter_c( ( 0x3FFFF00UL ), ( PROCESS_DATA_PGN | 0xFF ) << 8 ) );
#else
    getIsoBusInstance4Comm().deleteFilter( m_customer, IsoAgLib::iMaskFilter_c( ( 0x3FF0000UL ), PROCESS_DATA_PGN << 8 ) );
#endif

    getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( m_handler );

    setClosed();
  }


  TcClientConnection_c*
  TcClient_c::connect( IdentItem_c& identItem, IsoAgLib::iTcClientConnectionStateHandler_c& sh, const IsoItem_c& tcdl, DevicePool_c& pool ) {
    STL_NAMESPACE::map<const IsoItem_c*,ServerInstance_c>::iterator server = m_server.find( &tcdl );
    isoaglib_assert( server != m_server.end() );

    identData_t* d = getDataFor( identItem );
    if( ! d ) {
      m_identdata.push_back( identData_t() );
      d = &( m_identdata.back() );
      d->ident = &identItem;
    } else {
      // check for connections between this ident and the give server
      for( STL_NAMESPACE::list<TcClientConnection_c*>::iterator c = d->connections.begin(); c != d->connections.end(); ++c ) {
        if( ( *c )->getServerName() == tcdl.isoName() ) {
          isoaglib_assert( !"Double connect between one IdentItem_c and server detected!" );
          return 0;
        }
      }
    }

    // setup connection
    TcClientConnection_c* c = new TcClientConnection_c( identItem, *this, sh, tcdl.isoName(), pool );
    d->connections.push_back( c );

    // assign connection to server
    server->second.addConnection( *c );

    return c;
  }


  bool
  TcClient_c::disconnect( IdentItem_c& identItem ) {
    STL_NAMESPACE::list<identData_t>::iterator i = m_identdata.begin();
    for( ; i != m_identdata.end(); ++i ) {
      if( i->ident == &identItem ) {
        break;
      }
    }
    isoaglib_assert( i != m_identdata.end() );

    STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = ( *i ).connections.begin();
    while ( it != ( *i ).connections.end() ) {
      for( STL_NAMESPACE::map<const IsoItem_c*,ServerInstance_c>::iterator ic = m_server.begin(); ic != m_server.end(); ++ic ) {
        ic->second.removeConnection( **it );
      }
      delete( *it );
      ++it;
    }

    m_identdata.erase( i );

    return true;
  }


  void
  TcClient_c::processMsg( const CanPkg_c& data ) {
    ProcessPkg_c pkg( data, getMultitonInst() );

    if( ! pkg.isValid() || ( pkg.getMonitorItemForSA() == NULL ) )
      return;

    if( pkg.getMonitorItemForDA() != NULL )
      processMsgNonGlobal( pkg );
    else
      processMsgGlobal( pkg );
  }


  void
  TcClient_c::processMsgGlobal( const ProcessPkg_c& data ) {
    // process TC status message (for local instances)
    if ( data.men_command == ProcessPkg_c::taskControllerStatus ) {
      processTcStatusMsg( data[4], data.getMonitorItemForSA()->isoName() );
    }
  }


  void
  TcClient_c::processMsgNonGlobal( const ProcessPkg_c& pkg ) {
    identData_t* identData = getDataFor( *pkg.getMonitorItemForDA() );

    if( ! identData ) {
      return;
    }

    // no forther processing of NACK messages
    if ( pkg.men_command == ProcessPkg_c::nack )
      return;

    // ignore other working set task message
    // @TODO probably respond with NACK if it is addressed to us, otherwise just ignore
    if ( pkg.men_command == ProcessPkg_c::workingsetMasterMaintenance )
      return;

    STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = identData->connections.begin();
    while ( it != identData->connections.end() ) {
      ( *it )->processMsg( pkg );
      ++it;
    }
  }


  const IsoName_c&
  TcClient_c::getISONameFromType( IsoAgLib::ProcData::RemoteType_t ecuType ) const {
    for( STL_NAMESPACE::map<const IsoItem_c*,ServerInstance_c>::const_iterator i = m_server.begin(); i != m_server.end(); ++i ) {
      if( ecuType == i->second.getEcuType() ) {
        return i->first->isoName();
      }
    }
    return IsoName_c::IsoNameUnspecified();
  }


  IsoAgLib::ProcData::RemoteType_t
  TcClient_c::getTypeFromISOName( const IsoName_c& isoName ) const {
    for( STL_NAMESPACE::map<const IsoItem_c*,ServerInstance_c>::const_iterator i = m_server.begin(); i != m_server.end(); ++i ) {
      if( isoName == i->first->isoName() ) {
        return i->second.getEcuType();
      }
    }
    return IsoAgLib::ProcData::RemoteTypeUndefined;
  }


  void
  TcClient_c::reactOnIsoItemModification( ControlFunctionStateHandler_c::iIsoItemAction_e action, IsoItem_c const& isoItem ) {
    if( ( action == ControlFunctionStateHandler_c::RemoveFromMonitorList ) &&( ! isoItem.itemState( IState_c::Local ) ) ) {
      STL_NAMESPACE::map<const IsoItem_c*,ServerInstance_c>::iterator i = m_server.find( &isoItem );
      if( i != m_server.end() ) {
        i->second.close();
        m_server.erase( i );
      }
    }

#ifdef HAL_USE_SPECIFIC_FILTERS
    // @todo Clean that up properly, move it to TC-Client connection!!!
    // This is just a simple implementation until the Dynamic TC-Client is ready!
    // Note that the filter wouldn't be remove if the IdentItem is closed AFTER the TcClient.
    // This will be redone for 2.5.4 anyway. It's fine if the application is completely closed/restarted,
    // but may result in problems if only the TC-Client would be stopped without the rest...
    // But nowone does that in 2.5.3.........
    if( isoItem.itemState( IState_c::Local ) ) {
      switch( action ) {
        case ControlFunctionStateHandler_c::AddToMonitorList:
          getIsoFilterManagerInstance4Comm().insertIsoFilter( IsoFilter_s ( m_customer, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, ( PROCESS_DATA_PGN << 8 ) ), &isoItem.isoName(), NULL, 8 ) );
          break;
        case ControlFunctionStateHandler_c::RemoveFromMonitorList:
          getIsoFilterManagerInstance4Comm().removeIsoFilter( IsoFilter_s ( m_customer, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, ( PROCESS_DATA_PGN << 8 ) ), &isoItem.isoName(), NULL, 8 ) );
          break;
        default:
          break;
      }
    }
#endif
  }


  void
  TcClient_c::processTcStatusMsg( uint8_t tcStatus, const __IsoAgLib::IsoName_c& sender ) {
    IsoItem_c* s = __IsoAgLib::getIsoMonitorInstance4Comm().item( sender );
    isoaglib_assert( s );

    STL_NAMESPACE::map<const IsoItem_c*,ServerInstance_c>::iterator server = m_server.find( s );

    if( server == m_server.end() ) {
      const IsoAgLib::ProcData::RemoteType_t ecuType = sender.getEcuType() == IsoName_c::ecuTypeTaskControl ?
          IsoAgLib::ProcData::RemoteTypeTaskControl : IsoAgLib::ProcData::RemoteTypeUndefined;

      m_server[ s ] = ServerInstance_c( *s, ecuType );
      server = m_server.find( s );
      m_stateHandler->eventTcDlAvailable( static_cast<const IsoAgLib::iIsoItem_c&>( *s ), ecuType );

      // check for connections that useed this server. Readd those connections to that server
      // and set to inital state
      for( STL_NAMESPACE::list<identData_t>::iterator i = m_identdata.begin(); i != m_identdata.end(); ++i ) {
        for( STL_NAMESPACE::list<TcClientConnection_c*>::iterator c = i->connections.begin(); c != i->connections.end(); ++c ) {
          if( ( *c )->getServerName() == sender ) {
            if( server->second.addConnection( **c ) ) {
              ( *c )->setDevPoolState( TcClientConnection_c::PoolStateInit );
            }
          }
        }
      }
    }

    server->second.processStatus( tcStatus );
  }


  void
  TcClient_c::processChangeDesignator( IdentItem_c& ident, uint16_t objID, const char* newDesig ) {
    identData_t* identData = getDataFor( ident );
    isoaglib_assert( identData );

    STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = identData->connections.begin();
    while ( it != identData->connections.end() ) {
      if ( &ident == &( ( *it )->getIdentItem() ) )
        ( *it )->sendCommandChangeDesignator( objID, newDesig, CNAMESPACE::strlen( newDesig ) );
      ++it;
    }
  }


  TcClient_c::identData_t*
  TcClient_c::getDataFor( IdentItem_c& ident ) {
    return getDataFor( *ident.getIsoItem() );
  }


  TcClient_c::identData_t*
  TcClient_c::getDataFor( IsoItem_c& item ) {
    STL_NAMESPACE::list<identData_t>::iterator i = m_identdata.begin();
    for( ; i != m_identdata.end(); ++i ) {
      if( i->ident->getIsoItem() == &item ) {
        return &( *i );
      }
    }
    return 0;
  }


  ServerInstance_c* TcClient_c::getServer( const IsoName_c& name ) {
    STL_NAMESPACE::map<const IsoItem_c*,ServerInstance_c>::iterator server = m_server.find( __IsoAgLib::getIsoMonitorInstance4Comm().item( name ) );
    return ( server == m_server.end() ) ? 0 : &( server->second );
  }




} // __IsoAgLib
