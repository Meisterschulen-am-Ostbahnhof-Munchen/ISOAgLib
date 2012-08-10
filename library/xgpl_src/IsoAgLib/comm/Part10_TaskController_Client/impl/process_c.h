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
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/ProcDataLocal_c.h>
#include "processpkg_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/processdatachangehandler_c.h>
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
  /** initialisation for Process_c */
  void init( void );
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close( void );

  virtual ~Process_c() {}

  /**
    process msg
    @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
  */
  bool processMsg( const CanPkg_c& arc_data );

  DevPropertyHandler_c& getDevPropertyHandlerInstance( void );

  /**
    checks if a suitable ProcDataLocal_c item exist
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param acrc_isoNameReceiver isoName code of searched local Process Data instance
    @return true -> suitable instance found
  */
  bool existProcDataLocal( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoNameReceiver);

  /**
    search for suitable ProcDataLocal_c item

    possible errors:
        * Err_c::elNonexistent if element not found
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param acrc_isoNameReceiver isoName code of searched local Process Data instance
    @return reference to searched ProcDataLocal_c instance
  */
  ProcDataLocal_c& procDataLocal( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoNameReceiver);

  /**
    performs periodically actions
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );

  /** called when a new measurement is started */
  void resetTimerPeriod( void );

  /** register pointer to a new local process data instance
    * this function is called within construction of new local process data instance
    */
  bool registerLocalProcessData( ProcDataLocal_c* pc_localClient)
  { return registerC1( pc_localClient );}

  /** unregister pointer to a already registered local process data instance
    * this function is called within destruction of local process data instance
    */
  void unregisterLocalProcessData( ProcDataLocal_c* pc_localClient)
  { unregisterC1( pc_localClient );}

  /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
   * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
   * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
   */
  void reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);

  /**
    process TC status messages:
    - task status suspended: stop running measurement (started by default data logging)
    @param ui8_tcStatus
    @param rc_isoName         device key of TC
    @param ab_skipLastTcStatus true => don't check for changed TC status
    @return true
  */
  bool processTcStatusMsg(uint8_t ui8_tcStatus, const IsoName_c& rc_isoName, bool ab_skipLastTcStatus = false);

  /**
    @return isoName, saved from TC status messages
  */
  const IsoName_c* getTcISOName() { return mpc_tcISOName; };

#if DEBUG_SCHEDULER
  virtual const char* getTaskName() const;
#endif

  /** set the pointer to the handler class (used for callback when TC status message is processed)
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler )
   { mpc_processDataChangeHandler = apc_processDataChangeHandler; }

protected:
  //! Function set ui16_earlierInterval and
  //! ui16_laterInterval that will be used by
  //! getTimeToNextTrigger(retriggerType_t)
  //! can be overloaded by Childclass for special condition
  virtual void updateEarlierAndLatestInterval();

private: // Private methods

  virtual uint16_t getForcedMinExecTime() const
  {
    return getForcedMinExecTimeDefault();
  }

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

    virtual bool reactOnStreamStart(
        ReceiveStreamIdentifier_c const &ac_ident,
        uint32_t aui32_totalLen)
    {
      return reactOnStreamStartDefault(ac_ident, aui32_totalLen);
    }

    virtual void reactOnAbort(Stream_c &arc_stream)
    {
      reactOnAbortDefault(arc_stream);
    }

    virtual bool processPartStreamDataChunk(
        Stream_c &apc_stream,
        bool ab_isFirstChunk,
        bool ab_isLastChunk)
    {
      return processPartStreamDataChunkDefault(apc_stream,ab_isFirstChunk,ab_isLastChunk);
    }

    virtual void notificationOnMultiReceiveError(
        ReceiveStreamIdentifier_c const &ac_streamIdent,
        uint8_t aui8_multiReceiveError,
        bool ab_isGlobal)
    {
      notificationOnMultiReceiveErrorDefault(ac_streamIdent,aui8_multiReceiveError,ab_isGlobal);
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
    CONTAINER_CLIENT1_CTOR_INITIALIZER_LIST
  {}

  /**
    deliver reference to process pkg as reference to DevPropertyHandler_c which
    handles sending and processing of messages from can
  */
  DevPropertyHandler_c mc_devPropertyHandler;

  //STL_NAMESPACE::list<IsoName_c> ml_filtersToDeleteISO;
  const IsoName_c* mpc_tcISOName;
  uint8_t mui8_lastTcStatus;

  /** pointer to applications handler class, with handler functions
      which shall be called when a TC status message arrives
  */
  IsoAgLib::ProcessDataChangeHandler_c* mpc_processDataChangeHandler;

  Handler_t mt_handler;
  Customer_t mt_customer;
  CONTAINER_CLIENT1_MEMBER_FUNCTIONS_MAIN(ProcDataLocal_c);

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
