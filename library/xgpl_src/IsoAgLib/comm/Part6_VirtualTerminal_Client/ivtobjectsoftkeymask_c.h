/*
  ivtobjectsoftkeymask_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTSOFTKEYMASK_C_H
#define IVTOBJECTSOFTKEYMASK_C_H


// +X2C includes
#include "impl/vtobjectsoftkeymask_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 81 : iVtObjectSoftKeyMask_c
//!  Stereotype: Klasse
class iVtObjectSoftKeyMask_c : public __IsoAgLib::vtObjectSoftKeyMask_c
{

public:
  virtual ~iVtObjectSoftKeyMask_c(){}
  //  Operation: init
  //! Parameter:
  //! @param vtObjectSoftKeyMaskSROM:
  //! @param b_initPointer:
  void init(const iVtObjectSoftKeyMask_s* vtObjectSoftKeyMaskSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectSoftKeyMask_c::init (vtObjectSoftKeyMaskSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectSoftKeyMask_a
  const iVtObjectSoftKeyMask_s& get_vtObjectSoftKeyMask_a() { return *vtObjectSoftKeyMask_c::get_vtObjectSoftKeyMask_a(); }

  //  Operation: setBackgroundColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setBackgroundColour(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectSoftKeyMask_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES  
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const {
    return vtObjectSoftKeyMask_c::updateObjectType();
  }
  */
  
  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectSoftKeyMask_c::updateBackgroundColour(b_SendRequest);
  }
#endif
}; // ~X2C

} // end of namespace IsoAgLib

#endif // -X2C
