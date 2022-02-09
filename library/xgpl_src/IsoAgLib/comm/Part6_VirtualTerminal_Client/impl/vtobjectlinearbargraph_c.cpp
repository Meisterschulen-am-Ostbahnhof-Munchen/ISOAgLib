/*
  vtobjectlinearbargraph_c.cpp

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

#include "vtobjectlinearbargraph_c.h"

#ifdef CONFIG_USE_VTOBJECT_linearbargraph
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


struct vtObjectLinearBarGraph_c::iVtObjectLinearBarGraph_s : iVtObjectwMacro_s {
  uint16_t width;
  uint16_t height;
  IsoAgLib::Colour colour;
  IsoAgLib::Colour targetLineColour;
  uint8_t options;
  uint8_t numberOfTicks;
  uint16_t minValue;
  uint16_t maxValue;
  iVtObject_c* variableReference;
  uint16_t value;
  iVtObject_c* targetValueVariableReference;
  uint16_t targetValue;
  iVtObjectLinearBarGraph_s(
		    IsoAgLib::ObjectID ID,
  		    uint16_t width,
			uint16_t height,
			IsoAgLib::Colour colour,
			IsoAgLib::Colour targetLineColour,
			uint8_t options,
			uint8_t numberOfTicks,
			uint16_t minValue,
			uint16_t maxValue,
			iVtObject_c *variableReference,
			uint16_t value,
			iVtObject_c *targetValueVariableReference,
			uint16_t targetValue)
  : iVtObject_s(ID)
  , iVtObjectwMacro_s(ID)
  , width(width)
  , height(height)
  , colour(colour)
  , targetLineColour(targetLineColour)
  , options(options)
  , numberOfTicks(numberOfTicks)
  , minValue(minValue)
  , maxValue(maxValue)
  , variableReference(variableReference)
  , value(value)
  , targetValueVariableReference(targetValueVariableReference)
  , targetValue(targetValue)
  {}
};



int16_t
vtObjectLinearBarGraph_c::stream(uint8_t* destMemory,
                                 uint16_t maxBytes,
                                 objRange_t sourceOffset)
{
    uint16_t curBytes=0; /* current bytes written */;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObject_a->width;
    uint32_t height = (uint32_t)vtObject_a->height;
    MACRO_scaleSizeI32(width, height);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 18; // Object Type = Linear Bar Graph
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->colour, this, IsoAgLib::AColour);
      destMemory [8] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->targetLineColour, this, IsoAgLib::TargetLineColour);
      destMemory [9] = vtObject_a->options;
      destMemory [10] = vtObject_a->numberOfTicks;
      destMemory [11] = vtObject_a->minValue & 0xFF;
      destMemory [12] = vtObject_a->minValue >> 8;
      destMemory [13] = vtObject_a->maxValue & 0xFF;
      destMemory [14] = vtObject_a->maxValue >> 8;
      if (vtObject_a->variableReference != NULL) {
        destMemory [15] = vtObject_a->variableReference->getID() & 0xFF;
        destMemory [16] = vtObject_a->variableReference->getID() >> 8;
      } else {
        destMemory [15] = 0xFF;
        destMemory [16] = 0xFF;
      }
      destMemory [17] = vtObject_a->value & 0xFF;
      destMemory [18] = vtObject_a->value >> 8;
      if (vtObject_a->targetValueVariableReference != NULL) {
        destMemory [19] = vtObject_a->targetValueVariableReference->getID() & 0xFF;
        destMemory [20] = vtObject_a->targetValueVariableReference->getID() >> 8;
      } else {
        destMemory [19] = 0xFF;
        destMemory [20] = 0xFF;
      }
      destMemory [21] = vtObject_a->targetValue & 0xFF;
      destMemory [22] = vtObject_a->targetValue >> 8;
      destMemory [23] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 24;
      curBytes += 24;
    }

    MACRO_streamEventMacro(24);
    return curBytes;
}


IsoAgLib::ObjectID vtObjectLinearBarGraph_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


vtObjectLinearBarGraph_c::vtObjectLinearBarGraph_c(
		int ai_multitonInst,
		IsoAgLib::ObjectID ID,
		uint16_t width,
		uint16_t height,
		IsoAgLib::Colour colour,
		IsoAgLib::Colour targetLineColour,
		uint8_t options,
		uint8_t numberOfTicks,
		uint16_t minValue,
		uint16_t maxValue,
		iVtObject_c *variableReference,
		uint16_t value,
		iVtObject_c *targetValueVariableReference,
		uint16_t targetValue)
	:vtObjectLinearBarGraph_c(
			new iVtObjectLinearBarGraph_s(
		    		ID,
		    		width,
					height,
					colour,
		    		targetLineColour,
					options,
					numberOfTicks,
		    		minValue,
					maxValue,
					variableReference,
		    		value,
					targetValueVariableReference,
		    		targetValue),
			ai_multitonInst)
{


}

vtObjectLinearBarGraph_c::vtObjectLinearBarGraph_c(iVtObjectLinearBarGraph_s* vtObjectLinearBarGraphSROM , int ai_multitonInst)
            : vtObject_c(ai_multitonInst)
			, vtObject_a(vtObjectLinearBarGraphSROM)
    {}



