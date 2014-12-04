/*
  serverinstance_c.cpp: Instance of a TC or DL

  (C) Copyright 2013 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "serverinstance_c.h"
#include "tcclient_c.h"
#include "processpkg_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>


namespace __IsoAgLib {


  ServerInstance_c::ServerInstance_c( const IsoItem_c& isoItem, IsoAgLib::ProcData::RemoteType_t type )
    : PdRemoteNode_c( isoItem, true )
    , m_tcAliveCached( false )
    , m_lastActiveTaskTC( false )
    , m_lastTcState( 0 )
    , m_lastTcStateReceivedTime( -1 )
    , m_type( type )
  {
    // assume the SchedulerTask_c is properly c'ted in PdRemoteNode_c
    getSchedulerInstance().registerTask(*this,0);
  }


  ServerInstance_c::~ServerInstance_c()
  {
    getSchedulerInstance().deregisterTask(*this);
  }


  void ServerInstance_c::timeEvent() {
    const bool tcStatusOld = m_tcAliveCached;
    m_tcAliveCached = isAlive();
    if (m_tcAliveCached != tcStatusOld) {
      getTcClientInstance(m_isoItem.getMultitonInst()).notifyServerStatusChange(*this, m_tcAliveCached);
    }
  }


  bool ServerInstance_c::isAlive() const {
    return ( getLastStatusTime() != -1 ) && ( HAL::getTime() - getLastStatusTime() <= 6000 );
  }


  void
  ServerInstance_c::processMsg( const ProcessPkg_c& pkg )
  {
    PdRemoteNode_c::processMsg( pkg );

    switch (pkg.men_command)
    {
      case IsoAgLib::ProcData::taskControllerStatus:
        processStatus( pkg[4] );
        break;

      case IsoAgLib::ProcData::requestConfiguration:
      case IsoAgLib::ProcData::configurationResponse:
      case IsoAgLib::ProcData::nack:
      case IsoAgLib::ProcData::requestValue:
      case IsoAgLib::ProcData::setValue:
      case IsoAgLib::ProcData::measurementTimeValueStart:
      case IsoAgLib::ProcData::measurementDistanceValueStart:
      case IsoAgLib::ProcData::measurementMinimumThresholdValueStart:
      case IsoAgLib::ProcData::measurementMaximumThresholdValueStart:
      case IsoAgLib::ProcData::measurementChangeThresholdValueStart:
      case IsoAgLib::ProcData::commandReserved1:
      case IsoAgLib::ProcData::commandReserved2:
      case IsoAgLib::ProcData::commandReserved3:
      case IsoAgLib::ProcData::commandReserved4:
      case IsoAgLib::ProcData::workingsetMasterMaintenance:
      case IsoAgLib::ProcData::CommandUndefined:
        ; // not handled when global
    }
  }


  void
  ServerInstance_c::processStatus( uint8_t status )
  {
    m_lastTcState = status;
    m_lastTcStateReceivedTime = HAL::getTime();

    const bool activeTask = status & 0x01;

    if ( activeTask != m_lastActiveTaskTC ) {
      if ( activeTask ) {
        for( STL_NAMESPACE::list<PdConnection_c*>::const_iterator it = m_connections.begin();
             it != m_connections.end(); ++it )
        {
          static_cast<TcClientConnection_c*>( *it )->eventTaskStarted(); 
        }
      } else {
        for( STL_NAMESPACE::list<PdConnection_c*>::const_iterator it = m_connections.begin();
             it != m_connections.end(); ++it )
        {
          static_cast<TcClientConnection_c*>( *it )->stopRunningMeasurement();
          static_cast<TcClientConnection_c*>( *it )->eventTaskStopped();
        }
      }
    }

    m_lastActiveTaskTC = activeTask;
  }

}
