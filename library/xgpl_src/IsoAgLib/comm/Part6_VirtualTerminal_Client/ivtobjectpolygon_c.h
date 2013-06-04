/*
  ivtobjectpolygon_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTPOLYGON_C_H
#define IVTOBJECTPOLYGON_C_H


// +X2C includes
#include "impl/vtobjectpolygon_c.h"
// ~X2C
#ifdef USE_VTOBJECT_polygon

// Begin Namespace IsoAgLib
namespace IsoAgLib {



//  +X2C Class 119 : iVtObjectPolygon_c
//!  Stereotype: Klasse
class iVtObjectPolygon_c : public __IsoAgLib::vtObjectPolygon_c
{

public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_POLYGON; }

  //  Operation: init
  //! Parameter:
  //! @param vtObjectpolygonSROM:
  //! @param b_initPointer:
  void init(const iVtObjectPolygon_s* vtObjectPolygonSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectPolygon_c::init (vtObjectPolygonSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectPolygon_a
  const iVtObjectPolygon_s& get_vtObjectPolygon_a() { return *vtObjectPolygon_c::get_vtObjectPolygon_a(); }

  //  Operation: setWidth
  //! Parameter:
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setLineAttributes
  //! Parameter:
  //! @param newLineAttributes:
  //! @param b_updateObject:
  void setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setLineAttributes (newLineAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setFillAttributes
  //! Parameter:
  //! @param newFillAttributes:
  //! @param b_updateObject:
  void setFillAttributes(IsoAgLib::iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setFillAttributes (newFillAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setPolygonType
  //! Parameter:
  //! @param newPolygonType:
  //! @param b_updateObject:
  void setPolygonType(uint8_t newPolygonType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setPolygonType (newPolygonType, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectPolygon_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateHeight(b_SendRequest);
  }

  uint16_t updateLineAttributes(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateLineAttributes(b_SendRequest);
  }

  uint16_t updateFillAttributes(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateFillAttributes(b_SendRequest);
  }

  uint8_t updatePolygonType(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updatePolygonType(b_SendRequest);
  }
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // end namepsace IsoAgLibb

#endif
#endif
