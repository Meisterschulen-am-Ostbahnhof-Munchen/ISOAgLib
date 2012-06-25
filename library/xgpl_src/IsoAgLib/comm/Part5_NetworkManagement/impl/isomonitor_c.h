/*
  isomonitor_c.h: object for monitoring members (list of IsoItem_c)

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISO_MONITOR_H
#define ISO_MONITOR_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/config.h>

#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include "isoitem_c.h"
#include "saclaimhandler_c.h"
#include "isorequestpgnhandler_c.h"
#include "identitem_c.h"

#include <map>

#include <list>
namespace IsoAgLib { class iIsoMonitor_c;}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** type of map which is used to store ControlFunctionStateHandler_c clients corresponding to a IsoName_c reference */
typedef STL_NAMESPACE::vector<ControlFunctionStateHandler_c*> ControlFunctionStateHandlerVector_t;
typedef STL_NAMESPACE::vector<ControlFunctionStateHandler_c*>::iterator ControlFunctionStateHandlerVectorIterator_t;
typedef STL_NAMESPACE::vector<ControlFunctionStateHandler_c*>::const_iterator ControlFunctionStateHandlerVectorConstIterator_t;

/** this object manages a monitor list of all
  ISO members including inserting and administration of local own members.
  @short Manager for members of Scheduler_c (IsoItem_c)
  @see ISOItem
  @author Dipl.-Inform. Achim Spangler
*/
class IsoMonitor_c : public Scheduler_Task_c
{
  MACRO_MULTITON_CONTRIBUTION();
private:
  // private typedef alias names
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::list<IsoItem_c,MALLOC_TEMPLATE(IsoItem_c) > Vec_ISO;
  typedef STL_NAMESPACE::list<IsoItem_c,MALLOC_TEMPLATE(IsoItem_c) >::iterator Vec_ISOIterator;
  typedef STL_NAMESPACE::list<IsoItem_c,MALLOC_TEMPLATE(IsoItem_c) >::const_iterator Vec_ISOIteratorConst;
  #else
  typedef STL_NAMESPACE::list<IsoItem_c> Vec_ISO;
  typedef STL_NAMESPACE::list<IsoItem_c>::iterator Vec_ISOIterator;
  typedef STL_NAMESPACE::list<IsoItem_c>::const_iterator Vec_ISOIteratorConst;
  #endif

public:
  /** initialisation for IsoMonitor_c which can store optional pointer to central Scheduler_c instance */
  void init( void );

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close( void );

  /** register a properly set-up IdentItem.
      @return false in case the IdentItem couldn't be properly activated
  */
  bool registerIdentItem( IdentItem_c& arc_item );

  /** deregister a registered IdentItem
  */
  void deregisterIdentItem( IdentItem_c& arc_item );

  /** performs periodically actions,
    possible errors:
      * partial error caused by one of the memberItems
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );

  virtual ~IsoMonitor_c() {}


  /** deliver amount of ISO members in monitor list which optional (!!)
    match the condition of address claim state
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return amount of ISO members with claimed address
  */
  uint8_t isoMemberCnt(bool ab_forceClaimedAddress = false)
    {return isoMemberDevClassCnt(0xFF, ab_forceClaimedAddress);}

  /** deliver the n'th ISO member in monitor list which optional (!!)
    match the condition of address claim state
    check first with isoMemberCnt if enough members are registered
    in Monitor-List
    @see isoMemberCnt
    possible errors:
      * Err_c::range there exist less than aui8_ind members with claimed address
    @param aui8_ind position of the wanted member in the
                 sublist of members (first item has aui8_ind == 0 !!)
                 with the wanted property
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return reference to searched element
  */
  IsoItem_c& isoMemberInd(uint8_t aui8_ind, bool ab_forceClaimedAddress = false)
    {return isoMemberDevClassInd(0xFF, aui8_ind, ab_forceClaimedAddress);}

  /** deliver the count of members in the Monitor-List with given ECU-Type (which is an own IsoAgLib-definition!)
    which optional (!!) match the condition of address claim state
    @param a_ecuType searched ECU-Type code
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with ECU-Type == a_ecuType
  */
  uint8_t isoMemberEcuTypeCnt (IsoName_c::ecuType_t a_ecuType, bool ab_forceClaimedAddress = false);

  /** deliver one of the members with specific ECU_Type (which is an own IsoAgLib-definition!)
    which optional (!!) match the condition of address claim state
    check first with isoMemberEcuTypeCnt if enough members with wanted ECU-Type and
    optional (!!) property are registered in Monitor-List
    @see isoMemberEcuTypeCnt
    possible errors:
      * Err_c::range there exist less than aui8_ind members with ECU-Type a_ecuType
    @param a_ecuType searched ECU-Type code
    @param aui8_ind position of the wanted member in the
                  sublist of member with given ECU-Type (first item has aui8_ind == 0 !!)
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return reference to searched element
  */
  IsoItem_c& isoMemberEcuTypeInd (IsoName_c::ecuType_t a_ecuType, uint8_t aui8_ind, bool ab_forceClaimedAddress = false);

