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


#include <IsoAgLib/util/impl/util_funcs.h>
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


struct vtObjectOutputNumber_c::iVtObjectOutputNumber_s: iVtObjectwMacro_s {
	uint16_t width;
	uint16_t height;
	IsoAgLib::Colour backgroundColour;
	IsoAgLib::iVtObjectFontAttributes_c *fontAttributes;
	uint8_t options;
	iVtObject_c *variableReference;
	uint32_t value;
	int32_t offset;
	float scale;
	uint8_t numberOfDecimals;
	uint8_t format;
	uint8_t horizontalJustification;
	iVtObjectOutputNumber_s(
			IsoAgLib::ObjectID ID,
			uint16_t width,
			uint16_t height,
			IsoAgLib::Colour backgroundColour,
			IsoAgLib::iVtObjectFontAttributes_c *fontAttributes,
			uint8_t options,
			iVtObject_c *variableReference,
			uint32_t value,
			int32_t offset,
			float scale,
			uint8_t numberOfDecimals,
			uint8_t format,
			uint8_t horizontalJustification)
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
	, horizontalJustification(horizontalJustification)
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
      destMemory [2] = 12; // Object Type = Output Number
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
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

      __IsoAgLib::floatVar2LittleEndianStream (&vtObject_a->scale, &destMemory[21]);

      destMemory [25] = vtObject_a->numberOfDecimals;
      destMemory [26] = vtObject_a->format;
      destMemory [27] = vtObject_a->horizontalJustification;
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
  if (get_vtObjectOutputNumber_a()->variableReference == NULL) {
    // Save Value if requested
    if (b_updateObject)
    	vtObject_a->value = newValue;

    // Send Value update
    __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, (newValue >> 16) & 0xFF, newValue >> 24, b_enableReplaceOfCmd);
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

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectOutputNumber_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), width), sizeof(iVtObjectOutputNumber_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), width), sizeof(iVtObjectOutputNumber_s));
}

uint16_t
vtObjectOutputNumber_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), height), sizeof(iVtObjectOutputNumber_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), height), sizeof(iVtObjectOutputNumber_s));
}

uint8_t
vtObjectOutputNumber_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), backgroundColour), sizeof(iVtObjectOutputNumber_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), backgroundColour), sizeof(iVtObjectOutputNumber_s));
}

uint16_t
vtObjectOutputNumber_c::updateFontAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), fontAttributes), sizeof(iVtObjectOutputNumber_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), fontAttributes), sizeof(iVtObjectOutputNumber_s));
}

uint8_t
vtObjectOutputNumber_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), options), sizeof(iVtObjectOutputNumber_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), options), sizeof(iVtObjectOutputNumber_s));
}

uint16_t
vtObjectOutputNumber_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), variableReference), sizeof(iVtObjectOutputNumber_s), 6);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), variableReference), sizeof(iVtObjectOutputNumber_s));
}

int32_t
vtObjectOutputNumber_c::updateOffset(bool b_SendRequest)
{
  if (b_SendRequest)
    return getSignedValue32GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), offset), sizeof(iVtObjectOutputNumber_s), 7);
  else
    return getSignedValue32(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), offset), sizeof(iVtObjectOutputNumber_s));
}

float
vtObjectOutputNumber_c::updateScale(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValueFloatGetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), scale), sizeof(iVtObjectOutputNumber_s), 8);
  else
    return getValueFloat(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), scale), sizeof(iVtObjectOutputNumber_s));
}

uint8_t
vtObjectOutputNumber_c::updateNumberOfDecimals(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), numberOfDecimals), sizeof(iVtObjectOutputNumber_s), 9);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), numberOfDecimals), sizeof(iVtObjectOutputNumber_s));
}

uint8_t
vtObjectOutputNumber_c::updateFormat(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), format), sizeof(iVtObjectOutputNumber_s), 10);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), format), sizeof(iVtObjectOutputNumber_s));
}

uint8_t
vtObjectOutputNumber_c::updateJustification(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), horizontalJustification), sizeof(iVtObjectOutputNumber_s), 11);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), horizontalJustification), sizeof(iVtObjectOutputNumber_s));
}

/** that attribute is in parentheses in the spec, so commented out here
uint32_t
vtObjectOutputNumber_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue32GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), value), sizeof(iVtObjectOutputNumber_s), 12);
  else
    return getValue32(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), value), sizeof(iVtObjectOutputNumber_s));
}
*/

void
vtObjectOutputNumber_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), width), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), height), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), backgroundColour), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), fontAttributes), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), options), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), variableReference), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 7: saveSignedValue32(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), offset), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringI32(pui8_attributeValue)); break;
    case 8: saveValueFloat(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), scale), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringFloat(pui8_attributeValue)); break;
    case 9: saveValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), numberOfDecimals), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 10: saveValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), format), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 11: saveValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), horizontalJustification), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    /** that attribute is in parentheses in the spec, so commented out here
    case 12: saveValue32(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), value), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi32(pui8_attributeValue)); break;
    */
    default: break;
  }
}

#endif

    void vtObjectOutputNumber_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), width) : 0, sizeof(iVtObjectOutputNumber_s), 1, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), height) : 0, sizeof(iVtObjectOutputNumber_s), 2, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), backgroundColour) : 0, sizeof(iVtObjectOutputNumber_s), 3, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c *newValue, bool b_updateObject,
                                                   bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), fontAttributes) : 0, sizeof(iVtObjectOutputNumber_s), 4, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), options) : 0, sizeof(iVtObjectOutputNumber_s), 5, newValue, newValue, b_enableReplaceOfCmd);
    }

    void
    vtObjectOutputNumber_c::setVariableReference(IsoAgLib::iVtObjectNumberVariable_c *newValue, bool b_updateObject,
                                                 bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), variableReference) : 0, sizeof(iVtObjectOutputNumber_s), 6, (iVtObject_c*)newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setOffset(int32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue32SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), offset) : 0, sizeof(iVtObjectOutputNumber_s), 7, *((uint32_t*)&newValue), b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setScale(float newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValueFloatSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), scale) : 0, sizeof(iVtObjectOutputNumber_s), 8, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setNumberOfDecimals(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), numberOfDecimals) : 0, sizeof(iVtObjectOutputNumber_s), 9, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputNumber_c::setFormat(bool newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), format) : 0, sizeof(iVtObjectOutputNumber_s), 10, (newValue) ? 1 : 0, (newValue) ? 1 : 0, b_enableReplaceOfCmd);
    }

    void
    vtObjectOutputNumber_c::setHorizontalJustification(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), horizontalJustification) : 0, sizeof(iVtObjectOutputNumber_s), 11, newValue, newValue, b_enableReplaceOfCmd);
    }

    vtObjectOutputNumber_c::iVtObjectOutputNumber_s *vtObjectOutputNumber_c::get_vtObjectOutputNumber_a()
    {
    	return vtObject_a;
    }

    vtObjectOutputNumber_c::vtObjectOutputNumber_c(
    		vtObjectOutputNumber_c::iVtObjectOutputNumber_s *vtObjectOutputNumberSROMs, int ai_multitonInst)
            :vtObject_c((iVtObject_s*) vtObjectOutputNumberSROMs , ai_multitonInst)
    		,vtObject_a(vtObjectOutputNumberSROMs)
    {}



} // __IsoAgLib


