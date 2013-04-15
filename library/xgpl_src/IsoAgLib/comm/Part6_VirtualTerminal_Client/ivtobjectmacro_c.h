/*
  ivtobjectmacro_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTMACRO_C_H
#define IVTOBJECTMACRO_C_H


// +X2C includes
#include "impl/vtobjectmacro_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {



//  +X2C Class 179 : iVtObjectMacro_c
//!  Stereotype: Klasse
class iVtObjectMacro_c : public __IsoAgLib::vtObjectMacro_c
{

public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_MACRO; }

  //  Operation: init
  //! Parameter:
  //! @param vtObjectMacroSROM:
  //! @param b_initPointer:
  void init(const iVtObjectMacro_s* vtObjectMacroSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectMacro_c::init (vtObjectMacroSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectMacro_a
  const iVtObjectMacro_s& get_vtObjectMacro_a() { return *vtObjectMacro_c::get_vtObjectMacro_a(); }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES  
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectMacro_c::updateObjectType(); }
  */
#endif

  virtual uint16_t getObjectType() { return objectType(); }
}; // ~X2C

} // end namespace IsoAgLib

#endif // -X2C
