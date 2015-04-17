/*
  tcclientconnection_c.cpp: class for managing a TC-client/server connection

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
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measureprog_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/idevicepool_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif



namespace __IsoAgLib {

  TcClientConnection_c::TcClientConnection_c( const IdentItem_c& identItem, StateHandler_c& sh, ServerInstance_c& server, DevicePool_c& pool )
    : PdConnection_c( identItem, &server, pool )
    , m_multiSendEventHandler( *this )
    , m_multiSendStreamer( *this )
    , m_timeWsAnnounceKey( -1 )
    , m_stateHandler( &sh )
    , m_currentSendPosition( 0 )
    , m_storedSendPosition( 0 )
    , m_devicePoolToUpload()
    , m_uploadPoolState( DDOPRegistered )
    , m_uploadState( StateIdle )
    , m_uploadStep( UploadNone )
    , m_uploadTimestamp( 0 )
    , m_uploadTimeout( 0 )
    //, m_commandParameter()
    , m_initDone( false )
    , m_tcAliveNew( false )
    , m_timeStartWaitAfterAddrClaim( -1 )
    , m_timeWsTaskMsgSent( -1 )
    , m_structureLabel()
    , m_localizationLabel()
    , m_sendSuccess( SendStream_c::SendSuccess )
    , m_devPoolState( PoolStateInit )
    , m_devPoolAction( PoolActionIdle )
    , m_schedulerTaskProxy( *this, 100, false )
  {
    getDevicePool().init( getIdentItem() );

    getSchedulerInstance().registerTask( m_schedulerTaskProxy, 0 );
  }


  TcClientConnection_c::~TcClientConnection_c()
  {
    getSchedulerInstance().deregisterTask( m_schedulerTaskProxy );

    // TODO: send deactivate msg

    stopRunningMeasurement();

    if (getServer().getLastActiveTaskTC())
      eventTaskStopped();

    getDevicePool().close();
  }

  
  void
  TcClientConnection_c::timeEvent( void ) {
    if ( !m_identItem.isClaimedAddress() )
      return;

    // Wait until we are properly initialized before doing anything else
    if ( ! m_initDone ) {
      // Address claim is complete. Set time at which our address was claimed if it is
      // still in an initialized state    
      
      if ( m_timeStartWaitAfterAddrClaim == -1 ) {
        m_timeStartWaitAfterAddrClaim = HAL::getTime();
        return;
      }

      // init is finished when more then 6sec after addr claim and at least one TC status message was received
      if ( ( HAL::getTime() - m_timeStartWaitAfterAddrClaim >= 6000 ) && ( getServer().getLastStatusTime() != -1 ) )
      {
        m_timeWsAnnounceKey = m_identItem.getIsoItem()->startWsAnnounce();
        m_initDone = true;
      }
      return;
    }

    // Check if the working-set is completely announced
    if ( !m_identItem.getIsoItem()->isWsAnnounced ( m_timeWsAnnounceKey ) )
      return;

    // Send the WS task message to maintain connection with the TC
    const int32_t now = HAL::getTime();
    if ( now - m_timeWsTaskMsgSent >= 2000 ) {
      m_timeWsTaskMsgSent = now;
      sendMsg( 0xff, 0xff, 0xff, 0xff, getServer().getLastServerState(), 0x00, 0x00, 0x00 );
    }

    timeEventDevicePool();
  }


  void
  TcClientConnection_c::timeEventDevicePool() {
    if ( m_uploadPoolState == DDOPCannotBeUploaded )
      return;

    switch( m_uploadStep ) {
      case UploadUploading:
        if ( ( ( uint32_t ) HAL::getTime() ) > ( m_uploadTimeout + m_uploadTimestamp ) ) {
          startUpload();
          break;
        }
        switch ( m_sendSuccess ) {
          case __IsoAgLib::SendStream_c::Running:
            break;
          case __IsoAgLib::SendStream_c::SendAborted:
            // aborted sending
            m_uploadState = StateIdle;
            m_uploadStep = UploadFailed;
            break;

            // when the pool has been sent successfully, wait on the transfer response
          case __IsoAgLib::SendStream_c::SendSuccess:
            m_uploadStep = UploadWaitForOPTransferResponse;
            m_uploadTimeout = DEF_TimeOut_NormalCommand;
            break;
        }
        break;

        // Waiting for request to transfer response
      case UploadWaitForOPTransferResponse:
        if ( ( ( uint32_t ) HAL::getTime() ) > ( m_uploadTimeout + m_uploadTimestamp ) )
          startUpload();
        break;

      case UploadFailed:
        //m_commandParameter = procCmdPar_OPTransferMsg;
        m_uploadTimestamp = HAL::getTime();
        m_uploadTimeout = DEF_WaitFor_Reupload;
        break;

      case UploadNone:
      default:
        break;
    }

    if ( m_devPoolAction == PoolActionIdle ) {
        switch ( getDevPoolState() ) {
          case PoolStateActive:
            //if ( getDevicePool().isDirty() )
            //  setDevPoolState(PoolStateStale);
            // @todo trigger re-announce for fresh pool upload with changed objects
            break;
          case PoolStateInit:
            // Initialization state
            //	Retrieve the structure label from the current pool
            if ( getDevicePool().isEmpty() )
              break;

            setDevPoolState( PoolStatePresetting );
            doCommand( procCmdPar_RequestStructureLabelMsg );
            setDevPoolAction( PoolActionWaiting );
            break;

//      case  PoolStateError:
//        setDevPoolState( PoolStateInit );
//        m_PoolAction = PoolActionIdle;
//        break;

          case PoolStateInvalid:
            // Pool is present, but it is the wrong version
            doCommand( procCmdPar_OPDeleteMsg );
            setDevPoolAction( PoolActionWaiting );
            break;

          case PoolStateNotPresent:
            // No device pool present - upload it
            if ( getDevicePool().isEmpty() )
              break;

            m_devicePoolToUpload = getDevicePool().getBytestream( procCmdPar_OPTransferMsg );
            startUpload();
            setDevPoolAction( PoolActionWaiting );
            break;

          case PoolStateStale: {
              // Upload changed descriptions
              /*STL_NAMESPACE::vector<uint8_t> newBytes;
              if (getDevicePool().getDirtyBytestream(newBytes))
              {
                m_devicePoolToUpload = newBytes
                startUpload();
                setDevPoolAction( PoolActionWaiting );
              }
              else
                setDevPoolState(PoolStateActive);*/
              // @todo trigger re-announce for fresh pool upload with changed objects
            }
            break;

          case PoolStateUploaded:
            doCommand( procCmdPar_OPActivateMsg );
            setDevPoolAction( PoolActionWaiting );
            break;

          default:
            break;
        }
      }
  }


  void
  TcClientConnection_c::startUpload() {
    const uint32_t byteStreamLength = m_devicePoolToUpload.getEnd() - 1; // -1 to remove 0x61 opcode

    sendMsg( procCmdPar_RequestOPTransferMsg, ( uint8_t )( ( byteStreamLength ) & 0xff ),
             ( uint8_t )( ( byteStreamLength >> 8 ) & 0xff ), ( uint8_t )( ( byteStreamLength >> 16 ) & 0xff ),
             ( uint8_t )( ( byteStreamLength >> 24 ) & 0xff ), 0xff, 0xff, 0xff );

    m_uploadTimestamp = HAL::getTime();
    m_uploadTimeout = DEF_TimeOut_OPTransfer;

    m_uploadState = StateBusy;
    m_uploadStep = UploadNone;
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
    pkg.setMonitorItemForSA( m_identItem.getIsoItem() );

    getIsoBusInstance4Comm() << pkg;
  }


  void
  TcClientConnection_c::processMsgTc( const ProcessPkg_c& data )
  {
    // handling of NACK
    //  means that no device description is uploaded before
    //  so extract all necessary information from the byte-stream (structure and localization label)
    //  add the stream to the map
    if ( ( data.getUint8Data( 0 ) & 0xF ) == 0xD ) {
      //here starts nack handling
      // these two cmds could be answered with a NACK
      switch ( ( data.getUint8Data( 0 ) >> 4 ) & 0xF ) {
        case 0x0: //NACK Request StructureLabel
          m_uploadState = StateIdle;
          m_structureLabel.clear();
          eventStructureLabelResponse( 1, m_structureLabel );
          break;
        case 0x2: //NACK Request LocalizationLabel
          m_uploadState = StateIdle;
          m_localizationLabel.clear();
          eventLocalizationLabelResponse( 1, m_localizationLabel );
          break;
        default:
          break;
      }
    }

    switch ( data.getUint8Data ( 0 ) ) {
      case procCmdPar_VersionMsg:
        m_uploadState = StateIdle;
        break;

      case procCmdPar_StructureLabelMsg:
        // command is always successful if not NACK'd above
        m_structureLabel.clear();
        for ( int i = 1; i < 8; i++ )
          m_structureLabel.push_back( ( char )data.getUint8Data( i ) );

        m_uploadState = StateIdle;
        m_uploadStep = UploadNone;

        eventStructureLabelResponse( 0, m_structureLabel );
        break;

      case procCmdPar_LocalizationLabelMsg:
        // command is always successful if not NACK'd above
        m_localizationLabel.clear();
        for ( int i = 1; i < 8; i++ )
          m_localizationLabel.push_back( data.getUint8Data( i ) );

        m_uploadState = StateIdle;
        m_uploadStep = UploadNone;
        eventLocalizationLabelResponse( 0, m_localizationLabel );
        break;

      case procCmdPar_RequestOPTransferRespMsg:
        if ( data.getUint8Data( 1 ) == 0 ) { // on success, send the object pool
          m_sendSuccess = SendStream_c::SendSuccess;
          getMultiSendInstance4Comm().sendIsoTarget( m_identItem.isoName(), getRemoteName(), m_devicePoolToUpload.getBuffer(),
              m_devicePoolToUpload.getEnd(), PROCESS_DATA_PGN, &m_multiSendEventHandler );

          m_uploadState = StateBusy;
          m_uploadStep = UploadUploading;
        } else
          outOfMemory();
        break;

      case procCmdPar_OPTransferRespMsg:
        m_uploadState = StateIdle;
        m_uploadStep = UploadNone;
        eventPoolUploadResponse( data.getUint8Data( 1 ) );
        break;

      case procCmdPar_OPActivateRespMsg:
        m_uploadState = StateIdle;
        m_uploadStep = UploadNone;
        eventPoolActivateResponse( data.getUint8Data( 1 ) );
        break;

      case procCmdPar_OPDeleteRespMsg:
        m_uploadState = StateIdle;
        m_uploadStep = UploadNone;
        eventPoolDeleteResponse( data.getUint8Data( 1 ) );
        break;

      case procCmdPar_ChangeDesignatorRespMsg:
        m_uploadState = StateIdle;
        m_uploadStep = UploadNone;
        break;
      default:
        break;
    }
  }


  void
  TcClientConnection_c::outOfMemory() {
    m_uploadStep = UploadFailed;
    m_uploadPoolState = DDOPCannotBeUploaded;
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
  TcClientConnection_c::eventStructureLabelResponse( uint8_t result, const STL_NAMESPACE::vector<uint8_t>& label ) {
    if ( result == 0 && !label.empty() ) {
      DeviceObjectDvc_c* dvc = getDevicePool().getDvcObject();
      if ( dvc ) {
        const IsoAgLib::StructureLabel_s& strLbl = dvc->getStructureLabel();
        if ( STL_NAMESPACE::memcmp( ( void * )&strLbl, ( void * )&label[0], 7 ) != 0 ) {
          setDevPoolState( PoolStateInvalid );
        }
      }
    } else {
      //m_pool.setDirty();
      setDevPoolState( PoolStateNotPresent );
    }

    if ( getDevPoolState() == PoolStatePresetting ) {
      doCommand( procCmdPar_RequestLocalizationLabelMsg );
      setDevPoolAction( PoolActionWaiting );
    } else
      setDevPoolAction( PoolActionIdle );
  }


  void
  TcClientConnection_c::eventLocalizationLabelResponse( uint8_t result, const STL_NAMESPACE::vector<uint8_t>& label ) {
    if ( result == 0 && !label.empty() ) {
      DeviceObjectDvc_c* dvc = getDevicePool().getDvcObject();
      if ( dvc ) {
        const IsoAgLib::Localization_s& locale = dvc->getLocalization();
        if ( STL_NAMESPACE::memcmp( ( void* )&locale, ( void * )&label[0], 7 ) != 0 ) {
          getDevicePool().updateLocale();
        }
        setDevPoolState( PoolStateUploaded );
      }
    } else {
      //m_pool.setDirty();
      setDevPoolState( PoolStateNotPresent );
    }

    setDevPoolAction( PoolActionIdle );
  }


  void
  TcClientConnection_c::eventPoolUploadResponse( uint8_t result ) {
    if ( result == 0 ) {
      setDevPoolState( PoolStateUploaded );
      getDevicePool().freeByteStreamBuffer( m_devicePoolToUpload.getBuffer() );
    } else {
      setDevPoolState( PoolStateError );
    }

    setDevPoolAction( PoolActionIdle );
  }


  void
  TcClientConnection_c::eventPoolActivateResponse( uint8_t result ) {
    if ( result == 0 ) {
      setDevPoolState( PoolStateActive );
      if( getServer().getLastActiveTaskTC() )
        eventTaskStarted();
    } else {
      setDevPoolState( PoolStateError );
    }

    setDevPoolAction( PoolActionIdle );
  }


  void
  TcClientConnection_c::eventPoolDeleteResponse( uint8_t /* result */ ) {
    setDevPoolState( PoolStateNotPresent );
    setDevPoolAction( PoolActionIdle );
  }


  void
  TcClientConnection_c::doCommand( int32_t opcode, int32_t timeout ) {
    if ( m_uploadState == StateBusy )
      return; // StatusBusy
    if ( m_uploadState == StateTimeout )
      return; // StatusCannotProcess

    ProcessPkg_c pkg( opcode, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF );

    pkg.setMonitorItemForDA( const_cast<IsoItem_c*>( getRemoteItem() ) );
    pkg.setMonitorItemForSA( m_identItem.getIsoItem() );

    getIsoBusInstance4Comm() << pkg;

    m_uploadTimestamp = HAL::getTime();
    m_uploadTimeout = timeout;
    m_uploadStep = UploadNone;
    //m_commandParameter = opcode;
    m_uploadState = StateBusy;
  }


// MultiSendEventHandler_c implementation
  void
  TcClientConnection_c::reactOnStateChange( const SendStream_c& sendStream ) {
    m_sendSuccess = sendStream.getSendSuccess();
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
