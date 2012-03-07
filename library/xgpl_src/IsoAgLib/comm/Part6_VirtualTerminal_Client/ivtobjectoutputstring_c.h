/*
  ivtobjectoutputstring_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTOUTPUTSTRING_C_H
#define IVTOBJECTOUTPUTSTRING_C_H

// +X2C includes
#include "impl/vtobjectoutputstring_c.h"
//#include "ivtobjectstringvariable_c.h"
// ~X2C
#ifdef USE_VTOBJECT_outputstring
#include "iisoterminal_c.h"
// Begin Namespace IsoAgLib
namespace IsoAgLib {



//  +X2C Class 109 : iVtObjectOutputString_c
//!  Stereotype: Klasse
class iVtObjectOutputString_c : public __IsoAgLib::vtObjectOutputString_c
{

public:
  iVtObjectOutputString_c();
  ~iVtObjectOutputString_c();

  //  Operation: init
  //! Parameter:
  //! @param vtObjectOutputStringSROM:
  //! @param b_initPointer:
  void init(const iVtObjectOutputString_s* vtObjectOutputStringSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectOutputString_c::init (vtObjectOutputStringSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectOutputString_a
  const iVtObjectOutputString_s& get_vtObjectOutputString_a() { return *vtObjectOutputString_c::get_vtObjectOutputString_a(); }


  //  Operation: setValueCopy
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopy(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setValueCopy (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef USE_VT_UNICODE_SUPPORT
  //  Operation: setValueCopyUTF8
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopyUTF8(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setValueCopyUTF8 (newValue, b_updateObject, b_enableReplaceOfCmd);
  }
#endif

  //  Operation: setValueRef
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValueRef(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setValueRef (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newVariable:
  //! @param b_updateObject:
  void setVariableReference(iVtObjectStringVariable_c* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false)  {
    vtObjectOutputString_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: getString
  const char* getString() { return vtObjectOutputString_c::getString(); }


  //  Operation: setWidth
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setBackgroundColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setFontAttributes
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setFontAttributes(iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setFontAttributes (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setOptions
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHorizontalJustification
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setHorizontalJustification (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectOutputString_c::updateObjectType(); }
   */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateHeight(b_SendRequest);
  }

  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateBackgroundColour(b_SendRequest);
  }

  uint16_t updateFontAttributes(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateFontAttributes(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateOptions(b_SendRequest);
  }

  uint16_t updateVariableReference(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateVariableReference(b_SendRequest);
  }

  uint8_t updateJustification(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateJustification(b_SendRequest);
  }
#endif
};

} // end of namespace IsoAgLib

#endif // -X2C
#endif
