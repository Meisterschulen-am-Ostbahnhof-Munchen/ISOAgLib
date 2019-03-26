/*
  itcclientconnection_c.h: Class for managing a connection

  (C) Copyright 2009 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITCCLIENTCONNECTION_C_H
#define ITCCLIENTCONNECTION_C_H

#include "impl/tcclientconnection_c.h"
#include "iprocdata.h"
#include "itcclientserver_c.h"

namespace IsoAgLib {

  class iTcClient_c;
  class iProcData_c;

  class iTcClientConnection_c : private __IsoAgLib::TcClientConnection_c
  {
  public:

      class iStateHandler_c : private __IsoAgLib::TcClientConnection_c::StateHandler_c
      {
      public:
        //! Please note that some callbacks here may get called during shutdown of the system. So in case you're shutting down, treat it accordingly.
        virtual void eventConnectionRequest( const iIdentItem_c&, const iTcClientServer_c&, const ProcData::ServerCapabilities_s & ) = 0;
        virtual void eventDisconnectedOnServerLoss( iTcClientConnection_c& ) = 0;
        virtual void eventDefaultLoggingStarted( iTcClientConnection_c& ) = 0;
        virtual void eventTaskStarted( iTcClientConnection_c& ) = 0;
        virtual void eventTaskStopped( iTcClientConnection_c& ) = 0;

      private:
        virtual void _eventConnectionRequest( const __IsoAgLib::IdentItem_c& ident, const __IsoAgLib::ServerInstance_c& server, const ProcData::ServerCapabilities_s &caps ) {
          eventConnectionRequest( ident.toConstIidentItem_c(), server.toConstITcClientServer_c(), caps );
        }
        virtual void _eventDisconnectedOnServerLoss( TcClientConnection_c& ecu ) {
          eventDisconnectedOnServerLoss( static_cast<iTcClientConnection_c&>( ecu ) );
        }
        virtual void _eventDefaultLoggingStarted( TcClientConnection_c& ecu ) {
          eventDefaultLoggingStarted( static_cast<iTcClientConnection_c&>( ecu ) );
        }
        virtual void _eventTaskStarted( TcClientConnection_c& ecu ) {
          eventTaskStarted( static_cast<iTcClientConnection_c&>( ecu ) );
        }
        virtual void _eventTaskStopped( TcClientConnection_c& ecu ) {
          eventTaskStopped( static_cast<iTcClientConnection_c&>( ecu ) );
        }

        friend class iTcClient_c;
      };

      //! It will pass out negative PdAck messages to the application
      //! Note: In a proper running system no negative PdAcks should
      //! be on the bus, so this is for special purposes only!!!
      //! It can be set or unset, but only one is possible at a time!
      void setNackHandler( IsoAgLib::ProcData::iNackHandler_c* handler )
      { PdConnection_c::setNackHandler( handler ); }

      // This is to be able to change your DDOP during run-time.
      // NOTE: This is a VERY EXPERIMENTAL method, the proper way
      // would probably only work for TC Version 4 (Deactivate Command)
      // so use with caution. Safest thing would be to only change your
      // DDOP with a required power-cycle.
      // This function/interface is subject to be changed!!!
      // Only call if you're connected to a TC!
      void forceDisconnectAndInitiateReconnect( bool shouldDeletePool = false )
      {
        __IsoAgLib::TcClientConnection_c::forceDisconnectAndInitiateReconnect( shouldDeletePool );
      }

      const iTcClientServer_c& server() const
      {
        isoaglib_assert( connected() );
        return connected()->toConstITcClientServer_c();
      }

#if 0
// currently not supported
      /* TODO */
      bool sendCommandChangeDesignator( uint16_t objectID, const char* newString, uint8_t stringLength ) {
        return TcClientConnection_c::sendCommandChangeDesignator( objectID, newString, stringLength );
      }
#endif

    private:
      iTcClientConnection_c();

      friend class iTcClient_c;
      friend class iTcClientServer_c;
      friend class iProcData_c;
  };

} // IsoAgLib

#endif
