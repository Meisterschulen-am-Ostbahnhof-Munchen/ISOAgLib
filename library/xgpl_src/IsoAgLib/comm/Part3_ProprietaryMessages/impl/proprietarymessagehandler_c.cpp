/*
  proprietarymessagehandler_c.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "proprietarymessagehandler_c.h"

#include <list>


/**
  ISO 11783 proprietary message
  61184 = EF00 hex Prop A

  65280 = FF00 hex Prop B_00

  65535 = FFFF hex Prop B_255

  126720 = 1EF00 hex Prop A2
*/


namespace __IsoAgLib
{
  ProprietaryMessageHandler_c::ProprietaryMessageHandler_c()
    : mt_handler(*this)
    , mt_customer(*this)
  { // nop
  }


  /** initialize directly after the singleton instance is created.
  */
  void ProprietaryMessageHandler_c::singletonInit()
  {
    mc_data.setSingletonKey( getSingletonVecKey() );
    setAlreadyClosed();
    init();
  }

  /**
      die Parameter in init(##### const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode #####) werden
      wahrscheinlich nicht gebraucht
    */
  void ProprietaryMessageHandler_c::init()
  {
    if (checkAlreadyClosed())
    {
      getSchedulerInstance().registerClient( this );
      // register to get ISO monitor list changes
      __IsoAgLib::getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( mt_handler );
      mb_hardTiming = false;
      setTimePeriod (27012); // wait some silly large time as we have really NOTHING to do scheduled!
      clearAlreadyClosed();
    }
  }

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void ProprietaryMessageHandler_c::close( )
  {
    if ( ! checkAlreadyClosed() )
    {
      // avoid another call
      setAlreadyClosed();
      // unregister from timeEvent() call by Scheduler_c
      getSchedulerInstance().unregisterClient( this );
      // unregister ISO monitor list changes
      __IsoAgLib::getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( mt_handler );
    }
  }

  /** DESTRUCTOR
    */
  ProprietaryMessageHandler_c::~ProprietaryMessageHandler_c()
  {
    close();
  }

  /** register the proprietary message client pointer in an internal list of all clients.
      Derive and register from the member attributes:
          ui32_can, mui32_canFilter, mc_isonameRemoteECU, mpc_localIdent
      the suitable CAN message receive filters.
      The internal implementation will take care to adapt the receive filter as soon as
      the SA of the remote or local is changed.
      @return true if client is registered otherwise false
    */
  void ProprietaryMessageHandler_c::registerProprietaryMessageClient (ProprietaryMessageClient_c* apc_proprietaryclient)
  {
    // look in the whole list
    for ( ProprietaryMessageClientVectorConstIterator_t client_iterator = mvec_proprietaryclient.begin(); client_iterator != mvec_proprietaryclient.end(); client_iterator++ )
    { // client is registered
      if ( (*client_iterator).pc_client ==  apc_proprietaryclient )
        return;
    }
    /* define receive filter with "no"-values
    scui32_noFilter marks the whole filter as "not set"*/
    IsoFilter_s s_tempIsoFilter(mt_customer, scui32_noMask, scui32_noFilter, NULL, NULL);
    ClientNode_t t_tempClientNode (apc_proprietaryclient, s_tempIsoFilter);
    // push back new client
    mvec_proprietaryclient.push_back (t_tempClientNode);
  }

  /** deregister a ProprietaryMessageClient */
  void ProprietaryMessageHandler_c::deregisterProprietaryMessageClient (ProprietaryMessageClient_c* apc_proprietaryclient)
  {
    /* define receive filter with "no"-values*/
    apc_proprietaryclient->defineReceiveFilter (scui32_noMask, scui32_noFilter, screfc_noIsoName, spc_nolocalIdent);
    // look in the whole list
    for ( ProprietaryMessageClientVectorIterator_t client_iterator = mvec_proprietaryclient.begin(); client_iterator != mvec_proprietaryclient.end(); client_iterator++ )
    {
      // same Client -> erase client from list
      if ( (*client_iterator).pc_client == apc_proprietaryclient )
      {
        mvec_proprietaryclient.erase (client_iterator);
        break;
      }
    }
  }


