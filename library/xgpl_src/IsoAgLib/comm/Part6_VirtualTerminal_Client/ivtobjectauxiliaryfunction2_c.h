/*
  ivtobjectauxiliaryfunction_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTAUXILIARYFUNCTION2_C_H
#define IVTOBJECTAUXILIARYFUNCTION2_C_H

// +X2C includes
#include "impl/vtobjectauxiliaryfunction2_c.h"
// ~X2C

#ifdef USE_VTOBJECT_auxiliaryfunction2

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 179 : iVtObjectAuxiliaryFunction2_c
//!  Stereotype: Klasse
class iVtObjectAuxiliaryFunction2_c : public __IsoAgLib::vtObjectAuxiliaryFunction2_c
{
public:
  static uint16_t objectType() { return 31; }

  ~iVtObjectAuxiliaryFunction2_c(){};
  //  Operation: init
  //! Parameter:
  //! @param vtObjectAuxiliaryFunction2SROM:
  //! @param b_initPointer:
  void init(const iVtObjectAuxiliaryFunction2_s* vtObjectAuxiliaryFunction2SROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) { vtObject_c::init ((iVtObject_s*) vtObjectAuxiliaryFunction2SROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); };


  //  Operation: get_vtObjectAuxiliaryFunction2_a
  const iVtObjectAuxiliaryFunction2_s& get_vtObjectAuxiliaryFunction2_a() { return *vtObjectAuxiliaryFunction2_c::get_vtObjectAuxiliaryFunction2_a(); };

  //  Operation: moveChildLocation
  //! Parameter:
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  //! @param b_updateObject:
  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectAuxiliaryFunction2_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  //  Operation: setChildPosition
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectAuxiliaryFunction2_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectAuxiliaryFunction2_c::updateObjectType(); }
  */
#endif

  void getPreferredAssignedInput(__IsoAgLib::IsoName_c& arc_isoName, uint16_t& arui16_inputModelIdentificationCode, uint16_t& arui16_inputUid) const
  { __IsoAgLib::vtObjectAuxiliaryFunction2_c::getPreferredAssignedInput(arc_isoName, arui16_inputModelIdentificationCode, arui16_inputUid); }

  //! @return true, if values are changed
  bool setPreferredAssignedInput(const __IsoAgLib::IsoName_c& arc_isoName, uint16_t aui16_inputModelIdentificationCode, uint16_t aui16_inputUid)
  { return __IsoAgLib::vtObjectAuxiliaryFunction2_c::setPreferredAssignedInput(arc_isoName, aui16_inputModelIdentificationCode, aui16_inputUid); }

  //! @return true, if AUX2 input is assigned
  bool isAssigned() const
  { return __IsoAgLib::vtObjectAuxiliaryFunction2_c::isAssigned(); }

  virtual uint16_t getObjectType() { return objectType(); }
};

} // end of namespace IsoAgLib

#endif
#endif
