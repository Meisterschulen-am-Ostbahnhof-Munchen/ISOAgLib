/*
  tcclient_c.h: central managing instance for all Task Controller-Clients

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TCCLIENT_C_H
#define TCCLIENT_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <functional>

#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/container.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include "processpkg_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdatahandler_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devpropertyhandler_c.h>

#include <list>

namespace IsoAgLib {
  class iTcClient_c;
  class iDevPropertyHandler_c;
}

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {

class TcClient_c : public Scheduler_Task_c
{
  MACRO_MULTITON_CONTRIBUTION();
private:
  TcClient_c();
  virtual ~TcClient_c() {}

public:
  void init( void );
  void close( void );

  bool timeEvent();
  bool processMsg( const CanPkg_c& arc_data );

  DevPropertyHandler_c& getDevPropertyHandlerInstance( void );
  ProcData_c* procData( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoNameReceiver, bool& elementFound);

  void resetTimerPeriod();
  void registerAccessFlt() {}

  bool registerLocalProcessData( ProcData_c* pc_localClient)
  { bool b_result = registerC1( pc_localClient ); mpc_iter = c_arrClientC1.begin(); return b_result;}
  void unregisterLocalProcessData( ProcData_c* pc_localClient)
  { unregisterC1( pc_localClient ); mpc_iter = c_arrClientC1.begin();}

  void reactOnIsoItemModification (ControlFunctionStateHandler_c::iIsoItemAction_e /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);

  bool processTcStatusMsg(uint8_t ui8_tcStatus, const __IsoAgLib::IsoName_c& sender);

  const IsoName_c& getISONameFromType( IsoAgLib::ProcData::remoteType_t ecuType ) const;
  IsoAgLib::ProcData::remoteType_t getTypeFromISOName( const IsoName_c& isoName ) const;

#if DEBUG_SCHEDULER
  virtual const char* getTaskName() const;
#endif

  void setProcDataHandler( IsoAgLib::iProcDataHandler_c *apc_procDataHandler )
  { mpc_procDataHandler = apc_procDataHandler; }

  void sendNack( const IsoName_c& ac_da, const IsoName_c& ac_sa, int16_t ddi, int16_t element,
                 IsoAgLib::ProcData::nackResponse_t a_errorcodes ) const;

private:
  virtual void updateEarlierAndLatestInterval();
  virtual uint16_t getForcedMinExecTime() const
  { return getForcedMinExecTimeDefault(); }

  void stopRunningMeasurement( IsoAgLib::ProcData::remoteType_t ecuType );

  /// PROXY-CLASSES
private:
  class CanCustomerProxy_c : public CanCustomer_c {
  public:
    typedef TcClient_c Owner_t;

    CanCustomerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}
    virtual ~CanCustomerProxy_c() {}

  private:
    virtual bool processMsg( const CanPkg_c& arc_data )
    { return mrt_owner.processMsg( arc_data ); }

    Owner_t &mrt_owner;

  private:
    // not copyable
    CanCustomerProxy_c(CanCustomerProxy_c const &);
    CanCustomerProxy_c &operator=(CanCustomerProxy_c const &);
  };
  typedef CanCustomerProxy_c Customer_t;
  
  class ControlFunctionStateHandlerProxy_c : public ControlFunctionStateHandler_c {
  public:
    typedef TcClient_c Owner_t;

    ControlFunctionStateHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}
    virtual ~ControlFunctionStateHandlerProxy_c() {}

  private:
    virtual void reactOnIsoItemModification(
        iIsoItemAction_e at_action,
        IsoItem_c const &acrc_isoItem)
    { mrt_owner.reactOnIsoItemModification(at_action, acrc_isoItem); }

    Owner_t &mrt_owner;

  private:
    // not copyable
    ControlFunctionStateHandlerProxy_c(ControlFunctionStateHandlerProxy_c const &);
    ControlFunctionStateHandlerProxy_c &operator=(ControlFunctionStateHandlerProxy_c const &);
  };
  typedef ControlFunctionStateHandlerProxy_c Handler_t;

private:
  DevPropertyHandler_c mc_devPropertyHandler;

  bool m_lastActiveTaskTC;
  IsoName_c mc_isoNameTC;

#ifdef USE_DATALOGGER
  bool m_lastActiveTaskLogger;
  IsoName_c mc_isoNameLogger;
#endif

  IsoAgLib::iProcDataHandler_c* mpc_procDataHandler;

  Handler_t mt_handler;
  Customer_t mt_customer;
  CONTAINER_CLIENT1_MEMBER_FUNCTIONS_MAIN(ProcData_c);
  cacheTypeC1_t mpc_iter;

  friend TcClient_c &getTcClientInstance( uint8_t aui8_instance );
};


/** C-style function, to get access to the unique TcClient_c singleton instance
  * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
  */
TcClient_c &getTcClientInstance( uint8_t aui8_instance = 0 );
}

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

#endif
