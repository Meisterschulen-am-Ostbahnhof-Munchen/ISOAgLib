/*
  ivtobjectinputattributes_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTINPUTATTRIBUTES_C_H
#define IVTOBJECTINPUTATTRIBUTES_C_H


// +X2C includes
#include "impl/vtobjectinputattributes_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 167 : iVtObjectInputAttributes_c
//!  Stereotype: Klasse
class iVtObjectInputAttributes_c : public __IsoAgLib::vtObjectInputAttributes_c
{

public:

  /// Operation: init
  ///
  /// @param vtObjectInputAttributesSROM
  /// @param b_initPointer
  void init(const iVtObjectInputAttributes_s* vtObjectInputAttributesSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectInputAttributes_c::init (vtObjectInputAttributesSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectInputAttributes_a
  const iVtObjectInputAttributes_s& get_vtObjectInputAttributes_a() { return *vtObjectInputAttributes_c::get_vtObjectInputAttributes_a(); }



 //  Operation: getString
  const char* getString(){
      return vtObjectInputAttributes_c::getString();}


  //  Operation: setValidationStringCopy
  //! Parameter:
  //! @param newValidationString:
  //! @param b_updateObject:
  void setValidationStringCopy(const char* newValidationString, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputAttributes_c::setValidationStringCopy (newValidationString, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setValidationString
  //! Parameter:
  //! @param newValidationString:
  //! @param b_updateObject:
  void setValidationStringRef(const char* newValidationString, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputAttributes_c::setValidationStringRef (newValidationString, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputAttributes_c::updateObjectType(); }
  
  uint8_t updateValidationType(bool b_SendRequest=false) {
    return vtObjectInputAttributes_c::updateValidationType(b_SendRequest);
  }
  */
#endif
};

} // end namespace IsoAgLib

#endif // -X2C
