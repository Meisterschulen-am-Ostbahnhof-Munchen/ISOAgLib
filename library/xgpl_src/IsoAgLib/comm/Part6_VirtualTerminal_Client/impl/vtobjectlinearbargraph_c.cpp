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

#include "../ivtobjectnumbervariable_c.h"
#include "../ivtobject_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {



enum vtObjectLinearBarGraph_c::AttributeID:uint8_t
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


struct vtObjectLinearBarGraph_c::iVtObjectLinearBarGraph_s : iVtObjectwMacro_s {
  uint16_t width;
  uint16_t height;
  IsoAgLib::Colour colour;
  IsoAgLib::Colour targetLineColour;
  uint8_t options;
  uint8_t numberOfTicks;
  uint16_t minValue;
  uint16_t maxValue;
  IsoAgLib::iVtObjectNumberVariable_c* variableReference;
  uint16_t value;
  IsoAgLib::iVtObjectNumberVariable_c* targetValueVariableReference;
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
			IsoAgLib::iVtObjectNumberVariable_c *variableReference,
			uint16_t value,
			IsoAgLib::iVtObjectNumberVariable_c *targetValueVariableReference,
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
		IsoAgLib::iVtObjectNumberVariable_c *variableReference,
		uint16_t value,
		IsoAgLib::iVtObjectNumberVariable_c *targetValueVariableReference,
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



vtObjectLinearBarGraph_c::~vtObjectLinearBarGraph_c() = default;


uint32_t
vtObjectLinearBarGraph_c::fitTerminal() const
{
  return 24+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectLinearBarGraph_c::setValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (vtObject_a->variableReference == NULL) {
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
    getAttribute(Width);
  return vtObject_a->width;
}

uint16_t
vtObjectLinearBarGraph_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(Height);
  return vtObject_a->height;
}

IsoAgLib::Colour
vtObjectLinearBarGraph_c::updateColour(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(Colour);
  return vtObject_a->colour;
}

IsoAgLib::Colour
vtObjectLinearBarGraph_c::updateTargetLineColour(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(TargetLineColour);
  return vtObject_a->targetLineColour;
}

uint8_t
vtObjectLinearBarGraph_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(Options);
  return vtObject_a->options;
}

uint8_t
vtObjectLinearBarGraph_c::updateNumberOfTicks(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(NumberOfTicks);
  return vtObject_a->numberOfTicks;
}

uint16_t
vtObjectLinearBarGraph_c::updateMinValue(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(MinValue);
  return vtObject_a->minValue;
}

uint16_t
vtObjectLinearBarGraph_c::updateMaxValue(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(MaxValue);
  return vtObject_a->maxValue;
}

IsoAgLib::iVtObjectNumberVariable_c*
vtObjectLinearBarGraph_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(VariableReference);
  return vtObject_a->variableReference;
}

IsoAgLib::iVtObjectNumberVariable_c*
vtObjectLinearBarGraph_c::updateTargetValueVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(TargetValueVariableReference);
  return vtObject_a->targetValueVariableReference;
}

uint16_t
vtObjectLinearBarGraph_c::updateTargetValue(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(TargetValue);
  return vtObject_a->targetValue;
}

/** that attribute is in parentheses in the spec, so commented out here
uint16_t
vtObjectLinearBarGraph_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(TargetLineColour);
  return vtObject_a->targetLineColour;
}
*/

void
vtObjectLinearBarGraph_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case Width:                        vtObject_a->width                        = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case Height:                       vtObject_a->height                       = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case Colour:                       vtObject_a->colour                       = convertLittleEndianStringColour(pui8_attributeValue); break;
    case TargetLineColour:             vtObject_a->targetLineColour             = convertLittleEndianStringColour(pui8_attributeValue); break;
    case Options:                      vtObject_a->options                      = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case NumberOfTicks:                vtObject_a->numberOfTicks                = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case MinValue:                     vtObject_a->minValue                     = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case MaxValue:                     vtObject_a->maxValue                     = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    //case VariableReference:            vtObject_a->variableReference            = convertLittleEndianStringUi16(pui8_attributeValue); break; //TODO
    //case TargetValueVariableReference: vtObject_a->targetValueVariableReference = convertLittleEndianStringUi16(pui8_attributeValue); break; //TODO
    case TargetValue:                  vtObject_a->targetValue                  = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    /** that attribute is in parentheses in the spec, so commented out here
    case 12: vtObject_a->targetValue = convertLittleEndianStringUi16(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif




    void vtObjectLinearBarGraph_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->width = newValue;
        setAttribute(Width, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->height = newValue;
    	setAttribute(Height, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->colour = newValue;
    	setAttribute(Colour, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::AColour), b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setTargetLineColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->targetLineColour = newValue;
    	setAttribute(TargetLineColour, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::TargetLineColour), b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->options = newValue;
    	setAttribute(Options, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setNumberOfTicks(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->numberOfTicks = newValue;
    	setAttribute(NumberOfTicks, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setMinValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->minValue = newValue;
    	setAttribute(MinValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setMaxValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->maxValue = newValue;
    	setAttribute(MaxValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setVariableReference(IsoAgLib::iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->variableReference = newValue;
        setAttribute(VariableReference, newValue->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setTargetValueVariableReference(IsoAgLib::iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->targetValueVariableReference = newValue;
    	setAttribute(TargetValueVariableReference, newValue->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectLinearBarGraph_c::setTargetValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->targetValue = newValue;
        setAttribute(TargetValue, newValue, b_enableReplaceOfCmd);
    }



} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_linearbargraph