  void
  ProprietaryMessageHandler_c::triggerClientDataUpdate(
    ProprietaryMessageClient_c &arc_proprietaryclient,
    bool ab_forceFilterRemoval)
  {
    // default for "ab_forceFilterRemoval"
    IsoFilter_s s_newIsoFilter (mt_customer, scui32_noMask, scui32_noFilter, NULL, NULL);

    if (ab_forceFilterRemoval)
    { // keep default "s_newIsoFilter" from above.
    }
    else
    { // check if client's filter is valid and can be retrieved
      if ( (arc_proprietaryclient.mpc_localIdent != NULL ) && (!arc_proprietaryclient.mpc_localIdent->isClaimedAddress()) )
      { // client not yet ready
        // don't need to reconfigure, will be configured at
        // the next action anyway (like AddToMonitorList/ReclaimedAddress)
        return;
      }
      else
      { // client has a valid filter, so use it!
        s_newIsoFilter = arc_proprietaryclient.getCurrentFilter (mt_customer);
      }
    }

    // look in the whole list
    for ( ProprietaryMessageClientVectorIterator_t client_iterator = mvec_proprietaryclient.begin();
          client_iterator != mvec_proprietaryclient.end();
          ++client_iterator )
    {
      // if client is found in the list
      if ( (*client_iterator).pc_client == &arc_proprietaryclient )
      {
        // Have the filter settings changed?
        if ( (*client_iterator).s_isoFilter != s_newIsoFilter)
        {
          /// ## if old filter is not equal to "no filter"
          if ((*client_iterator).s_isoFilter.getFilter() != scui32_noFilter)
          {
            /** delete filter */
            __IsoAgLib::getIsoFilterManagerInstance4Comm().removeIsoFilter ((*client_iterator).s_isoFilter);
            /** deregister at multi-receive */
            getMultiReceiveInstance4Comm().deregisterClient (mt_customer,
                                                             (*client_iterator).s_isoFilter.getIsoNameDa(),
                                                             (*client_iterator).s_isoFilter.getFilter() >> 8,
                                                             (*client_iterator).s_isoFilter.getMask() >> 8);
          }

          /// ## if new filter is not equal to "no filter"
          if (s_newIsoFilter.getFilter() != scui32_noFilter)
          {
            //  insert new filter
            __IsoAgLib::getIsoFilterManagerInstance4Comm().insertIsoFilter (s_newIsoFilter);
            /** register for multi-receive */
            getMultiReceiveInstance4Comm().registerClientIso (mt_customer,
                                                              s_newIsoFilter.getIsoNameDa(),
                                                              s_newIsoFilter.getFilter() >> 8,
                                                              s_newIsoFilter.getMask() >> 8,
                                                              true /* also Broadcast */);
          }
          // update filter and mask
          (*client_iterator).s_isoFilter = s_newIsoFilter;
          // update was performed
          return;
        }
        // else: filters are equal, no need to update.
      } // if (client found)
    } // for
  }


  void
  ProprietaryMessageHandler_c::reactOnIsoItemModification(
    ControlFunctionStateHandler_c::IsoItemModification_t at_action,
    IsoItem_c const& acrc_isoItem)
  {
    switch (at_action)
    {
    case ControlFunctionStateHandler_c::AddToMonitorList: // a "defineReceiveFilter" could have occurred after RemoveFromMonitorList, so re-check!
    case ControlFunctionStateHandler_c::ReclaimedAddress: // a "defineReceiveFilter" could have occurred after LostAddress, so re-check!
    case ControlFunctionStateHandler_c::RemoveFromMonitorList:  // when removed, force filters to "NoFilter" in "triggerClientDataUpdate
      for ( ProprietaryMessageClientVectorConstIterator_t client_iterator = mvec_proprietaryclient.begin();
            client_iterator != mvec_proprietaryclient.end();
            ++client_iterator )
      {
        // look for the ident (if Proprietary B messages no ident is there)
        if ( (*client_iterator).pc_client->mpc_localIdent)
        {
          // address has claimed -> isoItem
          if ( (*client_iterator).pc_client->mpc_localIdent->getIsoItem() == &acrc_isoItem )
          {
            // insert/update filter now
            triggerClientDataUpdate(
              *((*client_iterator).pc_client),
              (at_action == ControlFunctionStateHandler_c::RemoveFromMonitorList)); // Remove? -> true (=forceFilterRemoval)
          }
        }
      }
      break;

    case ControlFunctionStateHandler_c::ChangedAddress: // nothing could have happened in between
    case ControlFunctionStateHandler_c::LostAddress: // nothing to be done, filters anyway not active.
      // nothing to do, because the filters are registered with ISONAME anyway.
      break;
    } // switch
  }


