/*
  ivtobjectauxiliaryfunction_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTAUXILIARYFUNCTION_C_H
#define IVTOBJECTAUXILIARYFUNCTION_C_H

// +X2C includes
#include "impl/vtobjectauxiliaryfunction_c.h"
// ~X2C

#ifdef USE_VTOBJECT_auxiliaryfunction

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 179 : iVtObjectAuxiliaryFunction_c
//!  Stereotype: Klasse
class iVtObjectAuxiliaryFunction_c : public __IsoAgLib::vtObjectAuxiliaryFunction_c
{

public:
  ~iVtObjectAuxiliaryFunction_c(){};
  //  Operation: init
  //! Parameter:
  //! @param vtObjectAuxiliaryFunctionSROM:
  //! @param b_initPointer:
  void init(const iVtObjectAuxiliaryFunction_s* vtObjectAuxiliaryFunctionSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) { vtObject_c::init ((iVtObject_s*) vtObjectAuxiliaryFunctionSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); };


  //  Operation: get_vtObjectAuxiliaryFunction_a
  const iVtObjectAuxiliaryFunction_s& get_vtObjectAuxiliaryFunction_a() { return *vtObjectAuxiliaryFunction_c::get_vtObjectAuxiliaryFunction_a(); };

  //  Operation: moveChildLocation
  //! Parameter:
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  //! @param b_updateObject:
  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectAuxiliaryFunction_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  //  Operation: setChildPosition
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectAuxiliaryFunction_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectAuxiliaryFunction_c::updateObjectType(); }
  */
#endif
};

} // end of namespace IsoAgLib

#endif
#endif
