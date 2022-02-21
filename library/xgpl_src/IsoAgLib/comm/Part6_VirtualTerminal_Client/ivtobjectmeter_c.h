/*
  ivtobjectmeter_c.h

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTMETER_C_H
#define IVTOBJECTMETER_C_H

#include "impl/vtobjectmeter_c.h"


#ifdef CONFIG_USE_VTOBJECT_meter

namespace IsoAgLib {

class iVtObjectMeter_c : public __IsoAgLib::vtObjectMeter_c
{
public:
  static ObjectType objectType() { return VT_OBJECT_TYPE_METER; }


  void setWidth(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setNeedleColour(Colour newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setNeedleColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setBorderColour(Colour newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setBorderColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setArcAndTickColour(Colour newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setArcAndTickColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setOptions(IsoAgLib::iVtObjectMeterOptions newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setNumberOfTicks(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setNumberOfTicks (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setStartAngle(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setStartAngle (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setEndAngle(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setEndAngle (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setMin(uint16_t newMin, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setMin (newMin, b_updateObject, b_enableReplaceOfCmd);
  }

  void setMax(uint16_t newMax, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setMax (newMax, b_updateObject, b_enableReplaceOfCmd);
  }

  void setValue(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) {
    vtObjectMeter_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setVariableReference(iVtObjectNumberVariable_c* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectMeter_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
  }
  
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
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

  IsoAgLib::iVtObjectMeterOptions updateOptions(bool b_SendRequest=false) {
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

  iVtObjectNumberVariable_c* updateVariableReference(bool b_SendRequest=false) {
    return vtObjectMeter_c::updateVariableReference(b_SendRequest);
  }

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false) {
    return vtObjectMeter_c::getValue(b_SendRequest);
  }
  */
#endif

  virtual ObjectType getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_meter

#endif //IVTOBJECTMETER_C_H
