/*
  ivtobjectarchedbargraph_c.h

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
#ifndef IVTOBJECTARCHEDBARGRAPH_C_H
#define IVTOBJECTARCHEDBARGRAPH_C_H

#include "impl/vtobjectarchedbargraph_c.h"



namespace IsoAgLib {

class iVtObjectArchedBarGraph_c : public __IsoAgLib::vtObjectArchedBarGraph_c
{
public:
  static uint16_t objectType();

  iVtObjectArchedBarGraph_c(
			iVtClientObjectPool_c* pool,
			ObjectID ID = autoID,
			uint16_t width = 100,
			uint16_t height = 100,
			Colour colour = RED,
			Colour targetLineColour = YELLOW,
			uint8_t options = 0,
			uint8_t startAngle = 0,
			uint8_t endAngle = 180,
			uint16_t barGraphWidth = 15,
			uint16_t minValue = 0,
			uint16_t maxValue = 100,
			iVtObject_c *variableReference = nullptr,
			uint16_t value = 20,
			iVtObject_c *targetValueVariableReference = nullptr,
			uint16_t targetValue = 50);


  ~iVtObjectArchedBarGraph_c() override = default;
  void setValue(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);
  void setWidth(uint16_t newWidth, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setHeight(uint16_t newHeight, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setColour(Colour newColour, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setTargetLineColour(Colour newTargetLineColour, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setOptions(uint8_t newOptions, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setStartAngle(uint8_t newStartAngle, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setEndAngle(uint8_t newEndAngle, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setBarGraphWidth(uint16_t newBarGraphWidth, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setMinValue(uint16_t newMin, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setMaxValue(uint16_t newMax, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setVariableReference(iVtObject_c* newVariableReference, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setTargetValueVariableReference(iVtObject_c* newTargetValueVariableReference, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setTargetValue(uint16_t newTargetValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectArchedBarGraph_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  uint8_t updateColour(bool b_SendRequest=false);
  uint8_t updateTargetLineColour(bool b_SendRequest=false);
  uint8_t updateOptions(bool b_SendRequest=false);
  uint8_t updateStartAngle(bool b_SendRequest=false);
  uint8_t updateEndAngle(bool b_SendRequest=false);
  uint16_t updateBarGraphWidth(bool b_SendRequest=false);
  uint16_t updateMinValue(bool b_SendRequest=false);
  uint16_t updateMaxValue(bool b_SendRequest=false);
  uint16_t updateVariableReference(bool b_SendRequest=false);
  uint16_t updateTargetValueVariableReference(bool b_SendRequest=false);
  uint16_t updateTargetValue(bool b_SendRequest=false);

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false) {
    return vtObjectArchedBarGraph_c::getValue(b_SendRequest);
  }
  */
#endif

  virtual uint16_t getObjectType() const;
};

} // IsoAgLib

#endif


