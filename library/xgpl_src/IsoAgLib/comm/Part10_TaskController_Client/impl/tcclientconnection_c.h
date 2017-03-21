/*
  tcclientconnection_c.h: class for managing a TC-client/server connection 

  (C) Copyright 2009 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)
  
  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TCCLIENTCONNECTION_H
#define TCCLIENTCONNECTION_H

#include <IsoAgLib/comm/Part3_DataLink/impl/multisendeventhandler_c.h>
#include <IsoAgLib/comm/Part3_DataLink/imultisendstreamer_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/pdconnection_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepool_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>

namespace IsoAgLib {
  class iDevicePool_c;
  class iTcClientConnection_c;
}

namespace __IsoAgLib {
  class IdentItem_c;
  class TcClient_c;
  class MultiSendPkg_c;
  class ProcessPkg_c;
  class ProcData_c;
  class DevicePool_c;
  class PdRemoteNode_c;
  class ServerInstance_c;


#define ISOAGLIB_LOCAL_PROXIES \
      class MultiSendEventHandlerProxy_c : public MultiSendEventHandler_c { \
        public: \
          typedef TcClientConnection_c Owner_t; \
 \
          MultiSendEventHandlerProxy_c( Owner_t &owner ) : m_owner( owner ) {} \
          ~MultiSendEventHandlerProxy_c() {} \
 \
        private: \
          void reactOnStateChange( const SendStream_c& sendStream ) { \
            m_owner.reactOnStateChange( sendStream ); \
          } \
 \
          MultiSendEventHandlerProxy_c( const MultiSendEventHandlerProxy_c & ); \
          MultiSendEventHandlerProxy_c &operator=( MultiSendEventHandlerProxy_c const & ); \
 \
          Owner_t &m_owner; \
      }; \
      class MultiSendStreamerProxy_c : public IsoAgLib::iMultiSendStreamer_c { \
        public: \
          typedef TcClientConnection_c Owner_t; \
 \
          MultiSendStreamerProxy_c( Owner_t &owner ) : m_owner( owner ) {} \
          ~MultiSendStreamerProxy_c() {} \
 \
        private: \
          virtual uint8_t getFirstByte() { return m_owner.getFirstByte(); } \
          virtual void resetDataNextStreamPart() { m_owner.resetDataNextStreamPart(); } \
          virtual void saveDataNextStreamPart() { m_owner.saveDataNextStreamPart(); } \
          virtual void restoreDataNextStreamPart() { m_owner.restoreDataNextStreamPart(); } \
          virtual uint32_t getStreamSize() { return m_owner.getStreamSize(); } \
          virtual void setDataNextStreamPart ( __IsoAgLib::MultiSendPkg_c* data, uint8_t bytes ) { \
            m_owner.setDataNextStreamPart( data, bytes ); } \
 \
          MultiSendStreamerProxy_c( const MultiSendStreamerProxy_c & );  \
          MultiSendStreamerProxy_c &operator=( MultiSendStreamerProxy_c const & ); \
 \
          Owner_t &m_owner; \
      };


  class TcClientConnection_c : public PdConnection_c
  {
    static const ecutime_t DEF_TimeOut_NormalCommand         = 1500;

    public:
      class StateHandler_c {
        public:
          virtual void _eventConnectionRequest( const IdentItem_c &, const ServerInstance_c &, const IsoAgLib::ProcData::ServerCapabilities_s & ) = 0;
          virtual void _eventDisconnectedOnServerLoss( TcClientConnection_c& ) = 0;
          virtual void _eventDefaultLoggingStarted( TcClientConnection_c& ) = 0;
          virtual void _eventTaskStarted( TcClientConnection_c& ) = 0;
          virtual void _eventTaskStopped( TcClientConnection_c& ) = 0;
      };

      TcClientConnection_c();
      TcClientConnection_c( const TcClientConnection_c & );
      virtual ~TcClientConnection_c();

      void preConnect(
        const IdentItem_c& identItem,
        ServerInstance_c& server,
        StateHandler_c& sh,
        const IsoAgLib::ProcData::ClientCapabilities_s& );

      void reConnect(
        const IdentItem_c& identItem,
        ServerInstance_c& server );

      bool fullConnect( const ServerInstance_c& server, DevicePool_c& pool );

      void disconnect( bool shouldDeletePool = false );

      void forceDisconnectAndInitiateReconnect( bool shouldDeletePool = false );

      ServerInstance_c* connected() const { return (ServerInstance_c *)( getRemoteNode() ); }
      const IsoName_c &getRemoteName() const { return getRemoteItem()->isoName(); } // no NULL check needed here!
      StateHandler_c* getStateHandler() const { return m_stateHandler; }
      IsoAgLib::ProcData::ClientCapabilities_s getClientCapabilities() const { return m_capsClient; }

#if 0
// not implemented yet
      bool sendCommandChangeDesignator( uint16_t /* objID */, const char* /* newString */, uint8_t /* length */ ) {
        return false; /* @todo: send msg */
      }
