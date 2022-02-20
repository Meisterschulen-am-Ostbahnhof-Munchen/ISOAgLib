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
  static ObjectType objectType() { return VT_OBJECT_TYPE_INPUT_ATTRIBUTES; }


  const char* getString(){
      return vtObjectInputAttributes_c::getString();}

  void setValidationStringCopy(const char* newValidationString, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputAttributes_c::setValidationStringCopy (newValidationString, b_updateObject, b_enableReplaceOfCmd);
  }

  void setValidationStringRef(char* newValidationString, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputAttributes_c::setValidationStringRef (newValidationString, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputAttributes_c::updateObjectType(); }
  
  uint8_t updateValidationType(bool b_SendRequest=false) {
    return vtObjectInputAttributes_c::updateValidationType(b_SendRequest);
  }
  */
#endif

  virtual ObjectType getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif
