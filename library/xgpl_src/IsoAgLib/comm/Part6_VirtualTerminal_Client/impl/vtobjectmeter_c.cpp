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

#include "../ivtobjectnumbervariable_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"


namespace __IsoAgLib {


enum vtObjectMeter_c::AttributeID:uint8_t
{
	Width             =  1,
	NeedleColour      =  2,
	BorderColour      =  3,
	ArcAndTickColour  =  4,
	Options           =  5,
	NumberOfTicks     =  6,
	StartAngle        =  7,
	EndAngle          =  8,
	MinValue          =  9,
	MaxValue          = 10,
	VariableReference = 11,
};


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
	IsoAgLib::iVtObjectNumberVariable_c *variableReference;
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
			IsoAgLib::iVtObjectNumberVariable_c *variableReference,
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
      destMemory [2] = VT_OBJECT_TYPE_METER; // Object Type = Meter
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->needleColour, this, IsoAgLib::NeedleColour);
      destMemory [6] = getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->borderColour, this, IsoAgLib::BorderColour);
      destMemory [7] = getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->arcAndTickColour, this, IsoAgLib::ArcAndTickColour);
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
  if (vtObject_a->variableReference == NULL) {
    if (b_updateObject)
    	vtObject_a->value = newValue;

    getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, 0x00, 0x00, b_enableReplaceOfCmd);
  }
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectMeter_c::updateWidth(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute (Width);
    return vtObject_a->width;
}

uint8_t
vtObjectMeter_c::updateNeedleColour(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute (NeedleColour);
    return vtObject_a->needleColour;
}

uint8_t
vtObjectMeter_c::updateBorderColour(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute (BorderColour);
    return vtObject_a->borderColour;
}

uint8_t
vtObjectMeter_c::updateArcAndTickColour(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute (ArcAndTickColour);
    return vtObject_a->arcAndTickColour;
}

uint8_t
vtObjectMeter_c::updateOptions(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute (Options);
    return vtObject_a->options;
}

uint8_t
vtObjectMeter_c::updateNumberOfTicks(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute (NumberOfTicks);
    return vtObject_a->numberOfTicks;
}

uint8_t
vtObjectMeter_c::updateStartAngle(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute (StartAngle);
    return vtObject_a->startAngle;
}

uint8_t
vtObjectMeter_c::updateEndAngle(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute (EndAngle);
    return vtObject_a->endAngle;
}

uint16_t
vtObjectMeter_c::updateMinValue(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute (MinValue);
    return vtObject_a->minValue;
}

uint16_t
vtObjectMeter_c::updateMaxValue(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute (MaxValue);
    return vtObject_a->maxValue;
}

IsoAgLib::iVtObjectNumberVariable_c *
vtObjectMeter_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
	  getAttribute (VariableReference);
  return vtObject_a->variableReference;
}

/** that attribute is in parentheses in the spec, so commented out here
uint16_t
vtObjectMeter_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectMeter_a(), value = 12);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectMeter_a(), value), sizeof(iVtObjectMeter_s));
}
*/

void
vtObjectMeter_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case Width:             vtObject_a->width             = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case NeedleColour:      vtObject_a->needleColour      = convertLittleEndianStringColour(pui8_attributeValue); break;
    case BorderColour:      vtObject_a->borderColour      = convertLittleEndianStringColour(pui8_attributeValue); break;
    case ArcAndTickColour:  vtObject_a->arcAndTickColour  = convertLittleEndianStringColour(pui8_attributeValue); break;
    case Options:           vtObject_a->options           = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case NumberOfTicks:     vtObject_a->numberOfTicks     = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case StartAngle:        vtObject_a->startAngle        = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case EndAngle:          vtObject_a->endAngle          = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case MinValue:          vtObject_a->minValue          = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case MaxValue:          vtObject_a->maxValue          = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    //case VariableReference: vtObject_a->variableReference = convertLittleEndianStringUi16(pui8_attributeValue); break; TODO
    /** that attribute is in parentheses in the spec, so commented out here
    case 12: vtObject_a->value = convertLittleEndianStringUi16(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
    vtObjectMeter_c::vtObjectMeter_c(vtObjectMeter_c::iVtObjectMeter_s *vtObjectMeterSROM,
                                     multiton ai_multitonInst)
            :vtObject_c(ai_multitonInst)
    		,vtObject_a(vtObjectMeterSROM)
    {}

    vtObjectMeter_c::~vtObjectMeter_c() = default;

    void vtObjectMeter_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->width = newValue;
    	setAttribute ( Width /* "Width" */, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setNeedleColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->needleColour = newValue;
    	setAttribute ( NeedleColour /* "Needle Colour" */, getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::NeedleColour), b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setBorderColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->borderColour = newValue;
    	setAttribute ( BorderColour /* "BorderColour" */, getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BorderColour), b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setArcAndTickColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->arcAndTickColour = newValue;
    	setAttribute ( ArcAndTickColour /* "Arc and Tick Colour" */, getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::ArcAndTickColour), b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->options = newValue;
    	setAttribute ( Options /* "Options" */, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setNumberOfTicks(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->numberOfTicks = newValue;
    	setAttribute ( NumberOfTicks /* "# of Ticks" */, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setStartAngle(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->startAngle = newValue;
    	setAttribute ( StartAngle /* "Start Angle" */, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setEndAngle(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->endAngle = newValue;
    	setAttribute ( EndAngle /* "End Angle" */, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setMin(uint16_t newMin, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->minValue = newMin;
    	setAttribute ( MinValue /* "Min value" */, newMin, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setMax(uint16_t newMax, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->maxValue = newMax;
    	setAttribute ( MaxValue /* "Max value" */, newMax, b_enableReplaceOfCmd);
    }

    void vtObjectMeter_c::setVariableReference(IsoAgLib::iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->variableReference = newValue;
    	setAttribute ( VariableReference /* "Variable Reference" */, newValue->getID(), b_enableReplaceOfCmd);
    }


} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_meter
