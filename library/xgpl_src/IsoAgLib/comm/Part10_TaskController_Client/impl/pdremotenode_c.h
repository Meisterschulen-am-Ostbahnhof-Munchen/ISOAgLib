/*
  serverinstance_c.h: Instance of a TC or DL

  (C) Copyright 2013 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
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
    ~PdRemoteNode_c();

    bool isServer() const { return m_isServer; }
    virtual void timeEvent() {} // only due to avoiding muiltiple inheritance!

    const IsoItem_c& getIsoItem() const { return m_isoItem; }

    void addConnection( PdConnection_c& c );
    void removeConnection( PdConnection_c& c );
  
    virtual void processMsg( const ProcessPkg_c& pkg );

  protected:
    const IsoItem_c& m_isoItem;
    STL_NAMESPACE::list<PdConnection_c*> m_connections;   
    bool m_isServer;
  };
  
}

#endif
