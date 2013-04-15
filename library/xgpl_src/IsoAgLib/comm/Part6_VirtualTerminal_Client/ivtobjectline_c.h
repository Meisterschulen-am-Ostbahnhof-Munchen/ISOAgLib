/*
  ivtobjectline_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTLINE_C_H
#define IVTOBJECTLINE_C_H

// +X2C includes
#include "impl/vtobjectline_c.h"
// ~X2C
#ifdef USE_VTOBJECT_line

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 142 : iVtObjectLine_c
//!  Stereotype: Klasse
class iVtObjectLine_c : public __IsoAgLib::vtObjectLine_c
{

public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_LINE; }

  //  Operation: init
  //! Parameter:
  //! @param vtObjectLineSROM:
  //! @param b_initPointer:
  void init(const iVtObjectLine_s* vtObjectLineSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectLine_c::init (vtObjectLineSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectLine_a
  const iVtObjectLine_s& get_vtObjectLine_a() { return *vtObjectLine_c::get_vtObjectLine_a(); }


  //  Operation: setLineAttributes
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setLineAttributes(iVtObjectLineAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLine_c::setLineAttributes(newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setWidth
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setWidth (int32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLine_c::setWidth(newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setHeight (int32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLine_c::setHeight(newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setLineDirection
  //! @param newValue:
  //! @param b_updateObject:
  void setLineDirection (uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLine_c::setLineDirection(newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setLineDirection
  //! @param newValue:
  //! @param b_updateObject:
  void setEndPoint (uint16_t newWidth, uint16_t newHeight, uint8_t newLineDirection, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLine_c::setEndPoint(newWidth, newHeight, newLineDirection, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSize
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLine_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectLine_c::updateObjectType(); }
  */

  uint16_t updateLineAttributes(bool b_SendRequest=false) {
    return vtObjectLine_c::updateLineAttributes(b_SendRequest);
  }

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectLine_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectLine_c::updateHeight(b_SendRequest);
  }

  uint8_t updateLineDirection(bool b_SendRequest=false) {
    return vtObjectLine_c::updateLineDirection(b_SendRequest);
  }
#endif

  virtual uint16_t getObjectType() { return objectType(); }
};

} // end of namespace IsoAgLib

#endif
#endif
