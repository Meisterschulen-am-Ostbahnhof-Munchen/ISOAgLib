/*
  ipdconnection_c.h: interface for managing of a process data connection

  (C) Copyright 2014 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPDCONNECTION_C_H
#define IPDCONNECTION_C_H

#include "impl/pdconnection_c.h"


namespace IsoAgLib {

  class iPdConnection_c : private __IsoAgLib::PdConnection_c
  {
    // to be filled on demand. Needed as handle for now only.
    
    friend class iTcClient_c;
    friend class iPdLocal_c;
  };

}

#endif
