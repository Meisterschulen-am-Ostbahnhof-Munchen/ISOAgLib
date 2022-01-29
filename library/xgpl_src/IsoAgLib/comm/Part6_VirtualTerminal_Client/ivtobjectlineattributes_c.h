/*
  ivtobjectlineattributes_c.h

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
#ifndef IVTOBJECTLINEATTRIBUTES_C_H
#define IVTOBJECTLINEATTRIBUTES_C_H

#include "impl/vtobjectlineattributes_c.h"


namespace IsoAgLib {

class iVtObjectLineAttributes_c : public __IsoAgLib::vtObjectLineAttributes_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_LINE_ATTRIBUTES; }

  iVtObjectLineAttributes_c(const iVtObjectLineAttributes_s* vtObjectLineAttributesSROM , int ai_multitonInst)
  :vtObjectLineAttributes_c(vtObjectLineAttributesSROM , ai_multitonInst)
  {}

  const iVtObjectLineAttributes_s& get_vtObjectLineAttributes_a() { return *vtObjectLineAttributes_c::get_vtObjectLineAttributes_a (); }

  void setLineColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLineAttributes_c::setLineColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setLineWidth(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLineAttributes_c::setLineWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setLineArt(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLineAttributes_c::setLineArt (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setLineAttributes(uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLineAttributes_c::setLineAttributes (newLineColour, newLineWidth, newLineArt, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectLineAttributes_c::updateObjectType(); }
  */

  uint8_t updateLineColour(bool b_SendRequest=false) {
    return vtObjectLineAttributes_c::updateLineColour(b_SendRequest);
  }

  uint8_t updateLineWidth(bool b_SendRequest=false) {
    return vtObjectLineAttributes_c::updateLineWidth(b_SendRequest);
  }

  uint16_t updateLineArt(bool b_SendRequest=false) {
    return vtObjectLineAttributes_c::updateLineArt(b_SendRequest);
  }
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif
