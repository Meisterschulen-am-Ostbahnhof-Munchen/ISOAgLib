/*
  proprietarymessageclient_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "proprietarymessageclient_c.h"
#include "proprietarymessagehandler_c.h"

#include <IsoAgLib/driver/can/imaskfilter_c.h>

namespace __IsoAgLib
{

  /** default constructor
      initializes the parameter for filter and mask to "novalue"
    */
  ProprietaryMessageClient_c::ProprietaryMessageClient_c()
    : mui32_canMask(MASK_INVALID),
      mui32_canFilter(MASK_INVALID),                                                
      mc_isonameRemoteECU(screfc_noIsoName), 
      mpc_localIdent(spc_nolocalIdent),                                             
      mui32_sendPeriodicMsec(0),
      mui32_nextSendTimeStamp(0),
      men_sendSuccess(SendStream_c::SendSuccess)
  {
    /* register the client */
    getProprietaryMessageHandlerInstance4Comm().registerProprietaryMessageClient(this);
  }


  /** second constructor
      initializes the parameter to actual values
    */
  ProprietaryMessageClient_c::ProprietaryMessageClient_c( const IsoAgLib::iMaskFilter_c& acrc_maskFilter,
                                                          const IsoName_c& acrc_rremoteECU,
                                                          const IdentItem_c& apc_localIdent)
    : mpc_localIdent(NULL),
      mui32_sendPeriodicMsec(0),
      mui32_nextSendTimeStamp(0),
      men_sendSuccess(SendStream_c::SendSuccess)
  {
    /* register the client */
    getProprietaryMessageHandlerInstance4Comm().registerProprietaryMessageClient(this);
    /* define receive filter */
    defineReceiveFilter( acrc_maskFilter, acrc_rremoteECU, &apc_localIdent);
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
  bool ProprietaryMessageClient_c::defineReceiveFilter( const IsoAgLib::iMaskFilter_c& acrc_maskFilter, const IsoName_c& acrc_rremoteECU, const IdentItem_c* apc_localIdent)
  {
    // require a minimum mask, so that no other messages could be retrieved, but only PROP A/A2/B
    if ( ( acrc_maskFilter.empty() &&
           (acrc_rremoteECU == screfc_noIsoName) &&
           (apc_localIdent == spc_nolocalIdent)
         )
         ||
         ( ( ((acrc_maskFilter.getMask() & 0x2FF0000LU) == 0x2FF0000LU) && ((acrc_maskFilter.getFilter() & 0x2FF0000LU) == 0x0EF0000LU) ) || /** Proprietary A/A2 may be receivable with one filter/mask combination! */
           ( ((acrc_maskFilter.getMask() & 0x3FF0000LU) == 0x3FF0000LU) && ((acrc_maskFilter.getFilter() & 0x3FF0000LU) == 0x0FF0000LU) )    /** Proprietary B */
         )
       )
    {
      /** set actual values for filter, mask, remote and local ident */
      mui32_canMask = acrc_maskFilter.getMask();
      mui32_canFilter = acrc_maskFilter.getFilter();
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
    // See if we have to repeat the send somewhen, if so, let the
    // Handler know it so it can adjust its timeInterval accordingly.
    if (mui32_sendPeriodicMsec != 0)
    { // periodic sending is requested
      mui32_nextSendTimeStamp = HAL::getTime() + mui32_sendPeriodicMsec;
      getProprietaryMessageHandlerInstance4Comm().updateSchedulingInformation();
    }
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
        /// Currently the next message is not being sent out immediately,
        /// an explicit call to "sendDataToHandler" would be needed.
        mui32_nextSendTimeStamp = HAL::getTime() + mui32_sendPeriodicMsec;
      }
    }
    // set time period in msec
    mui32_sendPeriodicMsec = aui32_sendPeriodMsec;
    // notify scheduler to re-thing his scheduling after we've changed something now
    getProprietaryMessageHandlerInstance4Comm().updateSchedulingInformation();
  }


  IsoFilter_s
  ProprietaryMessageClient_c::getCurrentFilter (CanCustomer_c &arc_customer)
  {
    const IsoName_c* localIsoName = (
                                        (mpc_localIdent == NULL)
                                        ||
                                        (((mui32_canFilter & 0x3FF0000LU) >> 8) == PROPRIETARY_B_PGN)
                                       )
                                        ? NULL // if we have no IdentItem, we have no IsoName
                                        : &(mpc_localIdent->isoName());

    // create new IsoFilter
    return IsoFilter_s (arc_customer,
                        IsoAgLib::iMaskFilter_c( mui32_canMask, mui32_canFilter ),
                        localIsoName,
                        &mc_isonameRemoteECU);
  }

  void
  ProprietaryMessageClient_c::reactOnStateChange(const SendStream_c& sendStream)
  {
    men_sendSuccess = sendStream.getSendSuccess();
  }

};
