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
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {




struct vtObjectInputNumber_c::iVtObjectInputNumber_s: iVtObjectwMacro_s {
	uint16_t width;
	uint16_t height;
	IsoAgLib::Colour backgroundColour;
	IsoAgLib::iVtObject_c *fontAttributes;
	uint8_t options;
	IsoAgLib::iVtObject_c *variableReference;
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
			IsoAgLib::iVtObject_c *fontAttributes,
			uint8_t options,
			IsoAgLib::iVtObject_c *variableReference,
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

      __IsoAgLib::floatVar2LittleEndianStream (&vtObject_a->scale, &destMemory[29]);

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
  if (get_vtObjectInputNumber_a()->variableReference == NULL) {
    if (b_updateObject)
    	vtObject_a->value = newValue;

    __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, (newValue >> 16) & 0xFF, newValue >> 24, b_enableReplaceOfCmd);
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

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectInputNumber_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), width), sizeof(iVtObjectInputNumber_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), width), sizeof(iVtObjectInputNumber_s));
}

uint16_t
vtObjectInputNumber_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), height), sizeof(iVtObjectInputNumber_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), height), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), backgroundColour), sizeof(iVtObjectInputNumber_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), backgroundColour), sizeof(iVtObjectInputNumber_s));
}

uint16_t
vtObjectInputNumber_c::updateFontAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), fontAttributes), sizeof(iVtObjectInputNumber_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), fontAttributes), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), options), sizeof(iVtObjectInputNumber_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), options), sizeof(iVtObjectInputNumber_s));
}

uint16_t
vtObjectInputNumber_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), variableReference), sizeof(iVtObjectInputNumber_s), 6);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), variableReference), sizeof(iVtObjectInputNumber_s));
}

uint32_t
vtObjectInputNumber_c::updateMinValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue32GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), minValue), sizeof(iVtObjectInputNumber_s), 7);
  else
    return getValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), minValue), sizeof(iVtObjectInputNumber_s));
}

uint32_t
vtObjectInputNumber_c::updateMaxValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue32GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), maxValue), sizeof(iVtObjectInputNumber_s), 8);
  else
    return getValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), maxValue), sizeof(iVtObjectInputNumber_s));
}

int32_t
vtObjectInputNumber_c::updateOffset(bool b_SendRequest)
{
  if (b_SendRequest)
    return getSignedValue32GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), offset), sizeof(iVtObjectInputNumber_s), 9);
  else
    return getSignedValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), offset), sizeof(iVtObjectInputNumber_s));
}

float
vtObjectInputNumber_c::updateScale(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValueFloatGetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), scale), sizeof(iVtObjectInputNumber_s), 10);
  else
    return getValueFloat(MACRO_getStructOffset(get_vtObjectInputNumber_a(), scale), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::updateNumberOfDecimals(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), numberOfDecimals), sizeof(iVtObjectInputNumber_s), 11);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), numberOfDecimals), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::updateFormat(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), format), sizeof(iVtObjectInputNumber_s), 12);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), format), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::updateJustification(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), horizontalJustification), sizeof(iVtObjectInputNumber_s), 13);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), horizontalJustification), sizeof(iVtObjectInputNumber_s));
}

/** these attributes are in parentheses in the spec, so commented out here
uint32_t
vtObjectInputNumber_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue32GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), value), sizeof(iVtObjectInputNumber_s), 14);
  else
    return getValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), value), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::update2ndOptionsByte(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), secondOptionsByte), sizeof(iVtObjectInputNumber_s), 15);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), secondOptionsByte), sizeof(iVtObjectInputNumber_s));
}
*/

void
vtObjectInputNumber_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), width), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), height), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), backgroundColour), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), fontAttributes), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), options), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), variableReference), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 7: saveValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), minValue), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi32(pui8_attributeValue)); break;
    case 8: saveValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), maxValue), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi32(pui8_attributeValue)); break;
    case 9: saveSignedValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), offset), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringI32(pui8_attributeValue)); break;
    case 10: saveValueFloat(MACRO_getStructOffset(get_vtObjectInputNumber_a(), scale), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringFloat(pui8_attributeValue)); break;
    case 11: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), numberOfDecimals), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 12: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), format), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 13: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), horizontalJustification), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    /** these attributes are in parentheses in the spec, so commented out here
    case 14: saveValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), value), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi32(pui8_attributeValue)); break;
    case 15: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), secondOptionsByte), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
    vtObjectInputNumber_c::vtObjectInputNumber_c(
    		vtObjectInputNumber_c::iVtObjectInputNumber_s *vtObjectInputNumberSROM, int ai_multitonInst)
            :vtObject_c(ai_multitonInst)
            ,vtObject_a(vtObjectInputNumberSROM)
    {}



    void vtObjectInputNumber_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), width) : 0, sizeof(iVtObjectInputNumber_s), 1, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), height) : 0, sizeof(iVtObjectInputNumber_s), 2, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), backgroundColour) : 0, sizeof(iVtObjectInputNumber_s), 3, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c *newValue, bool b_updateObject,
                                                  bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), fontAttributes) : 0, sizeof(iVtObjectInputNumber_s), 4, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), options) : 0, sizeof(iVtObjectInputNumber_s), 5, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setVariableReference(IsoAgLib::iVtObjectNumberVariable_c *newValue, bool b_updateObject,
                                                     bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), variableReference) : 0, sizeof(iVtObjectInputNumber_s), 6, (iVtObject_c*)newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setMinValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue32SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), minValue) : 0, sizeof(iVtObjectInputNumber_s), 7, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setMaxValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue32SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), maxValue) : 0, sizeof(iVtObjectInputNumber_s), 8, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setOffset(int32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue32SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), offset) : 0, sizeof(iVtObjectInputNumber_s), 9, (uint32_t) newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setScale(float newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValueFloatSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), scale) : 0, sizeof(iVtObjectInputNumber_s), 10, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setNumberOfDecimals(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), numberOfDecimals) : 0, sizeof(iVtObjectInputNumber_s), 11, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setFormat(bool newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), format) : 0, sizeof(iVtObjectInputNumber_s), 12, (newValue) ? 1 : 0, (newValue) ? 1 : 0, b_enableReplaceOfCmd);
    }

    void
    vtObjectInputNumber_c::setHorizontalJustification(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), horizontalJustification) : 0, sizeof(iVtObjectInputNumber_s), 13, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputNumber_c::setSecondOptionsByte(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), secondOptionsByte) : 0, sizeof(iVtObjectInputNumber_s), 14, newValue, newValue, b_enableReplaceOfCmd);
    }


} // __IsoAgLib

#endif
