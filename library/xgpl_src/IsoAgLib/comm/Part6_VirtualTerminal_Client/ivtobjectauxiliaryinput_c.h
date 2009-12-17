/*
  ivtobjectauxiliaryinput_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTAUXILIARYINPUT_C_H
#define IVTOBJECTAUXILIARYINPUT_C_H


// +X2C includes
#include "impl/vtobjectauxiliaryinput_c.h"
// ~X2C
#ifdef USE_VTOBJECT_auxiliaryinput

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 179 : iVtObjectAuxiliaryInput_c
//!  Stereotype: Klasse
class iVtObjectAuxiliaryInput_c : public __IsoAgLib::vtObjectAuxiliaryInput_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectAuxiliaryInputSROM:
  //! @param b_initPointer:
  void init(const iVtObjectAuxiliaryInput_s* vtObjectAuxiliaryInputSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) { vtObject_c::init ((iVtObject_s*) vtObjectAuxiliaryInputSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA); }


  //  Operation: get_vtObjectAuxiliaryInput_a
  const iVtObjectAuxiliaryInput_s& get_vtObjectAuxiliaryInput_a() { return *vtObjectAuxiliaryInput_c::get_vtObjectAuxiliaryInput_a(); }

  //  Operation: moveChildLocation
  //! Parameter:
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  //! @param b_updateObject:
  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectAuxiliaryInput_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  //  Operation: setChildPosition
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectAuxiliaryInput_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectAuxiliaryInput_c::updateObjectType(); }
  */
#endif
};

} // end of namespace IsoAgLib

#endif
#endif
