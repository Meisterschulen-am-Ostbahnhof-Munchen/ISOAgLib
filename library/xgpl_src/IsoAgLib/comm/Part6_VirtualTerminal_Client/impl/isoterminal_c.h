/*
  isoterminal_c.h: central ISO terminal management

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISO_TERMINAL_H
#define ISO_TERMINAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/iisoterminalobjectpool_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include "vtserverinstance_c.h"
#include "vtclientservercommunication_c.h"

#include <list>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class VtClientServerCommunication_c;
class iIdentItem_c;

/** central IsoAgLib terminal management object */
class IsoTerminal_c : public Scheduler_Task_c {
#if 1 < PRT_INSTANCE_CNT
  MACRO_MULTITON_CONTRIBUTION();
#else
  MACRO_SINGLETON_CONTRIBUTION();
#endif
public:
  virtual ~IsoTerminal_c() {}

  /** initialise element which can't be done during construct and registerIsoObjectPool
    possible errors:
  */
  void init();

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close();

  VtClientServerCommunication_c* initAndRegisterIsoObjectPool (IdentItem_c& apc_wsMasterIdentItem, IsoAgLib::iIsoTerminalObjectPool_c& arc_pool, char* apc_versionLabel);

  VtClientServerCommunication_c* initAndRegisterIsoObjectPoolForSlave (IdentItem_c& apc_slaveIdentItem, IsoAgLib::iIsoTerminalObjectPool_c& arc_pool);

  bool deregisterIsoObjectPool (IdentItem_c& apc_wsMasterIdentItem);

  /** periodically event -> call timeEvent for all  identities and parent objects
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent(void);

  /** @todo SOON-241 Remove this - this should only be temporary and a good solution needs to be implemented! */
  void TEMPORARYSOLUTION_setTimePeriod(uint16_t aui16_timePeriod) { setTimePeriod (aui16_timePeriod); }

  /** function that handles incoming can messages */
  virtual bool processMsg();

  bool sendCommandForDEBUG(IsoAgLib::iIdentItem_c& apc_wsMasterIdentItem, uint8_t* apui8_buffer, uint32_t ui32_size);

  /** deliver reference to data pkg
    @return reference to the member IsoTerminalPkg_c, which encapsulates the CAN send structure
  */
  CanPkgExt_c& data(){return mc_data;};

  /** deliver reference to data pkg as reference to CanPkgExt_c
    to implement the base virtual function correct
  */
  virtual CanPkgExt_c& dataBase();


  VtClientServerCommunication_c& getClientByID (uint8_t ui8_clientIndex) { return *mvec_vtClientServerComm[ui8_clientIndex]; }

  VtClientServerCommunication_c* getClientPtrByID (uint8_t ui8_clientIndex) { return (!mvec_vtClientServerComm.empty()) ? mvec_vtClientServerComm[ui8_clientIndex] : NULL; }

  bool isAnyVtAvailable() { return !ml_vtServerInst.empty(); }

  ////////////////////////
  // INTERFACE FUNTIONS //
  ////////////////////////
// the following define should be globally defined in the project settings...
#ifdef FAKE_VT_PROPERTIES
  void fakeVtProperties (uint16_t aui16_dimension, uint16_t aui16_skWidth, uint16_t aui16_skHeight, uint8_t aui16_colorDepth, uint16_t aui16_fontSizes);
#endif

#if DEBUG_SCHEDULER
  virtual const char* getTaskName() const;
#endif

private:
  uint16_t getClientCount();

  class CanCustomerProxy_c : public CanCustomer_c {
  public:
    typedef IsoTerminal_c Owner_t;

    CanCustomerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

    virtual ~CanCustomerProxy_c() {}

  private:
    virtual CanPkgExt_c& dataBase() {
      return mrt_owner.dataBase();
    }

    virtual bool processMsg() {
      return mrt_owner.processMsg();
    }

    virtual bool processInvalidMsg() {
      return mrt_owner.processInvalidMsg();
    }

    virtual bool isNetworkMgmt() const {
      return mrt_owner.isNetworkMgmt();
    }

    virtual bool reactOnStreamStart(
        ReceiveStreamIdentifier_c const &ac_ident,
        uint32_t aui32_totalLen)
    {
      return mrt_owner.reactOnStreamStart(ac_ident, aui32_totalLen);
    }

    virtual void reactOnAbort(Stream_c &arc_stream)
    {
      mrt_owner.reactOnAbort(arc_stream);
    }

    virtual bool processPartStreamDataChunk(
        Stream_c &apc_stream,
        bool ab_isFirstChunk,
        bool ab_isLastChunk)
    {
      return mrt_owner.processPartStreamDataChunk(
          apc_stream,
          ab_isFirstChunk,
          ab_isLastChunk);
    }

    virtual void notificationOnMultiReceiveError(
        ReceiveStreamIdentifier_c const &ac_streamIdent,
        uint8_t aui8_multiReceiveError,
        bool ab_isGlobal)
    {
      mrt_owner.notificationOnMultiReceiveError(
          ac_streamIdent,
          aui8_multiReceiveError,
          ab_isGlobal);
    }

    // CanCustomerProxy_c shall not be copyable. Otherwise the
    // reference to the containing object would become invalid.
    CanCustomerProxy_c(CanCustomerProxy_c const &);

    CanCustomerProxy_c &operator=(CanCustomerProxy_c const &);

    Owner_t &mrt_owner;
  };
  typedef CanCustomerProxy_c Customer_t;
  class ControlFunctionStateHandlerProxy_c : public ControlFunctionStateHandler_c {
  public:
    typedef IsoTerminal_c Owner_t;

    ControlFunctionStateHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

    virtual ~ControlFunctionStateHandlerProxy_c() {}

  private:
    virtual void reactOnIsoItemModification(
        IsoItemModification_t at_action,
        IsoItem_c const &acrc_isoItem)
    {
      mrt_owner.reactOnIsoItemModification(at_action, acrc_isoItem);
    }

    // ControlFunctionStateHandlerProxy_c shall not be copyable. Otherwise the
    // reference to the containing object would become invalid.
    ControlFunctionStateHandlerProxy_c(ControlFunctionStateHandlerProxy_c const &);

    ControlFunctionStateHandlerProxy_c &operator=(ControlFunctionStateHandlerProxy_c const &);

    Owner_t &mrt_owner;
  };
  typedef ControlFunctionStateHandlerProxy_c Handler_t;

  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of IsoTerminal_c within application
    */
  IsoTerminal_c();

  /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
   * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
   * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
   */
  virtual void reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem);

  virtual bool processInvalidMsg(){
    return false;
  }

  virtual bool isNetworkMgmt() const {
    return false;
  }

  virtual bool reactOnStreamStart(
      ReceiveStreamIdentifier_c const &ac_ident,
      uint32_t aui32_totalLen)
  {
    return mt_customer.reactOnStreamStartDefault(ac_ident, aui32_totalLen);
  }

  virtual void reactOnAbort(Stream_c &arc_stream)
  {
    mt_customer.reactOnAbortDefault(arc_stream);
  }

  virtual bool processPartStreamDataChunk(
      Stream_c &apc_stream,
      bool ab_isFirstChunk,
      bool ab_isLastChunk)
  {
    return mt_customer.processPartStreamDataChunkDefault(
        apc_stream,
        ab_isFirstChunk,
        ab_isLastChunk);
  }

  virtual void notificationOnMultiReceiveError(
      ReceiveStreamIdentifier_c const &ac_streamIdent,
      uint8_t aui8_multiReceiveError,
      bool ab_isGlobal)
  {
    mt_customer.notificationOnMultiReceiveErrorDefault(
        ac_streamIdent,
        aui8_multiReceiveError,
        ab_isGlobal);
  }

  virtual uint16_t getForcedMinExecTime() const
  {
    return getForcedMinExecTimeDefault();
  }

  VtClientServerCommunication_c* initAndRegisterIsoObjectPoolCommon (IdentItem_c& rc_identItem, IsoAgLib::iIsoTerminalObjectPool_c& arc_pool, char* apc_versionLabel, bool ab_isSlave);

protected:

private: // attributes

  /** temp data where received data is put */
  CanPkgExt_c mc_data;

  STL_NAMESPACE::list<VtServerInstance_c> ml_vtServerInst;

  STL_NAMESPACE::vector<VtClientServerCommunication_c*> mvec_vtClientServerComm;
  Handler_t mt_handler;
  Customer_t mt_customer;
  friend IsoTerminal_c &getIsoTerminalInstance(uint8_t aui8_instance);
};

/** C-style function, to get access to the unique Scheduler_c singleton instance
 * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
 */
IsoTerminal_c &getIsoTerminalInstance(uint8_t aui8_instance = 0);

/** this typedef is only for some time to provide backward compatibility at API level */
typedef IsoTerminal_c ISOTerminal_c;

}
#endif
