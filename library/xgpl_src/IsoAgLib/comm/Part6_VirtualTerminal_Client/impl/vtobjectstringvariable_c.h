/*
  vtobjectstringvariable_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTSTRINGVARIABLE_C_H
#define VTOBJECTSTRINGVARIABLE_C_H
#include "../ivtobjectstring_c.h"
#include <IsoAgLib/hal/hal_typedef.h>

#ifdef USE_VTOBJECT_stringvariable

#include "vtclientservercommunication_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectStringVariable_c : public IsoAgLib::iVtObjectString_c
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
  //! @param vtObjectStringVariableSROM:
  //! @param b_initPointer:
  void init(const iVtObjectStringVariable_s* vtObjectStringVariableSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectStringVariableSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectStringVariable_a
  iVtObjectStringVariable_s* get_vtObjectStringVariable_a() { return (iVtObjectStringVariable_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectStringVariable_c
  vtObjectStringVariable_c();

  //  Operation: size
  uint32_t fitTerminal() const;

  //  Operation: setValueCopy
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopy    (const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef USE_VT_UNICODE_SUPPORT
  void setValueCopyUTF8(const char* newValue, uint8_t aui8_fontType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#endif

  //  Operation: setValueRef
  //! @param newValue:
  //! @param b_updateObject:
  void setValueRef(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  //  Operation: getString
  const char* getString();
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 22; }
  */

  void saveReceivedAttribute (uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/) {};
#endif
};

} // end of namespace __IsoAgLib

#endif
#endif
