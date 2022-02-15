/*
  vtobjectmeter_c.cpp

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

#include "vtobjectmeter_c.h"

#ifdef CONFIG_USE_VTOBJECT_meter
#include "vtclient_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"


namespace __IsoAgLib {


struct vtObjectMeter_c::iVtObjectMeter_s: iVtObjectwMacro_s {
	uint16_t width;
	IsoAgLib::Colour needleColour;
	IsoAgLib::Colour borderColour;
	IsoAgLib::Colour arcAndTickColour;
	uint8_t options;
	uint8_t numberOfTicks;
	uint8_t startAngle;
	uint8_t endAngle;
	uint16_t minValue;
	uint16_t maxValue;
	IsoAgLib::iVtObject_c *variableReference;
	uint16_t value;
	iVtObjectMeter_s(
			IsoAgLib::ObjectID ID,
			uint16_t width,
			IsoAgLib::Colour needleColour,
			IsoAgLib::Colour borderColour,
			IsoAgLib::Colour arcAndTickColour,
			uint8_t options,
			uint8_t numberOfTicks,
			uint8_t startAngle,
			uint8_t endAngle,
			uint16_t minValue,
			uint16_t maxValue,
			IsoAgLib::iVtObject_c *variableReference,
			uint16_t value)
    : iVtObject_s(ID)
	, iVtObjectwMacro_s(ID)
	, width(width)
	, needleColour(needleColour)
	, borderColour(borderColour)
	, arcAndTickColour(arcAndTickColour)
	, options(options)
	, numberOfTicks(numberOfTicks)
	, startAngle(startAngle)
	, endAngle(endAngle)
	, minValue(minValue)
	, maxValue(maxValue)
	, variableReference(variableReference)
	, value(value)
	{
	}
};

int16_t
vtObjectMeter_c::stream(uint8_t* destMemory,
                        uint16_t maxBytes,
                        objRange_t sourceOffset)
{
    uint16_t curBytes=0; /* current bytes written */;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObject_a->width;
    MACRO_scaleDimension(width);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 17; // Object Type = Meter
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->needleColour, this, IsoAgLib::NeedleColour);
      destMemory [6] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->borderColour, this, IsoAgLib::BorderColour);
      destMemory [7] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->arcAndTickColour, this, IsoAgLib::ArcAndTickColour);
      destMemory [8] = vtObject_a->options;
      destMemory [9] = vtObject_a->numberOfTicks;
      destMemory [10] = vtObject_a->startAngle;
      destMemory [11] = vtObject_a->endAngle;
      destMemory [12] = vtObject_a->minValue & 0xFF;
      destMemory [13] = vtObject_a->minValue >> 8;
      destMemory [14] = vtObject_a->maxValue & 0xFF;
      destMemory [15] = vtObject_a->maxValue >> 8;
      if (vtObject_a->variableReference != NULL) {
        destMemory [16] = vtObject_a->variableReference->getID() & 0xFF;
        destMemory [17] = vtObject_a->variableReference->getID() >> 8;
      } else {
        destMemory [16] = 0xFF;
        destMemory [17] = 0xFF;
      }
      destMemory [18] = vtObject_a->value & 0xFF;
      destMemory [19] = vtObject_a->value >> 8;

      destMemory [20] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 21;
      curBytes += 21;
    }

    MACRO_streamEventMacro(21);
    return curBytes;
}

IsoAgLib::ObjectID vtObjectMeter_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectMeter_c::fitTerminal() const
{
  return 21+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectMeter_c::setValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (get_vtObjectMeter_a()->variableReference == NULL) {
    if (b_updateObject)
    	vtObject_a->value = newValue;

    __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, 0x00, 0x00, b_enableReplaceOfCmd);
  }
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectMeter_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectMeter_a(), width), sizeof(iVtObjectMeter_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectMeter_a(), width), sizeof(iVtObjectMeter_s));
}

uint8_t
vtObjectMeter_c::updateNeedleColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectMeter_a(), needleColour), sizeof(iVtObjectMeter_s), 2);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), needleColour), sizeof(iVtObjectMeter_s));
}

uint8_t
vtObjectMeter_c::updateBorderColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectMeter_a(), borderColour), sizeof(iVtObjectMeter_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), borderColour), sizeof(iVtObjectMeter_s));
}

uint8_t
vtObjectMeter_c::updateArcAndTickColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectMeter_a(), arcAndTickColour), sizeof(iVtObjectMeter_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), arcAndTickColour), sizeof(iVtObjectMeter_s));
}

