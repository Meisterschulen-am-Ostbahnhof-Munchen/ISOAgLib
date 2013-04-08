/*
  ivtobjectlineattributes_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTLINEATTRIBUTES_C_H
#define IVTOBJECTLINEATTRIBUTES_C_H

// +X2C includes
#include "impl/vtobjectlineattributes_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 172 : iVtObjectLineAttributes_c
//!  Stereotype: Klasse
class iVtObjectLineAttributes_c : public __IsoAgLib::vtObjectLineAttributes_c
{

public:

  /// Operation: init
  ///
  /// @param vtObjectLineAttributesSROM
  /// @param b_initPointer
  void init(const iVtObjectLineAttributes_s* vtObjectLineAttributesSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectLineAttributes_c::init (vtObjectLineAttributesSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectLineAtrtibutes_a
  const iVtObjectLineAttributes_s& get_vtObjectLineAttributes_a() { return *vtObjectLineAttributes_c::get_vtObjectLineAttributes_a (); }

  /// Operation: setLineColour
  ///
  /// @param newValue
  /// @param b_updateObject (default:false)
  /// @param b_enableReplaceOfCmd (default:false)
  void setLineColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLineAttributes_c::setLineColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  /// Operation: setLineWidth
  ///
  /// @param newValue:
  /// @param b_updateObject:
  void setLineWidth(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLineAttributes_c::setLineWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  /// Operation: setLineArt
  ///
  /// @param newValue:
  /// @param b_updateObject:
  void setLineArt(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLineAttributes_c::setLineArt (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  /// Operation: setLineAttributes
  ///
  /// @param newLineColour
  /// @param b_updateObject
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
}; // ~X2C

} // end of namespace IsoAgLib

#endif // -X2C
