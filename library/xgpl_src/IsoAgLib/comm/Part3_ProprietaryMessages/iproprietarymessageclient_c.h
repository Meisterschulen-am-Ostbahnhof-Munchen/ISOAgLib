/*
  iproprietarymessageclient_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

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

      virtual void processA( const iIsoItem_c& ) {}

      void init(const iIdentItem_c& ident, const iIsoName_c& remote, uint8_t dp) { __IsoAgLib::ProprietaryMessageA_c::init(ident, remote, dp); }
      void close() { __IsoAgLib::ProprietaryMessageA_c::close(); }

      void enableReception() { __IsoAgLib::ProprietaryMessageA_c::enableReception(); }
      void disableReception() { __IsoAgLib::ProprietaryMessageA_c::disableReception(); }

      IsoAgLib::iGenericData_c& getDataReceive() { return __IsoAgLib::ProprietaryMessageA_c::getDataReceive(); }
      IsoAgLib::iGenericData_c& getDataSend() { return __IsoAgLib::ProprietaryMessageA_c::getDataSend(); }

      bool send() { return __IsoAgLib::ProprietaryMessageA_c::send(); }
      bool isSending() const { return __IsoAgLib::ProprietaryMessageA_c::isSending(); }
  };


  class iProprietaryMessageB_c : private __IsoAgLib::ProprietaryMessageB_c {
    public:
      iProprietaryMessageB_c() : ProprietaryMessageB_c() {}
      virtual ~iProprietaryMessageB_c() {}

      virtual void processB( const iIsoItem_c& ) {}

      void init(const iIdentItem_c& ident, const iIsoName_c& remote, uint8_t dp) { __IsoAgLib::ProprietaryMessageB_c::init(ident, remote, dp); }
      void close() { __IsoAgLib::ProprietaryMessageB_c::close(); }

      void enableReception() { __IsoAgLib::ProprietaryMessageB_c::enableReception(); }
      void disableReception() { __IsoAgLib::ProprietaryMessageB_c::disableReception(); }

      IsoAgLib::iGenericData_c& getDataReceive() { return __IsoAgLib::ProprietaryMessageB_c::getDataReceive(); }
      IsoAgLib::iGenericData_c& getDataSend() { return __IsoAgLib::ProprietaryMessageB_c::getDataSend(); }

      bool send() { return __IsoAgLib::ProprietaryMessageB_c::send(); }
      bool isSending() const { return __IsoAgLib::ProprietaryMessageB_c::isSending(); }
  };
};

#endif
