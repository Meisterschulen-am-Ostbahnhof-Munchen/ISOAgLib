/*
  vtobjectinputstring_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTINPUTSTRING_C_H
#define VTOBJECTINPUTSTRING_C_H

#include "../ivtobjectstring_c.h"
#include <IsoAgLib/hal/hal_typedef.h>

#ifdef USE_VTOBJECT_inputstring
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectinputattributes_c.h"
#include "../ivtobjectstringvariable_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectInputString_c : public IsoAgLib::iVtObjectString_c
{
public:
  //  Operation: stream
  //! @param destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  //  Operation: init
  //! @param vtObjectInputStringSROM:
  //! @param b_initPointer:
  void init(const iVtObjectInputString_s* vtObjectInputStringSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectInputStringSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectInputString_a
  iVtObjectInputString_s* get_vtObjectInputString_a() { return (iVtObjectInputString_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectInputString_c
  vtObjectInputString_c();

  //  Operation: fitTerminal
  uint32_t fitTerminal() const;

  //  Operation: getString
  const char* getString();

  //  Operation: setValueRef
  //! @param newValue:
  //! @param b_updateObject:
  void setValueRef(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  //  Operation: setValueCopy
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopy(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

#ifdef USE_VT_UNICODE_SUPPORT
  //  Operation: setValueCopyUTF8
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopyUTF8(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
#endif

  //  Operation: setSize
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  // Operation: setWidth
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), width) : 0, sizeof(iVtObjectInputString_s), 1 /* "Width" */, newValue, b_enableReplaceOfCmd);
  }

  // Operation: setHeight
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), height) : 0, sizeof(iVtObjectInputString_s), 2 /* "Height" */, newValue, b_enableReplaceOfCmd);
  }

  //  Operation: setBackgroundColor
   //! @param colorValue:
  //! @param b_updateObject:
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), backgroundColour) : 0, sizeof(iVtObjectInputString_s), 3 /* "Background Colour" */, newValue, __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
  }

  // Operation: setFontAttributes
  //! @param newFontAttributes:
  //! @param b_updateObject:
  void setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c* newFontAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), fontAttributes) : 0, sizeof(iVtObjectInputString_s), 4 /* "Font Attributes" */, newFontAttributes, b_enableReplaceOfCmd);
  }

  // Operation: setInputAttributes
  //! @param newInputAttributes:
  //! @param b_updateObject:
  void setInputAttributes(IsoAgLib::iVtObjectInputAttributes_c* newInputAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), inputAttributes) : 0, sizeof(iVtObjectInputString_s), 5 /* "Input Attributes" */, newInputAttributes, b_enableReplaceOfCmd);
  }

  // Operation: setOptions
  //! @param newOptions:
  //! @param b_updateObject:
  void setOptions(uint8_t newOptions, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), options) : 0, sizeof(iVtObjectInputString_s), 6 /* "Options" */, newOptions, newOptions, b_enableReplaceOfCmd);
  }

  // Operation: setVariableReference
  //! @param newVariableRef:
  //! @param b_updateObject:
  void setVariableReference(IsoAgLib::iVtObjectStringVariable_c* newVariableRef, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), variableReference) : 0, sizeof(iVtObjectInputString_s), 7 /* "Variable Reference" */, newVariableRef, b_enableReplaceOfCmd);
  }

  // Operation: setHorizontalJustification
  //! @param newHorizontalJustification:
  //! @param b_updateObject:
  void setHorizontalJustification(uint8_t newHorizontalJustification, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), horizontalJustification) : 0, sizeof(iVtObjectInputString_s), 8 /* "Horizontal justification" */, newHorizontalJustification, newHorizontalJustification, b_enableReplaceOfCmd);
  }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 8; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);

  uint16_t updateHeight(bool b_SendRequest=false);

  uint8_t updateBackgroundColour(bool b_SendRequest=false);

  uint16_t updateFontAttributes(bool b_SendRequest=false);

  uint16_t updateInputAttributes(bool b_SendRequest=false);

  uint8_t updateOptions(bool b_SendRequest=false);

  uint16_t updateVariableReference(bool b_SendRequest=false);

  uint8_t updateJustification(bool b_SendRequest=false);

  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateEnabled(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
private:
  //  Operation: updateEnable
  //! @param b_enableOrDisable:
  void updateEnable(uint8_t aui8_enOrDis);
};

} // end namespace __IsoAgLib

#endif
#endif