  void ProprietaryMessageHandler_c::sendData(ProprietaryMessageClient_c& client)
  {
    /** get a CAN instance */
    CanIo_c& c_can = getCanInstance4Comm();

    /** get data from client */
    IsoAgLib::iGenericData_c& rc_sendData = client.getDataSend();

    /** length <= 8 Bytes */
    if (rc_sendData.getLen() <= 8)
    { /** single packet */
      /** sets the saved ident */
      mc_data.setIdent (rc_sendData.getIdent(), Ident_c::ExtendedIdent);

      /** if PGN is proprietary A1/A2 PGN then add destination address */
      if ( ( ( mc_data.isoPgn()) & 0x2FF00 ) == PROPRIETARY_A_PGN)
      { /** add destination address */
        mc_data.setISONameForDA( client.mc_isonameRemoteECU );
      }
      if (client.mpc_localIdent == NULL)
      { /** Sending with 0xFE as SA */
        mc_data.setISONameForSA (screfc_noIsoName);
      }
      else
      { /** Sending with LocalIdent as SA */
        /* The localIdent will always have an ISOName - if it's not yet unified,
            then the sending will not find it in the IsoMonitor_c-list so simply no
            packet is being sent */
        mc_data.setISONameForSA (client.mpc_localIdent->isoName());
      }

      /** default priority for Proprietary PGN as stated in PGN's definition in ISO 11783-3 */
      mc_data.setIsoPri(6);
      /** set data */
      mc_data.setDataFromString (rc_sendData.getDataStream(), rc_sendData.getLen());
      /** sending */
      c_can << mc_data;
    }
    else
    { /** multi-packet */
      /** variable should be evaluated */
      // const bool cb_couldStartMultiSend =
      // we could catch the information if the sending succeeded, but what to do with it anyway?
      getMultiSendInstance4Comm().sendIsoTarget (client.mpc_localIdent->isoName(),
                                                  client.mc_isonameRemoteECU,
                                                  rc_sendData.getDataStream(0),
                                                  rc_sendData.getLen(),
                                                  rc_sendData.getIdent() >> 8,
                                                  client.men_sendSuccess);
    }
  }


  void ProprietaryMessageHandler_c::updateTimePeriod (ProprietaryMessageClient_c* pc_nextClient, bool ab_fromTimeEvent)
  {
    if (pc_nextClient != NULL)
    { // we have a client requesting to send up next...
      mb_hardTiming = true;
      if (ab_fromTimeEvent)
      { // can't set fix retrigger time, so setting the timePeriod
        // (Note: retrigger isn't needed/allowed from timeEvent)
        int32_t i32_idleTimeSpread = pc_nextClient->mui32_nextSendTimeStamp - HAL::getTime();
        if (i32_idleTimeSpread < 0) i32_idleTimeSpread = 0;
        setTimePeriod (i32_idleTimeSpread);
      }
      else
      { // from outside, so set the nextSendTimeStamp needs to be retriggered
        getSchedulerInstance().changeRetriggerTimeAndResort (this, pc_nextClient->mui32_nextSendTimeStamp);
      }
    }
    else
    {
      mb_hardTiming = false;
      setTimePeriod (27012); // wait some silly large time as we have really NOTHING to do scheduled!
    }
  }


  /** Call updateSchedulingInformation() if client's nextTriggering has been changed */
  void ProprietaryMessageHandler_c::updateSchedulingInformation()
  {
    ProprietaryMessageClient_c* pc_nextClient = NULL;

    for ( ProprietaryMessageClientVectorIterator_t client_iterator = mvec_proprietaryclient.begin(); client_iterator != mvec_proprietaryclient.end(); client_iterator++ )
    {
      if ( (*client_iterator).pc_client->mui32_sendPeriodicMsec != 0)
      { // yes, the client wants periodic sending
        if ( pc_nextClient == NULL )
        { // not yet a next client found, so simply take this one
          pc_nextClient = (*client_iterator).pc_client;
        }
        else
        { // there was already a next client, so see if this one has to be on time earlier?
          if ( ((*client_iterator).pc_client->mui32_nextSendTimeStamp) < (pc_nextClient->mui32_nextSendTimeStamp) )
          { // yes, this one is next (so far)
            pc_nextClient = (*client_iterator).pc_client;
          }
        }
      }
    }
    updateTimePeriod (pc_nextClient, false);
  }


