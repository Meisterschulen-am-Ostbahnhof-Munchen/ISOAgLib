/*
  vtobjectarchedbargraph_c.h

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
#ifndef VTOBJECTARCHEDBARGRAPH_C_H
#define VTOBJECTARCHEDBARGRAPH_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_archedbargraph

//#include <memory> // PImpl
#include "vtobject_c.h"
#include "vtclientconnection_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

class vtObjectArchedBarGraph_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectArchedBarGraph_s;

	// Pointer to the internal implementation
	iVtObjectArchedBarGraph_s* vtObject_a;
	//TODO
	//std::unique_ptr<iVtObjectArchedBarGraph_s> vtObject_a;


public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  vtObjectArchedBarGraph_c(
		    int ai_multitonInst,
			IsoAgLib::ObjectID ID,
			uint16_t width,
			uint16_t height,
			IsoAgLib::Colour colour,
			IsoAgLib::Colour targetLineColour,
			uint8_t options,
			uint8_t startAngle,
			uint8_t endAngle,
			uint16_t barGraphWidth,
			uint16_t minValue,
			uint16_t maxValue,
			iVtObject_c *variableReference,
			uint16_t value,
			iVtObject_c *targetValueVariableReference,
			uint16_t targetValue);

  vtObjectArchedBarGraph_c(iVtObjectArchedBarGraph_s* vtObjectArchedBarGraphSROM , int ai_multitonInst);


  iVtObjectArchedBarGraph_s* get_vtObjectArchedBarGraph_a();
  ~vtObjectArchedBarGraph_c() override = default;
  uint32_t fitTerminal() const;
    virtual void setValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true);
    virtual void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setColour(IsoAgLib::Colour newColour, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setTargetLineColour(IsoAgLib::Colour newTargetLineColour, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setOptions(uint8_t newOptions, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setStartAngle(uint8_t newStartAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setEndAngle(uint8_t newEndAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setBarGraphWidth(uint16_t newBarGraphWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setMinValue(uint16_t newMin, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
    virtual void setMaxValue(uint16_t newMax, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
    virtual void setVariableReference(IsoAgLib::iVtObject_c* newVariableReference, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setTargetValueVariableReference(IsoAgLib::iVtObject_c* newTargetValueVariableReference, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setTargetValue(uint16_t newTargetValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 19; }
  */

    virtual uint16_t updateWidth(bool b_SendRequest=false);
    virtual uint16_t updateHeight(bool b_SendRequest=false);
    virtual uint8_t updateColour(bool b_SendRequest=false);
    virtual uint8_t updateTargetLineColour(bool b_SendRequest=false);
    virtual uint8_t updateOptions(bool b_SendRequest=false);
    virtual uint8_t updateStartAngle(bool b_SendRequest=false);
    virtual uint8_t updateEndAngle(bool b_SendRequest=false);
    virtual uint16_t updateBarGraphWidth(bool b_SendRequest=false);
    virtual uint16_t updateMinValue(bool b_SendRequest=false);
    virtual uint16_t updateMaxValue(bool b_SendRequest=false);
    virtual uint16_t updateVariableReference(bool b_SendRequest=false);
    virtual uint16_t updateTargetValueVariableReference(bool b_SendRequest=false);
    virtual uint16_t updateTargetValue(bool b_SendRequest=false);

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif

#endif
