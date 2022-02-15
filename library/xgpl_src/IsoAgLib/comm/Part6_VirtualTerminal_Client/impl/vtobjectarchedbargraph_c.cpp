/*
  vtobjectarchedbargraph_c.cpp

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

#include "vtobjectarchedbargraph_c.h"

#ifdef CONFIG_USE_VTOBJECT_archedbargraph

#include "../ivtobjectnumbervariable_c.h"
#include "../ivtobject_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {



enum vtObjectArchedBarGraph_c::AttributeID:uint8_t
{
	Width                        = 1,
	Height                       = 2,
	Colour                       = 3,
    TargetLineColour             = 4,
    Options                      = 5,
    NumberOfTicks                = 6,
    MinValue                     = 7,
	MaxValue                     = 8,
	VariableReference            = 9,
	TargetValueVariableReference = 10,
	TargetValue                  = 11,
};


struct vtObjectArchedBarGraph_c::iVtObjectArchedBarGraph_s : iVtObjectwMacro_s {
  uint16_t width;
  uint16_t height;
  IsoAgLib::Colour colour;
  IsoAgLib::Colour targetLineColour;
  uint8_t options;
  uint8_t startAngle;
  uint8_t endAngle;
  uint16_t barGraphWidth;
  uint16_t minValue;
  uint16_t maxValue;
  iVtObject_c* variableReference;
  uint16_t value;
  iVtObject_c* targetValueVariableReference;
  uint16_t targetValue;
  explicit iVtObjectArchedBarGraph_s(
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
};

    vtObjectArchedBarGraph_c::iVtObjectArchedBarGraph_s::iVtObjectArchedBarGraph_s(
    	   IsoAgLib::ObjectID ID,
		   uint16_t width, uint16_t height,
		   IsoAgLib::Colour colour,
		   IsoAgLib::Colour targetLineColour,
		   uint8_t options, uint8_t startAngle,
		   uint8_t endAngle,
		   uint16_t barGraphWidth,
		   uint16_t minValue, uint16_t maxValue,
		   IsoAgLib::iVtObject_c *variableReference,
		   uint16_t value,
		   IsoAgLib::iVtObject_c *targetValueVariableReference,
		   uint16_t targetValue)
			: iVtObject_s(ID)
			, iVtObjectwMacro_s(ID)
            , width(width)
            , height(height)
            , colour(colour)
            , targetLineColour(targetLineColour)
            , options(options)
            , startAngle(startAngle)
            , endAngle(endAngle)
            , barGraphWidth(barGraphWidth)
            , minValue(minValue)
            , maxValue(maxValue)
            , variableReference(variableReference)
            , value(value)
            , targetValueVariableReference(targetValueVariableReference)
            , targetValue(targetValue)
    {}















int16_t
vtObjectArchedBarGraph_c::stream(uint8_t* destMemory,
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
      destMemory [2] = 19; // Object Type = Arched Bar Graph
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->colour, this, IsoAgLib::AColour);
      destMemory [8] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->targetLineColour, this, IsoAgLib::TargetLineColour);
      destMemory [9] = vtObject_a->options;
      destMemory [10] = vtObject_a->startAngle;
      destMemory [11] = vtObject_a->endAngle;
      if ((s_properties.flags & FLAG_ORIGIN_SKM) || p_parentButtonObject)
      {
        destMemory [12] = (((uint32_t) vtObject_a->barGraphWidth*factorM)/factorD) & 0xFF;
        destMemory [13] = (((uint32_t) vtObject_a->barGraphWidth*factorM)/factorD) >> 8;
      }
      else
      {
        destMemory [12] = (((uint32_t) vtObject_a->barGraphWidth*vtDimension)/opDimension) & 0xFF;
        destMemory [13] = (((uint32_t) vtObject_a->barGraphWidth*vtDimension)/opDimension) >> 8;
      }
      if ( (vtObject_a->barGraphWidth > 0) &&
           (destMemory [12] == 0) && (destMemory [13] == 0))
      { // if the arch had a linewidth > 0, then don't allow downscaling to let it become 0!
        destMemory [12] = 0x01;
      //destMemory [13] = 0x00; // is already set to 0 as checked in the if-statement!
      }
      destMemory [14] = vtObject_a->minValue & 0xFF;
      destMemory [15] = vtObject_a->minValue >> 8;
      destMemory [16] = vtObject_a->maxValue & 0xFF;
      destMemory [17] = vtObject_a->maxValue >> 8;
      if (vtObject_a->variableReference != NULL) {
        destMemory [18] = vtObject_a->variableReference->getID() & 0xFF;
        destMemory [19] = vtObject_a->variableReference->getID() >> 8;
      } else {
        destMemory [18] = 0xFF;
        destMemory [19] = 0xFF;
      }
      destMemory [20] = vtObject_a->value & 0xFF;
      destMemory [21] = vtObject_a->value >> 8;
      if (vtObject_a->targetValueVariableReference != NULL) {
        destMemory [22] = vtObject_a->targetValueVariableReference->getID() & 0xFF;
        destMemory [23] = vtObject_a->targetValueVariableReference->getID() >> 8;
      } else {
        destMemory [22] = 0xFF;
        destMemory [23] = 0xFF;
      }
      destMemory [24] = vtObject_a->targetValue & 0xFF;
      destMemory [25] = vtObject_a->targetValue >> 8;
      destMemory [26] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 27;
      curBytes += 27;
    }

    MACRO_streamEventMacro(27);
    return curBytes;
}

IsoAgLib::ObjectID vtObjectArchedBarGraph_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

    vtObjectArchedBarGraph_c::vtObjectArchedBarGraph_c(
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
            uint16_t targetValue)
            :vtObjectArchedBarGraph_c(
            new iVtObjectArchedBarGraph_s(
                    ID,
                    width,
                    height,
                    colour,
                    targetLineColour,
                    options,
                    startAngle,
                    endAngle,
                    barGraphWidth,
                    minValue,
                    maxValue,
                    variableReference,
                    value,
                    targetValueVariableReference,
                    targetValue),
            ai_multitonInst)
    {
    }


    vtObjectArchedBarGraph_c::vtObjectArchedBarGraph_c(
            vtObjectArchedBarGraph_c::iVtObjectArchedBarGraph_s *vtObjectArchedBarGraphSROM, int ai_multitonInst)
            : vtObject_c(ai_multitonInst)
            , vtObject_a(vtObjectArchedBarGraphSROM)
    {}


uint32_t
vtObjectArchedBarGraph_c::fitTerminal() const
{
  return 27+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectArchedBarGraph_c::setValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (get_vtObjectArchedBarGraph_a()->variableReference == NULL) {
    if (b_updateObject)
    	vtObject_a->value = newValue;

    __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, 0x00, 0x00, b_enableReplaceOfCmd);
  }
}



void
vtObjectArchedBarGraph_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
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
vtObjectArchedBarGraph_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), width), sizeof(iVtObjectArchedBarGraph_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), width), sizeof(iVtObjectArchedBarGraph_s));
}

uint16_t
vtObjectArchedBarGraph_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), height), sizeof(iVtObjectArchedBarGraph_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), height), sizeof(iVtObjectArchedBarGraph_s));
}

uint8_t
vtObjectArchedBarGraph_c::updateColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), colour), sizeof(iVtObjectArchedBarGraph_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), colour), sizeof(iVtObjectArchedBarGraph_s));
}

uint8_t
vtObjectArchedBarGraph_c::updateTargetLineColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetLineColour), sizeof(iVtObjectArchedBarGraph_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetLineColour), sizeof(iVtObjectArchedBarGraph_s));
}

uint8_t
vtObjectArchedBarGraph_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), options), sizeof(iVtObjectArchedBarGraph_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), options), sizeof(iVtObjectArchedBarGraph_s));
}

uint8_t
vtObjectArchedBarGraph_c::updateStartAngle(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), startAngle), sizeof(iVtObjectArchedBarGraph_s), 6);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), startAngle), sizeof(iVtObjectArchedBarGraph_s));
}

uint8_t
vtObjectArchedBarGraph_c::updateEndAngle(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), endAngle), sizeof(iVtObjectArchedBarGraph_s), 7);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), endAngle), sizeof(iVtObjectArchedBarGraph_s));
}

uint16_t
vtObjectArchedBarGraph_c::updateBarGraphWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), barGraphWidth), sizeof(iVtObjectArchedBarGraph_s), 8);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), barGraphWidth), sizeof(iVtObjectArchedBarGraph_s));
}

uint16_t
vtObjectArchedBarGraph_c::updateMinValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), minValue), sizeof(iVtObjectArchedBarGraph_s), 9);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), minValue), sizeof(iVtObjectArchedBarGraph_s));
}

uint16_t
vtObjectArchedBarGraph_c::updateMaxValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), maxValue), sizeof(iVtObjectArchedBarGraph_s), 10);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), maxValue), sizeof(iVtObjectArchedBarGraph_s));
}

uint16_t
vtObjectArchedBarGraph_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), variableReference), sizeof(iVtObjectArchedBarGraph_s), 11);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), variableReference), sizeof(iVtObjectArchedBarGraph_s));
}

uint16_t
vtObjectArchedBarGraph_c::updateTargetValueVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValueVariableReference), sizeof(iVtObjectArchedBarGraph_s), 12);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValueVariableReference), sizeof(iVtObjectArchedBarGraph_s));
}

uint16_t
vtObjectArchedBarGraph_c::updateTargetValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValue), sizeof(iVtObjectArchedBarGraph_s), 13);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValue), sizeof(iVtObjectArchedBarGraph_s));
}

/** that attribute is in parentheses in the spec, so commented out here
uint16_t
vtObjectArchedBarGraph_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), value), sizeof(iVtObjectArchedBarGraph_s), 14);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), value), sizeof(iVtObjectArchedBarGraph_s));
}
*/

void
vtObjectArchedBarGraph_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), width), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), height), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), colour), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetLineColour), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), options), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue8(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), startAngle), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 7: saveValue8(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), endAngle), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 8: saveValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), barGraphWidth), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 9: saveValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), minValue), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 10: saveValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), maxValue), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 11: saveValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), variableReference), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 12: saveValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValueVariableReference), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 13: saveValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValue), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    /** that attribute is in parentheses in the spec, so commented out here
    case 14: saveValue16(MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), value), sizeof(iVtObjectArchedBarGraph_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    */
    default: break;
  }
}

#endif



    void vtObjectArchedBarGraph_c::setWidth(uint16_t newWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), width) : 0, sizeof(iVtObjectArchedBarGraph_s), 1 /* "Width" */, newWidth, b_enableReplaceOfCmd);
    }

    void vtObjectArchedBarGraph_c::setHeight(uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), height) : 0, sizeof(iVtObjectArchedBarGraph_s), 2 /* "Height" */, newHeight, b_enableReplaceOfCmd);
    }

    void vtObjectArchedBarGraph_c::setColour(IsoAgLib::Colour newColour, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), colour) : 0, sizeof(iVtObjectArchedBarGraph_s), 3 /* "Colour" */, newColour, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newColour, this, IsoAgLib::AColour), b_enableReplaceOfCmd);
    }

    void vtObjectArchedBarGraph_c::setTargetLineColour(IsoAgLib::Colour newTargetLineColour, bool b_updateObject,
                                                       bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetLineColour) : 0, sizeof(iVtObjectArchedBarGraph_s), 4 /* "Target Line Colour" */, newTargetLineColour, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newTargetLineColour, this, IsoAgLib::TargetLineColour), b_enableReplaceOfCmd);
    }

    void vtObjectArchedBarGraph_c::setOptions(uint8_t newOptions, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), options) : 0, sizeof(iVtObjectArchedBarGraph_s), 5 /* "Options" */, newOptions, newOptions, b_enableReplaceOfCmd);
    }

    void vtObjectArchedBarGraph_c::setStartAngle(uint8_t newStartAngle, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), startAngle) : 0, sizeof(iVtObjectArchedBarGraph_s), 6 /* "Start Angle" */, newStartAngle, newStartAngle, b_enableReplaceOfCmd);
    }

    void vtObjectArchedBarGraph_c::setEndAngle(uint8_t newEndAngle, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), endAngle) : 0, sizeof(iVtObjectArchedBarGraph_s), 7 /* "End Angle" */, newEndAngle, newEndAngle, b_enableReplaceOfCmd);
    }

    void vtObjectArchedBarGraph_c::setBarGraphWidth(uint16_t newBarGraphWidth, bool b_updateObject,
                                                    bool b_enableReplaceOfCmd) {
        saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), barGraphWidth) : 0, sizeof(iVtObjectArchedBarGraph_s), 8 /* "Bar Graph Width" */, newBarGraphWidth, b_enableReplaceOfCmd);
    }

    void vtObjectArchedBarGraph_c::setMinValue(uint16_t newMin, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), minValue) : 0, sizeof(iVtObjectArchedBarGraph_s), 9 /* "Min value" */, newMin, b_enableReplaceOfCmd);
    }

    void vtObjectArchedBarGraph_c::setMaxValue(uint16_t newMax, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), maxValue) : 0, sizeof(iVtObjectArchedBarGraph_s), 10 /* "Max value" */, newMax, b_enableReplaceOfCmd);
    }

    void
    vtObjectArchedBarGraph_c::setVariableReference(IsoAgLib::iVtObject_c *newVariableReference, bool b_updateObject,
                                                   bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), variableReference) : 0, sizeof(iVtObjectArchedBarGraph_s), 11 /* "Variable Reference" */, newVariableReference, b_enableReplaceOfCmd);
    }

    void
    vtObjectArchedBarGraph_c::setTargetValueVariableReference(IsoAgLib::iVtObject_c *newTargetValueVariableReference,
                                                              bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValueVariableReference) : 0, sizeof(iVtObjectArchedBarGraph_s), 12 /* "Target Value Variable Reference" */, newTargetValueVariableReference, b_enableReplaceOfCmd);
    }

    void
    vtObjectArchedBarGraph_c::setTargetValue(uint16_t newTargetValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValue) : 0, sizeof(iVtObjectArchedBarGraph_s), 13 /* "Target Value" */, newTargetValue, b_enableReplaceOfCmd);
    }


} // __IsoAgLib
#endif
