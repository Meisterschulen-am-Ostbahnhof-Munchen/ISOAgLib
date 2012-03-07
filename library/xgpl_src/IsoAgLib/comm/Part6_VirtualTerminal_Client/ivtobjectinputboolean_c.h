/*
  ivtobjectinputboolean_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTINPUTBOOLEAN_C_H
#define IVTOBJECTINPUTBOOLEAN_C_H

// +X2C includes
#include "impl/vtobjectinputboolean_c.h"
// ~X2C
#ifdef USE_VTOBJECT_inputboolean

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 103 : iVtObjectInputBoolean_c
//!  Stereotype: Klasse
class iVtObjectInputBoolean_c : public __IsoAgLib::vtObjectInputBoolean_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectInputBooleanSROM:
  //! @param b_initPointer:
  void init(const iVtObjectInputBoolean_s* vtObjectInputBooleanSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectInputBoolean_c::init (vtObjectInputBooleanSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectInputBoolean_a
  const iVtObjectInputBoolean_s& get_vtObjectInputBoolean_a() { return *vtObjectInputBoolean_c::get_vtObjectInputBoolean_a(); }

  //  Operation: setValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(bool newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputBoolean_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setBackgroundColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputBoolean_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setWidth
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputBoolean_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setForegroundColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setForegroundColour(iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputBoolean_c::setForegroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setVariableReference(iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputBoolean_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

    //  Operation: enable
  //! Parameter:
  //! @param b_updateObject:
  bool enable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) { return vtObject_c::able (1, b_updateObject, b_enableReplaceOfCmd); }

  //  Operation: disable
  //! Parameter:
  //! @param b_updateObject:
  bool disable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) { return vtObject_c::able (0, b_updateObject, b_enableReplaceOfCmd); }

  //  Operation: select
  bool select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES  
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputBoolean_c::updateObjectType(); }
  */
  
  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::updateBackgroundColour(b_SendRequest);
  }
  
  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::updateWidth(b_SendRequest);
  }

  uint16_t updateForegroundColour(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::updateForegroundColour(b_SendRequest);
  }

  uint16_t updateVariableReference(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::updateVariableReference(b_SendRequest);
  }

  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateValue(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::getValue(b_SendRequest);
  }

  uint8_t updateEnabled(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::updateEnabled(b_SendRequest);
  }
  */
#endif
}; // ~X2C

} // end of namespace IsoAgLib

#endif // -X2C
#endif
