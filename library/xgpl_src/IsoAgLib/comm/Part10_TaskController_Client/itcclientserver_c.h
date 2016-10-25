/*
  itcclientserver_c.h: managing a TC-/DL- or proprietary Server

  (C) Copyright 2009 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITCCLIENTSERVER_C_H
#define ITCCLIENTSERVER_C_H

#include "impl/serverinstance_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoitem_c.h>


namespace IsoAgLib {

  class iTcClientServer_c : private __IsoAgLib::ServerInstance_c
  {
  public:
    const iIsoItem_c& getIsoItem() const { return static_cast<const iIsoItem_c &>( __IsoAgLib::ServerInstance_c::getIsoItem() ); }
    IsoAgLib::ProcData::ServerType_t getEcuType() const { return __IsoAgLib::ServerInstance_c::getEcuType(); }

  private:
    iTcClientServer_c();

    friend class iTcClient_c;
    friend class iTcClientConnection_c;
  };

}

#endif
