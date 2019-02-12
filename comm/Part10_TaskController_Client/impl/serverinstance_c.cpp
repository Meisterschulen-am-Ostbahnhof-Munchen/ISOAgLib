/*
  serverinstance_c.cpp: Instance of a TC or DL

  (C) Copyright 2013 - 2016 by OSB AG and developing partners

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


  ServerInstance_c::ServerInstance_c( const IsoItem_c& isoItem, IsoAgLib::ProcData::ServerType_t type )
    : PdRemoteNode_c( isoItem, true )
    , m_tcAliveCached( false )
    , m_lastTcState( 0 )
    , m_lastTcStateReceivedTime( -1 )
    , m_tcNotBusyCount( 0 )
    , m_type( type )
  {
    // assume the SchedulerTask_c is properly c'ted in PdRemoteNode_c
    getSchedulerInstance().registerTask(*this,0);
  }


  ServerInstance_c::~ServerInstance_c()
  {
    getSchedulerInstance().deregisterTask(*this);
  }


  /** convert function - avoids lots of explicit static_casts */
  const IsoAgLib::iTcClientServer_c&  ServerInstance_c::toConstITcClientServer_c() const
  {
    return (const IsoAgLib::iTcClientServer_c&)(*this);
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
  
  bool ServerInstance_c::isBusy() const {
    return ( ((m_lastTcState & (1 << 1)) != 0) ||
             ((m_lastTcState & (1 << 2)) != 0) ||
             ((m_lastTcState & (1 << 3)) != 0) );
  }


  void
  ServerInstance_c::processMsg( const ProcessPkg_c& pkg )
  {
    PdRemoteNode_c::processMsg( pkg );

    if( pkg.getMonitorItemForDA() == NULL )
    {
      switch (pkg.men_command)
      {
        case IsoAgLib::ProcData::TaskControllerStatus:
          processStatus( pkg[4] );
          break;

        case IsoAgLib::ProcData::TechnicalData:
        case IsoAgLib::ProcData::DeviceDescriptor:
        case IsoAgLib::ProcData::ProcessDataAcknowledge:
        case IsoAgLib::ProcData::RequestValue:
        case IsoAgLib::ProcData::Value:
        case IsoAgLib::ProcData::MeasurementTimeValueStart:
        case IsoAgLib::ProcData::MeasurementDistanceValueStart:
        case IsoAgLib::ProcData::MeasurementMinimumThresholdValueStart:
        case IsoAgLib::ProcData::MeasurementMaximumThresholdValueStart:
        case IsoAgLib::ProcData::MeasurementChangeThresholdValueStart:
        case IsoAgLib::ProcData::ControlAssignment:
        case IsoAgLib::ProcData::SetValueAndAcknowledge:
        case IsoAgLib::ProcData::ReservedB:
        case IsoAgLib::ProcData::ReservedC:
        case IsoAgLib::ProcData::ClientTaskOrNotApplicable:
          ; // not handled when global
      }
    }
    // else: Destination specific, not handled HERE.
  }


  void
  ServerInstance_c::processStatus( uint8_t status )
  {
    const bool activeTaskOld = getLastStatusTaskTotalsActive();

    m_lastTcState = status;
    m_lastTcStateReceivedTime = HAL::getTime();
    
    if ( isBusy() )
      m_tcNotBusyCount = 0;
    else
      m_tcNotBusyCount++;

    const bool activeTaskNew = getLastStatusTaskTotalsActive();

    if ( activeTaskNew != activeTaskOld ) {
      for( STL_NAMESPACE::list<PdConnection_c*>::const_iterator it = m_connections.begin();
            it != m_connections.end(); ++it )
      {
        static_cast<TcClientConnection_c*>( *it )->eventTaskStartStop( activeTaskNew );
      }
    }
  }


  void
  ServerInstance_c::notifyOnDropOff()
  {
    m_lastTcStateReceivedTime = -1;
  }


}