  bool ProprietaryMessageHandler_c::reactOnStreamStart (const ReceiveStreamIdentifier_c& ac_ident, uint32_t /** aui32_totalLen */)
  {
    // if remote_ECU is specified and not the ident's SA is the remote_ECU -> don't react on stream
    // look in the whole list
    for ( ProprietaryMessageClientVectorIterator_t client_iterator = mvec_proprietaryclient.begin(); client_iterator != mvec_proprietaryclient.end(); client_iterator++ )
    {
      if ( ( (ac_ident.getPgn() << 8) & (*client_iterator).pc_client->mui32_canMask) ==  (*client_iterator).pc_client->mui32_canFilter )
      { // PGN check okay, if unspecified there is no special request
        if ( ( (*client_iterator).pc_client->mc_isonameRemoteECU.isUnspecified() || ((*client_iterator).pc_client->mc_isonameRemoteECU == ac_ident.getSaIsoName() )))
        { // SA check okay
          if ( (ac_ident.getDaIsoName() == screfc_noIsoName.toConstIisoName_c()) /* msg addressed to global => OKAY */
                ||
                ( (*client_iterator).pc_client->mpc_localIdent == NULL) /* we have no identity => OKAY */
                ||
                ( (*client_iterator).pc_client->mpc_localIdent->isoName() == ac_ident.getDaIsoName() ) /* we have an identity and it's addressed to us => OAKY */
              )
          { // DA check okay
            return true; // accept this stream!
          }
        }
      }
    }
    return false;
  }


  bool
  ProprietaryMessageHandler_c::processPartStreamDataChunk (Stream_c& arc_stream, bool, bool ab_isLastChunk)
  {
    // if last byte is received
    if (ab_isLastChunk)
    {
      // get the ident from the stream
      const ReceiveStreamIdentifier_c& ac_ident = arc_stream.getIdent();
      std::vector<ProprietaryMessageClientVectorIterator_t> vec_consumers;

      // look for the right sender
      for ( ProprietaryMessageClientVectorIterator_t client_iterator = mvec_proprietaryclient.begin(); client_iterator != mvec_proprietaryclient.end(); ++client_iterator )
      {
        // check to process the information
        if ( ( (ac_ident.getPgn() << 8) & (*client_iterator).pc_client->mui32_canMask) ==  (*client_iterator).pc_client->mui32_canFilter )
        { // PGN check okay, if unspecified there is no special request
          if ( ( (*client_iterator).pc_client->mc_isonameRemoteECU.isUnspecified() || ((*client_iterator).pc_client->mc_isonameRemoteECU == ac_ident.getSaIsoName())))
          { // SA check okay
            if ( (ac_ident.getDaIsoName() == screfc_noIsoName.toConstIisoName_c()) /* msg addressed to global => OKAY */
                ||
                ((*client_iterator).pc_client->mpc_localIdent == NULL) /* we have no identity => OKAY */
                ||
                ((*client_iterator).pc_client->mpc_localIdent->isoName() == ac_ident.getDaIsoName() ) /* we have an identity and it's addressed to us => OAKY */
              )
            { // DA check okay
              if ( ( (ac_ident.getPgn() >> 8 ) & 0x00FF ) < 0xF0 )
              {
                /** Proprietary_A_PGN and Proprietary_A2_PGN depends on destination address */
                (*client_iterator).pc_client->ms_receivedData.setIdent( (ac_ident.getPgn() << 8) | (ac_ident.getDa() << 8) | ac_ident.getSa() );
              }
              else
              {
                /** Proprietary_B_PGN --> no destination address */
                (*client_iterator).pc_client->ms_receivedData.setIdent( (ac_ident.getPgn() << 8) | ac_ident.getSa() );
              }

              (*client_iterator).pc_client->ms_receivedData.clearVector();
              vec_consumers.push_back(client_iterator);
            }
          }
        }
      }
      if ( !vec_consumers.empty() )
      { // now feed data in all detected and already prepared consumers
        /** get first data byte */
        uint8_t ui8_databyte =arc_stream.getFirstByte();
        /** loop to the last data byte */
        for (uint32_t ui32_cnt = 0; ui32_cnt < arc_stream.getByteTotalSize(); ui32_cnt++)
        {
          /** extract data bytes from string and store data in receive buffer */
          for ( std::vector<ProprietaryMessageClientVectorIterator_t>::iterator iter_consumer = vec_consumers.begin(); iter_consumer != vec_consumers.end(); ++iter_consumer)
          { // feed data in all clients
            (*iter_consumer)->pc_client->ms_receivedData.setDataUi8( ui32_cnt,ui8_databyte );
          }
          ui8_databyte = arc_stream.getNextNotParsed();
        }
        for ( std::vector<ProprietaryMessageClientVectorIterator_t>::iterator iter_consumer = vec_consumers.begin(); iter_consumer != vec_consumers.end(); ++iter_consumer)
        { /** call process message from client to evaluate the stored data */
          (*iter_consumer)->pc_client->processProprietaryMsg();
        }
      }
    }
    // don't keep the stream - we processed it right now!
    return false;
  }


