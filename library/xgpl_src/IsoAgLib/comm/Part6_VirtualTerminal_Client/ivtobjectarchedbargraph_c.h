/*
  ivtobjectarchedbargraph_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTARCHEDBARGRAPH_C_H
#define IVTOBJECTARCHEDBARGRAPH_C_H

#include "impl/vtobjectarchedbargraph_c.h"

#ifdef USE_VTOBJECT_archedbargraph

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 119 : vtObjectArchedBarGraph_c
//!  Stereotype: Klasse
class iVtObjectArchedBarGraph_c : public __IsoAgLib::vtObjectArchedBarGraph_c
{

public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_ARCHED_BAR_GRAPH; }

  //  Operation: init
  //! Parameter:
  //! @param vtObjectArchedBarGraphSROM:
  //! @param b_initPointer:
  void init(const iVtObjectArchedBarGraph_s* vtObjectArchedBarGraphSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectArchedBarGraph_c::init (vtObjectArchedBarGraphSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectArchedBarGraph_a
  const iVtObjectArchedBarGraph_s& get_vtObjectArchedBarGraph_a() { return *vtObjectArchedBarGraph_c::get_vtObjectArchedBarGraph_a(); }
  ~iVtObjectArchedBarGraph_c(){}

  //  Operation: setValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) {
    vtObjectArchedBarGraph_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setWidth
  //! Parameter:
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newWidth, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newHeight, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setColour
  //! Parameter:
  //! @param newColour:
  //! @param b_updateObject:
  void setColour(uint8_t newColour, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setColour (newColour, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setTargetLineColour
  //! Parameter:
  //! @param newTargetLineColour:
  //! @param b_updateObject:
  void setTargetLineColour(uint8_t newTargetLineColour, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setTargetLineColour (newTargetLineColour, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setOptions
  //! Parameter:
  //! @param newOptions:
  //! @param b_updateObject:
  void setOptions(uint8_t newOptions, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setOptions (newOptions, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setStartAngle
  //! Parameter:
  //! @param newStartAngle:
  //! @param b_updateObject:
  void setStartAngle(uint8_t newStartAngle, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setStartAngle (newStartAngle, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setEndAngle
  //! Parameter:
  //! @param newEndAngle:
  //! @param b_updateObject:
  void setEndAngle(uint8_t newEndAngle, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setEndAngle (newEndAngle, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setBarGraphWidth
  //! Parameter:
  //! @param newBarGraphWidth:
  //! @param b_updateObject:
  void setBarGraphWidth(uint16_t newBarGraphWidth, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setBarGraphWidth (newBarGraphWidth, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setMinValue
  //! Parameter:
  //! @param newMin:
  //! @param b_updateObject:
  void setMinValue(uint32_t newMin, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setMinValue (newMin, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setMaxValue
  //! Parameter:
  //! @param newMax:
  //! @param b_updateObject:
  void setMaxValue(uint32_t newMax, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setMaxValue (newMax, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newVariableReference:
  //! @param b_updateObject:
  void setVariableReference(iVtObject_c* newVariableReference, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setVariableReference (newVariableReference, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setTargetValueVariableReference
  //! Parameter:
  //! @param newTargetValueVariableReference:
  //! @param b_updateObject:
  void setTargetValueVariableReference(iVtObject_c* newTargetValueVariableReference, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setTargetValueVariableReference (newTargetValueVariableReference, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setTargetValue
  //! Parameter:
  //! @param newTargetValue:
  //! @param b_updateObject:
  void setTargetValue(uint16_t newTargetValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setTargetValue (newTargetValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectArchedBarGraph_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateHeight(b_SendRequest);
  }

  uint8_t updateColour(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateColour(b_SendRequest);
  }

  uint8_t updateTargetLineColour(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateTargetLineColour(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateOptions(b_SendRequest);
  }

  uint8_t updateStartAngle(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateStartAngle(b_SendRequest);
  }

  uint8_t updateEndAngle(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateEndAngle(b_SendRequest);
  }

  uint16_t updateBarGraphWidth(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateBarGraphWidth(b_SendRequest);
  }

  uint16_t updateMinValue(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateMinValue(b_SendRequest);
  }

  uint16_t updateMaxValue(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateMaxValue(b_SendRequest);
  }

  uint16_t updateVariableReference(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateVariableReference(b_SendRequest);
  }

  uint16_t updateTargetValueVariableReference(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateTargetValueVariableReference(b_SendRequest);
  }

  uint16_t updateTargetValue(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::updateTargetValue(b_SendRequest);
  }

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::getValue(b_SendRequest);
  }
  */
#endif

  virtual uint16_t getObjectType() { return objectType(); }
};

} // end namespace __IsoAgLib

#endif
#endif
