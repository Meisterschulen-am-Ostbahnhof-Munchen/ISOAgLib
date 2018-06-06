/*
  ipdconnection_c.h: interface for managing of a process data connection

  (C) Copyright 2014 - 2016 by OSB AG and developing partners

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
    //! It will pass out negative PdAck messages to the application
    //! Note: In a proper running system no negative PdAcks should
    //! be on the bus, so this is for special purposes only!!!
    //! It can be set or unset, but only one is possible at a time!
    void setNackHandler( IsoAgLib::ProcData::iNackHandler_c* handler )
    { PdConnection_c::setNackHandler(handler); }

    friend class iTcClient_c;
    friend class iPdLocal_c;
  
  private:
    iPdConnection_c(); // not constructable!
    iPdConnection_c( const iPdConnection_c& ); // not copy-constructable!
    iPdConnection_c& operator=( const iPdConnection_c& ); // not assignable!
  };

}

#endif
