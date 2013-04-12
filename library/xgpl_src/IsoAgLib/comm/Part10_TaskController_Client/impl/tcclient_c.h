/*
  tcclient_c.h: central managing instance for all Task Controller-Clients

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TCCLIENT_C_H
#define TCCLIENT_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>

#include "tcclientconnection_c.h"

#include <list>

namespace IsoAgLib {
  class iTcClient_c;
  class iTcClientConnection_c;
}

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {
class TcClientConnection_c;
class iIdentItem_c;
class ProcData_c;

class TcClient_c : public Subsystem_c
{
  MACRO_MULTITON_CONTRIBUTION();
public:
  virtual ~TcClient_c() {}

  void init();
  void close();

  TcClientConnection_c* initAndRegister( IdentItem_c&, IsoAgLib::iProcDataHandler_c&, IsoAgLib::iDevicePool_c& );
  bool deregister( IdentItem_c& );

  bool isTcAvailable() const { return m_isoNameTC.isSpecified(); }

#ifdef USE_DATALOGGER
  bool isLoggerAvailable() const { return m_isoNameLogger.isSpecified(); }
#endif

  void processMsg( const CanPkg_c& );
  void processMsgGlobal( const ProcessPkg_c& );
  void processMsgNonGlobal( const ProcessPkg_c& );

  const TcClientConnection_c& getTcClientConnection( const IdentItem_c& ) const;

  const IsoName_c& getISONameFromType( IsoAgLib::ProcData::RemoteType_t ) const;
  IsoAgLib::ProcData::RemoteType_t getTypeFromISOName( const IsoName_c& ) const;

  void reactOnIsoItemModification ( ControlFunctionStateHandler_c::iIsoItemAction_e, IsoItem_c const& );

  bool processTcStatusMsg(uint8_t ui8_tcStatus, const __IsoAgLib::IsoName_c& sender);

  void processChangeDesignator( IdentItem_c&, uint16_t, const char* );

private:
  TcClient_c();

  /// PROXY-CLASSES
  class CanCustomerProxy_c : public CanCustomer_c {
  public:
    typedef TcClient_c Owner_t;

    CanCustomerProxy_c(Owner_t &owner) : m_owner(owner) {}
    virtual ~CanCustomerProxy_c() {}

  private:
    virtual void processMsg( const CanPkg_c& data )
    { return m_owner.processMsg( data ); }

    Owner_t &m_owner;

  private:
    // not copyable
    CanCustomerProxy_c(CanCustomerProxy_c const &);
    CanCustomerProxy_c &operator=(CanCustomerProxy_c const &);
  };
  typedef CanCustomerProxy_c Customer_t;
  
  class ControlFunctionStateHandlerProxy_c : public ControlFunctionStateHandler_c {
  public:
    typedef TcClient_c Owner_t;

    ControlFunctionStateHandlerProxy_c(Owner_t &owner) : m_owner(owner) {}
    virtual ~ControlFunctionStateHandlerProxy_c() {}

  private:
    virtual void reactOnIsoItemModification( iIsoItemAction_e action, IsoItem_c const &isoItem)
    { m_owner.reactOnIsoItemModification(action, isoItem); }

    Owner_t &m_owner;

  private:
    // not copyable
    ControlFunctionStateHandlerProxy_c(ControlFunctionStateHandlerProxy_c const &);
    ControlFunctionStateHandlerProxy_c &operator=(ControlFunctionStateHandlerProxy_c const &);
  };
  typedef ControlFunctionStateHandlerProxy_c Handler_t;

private:
  STL_NAMESPACE::list<TcClientConnection_c*> m_tcConnections;

  bool m_lastActiveTaskTC;
  IsoName_c m_isoNameTC;

#ifdef USE_DATALOGGER
  bool m_lastActiveTaskLogger;
  IsoName_c m_isoNameLogger;
#endif

  Handler_t m_handler;
  Customer_t m_customer;

  friend TcClient_c &getTcClientInstance( uint8_t instance );
};


/** C-style function, to get access to the unique TcClient_c singleton instance
  * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
  */
TcClient_c &getTcClientInstance( uint8_t instance = 0 );
}

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

#endif