  /** deliver the count of members in the Monitor-List with given DEVCLASS
    which optional (!!) match the condition of address claim state
    @param aui8_devClass searched DEVCLASS code
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with DEVCLASS == aui8_devClass
  */
  uint8_t isoMemberDevClassCnt(uint8_t aui8_devClass, bool ab_forceClaimedAddress = false);

  /** deliver the count of members in the Monitor-List with given FUNCTION
    and device classe which optional (!!) match the condition of address claim state
    @param aui8_function searched FUNCTION code
    @param aui8_devClass searched DEVCLASS code
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with FUNCTION == aui8_function and DEVCLASS == aui8_devClass
  */
  uint8_t isoMemberDevClassFuncCnt(uint8_t aui8_devClass, uint8_t aui8_function, bool ab_forceClaimedAddress);

  /** deliver one of the members with specific DEVCLASS
    which optional (!!) match the condition of address claim state
    check first with isoMemberDevClassCnt if enough members with wanted DEVCLASS and
    optional (!!) property are registered in Monitor-List
    @see isoMemberDevClassCnt
    possible errors:
      * Err_c::range there exist less than aui8_ind members with DEVCLASS aui8_devClass
   @param aui8_devClass searched DEVCLASS
   @param aui8_ind position of the wanted member in the
                 sublist of member with given DEVCLASS (first item has aui8_ind == 0 !!)
   @param ab_forceClaimedAddress true -> only members with claimed address are used
         (optional, default false)
   @return reference to searched element
  */
  IsoItem_c& isoMemberDevClassInd(uint8_t aui8_devClass, uint8_t aui8_ind, bool ab_forceClaimedAddress = false);

  /** check if a memberItem with given ISOName exist
    which optional (!!) match the condition of address claim state
    and update local mpc_isoMemberCache
    @param acrc_isoName searched ISOName
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return true -> searched member exist
  */
  bool existIsoMemberISOName(const IsoName_c& acrc_isoName, bool ab_forceClaimedAddress = false);

  /** check if a member with given number exist
    which optional (!!) match the condition of address claim state
    and update local mpc_isoMemberCache
    @param aui8_nr searched member number
    @return true -> item found
  */
  bool existIsoMemberNr(uint8_t aui8_nr);

  /**
    check if member is in member list with wanted ISOName,
    adapt instance if member with claimed address with other device class inst exist
    @param rc_isoName ISOName to search (-> it's updated if member with claimed address with other dev class inst is found)
    @return true -> member with claimed address with given DEVCLASS found (and rc_isoName has now its ISOName)
  */
  bool isoDevClass2ISONameClaimedAddress(IsoName_c &rc_isoName);

  /** insert a new IsoItem_c in the list; with unset aui8_nr the member is initiated as
    address claim state; otherwise the given state can be given or state Active is used
    possible errors:
      * Err_c::badAlloc not enough memory to insert new IsoItem_c instance
      * Err_c::busy another member with same ident exists already in the list
    @param acrc_isoName ISOName of the member
    @param aui8_nr member number
    @param ren_state wanted status
    @param apc_identItemForLocalItems back reference to the IdentItem, set if the IsoItem is local...
    @param ab_announceAddition
    @return pointer to new IsoItem_c or NULL if not succeeded
  */
  IsoItem_c* insertIsoMember(const IsoName_c& acrc_isoName, uint8_t aui8_nr = 0xFF,
                             IState_c::itemState_t ren_state = IState_c::Active, IdentItem_c* apc_identItemForLocalItems = NULL, bool ab_announceAddition=false);


  /** deliver the amount of local members which matches the searched proto types
      @return amount of registered local members with wanted type
     */
  uint8_t localIsoMemberCnt();

  /** deliver pointer to local member of wanted type by index
      @see localMemberCnt
      @param aui8_ind index of wanted member (first item == 0)
      @return pointer to wanted local member (NULL if no suitable MonitorItem_c found)
      (MonitorItem_c is base class of IsoItem_c which serves address, isoName, itemState)
     */
  IsoItem_c& localIsoMemberInd( uint8_t aui8_ind );

  /** check if one of the own local members is active with claimed address at ISO11783
      @return true -> at least one of the own iso identities is active with claimed address at ISO11783
      @see SystemMgmt_c::getActiveLocalMember
     */
  bool existActiveLocalIsoMember();

