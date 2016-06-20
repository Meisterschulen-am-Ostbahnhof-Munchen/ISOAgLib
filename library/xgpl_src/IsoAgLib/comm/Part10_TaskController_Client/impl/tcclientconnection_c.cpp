/*
  tcclientconnection_c.cpp: class for managing a client/server connection
                            to TC/DL/Proprietary Servers

  (C) Copyright 2009 - 2015 by OSB AG and developing partners

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
    , m_currentSendPosition( 0 )
    , m_storedSendPosition( 0 )
    , m_devicePoolToUpload()
    , m_timeWsAnnounceKey( -1 )
    , m_timeWaitWithAnyCommunicationUntil( -1 )
    , m_timeLastWsTaskMsgSent( -1 )
    , m_stateHandler( NULL )
    , m_devPoolState( PoolStateDisconnected )
    , m_cmdState( CommandStateNone )
    , m_cmdSent( 0x00 )
    , m_cmdSentTimestamp( 0 )
    , m_cmdTimeout( 0 )
    , m_capabilities()
    , m_schedulerTaskProxy( *this, 100, false )
  {
  }


  TcClientConnection_c::TcClientConnection_c( const TcClientConnection_c &rhs )
    : PdConnection_c( rhs )
    , m_multiSendEventHandler( *this )
    , m_multiSendStreamer( *this )
    , m_currentSendPosition( rhs.m_currentSendPosition )
    , m_storedSendPosition( rhs.m_storedSendPosition )
    , m_devicePoolToUpload( rhs.m_devicePoolToUpload )
    , m_timeWsAnnounceKey( rhs.m_timeWsAnnounceKey )
    , m_timeWaitWithAnyCommunicationUntil( rhs.m_timeWaitWithAnyCommunicationUntil )
    , m_timeLastWsTaskMsgSent( rhs.m_timeLastWsTaskMsgSent )
    , m_stateHandler( rhs.m_stateHandler )
    , m_devPoolState( rhs.m_devPoolState )
    , m_cmdState( rhs.m_cmdState )
    , m_cmdSent( rhs.m_cmdSent )
    , m_cmdSentTimestamp( rhs.m_cmdSentTimestamp )
    , m_cmdTimeout( rhs.m_cmdTimeout )
    , m_capabilities( rhs.m_capabilities )
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
    m_capabilities = capabilities;

    getSchedulerInstance().registerTask( m_schedulerTaskProxy, 0 );

    m_cmdState = CommandStateNone;
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

    m_cmdState = CommandStateNone;
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

    doCommand( procCmdPar_RequestStructureLabelMsg );

    setDevPoolState( PoolStateConnecting );

    return true;
  }


  void TcClientConnection_c::disconnect( bool shouldDeletePool )
  {
    isoaglib_assert( connected() );

    getMultiSendInstance( m_identItem->getMultitonInst() ).abortSend( m_multiSendEventHandler );

    // is it worth sending a proper "I'm disconnecting" message??
    if( connected()->isAlive() )
    {
      if( getDevPoolState() == PoolStateUploaded || getDevPoolState() == PoolStateActive )
      {
        doCommand( procCmdPar_OPActivateMsg, 1500, 0 );

        if (shouldDeletePool)
        {
          doCommand( procCmdPar_OPDeleteMsg, 1500 );
        }
      }
    }
  
    getSchedulerInstance().deregisterTask( m_schedulerTaskProxy );

    stopRunningMeasurement();

    if (connected()->getLastStatusTaskTotalsActive())
      eventTaskStopped();

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

    switch( m_cmdState )
    {
    case CommandStateNone:
      switch ( getDevPoolState() )
      {
      case PoolStatePreconnecting:
        doCommand( procCmdPar_RequestVersionMsg );
        break;

      case PoolStateAwaitingConnectionDecision:
        // Well yeah, wait for the application to finally decide and in case of connecting, have its DDOP ready...
        break;

      case PoolStateUploading:
        m_devicePoolToUpload = getDevicePool().getBytestream( procCmdPar_OPTransferMsg );
        doCommand( procCmdPar_RequestOPTransferMsg );
        break;

      /*case PoolStateStale: {
        // Upload changed descriptions
        STL_NAMESPACE::vector<uint8_t> newBytes;
        if (getDevicePool().getDirtyBytestream(newBytes))
        {
          m_devicePoolToUpload = newBytes
          doCommand( procCmdPar_OPTransferMsg );
        }
        else
          setDevPoolState(PoolStateActive);
        // @todo trigger re-announce for fresh pool upload with changed objects
      } break;*/

      case PoolStateUploaded:
        doCommand( procCmdPar_OPActivateMsg );
        break;

      case PoolStateActive:
        //if ( getDevicePool().isDirty() )
        //  setDevPoolState(PoolStateStale);
        // @todo trigger re-announce for fresh pool upload with changed objects
        break;

      case PoolStateError:
        getTcClientInstance( getIdentItem().getMultitonInst() ).notifyConnectionToBeEnded( *this );
        break;

      default:
        break;
      }
      break;

    case CommandStateWaitingForResponse:
      // wait for (E)TP to finished before checking timeouts?
      if( m_cmdSentTimestamp < 0 )
        break;

      if( HAL::getTime() > ( m_cmdSentTimestamp + m_cmdTimeout ) )
      {
        // Timeout, retry last command?
        // For now, give up!
        m_cmdState = CommandStateNone;
        // don't go to PoolStateError, as it cannot be detected then anymore if it was still in Preconnecting or Connected.
        getTcClientInstance( getIdentItem().getMultitonInst() ).notifyConnectionToBeEnded( *this );
      }
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


  void
  TcClientConnection_c::processMsgTc( const ProcessPkg_c& data )
  {
    if( getDevPoolState() == PoolStateDisconnected )
      return;

    /// HANDLE UNSOLICITED MESSAGES
    ///////////////////////////////

    switch ( data.getUint8Data ( 0 ) )
    {
      case procCmdPar_RequestVersionMsg:
      {
        static const bool peerControl = false;
        sendMsg(
          procCmdPar_VersionMsg,
          m_capabilities.versionNr,
          0xFF,
          (m_capabilities.hasTcBas ? 0x01:0) | (m_capabilities.hasTcGeo ? 0x02:0) | (m_capabilities.hasTcGeo ? 0x04:0)  | (peerControl ? 0x08:0) | (m_capabilities.hasTcSc ? 0x10:0),
          0x00,
          uint8_t( m_capabilities.numBoom ),
          uint8_t( m_capabilities.numSection ),
          uint8_t( m_capabilities.numBin ) );
      } break;
    }

    /// HANDLE RESPONSES
    ////////////////////

    if( m_cmdState != CommandStateWaitingForResponse )
      return;

    /// HANDLE RESPONSES
    ////////////////////
    switch ( data.getUint8Data ( 0 ) )
    {
      case procCmdPar_VersionMsg:
      {
        if( m_cmdSent != procCmdPar_RequestVersionMsg )
          break;
        m_cmdState = CommandStateNone;

        setDevPoolState( PoolStateAwaitingConnectionDecision );

        IsoAgLib::ProcData::ServerCapabilities_s caps;
        caps.versionNr = data.getUint8Data( 2-1 );

        caps.hasTcBas           = ( data.getUint8Data( 4-1 ) & 0x01 ) != 0;
        caps.hasTcGeoWithPbc    = ( data.getUint8Data( 4-1 ) & 0x02 ) != 0;
        caps.hasTcGeoWithoutPbc = ( data.getUint8Data( 4-1 ) & 0x04 ) != 0;
        caps.hasPeerControl     = ( data.getUint8Data( 4-1 ) & 0x08 ) != 0;
        caps.hasTcSc            = ( data.getUint8Data( 4-1 ) & 0x10 ) != 0;

        caps.numBoom    = data.getUint8Data( 6-1 );
        caps.numSection = data.getUint8Data( 7-1 );
        caps.numBin     = data.getUint8Data( 8-1 );

        m_stateHandler->_eventConnectionRequest( getIdentItem(), *connected(), caps );
        break;
      }

      case procCmdPar_StructureLabelMsg:
      {
        if( m_cmdSent != procCmdPar_RequestStructureLabelMsg )
          break;
        m_cmdState = CommandStateNone;

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
        if( m_cmdSent != procCmdPar_RequestLocalizationLabelMsg )
          break;
        m_cmdState = CommandStateNone;

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
        if( m_cmdSent != procCmdPar_RequestOPTransferMsg )
          break;
        m_cmdState = CommandStateNone;

        if ( data.getUint8Data( 1 ) == 0 ) // on success, send the object pool
          doCommand( procCmdPar_OPTransferMsg );
        else
          m_devPoolState = PoolStateError;
        break;

      case procCmdPar_OPTransferRespMsg:
        if( m_cmdSent != procCmdPar_OPTransferMsg )
          break;
        m_cmdState = CommandStateNone;

        eventPoolUploadResponse( data.getUint8Data( 1 ) );
        break;

      case procCmdPar_OPActivateRespMsg:
        if( m_cmdSent != procCmdPar_OPActivateMsg )
          break;
        m_cmdState = CommandStateNone;

        eventPoolActivateResponse( data.getUint8Data( 1 ) );
        break;

#if 0
// not supported yet
      case procCmdPar_ChangeDesignatorRespMsg:
        if( m_cmdSent != procCmdPar_ChangeDesignatorMsg )
          break;
        m_cmdState = CommandStateNone;

        break;
#endif

      default:
        break;
    }
  }


  void TcClientConnection_c::eventTaskStarted() {
    isoaglib_assert( m_stateHandler );
    if( getDevPoolState() == PoolStateActive )
      m_stateHandler->_eventTaskStarted( *this );
  }


  void TcClientConnection_c::eventTaskStopped() {
    isoaglib_assert( m_stateHandler );
    if( getDevPoolState() == PoolStateActive )
      m_stateHandler->_eventTaskStopped( *this );
  }


  void
  TcClientConnection_c::stopRunningMeasurement() {
    for( ConnectedPdMap_t::iterator i = m_connectedPds.begin(); i != m_connectedPds.end(); ++i ) {
      static_cast<MeasureProg_c *>( i->second )->stopAllMeasurements();
    }
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
        doCommand( procCmdPar_RequestLocalizationLabelMsg );
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
  TcClientConnection_c::eventPoolUploadResponse( uint8_t result ) {
    if ( result == 0 ) {
      setDevPoolState( PoolStateUploaded );
      getDevicePool().freeByteStreamBuffer( m_devicePoolToUpload.getBuffer() );
    } else {
      setDevPoolState( PoolStateError );
    }
  }


  void
  TcClientConnection_c::eventPoolActivateResponse( uint8_t result ) {
    if ( result == 0 ) {
      if( getDevPoolState() == PoolStateUploaded )
      {
        setDevPoolState( PoolStateActive );
        if( connected()->getLastStatusTaskTotalsActive() )
          eventTaskStarted();
      }
    } else {
      setDevPoolState( PoolStateError );
    }
  }


  void
  TcClientConnection_c::doCommand( ProcessDataMsg_t cmd, int32_t timeout, uint8_t param )
  {
    isoaglib_assert( (m_identItem != NULL) && connected() );

    m_cmdState = CommandStateWaitingForResponse;
    m_cmdSent = cmd;
    m_cmdTimeout = timeout;
    m_cmdSentTimestamp = HAL::getTime();

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

      // Special for (E)TP Commands: Will be set after sent out completely (EoMAck received)
      m_cmdSentTimestamp = -1;
      break;

    case procCmdPar_RequestOPTransferMsg:
      {
        const uint32_t byteStreamLength = m_devicePoolToUpload.getEnd() - 1; // -1 to remove 0x61 opcode

        sendMsg( cmd,
          ( uint8_t )( ( byteStreamLength ) & 0xff ),
          ( uint8_t )( ( byteStreamLength >> 8 ) & 0xff ),
          ( uint8_t )( ( byteStreamLength >> 16 ) & 0xff ),
          ( uint8_t )( ( byteStreamLength >> 24 ) & 0xff ),
          0xff, 0xff, 0xff );
      }
      break;

    default:
      {
        ProcessPkg_c pkg( cmd, param, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF );

        pkg.setMonitorItemForDA( const_cast<IsoItem_c*>( getRemoteItem() ) );
        pkg.setMonitorItemForSA( m_identItem->getIsoItem() );

        getIsoBusInstance4Comm() << pkg;
      }
      break;
    }
  }




// MultiSendEventHandler_c implementation
  void
  TcClientConnection_c::reactOnStateChange( const SendStream_c& sendStream )
  {
    if( !connected()->isAlive() )
      return;

    switch ( sendStream.getSendSuccess() )
    {
    case __IsoAgLib::SendStream_c::Running:
      break;

    case __IsoAgLib::SendStream_c::SendAborted:
      doCommand( procCmdPar_OPTransferMsg );
      break;

    case __IsoAgLib::SendStream_c::SendSuccess:
      // timeout counts from now on!
      m_cmdSentTimestamp = HAL::getTime();
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


}; // __IsoAgLib
