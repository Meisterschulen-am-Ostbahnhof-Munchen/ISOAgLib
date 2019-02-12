/*
  tcclientconnection_c.cpp: class for managing a client/server connection
                            to TC/DL/Proprietary Servers

  (C) Copyright 2009 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "tcclientconnection_c.h"

#include <IsoAgLib/util/iassert.h>
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#ifdef HAL_USE_SPECIFIC_FILTERS
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#endif
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/serverinstance_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclient_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measureprog_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/idevicepool_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif



namespace __IsoAgLib {

  TcClientConnection_c::TcClientConnection_c() 
    : PdConnection_c()
    , m_multiSendEventHandler( *this )
    , m_multiSendStreamer( *this )
    , mc_mrClient( *this )
    , mc_localCsHandler( *this )
    , m_currentSendPosition( 0 )
    , m_storedSendPosition( 0 )
    , m_devicePoolToUpload()
    , m_timeWsAnnounceKey( -1 )
    , m_timeWaitWithAnyCommunicationUntil( -1 )
    , m_timeLastWsTaskMsgSent( -1 )
    , m_stateHandler( NULL )
    , m_devPoolState( PoolStateDisconnected )
    , m_currentCommand()
    , m_capsClient()
    , m_capsServer()
    , m_schedulerTaskProxy( *this, 100, false )
  {
  }

  TcClientConnection_c::TcClientConnection_c( const TcClientConnection_c &rhs )
    : PdConnection_c( rhs )
    , m_multiSendEventHandler( *this )
    , m_multiSendStreamer( *this )
    , mc_mrClient( *this )
    , mc_localCsHandler( *this )
    , m_currentSendPosition( rhs.m_currentSendPosition )
    , m_storedSendPosition( rhs.m_storedSendPosition )
    , m_devicePoolToUpload( rhs.m_devicePoolToUpload )
    , m_timeWsAnnounceKey( rhs.m_timeWsAnnounceKey )
    , m_timeWaitWithAnyCommunicationUntil( rhs.m_timeWaitWithAnyCommunicationUntil )
    , m_timeLastWsTaskMsgSent( rhs.m_timeLastWsTaskMsgSent )
    , m_stateHandler( rhs.m_stateHandler )
    , m_devPoolState( rhs.m_devPoolState )
    , m_currentCommand( rhs.m_currentCommand )
    , m_capsClient( rhs.m_capsClient )
    , m_capsServer( rhs.m_capsServer )
    , m_schedulerTaskProxy( *this, 100, false )
  {
  }

  TcClientConnection_c::~TcClientConnection_c()
  {
  }


  void TcClientConnection_c::preConnect(
    const IdentItem_c& identItem,
    ServerInstance_c& server,
    StateHandler_c& sh,
    const IsoAgLib::ProcData::ClientCapabilities_s& capabilities )
  {
    isoaglib_assert( getDevPoolState() == PoolStateDisconnected );

    PdConnection_c::init( identItem, &server );

    m_stateHandler = &sh;
    m_capsClient = capabilities;

    getSchedulerInstance().registerTask( m_schedulerTaskProxy, 0 );

    m_currentCommand.init();
    m_timeWsAnnounceKey = -1;
    m_timeWaitWithAnyCommunicationUntil = -1;
    m_timeLastWsTaskMsgSent = -1;

    setDevPoolState( PoolStatePreconnecting );
  }


  void TcClientConnection_c::reConnect(
    const IdentItem_c& identItem,
    ServerInstance_c& server
    )
  {
    isoaglib_assert( getDevPoolState() == PoolStateDisconnected );

    PdConnection_c::init( identItem, &server );

    getSchedulerInstance().registerTask( m_schedulerTaskProxy, 0 );

    m_currentCommand.init();
    m_timeWsAnnounceKey = -1;
    m_timeWaitWithAnyCommunicationUntil = -1;
    m_timeLastWsTaskMsgSent = -1;

    setDevPoolState( PoolStatePreconnecting );
  }

  bool TcClientConnection_c::fullConnect( const ServerInstance_c& server, DevicePool_c& pool )
  {
    if( pool.isEmpty() || ( pool.getDvcObject() == NULL ) )
      return false;

    if( getDevPoolState() != PoolStateAwaitingConnectionDecision )
      return false;

    if( connected() != &server )
      return false;

    PdConnection_c::start( pool );
    
    getDevicePool().init( getIdentItem() );

    doCommand( procCmdPar_RequestStructureLabelMsg, procCmdPar_StructureLabelMsg );

    setDevPoolState( PoolStateConnecting );

    getMultiReceiveInstance( m_identItem->getMultitonInst() ).registerClientIso( mc_mrClient,  m_identItem->isoName(), PROCESS_DATA_PGN, 0x3FFFFUL, false, false, &connected()->getIsoItem().isoName() );

    return true;
  }


  void TcClientConnection_c::disconnect( bool shouldDeletePool )
  {
    isoaglib_assert( connected() );

    getMultiReceiveInstance( m_identItem->getMultitonInst() ).deregisterClient( mc_mrClient, m_identItem->isoName(), PROCESS_DATA_PGN, 0x3FFFFUL, &connected()->getIsoItem().isoName() );
    getMultiSendInstance( m_identItem->getMultitonInst() ).abortSend( m_multiSendEventHandler );

    if( m_devicePoolToUpload.hasBuffer() )
    {
      getDevicePool().freeByteStreamBuffer( m_devicePoolToUpload.getBuffer() );
      m_devicePoolToUpload.setBuffer( NULL );
    }

    // is it worth sending a proper "I'm disconnecting" message??
    if( connected()->isAlive() )
    {
      if( getDevPoolState() == PoolStateUploaded || getDevPoolState() == PoolStateActive )
      {
        doCommand( procCmdPar_OPActivateMsg, procCmdPar_OPActivateRespMsg, DEF_TimeOut_DdopDeactivation, 0 );

        if (shouldDeletePool)
        {
          doCommand( procCmdPar_OPDeleteMsg, procCmdPar_OPDeleteRespMsg, DEF_TimeOut_DdopDeactivation );
        }
      }
    }
  
    getSchedulerInstance().deregisterTask( m_schedulerTaskProxy );

    stopRunningMeasurement();
    mc_localCsHandler.stop();

    if (connected()->getLastStatusTaskTotalsActive())
      eventTaskStartStop( false );

    getDevicePool().close();

    PdConnection_c::stop();
    PdConnection_c::close();

    const bool wasFullyConnected = ( getDevPoolState() != PoolStatePreconnecting ) && ( getDevPoolState() != PoolStateAwaitingConnectionDecision );
    setDevPoolState( PoolStateDisconnected );

    if( wasFullyConnected )
      m_stateHandler->_eventDisconnectedOnServerLoss( *this );
  }

  void TcClientConnection_c::forceDisconnectAndInitiateReconnect( bool shouldDeletePool )
  {
      ServerInstance_c* server = connected();
      isoaglib_assert(server);

      const IdentItem_c& identItem = getIdentItem();
      
      disconnect( shouldDeletePool );
      reConnect( identItem, *server );
  }

  void
  TcClientConnection_c::timeEvent( void )
  {
    if( getDevPoolState() == PoolStateDisconnected )
      return;

    isoaglib_assert( ( m_identItem != NULL ) && connected() );

    if( !m_identItem->isClaimedAddress() )
      return;

    if( m_timeWaitWithAnyCommunicationUntil < 0 )
      m_timeWaitWithAnyCommunicationUntil = HAL::getTime() + 6000;

    if( HAL::getTime() < m_timeWaitWithAnyCommunicationUntil )
      return;

    if( connected()->getLastStatusTime() < 0 )
      return;

    if( m_timeWsAnnounceKey < 0 )
      m_timeWsAnnounceKey = m_identItem->getIsoItem()->startWsAnnounce();

    if( !m_identItem->getIsoItem()->isWsAnnounced( m_timeWsAnnounceKey ) )
      return;

    // Send the WS task message to maintain connection with the TC
    const ecutime_t now = HAL::getTime();
    if ( now - m_timeLastWsTaskMsgSent >= 2000 ) {
      m_timeLastWsTaskMsgSent = now;
      sendMsg( 0xff, 0xff, 0xff, 0xff, (connected()->getLastStatusTaskTotalsActive() ? 0x01 : 0x00), 0x00, 0x00, 0x00 );
    }

    switch( m_currentCommand.getState() )
    {
    case CurrentCommand_c::Idle:
    {
      switch ( getDevPoolState() )
      {
      case PoolStatePreconnecting:
        doCommand( procCmdPar_RequestVersionMsg, procCmdPar_VersionMsg );
        break;

      case PoolStateAwaitingConnectionDecision:
        // Well yeah, wait for the application to finally decide and in case of connecting, have its DDOP ready...
        break;

      case PoolStateUploading:
        m_devicePoolToUpload = getDevicePool().getBytestream( procCmdPar_OPTransferMsg, m_capsConnection );
        doCommand( procCmdPar_RequestOPTransferMsg, procCmdPar_RequestOPTransferRespMsg );
        break;

      case PoolStateUploaded:
        doCommand( procCmdPar_OPActivateMsg, procCmdPar_OPActivateRespMsg );
        break;

      case PoolStateActive:
        break;

      case PoolStateError:
        getTcClientInstance( getIdentItem().getMultitonInst() ).notifyConnectionToBeEnded( *this );
        break;

      default:
        break;
      }
    }
      break;
    case CurrentCommand_c::Running:
       break;
    case CurrentCommand_c::FinishedAndWaitingForResponse:
    {
      isoaglib_assert( connected() );

      if ( m_currentCommand.isTimedOut( *connected() ) )
      {
        // Timeout, retry last command?
        // For now, give up!
        m_currentCommand.finished();
        // don't go to PoolStateError, as it cannot be detected then anymore if it was still in Preconnecting or Connected.
        getTcClientInstance( getIdentItem().getMultitonInst() ).notifyConnectionToBeEnded( *this );
      }
    }
    break;
    case CurrentCommand_c::Aborted:
    {
      // The only possible TP message here, is the OP transfer. So no other cmd could have been aborted. Start a retry of the transfer.
      doCommand( procCmdPar_OPTransferMsg, procCmdPar_OPTransferRespMsg );
    }
      break;
    default:
      break;
   }
  }


  void TcClientConnection_c::processRequestDefaultDataLogging()
  {
    m_stateHandler->_eventDefaultLoggingStarted( *this );
  }


  void
  TcClientConnection_c::sendMsg(
    uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3,
    uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7 ) const
  {
    ProcessPkg_c pkg( b0, b1, b2, b3, b4, b5, b6, b7);

    pkg.setMonitorItemForDA( const_cast<IsoItem_c*>( getRemoteItem() ) );
    pkg.setMonitorItemForSA( m_identItem->getIsoItem() );

    getIsoBusInstance4Comm() << pkg;
  }


  bool
  TcClientConnection_c::processControlAssignmentReceiver( bool assign, uint16_t elem, uint16_t ddi, const IsoName_c& name )
  {
    const uint32_t key = getMapKey( ddi, elem );
    ConnectedPdMap_t::iterator iter = m_connectedPds.find(key);

    bool success = false;

    if ( iter != m_connectedPds.end())
    {
      const IsoItem_c* isoItem = getIsoMonitorInstance4Comm().item( name );
      if( isoItem )
      {
        if( assign )
        {
          success = mc_localCsHandler.addSetpointValueSource( elem, ddi, *isoItem );
        }
        else
        {
          success = mc_localCsHandler.removeSetpointValueSource( elem, ddi, *isoItem );
        }
      }
    }

    return success;
  }


  bool
  TcClientConnection_c::processControlAssignmentTransmitter( bool assign, uint16_t elem, uint16_t ddi, const IsoName_c& name, uint16_t destElem )
  {
    const uint32_t key = getMapKey( ddi, elem );
    ConnectedPdMap_t::iterator iter = m_connectedPds.find(key);

    bool success = false;

    if ( iter != m_connectedPds.end())
    {
      const IsoItem_c* isoItem = getIsoMonitorInstance4Comm().item( name );
      MeasureProg_c* mprog = static_cast< MeasureProg_c* >( iter->second );

      if( isoItem && mprog->pdLocal().isControlSource() )
      {

        if( assign )
        {
          mprog->assignSetpointValueUser( isoItem->isoName(), destElem );

          const bool validTriggerMethodT = mprog->startMeasurement( IsoAgLib::ProcData::MeasurementCommandTimeProp, 1000 );
          const bool validTriggerMethodO = mprog->startMeasurement( IsoAgLib::ProcData::MeasurementCommandOnChange, 1 );

          isoaglib_assert( validTriggerMethodT && validTriggerMethodO );
          ( void )validTriggerMethodT; // must be valid in case Control Source bit is set
          ( void )validTriggerMethodO; // must be valid in case Control Source bit is set
        }
        else
        {
          // @todo Not sure if this is correct, or only Time/Chg measurements should be stopped.
          mprog->stopAllMeasurements();

          mprog->unassignSetpointValueUser();
        }
      }
    }

    return success;
  }


  void
  TcClientConnection_c::processMultiPacket( Stream_c &stream )
  {
    const IsoAgLib::ProcData::CommandType_t command = static_cast< IsoAgLib::ProcData::CommandType_t >( stream.getFirstByte() & 0x0F );

    switch( command )
    {
    case IsoAgLib::ProcData::ControlAssignment:
      {
        if( stream.getByteTotalSize() != 14 )
          break;

        // parse message...
        uint16_t elem = ( uint16_t( stream.getFirstByte() ) >> 4 ) | ( uint16_t( stream.getNextNotParsed() ) << 4 );
        uint16_t ddi  = uint16_t( stream.getNextNotParsed() ); // do not put into one term because of unspecified evaluation order!
                 ddi |= uint16_t( stream.getNextNotParsed() ) << 8;

        uint8_t byte5 = stream.getNextNotParsed();
        uint8_t byte6 = stream.getNextNotParsed();

        const ControlAssignmentMode_t mode = static_cast< ControlAssignmentMode_t >( byte5 & 0x0F );
        uint16_t destElem = ( uint16_t( byte5 ) >> 4 ) | ( uint16_t( byte6 ) << 4 );

        uint8_t rawName[ 8 ];
        for( unsigned i=0; i<8; ++i )
          rawName[ i ] = stream.getNextNotParsed();
        const IsoName_c name( rawName );

        // process message...
        bool assign = true;
        bool transmitter = false;
        bool success = false;
        switch( mode )
        {
        case UnassignReceiver:
          assign = false;
        case AssignReceiver:
          success = processControlAssignmentReceiver( assign, elem, ddi, name );
          break;

        case UnassignTransmitter:
          assign = false;
        case AssignTransmitter:
          transmitter = true;
          success = processControlAssignmentTransmitter( assign, elem, ddi, name, destElem );
          break;

        default:
          ; // ignore invalid message.
        }

        if( success )
          sendProcMsg( IsoAgLib::ProcData::ControlAssignment, ddi, elem, int32_t( 0xfffffff0UL | uint32_t( transmitter ? TransmitterAck : ReceiverAck ) ) );
        else
          // we currently don't have a better error-code for NAME unknown/don't assign to TC or DL/don't reassign/etc., so....
          sendNackNotFound( ddi, elem, command, false );
      }
      break;

    default:
      ;
    }
  }


  void
  TcClientConnection_c::processProcMsg( const ProcessPkg_c& pkg )
  {
    const IsoItem_c* spValueSource = NULL; // will be set in the switch!

    // Additionally accept the message if from a valid Peer Control / Setpoint Value source!
    switch (pkg.men_command)
    {
      case IsoAgLib::ProcData::Value:
        spValueSource = mc_localCsHandler.getSetpointValueSource( pkg.mui16_element, pkg.mui16_DDI );

        // if no special Control Source is given, commands are only accepted from the connected TC
        if( spValueSource == NULL )
          spValueSource = getRemoteItem();
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
      case IsoAgLib::ProcData::ProcessDataAcknowledge:
      case IsoAgLib::ProcData::TaskControllerStatus:
      case IsoAgLib::ProcData::ClientTaskOrNotApplicable:
        // accept these messages only from the connected TC/DL.
        spValueSource = getRemoteItem();
    }

    if( pkg.getMonitorItemForSA() == spValueSource )
    {
      PdConnection_c::processProcMsg( pkg );

      if( pkg.getMonitorItemForDA() )
      {
        switch (pkg.men_command)
        {
          case IsoAgLib::ProcData::TechnicalData:
          case IsoAgLib::ProcData::DeviceDescriptor:
          case IsoAgLib::ProcData::ProcessDataAcknowledge:
          case IsoAgLib::ProcData::ControlAssignment:
            processMsgTc( pkg );
            break;
        
          default:
            ;
        }
      }
      // else: Global, not handled in a connection.
    }
  }


  void
  TcClientConnection_c::processMsgTc( const ProcessPkg_c& data )
  {
    if( getDevPoolState() == PoolStateDisconnected )
      return;

    /// HANDLE UNSOLICITED MESSAGES
    ///////////////////////////////
    // (not handling Peer Control Assignment here as it's 14-byte TP!)

    switch ( data.getUint8Data ( 0 ) )
    {
      case procCmdPar_RequestVersionMsg:
      {
        static const bool peerControl = true;
        sendMsg(
          procCmdPar_VersionMsg,
          m_capsClient.versionNr,
          0xFF, // client does not send boot-time information.
          (m_capsClient.hasTcBas ? 0x01:0) | (m_capsClient.hasTcGeo ? 0x02:0) | (m_capsClient.hasTcGeo ? 0x04:0)  | (peerControl ? 0x08:0) | (m_capsClient.hasTcSc ? 0x10:0),
          0x00, // optiones byte 2
          uint8_t( m_capsClient.numBoom ),
          uint8_t( m_capsClient.numSection ),
          uint8_t( m_capsClient.numBin ) );
      } break;
    }

    /// HANDLE RESPONSES
    ////////////////////

    if( m_currentCommand.getState() != CurrentCommand_c::FinishedAndWaitingForResponse )
      return;

    if ( !m_currentCommand.handleResponseMessage( data.getUint8Data( 0 ) ) )
      return;

    /// HANDLE RESPONSES
    ////////////////////
    switch ( data.getUint8Data ( 0 ) )
    {
      case procCmdPar_VersionMsg:
      {
        setDevPoolState( PoolStateAwaitingConnectionDecision );

        m_capsServer.versionNr = data.getUint8Data( 2-1 );

        m_capsServer.hasTcBas           = ( data.getUint8Data( 4 - 1 ) & 0x01 ) != 0;
        m_capsServer.hasTcGeoWithPbc    = ( data.getUint8Data( 4 - 1 ) & 0x02 ) != 0;
        m_capsServer.hasTcGeoWithoutPbc = ( data.getUint8Data( 4 - 1 ) & 0x04 ) != 0;
        m_capsServer.hasPeerControl     = ( data.getUint8Data( 4 - 1 ) & 0x08 ) != 0;
        m_capsServer.hasTcSc            = ( data.getUint8Data( 4 - 1 ) & 0x10 ) != 0;

        m_capsServer.numBoom    = data.getUint8Data( 6 - 1 );
        m_capsServer.numSection = data.getUint8Data( 7 - 1 );
        m_capsServer.numBin     = data.getUint8Data( 8 - 1 );

        m_capsConnection.versionNr      = ( m_capsServer.versionNr < m_capsClient.versionNr ) ? m_capsServer.versionNr : m_capsClient.versionNr;
        m_capsConnection.hasPeerControl = m_capsServer.hasPeerControl;

        m_stateHandler->_eventConnectionRequest( getIdentItem(), *connected(), m_capsServer );
        break;
      }

      case procCmdPar_StructureLabelMsg:
      {
        STL_NAMESPACE::vector<uint8_t> structureLabel;
        for( int i = 1; i < 8; i++ )
        {
          if( data.getUint8Data( i ) == 0xFF )
            continue;

          for ( int i = 1; i < 8; i++ )
            structureLabel.push_back( ( char )data.getUint8Data( i ) );
          break;
        }

        eventStructureLabelResponse( structureLabel );
      } break;

      case procCmdPar_LocalizationLabelMsg:
      {
        STL_NAMESPACE::vector<uint8_t> localizationLabel;
        for( int i = 1; i < 8; i++ )
        {
          if( data.getUint8Data( i ) == 0xFF )
            continue;

          for ( int i = 1; i < 8; i++ )
            localizationLabel.push_back( data.getUint8Data( i ) );
          break;
        }

        eventLocalizationLabelResponse( localizationLabel );
      } break;

      case procCmdPar_RequestOPTransferRespMsg:
        if ( data.getUint8Data( 1 ) == 0 ) // on success, send the object pool
          doCommand( procCmdPar_OPTransferMsg, procCmdPar_OPTransferRespMsg );
        else
          m_devPoolState = PoolStateError;
        break;

      case procCmdPar_OPTransferRespMsg:
        eventPoolUploadResponse( data.getUint8Data( 1 ) );
        break;

      case procCmdPar_OPActivateRespMsg:
        eventPoolActivateResponse( data.getUint8Data( 1 ) );
        break;

#if 0
// not supported yet
      case procCmdPar_ChangeDesignatorRespMsg:
        break;
#endif

      default:
        break;
    }
  }


  void TcClientConnection_c::eventTaskStartStop( bool started )
  {
    isoaglib_assert( m_stateHandler );

    if( getDevPoolState() == PoolStateActive )
    {
      if( started )
      {
        m_stateHandler->_eventTaskStarted( *this );
        // Application will need to reset totals to zero here!
      }
      else
      {
        stopRunningMeasurement();
        mc_localCsHandler.stop();

        m_stateHandler->_eventTaskStopped( *this );
      }
    }
  }


  void
  TcClientConnection_c::stopRunningMeasurement()
  {
    for( ConnectedPdMap_t::iterator i = m_connectedPds.begin(); i != m_connectedPds.end(); ++i )
      static_cast<MeasureProg_c *>( i->second )->stopAllMeasurements();
  }


  void
  TcClientConnection_c::eventStructureLabelResponse( const STL_NAMESPACE::vector<uint8_t>& label )
  {
    if( label.empty() )
    {
      setDevPoolState( PoolStateUploading ); // not present -> upload
    }
    else
    {
      DeviceObjectDvc_c* dvc = getDevicePool().getDvcObject();
      isoaglib_assert( dvc );
      const IsoAgLib::StructureLabel_s& strLbl = dvc->getStructureLabel();
      if ( STL_NAMESPACE::memcmp( ( void * )&strLbl, ( void * )&label[0], 7 ) != 0 )
      {
        setDevPoolState( PoolStateUploading ); // present, but wrong version -> upload
      }
      else
      {
        doCommand( procCmdPar_RequestLocalizationLabelMsg, procCmdPar_LocalizationLabelMsg );
      }
    }
  }


  void
  TcClientConnection_c::eventLocalizationLabelResponse( const STL_NAMESPACE::vector<uint8_t>& label )
  {
    if( label.empty() )
    {
      setDevPoolState( PoolStateUploading );
    }
    else
    {
      DeviceObjectDvc_c* dvc = getDevicePool().getDvcObject();
      isoaglib_assert( dvc );
      const IsoAgLib::Localization_s& locale = dvc->getLocalization();
      if ( STL_NAMESPACE::memcmp( ( void* )&locale, ( void * )&label[0], 7 ) != 0 ) 
      {
#if 0
        // updateLocale() doesn't do anything yet, so this is not a good solution for now
        getDevicePool().updateLocale();
        setDevPoolState(PoolStateUploaded);
#else
        // So, just upload a full DDOP when the localization label changes
        setDevPoolState(PoolStateUploading);
#endif
      }
      else
      {
        setDevPoolState(PoolStateUploaded);
      }
    }
  }


  void
  TcClientConnection_c::eventPoolUploadResponse( uint8_t result )
  {
    setDevPoolState( ( result == 0 )
                       ? PoolStateUploaded
                       : PoolStateError );

    getDevicePool().freeByteStreamBuffer( m_devicePoolToUpload.getBuffer() );
    m_devicePoolToUpload.setBuffer( NULL );
  }


  void
  TcClientConnection_c::eventPoolActivateResponse( uint8_t result ) {
    if ( result == 0 ) {
      if( getDevPoolState() == PoolStateUploaded )
      {
        setDevPoolState( PoolStateActive );
        if( connected()->getLastStatusTaskTotalsActive() )
          eventTaskStartStop( true );
      }
    } else {
      setDevPoolState( PoolStateError );
    }
  }


  void
  TcClientConnection_c::doCommand( ProcessDataMsg_t cmd, ProcessDataMsg_t expectedCmd, int32_t timeout, uint8_t param )
  {
    isoaglib_assert( (m_identItem != NULL) && connected() );

    switch( cmd )
    {
    case procCmdPar_OPTransferMsg:
      getMultiSendInstance4Comm().sendIsoTarget(
        m_identItem->isoName(),
        getRemoteName(),
        m_devicePoolToUpload.getBuffer(),
        m_devicePoolToUpload.getEnd(),
        PROCESS_DATA_PGN,
        &m_multiSendEventHandler );

      m_currentCommand.setRunningTp(cmd, expectedCmd, timeout);
      break;

    case procCmdPar_RequestOPTransferMsg:
      {
        const uint32_t byteStreamLength = m_devicePoolToUpload.getEnd() - 1; // -1 to remove 0x61 opcode

        sendMsg( cmd,
          ( uint8_t )( ( byteStreamLength       ) & 0xff ),
          ( uint8_t )( ( byteStreamLength >>  8 ) & 0xff ),
          ( uint8_t )( ( byteStreamLength >> 16 ) & 0xff ),
          ( uint8_t )( ( byteStreamLength >> 24 ) & 0xff ),
          0xff, 0xff, 0xff );

        m_currentCommand.sentSinglePkg( cmd, expectedCmd, timeout, connected()->getNotBusyCount());
      }
      break;

    default:
      {
        sendMsg( cmd, param, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff );

        m_currentCommand.sentSinglePkg( cmd, expectedCmd, timeout, connected()->getNotBusyCount());
      }
      break;
    }
  }




// MultiSendEventHandler_c implementation
  void
  TcClientConnection_c::reactOnStateChange( const SendStream_c& sendStream )
  {
    isoaglib_assert( connected() );
    if( !connected()->isAlive() )
      return;

    switch ( sendStream.getSendSuccess() )
    {
    case __IsoAgLib::SendStream_c::Running:
      break;

    case __IsoAgLib::SendStream_c::SendAborted:
      m_currentCommand.setAborted();
      break;

    case __IsoAgLib::SendStream_c::SendSuccess:
      // timeout counts from now on!
      m_currentCommand.sentMultiPkg( connected()->getNotBusyCount() );
      break;
    }
  }

// MultiSendStreamer_c implementation
  void
  TcClientConnection_c::setDataNextStreamPart( MultiSendPkg_c* data, uint8_t bytes ) {
    data->setDataPart ( m_devicePoolToUpload.getBuffer(), m_currentSendPosition, bytes );
    m_currentSendPosition += bytes;
  }

  void
  TcClientConnection_c::resetDataNextStreamPart() {
    m_currentSendPosition = 0;
  }

  void
  TcClientConnection_c::saveDataNextStreamPart() {
    m_storedSendPosition = m_currentSendPosition;
  }

  void
  TcClientConnection_c::restoreDataNextStreamPart() {
    m_currentSendPosition = m_storedSendPosition;
  }

  uint32_t
  TcClientConnection_c::getStreamSize() {
    return m_devicePoolToUpload.getEnd();
  }

  uint8_t
  TcClientConnection_c::getFirstByte() {
    return m_devicePoolToUpload[0];
  }


  ////////////////////////////////////////////////////////////////////////////////////////////
  /// ************************* TcClientConnection_c::cmdSend_c ************************** ///
  ////////////////////////////////////////////////////////////////////////////////////////////



  TcClientConnection_c::CurrentCommand_c::CurrentCommand_c()
    : m_cmdSendFinishedTimestamp( -1 )
    , m_state( Idle )
    , m_dataMessageSent( procCmdPar_RequestVersionMsg )
    , m_expectedResponse( procCmdPar_RequestVersionMsg )
    , m_cmdTimeout( 0 )
    , m_cmdTcNotBusyCountStart( 0 )
  {
  }

  TcClientConnection_c::CurrentCommand_c::CurrentCommand_c( const CurrentCommand_c& a_rhs )
    : m_cmdSendFinishedTimestamp( a_rhs.m_cmdSendFinishedTimestamp )
    , m_state( a_rhs.m_state )
    , m_dataMessageSent( a_rhs.m_dataMessageSent )
    , m_expectedResponse( a_rhs.m_expectedResponse )
    , m_cmdTimeout( a_rhs.m_cmdTimeout )
    , m_cmdTcNotBusyCountStart( a_rhs.m_cmdTcNotBusyCountStart )
  {
  }

  void
  TcClientConnection_c::CurrentCommand_c::init()
  {
    m_state = Idle;
  }

  void
  TcClientConnection_c::CurrentCommand_c::setRunningTp(ProcessDataMsg_t cmd, ProcessDataMsg_t expectedResponse, int32_t timeout)
  {
    m_cmdSendFinishedTimestamp = -1;
    m_state = Running;
    m_dataMessageSent = cmd;
    m_expectedResponse = expectedResponse;
    m_cmdTimeout = timeout;
  }

  void
  TcClientConnection_c::CurrentCommand_c::sentSinglePkg(ProcessDataMsg_t cmd, ProcessDataMsg_t expectedResponse, int32_t timeout, uint32_t busyCount)
  {
    m_cmdSendFinishedTimestamp = HAL::getTime();
    m_state = FinishedAndWaitingForResponse;
    m_dataMessageSent = cmd;
    m_expectedResponse = expectedResponse;
    m_cmdTimeout = timeout;
    m_cmdTcNotBusyCountStart = busyCount;
  }

  void
  TcClientConnection_c::CurrentCommand_c::sentMultiPkg(uint32_t busyCount)
  {
    m_cmdSendFinishedTimestamp = HAL::getTime();
    m_state = FinishedAndWaitingForResponse;
    m_cmdTcNotBusyCountStart = busyCount;
  }

  void
  TcClientConnection_c::CurrentCommand_c::setAborted()
  {
    m_cmdSendFinishedTimestamp = HAL::getTime();
    m_state = Aborted; }

  void
  TcClientConnection_c::CurrentCommand_c::finished()
  {
    m_state = Idle;
  }

  TcClientConnection_c::CurrentCommand_c::State_en
  TcClientConnection_c::CurrentCommand_c::getState() const
  { 
    return ( m_state );
  }

  bool
  TcClientConnection_c::CurrentCommand_c::isTimedOut( ServerInstance_c& a_connected )
  {
    isoaglib_assert( m_state == FinishedAndWaitingForResponse );
    isoaglib_assert( m_cmdSendFinishedTimestamp != -1 );

    // most commands do not have a time-out. 
    if ( m_cmdTimeout > 0)
    {
      return ( isDefinedTimeoutTriggered() );
    }
    else
    {
      const uint32_t notBusyCount = a_connected.getNotBusyCount();

      if ( notBusyCount < m_cmdTcNotBusyCountStart )
        m_cmdTcNotBusyCountStart = notBusyCount;

      // There DEF_MaxTcNotBusyCount consec. 0-bits for Busy need to be in the TC Status message.
      return ( notBusyCount > ( m_cmdTcNotBusyCountStart + DEF_MaxTcNotBusyCount ) );
    }
  }

  bool
  TcClientConnection_c::CurrentCommand_c::handleResponseMessage( uint8_t response )
  {
    isoaglib_assert( m_state == FinishedAndWaitingForResponse );
    const bool retValue = (response == m_expectedResponse);
    if ( retValue )
      finished();
    return ( retValue );
  }

  TcClientConnection_c::ProcessDataMsg_t
  TcClientConnection_c::CurrentCommand_c::getLastSentCommand() const
  {
    return ( m_dataMessageSent );
  }

  bool
  TcClientConnection_c::CurrentCommand_c::isDefinedTimeoutTriggered() const
  {
    isoaglib_assert( m_cmdSendFinishedTimestamp != -1 );
    return ( HAL::getTime() > ( m_cmdSendFinishedTimestamp + m_cmdTimeout ) );
  }



  ////////////////////////////////////////////////////////////////////////////////////////////
  /// ******************* TcClientConnection_c::ControlSourceHandler_c ******************* ///
  ////////////////////////////////////////////////////////////////////////////////////////////



  TcClientConnection_c::ControlSourceHandler_c::ControlSourceHandler_c( TcClientConnection_c &tcClientConnection )
    : SchedulerTask_c( 3000, false ) // dummy period, not using period, always triggering by nextTime!
    , m_tcClientConnection( tcClientConnection )
  {}


  void
  TcClientConnection_c::ControlSourceHandler_c::stop()
  {
    for( SpValSourceListIter iter = m_listSpValueSource.begin(); iter != m_listSpValueSource.end(); )
    {
      iter = doRemoveSetpointValueSource( iter );
    }
  }


  const IsoItem_c*
  TcClientConnection_c::ControlSourceHandler_c::getSetpointValueSource( uint16_t _element, uint16_t _ddi )
  {
    for( SpValSourceListIter iter = m_listSpValueSource.begin(); iter != m_listSpValueSource.end(); ++iter )
    {
      if( ( iter->element == _element )
        &&( iter->ddi == _ddi ) )
        return &iter->controlSource.getIsoItem();
    }

    return NULL;
  }


  TcClientConnection_c::ControlSourceHandler_c::SpValSourceListIter
  TcClientConnection_c::ControlSourceHandler_c::findSetpointValueSource( uint16_t _element, uint16_t _ddi, const IsoItem_c& isoItem )
  {
    SpValSourceListIter iter = m_listSpValueSource.begin();

    for( ; iter != m_listSpValueSource.end(); ++iter )
    {
      if( ( iter->element == _element )
        &&( iter->ddi == _ddi )
        &&( &iter->controlSource.getIsoItem() == &isoItem ) )
        break;
    }

    return iter;
  }

  
  TcClientConnection_c::ControlSourceHandler_c::SpValSourceListIter
  TcClientConnection_c::ControlSourceHandler_c::findControlSource( const IsoItem_c& isoItem )
  {
    SpValSourceListIter iter = m_listSpValueSource.begin();

    for( ; iter != m_listSpValueSource.end(); ++iter )
    {
      if( &iter->controlSource.getIsoItem() == &isoItem )
        break;
    }

    return iter;
  }


        
  bool
  TcClientConnection_c::ControlSourceHandler_c::addSetpointValueSource( uint16_t _element, uint16_t _ddi, const IsoItem_c& isoItem )
  {
    switch( isoItem.isoName().getEcuType() )
    {
    case IsoName_c::ecuTypeTaskControl:
    case IsoName_c::ecuTypeDataLogger:
      // Do not assign to a TC/DL; only to PEERs!
      return false;

    default:
      ;
    }

    if( getSetpointValueSource( _element, _ddi ) )
      return false; // currently no REassignments. (require unassign first!)

#if 0
    "** This kind of code would only be needed if we'd remove the gottenSetpointValueSource above and reassign to another. Let's wait what standard/certification will say..."
    // was it already added/assigned?
    if( findSetpointValueSource( _element, _ddi, isoItem ) != m_listSpValueSource.end() )
      return false; // @todo is this correct: right now -> reject!
#endif

    SpValSourceListIter iter = findControlSource( isoItem );
    PdRemoteNode_c& cs = ( iter == m_listSpValueSource.end() )
      ? getTcClientInstance( m_tcClientConnection.getMultitonInst() ).addConnectionToControlSource( m_tcClientConnection, isoItem )
      : iter->controlSource;

    m_listSpValueSource.push_back( SetpointValueSource_s( _element, _ddi, cs ) );

    recalcTimeEventTrigger();
    return true;
  }

  
  bool
  TcClientConnection_c::ControlSourceHandler_c::removeSetpointValueSource( uint16_t _element, uint16_t _ddi, const IsoItem_c& _isoItem )
  {
    SpValSourceListIter iter = findSetpointValueSource( _element, _ddi, _isoItem ) ;
    if( iter != m_listSpValueSource.end() )
    {
      ( void )doRemoveSetpointValueSource( iter );
      
      recalcTimeEventTrigger();
      return true;
    }

    return false;
  }

  TcClientConnection_c::ControlSourceHandler_c::SpValSourceListIter
  TcClientConnection_c::ControlSourceHandler_c::doRemoveSetpointValueSource( SpValSourceListIter iter )
  {
    isoaglib_assert( iter != m_listSpValueSource.end() );

    const IsoItem_c& _isoItem = iter->controlSource.getIsoItem();
    
    SpValSourceListIter result = m_listSpValueSource.erase( iter );

    if( findControlSource( _isoItem ) == m_listSpValueSource.end() )
      getTcClientInstance( m_tcClientConnection.getMultitonInst() ).removeConnectionFromControlSource( m_tcClientConnection, _isoItem );

    return result;
  }

  
  void 
  TcClientConnection_c::ControlSourceHandler_c::notifyOnPeerDropOff( PdRemoteNode_c& pdRemoteNode )
  {
    bool atLeastOneSpValueSourceWasConnected = false;

    for( SpValSourceListIter iter = m_listSpValueSource.begin(); iter != m_listSpValueSource.end(); )
    {
      const IsoItem_c& isoItem = iter->controlSource.getIsoItem();
      if( &pdRemoteNode.getIsoItem() == &isoItem )
      {
        atLeastOneSpValueSourceWasConnected = true;
        iter = m_listSpValueSource.erase( iter );
      }
      else
        ++iter;
    }

    if( atLeastOneSpValueSourceWasConnected )
      pdRemoteNode.removeConnection( m_tcClientConnection );

    recalcTimeEventTrigger();
  }


  void TcClientConnection_c::ControlSourceHandler_c::recalcTimeEventTrigger()
  {
    if( m_listSpValueSource.empty() )
    {
      if( SchedulerTask_c::isRegistered() )
        getSchedulerInstance().deregisterTask( *this );
    }
    else
    {
      if( !SchedulerTask_c::isRegistered() )
        getSchedulerInstance().registerTask( *this, 0 );

      SpValSourceListIter iter = m_listSpValueSource.begin(); 
      ecutime_t oldestOverallReceiveTime = iter->m_lastReceivedTime;
      ++iter;

      for( ; iter != m_listSpValueSource.end(); ++iter )
      {
        if( iter->m_lastReceivedTime < oldestOverallReceiveTime )
          oldestOverallReceiveTime = iter->m_lastReceivedTime;
      }

      SchedulerTask_c::setNextTriggerTime( oldestOverallReceiveTime + 3000 );
    }
  }


  void TcClientConnection_c::ControlSourceHandler_c::timeEvent()
  {
    for( SpValSourceListIter iter = m_listSpValueSource.begin(); iter != m_listSpValueSource.end(); )
    {
      if( HAL::getTime() > ( iter->m_lastReceivedTime + 3000 ) )
        iter = doRemoveSetpointValueSource( iter );
      else
        ++iter;
    }

    recalcTimeEventTrigger();
  }


}
