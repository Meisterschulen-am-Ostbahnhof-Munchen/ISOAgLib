/*
  ivtobjectrectangle_c.h

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
#ifndef IVTOBJECTRECTANGLE_C_H
#define IVTOBJECTRECTANGLE_C_H

#include "impl/vtobjectrectangle_c.h"


#ifdef CONFIG_USE_VTOBJECT_rectangle

namespace IsoAgLib {

class iVtObjectRectangle_c : public __IsoAgLib::vtObjectRectangle_c
{
public:
  static ObjectType objectType() { return VT_OBJECT_TYPE_RECTANGLE; }

  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  void setLineAttributes(iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setLineAttributes (newLineAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  void setLineSuppression(IsoAgLib::LineSuppression  newLineSupressionValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setLineSuppression (newLineSupressionValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setFillAttributes(iVtObjectFillAttributes_c* newFillAttribute, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setFillAttributes (newFillAttribute, b_updateObject, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectRectangle_c::updateObjectType(); }
  */

  iVtObjectLineAttributes_c* updateLineAttributes(bool b_SendRequest=false) {
    return vtObjectRectangle_c::updateLineAttributes(b_SendRequest);
  }

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectRectangle_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectRectangle_c::updateHeight(b_SendRequest);
  }

  IsoAgLib::LineSuppression  updateLineSuppression(bool b_SendRequest=false) {
    return vtObjectRectangle_c::updateLineSuppression(b_SendRequest);
  }

  iVtObjectFillAttributes_c* updateFillAttributes(bool b_SendRequest=false) {
    return vtObjectRectangle_c::updateFillAttributes(b_SendRequest);
  }
#endif

  virtual ObjectType getObjectType() const { return objectType(); }
};



} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_rectangle

#endif //IVTOBJECTRECTANGLE_C_H
