/*
  process_c.h: central managing instance for all process data
    informations in the system

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCESS_H
#define PROCESS_H

#include <IsoAgLib/isoaglib_config.h>

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
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
  class iProcess_c;
  class iDevPropertyHandler_c;
}


#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4355 )
#endif


// Begin Namespace IsoAgLib
namespace __IsoAgLib {

class Process_c : public Scheduler_Task_c
{
  MACRO_MULTITON_CONTRIBUTION();
public:
  void init( void );
  void close( void );

  virtual ~Process_c() {}

  bool processMsg( const CanPkg_c& arc_data );

  DevPropertyHandler_c& getDevPropertyHandlerInstance( void );

  ProcData_c* procData( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoNameReceiver, bool& elementFound);

  bool timeEvent();

  void resetTimerPeriod();

  void registerAccessFlt() {}

  bool registerLocalProcessData( ProcData_c* pc_localClient)
  { bool b_result = registerC1( pc_localClient ); mpc_iter = c_arrClientC1.begin(); return b_result;}
  void unregisterLocalProcessData( ProcData_c* pc_localClient)
  { unregisterC1( pc_localClient ); mpc_iter = c_arrClientC1.begin();}

  void reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);

  bool processTcStatusMsg(uint8_t ui8_tcStatus, const __IsoAgLib::IsoName_c& sender);

  const IsoName_c& getISONameFromType( IsoAgLib::ProcData::remoteType_t ecuType ) const;
  IsoAgLib::ProcData::remoteType_t getTypeFromISOName( const IsoName_c& isoName ) const;

#if DEBUG_SCHEDULER
  virtual const char* getTaskName() const;
#endif

  /** set the pointer to the handler class (used for callback when TC status message is processed)
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcDataHandler( IsoAgLib::iProcDataHandler_c *apc_procDataHandler )
   { mpc_procDataHandler = apc_procDataHandler; }

  /** send NACK */
  void sendNack( const IsoName_c& ac_da,
                 const IsoName_c& ac_sa,
                 int16_t ddi,
                 int16_t element,
                 IsoAgLib::ProcData::nackResponse_t a_errorcodes) const;

private: // Private methods
  //! Function set ui16_earlierInterval and
  //! ui16_laterInterval that will be used by
  //! getTimeToNextTrigger(retriggerType_t)
  //! can be overloaded by Childclass for special condition
  virtual void updateEarlierAndLatestInterval();

  virtual uint16_t getForcedMinExecTime() const
  {
    return getForcedMinExecTimeDefault();
  }

  void stopRunningMeasurement(IsoAgLib::ProcData::remoteType_t ecuType);

private: // Private attributes

  class CanCustomerProxy_c : public CanCustomer_c {
  public:
    typedef Process_c Owner_t;

    CanCustomerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

    virtual ~CanCustomerProxy_c() {}

  private:
    virtual bool processMsg( const CanPkg_c& arc_data ) {
      return mrt_owner.processMsg( arc_data );
    }

    Owner_t &mrt_owner;

  private:
    // not copyable
    CanCustomerProxy_c(CanCustomerProxy_c const &);
    CanCustomerProxy_c &operator=(CanCustomerProxy_c const &);
  };
  typedef CanCustomerProxy_c Customer_t;
  class ControlFunctionStateHandlerProxy_c : public ControlFunctionStateHandler_c {
  public:
    typedef Process_c Owner_t;

    ControlFunctionStateHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

    virtual ~ControlFunctionStateHandlerProxy_c() {}

  private:
    virtual void reactOnIsoItemModification(
        iIsoItemAction_e at_action,
        IsoItem_c const &acrc_isoItem)
    {
      mrt_owner.reactOnIsoItemModification(at_action, acrc_isoItem);
    }

    Owner_t &mrt_owner;

  private:
    // not copyable
    ControlFunctionStateHandlerProxy_c(ControlFunctionStateHandlerProxy_c const &);
    ControlFunctionStateHandlerProxy_c &operator=(ControlFunctionStateHandlerProxy_c const &);
  };
  typedef ControlFunctionStateHandlerProxy_c Handler_t;

  /**
    HIDDEN constructor for a Process_c object instance
    NEVER instantiate a variable of type Process_c within application
    only access Process_c via getProcessInstance() or getProcessInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
    */
  Process_c() :
    mt_handler(*this),
    mt_customer(*this),
    mc_isoNameTC(IsoName_c::IsoNameUnspecified()),
#ifdef USE_DATALOGGER
    mc_isoNameLogger(IsoName_c::IsoNameUnspecified()),
#endif
    CONTAINER_CLIENT1_CTOR_INITIALIZER_LIST
  {}

  /**
    deliver reference to process pkg as reference to DevPropertyHandler_c which
    handles sending and processing of messages from can
  */
  DevPropertyHandler_c mc_devPropertyHandler;

  bool m_lastActiveTaskTC;
  IsoName_c mc_isoNameTC;

#ifdef USE_DATALOGGER
  bool m_lastActiveTaskLogger;
  IsoName_c mc_isoNameLogger;
#endif

  /** pointer to applications handler class, with handler functions
      which shall be called when a TC status message arrives
  */
  IsoAgLib::iProcDataHandler_c* mpc_procDataHandler;

  Handler_t mt_handler;
  Customer_t mt_customer;
  CONTAINER_CLIENT1_MEMBER_FUNCTIONS_MAIN(ProcData_c);
  cacheTypeC1_t mpc_iter;

  friend Process_c &getProcessInstance( uint8_t aui8_instance );
};


/** C-style function, to get access to the unique Process_c singleton instance
  * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
  */
Process_c &getProcessInstance( uint8_t aui8_instance = 0 );
}

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

#endif