#endif

      void eventTaskStartStop( bool start );

      void notifyOnPeerDropOff( PdRemoteNode_c& pdRemoteNode ) { mc_localCsHandler.notifyOnPeerDropOff( pdRemoteNode ); }

    private:
      enum ControlAssignmentMode_t {
        AssignReceiver = 0,
        UnassignReceiver = 1,
        ReceiverAck = 2,
        AssignTransmitter = 3,
        UnassignTransmitter = 4,
        TransmitterAck = 5,
        Reserved6 = 6,
        Reserved7 = 7,
        Reserved8 = 8,
        Reserved9 = 9,
        Reserved10 = 10,
        Reserved11 = 11,
        Reserved12 = 12,
        Reserved13 = 13,
        Reserved14 = 14,
        Reserved15 = 15,
      };

      enum DevPoolState_t {
        PoolStateDisconnected,
        PoolStatePreconnecting,
        PoolStateAwaitingConnectionDecision,
        PoolStateConnecting,
        PoolStateUploading,
        //PoolStateStale,
        PoolStateUploaded,
        PoolStateActive,
        PoolStateError
      };

      enum CommandState_t {
        CommandStateNone = 0,
        CommandStateWaitingForResponse,
      };

      enum ProcessDataMsg_t {
        procCmdPar_RequestVersionMsg = 0x00,
        procCmdPar_VersionMsg = 0x10,
        procCmdPar_RequestStructureLabelMsg = 0x01,
        procCmdPar_StructureLabelMsg = 0x11,
        procCmdPar_RequestLocalizationLabelMsg = 0x21,
        procCmdPar_LocalizationLabelMsg = 0x31,
        procCmdPar_RequestOPTransferMsg = 0x41,
        procCmdPar_RequestOPTransferRespMsg = 0x51,
        procCmdPar_OPTransferMsg = 0x61,
        procCmdPar_OPTransferRespMsg = 0x71,
        procCmdPar_OPActivateMsg = 0x81,
        procCmdPar_OPActivateRespMsg = 0x91,
        procCmdPar_OPDeleteMsg = 0xA1,
        procCmdPar_OPDeleteRespMsg = 0xB1,
        procCmdPar_ChangeDesignatorMsg = 0xC1,
        procCmdPar_ChangeDesignatorRespMsg = 0xD1
      };

      int32_t requestPoolTransfer( ByteStreamBuffer_c pool );

      void eventStructureLabelResponse( const STL_NAMESPACE::vector<uint8_t>& label );
      void eventLocalizationLabelResponse( const STL_NAMESPACE::vector<uint8_t>& label );
      void eventPoolUploadResponse( uint8_t result );
      void eventPoolActivateResponse( uint8_t result );

      void setDevPoolState( DevPoolState_t newState );
      DevPoolState_t getDevPoolState() const { return m_devPoolState; }

      DevicePool_c &getDevicePool() { return static_cast<DevicePool_c &>( *m_pool ); }

      void timeEvent();

      void processMultiPacket( Stream_c &stream );
      virtual void processProcMsg( const ProcessPkg_c& );
      void processMsgTc( const ProcessPkg_c& data );

      virtual void processRequestDefaultDataLogging();

      bool processControlAssignmentReceiver(    bool assign, uint16_t elem, uint16_t ddi, const IsoName_c& name );
      bool processControlAssignmentTransmitter( bool assign, uint16_t elem, uint16_t ddi, const IsoName_c& name, uint16_t destElem );

      void handleNack( int16_t ddi, int16_t element );

      void doCommand( ProcessDataMsg_t cmd, int32_t timeout = DEF_TimeOut_NormalCommand, uint8_t param = 0xff );
      void sendMsg( uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t ) const;

      void stopRunningMeasurement();

      ISOAGLIB_LOCAL_PROXIES

      // MultiSendEventHandling
      void reactOnStateChange( const SendStream_c& );

      // MultiSendStreaming
      void setDataNextStreamPart( MultiSendPkg_c*, uint8_t );
      void resetDataNextStreamPart();
      void saveDataNextStreamPart();
      void restoreDataNextStreamPart();
      uint32_t getStreamSize();
      uint8_t getFirstByte();

    private:
      MultiSendEventHandlerProxy_c m_multiSendEventHandler;
      MultiSendStreamerProxy_c m_multiSendStreamer;

      class MultiReceiveClient_c : public CanCustomer_c
      {
      public:
        MultiReceiveClient_c( TcClientConnection_c& owner ) : m_owner( owner ) {}

        virtual bool reactOnStreamStart( ReceiveStreamIdentifier_c const &, uint32_t aui32_totalLen )
        { // currently only accept Peer Control Assignment commands.
          return ( aui32_totalLen == 14 );
        }

        virtual bool processPartStreamDataChunk( Stream_c &stream, bool isFirstChunk, bool isLastChunk )
        {
          if( isLastChunk )
            m_owner.processMultiPacket( stream );

          return false; // don't keep stream.
        }

        // MultiReceiveClient_c shall not be copyable. Otherwise the
        // reference to the containing object would become invalid.
        MultiReceiveClient_c(MultiReceiveClient_c const &);
        MultiReceiveClient_c &operator=(MultiReceiveClient_c const &);

      private:
        TcClientConnection_c &m_owner;
      } mc_mrClient;

      class ControlSourceHandler_c : SchedulerTask_c
      {
      public:
        ControlSourceHandler_c( TcClientConnection_c &tcClientConnection );

        void stop();

        bool addSetpointValueSource( uint16_t _element, uint16_t _ddi, const IsoItem_c& );
        bool removeSetpointValueSource( uint16_t _element, uint16_t _ddi, const IsoItem_c& );

        void notifyOnPeerDropOff( PdRemoteNode_c& );

        const IsoItem_c* getSetpointValueSource( uint16_t _element, uint16_t _ddi );

      private:
        struct SetpointValueSource_s
        {
          SetpointValueSource_s( uint16_t _element, uint16_t _ddi, PdRemoteNode_c& _controlSource )
            : element( _element )
            , ddi( _ddi )
            , m_lastReceivedTime( HAL::getTime() )
            , controlSource( _controlSource )
          {}
          SetpointValueSource_s( const SetpointValueSource_s& rhs )
            : element( rhs.element )
            , ddi( rhs.ddi )
            , m_lastReceivedTime( rhs.m_lastReceivedTime )
            , controlSource( rhs.controlSource )
          {}
          SetpointValueSource_s& operator=( const SetpointValueSource_s& rhs ); // cannot (and shouldn't) be implemented due to reference to ControlSource_c!

          uint16_t element;
          uint16_t ddi;
          PdRemoteNode_c& controlSource;

          ecutime_t m_lastReceivedTime;
        };

        typedef std::list<SetpointValueSource_s> SpValSourceList;
        typedef std::list<SetpointValueSource_s>::iterator SpValSourceListIter;
        
        SpValSourceListIter findSetpointValueSource( uint16_t _element, uint16_t _ddi, const IsoItem_c& isoItem );
        SpValSourceListIter findControlSource( const IsoItem_c& isoItem );

      private:
        void recalcTimeEventTrigger();
        virtual void timeEvent();

        SpValSourceListIter doRemoveSetpointValueSource( SpValSourceListIter iter );

      private:
        TcClientConnection_c& m_tcClientConnection; // back-ref.

        SpValSourceList m_listSpValueSource;
      } mc_localCsHandler;

      // MultiSendStreamer_c variables
      uint16_t m_currentSendPosition;
      uint16_t m_storedSendPosition;
      ByteStreamBuffer_c m_devicePoolToUpload;

      // timeEvent variables
      ecutime_t m_timeWsAnnounceKey;
      ecutime_t m_timeWaitWithAnyCommunicationUntil;
      ecutime_t m_timeLastWsTaskMsgSent;

      StateHandler_c* m_stateHandler;

      DevPoolState_t m_devPoolState;

      // Command handling
      CommandState_t m_cmdState;
      uint8_t m_cmdSent;
      ecutime_t m_cmdSentTimestamp;
      int32_t m_cmdTimeout;

      IsoAgLib::ProcData::ClientCapabilities_s m_capsClient;
      IsoAgLib::ProcData::ServerCapabilities_s m_capsServer;

      CLASS_SCHEDULER_TASK_PROXY( TcClientConnection_c )
      SchedulerTaskProxy_c m_schedulerTaskProxy;

  }; // TcClientConnection_c

  inline void
  TcClientConnection_c::setDevPoolState( DevPoolState_t newState )
  {
    m_devPoolState = newState;
  
    switch( getDevPoolState() )
    {
    case PoolStatePreconnecting:
    case PoolStateConnecting:
    case PoolStateUploading:
    //case PoolStateStale:
    case PoolStateUploaded:
    case PoolStateActive:
    case PoolStateError:
      m_schedulerTaskProxy.retriggerNow();
      break;

    case PoolStateDisconnected:
    case PoolStateAwaitingConnectionDecision:
      break;
    }
  }

} // __IsoAgLib

#endif
