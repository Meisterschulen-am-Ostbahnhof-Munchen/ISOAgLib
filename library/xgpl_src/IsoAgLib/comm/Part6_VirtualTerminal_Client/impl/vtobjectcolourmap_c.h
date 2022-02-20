/*
  vtobjectcolourmap_c.h

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTCOLOURMAP_C_H
#define VTOBJECTCOLOURMAP_C_H

#include <IsoAgLib/isoaglib_config.h>

#ifdef CONFIG_USE_VTOBJECT_colourmap


#include <memory> // PImpl
#include "vtobject_c.h"
#include "vtclient_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectColourMap_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectColourMap_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectColourMap_s> vtObject_a;

public:
  uint16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  vtObjectColourMap_c(iVtObjectColourMap_s* vtObjectColourMapSROM , multiton ai_multitonInst);
  ~vtObjectColourMap_c();

  uint32_t fitTerminal() const;

  static uint8_t const pgheaderSize;

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} //__IsoAgLib

#endif //CONFIG_USE_VTOBJECT_colourmap

#endif //VTOBJECTCOLOURMAP_C_H
