/*
  pdremotenode_c.cpp: Instance of a DDOP-less PD-node

  (C) Copyright 2013 - 2019 by OSB AG

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
#include "serverinstance_c.h"
#include "tcclient_c.h"
#include "processpkg_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>


namespace __IsoAgLib {


  PdRemoteNode_c::PdRemoteNode_c( const IsoItem_c& isoItem, bool isServer )
    : SchedulerTask_c( 100, false )
    , m_isoItem( isoItem )
    , m_connections()
    , m_isServer( isServer )
  {
  }


  PdRemoteNode_c::~PdRemoteNode_c()
  {
    isoaglib_assert( m_connections.empty() );
  }


  void
  PdRemoteNode_c::addConnection( PdConnection_c& c )
  {
    // avoid double connect
    // - manual connect from event-callback (probably due to app. only connecting later)
    // - reconnect inside of ISOAgLib
    for( std::list<PdConnection_c*>::iterator it = m_connections.begin();
         it != m_connections.end(); ++it )
    {
      if( *it == &c )
        return;
    }
    m_connections.push_back( &c );
  }


  void
  PdRemoteNode_c::removeConnection( PdConnection_c& c )
  {
    for( std::list<PdConnection_c*>::iterator it = m_connections.begin();
         it != m_connections.end(); ++it )
    {
      if( *it == &c )
      {
        m_connections.erase( it );
        break;
      }
    }
  }


  void
  PdRemoteNode_c::processMsg( const ProcessPkg_c& pkg )
  {
    for( std::list<PdConnection_c*>::const_iterator it = m_connections.begin();
         it != m_connections.end(); ++it )
    {
      if( ( pkg.getMonitorItemForDA() == NULL ) || 
          ( pkg.getMonitorItemForDA() == (*it)->getIdentItem().getIsoItem() ) )
      {
        ( *it )->processProcMsg( pkg );
        return;
      }
    }
  }

}
