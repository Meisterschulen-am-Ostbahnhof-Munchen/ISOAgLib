/*
  proprietarymessageclient_c.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "proprietarymessageclient_c.h"
#include "proprietarymessagehandler_c.h"


namespace __IsoAgLib
{

  /** default constructor
      initializes the parameter for filter and mask to "novalue"
    */
  ProprietaryMessageClient_c::ProprietaryMessageClient_c(): mui32_canMask(scui32_noMask), mui32_canFilter(scui32_noFilter),
                                                            mc_isonameRemoteECU(screfc_noIsoName), mpc_localIdent(spc_nolocalIdent),
                                                            men_sendSuccess(MultiSend_c::SendSuccess)
  {
    setSendPeriodMsec(0);
    mui32_nextSendTimeStamp = 0;
    /* register the client */
    getProprietaryMessageHandlerInstance4Comm().registerProprietaryMessageClient(this);
  }


  /** second constructor
      initializes the parameter to actual values
    */
  ProprietaryMessageClient_c::ProprietaryMessageClient_c( uint32_t aui32_mask, uint32_t aui32_filter,
                                                          const IsoName_c& acrc_rremoteECU,
                                                          const IdentItem_c& apc_localIdent)
                                                          : mpc_localIdent(NULL),
                                                            men_sendSuccess(MultiSend_c::SendSuccess)
  {
    setSendPeriodMsec(0);
    mui32_nextSendTimeStamp = 0;
    /* register the client */
    getProprietaryMessageHandlerInstance4Comm().registerProprietaryMessageClient(this);
    /* define receive filter */
    defineReceiveFilter( aui32_mask, aui32_filter, acrc_rremoteECU, &apc_localIdent);
  }

  ProprietaryMessageClient_c::~ProprietaryMessageClient_c()
  {
    /* deregister the client */
    getProprietaryMessageHandlerInstance4Comm().deregisterProprietaryMessageClient(this);
  }

  /** will be used by ProprietaryMessageHandler_c for definition of CAN-Filter
      trigger an update of CAN receive filters with call of
        - ProprietaryMessageHandler_c::triggerClientDataUpdate()
      @return true when wanted PGN is in allowed range
    */
  bool ProprietaryMessageClient_c::defineReceiveFilter( uint32_t aui32_mask, uint32_t aui32_filter, const IsoName_c& acrc_rremoteECU, const IdentItem_c* apc_localIdent)
  {
    // require a minimum mask, so that no other messages could be retrieved, but only PROP A/A2/B
    if ( ( (aui32_mask == scui32_noMask) &&
           (aui32_filter == scui32_noFilter) &&
           (acrc_rremoteECU == screfc_noIsoName) &&
           (apc_localIdent == spc_nolocalIdent)
         )
         ||
         ( ( ((aui32_mask & 0x2FF0000) == 0x2FF0000) && ((aui32_filter & 0x2FF0000) == 0x0EF0000) ) || /** Proprietary A/A2 may be receivable with one filter/mask combination! */
           ( ((aui32_mask & 0x3FF0000) == 0x3FF0000) && ((aui32_filter & 0x3FF0000) == 0x0FF0000) )    /** Proprietary B */
         )
       )
    {
      /** set actual values for filter, mask, remote and local ident */
      mui32_canMask = aui32_mask;
      mui32_canFilter = aui32_filter;
      mc_isonameRemoteECU = acrc_rremoteECU;
      mpc_localIdent = apc_localIdent;

      /** force an update */
      getProprietaryMessageHandlerInstance4Comm().triggerClientDataUpdate (*this, false);
      return(true);
    }
    return(false);
  }

  /** function to tell "i will send data" to the handler */
  void ProprietaryMessageClient_c::sendDataToHandler()
  {
    getProprietaryMessageHandlerInstance4Comm().sendData(*this);
  }

  /** set time period in milliseconds for repeated send of the data that has been stored in c_sendData()
      only one message is sent when period == 0
    */
  void ProprietaryMessageClient_c::setSendPeriodMsec(uint32_t aui32_sendPeriodMsec)
  {
    // notify Handler because we may be next that need sending, so it may has to reschedule!
    /** see if we have to repeat the send somewhen and have to adjust our timeInterval accordingly? */
    if (aui32_sendPeriodMsec != 0)
    { // periodic sending is requested
      if (mui32_sendPeriodicMsec != 0)
      { // periodic sending was already activated
        mui32_nextSendTimeStamp -= mui32_sendPeriodicMsec;
        mui32_nextSendTimeStamp += aui32_sendPeriodMsec;
      }
      else
      { // no periodic sending was activated before, so just set the time to the next one!
        /* saving the time stamp of this client */
        mui32_nextSendTimeStamp = HAL::getTime() + mui32_sendPeriodicMsec;
      }
    }
    // set time period in msec
    mui32_sendPeriodicMsec = aui32_sendPeriodMsec;
    // notify scheduler to re-thing his scheduling after we've changed something now
    getProprietaryMessageHandlerInstance4Comm().updateSchedulingInformation (this);
  }


  IsoFilter_s
  ProprietaryMessageClient_c::getCurrentFilter (CanCustomer_c &arc_customer)
  {
    const IsoName_c& rc_localIsoName = (
                                        (mpc_localIdent == NULL)
                                        ||
                                        (((mui32_canFilter & 0x3FF0000) >> 8) == PROPRIETARY_B_PGN)
                                       )
                                        ? IsoName_c::IsoNameUnspecified() // if we have no IdentItem, we have no IsoName
                                        : mpc_localIdent->isoName();

    // create new IsoFilter
    return IsoFilter_s (arc_customer,
                        mui32_canMask,
                        mui32_canFilter,
                        &rc_localIsoName,
                        &mc_isonameRemoteECU);
  }

};
