/*
  serverinstance_c.h: Instance of a TC or DL

  (C) Copyright 2013 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SERVERINSTANCE_C_H
#define SERVERINSTANCE_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/pdremotenode_c.h>

#include <list>


namespace __IsoAgLib {

  class ServerInstance_c : public PdRemoteNode_c
  {
    public:
      ServerInstance_c( const IsoItem_c& it, IsoAgLib::ProcData::RemoteType_t type );
      ~ServerInstance_c();

      virtual void processMsg( const ProcessPkg_c& pkg );
      void processStatus( uint8_t status );

      bool isAlive() const;

      ecutime_t getLastStatusTime()   const { return m_lastTcStateReceivedTime; }
      uint8_t   getLastServerState()  const { return m_lastTcState; }
      bool      getLastActiveTaskTC() const { return m_lastActiveTaskTC; }

      IsoAgLib::ProcData::RemoteType_t getEcuType() const { return m_type; }

    private:
      virtual void timeEvent();

    private:
      bool m_tcAliveCached;
      
      bool m_lastActiveTaskTC;
      uint8_t m_lastTcState;
      ecutime_t m_lastTcStateReceivedTime;
      IsoAgLib::ProcData::RemoteType_t m_type;

    private:
      ServerInstance_c(ServerInstance_c const &);
      ServerInstance_c &operator=(ServerInstance_c const &);
  };

}

#endif
