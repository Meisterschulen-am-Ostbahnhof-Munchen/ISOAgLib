/*
  serverinstance_c.cpp:

  (C) Copyright 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "serverinstance_c.h"

namespace __IsoAgLib {


  ServerInstance_c::ServerInstance_c( const IsoItem_c& it, IsoAgLib::ProcData::RemoteType_t type ) :
    m_isoItem( it ),
    m_lastActiveTaskTC( false ),
    m_lastTcState( 0 ),
    m_lastTcStateReceivedTime( -1 ),
    m_type( type )
  {}


  ServerInstance_c::~ServerInstance_c() {
    STL_NAMESPACE::list<TcClientConnection_c*>::iterator it = m_connections.begin();
    while ( it != m_connections.end() ) {
      ( *it )->stopRunningMeasurement();
      ( *it )->setServer( 0 );
      ++it;
    }
  }


  bool ServerInstance_c::isAlive() const {
    return ( getLastStatusTime() != -1 ) && ( HAL::getTime() - getLastStatusTime() <= 6000 );
  }


  void ServerInstance_c::removeConnection( TcClientConnection_c& c ) {
    STL_NAMESPACE::list<TcClientConnection_c*>::iterator it = m_connections.begin();
    while ( it != m_connections.end() ) {
      if( *it == &c ) {
        m_connections.erase( it );
        break;
      }
      ++it;
    }
  }


  void ServerInstance_c::processStatus( uint8_t status ) {

    m_lastTcState = status;
    m_lastTcStateReceivedTime = HAL::getTime();

    const bool activeTask = status & 0x1;

    if ( activeTask != m_lastActiveTaskTC ) {
      if ( activeTask ) {
        STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_connections.begin();
        while ( it != m_connections.end() ) {
          ( *it )->eventTaskStarted();
          ++it;
        }
      } else {
        STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_connections.begin();
        while ( it != m_connections.end() ) {
          ( *it )->stopRunningMeasurement();
          ( *it )->eventTaskStopped();
          ++it;
        }
      }
    }

    m_lastActiveTaskTC = activeTask;
  }

}
