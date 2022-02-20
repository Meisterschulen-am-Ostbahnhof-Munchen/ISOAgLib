/*
  vtobjectinputnumber_c.cpp

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

#include "vtobjectinputnumber_c.h"

#ifdef CONFIG_USE_VTOBJECT_inputnumber
#include <IsoAgLib/util/impl/util_funcs.h>
#include "../ivtobjectnumbervariable_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


enum vtObjectInputNumber_c::AttributeID:uint8_t
{
    Width                   = 1,
    Height                  = 2,
	BackgroundColour        = 3,
	FontAttributes          = 4,
	Options                 = 5,
	VariableReference       = 6,
	MinValue                = 7,
	MaxValue                = 8,
	Offset                  = 9,
	Scale                   = 10,
	NumberOfDecimals        = 11,
	Format                  = 12,
	HorizontalJustification = 13,
	SecondOptionsByte       = 14,
};




struct vtObjectInputNumber_c::iVtObjectInputNumber_s: iVtObjectwMacro_s {
	uint16_t width;
	uint16_t height;
	IsoAgLib::Colour backgroundColour;
	IsoAgLib::iVtObjectFontAttributes_c *fontAttributes;
	uint8_t options;
	IsoAgLib::iVtObjectNumberVariable_c* variableReference;
	uint32_t value;
	uint32_t minValue;
	uint32_t maxValue;
	int32_t offset;
	float scale;
	uint8_t numberOfDecimals;
	uint8_t format;
	uint8_t horizontalJustification;
	uint8_t secondOptionsByte;
	iVtObjectInputNumber_s(
			IsoAgLib::ObjectID ID,
			uint16_t width,
			uint16_t height,
			IsoAgLib::Colour backgroundColour,
			IsoAgLib::iVtObjectFontAttributes_c *fontAttributes,
			uint8_t options,
			IsoAgLib::iVtObjectNumberVariable_c *variableReference,
			uint32_t value,
			uint32_t minValue,
			uint32_t maxValue,
			int32_t offset,
			float scale,
			uint8_t numberOfDecimals,
			uint8_t format,
			uint8_t horizontalJustification,
			uint8_t secondOptionsByte)
    : iVtObject_s(ID)
	, iVtObjectwMacro_s(ID)
	, width(width)
	, height(height)
	, backgroundColour(backgroundColour)
	, fontAttributes(fontAttributes)
	, options(options)
	, variableReference(variableReference)
	, value(value)
	, minValue(minValue)
	, maxValue(maxValue)
	, offset(offset)
	, scale(scale)
	, numberOfDecimals(numberOfDecimals)
	, format(format)
	, horizontalJustification(horizontalJustification)
	, secondOptionsByte(secondOptionsByte)
	{
	}

};




int16_t
vtObjectInputNumber_c::stream(uint8_t* destMemory,
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
      destMemory [2] = 9; // Object Type = Input Number
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [8] = vtObject_a->fontAttributes->getID() & 0xFF;
      destMemory [9] = vtObject_a->fontAttributes->getID() >> 8;
      destMemory [10] = vtObject_a->options;
      if (vtObject_a->variableReference != NULL) {
          destMemory [11] = vtObject_a->variableReference->getID() & 0xFF;
          destMemory [12] = vtObject_a->variableReference->getID() >> 8;
      } else {
          destMemory [11] = 0xFF;
          destMemory [12] = 0xFF;
      }
      destMemory [13] = (vtObject_a->value) & 0xFF;
      destMemory [14] = (vtObject_a->value >> 8) & 0xFF;
      destMemory [15] = (vtObject_a->value >> 16) & 0xFF;
      destMemory [16] = (vtObject_a->value >> 24) & 0xFF;

      destMemory [17] = (vtObject_a->minValue) & 0xFF;
      destMemory [18] = (vtObject_a->minValue >> 8) & 0xFF;
      destMemory [19] = (vtObject_a->minValue >> 16) & 0xFF;
      destMemory [20] = (vtObject_a->minValue >> 24) & 0xFF;

      destMemory [21] = (vtObject_a->maxValue) & 0xFF;
      destMemory [22] = (vtObject_a->maxValue >> 8) & 0xFF;
      destMemory [23] = (vtObject_a->maxValue >> 16) & 0xFF;
      destMemory [24] = (vtObject_a->maxValue >> 24) & 0xFF;

      destMemory [25] = (vtObject_a->offset) & 0xFF;
      destMemory [26] = (vtObject_a->offset >> 8) & 0xFF;
      destMemory [27] = (vtObject_a->offset >> 16) & 0xFF;
      destMemory [28] = (vtObject_a->offset >> 24) & 0xFF;

      floatVar2LittleEndianStream (&vtObject_a->scale, &destMemory[29]);

      destMemory [33] = vtObject_a->numberOfDecimals;
      destMemory [34] = vtObject_a->format;
      destMemory [35] = vtObject_a->horizontalJustification;
      destMemory [36] = vtObject_a->secondOptionsByte;
      destMemory [37] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 38;
      curBytes += 38;
    }

    MACRO_streamEventMacro(38);
    return curBytes;
}


IsoAgLib::ObjectID vtObjectInputNumber_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

uint32_t
vtObjectInputNumber_c::fitTerminal() const
{
  return 38+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectInputNumber_c::updateEnable(uint8_t aui8_enOrDis)
{
	vtObject_a->secondOptionsByte = aui8_enOrDis;
}


void
vtObjectInputNumber_c::setValue(uint32_t newValue,
                                bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (vtObject_a->variableReference == NULL) {
    if (b_updateObject)
    	vtObject_a->value = newValue;

    getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, (newValue >> 16) & 0xFF, newValue >> 24, b_enableReplaceOfCmd);
  }
}

void
vtObjectInputNumber_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	  vtObject_a->width = newWidth;
	  vtObject_a->height = newHeight;
  }

  scaleSize( newWidth, newHeight );

  getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectInputNumber_c::updateWidth(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Width);
	return vtObject_a->width;
}

uint16_t
vtObjectInputNumber_c::updateHeight(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Height);
	return vtObject_a->height;
}

IsoAgLib::Colour
vtObjectInputNumber_c::updateBackgroundColour(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(BackgroundColour);
	return vtObject_a->backgroundColour;
}

IsoAgLib::iVtObjectFontAttributes_c*
vtObjectInputNumber_c::updateFontAttributes(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(FontAttributes);
	return vtObject_a->fontAttributes;
}

uint8_t
vtObjectInputNumber_c::updateOptions(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Options);
	return vtObject_a->options;
}

IsoAgLib::iVtObjectNumberVariable_c*
vtObjectInputNumber_c::updateVariableReference(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(VariableReference);
	return vtObject_a->variableReference;
}


uint32_t
vtObjectInputNumber_c::updateMinValue(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(MinValue);
	return vtObject_a->minValue;
}

uint32_t
vtObjectInputNumber_c::updateMaxValue(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(MaxValue);
	return vtObject_a->maxValue;
}

int32_t
vtObjectInputNumber_c::updateOffset(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Options);
	return vtObject_a->options;
}

float
vtObjectInputNumber_c::updateScale(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Scale);
	return vtObject_a->scale;
}

uint8_t
vtObjectInputNumber_c::updateNumberOfDecimals(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(NumberOfDecimals);
	return vtObject_a->numberOfDecimals;
}

uint8_t
vtObjectInputNumber_c::updateFormat(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Format);
	return vtObject_a->format;
}

uint8_t
vtObjectInputNumber_c::updateJustification(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(HorizontalJustification);
	return vtObject_a->horizontalJustification;
}

/** these attributes are in parentheses in the spec, so commented out here
uint32_t
vtObjectInputNumber_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue32GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), value = 14);
  else
    return getValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), value), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::update2ndOptionsByte(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), secondOptionsByte = 15);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), secondOptionsByte), sizeof(iVtObjectInputNumber_s));
}
*/

