/*
  ivtobjectinputnumber_c.h

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
#ifndef IVTOBJECTINPUTNUMBER_C_H
#define IVTOBJECTINPUTNUMBER_C_H

#include "impl/vtobjectinputnumber_c.h"


#ifdef CONFIG_USE_VTOBJECT_inputnumber

namespace IsoAgLib {

class iVtObjectInputNumber_c : public __IsoAgLib::vtObjectInputNumber_c
{
private:
	iVtObjectInputNumber_c() = delete;

public:


	iVtObjectInputNumber_c(
			    iVtClientObjectPool_c* pool,
			    ObjectID ID = autoID,
				uint16_t width = 100,
				uint16_t height = 25,
				Colour backgroundColour = WHITE,
				iVtObjectFontAttributes_c *fontAttributes = nullptr,
				uint8_t options = 0,
				iVtObjectNumberVariable_c *variableReference = nullptr,
				uint32_t value = 9,
				uint32_t minValue = 0,
				uint32_t maxValue = 100,
				int32_t offset = 0,
				float scale = 1.0,
				uint8_t numberOfDecimals = 1,
				uint8_t format = 0,
				uint8_t horizontalJustification = 0,
				uint8_t secondOptionsByte = 0);

  ~iVtObjectInputNumber_c() override;

  static uint16_t objectType() { return VT_OBJECT_TYPE_INPUT_NUMBER; }

  void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputNumber_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setBackgroundColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setFontAttributes(iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setFontAttributes (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setVariableReference(iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setMinValue(uint32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setMinValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setMaxValue(uint32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setMaxValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setOffset(int32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setOffset (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setScale(float newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setScale (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setNumberOfDecimals(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setNumberOfDecimals (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setFormat(bool newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setFormat (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setHorizontalJustification (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setSecondOptionsByte(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputNumber_c::setSecondOptionsByte (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false){
    vtObjectInputNumber_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  bool enable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false); //TODO
  bool disable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false); //TODO

  bool getEnabled(); //TODO

  bool select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputNumber_c::updateObjectType(); }
  */
  
  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateWidth(b_SendRequest);
  }
  
  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateHeight(b_SendRequest);
  }

  Colour updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateBackgroundColour(b_SendRequest);
  }

  iVtObjectFontAttributes_c* updateFontAttributes(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateFontAttributes(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateOptions(b_SendRequest);
  }

  iVtObjectNumberVariable_c* updateVariableReference(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateVariableReference(b_SendRequest);
  }

  uint32_t updateMinValue(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateMinValue(b_SendRequest);
  }

  uint32_t updateMaxValue(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateMaxValue(b_SendRequest);
  }

  int32_t updateOffset(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateOffset(b_SendRequest);
  }

  float updateScale(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateScale(b_SendRequest);
  }

  uint8_t updateNumberOfDecimals(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateNumberOfDecimals(b_SendRequest);
  }

  uint8_t updateFormat(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateFormat(b_SendRequest);
  }

  uint8_t updateJustification(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::updateJustification(b_SendRequest);
  }

  /** these attributes are in parentheses in the spec, so commented out here
  uint32_t updateValue(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::getValue(b_SendRequest);
  }

  uint8_t update2ndOptionsByte(bool b_SendRequest=false) {
    return vtObjectInputNumber_c::update2ndOptionsByte(b_SendRequest);
  }
  */
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_inputnumber

#endif //IVTOBJECTINPUTNUMBER_C_H
