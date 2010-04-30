/*
  process_c.h: central managing instance for all process data
    informations in the system

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCESS_H
#define PROCESS_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <functional>

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include "../Local/impl/procdatalocalbase_c.h"
#include "../Remote/impl/procdataremotebase_c.h"
#include "processpkg_c.h"
#include "../processdatachangehandler_c.h"
#include <IsoAgLib/comm/Part7_ProcessData/impl/processwsmtaskmsghandler_c.h>
#if defined(USE_PROC_DATA_DESCRIPTION_POOL)
  #include <IsoAgLib/comm/Part10_TaskController_Client/impl/devpropertyhandler_c.h>
#endif

#include <list>

namespace IsoAgLib { class iProcess_c;class iDevPropertyHandler_c;}

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
class Process_c;
typedef SINGLETON_DERIVED_CLIENT2(Process_c, Scheduler_Task_c, ProcDataLocalBase_c, ProcIdent_c, ProcDataRemoteBase_c, ProcIdent_c ) SingletonProcess_c;

/**
  Central managing instance for all process data
  informations in the system

  <b>Basic Rules for Matching of Received Messages)</b>
  1) primary match with LIS, DEVCLASS, ZAEHLNUM, WERT, INST
  2) distinguishing of more hits by POS
  \n
  <b>Acceptance Guidlines for Received Messages)</b>
  1) EMPF must fit to local member, SEND must be of existing member with claimed address
  2) special addition for Remote Process Data:
       alternatively SEND fit to owner of some Remote Data used to
       detect setpoints sent by owner of Remote Process Data
       -> possible to detect if local commander gets master or not
       (measure values are ignored by rule 2 unless MeasureProgRemote_c::receiveForeignMeasurement()
        was called for this ProcessDataRemote instance)
  \n
  The most functions of (i)Process_c are only relevant for the internal
  implementation, so that the interface to the API is quite small.
  Some of the internal tasks are:
  - handle list of pointers to all process data instances in the application
  - distribute the periodic timeEvent() call to all local process data instances,
    so that %e.g. value can be sent for measurement programs
  - forward incoming CAN messages to the appropriate process data to update values
    or to register measure progs, setpoints, etc.

  An overall description of Process Data management in IsoAgLib can be found in
  \ref ProcDataPage .

  @author Dipl.-Inform. Achim Spangler
*/
class Process_c : public SingletonProcess_c
{
public:
  /** initialisation for Process_c
  */
  void init( void );
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void );

  /** default destructor which has nothing to do */
  virtual ~Process_c();

  /**
    deliver reference to data pkg
    @return reference to ProcessPkg_c which handles CAN I/O of process data
  */
  ProcessPkg_c& data(){return mc_data;};
  /**
    deliver reference to data pkg as reference to CanPkgExt_c
    to implement the base virtual function correct
  */
  virtual CanPkgExt_c& dataBase();

  /**
    start processing of a process msg
    ignore all invalid messages where sender has no address (0xFE)
    @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
  */
  bool processMsg();

  // needed for bus-snooping!
  virtual bool isNetworkMgmt() const { return true; }

#if defined(USE_PROC_DATA_DESCRIPTION_POOL)
  DevPropertyHandler_c& getDevPropertyHandlerInstance( void );
#endif

  /**
    if the amount of created local process data is known, then enough capacity for the
    vector with pointers to all of them can be reserved. Otherwise the vector
    will increase with several reallocations, where each reallocation triggers
    increase of capacity by factor 2 ( capacity is the amount of elements,
    which can be stored before reallocation takes place ).
    @param aui16_localProcCapacity
  */
  void localProcDataReserveCnt( uint16_t aui16_localProcCapacity );
  /**
    if the amount of created remote process data is known, then enough capacity for the
    vector with pointers to all of them can be reserved. Otherwise the vector
    will increase with several reallocations, where each reallocation triggers
    increase of capacity by factor 2 ( capacity is the amount of elements,
    which can be stored before reallocation takes place ).
    @param aui16_remoteProcCapacity
  */
  void remoteProcDataReserveCnt( uint16_t aui16_remoteProcCapacity );

  /**
    checks if a suitable ProcDataLocalBase_c item exist
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param acrc_isoNameReceiver isoName code of searched local Process Data instance
    @return true -> suitable instance found
  */
  bool existProcDataLocal( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoNameReceiver);

  /**
    checks if a suitable ProcDataRemoteBase_c item exist
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param acrc_isoNameSender isoName of the sender (used for check against ownerISOName())
    @param acrc_isoNameReceiver isoName code of searched local Process Data instance
    @return true -> suitable instance found
  */
  bool existProcDataRemote( uint16_t aui16_DDI, uint16_t aui16_element,
                            const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver);

  /**
    search for suitable ProcDataLocalBase_c item; create on if not found AND if wanted

    possible errors:
        * Err_c::badAlloc not enough memory to add new ProcDataLocal_c
          (can cause badAlloc exception)
        * Err_c::elNonexistent if element not found and ab_doCreate == false
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param acrc_isoNameReceiver isoName code of searched local Process Data instance
    @return reference to searched/created ProcDataLocalBase_c instance
  */
  ProcDataLocalBase_c& procDataLocal( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoNameReceiver);

  /**
    search for suitable ProcDataRemoteBase_c item; create on if not found AND if wanted

    possible errors:
        * Err_c::badAlloc not enough memory to add new ProcDataRemote_c
          (can cause badAlloc exception)
        * Err_c::elNonexistent if element not found and ab_doCreate == false
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param acrc_isoNameSender isoName of the sender (used for check against isoName())
    @param acrc_isoNameReceiver isoName code of searched local Process Data instance
    @return reference to searched/created ProcDataRemoteBase_c instance
    @exception badAlloc
  */
 ProcDataRemoteBase_c& procDataRemote( uint16_t aui16_DDI, uint16_t aui16_element,
                                       const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver);


  /**
    delivers count of local process data entries with similar ident
    (which differs only in _instance_ of owner)
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param acrc_isoName isoName code of searched local Process Data instance
    @return count of similar local process data entries
  */
  uint8_t procDataLocalCnt( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoName);

  /**
    delivers count of remote process data entries with similar ident
    (which differs only in _instance_ of owner)
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param acrc_isoNameSender isoName of the sender (used for check against isoName())
    @param acrc_isoName isoName code of searched remote Process Data instance
    @return count of similar remote process data entries
  */
  uint8_t procDataRemoteCnt( uint16_t aui16_DDI, uint16_t aui16_element,
                             const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoName);

  /**
    performs periodically actions

    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );

  /** called when a new measurement is started */
  void resetTimerPeriod( void );

  /** handler function for access to undefined client.
    * the base Singleton calls this function, if it detects an error
    */
  void registerAccessFlt( void );

  /** register pointer to a new local process data instance
    * this function is called within construction of new local process data instance
    */
  bool registerLocalProcessData( ProcDataLocalBase_c* pc_localClient)
  { return registerC1( pc_localClient );}

  /** unregister pointer to a already registered local process data instance
    * this function is called within destruction of local process data instance
    */
  void unregisterLocalProcessData( ProcDataLocalBase_c* pc_localClient)
  { unregisterC1( pc_localClient );}

  /** register pointer to a new remote process data instance
    * this function is called within construction of new remote process data instance
    */
  bool registerRemoteProcessData( ProcDataRemoteBase_c* pc_remoteClient);

  /** unregister pointer to a already registered remote process data instance
    * this function is called within destruction of remote process data instance
    */
  void unregisterRemoteProcessData( ProcDataRemoteBase_c* pc_remoteClient);

  void registerWsmTaskMsgHandler(ProcessWsmTaskMsgHandler_c* apc_processWsmTaskMsgHandler)
  { mpc_processWsmTaskMsgHandler = apc_processWsmTaskMsgHandler; }

  /**
    delete FilterBox_c for receive from remote isoName if needed
    (important to delete old Filter Boxes after deletion of
    of remote device from monitor list or after re-adressclaim with different SA)
    @param acrc_isoName ISOName code of remote owner who sent the message
    @return true -> member exist and Filter Box deleted
  */
  bool deleteRemoteFilter(const IsoName_c& acrc_isoName);

  /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
   * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
   * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
   */
  void reactOnIsoItemModification (SaClaimHandler_c::IsoItemModification_t /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);

  /**
    process TC status messages:
    - task status suspended: stop running measurement (started by default data logging)
    @param ui8_tcStatus
    @param rc_isoName         device key of TC
    @param ab_skipLastTcStatus TRUE => don't check for changed TC status
    @return TRUE
  */
  bool processTcStatusMsg(uint8_t ui8_tcStatus, const IsoName_c& rc_isoName, bool ab_skipLastTcStatus = FALSE);

  /**
    @return isoName, saved from TC status messages
  */
  const IsoName_c* getTcISOName() { return mpc_tcISOName; };

  /**
    process working set task messages
    @param ui8_tcStatus
    @param rc_isoName         device key of working set
    @return TRUE
  */
  bool processWorkingSetTaskMsg(uint8_t /* ui8_tcStatus */, const IsoName_c& /* rc_isoName */);


  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;


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
  /**
    update the cache with search for according ProcDataLocalBase_c item
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param acrc_isoNameReceiver isoName code of created local Process Data instance
  */
  bool updateLocalCache( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoNameReceiver);

  /**
    update the cache with search for according ProcDataRemoteBase_c item
    ISO parameter
    @param aui16_DDI
    @param aui16_element
    @param acrc_isoNameSender isoName of the sender (used for check against isoName())
    @param acrc_isoNameReceiver isoName code of searched local Process Data instance
  */
  bool updateRemoteCache(uint16_t aui16_DDI, uint16_t aui16_element,
                         const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver);

  /**
   * check if any remote process data needs a new receive filter
   * @return true -> a remote filter has been created
   */
  bool checkCreateRemoteReceiveFilter();

  /**
    insert FilterBox_c for receive from remote isoName if needed
    @param acrc_isoName ISOName code of remote owner who sent the message
    @return true -> member exist and Filter Box created
  */
  bool createRemoteFilter(const IsoName_c& acrc_isoName);

  /** checks if a DDI can be added to a group and return ptr to proc data if successfully */
  ProcDataRemoteBase_c* addDDI2ExistingProcData(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& acrc_isoName, ProcessCmd_c::ValueGroup_t& ren_valueGroup, bool& rb_isSetpoint);

  /** checks if a DDI can be added to a group and if yes then add it! */
  bool checkAndAddMatchingDDI2Group(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& acrc_isoName);

  /** adds a proprietary DDI to a group */
  bool addProprietaryDDI2Group(uint16_t aui16_DDI, uint16_t aui_deviceElement, bool mb_isSetpoint, ProcessCmd_c::ValueGroup_t ddiType, const IsoName_c &acrc_isoName);

  /** checks if several DDI's can be summed up in groups */
  ProcDataRemoteBase_c* check4DDIGroupMatch(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& acrc_isoName);

  /** checks this DDI already exists in one ProcDataRemoteBase_c instance */
  bool check4DDIExisting(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& acrc_isoName);

  /** checks if proprietary DDI's can be summed up in groups */
  ProcDataRemoteBase_c* check4ProprietaryDDIGroupMatch(uint16_t aui_deviceElement, const IsoName_c& acrc_isoName);

  virtual bool processInvalidMsg() {
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

private: // Private attributes
  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit() { init(); };

  class CanCustomerProxy_c : public CanCustomer_c {
  public:
    typedef Process_c Owner_t;

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

#if defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL)
    virtual bool isProprietaryMessageOnStandardizedCan() const
    {
      return mrt_owner.isProprietaryMessageOnStandardizedCan();
    }
#endif

    // CanCustomerProxy_c shall not be copyable. Otherwise the
    // reference to the containing object would become invalid.
    CanCustomerProxy_c(CanCustomerProxy_c const &);

    CanCustomerProxy_c &operator=(CanCustomerProxy_c const &);

    Owner_t &mrt_owner;
  };
  typedef CanCustomerProxy_c Customer_t;
  class SaClaimHandlerProxy_c : public SaClaimHandler_c {
  public:
    typedef Process_c Owner_t;

    SaClaimHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

    virtual ~SaClaimHandlerProxy_c() {}

  private:
    virtual void reactOnIsoItemModification(
        IsoItemModification_t at_action,
        IsoItem_c const &acrc_isoItem)
    {
      mrt_owner.reactOnIsoItemModification(at_action, acrc_isoItem);
    }

    // SaClaimHandlerProxy_c shall not be copyable. Otherwise the
    // reference to the containing object would become invalid.
    SaClaimHandlerProxy_c(SaClaimHandlerProxy_c const &);

    SaClaimHandlerProxy_c &operator=(SaClaimHandlerProxy_c const &);

    Owner_t &mrt_owner;
  };
  typedef SaClaimHandlerProxy_c Handler_t;

  friend class SINGLETON_DERIVED(Process_c,Scheduler_Task_c);
  friend class IsoAgLib::iProcess_c;
  friend class IsoAgLib::iDevPropertyHandler_c;
  /**
    HIDDEN constructor for a Process_c object instance
    NEVER instantiate a variable of type Process_c within application
    only access Process_c via getProcessInstance() or getProcessInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
    */
  Process_c() :
    mt_handler(*this),
    mt_customer(*this)
  {};

  /** msg object for CAN I/O */
  ProcessPkg_c mc_data;

#if defined(USE_PROC_DATA_DESCRIPTION_POOL)
  /**
    deliver reference to process pkg as reference to DevPropertyHandler_c which
    handles sending and processing of messages from can
  */
  DevPropertyHandler_c mc_devPropertyHandler;
#endif

  /** last timestamp with FilterBox_c check */
  int32_t mi32_lastFilterBoxTime;

  STL_NAMESPACE::list<IsoName_c> ml_filtersToDeleteISO;
  bool mb_needCallOfCheckCreateRemoteReceiveFilter;
  const IsoName_c* mpc_tcISOName;
  uint8_t mui8_lastTcStatus;

  ProcessWsmTaskMsgHandler_c* mpc_processWsmTaskMsgHandler;

  /** pointer to applications handler class, with handler functions
      which shall be called when a TC status message arrives
  */
  IsoAgLib::ProcessDataChangeHandler_c* mpc_processDataChangeHandler;
  Handler_t mt_handler;
  Customer_t mt_customer;
};



#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Process_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  Process_c& getProcessInstance( uint8_t aui8_instance = 0 );
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  Process_c& getProcessInstance( void );
#endif

}
#endif
