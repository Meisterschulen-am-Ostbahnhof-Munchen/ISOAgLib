/*
  ivtobjectnumbervariable_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTNUMBERVARIABLE_C_H
#define IVTOBJECTNUMBERVARIABLE_C_H


// +X2C includes
#include "impl/vtobjectnumbervariable_c.h"
// ~X2C
#ifdef USE_VTOBJECT_numbervariable

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 156 : iVtObjectNumberVariable_c
//!  Stereotype: Klasse
class iVtObjectNumberVariable_c : public __IsoAgLib::vtObjectNumberVariable_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectNumberVariableSROM:
  //! @param b_initPointer:
  void init(const iVtObjectNumberVariable_s* vtObjectNumberVariableSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectNumberVariable_c::init (vtObjectNumberVariableSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectNumberVariable_a
  const iVtObjectNumberVariable_s& get_vtObjectNumberVariable_a() { return *vtObjectNumberVariable_c::get_vtObjectNumberVariable_a(); }

  //  Operation: setValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) {
    vtObjectNumberVariable_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd); }


#ifdef USE_ISO_TERMINAL_GETATTRIBUTES  
  // ///////////////////////// getter for attributes
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectNumberVariable_c::updateObjectType(); }
  
  uint32_t updateValue(bool b_SendRequest=false) {
    return vtObjectNumberVariable_c::getValue(b_SendRequest);
  }
  */
#endif
}; // ~X2C

} // end of namespace IsoAgLib

#endif // -X2C
#endif
