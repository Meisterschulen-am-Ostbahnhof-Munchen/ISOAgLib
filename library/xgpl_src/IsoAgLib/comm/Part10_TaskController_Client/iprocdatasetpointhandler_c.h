/*
  iprocdatasetpointhandler_c.h: base class for application setpoint handler

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPROCDATASETPOINTHANDLER_C_H
#define IPROCDATASETPOINTHANDLER_C_H

#include <IsoAgLib/comm/Part5_NetworkManagement/iisoitem_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>


namespace IsoAgLib {

  class iProcData_c;

  class iProcDataSetpointHandler_c {
    public:
      virtual ~iProcDataSetpointHandler_c() {}
      virtual void processSetpointSet( IsoAgLib::iProcData_c& procdata, int32_t value, IsoAgLib::iIsoItem_c& ecu, bool change ) = 0;
  };

}

#endif

