/*
  vtobjectcolourmap_c.h

  (C) Copyright 2009 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTCOLOURMAP_C_H
#define VTOBJECTCOLOURMAP_C_H

#include "vtobject_c.h"
#include "vtclient_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectColourMap_c : public vtObject_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  void init(const iVtObjectColourMap_s* vtObjectColourMapSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectColourMapSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); }

  inline iVtObjectColourMap_s* get_vtObjectColourMap_a() { return (iVtObjectColourMap_s *)&(get_vtObject_a()); }

  vtObjectColourMap_c();

  uint32_t fitTerminal() const;

  static uint8_t const pgheaderSize;

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} //__IsoAgLib

#endif
