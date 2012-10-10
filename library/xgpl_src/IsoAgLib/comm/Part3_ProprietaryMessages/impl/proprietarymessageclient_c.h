/*
  proprietarymessageclient_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef PROPRIETARYMESSAGECLIENT_H
#define PROPRIETARYMESSAGECLIENT_H

#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisendeventhandler_c.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include "../igenericdata_c.h"


namespace __IsoAgLib
{

  class ProprietaryMessageHandler_c;


  /** Class proprietarymessageclient_c
      internal base class where each object represents one "proprietary PGN, Local node, remote node"-set
    */
  class ProprietaryMessage_c : public MultiSendEventHandler_c
  {
    public:
      ProprietaryMessage_c() : m_sendSuccess( SendStream_c::SendSuccess ), ms_receivedData(), ms_sendData() {}
      virtual ~ProprietaryMessage_c() {}

      IsoAgLib::iGenericData_c& getDataReceive() { return ms_receivedData; }
      IsoAgLib::iGenericData_c& getDataSend() { return ms_sendData; }

      /** User can check if the sendData is currently being used because MultiSend_c
        is streaming out right now. In this case DO NOT MODIFY the
        GenericData_c SendData via getDataSend() !!!
        */
      bool isSending() const { return ( m_sendSuccess == SendStream_c::Running ); }


    private:
      void reactOnStateChange( const SendStream_c& sendStream ) {
        m_sendSuccess = sendStream.getSendSuccess();
      }
      SendStream_c::sendSuccess_t m_sendSuccess;

      IsoAgLib::iGenericData_c ms_receivedData;
      IsoAgLib::iGenericData_c ms_sendData;


      /** forbid copy construction / assignment as it would
          perform too many client-/filter-/mr-/ms-(de)registrations */
      ProprietaryMessage_c(const ProprietaryMessage_c& );
      ProprietaryMessage_c& operator= ( const ProprietaryMessage_c& );
  };


  class ProprietaryMessageA_c : public ProprietaryMessage_c {
    public:
      ProprietaryMessageA_c( const IdentItem_c& ident, const IsoName_c& remote, uint8_t dp ) : m_ident( ident ), m_remote( remote ), m_dp( dp ) {}
      virtual ~ProprietaryMessageA_c() {}

      void init();
      void close();

      virtual void processA( const IsoAgLib::iIsoItem_c& ) {}

      bool send();

      const IdentItem_c& m_ident;
      IsoName_c m_remote;
      uint8_t m_dp;
  };


  class ProprietaryMessageB_c : public ProprietaryMessage_c {
    public:
      ProprietaryMessageB_c( const IdentItem_c& ident, const IsoName_c& remote, uint8_t dp ) : m_ident( ident ), m_remote( remote ), m_dp( dp ) {}
      virtual ~ProprietaryMessageB_c() {}

      void init();
      void close();

      virtual void processB( const IsoAgLib::iIsoItem_c& ) {}

      bool send();

      const IdentItem_c& m_ident;
      IsoName_c m_remote;
      uint8_t m_dp;
  };

};

#endif
