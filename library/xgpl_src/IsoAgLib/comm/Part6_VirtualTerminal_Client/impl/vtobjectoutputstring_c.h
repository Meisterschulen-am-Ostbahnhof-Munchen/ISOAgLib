/*
  vtobjectoutputstring_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTOUTPUTSTRING_C_H
#define VTOBJECTOUTPUTSTRING_C_H

#include <IsoAgLib/hal/hal_typedef.h>
#ifdef USE_VTOBJECT_outputstring
#include "../ivtobjectstring_c.h"
#include "vtclientservercommunication_c.h"
#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectOutputString_c : public IsoAgLib::iVtObjectString_c
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
  //! @param vtObjectOutputStringSROM:
  //! @param b_initPointer:
  void init(const iVtObjectOutputString_s* vtObjectOutputStringSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectOutputStringSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectOutputString_a
  iVtObjectOutputString_s* get_vtObjectOutputString_a() { return (iVtObjectOutputString_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectOutputString_c
  vtObjectOutputString_c();
  ~vtObjectOutputString_c();

  //  Operation: fitTerminal
  uint32_t fitTerminal() const;

  //  Operation: setOriginSKM
  //! @param b_SKM:
  void setOriginSKM(bool b_SKM);

  //! @param p_btn:
  void setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn);

  //  Operation: getString
  const char* getString();

  //  Operation: setValueCopy / setValueCopyUTF8
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopy    (const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef USE_VT_UNICODE_SUPPORT
  void setValueCopyUTF8(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#endif

  //  Operation: setValueRef
  //! @param newValue:
  //! @param b_updateObject:
  void setValueRef(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), width) : 0, sizeof(iVtObjectOutputString_s), 1, newValue, b_enableReplaceOfCmd);
  }
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), height) : 0, sizeof(iVtObjectOutputString_s), 2, newValue, b_enableReplaceOfCmd);
  }
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), backgroundColour) : 0, sizeof(iVtObjectOutputString_s), 3, newValue, __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
  }
  void setFontAttributes(IsoAgLib::iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), fontAttributes) : 0, sizeof(iVtObjectOutputString_s), 4, newValue, b_enableReplaceOfCmd);
  }
  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), options) : 0, sizeof(iVtObjectOutputString_s), 5, newValue, newValue, b_enableReplaceOfCmd);
  }
  void setVariableReference(IsoAgLib::iVtObjectStringVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), variableReference) : 0, sizeof(iVtObjectOutputString_s), 6, (iVtObject_c*)newValue, b_enableReplaceOfCmd);
  }
  void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), horizontalJustification) : 0, sizeof(iVtObjectOutputString_s), 7, newValue, newValue, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 11; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);

  uint16_t updateHeight(bool b_SendRequest=false);

  uint8_t updateBackgroundColour(bool b_SendRequest=false);

  uint16_t updateFontAttributes(bool b_SendRequest=false);

  uint8_t updateOptions(bool b_SendRequest=false);

  uint16_t updateVariableReference(bool b_SendRequest=false);

  uint8_t updateJustification(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // end of namespace __IsoAgLib

#endif
#endif