  /** delivers reference to the first active local member;
      send of ISO11783 system msg demands telling a sender ident member no
      -> using the number of the first active member serves as default (f.e. for requesting other member names)
      can throw an preconditionViolation error, if none of the own identities is active/claimed address yet
          possible errors:
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident
      @return reference to the MonitorItem_c of the first active local member
      (MonitorItem_c is baes class of IsoItem_c which serves
      adress, isoName, itemState)
      @exception preconditionViolation
     */
  IsoItem_c& getActiveLocalIsoMember();

  /** check for own ident with given member no
      @param aui8_nr member no to search for
      @return true -> one of the own identities has the wanted member no
     */
  bool existLocalIsoMemberNr (uint8_t aui8_nr);

  /** check for own ident with given ISOName
      @param acrc_isoName              ISOName to search for
      @param ab_forceClaimedAddress true -> only members with claimed address are used
      (optional, default false)
      @return true -> one of the own identities has the wanted ISOName
     */
  bool existLocalIsoMemberISOName (const IsoName_c& acrc_isoName, bool ab_forceClaimedAddress = false);

  /** register a ControlFunctionStateHandler_c */
  void registerControlFunctionStateHandler (ControlFunctionStateHandler_c & arc_client);

  /** deregister a ControlFunctionStateHandler */
  void deregisterControlFunctionStateHandler (ControlFunctionStateHandler_c & arc_client);

  void broadcastIsoItemModification2Clients( ControlFunctionStateHandler_c::IsoItemModification_t at_isoItemModification, IsoItem_c const& acrc_isoItem ) const;

  /**
    deliver member item with given isoName
    (check with existIsoMemberISOName before access to not defined item)
    possible errors:
      * Err_c::elNonexistent on failed search
    @param acrc_isoName searched ISOName
    @return reference to searched ISOItem
     @exception containerElementNonexistant
  */
  IsoItem_c& isoMemberISOName(const IsoName_c& acrc_isoName, bool ab_forceClaimedAddress = false);

  /** deliver member item with given nr
    (check with existIsoMemberNr before access to not defined item)
    possible errors:
      * Err_c::elNonexistent on failed search
    @param aui8_nr searched number
    @return reference to searched ISOItem
     @exception containerElementNonexistant
  */
  IsoItem_c& isoMemberNr(uint8_t aui8_nr);

  /**
    delete item with specified isoName
    possible errors:
      * Err_c::elNonexistent no member with given ISOName exists
    @param acrc_isoName ISOName of to be deleted member
  */
  bool deleteIsoMemberISOName(const IsoName_c& acrc_isoName);

#if 0
  /** delete item with specified member number
    possible errors:
      * Err_c::elNonexistent no member with given ISOName exists
    @param aui8_nr number of to be deleted member
  */
  bool deleteIsoMemberNr(uint8_t aui8_nr);
#endif

  /** check if SA of an announcing IsoItem_c is unique and deliver
    another free SA if not yet unique (else deliver its actual SA if unique yet)
    @param apc_isoItem pointer to announcing IsoItem_c
    @param ab_resolveConflict true => don't use current SA because someone else
                                      claimed it with a higher prior isoname,
                                      so we have to change our SA. This is needed
                                      as the new member is not yet in the list,
                                      so the algorithm would still take the current SA.
                                      We can't insert the new item, as we don't want
                                      a state where two items have the same SA.
                              false => no conflict to resolve, so we can take the current
                                       source address if it's available!
    @return free unique SA (if possible the actual SA of the pointed item)
      (if wanted SA is not free for NOT-self-conf item or if no free SA is available
       254 is answered -> special flag for NACK)
  */
  uint8_t unifyIsoSa(const IsoItem_c* apc_isoItem, bool ab_resolveConflict);

  /** deliver timestamp of last ISO request for SA claim msg
    @return time of last Request PG for Adress Claim on BUS
  */
  int32_t lastIsoSaRequest() const {return mi32_lastSaRequest;}

  /** set timestamp of last ISO request for SA claim msg
    @param ai32_time set timestamp to ai32_time or use actual time on default

    changed by M.Wodok so that NO system time is used,
    because REQUEST_FOR_CLAIMED_ADDRESS may be processed AFTER
    ADDRESS_CLAIM, so it's NOT chronologically. So CAN-Pkg-Times
    should be used here instead!!
  */
  void setLastIsoSaRequest (int32_t ai32_time/* = -1*/) {mi32_lastSaRequest = /*(ai32_time != -1)?*/ai32_time/*:System_c::getTime()*/;}

  /** trigger a request for claimed addreses
    @param ab_force false -> send request only if no request was detected until now
    @return true -> request was sent
   */
  bool sendRequestForClaimedAddress( bool ab_force, IsoItem_c *sender );

  /** get the Xth slave of the master isoItem
    @param index tells which slave of the working set to get
    @param apc_masterItem this is the master of which we want all the slaves
  */
  IsoItem_c* getSlave (uint8_t index, IsoItem_c* apc_masterItem);