void
vtObjectInputNumber_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case Width:                   vtObject_a->width                   = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case Height:                  vtObject_a->height                  = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case BackgroundColour:        vtObject_a->backgroundColour        = convertLittleEndianStringColour(pui8_attributeValue); break;
    //case FontAttributes:          vtObject_a->fontAttributes          = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case Options:                 vtObject_a->options                 = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    //case VariableReference:       vtObject_a->variableReference       = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case MinValue:                vtObject_a->minValue                = convertLittleEndianStringUi32(  pui8_attributeValue); break;
    case MaxValue:                vtObject_a->maxValue                = convertLittleEndianStringUi32(  pui8_attributeValue); break;
    case Offset:                  vtObject_a->offset                  = convertLittleEndianStringI32(   pui8_attributeValue); break;
    case Scale:                   vtObject_a->scale                   = convertLittleEndianStringFloat( pui8_attributeValue); break;
    case NumberOfDecimals:        vtObject_a->numberOfDecimals        = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case Format:                  vtObject_a->format                  = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case HorizontalJustification: vtObject_a->horizontalJustification = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    /** these attributes are in parentheses in the spec, so commented out here
    case 14: vtObject_a->value = convertLittleEndianStringUi32(pui8_attributeValue)); break;
    case 15: vtObject_a->secondOptionsByte = convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
    vtObjectInputNumber_c::vtObjectInputNumber_c(
    		vtObjectInputNumber_c::iVtObjectInputNumber_s *vtObjectInputNumberSROM, multiton ai_multitonInst)
            :vtObject_c(ai_multitonInst)
            ,vtObject_a(vtObjectInputNumberSROM)
    {}



    void vtObjectInputNumber_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	  MACRO_scaleLocalVars
    	  MACRO_scaleSKLocalVars

    	  uint32_t scaledDim = uint32_t( newValue );
    	#ifndef USE_VT_CLIENT_OLD_UNSCALED_SIZE_COMMANDS
    	  MACRO_scaleDimension( scaledDim )
    	#endif

      	if (b_updateObject)
      		vtObject_a->width = newValue;
    	  setAttribute (Width, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
  	  MACRO_scaleLocalVars
  	  MACRO_scaleSKLocalVars

  	  uint32_t scaledDim = uint32_t( newValue );
  	#ifndef USE_VT_CLIENT_OLD_UNSCALED_SIZE_COMMANDS
  	  MACRO_scaleDimension( scaledDim )
  	#endif

    	if (b_updateObject)
    		vtObject_a->height = newValue;
  	  setAttribute (Height, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->backgroundColour = newValue;
    	setAttribute ( BackgroundColour, getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fontAttributes = newValue;
    	setAttribute (FontAttributes, (newValue == NULL) ? 65535 : newValue->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->options = newValue;
    	setAttribute ( Options, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setVariableReference(IsoAgLib::iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->variableReference = newValue;
    	setAttribute ( VariableReference, newValue->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setMinValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->minValue = newValue;
    	setAttribute ( MinValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setMaxValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->maxValue = newValue;
    	setAttribute ( MaxValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setOffset(int32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->format = newValue;
    	setAttribute ( Offset, *((uint32_t*)&newValue), b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setScale(float newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->scale = newValue;
    	setAttribute ( Scale, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setNumberOfDecimals(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->numberOfDecimals = newValue;
    	setAttribute ( NumberOfDecimals, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setFormat(bool newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->format = newValue;
    	setAttribute ( Format, (newValue) ? 1 : 0, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setHorizontalJustification(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->horizontalJustification = newValue;
    	setAttribute (HorizontalJustification, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setSecondOptionsByte(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->secondOptionsByte = newValue;
    	setAttribute (SecondOptionsByte, newValue, b_enableReplaceOfCmd);
    }

    vtObjectInputNumber_c::vtObjectInputNumber_c(
            multiton ai_multitonInst,
            IsoAgLib::ObjectID ID,
            uint16_t width,
            uint16_t height,
            IsoAgLib::Colour backgroundColour,
            IsoAgLib::iVtObjectFontAttributes_c *fontAttributes,
            uint8_t options,
            IsoAgLib::iVtObjectNumberVariable_c *variableReference,
            uint32_t value,
            uint32_t minValue,
            uint32_t maxValue,
            int32_t offset,
            float scale,
            uint8_t numberOfDecimals,
            uint8_t format,
            uint8_t horizontalJustification,
            uint8_t secondOptionsByte)
            :vtObjectInputNumber_c(
            new iVtObjectInputNumber_s(
                    ID,
                    width,
                    height,
                    backgroundColour,
                    fontAttributes,
                    options,
                    variableReference,
                    value,
                    minValue,
                    maxValue,
                    offset,
                    scale,
                    numberOfDecimals,
                    format,
                    horizontalJustification,
                    secondOptionsByte),
            ai_multitonInst)
    {
    }

    vtObjectInputNumber_c::~vtObjectInputNumber_c() = default;


} // __IsoAgLib

#endif
