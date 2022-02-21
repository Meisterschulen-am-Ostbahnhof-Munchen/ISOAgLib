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


  virtual ~iVtObjectMeter_c() override;

  iVtObjectMeter_c() = delete;

  explicit iVtObjectMeter_c(
			iVtClientObjectPool_c* pool,
	  		ObjectID ID = autoID,
			uint16_t width = 80,
			Colour needleColour = BLACK,
			Colour borderColour = BLACK,
			Colour arcAndTickColour = BLACK,
			iVtObjectMeterOptions options = iVtObjectMeterOptions(allDrawMeter),
			uint8_t numberOfTicks = 12,
			uint8_t startAngle = 0,
			uint8_t endAngle = 90,
			uint16_t minValue = 0,
			uint16_t maxValue = 500,
			iVtObjectNumberVariable_c *variableReference = nullptr,
			uint16_t value = 0);

  void setWidth(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setNeedleColour(Colour newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setBorderColour(Colour newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setArcAndTickColour(Colour newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setOptions(iVtObjectMeterOptions newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setNumberOfTicks(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setStartAngle(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setEndAngle(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setMin(uint16_t newMin, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setMax(uint16_t newMax, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setValue(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) override;
  void setVariableReference(iVtObjectNumberVariable_c* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectMeter_c::updateObjectType(); }
  */
  
  uint16_t updateWidth(bool b_SendRequest=false) override;
  uint8_t updateNeedleColour(bool b_SendRequest=false) override;
  uint8_t updateBorderColour(bool b_SendRequest=false) override;
  uint8_t updateArcAndTickColour(bool b_SendRequest=false) override;
  iVtObjectMeterOptions updateOptions(bool b_SendRequest=false) override;
  uint8_t updateNumberOfTicks(bool b_SendRequest=false) override;
  uint8_t updateStartAngle(bool b_SendRequest=false) override;
  uint8_t updateEndAngle(bool b_SendRequest=false) override;
  uint16_t updateMinValue(bool b_SendRequest=false) override;
  uint16_t updateMaxValue(bool b_SendRequest=false) override;
  iVtObjectNumberVariable_c* updateVariableReference(bool b_SendRequest=false) override;

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false) {
    return vtObjectMeter_c::getValue(b_SendRequest);
  }
  */
#endif

  ObjectType getObjectType() const override;
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_meter

#endif //IVTOBJECTMETER_C_H
