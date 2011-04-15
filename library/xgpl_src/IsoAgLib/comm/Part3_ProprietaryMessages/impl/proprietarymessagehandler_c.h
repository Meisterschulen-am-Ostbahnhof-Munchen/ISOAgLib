/*
  proprietarymessagehandler_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef PROPRIETARYMESSAGEHANDLER_H
#define PROPRIETARYMESSAGEHANDLER_H

#include "proprietarymessageclient_c.h"

#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/saclaimhandler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisofilter_s.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>

#include <cstdlib>	// Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>


namespace __IsoAgLib
{
  /** initialization parameter for local ident */
  static const IdentItem_c* spc_nolocalIdent = NULL;

  /** initialization parameter for IsoName */
  static const IsoName_c& screfc_noIsoName = IsoName_c::IsoNameUnspecified();

  class ProprietaryMessageHandler_c : public Scheduler_Task_c
  {
#if 1 < PRT_INSTANCE_CNT
    MACRO_MULTITON_CONTRIBUTION();
#else
    MACRO_SINGLETON_CONTRIBUTION();
#endif
  public:
    ProprietaryMessageHandler_c();

    virtual ~ProprietaryMessageHandler_c() {}

    /** initialisation */
    void init( void );

    /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
    void close( void );

    /** overloading processMsg() to get can-messages
      */
    virtual bool processMsg( const CanPkg_c& arc_data );

    /** overloading reactOnStreamStart to get (E)TP-messages */
    virtual bool reactOnStreamStart (const ReceiveStreamIdentifier_c& ac_ident, uint32_t aui32_totalLen);

    /** implementing processPartStreamDataChunk to process (E)TP-messages */
    virtual bool processPartStreamDataChunk (Stream_c& apc_stream, bool ab_isFirstChunk, bool ab_isLastChunk);

    /** performs periodically actions,
      possible errors:
        * partial error caused by one of the memberItems
      @return true -> all planned activities performed in allowed time
    */
    virtual bool timeEvent( void );

    /** register the proprietary message client pointer in an interanl list of all clients.
        Derive and register from the member attributes:

            mui32_canMask, mui32_canFilter, mc_isonameRemoteECU, mpc_localIdent

        the suitable CAN message receive filters.
        The internal implementation will take care to adapt the receive filter as soon as
        the SA of the remote or local is changed.
    */
    void registerProprietaryMessageClient (ProprietaryMessageClient_c* apc_proprietaryclient);

    /** deregister ProprietaryMessageClient */
    void deregisterProprietaryMessageClient (ProprietaryMessageClient_c* apc_proprietaryclient);

    /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
     * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
     * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
     */
    void reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);

#if DEBUG_SCHEDULER
    virtual const char* getTaskName() const;
#endif

    /** force an update of the CAN receive filter (if possible), as initial or
        new data has been set in an already registered client.
        @param arc_proprietaryclient the registered client.
        @param ab_forceFilterRemoval true => force removal of the filter for now
                                     (without changing it to "NoFilter" in the client!)
    */
    void triggerClientDataUpdate(
      ProprietaryMessageClient_c &arc_proprietaryclient,
      bool ab_forceFilterRemoval);

    /** send the data in
            ProprietaryMessageClient_c::ms_sendData
        the data can be accessed directly by
           iProprietaryMessageHandler_c as its a friend of ProprietaryMessageClient_c
        Repeated sending will NOT be handled here (anymore)
     */
    void sendData(ProprietaryMessageClient_c& client);

    /** struct to store proprietary message clients with filter and mask
      */
    struct ClientNode_t
    {
      ClientNode_t (ProprietaryMessageClient_c* apc_client,
                    const IsoFilter_s& rrefcs_isoFilter) : pc_client(apc_client), s_isoFilter (rrefcs_isoFilter) {}

      ProprietaryMessageClient_c* pc_client;
      IsoFilter_s s_isoFilter;
    };

    /** type of map which is used to store proprietary clients */
    typedef STL_NAMESPACE::vector<ClientNode_t> ProprietaryMessageClientVector_t;
    typedef STL_NAMESPACE::vector<ClientNode_t>::iterator ProprietaryMessageClientVectorIterator_t;
    typedef STL_NAMESPACE::vector<ClientNode_t>::const_iterator ProprietaryMessageClientVectorConstIterator_t;

    /** Call updateSchedulingInformation() if client's nextTriggering has been changed */
    void updateSchedulingInformation();

  protected:
    //! Function set ui16_earlierInterval and
    //! ui16_laterInterval that will be used by
    //! getTimeToNextTrigger(retriggerType_t)
    //! can be overloaded by Childclass for special condition
    virtual void updateEarlierAndLatestInterval();

  private:
    void updateTimePeriod (ProprietaryMessageClient_c* pc_nextClient, bool ab_fromTimeEvent);

  private:
private:
  class CanCustomerProxy_c : public CanCustomer_c {
  public:
    typedef ProprietaryMessageHandler_c Owner_t;

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
      (void)arc_stream;
      // nop - mrt_owner.reactOnAbort(arc_stream);
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
      (void)ac_streamIdent;
      (void)aui8_multiReceiveError;
      (void)ab_isGlobal;
      // nop - mrt_owner.notificationOnMultiReceiveError(
      //     ac_streamIdent,
      //     aui8_multiReceiveError,
      //     ab_isGlobal);
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
    typedef ProprietaryMessageHandler_c Owner_t;

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

    /** dynamic array of memberItems for handling
        of single member informations
    */
    ProprietaryMessageClientVector_t mvec_proprietaryclient;

    bool mb_hardTiming;

    Handler_t mt_handler;
    Customer_t mt_customer;
    friend ProprietaryMessageHandler_c &getProprietaryMessageHandlerInstance(uint8_t aui8_instance);
  };

  /** C-style function, to get access to the unique ProprietaryMesageHandler_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  ProprietaryMessageHandler_c &getProprietaryMessageHandlerInstance(uint8_t aui8_instance);

}
#endif


