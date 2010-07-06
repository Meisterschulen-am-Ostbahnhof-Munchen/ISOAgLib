/*
  iproprietarymessageclient_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPROPRIETARYMESSAGECLIENT_C_H
#define IPROPRIETARYMESSAGECLIENT_C_H

#include "impl/proprietarymessageclient_c.h"

#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>


// Begin Namespace IsoAgLib
namespace IsoAgLib
{
  /** initialization parameter for local ident */
  static const iIdentItem_c* spc_nolocalIdent = NULL;

  /** initialization parameter for IsoName */
  static const iIsoName_c& srcc_noIsoName = iIsoName_c::iIsoNameUnspecified();

  /** Handler for a specific registered proprietary PGN */
  class iProprietaryMessageClient_c : private __IsoAgLib::ProprietaryMessageClient_c
  {
  public:
    /** Constructor of the API class registers the new instance immediatly at ProprietaryMessageHandler_c
        so that each succeeding setup call will be reflected there by call of
    */
    iProprietaryMessageClient_c()
    : ProprietaryMessageClient_c()
    {
      (void)srcc_noIsoName;
      (void)spc_nolocalIdent;
    }

    /** Constructor of the API class registers the new instance immediatly at ProprietaryMessageHandler_c
        so that each succeeding setup call will be reflected there by call of
    */
    iProprietaryMessageClient_c(uint32_t aui32_filter, uint32_t aui32_mask,
                                const IsoAgLib::iIsoName_c& acrc_rremoteECU,
                                const IsoAgLib::iIdentItem_c& apc_localIdent)
    : ProprietaryMessageClient_c(aui32_filter, aui32_mask, static_cast<const __IsoAgLib::IsoName_c&>(acrc_rremoteECU), static_cast<const __IsoAgLib::IdentItem_c&>(apc_localIdent)) {}

    virtual ~iProprietaryMessageClient_c() {}

    /** receives the messages
        has to be overloaded by the Application
    */
    virtual void processProprietaryMsg() = 0;

    /** define receive filter which will be used by ProprietaryMessageHandler for definition of CAN filter.
        trigger an update of CAN receive filters with call of
              ProprietaryMessageHandler::tiggerClientDataUpdate()
        @return true, when wanted PGN is from allowed range
    */
    bool defineReceiveFilter( uint32_t aui32_mask, uint32_t aui32_filter, const IsoAgLib::iIsoName_c& acrc_rremoteECU,
                                                                          const IsoAgLib::iIdentItem_c* apc_localIdent)
    { return ProprietaryMessageClient_c::defineReceiveFilter (aui32_mask, aui32_filter, static_cast<const __IsoAgLib::IsoName_c&>(acrc_rremoteECU), static_cast<const __IsoAgLib::IdentItem_c*>(apc_localIdent)); }

    /** function to tell "i will send data" to the handler */
    void sendDataToHandler() { ProprietaryMessageClient_c::sendDataToHandler(); }

    /** User can check if the sendData is currently being used because MultiSend_c
        is streaming out right now. In this case DO NOT MODIFY the
        GenericData_c SendData via getDataSend() !!!
    */
    bool isSending() { return ProprietaryMessageClient_c::isSending(); }

    /** the application shall only get a constant reference to the received data is only set by the friend class
        __IsoAgLib::ProprietaryMessageHandler_c
    */
    const iGenericData_c& getDataReceive() const { return ProprietaryMessageClient_c::getDataReceive(); }

    /** the application should be able to set the data for send
    */
    iGenericData_c& getDataSend() { return ProprietaryMessageClient_c::getDataSend(); }

    /** set time period on millisec for repeated send of the data that has been stored in c_sendData.
        period = 0 means: send only one message for each call of iProprietaryMessageHandler::sendData()
    */
    void setSendPeriodMsec (uint32_t aui32_sendPeriodMsec) { ProprietaryMessageClient_c::setSendPeriodMsec(aui32_sendPeriodMsec); }
  };
};

#endif



