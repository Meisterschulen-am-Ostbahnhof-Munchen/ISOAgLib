/*
  itcclientconnection_c.h: Class for managing a connection 

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITCCLIENTCONNECTION_C_H
#define ITCCLIENTCONNECTION_C_H

#include "impl/tcclientconnection_c.h"
#include "itcclient_c.h"

namespace IsoAgLib {
class iProcDataHandler_c;

class iTcClientConnection_c : public __IsoAgLib::TcClientConnection_c
{
public:
  bool sendCommandChangeDesignator(uint16_t objectID, const char* newString, uint8_t stringLength)
    { return TcClientConnection_c::sendCommandChangeDesignator(objectID, newString, stringLength); }

  iIdentItem_c& getIdentItem() const { return static_cast<IsoAgLib::iIdentItem_c&>(TcClientConnection_c::getIdentItem()); }

  void setProcDataHandler( iProcDataHandler_c *procDataHandler )
  { TcClientConnection_c::setProcDataHandler( procDataHandler ); }

private:
  /** PRIVATE constructor to forbid instantiation of this interface class.
    * it can only be static_cast'ed to this class, not constructed!
    */
  iTcClientConnection_c();

  friend class iTcClient_c;
  friend class __IsoAgLib::TcClientConnection_c;
};

} // IsoAgLib

#endif
