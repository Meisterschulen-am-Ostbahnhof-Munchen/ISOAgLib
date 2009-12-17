/*
  ivtobjectstringvariable_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTSTRINGVARIABLE_C_H
#define IVTOBJECTSTRINGVARIABLE_C_H

#include "impl/vtobjectstringvariable_c.h"

#ifdef USE_VTOBJECT_stringvariable 
#include "ivtobjectoutputstring_c.h"

#ifdef USE_VT_UNICODE_SUPPORT
#include "ivtobjectfontattributes_c.h"
#endif
// Begin Namespace IsoAgLib
namespace IsoAgLib {

class iVtObjectStringVariable_c : public __IsoAgLib::vtObjectStringVariable_c
{
public:
  iVtObjectStringVariable_c();
  //  Operation: init
  //! @param vtObjectStringVariableSROM:
  //! @param b_initPointer:
  void init(const iVtObjectStringVariable_s* vtObjectStringVariableSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectStringVariable_c::init (vtObjectStringVariableSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectStringVariable_a
  const iVtObjectStringVariable_s& get_vtObjectStringVariable_a() {
    return *vtObjectStringVariable_c::get_vtObjectStringVariable_a ();
  }

  //  Operation: setValueCopy
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopy(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectStringVariable_c::setValueCopy (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef USE_VT_UNICODE_SUPPORT
  //  Operation: setValueCopyUTF8
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopyUTF8(const char* newValue, uint8_t aui8_fontType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectStringVariable_c::setValueCopyUTF8 (newValue, aui8_fontType, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setValueCopyUTF8
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopyUTF8(const char* newValue, iVtObjectOutputString_c& arc_ops, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    const uint8_t cui8_fontType = arc_ops.get_vtObjectOutputString_a().fontAttributes->get_vtObjectFontAttributes_a().fontType;
    vtObjectStringVariable_c::setValueCopyUTF8 (newValue, cui8_fontType, b_updateObject, b_enableReplaceOfCmd);
  }
#endif

  //  Operation: setValueRef
  //! @param newValue:
  //! @param b_updateObject:
  void setValueRef(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectStringVariable_c::setValueRef (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: getString
  const char* getString() { return vtObjectStringVariable_c::getString(); }

  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectStringVariable_c::updateObjectType(); }
  */
};

} // end of namespace IsoAgLib

#endif
#endif
