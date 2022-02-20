/*
  vtobjectoutputnumber_c.cpp

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

#include "vtobjectoutputnumber_c.h"

#ifdef CONFIG_USE_VTOBJECT_outputnumber
#include <IsoAgLib/util/impl/util_funcs.h>
#include "../ivtobjectnumbervariable_c.h"
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {



enum vtObjectOutputNumber_c::AttributeID:uint8_t
{

    Width                   = 1,
    Height                  = 2,
	BackgroundColour        = 3,
	FontAttributes          = 4,
	Options                 = 5,
	VariableReference       = 6,
	Offset                  = 7,
	Scale                   = 8,
	NumberOfDecimals        = 9,
	Format                  = 10,
	Justification = 11,
};




struct vtObjectOutputNumber_c::iVtObjectOutputNumber_s: iVtObjectwMacro_s {
	uint16_t width;
	uint16_t height;
	IsoAgLib::Colour backgroundColour;
	IsoAgLib::iVtObjectFontAttributes_c *fontAttributes;
	uint8_t options;
	IsoAgLib::iVtObjectNumberVariable_c *variableReference;
	uint32_t value;
	int32_t offset;
	float scale;
	uint8_t numberOfDecimals;
	uint8_t format;
	Justification justification;
	iVtObjectOutputNumber_s(
			IsoAgLib::ObjectID ID,
			uint16_t width,
			uint16_t height,
			IsoAgLib::Colour backgroundColour,
			IsoAgLib::iVtObjectFontAttributes_c *fontAttributes,
			uint8_t options,
			IsoAgLib::iVtObjectNumberVariable_c *variableReference,
			uint32_t value,
			int32_t offset,
			float scale,
			uint8_t numberOfDecimals,
			uint8_t format,
			Justification justification)
    : iVtObject_s(ID)
	, iVtObjectwMacro_s(ID)
	, width(width)
	, height(height)
	, backgroundColour(backgroundColour)
	, fontAttributes(fontAttributes)
	, options(options)
	, variableReference(variableReference)
	, value(value)
	, offset(offset)
	, scale(scale)
	, numberOfDecimals(numberOfDecimals)
	, format(format)
	, justification(justification)
	{
	}


};

int16_t
vtObjectOutputNumber_c::stream(uint8_t* destMemory,
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
      destMemory [2] = VT_OBJECT_TYPE_OUTPUT_NUMBER; // Object Type = Output Number
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

      uint32_t offset = *((uint32_t*)&(vtObject_a->offset));
      destMemory [17] = (offset) & 0xFF;
      destMemory [18] = (offset >> 8) & 0xFF;
      destMemory [19] = (offset >> 16) & 0xFF;
      destMemory [20] = (offset >> 24) & 0xFF;

      floatVar2LittleEndianStream (&vtObject_a->scale, &destMemory[21]);

      destMemory [25] = vtObject_a->numberOfDecimals;
      destMemory [26] = vtObject_a->format;
      destMemory [27] = vtObject_a->justification;
      destMemory [28] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 29;
      curBytes += 29;
    }

    MACRO_streamEventMacro(29);
    return curBytes;
}


IsoAgLib::ObjectID vtObjectOutputNumber_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

uint32_t
vtObjectOutputNumber_c::fitTerminal() const
{
  return 29+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectOutputNumber_c::setValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (vtObject_a->variableReference == NULL) {
    // Save Value if requested
    if (b_updateObject)
    	vtObject_a->value = newValue;

    // Send Value update
    getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, (newValue >> 16) & 0xFF, newValue >> 24, b_enableReplaceOfCmd);
  }
}


void
vtObjectOutputNumber_c::setOriginSKM(bool b_SKM)
{
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    vtObject_a->fontAttributes->setOriginSKM (b_SKM);
  }
}

void
vtObjectOutputNumber_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn)
{
  if (p_btn)
  {
    p_parentButtonObject = p_btn;
    vtObject_a->fontAttributes->setOriginBTN (p_btn);
  }
}

void
vtObjectOutputNumber_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
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
vtObjectOutputNumber_c::updateWidth(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Width);
	return vtObject_a->width;
}


uint16_t
vtObjectOutputNumber_c::updateHeight(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Height);
	return vtObject_a->height;
}

IsoAgLib::Colour
vtObjectOutputNumber_c::updateBackgroundColour(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(BackgroundColour);
	return vtObject_a->backgroundColour;
}

IsoAgLib::iVtObjectFontAttributes_c*
vtObjectOutputNumber_c::updateFontAttributes(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(FontAttributes);
	return vtObject_a->fontAttributes;
}

uint8_t
vtObjectOutputNumber_c::updateOptions(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Options);
	return vtObject_a->options;
}

IsoAgLib::iVtObjectNumberVariable_c *
vtObjectOutputNumber_c::updateVariableReference(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(VariableReference);
	return vtObject_a->variableReference;
}

int32_t
vtObjectOutputNumber_c::updateOffset(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Options);
	return vtObject_a->options;
}

float
vtObjectOutputNumber_c::updateScale(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Scale);
	return vtObject_a->scale;
}

uint8_t
vtObjectOutputNumber_c::updateNumberOfDecimals(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(NumberOfDecimals);
	return vtObject_a->numberOfDecimals;
}

uint8_t
vtObjectOutputNumber_c::updateFormat(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Format);
	return vtObject_a->format;
}

uint8_t
vtObjectOutputNumber_c::updateJustification(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Justification);
	return vtObject_a->justification;
}

/** that attribute is in parentheses in the spec, so commented out here
uint32_t
vtObjectOutputNumber_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue32GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), value = 12);
  else
    return getValue32(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), value), sizeof(iVtObjectOutputNumber_s));
}
*/

