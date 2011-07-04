/*
  ivtobjectcontainer_c.cpp

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTCONTAINER_C_H
#define IVTOBJECTCONTAINER_C_H

// +X2C includes
#include "impl/vtobjectcontainer_c.h"
// ~X2C

#ifdef USE_VTOBJECT_container

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 4 : iVtObjectContainer_c
//!  Stereotype: Klasse
class iVtObjectContainer_c : public __IsoAgLib::vtObjectContainer_c
{

public:
  iVtObjectContainer_c();
  ~iVtObjectContainer_c();

  //  Operation: init
  //! Parameter:
  //! @param vtObjectContainer_sROM:
  //! @param b_initPointer:
  void init(const iVtObjectContainer_s* vtObjectContainer_sROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectContainer_c::init (vtObjectContainer_sROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectContainer_a
  const iVtObjectContainer_s& get_vtObjectContainer_a() { return *vtObjectContainer_c::get_vtObjectContainer_a(); }

  //  Operation: hide
  //! Parameter:
  //! @param b_updateObject:
  void hide(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) { vtObjectContainer_c::hideShow (0, b_updateObject, b_enableReplaceOfCmd); }

  //  Operation: show
  //! Parameter:
  //! @param b_updateObject:
  void show(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) { vtObjectContainer_c::hideShow (1, b_updateObject, b_enableReplaceOfCmd); }

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectContainer_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: moveChildLocation
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  //! @param b_updateObject:
  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectContainer_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  //  Operation: setChildPosition
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectContainer_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const {
    return vtObjectContainer_c::updateObjectType();
  }


  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectContainer_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectContainer_c::updateHeight(b_SendRequest);
  }

  uint8_t updateHidden(bool b_SendRequest=false) {
    return vtObjectContainer_c::updateHidden(b_SendRequest);
  }
  */
#endif
};

} // end of namespace IsoAgLib

#endif
#endif
