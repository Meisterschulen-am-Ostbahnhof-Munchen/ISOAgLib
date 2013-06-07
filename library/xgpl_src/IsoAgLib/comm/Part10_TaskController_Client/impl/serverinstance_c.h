/*
  serverinstance_c.h:

  (C) Copyright 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/isoaglib_config.h>

#include <list>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclientconnection_c.h>

namespace __IsoAgLib {

  class ServerInstance_c {
    public:
      ServerInstance_c();
      ServerInstance_c( IsoItem_c& it, IsoAgLib::ProcData::RemoteType_t type );
      void close();
      void processStatus( uint8_t status );

      bool isAlive() const;

      const IsoItem_c& getIsoItem() const {
        return *m_isoItem;
      }

      bool addConnection( TcClientConnection_c& c );
      void removeConnection( TcClientConnection_c& c );

      int32_t getLastStatusTime() const {
        return m_lastTcStateReceivedTime;
      }
      uint8_t getLastServerState() const {
        return m_lastTcState;
      }

      IsoAgLib::ProcData::RemoteType_t getEcuType() const {
        return m_type;
      }
    private:
      IsoItem_c* m_isoItem;

      bool m_lastActiveTaskTC;
      IsoAgLib::ProcData::RemoteType_t m_type;

      int32_t m_lastTcStateReceivedTime;
      uint8_t m_lastTcState;

      STL_NAMESPACE::list<TcClientConnection_c*> m_connections;

  };

}
