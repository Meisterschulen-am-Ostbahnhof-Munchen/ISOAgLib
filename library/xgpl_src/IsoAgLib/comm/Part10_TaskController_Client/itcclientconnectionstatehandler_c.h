/*
  itcclientconnectionstatehandler_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef ITCCLIENTCONNECTIONSTATEHANDLER_C_H
#define ITCCLIENTCONNECTIONSTATEHANDLER_C_H
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoitem_c.h>

namespace IsoAgLib {

  class iTcClientConnectionStateHandler_c {
    public:
      virtual void processDefaultLoggingStart( IsoAgLib::iIsoItem_c & ecu ) = 0;
      virtual void processTaskStarted( IsoAgLib::iIsoItem_c& ecu ) = 0;
      virtual void processTaskStopped( IsoAgLib::iIsoItem_c& ecu ) = 0;

  };
}

#endif