uint8_t
vtObjectMeter_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectMeter_a(), options), sizeof(iVtObjectMeter_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), options), sizeof(iVtObjectMeter_s));
}

uint8_t
vtObjectMeter_c::updateNumberOfTicks(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectMeter_a(), numberOfTicks), sizeof(iVtObjectMeter_s), 6);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), numberOfTicks), sizeof(iVtObjectMeter_s));
}

uint8_t
vtObjectMeter_c::updateStartAngle(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectMeter_a(), startAngle), sizeof(iVtObjectMeter_s), 7);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), startAngle), sizeof(iVtObjectMeter_s));
}

uint8_t
vtObjectMeter_c::updateEndAngle(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectMeter_a(), endAngle), sizeof(iVtObjectMeter_s), 8);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), endAngle), sizeof(iVtObjectMeter_s));
}

uint16_t
vtObjectMeter_c::updateMinValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectMeter_a(), minValue), sizeof(iVtObjectMeter_s), 9);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectMeter_a(), minValue), sizeof(iVtObjectMeter_s));
}

uint16_t
vtObjectMeter_c::updateMaxValue(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute (10, false);
    return vtObject_a->maxValue;
}

IsoAgLib::iVtObject_c *
vtObjectMeter_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
	  getAttribute (11, false);
  return vtObject_a->variableReference;
}

/** that attribute is in parentheses in the spec, so commented out here
uint16_t
vtObjectMeter_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectMeter_a(), value), sizeof(iVtObjectMeter_s), 12);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectMeter_a(), value), sizeof(iVtObjectMeter_s));
}
*/

void
vtObjectMeter_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectMeter_a(), width), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), needleColour), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), borderColour), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), arcAndTickColour), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), options), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), numberOfTicks), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 7: saveValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), startAngle), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 8: saveValue8(MACRO_getStructOffset(get_vtObjectMeter_a(), endAngle), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 9: saveValue16(MACRO_getStructOffset(get_vtObjectMeter_a(), minValue), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 10: saveValue16(MACRO_getStructOffset(get_vtObjectMeter_a(), maxValue), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 11: saveValue16(MACRO_getStructOffset(get_vtObjectMeter_a(), variableReference), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    /** that attribute is in parentheses in the spec, so commented out here
    case 12: saveValue16(MACRO_getStructOffset(get_vtObjectMeter_a(), value), sizeof(iVtObjectMeter_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
    vtObjectMeter_c::vtObjectMeter_c(vtObjectMeter_c::iVtObjectMeter_s *vtObjectMeterSROM,
                                     int ai_multitonInst)
            :vtObject_c(ai_multitonInst)
    		,vtObject_a(vtObjectMeterSROM)
    {}



    void vtObjectMeter_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectMeter_a(), width) : 0, sizeof(iVtObjectMeter_s), 1 /* "Width" */, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setNeedleColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectMeter_a(), needleColour) : 0, sizeof(iVtObjectMeter_s), 2 /* "Needle Colour" */, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::NeedleColour), b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setBorderColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectMeter_a(), borderColour) : 0, sizeof(iVtObjectMeter_s), 3 /* "BorderColour" */, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BorderColour), b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setArcAndTickColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectMeter_a(), arcAndTickColour) : 0, sizeof(iVtObjectMeter_s), 4 /* "Arc and Tick Colour" */, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::ArcAndTickColour), b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectMeter_a(), options) : 0, sizeof(iVtObjectMeter_s), 5 /* "Options" */, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setNumberOfTicks(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectMeter_a(), numberOfTicks) : 0, sizeof(iVtObjectMeter_s), 6 /* "# of Ticks" */, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setStartAngle(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectMeter_a(), startAngle) : 0, sizeof(iVtObjectMeter_s), 7 /* "Start Angle" */, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setEndAngle(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectMeter_a(), endAngle) : 0, sizeof(iVtObjectMeter_s), 8 /* "End Angle" */, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setMin(uint16_t newMin, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectMeter_a(), minValue) : 0, sizeof(iVtObjectMeter_s), 9 /* "Min value" */, newMin, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setMax(uint16_t newMax, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectMeter_a(), maxValue) : 0, sizeof(iVtObjectMeter_s), 10 /* "Max value" */, newMax, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setVariableReference(IsoAgLib::iVtObject_c *newValue, bool b_updateObject,
                                               bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectMeter_a(), variableReference) : 0, sizeof(iVtObjectMeter_s), 11 /* "Variable Reference" */, newValue, b_enableReplaceOfCmd);
    }


} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_meter
