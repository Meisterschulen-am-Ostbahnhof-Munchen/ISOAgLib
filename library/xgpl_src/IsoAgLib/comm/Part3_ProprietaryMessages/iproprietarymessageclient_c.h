/*
  iproprietarymessageclient_c.h

  (C) Copyright 2009 - 2014 by OSB AG and developing partners

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


namespace IsoAgLib
{
  class iProprietaryMessageA_c : private __IsoAgLib::ProprietaryMessageA_c {
    public:
      iProprietaryMessageA_c() : ProprietaryMessageA_c() {}
      virtual ~iProprietaryMessageA_c() {}

      virtual void processA( const iIsoItem_c& sender ) { (void)sender; }

      void init(const iIdentItem_c& a_ident, const iIsoName_c& a_remote, uint8_t a_dp) { __IsoAgLib::ProprietaryMessageA_c::init(a_ident, a_remote, a_dp); }
      void close() { __IsoAgLib::ProprietaryMessageA_c::close(); }

      void enableReception() { __IsoAgLib::ProprietaryMessageA_c::enableReception(); }
      void disableReception() { __IsoAgLib::ProprietaryMessageA_c::disableReception(); }

      IsoAgLib::iGenericData_c& getDataReceive() { return __IsoAgLib::ProprietaryMessageA_c::getDataReceive(); }
      IsoAgLib::iGenericData_c& getDataSend() { return __IsoAgLib::ProprietaryMessageA_c::getDataSend(); }

      // If a_overwrite_remote is specified, the message is sent to a_overwrite_remote
      // else it is sent to m_remote
      bool send(const iIsoName_c& a_overwrite_remote = iIsoName_c::iIsoNameUnspecified())
      {
          return __IsoAgLib::ProprietaryMessageA_c::send(a_overwrite_remote);
      }

      // see send() for a_overwrite_remote
      bool sendWithPrio( unsigned prio, const iIsoName_c& a_overwrite_remote = iIsoName_c::iIsoNameUnspecified() )
      {
          return __IsoAgLib::ProprietaryMessageA_c::sendWithPrio( prio, a_overwrite_remote );
      }
      
      bool isSending() const { return __IsoAgLib::ProprietaryMessageA_c::isSending(); }
  };


  class iProprietaryMessageB_c : private __IsoAgLib::ProprietaryMessageB_c {
    public:
      iProprietaryMessageB_c() : ProprietaryMessageB_c() {}
      virtual ~iProprietaryMessageB_c() {}

      virtual void processB( const iIsoItem_c& sender, uint8_t ps ) { (void)sender; (void)ps; }

      void init(const iIdentItem_c& a_ident, const iIsoName_c& a_remote, uint8_t a_dp) { __IsoAgLib::ProprietaryMessageB_c::init(a_ident, a_remote, a_dp); }
      void close() { __IsoAgLib::ProprietaryMessageB_c::close(); }

      // You can enable/disable multiple PropB PGNs (i.e. with different PS field)
      void enableReception( uint8_t ps ) { __IsoAgLib::ProprietaryMessageB_c::enableReception( ps ); }
      void disableReception( uint8_t ps ) { __IsoAgLib::ProprietaryMessageB_c::disableReception( ps ); }

      IsoAgLib::iGenericData_c& getDataReceive() { return __IsoAgLib::ProprietaryMessageB_c::getDataReceive(); }
      IsoAgLib::iGenericData_c& getDataSend() { return __IsoAgLib::ProprietaryMessageB_c::getDataSend(); }

      bool send( uint8_t ps, const iIsoName_c& a_overwrite_remote = iIsoName_c::iIsoNameUnspecified() )
      { return __IsoAgLib::ProprietaryMessageB_c::send( ps, a_overwrite_remote ); }
      
      bool sendWithPrio( uint8_t ps, unsigned prio, const iIsoName_c& a_overwrite_remote = iIsoName_c::iIsoNameUnspecified() )
      { return __IsoAgLib::ProprietaryMessageB_c::sendWithPrio( ps, prio, a_overwrite_remote ); }
      
      bool isSending() const { return __IsoAgLib::ProprietaryMessageB_c::isSending(); }
  };
};

#endif
