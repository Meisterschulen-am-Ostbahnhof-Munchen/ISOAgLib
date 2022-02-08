/*
  ivtobjectinputstring_c.h

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
#ifndef IVTOBJECTINPUTSTRING_C_H
#define IVTOBJECTINPUTSTRING_C_H

#include "impl/vtobjectinputstring_c.h"



namespace IsoAgLib {

class iVtObjectInputString_c : public __IsoAgLib::vtObjectInputString_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_INPUT_STRING; }

  const char* getString() { return vtObjectInputString_c::getString(); }

  void setValueRef(char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setValueRef (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setValueCopy(char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setValueCopy (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef USE_VT_UNICODE_SUPPORT
  void setValueCopyUTF8(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setValueCopyUTF8 (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setValueCopyUTF16(const char* newValue, uint16_t length, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setValueCopyUTF16 (newValue, length, b_updateObject, b_enableReplaceOfCmd);
  }
#endif

  void setVariableReference(iVtObjectStringVariable_c* newVariable, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setVariableReference (newVariable, b_updateObject, b_enableReplaceOfCmd);
  }

  void changeBackgroundColour(Colour colorValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setBackgroundColour (colorValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  void setFontAttributes(iVtObjectFontAttributes_c* newFontAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setFontAttributes (newFontAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  void setInputAttributes(iVtObjectInputAttributes_c* newInputAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setInputAttributes (newInputAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  void setOptions(uint8_t newOptions, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setOptions (newOptions, b_updateObject, b_enableReplaceOfCmd);
  }

  void setHorizontalJustification(uint8_t newHorizontalJustification, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setHorizontalJustification (newHorizontalJustification, b_updateObject, b_enableReplaceOfCmd);
  }

  bool enable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false); //TODO
  bool disable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false); //TODO

  bool getEnabled(); //TODO

  bool select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputString_c::updateObjectType(); }
  */
  
  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectInputString_c::updateWidth(b_SendRequest);
  }
  
  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectInputString_c::updateHeight(b_SendRequest);
  }

  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectInputString_c::updateBackgroundColour(b_SendRequest);
  }

  uint16_t updateFontAttributes(bool b_SendRequest=false) {
    return vtObjectInputString_c::updateFontAttributes(b_SendRequest);
  }

  uint16_t updateInputAttributes(bool b_SendRequest=false) {
    return vtObjectInputString_c::updateInputAttributes(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectInputString_c::updateOptions(b_SendRequest);
  }

  uint16_t updateVariableReference(bool b_SendRequest=false) {
    return vtObjectInputString_c::updateVariableReference(b_SendRequest);
  }

  uint8_t updateJustification(bool b_SendRequest=false) {
    return vtObjectInputString_c::updateJustification(b_SendRequest);
  }

  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateEnabled(bool b_SendRequest=false) {
    return vtObjectInputString_c::updateEnabled(b_SendRequest);
  }
  */
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif

