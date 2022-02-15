/*
  ivtobjectlinearbargraph_c.h

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
#ifndef IVTOBJECTLINEARBARGRAPH_C_H
#define IVTOBJECTLINEARBARGRAPH_C_H

#include "impl/vtobjectlinearbargraph_c.h"


#ifdef CONFIG_USE_VTOBJECT_linearbargraph

namespace IsoAgLib {

class iVtObjectLinearBarGraph_c : public __IsoAgLib::vtObjectLinearBarGraph_c
{
public:
  static uint16_t objectType();

  iVtObjectLinearBarGraph_c(
			iVtClientObjectPool_c* pool,
		    ObjectID ID = autoID,
		    uint16_t width = 30,
			uint16_t height = 100,
			Colour colour = NAVY,
			Colour targetLineColour = YELLOW,
			uint8_t options = 0,
			uint8_t numberOfTicks = 4,
			uint16_t minValue = 0,
			uint16_t maxValue = 100,
			iVtObjectNumberVariable_c *variableReference = nullptr,
			uint16_t value = 40,
			iVtObjectNumberVariable_c *targetValueVariableReference = nullptr,
			uint16_t targetValue = 50);

  void setValue(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setTargetLineColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setNumberOfTicks(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setMinValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setMaxValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setVariableReference(iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setTargetValueVariableReference(iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setTargetValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectLinearBarGraph_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  Colour updateColour(bool b_SendRequest=false);
  Colour updateTargetLineColour(bool b_SendRequest=false);
  uint8_t updateOptions(bool b_SendRequest=false);
  uint8_t updateNumberOfTicks(bool b_SendRequest=false);
  uint16_t updateMinValue(bool b_SendRequest=false);
  uint16_t updateMaxValue(bool b_SendRequest=false);
  iVtObjectNumberVariable_c* updateVariableReference(bool b_SendRequest=false);
  iVtObjectNumberVariable_c* updateTargetValueVariableReference(bool b_SendRequest=false);
  uint16_t updateTargetValue(bool b_SendRequest=false);

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::getValue(b_SendRequest);
  }
  */
#endif

  virtual uint16_t getObjectType() const;
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_linearbargraph

#endif //IVTOBJECTLINEARBARGRAPH_C_H
