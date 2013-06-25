/*
  itcclient_c.h: central managing instance for all process data

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITCCLIENT_C_H
#define ITCCLIENT_C_H

#include "impl/tcclient_c.h"
#include "iprocdata.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/itcclientconnection_c.h>


namespace IsoAgLib {

  class iDevicePool_c;


  class iTcClient_c : private __IsoAgLib::TcClient_c {
    public:
      class iServerStateHandler_c : private __IsoAgLib::TcClient_c::ServerStateHandler_c {
        public:
          virtual void eventServerAvailable( const iIsoItem_c&, ProcData::RemoteType_t ) = 0;

        private:
          virtual void _eventServerAvailable( const __IsoAgLib::IsoItem_c& item, IsoAgLib::ProcData::RemoteType_t type ) {
            eventServerAvailable( static_cast<const iIsoItem_c&>( item ), type );
          }
          friend class iTcClient_c;
      };

      void init() {
        TcClient_c::init();
      }

      void setServerStateHandler( iServerStateHandler_c& hdl )
      {
        TcClient_c::setServerStateHandler( hdl );
      }

      void close() {
        TcClient_c::close();
      }

      iTcClientConnection_c* connect( iIdentItem_c& identItem, iTcClientConnection_c::iStateHandler_c& statehandler, const iIsoItem_c& server, iDevicePool_c& pool ) {
        return static_cast<iTcClientConnection_c*>( TcClient_c::connect(
            static_cast<__IsoAgLib::IdentItem_c&>( identItem ),
            static_cast<__IsoAgLib::TcClientConnection_c::StateHandler_c&>( statehandler ),
            static_cast<const __IsoAgLib::IsoItem_c&>( server ),
            static_cast<__IsoAgLib::DevicePool_c&>( pool ) ) );
      }

      bool disconnect( iIdentItem_c& identItem ) {
        return TcClient_c::disconnect( static_cast<__IsoAgLib::IdentItem_c&>( identItem ) );
      }

      /* changing designators is not yet supported */
      void processChangeDesignator( iIdentItem_c& identItem, uint16_t objID, const char* newDesignator ) {
        return TcClient_c::processChangeDesignator( static_cast<__IsoAgLib::IdentItem_c&>( identItem ), objID, newDesignator );
      }

    private:
      friend iTcClient_c& getItcClientInstance( uint8_t instance );
  };

  inline iTcClient_c& getItcClientInstance( uint8_t instance = 0 ) {
    return static_cast<iTcClient_c&>( __IsoAgLib::getTcClientInstance( instance ) );
  }

}

#endif
