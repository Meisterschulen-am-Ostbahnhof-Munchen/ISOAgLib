/*
  ivtobjectinputattributes_c.h

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
#ifndef IVTOBJECTINPUTATTRIBUTES_C_H
#define IVTOBJECTINPUTATTRIBUTES_C_H

#include "impl/vtobjectinputattributes_c.h"


namespace IsoAgLib {

class iVtObjectInputAttributes_c : public __IsoAgLib::vtObjectInputAttributes_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_INPUT_ATTRIBUTES; }

  iVtObjectInputAttributes_c(const iVtObjectInputAttributes_s* vtObjectInputAttributesSROM , int ai_multitonInst)
  :vtObjectInputAttributes_c(vtObjectInputAttributesSROM , ai_multitonInst)
  {}

  const iVtObjectInputAttributes_s& get_vtObjectInputAttributes_a() { return *vtObjectInputAttributes_c::get_vtObjectInputAttributes_a(); }

  const char* getString(){
      return vtObjectInputAttributes_c::getString();}

  void setValidationStringCopy(const char* newValidationString, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputAttributes_c::setValidationStringCopy (newValidationString, b_updateObject, b_enableReplaceOfCmd);
  }

  void setValidationStringRef(const char* newValidationString, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputAttributes_c::setValidationStringRef (newValidationString, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputAttributes_c::updateObjectType(); }
  
  uint8_t updateValidationType(bool b_SendRequest=false) {
    return vtObjectInputAttributes_c::updateValidationType(b_SendRequest);
  }
  */
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif
