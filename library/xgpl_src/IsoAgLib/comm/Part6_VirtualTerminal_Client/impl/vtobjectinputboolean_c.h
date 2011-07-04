/*
  vtobjectinputboolean_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTINPUTBOOLEAN_C_H
#define VTOBJECTINPUTBOOLEAN_C_H

#include <IsoAgLib/hal/hal_typedef.h>

#ifdef USE_VTOBJECT_inputboolean
#include "vtobject_c.h"
#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectInputBoolean_c : public vtObject_c
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
  //! @param vtObjectInputBooleanSROM:
  //! @param b_initPointer:
  void init(const iVtObjectInputBoolean_s* vtObjectInputBooleanSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectInputBooleanSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectInputBoolean_a
  iVtObjectInputBoolean_s* get_vtObjectInputBoolean_a() { return (iVtObjectInputBoolean_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectInputBoolean_c
  vtObjectInputBoolean_c();

  //  Operation: size
  uint32_t fitTerminal() const;

  //  Operation: setValue
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(bool newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true);

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputBoolean_a(), backgroundColour) : 0, sizeof(iVtObjectInputBoolean_s), 1, newValue, __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
  }

  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputBoolean_a(), width) : 0, sizeof(iVtObjectInputBoolean_s), 2, newValue, b_enableReplaceOfCmd);
  }

  void setForegroundColour(IsoAgLib::iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputBoolean_a(), foregroundColour) : 0, sizeof(iVtObjectInputBoolean_s), 3, newValue, b_enableReplaceOfCmd);
  }

  void setVariableReference(IsoAgLib::iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputBoolean_a(), variableReference) : 0, sizeof(iVtObjectInputBoolean_s), 4, newValue, b_enableReplaceOfCmd);
  }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 7; }
  */

  uint8_t updateBackgroundColour(bool b_SendRequest=false);

  uint16_t updateWidth(bool b_SendRequest=false);

  uint16_t updateForegroundColour(bool b_SendRequest=false);

  uint16_t updateVariableReference(bool b_SendRequest=false);

  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateValue(bool b_SendRequest=false);

  uint8_t updateEnabled(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
private:
  //  Operation: updateEnable
  //! @param b_enableOrDisable:
  void updateEnable(uint8_t aui8_enOrDis);
};

} // end of namespace __IsoAgLib

#endif
#endif
