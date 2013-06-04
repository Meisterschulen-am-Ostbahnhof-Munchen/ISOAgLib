/*
  ivtobjectellipse_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTELLIPSE_C_H
#define IVTOBJECTELLIPSE_C_H

// +X2C includes
#include "impl/vtobjectellipse_c.h"
// ~X2C
#ifdef USE_VTOBJECT_ellipse

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 119 : vtObjectellipse_c
//!  Stereotype: Klasse
class iVtObjectEllipse_c : public __IsoAgLib::vtObjectEllipse_c
{

public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_ELLIPSE; }

  //  Operation: init
  //! Parameter:
  //! @param vtObjectellipseSROM:
  //! @param b_initPointer:
  void init(const iVtObjectEllipse_s* vtObjectellipseSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectEllipse_c::init (vtObjectellipseSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectmeter_a
  const iVtObjectEllipse_s& get_vtObjectEllipse_a() { return *vtObjectEllipse_c::get_vtObjectEllipse_a(); }

  //  Operation: setLineAttributes
  //! Parameter:
  //! @param newLineAttributes:
  //! @param b_updateObject:
  void setLineAttributes(iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectEllipse_c::setLineAttributes (newLineAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setWidth
  //! Parameter:
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectEllipse_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectEllipse_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setEllipseType
  //! Parameter:
  //! @param newEllipseType:
  //! @param b_updateObject:
  void setEllipseType(uint8_t newEllipseType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectEllipse_c::setEllipseType (newEllipseType, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setStartAngle
  //! Parameter:
  //! @param newStartAngle:
  //! @param b_updateObject:
  void setStartAngle(uint8_t newStartAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectEllipse_c::setStartAngle (newStartAngle, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setEndAngle
  //! Parameter:
  //! @param newEndAngle:
  //! @param b_updateObject:
  void setEndAngle(uint8_t newEndAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectEllipse_c::setEndAngle (newEndAngle, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setFillAttributes
  //! Parameter:
  //! @param newFillAttribute:
  //! @param b_updateObject:
  void setFillAttributes(iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectEllipse_c::setFillAttributes (newFillAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectEllipse_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectEllipse_c::updateObjectType(); }
  */
  
  uint16_t updateLineAttributes(bool b_SendRequest=false) {
    return vtObjectEllipse_c::updateLineAttributes(b_SendRequest);
  }

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectEllipse_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectEllipse_c::updateHeight(b_SendRequest);
  }

  uint8_t updateEllipseType(bool b_SendRequest=false) {
    return vtObjectEllipse_c::updateEllipseType(b_SendRequest);
  }

  uint8_t updateStartAngle(bool b_SendRequest=false) {
    return vtObjectEllipse_c::updateStartAngle(b_SendRequest);
  }

  uint8_t updateEndAngle(bool b_SendRequest=false) {
    return vtObjectEllipse_c::updateEndAngle(b_SendRequest);
  }

  uint16_t updateFillAttributes(bool b_SendRequest=false) {
    return vtObjectEllipse_c::updateFillAttributes(b_SendRequest);
  }
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
}; // ~X2C

} // end namespace IsoAgLib

#endif // -X2C
#endif
