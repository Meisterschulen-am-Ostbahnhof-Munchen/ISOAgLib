/*
  ivtobjectrectangle_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTRECTANGLE_C_H
#define IVTOBJECTRECTANGLE_C_H


// +X2C includes
#include "impl/vtobjectrectangle_c.h"
// ~X2C
#ifdef USE_VTOBJECT_rectangle

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 119 : iVtObjectRectangle_c
//!  Stereotype: Klasse
class iVtObjectRectangle_c : public __IsoAgLib::vtObjectRectangle_c
{

public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_RECTANGLE; }

  //  Operation: init
  //! Parameter:
  //! @param vtObjectrectangleSROM:
  //! @param b_initPointer:
  void init(const iVtObjectRectangle_s* vtObjectRectangleSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectRectangle_c::init (vtObjectRectangleSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectmeter_a
  const iVtObjectRectangle_s& get_vtObjectRectangle_a() { return *vtObjectRectangle_c::get_vtObjectRectangle_a(); }

  //  Operation: setWidth
  //! Parameter:
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setLineAttributes
  //! Parameter:
  //! @param newLineAttribute:
  //! @param b_updateObject:
  void setLineAttributes(iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setLineAttributes (newLineAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setLineSuppression
  //! Parameter:
  //! @param newLineSupressionValue:
  //! @param b_updateObject:
  void setLineSuppression(uint8_t newLineSupressionValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setLineSuppression (newLineSupressionValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setFillAttributes
  //! Parameter:
  //! @param newFillAttribute:
  //! @param b_updateObject:
  void setFillAttributes(iVtObjectFillAttributes_c* newFillAttribute, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setFillAttributes (newFillAttribute, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectRectangle_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectRectangle_c::updateObjectType(); }
  */

  uint16_t updateLineAttributes(bool b_SendRequest=false) {
    return vtObjectRectangle_c::updateLineAttributes(b_SendRequest);
  }

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectRectangle_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectRectangle_c::updateHeight(b_SendRequest);
  }

  uint8_t updateLineSuppression(bool b_SendRequest=false) {
    return vtObjectRectangle_c::updateLineSuppression(b_SendRequest);
  }

  uint16_t updateFillAttributes(bool b_SendRequest=false) {
    return vtObjectRectangle_c::updateFillAttributes(b_SendRequest);
  }
#endif

  virtual uint16_t getObjectType() { return objectType(); }
};

} // end namespace __IsoAgLib

#endif
#endif