vtObjectLinearBarGraph_c::iVtObjectLinearBarGraph_s *vtObjectLinearBarGraph_c::get_vtObjectLinearBarGraph_a() {
	return vtObject_a;
}


uint32_t
vtObjectLinearBarGraph_c::fitTerminal() const
{
  return 24+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectLinearBarGraph_c::setValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (get_vtObjectLinearBarGraph_a()->variableReference == NULL) {
	    if (b_updateObject)
	    	vtObject_a->value = newValue;

    __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, 0x00, 0x00, b_enableReplaceOfCmd);
  }
}

void
vtObjectLinearBarGraph_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	  vtObject_a->width = newWidth;
	  vtObject_a->height = newHeight;
  }

  scaleSize( newWidth, newHeight );

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectLinearBarGraph_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), width), sizeof(iVtObjectLinearBarGraph_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), width), sizeof(iVtObjectLinearBarGraph_s));
}

uint16_t
vtObjectLinearBarGraph_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), height), sizeof(iVtObjectLinearBarGraph_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), height), sizeof(iVtObjectLinearBarGraph_s));
}

uint8_t
vtObjectLinearBarGraph_c::updateColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), colour), sizeof(iVtObjectLinearBarGraph_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), colour), sizeof(iVtObjectLinearBarGraph_s));
}

uint8_t
vtObjectLinearBarGraph_c::updateTargetLineColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetLineColour), sizeof(iVtObjectLinearBarGraph_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetLineColour), sizeof(iVtObjectLinearBarGraph_s));
}

uint8_t
vtObjectLinearBarGraph_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), options), sizeof(iVtObjectLinearBarGraph_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), options), sizeof(iVtObjectLinearBarGraph_s));
}

uint8_t
vtObjectLinearBarGraph_c::updateNumberOfTicks(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), numberOfTicks), sizeof(iVtObjectLinearBarGraph_s), 6);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), numberOfTicks), sizeof(iVtObjectLinearBarGraph_s));
}

uint16_t
vtObjectLinearBarGraph_c::updateMinValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), minValue), sizeof(iVtObjectLinearBarGraph_s), 7);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), minValue), sizeof(iVtObjectLinearBarGraph_s));
}

uint16_t
vtObjectLinearBarGraph_c::updateMaxValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), maxValue), sizeof(iVtObjectLinearBarGraph_s), 8);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), maxValue), sizeof(iVtObjectLinearBarGraph_s));
}

uint16_t
vtObjectLinearBarGraph_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), variableReference), sizeof(iVtObjectLinearBarGraph_s), 9);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), variableReference), sizeof(iVtObjectLinearBarGraph_s));
}

uint16_t
vtObjectLinearBarGraph_c::updateTargetValueVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetValueVariableReference), sizeof(iVtObjectLinearBarGraph_s), 10);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetValueVariableReference), sizeof(iVtObjectLinearBarGraph_s));
}

uint16_t
vtObjectLinearBarGraph_c::updateTargetValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetValue), sizeof(iVtObjectLinearBarGraph_s), 11);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetValue), sizeof(iVtObjectLinearBarGraph_s));
}

/** that attribute is in parentheses in the spec, so commented out here
uint16_t
vtObjectLinearBarGraph_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), value), sizeof(iVtObjectLinearBarGraph_s), 12);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), value), sizeof(iVtObjectLinearBarGraph_s));
}
*/

void
vtObjectLinearBarGraph_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), width), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), height), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), colour), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetLineColour), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), options), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue8(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), numberOfTicks), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 7: saveValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), minValue), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 8: saveValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), maxValue), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 9: saveValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), variableReference), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 10: saveValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetValueVariableReference), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 11: saveValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetValue), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    /** that attribute is in parentheses in the spec, so commented out here
    case 12: saveValue16(MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetValue), sizeof(iVtObjectLinearBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif




    void vtObjectLinearBarGraph_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), width) : 0, sizeof(iVtObjectLinearBarGraph_s), 1, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), height) : 0, sizeof(iVtObjectLinearBarGraph_s), 2, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), colour) : 0, sizeof(iVtObjectLinearBarGraph_s), 3, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::AColour), b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setTargetLineColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetLineColour) : 0, sizeof(iVtObjectLinearBarGraph_s), 4, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::TargetLineColour), b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), options) : 0, sizeof(iVtObjectLinearBarGraph_s), 5, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setNumberOfTicks(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), numberOfTicks) : 0, sizeof(iVtObjectLinearBarGraph_s), 6, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setMinValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), minValue) : 0, sizeof(iVtObjectLinearBarGraph_s), 7, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setMaxValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), maxValue) : 0, sizeof(iVtObjectLinearBarGraph_s), 8, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setVariableReference(IsoAgLib::iVtObject_c *newValue, bool b_updateObject,
                                                        bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), variableReference) : 0, sizeof(iVtObjectLinearBarGraph_s), 9, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setTargetValueVariableReference(IsoAgLib::iVtObject_c *newValue, bool b_updateObject,
                                                                   bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetValueVariableReference) : 0, sizeof(iVtObjectLinearBarGraph_s), 10, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setTargetValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetValue) : 0, sizeof(iVtObjectLinearBarGraph_s), 11, newValue, b_enableReplaceOfCmd);
    }



} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_linearbargraph
