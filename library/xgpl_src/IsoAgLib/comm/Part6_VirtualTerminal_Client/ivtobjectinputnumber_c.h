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
public:
	iVtObjectInputNumber_c() = delete;


	explicit iVtObjectInputNumber_c(
			    iVtClientObjectPool_c* pool,
			    ObjectID ID = autoID,
				uint16_t width = 100,
				uint16_t height = 25,
				Colour backgroundColour = WHITE,
				iVtObjectFontAttributes_c *fontAttributes = nullptr,
				iVtObjectInputNumberOptions options = iVtObjectInputNumberOptions(),
				iVtObjectNumberVariable_c *variableReference = nullptr,
				uint32_t value = 9,
				uint32_t minValue = 0,
				uint32_t maxValue = 100,
				int32_t offset = 0,
				float scale = 1.0,
				uint8_t numberOfDecimals = 1,
				uint8_t format = 0,
				uint8_t horizontalJustification = 0,
				iVtObjectInputNumberOptions2 secondOptionsByte = iVtObjectInputNumberOptions2(enabled));

  ~iVtObjectInputNumber_c() override;
  static ObjectType objectType();
  void setValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setBackgroundColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setFontAttributes(iVtObjectFontAttributes_c* newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setOptions(iVtObjectInputNumberOptions newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setVariableReference(iVtObjectNumberVariable_c* newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setMinValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setMaxValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setOffset(int32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setScale(float newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setNumberOfDecimals(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setFormat(bool newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setHorizontalJustification(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setSecondOptionsByte(iVtObjectInputNumberOptions2 newValue, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) override;
  bool enable(bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  bool disable(bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  Enabled getEnabled() override;
  bool select(uint8_t selectOrActivate) override;
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputNumber_c::updateObjectType(); }
  */
  
  uint16_t updateWidth(bool b_SendRequest) override;
  uint16_t updateHeight(bool b_SendRequest) override;
  Colour updateBackgroundColour(bool b_SendRequest) override;
  iVtObjectFontAttributes_c* updateFontAttributes(bool b_SendRequest) override;
  iVtObjectInputNumberOptions updateOptions(bool b_SendRequest) override;
  iVtObjectNumberVariable_c* updateVariableReference(bool b_SendRequest) override;
  uint32_t updateMinValue(bool b_SendRequest) override;
  uint32_t updateMaxValue(bool b_SendRequest) override;
  int32_t updateOffset(bool b_SendRequest) override;
  float updateScale(bool b_SendRequest) override;
  uint8_t updateNumberOfDecimals(bool b_SendRequest) override;
  uint8_t updateFormat(bool b_SendRequest) override;
  uint8_t updateJustification(bool b_SendRequest) override;

  /** these attributes are in parentheses in the spec, so commented out here
  uint32_t updateValue(bool b_SendRequest) {
    return vtObjectInputNumber_c::getValue(b_SendRequest);
  }

  uint8_t update2ndOptionsByte(bool b_SendRequest) {
    return vtObjectInputNumber_c::update2ndOptionsByte(b_SendRequest);
  }
  */
#endif

  ObjectType getObjectType() const override;
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_inputnumber

#endif //IVTOBJECTINPUTNUMBER_C_H
