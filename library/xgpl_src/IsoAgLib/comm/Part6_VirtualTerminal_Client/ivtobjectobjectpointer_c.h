/*
  ivtobjectobjectpointer_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTOBJECTPOINTER_C_H
#define IVTOBJECTOBJECTPOINTER_C_H

// +X2C includes
#include "impl/vtobjectobjectpointer_c.h"
// ~X2C
#ifdef USE_VTOBJECT_objectpointer

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 179 : iVtObjectObjectPointer_c
//!  Stereotype: Klasse
class iVtObjectObjectPointer_c : public __IsoAgLib::vtObjectObjectPointer_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectObjectPointerSROM:
  //! @param b_initPointer:
  void init(const iVtObjectObjectPointer_s* vtObjectObjectPointerSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectObjectPointer_c::init (vtObjectObjectPointerSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectObjectPointer_a
  const iVtObjectObjectPointer_s& get_vtObjectObjectPointer_a() { return *vtObjectObjectPointer_c::get_vtObjectObjectPointer_a(); }

  //  Operation: setValue
  //! Parameter:
  //! @param apc_newObject:
  //! @param b_updateObject:
  void setValue(iVtObject_c* apc_newObject, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectObjectPointer_c::setValue (apc_newObject, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputAttributes_c::updateObjectType(); }
  
  uint16_t updateValue(bool b_SendRequest=false) {
    return vtObjectInputAttributes_c::getValue(b_SendRequest);
  }
  */
#endif
};

} // end of namespace IsoAgLib

#endif // -X2C
#endif
