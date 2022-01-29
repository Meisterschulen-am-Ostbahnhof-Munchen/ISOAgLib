/*
  vtobjectrectangle_c.h

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
#ifndef VTOBJECTRECTANGLE_C_H
#define VTOBJECTRECTANGLE_C_H

#include <IsoAgLib/isoaglib_config.h>




#include "vtobject_c.h"


namespace __IsoAgLib {

class vtObjectRectangle_c : public vtObject_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  vtObjectRectangle_c(const iVtObjectRectangle_s* vtObjectRectangleSROM , int ai_multitonInst)
  :vtObject_c((iVtObject_s*) vtObjectRectangleSROM , ai_multitonInst)
  {}

  iVtObjectRectangle_s* get_vtObjectRectangle_a() { return dynamic_cast<iVtObjectRectangle_s *>(&(get_vtObject_a())); }


  uint32_t fitTerminal() const;

  void setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectRectangle_a(), lineAttributes) : 0, sizeof(iVtObjectRectangle_s), 1 /* "Line Attribute" */, (IsoAgLib::iVtObject_c*) newLineAttributes, b_enableReplaceOfCmd);
  }

  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectRectangle_a(), width) : 0, sizeof(iVtObjectRectangle_s), 2 /* "Width" */, newWidth, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectRectangle_a(), height) : 0, sizeof(iVtObjectRectangle_s), 3 /* "Height" */, newHeight, b_enableReplaceOfCmd);
  }

  void setLineSuppression(uint8_t newLineSupressionValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectRectangle_a(), lineSuppression) : 0, sizeof(iVtObjectRectangle_s), 4 /* "Line Suppression" */, newLineSupressionValue, newLineSupressionValue, b_enableReplaceOfCmd);
  }

  void setFillAttributes(IsoAgLib::iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectRectangle_a(), fillAttributes) : 0, sizeof(iVtObjectRectangle_s), 5 /* "Fill Attributes" */, (IsoAgLib::iVtObject_c*) newFillAttributes, b_enableReplaceOfCmd);
  }

  void setOriginSKM(bool b_SKM);

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 14; }
  */

  uint16_t updateLineAttributes(bool b_SendRequest=false);
  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  uint8_t updateLineSuppression(bool b_SendRequest=false);
  uint16_t updateFillAttributes(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif


