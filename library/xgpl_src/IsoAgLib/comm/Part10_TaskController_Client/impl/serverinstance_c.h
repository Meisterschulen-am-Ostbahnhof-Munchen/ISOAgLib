/*
  serverinstance_c.h: Instance of a TC or DL

  (C) Copyright 2013 - 2019 by OSB AG

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
#ifndef SERVERINSTANCE_C_H
#define SERVERINSTANCE_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/pdremotenode_c.h>

#include <list>

namespace IsoAgLib {
  class iTcClient_c;
  class iTcClientServer_c;
}


namespace __IsoAgLib {

  class ServerInstance_c : public PdRemoteNode_c
  {
    public:
      ServerInstance_c( const IsoItem_c& it, IsoAgLib::ProcData::ServerType_t type );
      ~ServerInstance_c();

      virtual void processMsg( const ProcessPkg_c& pkg );
      void processStatus( uint8_t status );

      bool isAlive() const;
      uint32_t getNotBusyCount() const
      {
          return m_tcNotBusyCount;
      }

      ecutime_t getLastStatusTime()             const { return m_lastTcStateReceivedTime; }
      bool      getLastStatusTaskTotalsActive() const { return m_lastTcState & 0x01; }

      void notifyOnDropOff();

      IsoAgLib::ProcData::ServerType_t getEcuType() const { return m_type; }

      const IsoAgLib::iTcClientServer_c& toConstITcClientServer_c() const;

    private:
      virtual void timeEvent();
      
      bool isBusy() const;

    private:
      bool m_tcAliveCached;

      uint8_t   m_lastTcState;
      ecutime_t m_lastTcStateReceivedTime;
      uint32_t  m_tcNotBusyCount;

      IsoAgLib::ProcData::ServerType_t m_type;

    private:
      ServerInstance_c(ServerInstance_c const &);
      ServerInstance_c &operator=(ServerInstance_c const &);

      friend class IsoAgLib::iTcClient_c;
  };

}

#endif
