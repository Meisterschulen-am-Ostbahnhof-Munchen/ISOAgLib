/*
  tcclientconnection_c.cpp: class for managing the
    communication between tc client and server

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

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
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/itcclientconnection_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclient_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/idevicepool_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


// @TODO : check if TCisAlive ? if TC status not sent for 6 seconds, stop sending measurment (call stopMeasurement)
// @TODO : manage TOTALS


namespace __IsoAgLib {

  void ByteStreamBuffer_c::format( uint8_t val ) {
    push_back( val );
  }


  void ByteStreamBuffer_c::format( uint16_t val ) {
    push_back( ( uint8_t )( val & 0xff ) );
    push_back( ( uint8_t )( ( val >> 8 ) & 0xff ) );
  }


  void ByteStreamBuffer_c::format( uint32_t val ) {
    format( ( uint16_t )( val & 0xffff ) );
    format( ( uint16_t )( ( val >> 16 ) & 0xffff ) );
  }


  void ByteStreamBuffer_c::format( const uint8_t* bp, size_t len ) {
    while ( len-- )
      push_back( *bp++ );
  }


  void ByteStreamBuffer_c::format( const char* str ) {
    const size_t l = CNAMESPACE::strlen( str );
    push_back( uint8_t( l ) );
    format( ( const uint8_t* )str, l );
  }


  void ByteStreamBuffer_c::format( int32_t val ) {
    format( ( uint32_t )val );
  }


  void ByteStreamBuffer_c::format( float val ) {
    uint32_t iVal = 0;
    CNAMESPACE::memcpy( &iVal, &val, sizeof( float ) );
#if defined(__TSW_CPP__) // Tasking uses mixed endian
    uint16_t lo = iVal >> 16;
    iVal = ( iVal << 16 ) | lo;
#endif
    format( iVal );
  }

  TcClientConnection_c::TcClientConnection_c( IdentItem_c& identItem, TcClient_c& tcClient, IsoAgLib::iTcClientConnectionStateHandler_c& sh, const IsoName_c& serverName, DevicePool_c& pool )
    : m_multiSendEventHandler( *this )
    , m_multiSendStreamer( *this )
    , m_identItem( &identItem )
    , m_timeWsAnnounceKey( -1 )
    , m_tcClient( &tcClient )
    , m_stateHandler( &sh )
    , m_serverName( serverName )
    , m_receiveFilterCreated( false )
    , m_currentSendPosition( 0 )
    , m_storedSendPosition( 0 )
    , m_uploadPoolState( DDOPRegistered )
    , m_uploadState( StateIdle )
    , m_uploadStep( UploadNone )
    , m_uploadTimestamp( 0 )
    , m_uploadTimeout( 0 )
    , m_commandParameter()
    , m_initDone( false )
    , m_tcAliveNew( false )
    , m_timeStartWaitAfterAddrClaim( -1 )
    , m_timeWsTaskMsgSent( -1 )
    , m_sendSuccess( SendStream_c::SendSuccess )
    , m_pool( &pool )
    , m_devPoolState( PoolSateDisabled )
    , m_devPoolAction( PoolActionIdle )
    , m_schedulerTaskProxy( *this, 100, false ) {
    getSchedulerInstance().registerTask( m_schedulerTaskProxy, 0 );
    m_pool->init( identItem );
    createMeasureProgs();
  }


  TcClientConnection_c::~TcClientConnection_c() {
    getMultiReceiveInstance4Comm().deregisterClient ( *this );
    getSchedulerInstance().deregisterTask( m_schedulerTaskProxy );
    // TODO: send deacticate msg
    //
    for( STL_NAMESPACE::map<uint32_t, MeasureProg_c*>::iterator i = m_measureProg.begin(); i != m_measureProg.end(); ++i ) {
      delete i->second;
    }
  }


  IsoAgLib::iTcClientConnection_c&
  TcClientConnection_c::toInterfaceReference() {
    return static_cast<IsoAgLib::iTcClientConnection_c&>( *this );
  }

  IsoAgLib::iTcClientConnection_c*
  TcClientConnection_c::toInterfacePointer() {
    return static_cast<IsoAgLib::iTcClientConnection_c*>( this );
  }


  void TcClientConnection_c::enable( bool a ) {
    if( a ) {
      if( getDevPoolState() == PoolSateDisabled ) {
        setDevPoolState( PoolStateInit );
      }
    } else {
      // TODO verify this works! Maybe something else need to be done....
      setDevPoolState( PoolSateDisabled );
    }
  }


  void
  TcClientConnection_c::timeEvent( void ) {
    // do further activities only if registered ident is initialized as ISO and already successfully address-claimed...
    if ( !m_identItem->isClaimedAddress() )
      return;


    // reset connection if the server is away
    ServerInstance_c* server = m_tcClient->getServer( m_serverName );
    if( ! server ) {
      m_initDone = false;
      return;
    }


    if ( ! m_receiveFilterCreated ) {
      getMultiReceiveInstance4Comm().registerClientIso ( *this, getIdentItem().isoName(), VT_TO_ECU_PGN );

      m_receiveFilterCreated = true;
    }

    // Wait until we are properly initialized before doing anything else
    if ( ! m_initDone ) {
      // Address claim is complete. Set time at which our address was claimed if it is
      // still in an initialized state
      if ( m_timeStartWaitAfterAddrClaim == -1 ) {
        m_timeStartWaitAfterAddrClaim = HAL::getTime();
        return;
      }

      // init is finished when more then 6sec after addr claim and at least one TC status message was received
      if ( ( HAL::getTime() - m_timeStartWaitAfterAddrClaim >= 6000 ) && ( server->getLastStatusTime() != -1 ) )
        m_initDone = true;
      return;
    }

    checkAndHandleTcStateChange();

    if( getDevPoolState() == PoolSateDisabled ) return;

    // ### Do nothing if there's no TC alive ###
    if ( !isTcAlive() )
      return;

    // Check if the working-set is completely announced
    if ( !m_identItem->getIsoItem()->isWsAnnounced ( m_timeWsAnnounceKey ) )
      return;

    // Send the WS task message to maintain connection with the TC
    sendWorkingSetTaskMsg();


    timeEventDevicePool();
  }


  void
  TcClientConnection_c::checkAndHandleTcStateChange() {
    const bool tcAliveOld = m_tcAliveNew;
    m_tcAliveNew = isTcAlive();

    if ( tcAliveOld != m_tcAliveNew ) {
      // react on tc alive change "false->true"
      if( m_tcAliveNew ) {
        m_uploadState = StateIdle;
        m_timeWsAnnounceKey = m_identItem->getIsoItem()->startWsAnnounce();
      } else {
        m_timeWsAnnounceKey = -1;
      }

      eventTcAlive( m_tcAliveNew );
    }
  }


  void
  TcClientConnection_c::timeEventDevicePool() {
    // If there is no device pool or the pool cannot be uploaded, do nothing
    if ( m_uploadPoolState == DDOPCannotBeUploaded )
      return;

    // device pool is registered, continue with upload
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
        m_commandParameter = procCmdPar_OPTransferMsg;
        m_uploadTimestamp = HAL::getTime();
        m_uploadTimeout = DEF_WaitFor_Reupload;
        break;

      case UploadNone:
      default:
        break;
    }

    if ( m_devPoolAction == PoolActionIdle ) {
      if ( getDevPoolState() == PoolStateActive ) {
        //if ( m_pool.isDirty() )
        //  setDevPoolState(PoolStateStale);
        // @todo trigger re-announce for fresh pool upload with changed objects
      } else {
        switch ( getDevPoolState() ) {
          case PoolStateInit:
            // Initialization state
            //	Retrieve the structure label from the current pool
            if ( m_pool->isEmpty() )
              break;

            setDevPoolState( PoolStatePresetting );
            requestStructureLabel();
            setDevPoolAction( PoolActionWaiting );
            break;

//      case  PoolStateError:
//        setDevPoolState( PoolStateInit );
//        m_PoolAction = PoolActionIdle;
//        break;

          case PoolStateInvalid:
            // Pool is present, but it is the wrong version -- delete it
            requestPoolDelete();
            setDevPoolAction( PoolActionWaiting );
            break;

          case PoolStateNotPresent:
            // No device pool present - upload it
            if ( m_pool->isEmpty() )
              break;

            requestPoolTransfer( m_pool->getBytestream() );
            setDevPoolAction( PoolActionWaiting );
            break;

          case PoolStateStale: {
              // Upload changed descriptions
              /*std::vector<uint8_t> newBytes;
              if (m_pool->getDirtyBytestream(newBytes))
              {
                requestPoolTransfer(newBytes);
                setDevPoolAction( PoolActionWaiting );
              }
              else
                setDevPoolState(PoolStateActive);*/
              // @todo trigger re-announce for fresh pool upload with changed objects
            }
            break;

          case PoolStateUploaded:
            // Pool has been uploaded - activate it
            requestPoolActivate();
            setDevPoolAction( PoolActionWaiting );
            break;

          default:
            break;
        }
      }
    }
  }


  bool
  TcClientConnection_c::isTcAlive() {
    ServerInstance_c* server = m_tcClient->getServer( m_serverName );
    return server ? server->isAlive() : false;
  }


  void
  TcClientConnection_c::sendWorkingSetTaskMsg() {
    // Ensure that we and the server are on the bus...
    ServerInstance_c* server = m_tcClient->getServer( m_serverName );
    if ( m_identItem == 0 || ! m_identItem->isClaimedAddress() || ! server )
      return;

    if ( ! isTcAlive() )
      return;

    // Send the WS task message every two seconds
    const int32_t now = HAL::getTime();
    if ( now - m_timeWsTaskMsgSent >= 2000 ) {
      m_timeWsTaskMsgSent = now;
      sendMsg( 0xff, 0xff, 0xff, 0xff, server->getLastServerState(), 0x00, 0x00, 0x00 );
    }
  }

  void
  TcClientConnection_c::startUpload() {
    const uint32_t byteStreamLength = m_devicePoolToUpload.getEnd() - 1;		// -1 to remove 0x61 opcode

    sendMsg( procCmdPar_RequestOPTransferMsg, ( uint8_t )( ( byteStreamLength ) & 0xff ),
             ( uint8_t )( ( byteStreamLength >> 8 ) & 0xff ), ( uint8_t )( ( byteStreamLength >> 16 ) & 0xff ),
             ( uint8_t )( ( byteStreamLength >> 24 ) & 0xff ), 0xff, 0xff, 0xff );

    m_uploadTimestamp = HAL::getTime();
    m_uploadTimeout = DEF_TimeOut_OPTransfer;

    m_uploadState = StateBusy;
    m_uploadStep = UploadNone;
  }


  void TcClientConnection_c::processMsgEntry( const ProcessPkg_c& pkg ) {

    if( pkg.getMonitorItemForDA() != m_identItem->getIsoItem() ) {
      return;
    }

    // first check if this is a device property message -> then DevPropertyHandler_c should process this msg
    if ( ( pkg.men_command == ProcessPkg_c::requestConfiguration )
        || ( pkg.men_command == ProcessPkg_c::configurationResponse )
        || ( pkg.men_command == ProcessPkg_c::nack ) ) {
       processMsgTc( pkg );
    } else if ( ( pkg.isoPgn() & 0x3FF00 ) == PROCESS_DATA_PGN ) {
      processMsgProcData( pkg );
    }
  }


  void TcClientConnection_c::processMsgProcData( const ProcessPkg_c& data ) {
    if( ( data.mui16_DDI == IsoAgLib::ProcData::DefaultDataLoggingDDI ) && ( data.men_command == ProcessPkg_c::requestValue ) ) {
      m_stateHandler->processDefaultLoggingStart( static_cast<IsoAgLib::iIsoItem_c&>( *data.getMonitorItemForSA() ) );
    } else {
      bool elementFound = false;
      ProcData_c* pd = procData( data.mui16_DDI, data.mui16_element, elementFound );
      if ( pd ) {
        pd->processMsg( data );
      } else {
        // element exists but DDI not present -> DDI not supported by element
        sendNack( data.mui16_DDI, data.mui16_element, elementFound ? IsoAgLib::ProcData::NackDDINoSupportedByElement : IsoAgLib::ProcData::NackInvalidElementNumber );
      }
    }
  }


  void TcClientConnection_c::processMsgTc( const ProcessPkg_c& data ) {
    if( getDevPoolState() == PoolSateDisabled ) {
      return;
    }

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
        m_versionLabel = data.getUint8Data( 1 );
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
          // @TODO AKA how to handle NULL response ? --> MWD: Better check if we actually sent the request message!!

          m_sendSuccess = SendStream_c::SendSuccess;
          getMultiSendInstance4Comm().sendIsoTarget( m_identItem->isoName(), m_serverName, m_devicePoolToUpload.getBuffer(),
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
    // can't (up)load the pool.
    m_uploadStep = UploadFailed;
    m_uploadPoolState = DDOPCannotBeUploaded;
  }


  int32_t
  TcClientConnection_c::requestPoolTransfer( ByteStreamBuffer_c pool ) {
    if ( pool.getEnd() <= 1 )
      return StatusCannotProcess;

    m_devicePoolToUpload = pool;
    // The pool will be sent in a multipacket. Insert the upload command byte (061) at the head of buffer
    m_devicePoolToUpload[0] = uint8_t( procCmdPar_OPTransferMsg );
    startUpload();

    return StatusNoError;
  }


  ProcData_c*
  TcClientConnection_c::procData( uint16_t DDI, uint16_t element, bool& elementFound ) const {
    isoaglib_assert( m_pool );
    elementFound = false;
    for( DevicePool_c::procDataList_t::iterator i = m_pool->m_procDatas.begin(); i != m_pool->m_procDatas.end(); ++i ) {
      if ( ( *i )->element() == element ) {
        elementFound = true;
        if ( ( *i )->DDI() == DDI ) {
          return *i;
        }
      }
    }

    return NULL;
  }


  void TcClientConnection_c::processTaskStarted( IsoItem_c& ecu ) {
    isoaglib_assert( m_stateHandler );
    m_stateHandler->processTaskStarted( static_cast<IsoAgLib::iIsoItem_c&>( ecu ) );
  }


  void TcClientConnection_c::processTaskStopped( IsoItem_c& ecu ) {
    isoaglib_assert( m_stateHandler );
    m_stateHandler->processTaskStopped( static_cast<IsoAgLib::iIsoItem_c&>( ecu ) );
  }


  void
  TcClientConnection_c::stopRunningMeasurement() {
    for( DevicePool_c::procDataList_t::iterator i = m_pool->m_procDatas.begin(); i != m_pool->m_procDatas.end(); ++i ) {
      ( *i )->stopRunningMeasurement();
    }
  }


  void
  TcClientConnection_c::sendNack( int16_t ddi, int16_t element, IsoAgLib::ProcData::NackResponse_t errorcodes ) const {
    isoaglib_assert( errorcodes != IsoAgLib::ProcData::NackReserved1 );
    isoaglib_assert( errorcodes != IsoAgLib::ProcData::NackReserved2 );
    isoaglib_assert( errorcodes != IsoAgLib::ProcData::NackUndefined );

    const uint8_t ui8_cmd = 0xd;
    sendMsg( ( uint8_t )( ( ui8_cmd & 0xf ) | ( ( element & 0xf ) << 4 ) ), ( uint8_t )( element >> 4 ), ( uint8_t )( ddi & 0x00FF ),
             ( uint8_t )( ( ddi & 0xFF00 ) >> 8 ), ( uint8_t )errorcodes, 0xff, 0xff, 0xff );
  }


  void
  TcClientConnection_c::sendMsg( uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4,
                                 uint8_t b5, uint8_t b6, uint8_t b7 ) const {
    ProcessPkg_c pkg;

    pkg.setISONameForDA( m_serverName ); // TODO isoitem
    pkg.setMonitorItemForSA( m_identItem->getIsoItem() );
    pkg.setIsoPri( 3 );
    pkg.setIsoPgn( PROCESS_DATA_PGN );
    pkg.setUint8Data( 0, b0 );
    pkg.setUint8Data( 1, b1 );
    pkg.setUint8Data( 2, b2 );
    pkg.setUint8Data( 3, b3 );
    pkg.setUint8Data( 4, b4 );
    pkg.setUint8Data( 5, b5 );
    pkg.setUint8Data( 6, b6 );
    pkg.setUint8Data( 7, b7 );
    pkg.setLen( 8 );

    getIsoBusInstance4Comm() << pkg;
  }

  void
  TcClientConnection_c::eventTcAlive( bool isAlive ) {
    setDevPoolAction( PoolActionIdle );
    if( isAlive ) {
      enable( true ); // TODO call app handler
    }
  }


  void
  TcClientConnection_c::eventStructureLabelResponse( uint8_t result, const std::vector<uint8_t>& label ) {
    if ( result == 0 && !label.empty() ) {
      DeviceObjectDvc_c* dvc = m_pool->getDvcObject();
      if ( dvc ) {
        const IsoAgLib::StructureLabel_s& strLbl = dvc->getStructureLabel();
        if ( std::memcmp( ( void * )&strLbl, ( void * )&label[0], 7 ) != 0 ) {
          setDevPoolState( PoolStateInvalid );
        }
      }
    } else {
      //m_pool->setDirty();
      setDevPoolState( PoolStateNotPresent );
    }

    if ( getDevPoolState() == PoolStatePresetting ) {
      requestLocalizationLabel();
      setDevPoolAction( PoolActionWaiting );
    } else
      setDevPoolAction( PoolActionIdle );
  }


  void
  TcClientConnection_c::eventLocalizationLabelResponse( uint8_t result, const std::vector<uint8_t>& label ) {
    if ( result == 0 && !label.empty() ) {
      DeviceObjectDvc_c* dvc = m_pool->getDvcObject();
      if ( dvc ) {
        const IsoAgLib::Localization_s& locale = dvc->getLocalization();
        if ( std::memcmp( ( void* )&locale, ( void * )&label[0], 7 ) != 0 ) {
          m_pool->updateLocale();
        }
        setDevPoolState( PoolStateUploaded );
      }
    } else {
      //m_pool->setDirty();
      setDevPoolState( PoolStateNotPresent );
    }

    setDevPoolAction( PoolActionIdle );
  }


  void
  TcClientConnection_c::eventPoolUploadResponse( uint8_t result ) {
    if ( result == 0 ) {
      setDevPoolState( PoolStateUploaded );
      m_pool->freeByteStreamBuffer( m_devicePoolToUpload.getBuffer() );
    } else {
      setDevPoolState( PoolStateError );
    }

    setDevPoolAction( PoolActionIdle );
  }


  void
  TcClientConnection_c::eventPoolActivateResponse( uint8_t result ) {
    if ( result == 0 ) {
      setDevPoolState( PoolStateActive );
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


  void TcClientConnection_c::sendProcMsg( uint16_t ddi, uint16_t element, int32_t pdValue ) const {
    ProcessPkg_c pkg;

    pkg.men_command = ProcessPkg_c::setValue;
    pkg.setISONameForDA( m_serverName );
    pkg.setMonitorItemForSA( m_identItem->getIsoItem() );
    pkg.setIsoPri( 3 );
    pkg.setIsoPgn( PROCESS_DATA_PGN );
    pkg.mui16_element = element;
    pkg.mui16_DDI = ddi;
    pkg.mi32_pdValue = pdValue;

    // check if command is valid
    isoaglib_assert( pkg.men_command != ProcessPkg_c::commandReserved1 );
    isoaglib_assert( pkg.men_command != ProcessPkg_c::commandReserved2 );
    isoaglib_assert( pkg.men_command != ProcessPkg_c::commandReserved3 );
    isoaglib_assert( pkg.men_command != ProcessPkg_c::commandReserved4 );
    isoaglib_assert( pkg.men_command != ProcessPkg_c::CommandUndefined );

    uint8_t ui8_cmd = static_cast<uint8_t>( pkg.men_command );
    pkg.setUint8Data( 0, ( ui8_cmd & 0xf ) | ( ( pkg.mui16_element & 0xf ) << 4 ) );
    pkg.setUint8Data( 1, pkg.mui16_element >> 4 );
    pkg.setUint8Data( 2, pkg.mui16_DDI & 0x00FF );
    pkg.setUint8Data( 3, ( pkg.mui16_DDI& 0xFF00 ) >> 8 );
    // for ISO the ident is directly read and written
    pkg.setInt32Data( 4, pkg.mi32_pdValue );
    pkg.setLen( 8 );


    getIsoBusInstance4Comm() << pkg;
  }


  int32_t
  TcClientConnection_c::doCommand( int32_t opcode, int32_t timeout ) {

    ServerInstance_c* server = m_tcClient->getServer( m_serverName );
    if ( ( ! m_identItem ) || ( ! m_identItem->getIsoItem() ) || ( ! server ) )
      return StatusNotInitted;

    if ( m_uploadState == StateBusy )
      return StatusBusy;
    if ( m_uploadState == StateTimeout )
      return StatusCannotProcess;

    ProcessPkg_c pkg;

    pkg.setIsoPri( 3 );
    pkg.setIsoPgn( PROCESS_DATA_PGN );
    pkg.setMonitorItemForDA( const_cast<IsoItem_c*>( &server->getIsoItem() ) );
    pkg.setMonitorItemForSA( m_identItem->getIsoItem() );
    pkg.setUint8Data ( 0, opcode );
    pkg.setUint8Data ( 1, 0xff );
    pkg.setUint8Data ( 2, 0xff );
    pkg.setUint8Data ( 3, 0xff );
    pkg.setUint8Data ( 4, 0xff );
    pkg.setUint8Data ( 5, 0xff );
    pkg.setUint8Data ( 6, 0xff );
    pkg.setUint8Data ( 7, 0xff );
    pkg.setLen ( 8 );

    getIsoBusInstance4Comm() << pkg;

    m_uploadTimestamp = HAL::getTime();
    m_uploadTimeout = timeout;
    m_uploadStep = UploadNone;
    m_commandParameter = opcode;
    m_uploadState = StateBusy;

    return StatusNoError;
  }


  void TcClientConnection_c::createMeasureProgs() {
    for( DevicePool_c::procDataList_t::iterator i = m_pool->m_procDatas.begin(); i != m_pool->m_procDatas.end(); ++i ) {
      ProcData_c* pd = ( *i );
      uint8_t methods = pd->triggerMethod();
      const uint16_t ddi = pd->DDI();
      if( methods ) {
        uint32_t key = ( uint32_t( uint32_t( ddi ) << 16 ) ) | pd->element();
        MeasureProg_c* mp = new MeasureProg_c( this );
        m_measureProg[ key ] = mp;
        pd->addMeasureProg( mp );
      }
    }
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

}; // namespace __IsoAgLib
