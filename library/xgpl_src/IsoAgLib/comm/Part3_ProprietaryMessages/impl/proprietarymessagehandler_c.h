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

#include <IsoAgLib/comm/Part5_NetworkManagement/iisofilter_s.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>

#include <cstdlib>	// Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>


namespace __IsoAgLib
{
  /** initialization parameter for local ident */
  static const IdentItem_c* spc_nolocalIdent = NULL;

  /** initialization parameter for IsoName */
  static const IsoName_c& screfc_noIsoName = IsoName_c::IsoNameUnspecified();

  class ProprietaryMessageHandler_c;
  typedef SINGLETON_DERIVED(ProprietaryMessageHandler_c, Scheduler_Task_c) SingletonProprietaryMessageHandler_c;

  class ProprietaryMessageHandler_c : public SingletonProprietaryMessageHandler_c
  {
  public:
    /** default destructor which does just close() everything */
    virtual ~ProprietaryMessageHandler_c();

    /** initialisation */
    void init( void );

    /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
    void close( void );

    /** overloading processMsg() to get can-messages
      */
    virtual bool processMsg();

    /** overloading reactOnStreamStart to get (E)TP-messages */
    virtual bool reactOnStreamStart (const IsoAgLib::ReceiveStreamIdentifier_c& ac_ident, uint32_t aui32_totalLen);

    /** overloading processPartStreamDataChunk to process (E)TP-messages */
    virtual bool processPartStreamDataChunk (IsoAgLib::iStream_c& apc_stream, bool ab_isFirstChunk, bool ab_isLastChunk);

    /** performs periodically actions,
      possible errors:
        * partial error caused by one of the memberItems
      @return true -> all planned activities performed in allowed time
    */
    virtual bool timeEvent( void );

    /** deliver reference to data pkg as reference to CanPkgExt_c
      to implement the base virtual function correct
    */
    virtual CanPkgExt_c& dataBase()
    {
      return mc_data;
    }


    /** register the proprietary message client pointer in an interanl list of all clients.
        Derive and register from the member attributes:

            mui32_canMask, mui32_canFilter, mc_isonameRemoteECU, mpc_localIdent

        the suitable CAN message receive filters.
        The internal implementation will take care to adapt the receive filter as soon as
        the SA of the remote or local is changed.
    */
    bool registerProprietaryMessageClient (ProprietaryMessageClient_c* apc_proprietaryclient);

    /** deregister ProprietaryMessageClient */
    bool deregisterProprietaryMessageClient (ProprietaryMessageClient_c* apc_proprietaryclient);

    /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
     * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
     * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
     */
    void reactOnIsoItemModification (IsoItemModification_t /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);

    /** Function for Debugging in Scheduler_c */
    virtual const char* getTaskName() const;

    /** force an update of the CAN receive filter, as new data has been set in an already
        registered client.
        @return true, when the client has been found, so that an update has been performed
    */
    bool triggerClientDataUpdate(ProprietaryMessageClient_c* client);

    /** send the data in
            ProprietaryMessageClient_c::ms_sendData
        the data can be accessed directly by
           iProprietaryMessageHandler_c as its a friend of ProprietaryMessageClient_c
        the variable ui32_sendPeriodicMSec (in ProprietaryMessageClient_c) will be
        used to control repeated sending
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


    /**
      singletonInit() will be called exactly once directly after the singleton is created.
    */
    void singletonInit();

    /** Call updateSchedulingInformation() if client's nextTriggering has been changed */
    void updateSchedulingInformation(ProprietaryMessageClient_c* client);

  protected:
    //! Function set ui16_earlierInterval and
    //! ui16_laterInterval that will be used by
    //! getTimeToNextTrigger(retriggerType_t)
    //! can be overloaded by Childclass for special condition
    virtual void updateEarlierAndLatestInterval();

  private:
    void updateTimePeriod (ProprietaryMessageClient_c* pc_nextClient);

  private:
    friend class SINGLETON_DERIVED(IsoMonitor_c,Scheduler_Task_c);

    /** temp data where received and to be sent data is put */
    CanPkgExt_c mc_data;

    /** dynamic array of memberItems for handling
        of single member informations
    */
    ProprietaryMessageClientVector_t mvec_proprietaryclient;

    bool mb_hardTiming;
  };

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique ProprietaryMesageHandler_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline ProprietaryMessageHandler_c& getProprietaryMessageHandlerInstance(uint8_t aui8_instance)
  {
    return ProprietaryMessageHandler_c::instance( aui8_instance );
  }
#else
  /** C-style function, to get access to the unique ProprietaryMesageHandler_c singleton instance */
  inline ProprietaryMessageHandler_c& getProprietaryMessageHandlerInstance( void )
  {
    return ProprietaryMessageHandler_c::instance();
  }
#endif

};
#endif