void
vtObjectOutputNumber_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case Width:                   vtObject_a->width                   = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case Height:                  vtObject_a->height                  = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case BackgroundColour:        vtObject_a->backgroundColour        = convertLittleEndianStringColour(pui8_attributeValue); break;
    //case FontAttributes:          vtObject_a->fontAttributes          = convertLittleEndianStringUi16(pui8_attributeValue); break;
    case Options:                 vtObject_a->options                 = convertLittleEndianStringUi8(pui8_attributeValue); break;
    //case VariableReference:       vtObject_a->variableReference       = convertLittleEndianStringUi16(pui8_attributeValue); break;
    case Offset:                  vtObject_a->offset                  = convertLittleEndianStringI32(pui8_attributeValue); break;
    case Scale:                   vtObject_a->scale                   = convertLittleEndianStringFloat(pui8_attributeValue); break;
    case NumberOfDecimals:        vtObject_a->numberOfDecimals        = convertLittleEndianStringUi8(pui8_attributeValue); break;
    case Format:                  vtObject_a->format                  = convertLittleEndianStringUi8(pui8_attributeValue); break;
    case Justification: vtObject_a->justification = convertLittleEndianStringUi8(pui8_attributeValue); break;
    /** that attribute is in parentheses in the spec, so commented out here
    case 12: saveValue32(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), value = convertLittleEndianStringUi32(pui8_attributeValue)); break;
    */
    default: break;
  }
}

#endif

    void vtObjectOutputNumber_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
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

    void vtObjectOutputNumber_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
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

    void vtObjectOutputNumber_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->backgroundColour = newValue;
    	setAttribute ( BackgroundColour, getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c *newValue, bool b_updateObject,bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fontAttributes = newValue;
    	setAttribute (FontAttributes, (newValue == NULL) ? 65535 : newValue->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->options = newValue;
    	setAttribute ( Options, newValue, b_enableReplaceOfCmd);
    }

    void
    vtObjectOutputNumber_c::setVariableReference(IsoAgLib::iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->variableReference = newValue;
    	setAttribute ( VariableReference, newValue->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setOffset(int32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->format = newValue;
    	setAttribute ( Offset, *((uint32_t*)&newValue), b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setScale(float newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->scale = newValue;
    	setAttribute ( Scale, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setNumberOfDecimals(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->numberOfDecimals = newValue;
    	setAttribute ( NumberOfDecimals, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setFormat(bool newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    	    vtObject_a->format = newValue;
    	setAttribute ( Format, (newValue) ? 1 : 0, b_enableReplaceOfCmd);
    }

    void
    vtObjectOutputNumber_c::setJustification(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->justification = newValue;
    	setAttribute (Justification, newValue, b_enableReplaceOfCmd);
    }





    vtObjectOutputNumber_c::vtObjectOutputNumber_c(
    		vtObjectOutputNumber_c::iVtObjectOutputNumber_s *vtObjectOutputNumberSROMs, multiton ai_multitonInst)
            :vtObject_c(ai_multitonInst)
    		,vtObject_a(vtObjectOutputNumberSROMs)
    {}

    vtObjectOutputNumber_c::~vtObjectOutputNumber_c() = default;

} // __IsoAgLib

#endif
