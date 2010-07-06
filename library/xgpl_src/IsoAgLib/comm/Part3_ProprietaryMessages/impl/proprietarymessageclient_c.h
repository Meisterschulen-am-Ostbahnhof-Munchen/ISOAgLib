/*
  proprietarymessageclient_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef PROPRIETARYMESSAGECLIENT_H
#define PROPRIETARYMESSAGECLIENT_H

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofilterbox_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include "../igenericdata_c.h"


namespace IsoAgLib
{ // forward declarations for friends
  class iProprietaryMessageHandler_c;
}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib
{
  // forward declarations for friends
  class ProprietaryMessageHandler_c;

  /** initialization parameter for filter must be > 29 Bit */
  static const uint32_t scui32_noFilter = 0xFFFFFFFF;

  /** initialization parameter for mask must be > 29 Bit */
  static const uint32_t scui32_noMask = 0xFFFFFFFF;

  /** Class proprietarymessageclient_c
      internal base class where each object represents one "proprietary PGN, Local node, remote node"-set
    */
  class ProprietaryMessageClient_c
  {
    public:

      /** default constructor
          initializes the parameter for filter and mask to "novalue"
        */
      ProprietaryMessageClient_c();

      /** second constructor
          initializes the parameter for filter and mask to ""
        */
      ProprietaryMessageClient_c(uint32_t aui32_filter, uint32_t aui32_mask,
                                 const IsoName_c& acrc_rremoteECU,
                                 const IdentItem_c& apc_localIdent);

      /** destructor which has nothing to do
        */
      virtual ~ProprietaryMessageClient_c();

    private:
      /** forbid copy construction / assignment as it would
          perform too many client-/filter-/mr-/ms-(de)registrations */
      ProprietaryMessageClient_c(const ProprietaryMessageClient_c& rhs);
      ProprietaryMessageClient_c& operator= (const ProprietaryMessageClient_c& rhs);


    public:
      /** virtual bool processProprietaryMsg() must be overloaded by the application
        */
      virtual void processProprietaryMsg() = 0;

      /** the application shall only get a constant reference to the received data
          is only set by the friend class __IsoAgLib::ProprietaryMessageHandler_c
        */
      const IsoAgLib::iGenericData_c& getDataReceive() const
      {
        return (ms_receivedData);
      }

      /** the application shall be able to set the data for send
          !!!!!!!! ATTENTION: Only change the send-data if "!isSending()" !!!!!!!!!!
        */
      IsoAgLib::iGenericData_c& getDataSend()
      {
        return (ms_sendData);
      }

      /** User can check if the sendData is currently being used because MultiSend_c
        is streaming out right now. In this case DO NOT MODIFY the
        GenericData_c SendData via getDataSend() !!!
        */
      bool isSending() { return (men_sendSuccess == MultiSend_c::Running); }

      /** function to tell "i will send data" to the handler */
      void sendDataToHandler();

      /** will be used by ProprietaryMessageHandler_c for definition of CAN-Filter
          trigger an update of CAN receive filters with call of
            - ProprietaryMessageHandler_c::triggerClientDataUpdate()
          @return true when wanted PGN is in allowed range
        */
      bool defineReceiveFilter( uint32_t aui32_mask, uint32_t aui32_filter, const IsoName_c& acrc_rremoteECU,
                                                                            const IdentItem_c* apc_localIdent);

      /** set time period in milliseconds for repeated send of the data that has been stored in c_sendData()
          only one message is sent when period == 0
        */
      void setSendPeriodMsec(uint32_t aui32_sendPeriodMsec);

      uint32_t getSendPeriodMsec(void)
      {
        // set time period in msec
        return( mui32_sendPeriodicMsec );
      }

      uint32_t getNextSendTimeStamp(void)
      {
        // set time period in msec
        return( mui32_nextSendTimeStamp );
      }

      //! Attention: Current filter may not be valid in case the
      //! address wasn't/isn't claimed!
      IsoFilter_s getCurrentFilter (CanCustomer_c &);

    /// Using the singletonVecKey from mpc_localIdent (-->IdentItem_c)
    SINGLETON_PAR_ARR_DEF(mpc_localIdent)


    private:

      /* allow the IsoAgLib Handler class to directly access the data so that only the internal handler class
         can set the received data. The application shall only read received data.
       */
      friend class IsoAgLib::iProprietaryMessageHandler_c;
      friend class __IsoAgLib::ProprietaryMessageHandler_c;

      /* information for definition of received filters, the application sets the data
         ProprietaryMessageHandler_c::registerProprietaryMessageClient() reads the following
         attributes directly (see friend) to verify allowed PGN range and to set appropriate
         CAN receive filters */

      /// Mask for selection of decision bits
      uint32_t mui32_canMask;
      /// Filter to define receive compare
      uint32_t mui32_canFilter;

      /**
          define individual remote ECU for selection of message receive and selection of message send target
          set IsoName_c to Unspecified, when messages from global shall received and messages shall be sent
          to global
      */
      IsoName_c mc_isonameRemoteECU;

      /// local ident for spec of sending and recieving filter definition
      const IdentItem_c* mpc_localIdent;

      /** data structure for send and receive - separated iProprietaryMessageHandler_c places received data HERE.
          before it calls ProprietaryMessageClient_c::processProprietaryMsg()
      */
      IsoAgLib::iGenericData_c ms_receivedData;

      /** application should place here any data that should be send
      */
      IsoAgLib::iGenericData_c ms_sendData;

      /** is used to control repeated sending
          0 - send only one time when iProprietaryMessageHandler_c::sendData is called
          else the sendData() starts periodically send of data
       */
      uint32_t mui32_sendPeriodicMsec;

      /** is used to control repeated sending
          the lowest timestamp must start sendData
          not set/used when mui32_sendPeriodicMsec==0.
       */
      uint32_t mui32_nextSendTimeStamp;

      /** actual (E)TP-send state */
      MultiSend_c::sendSuccess_t men_sendSuccess;
  };
};

#endif