  /**  Operation:  Function for Debugging in Scheduler_c
  */
  const char* ProprietaryMessageHandler_c::getTaskName() const
  {
    return "ProprietaryMessageHandler_c()";
  }


  bool ProprietaryMessageHandler_c::processMsg()
  {
    // look in the whole list
    for ( ProprietaryMessageClientVectorIterator_t client_iterator = mvec_proprietaryclient.begin(); client_iterator != mvec_proprietaryclient.end(); client_iterator++ )
    {
      if ( ( (mc_data.isoPgn() << 8) & (*client_iterator).pc_client->mui32_canMask) == (*client_iterator).pc_client->mui32_canFilter )
      { // PGN check okay
        if ( ( (*client_iterator).pc_client->mc_isonameRemoteECU.isUnspecified() || ((*client_iterator).pc_client->mc_isonameRemoteECU == mc_data.getISONameForSA())))
        { // SA check okay
          // following check checks for PROPRIETARY_A_PGN and PROPRIETARY_A2_PGN at once!
          if ( (mc_data.isoPgn() & 0x2FF00) == PROPRIETARY_A_PGN)
          { // DA check
            if ( ! ( (mc_data.getISONameForDA() == screfc_noIsoName) /* msg addressed to global => OKAY */
                    ||
                    ((*client_iterator).pc_client->mpc_localIdent == NULL) /* we have no identity => OKAY */
                    ||
                    ((*client_iterator).pc_client->mpc_localIdent->isoName() == mc_data.getISONameForDA()) /* we have an identity and it's addressed to us => OAKY */
               )   )
            { // DA did NOT match
              continue;
            }
          }
          /** reset vector */
          (*client_iterator).pc_client->ms_receivedData.clearVector();
          // Destination Address matched
          (*client_iterator).pc_client->ms_receivedData.setIdent( mc_data.ident() );
          /** set data bytes along with len */
          (*client_iterator).pc_client->ms_receivedData.setDataStream(0, mc_data.getUint8DataConstPointer(), mc_data.getLen());
          /** process message from client */
          (*client_iterator).pc_client->processProprietaryMsg();
          /** ms_receivedData will NOT be cleared here in case the client
              wants the data to remain. it can clear it itself in processProprietaryMsg() */
        }
      }
    }
    return (true);
  }


  void ProprietaryMessageHandler_c::updateEarlierAndLatestInterval()
  {
    if (mb_hardTiming)
    {
      mui16_earlierInterval = 0; //( ( getTimePeriod() * 3) / 4);
      mui16_latestInterval = ( getTimePeriod() / 2);
      if (mui16_latestInterval > 20) mui16_latestInterval = 20; // allow max. 20ms latest-jitter if we have hard-timing enabled!
    }
    else
    {
      mui16_earlierInterval = ( ( getTimePeriod() * 3) / 4);
      mui16_latestInterval = ( getTimePeriod() / 2);
    }
  }


  bool ProprietaryMessageHandler_c::timeEvent()
  {
    ProprietaryMessageClient_c* pc_nextClient = NULL;

    for ( ProprietaryMessageClientVectorIterator_t client_iterator = mvec_proprietaryclient.begin(); client_iterator != mvec_proprietaryclient.end(); client_iterator++ )
    {
      // don't care for right now if client has a localIdent set or is claimed...
      if ( (*client_iterator).pc_client->mui32_sendPeriodicMsec != 0)
      { // yes, the client wants periodic sending
        const uint32_t cui32_timestamp = HAL::getTime();
        // has its time come?
        if (cui32_timestamp >= (*client_iterator).pc_client->mui32_nextSendTimeStamp)
        { // send the data out!
          (*client_iterator).pc_client->mui32_nextSendTimeStamp = cui32_timestamp + (*client_iterator).pc_client->mui32_sendPeriodicMsec;
          sendData(*(*client_iterator).pc_client);
        }
        // go for it, send it
        if ( pc_nextClient == NULL )
        { // not yet a next client found, so simply take this one
          pc_nextClient = (*client_iterator).pc_client;
        }
        else
        { // there was already a next client, so see if this one has to be on time earlier?
          if ( ((*client_iterator).pc_client->mui32_nextSendTimeStamp) < (pc_nextClient->mui32_nextSendTimeStamp) )
          { // yes, this one is next (so far)
            pc_nextClient = (*client_iterator).pc_client;
          }
        }
      }
    }

    updateTimePeriod (pc_nextClient, true);

    return (true);
  }

};


