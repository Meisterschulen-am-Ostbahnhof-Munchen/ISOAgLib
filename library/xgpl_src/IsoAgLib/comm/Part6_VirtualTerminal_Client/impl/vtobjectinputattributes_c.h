/*
  vtobjectinputattributes_c.h

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
#ifndef VTOBJECTINPUTATTRIBUTES_C_H
#define VTOBJECTINPUTATTRIBUTES_C_H

#include "../ivtobjectstring_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectInputAttributes_c : public IsoAgLib::iVtObjectString_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  vtObjectInputAttributes_c(const iVtObjectInputAttributes_s* vtObjectInputAttributesSROM , int ai_multitonInst);

  iVtObjectInputAttributes_s* get_vtObjectInputAttributes_a() { return dynamic_cast<iVtObjectInputAttributes_s *>(&(get_vtObject_a())); }



  uint32_t fitTerminal() const;

  const char* getString();

  void setValidationStringCopy(const char* newValidationString, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setValidationStringRef(const char* newValidationString, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 26; }

  uint8_t updateValidationType(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif
