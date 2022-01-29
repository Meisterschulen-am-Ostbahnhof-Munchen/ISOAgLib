/*
  vtobjectinputboolean_c.h

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
#ifndef VTOBJECTINPUTBOOLEAN_C_H
#define VTOBJECTINPUTBOOLEAN_C_H

#include <IsoAgLib/isoaglib_config.h>




#include "vtobject_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

class vtObjectInputBoolean_c : public vtObject_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  vtObjectInputBoolean_c(const iVtObjectInputBoolean_s* vtObjectInputBooleanSROM , int ai_multitonInst)
  :vtObject_c((iVtObject_s*) vtObjectInputBooleanSROM , ai_multitonInst)
  {}

  iVtObjectInputBoolean_s* get_vtObjectInputBoolean_a() { return dynamic_cast<iVtObjectInputBoolean_s *>(&(get_vtObject_a())); }

  vtObjectInputBoolean_c();

  uint32_t fitTerminal() const;

  void setValue(bool newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true);

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputBoolean_a(), backgroundColour) : 0, sizeof(iVtObjectInputBoolean_s), 1, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
  }

  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputBoolean_a(), width) : 0, sizeof(iVtObjectInputBoolean_s), 2, newValue, b_enableReplaceOfCmd);
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
  void updateEnable(uint8_t aui8_enOrDis);
};

} // __IsoAgLib

#endif