  /** get the number of slaves attached to the given working set master
    @param apc_masterItem this is the master of which we want to know how many slaves he has
  */
  uint8_t getSlaveCount (IsoItem_c* apc_masterItem);

  virtual bool processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* apc_isoItemReceiver, int32_t ai_requestTimestamp );

#if DEBUG_SCHEDULER
  virtual const char* getTaskName() const;
#endif

  /// Function notify Scheduler_c to set new retriggerTime
  /// will be called from IdentItem_c and registerClient
  /// IsoMonitor_c.timeEvent() should be called in 50 ms
  void changeRetriggerTime() { getSchedulerInstance().changeRetriggerTimeAndResort(this,System_c::getTime() + 50 );};

#if DEBUG_ISOMONITOR
  void debugPrintNameTable();
#endif


protected: // Protected methods
  /** process system msg with informations which are
    important for managing of members
    (this function is only called if NO conflict is detected)
    @return true -> message processed by IsoMonitor_c; false -> process msg by ServiceMonitor
  */
  bool processMsg( const CanPkg_c& arc_data );

  //! Function set ui16_earlierInterval and
  //! ui16_laterInterval that will be used by
  //! getTimeToNextTrigger(retriggerType_t)
  //! can be overloaded by Childclass for special condition
  virtual void updateEarlierAndLatestInterval();

private:
  /** constructor for IsoMonitor_c which can store optional pointer to central Scheduler_c instance */
  IsoMonitor_c( void );

  Vec_ISOIterator internalIsoItemErase( Vec_ISOIterator aiter_toErase);

private:
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

  virtual void reactOnAbort(Stream_c &arc_stream)
  {
    mt_customer.reactOnAbortDefault(arc_stream);
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

  virtual bool reactOnStreamStart(
      ReceiveStreamIdentifier_c const &ac_ident,
      uint32_t aui32_totalLen)
  {
    return mt_customer.reactOnStreamStartDefault(ac_ident, aui32_totalLen);
  }

  virtual uint16_t getForcedMinExecTime() const
  {
    return getForcedMinExecTimeDefault();
  }

  class CanCustomerProxy_c : public CanCustomer_c {
  public:
    typedef IsoMonitor_c Owner_t;

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
  class IsoRequestPgnHandlerProxy_c : public IsoRequestPgnHandler_c {
  public:
    typedef IsoMonitor_c Owner_t;

    IsoRequestPgnHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

    virtual ~IsoRequestPgnHandlerProxy_c() {}

  private:
    virtual bool processMsgRequestPGN(
        uint32_t aui32_pgn,
        IsoItem_c *apc_isoItemSender,
        IsoItem_c *apc_isoItemReceiver,
        int32_t ai_requestTimestamp )
    {
      return mrt_owner.processMsgRequestPGN(
          aui32_pgn,
          apc_isoItemSender,
          apc_isoItemReceiver,
          ai_requestTimestamp );
    }

    // IsoRequestPgnHandlerProxy_c shall not be copyable. Otherwise
    // the reference to the containing object would become invalid.
    IsoRequestPgnHandlerProxy_c(IsoRequestPgnHandlerProxy_c const &);

    IsoRequestPgnHandlerProxy_c &operator=(IsoRequestPgnHandlerProxy_c const &);

    Owner_t &mrt_owner;
  };
  typedef IsoRequestPgnHandlerProxy_c Handler_t;

  // Private attributes
  friend class IsoAgLib::iIsoMonitor_c;

  /** Cache pointer to speedup access to first active
    MonitorItem_c local member after check for existance
     */
  IsoItem_c* mpc_activeLocalMember;

  /** dynamic array of memberItems for handling
      of single member informations
  */
  Vec_ISO mvec_isoMember;

  /** last time of request for adress claim */
  int32_t mi32_lastSaRequest;

  /** temporary memberItem instance for better inserting of new elements */
  IsoItem_c mc_tempIsoMemberItem;

  /** cache pointer to speed serial of access to the same ISOItem */
  Vec_ISOIterator mpc_isoMemberCache;
  /** map of ControlFunctionStateHandler_c clients that want to be informed on monitor list changes */

  ControlFunctionStateHandlerVector_t mvec_saClaimHandler;

  Handler_t mt_handler;
  Customer_t mt_customer;

  CONTAINER_CLIENT1_MEMBER_FUNCTIONS_MAIN(IdentItem_c);
  friend IsoMonitor_c &getIsoMonitorInstance( uint8_t aui8_instance );
};

/** C-style function, to get access to the unique IsoMonitor_c singleton instance
 * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
 */
  IsoMonitor_c &getIsoMonitorInstance( uint8_t aui8_instance = 0 );

}
#endif
