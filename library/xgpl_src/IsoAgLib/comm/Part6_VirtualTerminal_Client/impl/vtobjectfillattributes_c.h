/*
  vtobjectfillattributes_c.h

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
#ifndef VTOBJECTFILLATTRIBUTES_C_H
#define VTOBJECTFILLATTRIBUTES_C_H

#include "vtobject_c.h"
#include "../ivtobjectpicturegraphic_c.h"


namespace __IsoAgLib {

class vtObjectFillAttributes_c : public vtObject_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  void init(const iVtObjectFillAttributes_s* vtObjectFillAttributesSROM , int ai_multitonInst)
  { vtObject_c::init ((iVtObject_s*) vtObjectFillAttributesSROM , ai_multitonInst); }

  iVtObjectFillAttributes_s* get_vtObjectFillAttributes_a() { return (iVtObjectFillAttributes_s *)&(get_vtObject_a()); }

  vtObjectFillAttributes_c();

  uint32_t fitTerminal() const;

  void setFillType(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillType) : 0, sizeof(iVtObjectFillAttributes_s), 1 /* "Fill Type" */, newValue, newValue, b_enableReplaceOfCmd);
  }

  void setFillColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillColour) : 0, sizeof(iVtObjectFillAttributes_s), 2 /* "Fill Colour" */, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::FillColour), b_enableReplaceOfCmd);
  }

  void setFillPattern(IsoAgLib::iVtObjectPictureGraphic_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillPatternObject) : 0, sizeof(iVtObjectFillAttributes_s), 3 /* "Fill Pattern" */, newValue, b_enableReplaceOfCmd);
  }

  void setFillAttributes(uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPattern, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 25; }
  */

  uint8_t updateFillType(bool b_SendRequest=false);

  uint8_t updateFillColour(bool b_SendRequest=false);

  uint16_t updateFillPattern(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif
