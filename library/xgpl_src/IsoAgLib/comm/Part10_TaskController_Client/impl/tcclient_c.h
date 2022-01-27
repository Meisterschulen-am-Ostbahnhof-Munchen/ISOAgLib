/*
  tcclient_c.h: central managing instance for all Task Controller-Clients

  (C) Copyright 2009 - 2019 by OSB AG

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
#ifndef TCCLIENT_C_H
#define TCCLIENT_C_H

#include "tcclientconnection_c.h"

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/util/impl/multiton.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>

#include <list>
#include <map>

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {
  class ProcData_c;
  class PdPool_c;

  class TcClient_c : public Subsystem_c {
      MACRO_MULTITON_CONTRIBUTION();

    public:
      class Provider_c {
      public:
        virtual uint32_t provideDistance() const = 0;
        virtual uint16_t provideSpeed() const = 0;
      };

      virtual ~TcClient_c() {}

      void init();
      void close();

      void setProvider( Provider_c * );
      Provider_c *getProvider() const;

      bool registerClient( IdentItem_c&, const IsoAgLib::ProcData::ClientCapabilities_s&, TcClientConnection_c::StateHandler_c& );
      bool deregisterClient( IdentItem_c& );

      // Note: The connections will automatically get disconnected and destructed on RemoteNode-Loss.
      // important to either call "doConnect" or "dontConnect" after an eventConnectionRequest!
      TcClientConnection_c* doConnect( const IdentItem_c&, const ServerInstance_c&, DevicePool_c& );
      void                dontConnect( const IdentItem_c&, const ServerInstance_c& );
      
      PdConnection_c* connectPeer(          const IdentItem_c&, const IsoItem_c& pdItem, PdPool_c& );
      PdConnection_c* connectPeerBroadcast( const IdentItem_c&, PdPool_c& );

      void disconnectPeer( const IdentItem_c& );
      void disconnectPeer( const PdConnection_c& );

      void proprietaryServer( const IsoItem_c &, bool available );

#if 0
// currently not supported
      void processChangeDesignator( const IdentItem_c&, uint16_t, const char* );
#endif

      void notifyServerStatusChange( ServerInstance_c& server, bool new_status );
      void notifyPeerAndControlSourceDestruction( PdRemoteNode_c& pdRemoteNode );
      void notifyConnectionToBeEnded( TcClientConnection_c& );

      PdRemoteNode_c& addConnectionToControlSource( PdConnection_c&, const IsoItem_c & );
      void removeConnectionFromControlSource( PdConnection_c&, const IsoItem_c & );


#if defined(USE_DIRECT_PD_HANDLING)
      /// /////////////////////////////////////////////////
      /// WARNING: only use if you know what you are doing!
      /// /////////////////////////////////////////////////
      class PdMessageHandler_c {
        public:
          virtual void _eventPdMessageReceived( const IsoItem_c&, const IsoItem_c*, IsoAgLib::ProcData::CommandType_t, uint16_t, uint16_t, int32_t) = 0;
      };

      void setPdMessageHandler( PdMessageHandler_c& hdl );
      void clearPdMessageHandler();

      void sendPdMessage( const IsoItem_c& sa_item, 
                          const IsoItem_c* da_item,
                          IsoAgLib::ProcData::CommandType_t command, uint16_t element, uint16_t ddi, int32_t value );

    private:
      PdMessageHandler_c* m_pdMessageHandler;
#endif


    private:
      TcClient_c();

      void processMsg( const CanPkg_c& );
      void reactOnIsoItemModification ( ControlFunctionStateHandler_c::iIsoItemAction_e, IsoItem_c const& );

      void removeRemotePd( const IsoItem_c & );
      void addServer( const IsoItem_c &, IsoAgLib::ProcData::ServerType_t type );

      ServerInstance_c *findNextServerOfSameType( const ServerInstance_c &thisServer ) const;

      PdRemoteNode_c *findRemoteNode( const IsoItem_c & ) const;


      /// PROXY-CLASSES
      class CanCustomerProxy_c : public CanCustomer_c {
        public:
          typedef TcClient_c Owner_t;

          CanCustomerProxy_c( Owner_t &owner ) : m_owner( owner ) {}
          virtual ~CanCustomerProxy_c() {}

        private:
          virtual void processMsg( const CanPkg_c& data ) {
            return m_owner.processMsg( data );
          }

          Owner_t &m_owner;

        private:
          // not copyable
          CanCustomerProxy_c( CanCustomerProxy_c const & );
          CanCustomerProxy_c &operator=( CanCustomerProxy_c const & );
      };
      typedef CanCustomerProxy_c Customer_t;

      class ControlFunctionStateHandlerProxy_c : public ControlFunctionStateHandler_c {
        public:
          typedef TcClient_c Owner_t;

          ControlFunctionStateHandlerProxy_c( Owner_t &owner ) : m_owner( owner ) {}
          virtual ~ControlFunctionStateHandlerProxy_c() {}

        private:
          virtual void reactOnIsoItemModification( iIsoItemAction_e action, IsoItem_c const &isoItem ) {
            m_owner.reactOnIsoItemModification( action, isoItem );
          }

          Owner_t &m_owner;

        private:
          // not copyable
          ControlFunctionStateHandlerProxy_c( ControlFunctionStateHandlerProxy_c const & );
          ControlFunctionStateHandlerProxy_c &operator=( ControlFunctionStateHandlerProxy_c const & );
      };
      typedef ControlFunctionStateHandlerProxy_c Handler_t;

    private:
      Provider_c *m_provider;
      Handler_t m_handler;
      Customer_t m_customer;

      struct ClientInfo_s
      {
        // 1/2: TcClient-PdConnections.
        TcClientConnection_c m_serverConnections[ IsoAgLib::ProcData::ServerTypes ];

        IsoAgLib::ProcData::ClientCapabilities_s capabilities;
        TcClientConnection_c::StateHandler_c *stateHandler;
      };
      typedef STL_NAMESPACE::map<IdentItem_c*, struct ClientInfo_s> IdentToClientInfoMap_t;
      IdentToClientInfoMap_t m_clientInfo;

      // 2/2: Peer-PdConnections.
      STL_NAMESPACE::list<PdConnection_c*> m_peerConnections;

      typedef STL_NAMESPACE::list<PdRemoteNode_c*> ItemToRemoteNodeMap_t;
      ItemToRemoteNodeMap_t m_pdRemoteNodes;

      friend TcClient_c &getTcClientInstance( unsigned instance );
      friend class ProcData_c;
  };

  inline
  void
  TcClient_c::setProvider( TcClient_c::Provider_c *provider )
  {
    m_provider = provider;
  }


  inline
  TcClient_c::Provider_c *
  TcClient_c::getProvider() const
  {
    return m_provider;
  }




  TcClient_c &getTcClientInstance( unsigned instance );

}

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

#endif
