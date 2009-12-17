/*
  ivtobjectmeter_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTMETER_C_H
#define IVTOBJECTMETER_C_H

// +X2C includes
#include "impl/vtobjectmeter_c.h"
// ~X2C
#ifdef USE_VTOBJECT_meter

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 119 : iVtObjectmeter_c
//!  Stereotype: Klasse
class iVtObjectMeter_c : public __IsoAgLib::vtObjectMeter_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectmeterSROM:
  //! @param b_initPointer:
  void init(const iVtObjectMeter_s* vtObjectMeterSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectMeter_c::init (vtObjectMeterSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectMeter_a
  const iVtObjectMeter_s& get_vtObjectMeter_a() { return *vtObjectMeter_c::get_vtObjectMeter_a(); }

  //  Operation: setWidth
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setNeedleColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setNeedleColour(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setNeedleColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setBorderColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setBorderColour(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setBorderColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setArcAndTickColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setArcAndTickColour(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setArcAndTickColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setOptions
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setOptions(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setNumberOfTicks
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setNumberOfTicks(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setNumberOfTicks (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setStartAngle
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setStartAngle(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setStartAngle (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setEndAngle
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setEndAngle(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setEndAngle (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setMin
  //! Parameter:
  //! @param newMin:
  //! @param b_updateObject:
  void setMin(uint32_t newMin, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setMin (newMin, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setMax
  //! Parameter:
  //! @param newMax:
  //! @param b_updateObject:
  void setMax(uint32_t newMax, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setMax (newMax, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) {
    vtObjectMeter_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setVariableReference(iVtObject_c* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
  }
  
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectMeter_c::updateObjectType(); }
  */
  
  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateWidth(b_SendRequest);
  }

  uint8_t updateNeedleColour(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateNeedleColour(b_SendRequest);
  }

  uint8_t updateBorderColour(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateBorderColour(b_SendRequest);
  }

  uint8_t updateArcAndTickColour(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateArcAndTickColour(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateOptions(b_SendRequest);
  }

  uint8_t updateNumberOfTicks(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateNumberOfTicks(b_SendRequest);
  }

  uint8_t updateStartAngle(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateStartAngle(b_SendRequest);
  }

  uint8_t updateEndAngle(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateEndAngle(b_SendRequest);
  }

  uint16_t updateMinValue(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateMinValue(b_SendRequest);
  }

  uint16_t updateMaxValue(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateMaxValue(b_SendRequest);
  }

  uint16_t updateVariableReference(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateVariableReference(b_SendRequest);
  }

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false) {
    return vtObjectMeter_c::getValue(b_SendRequest);
  }
  */
#endif
}; // ~X2C

} // end namespace IsoAgLib

#endif // -X2C
#endif
