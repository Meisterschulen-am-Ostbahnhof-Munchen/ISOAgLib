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
#ifndef PDREMOTENODE_C_H
#define PDREMOTENODE_C_H

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/pdconnection_c.h>
#include <list>


namespace __IsoAgLib {

  class PdRemoteNode_c : public SchedulerTask_c // only due to avoiding muiltiple inheritance!
  {
  public:
    PdRemoteNode_c( const IsoItem_c& it, bool isServer );
    virtual ~PdRemoteNode_c();

    bool isServer() const { return m_isServer; }
    virtual void timeEvent() {} // only due to avoiding muiltiple inheritance!

    const IsoItem_c& getIsoItem() const { return m_isoItem; }

    void addConnection( PdConnection_c& c );
    void removeConnection( PdConnection_c& c );
  
    bool hasConnections() const { return !m_connections.empty(); }

    virtual void processMsg( const ProcessPkg_c& pkg );

  protected:
    const IsoItem_c& m_isoItem;
    std::list<PdConnection_c*> m_connections;   
    bool m_isServer;
  };
  
}

#endif
