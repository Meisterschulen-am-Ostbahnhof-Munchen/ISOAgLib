/*
  idevpropertyhandler_c.h - 

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IDEVPROPERTYHANDLER_C_H
#define IDEVPROPERTYHANDLER_C_H

#include "impl/devpropertyhandler_c.h"


namespace IsoAgLib {

class iDevPropertyHandler_c : public __IsoAgLib::DevPropertyHandler_c
{
public:
  bool registerDevicePool (const IsoAgLib::iIdentItem_c* apc_wsMasterIdentItem, const HUGE_MEM uint8_t* apc_devicePoolByteArray, const uint32_t aui32_bytestreamLength, bool ab_setToDefault)
    { return DevPropertyHandler_c::registerDevicePool (static_cast<const __IsoAgLib::IdentItem_c*>(apc_wsMasterIdentItem), apc_devicePoolByteArray, aui32_bytestreamLength, ab_setToDefault); }

  bool sendCommandChangeDesignator(uint16_t apui16_objectID, const char* apc_newString, uint8_t stringLength)
    { return DevPropertyHandler_c::sendCommandChangeDesignator(apui16_objectID, apc_newString, stringLength); }

private:
  friend class iTcClient_c;
};

} // IsoAgLib

#endif
