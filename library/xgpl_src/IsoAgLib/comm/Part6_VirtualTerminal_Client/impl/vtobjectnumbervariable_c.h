/*
  vtobjectnumbervariable_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTNUMBERVARIABLE_C_H
#define VTOBJECTNUMBERVARIABLE_C_H

#include <IsoAgLib/isoaglib_config.h>

#ifdef USE_VTOBJECT_numbervariable
#include "vtobject_c.h"
#include "vtclientservercommunication_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectNumberVariable_c : public vtObject_c
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
  //! @param vtObjectNumberVariableSROM:
  //! @param b_initPointer:
  void init(const iVtObjectNumberVariable_s* vtObjectNumberVariableSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectNumberVariableSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectNumberVariable_a
  iVtObjectNumberVariable_s* get_vtObjectNumberVariable_a() { return (iVtObjectNumberVariable_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectNumberVariable_c
  vtObjectNumberVariable_c();

  //  Operation: size
  uint32_t fitTerminal() const;

  //  Operation: setValue
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 21; }

  uint32_t updateValue(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // end of namespace __IsoAgLib

#endif
#endif
