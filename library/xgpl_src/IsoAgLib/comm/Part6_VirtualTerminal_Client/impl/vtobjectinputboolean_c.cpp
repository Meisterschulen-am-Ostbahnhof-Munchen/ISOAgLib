/*
  vtobjectinputboolean_c.cpp

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

#include "vtobjectinputboolean_c.h"

#ifdef CONFIG_USE_VTOBJECT_inputboolean
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectnumbervariable_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {



enum vtObjectInputBoolean_c::AttributeID:uint8_t
{
	BackgroundColour    = 1,
	Width               = 2,
	ForegroundColour    = 3,
	VariableReference   = 4,
	Value               = 5,
	Enabled             = 6,
};


struct vtObjectInputBoolean_c::iVtObjectInputBoolean_s : iVtObjectwMacro_s {
  IsoAgLib::Colour backgroundColour;
  uint16_t width;
  IsoAgLib::iVtObjectFontAttributes_c* foregroundColour;
  IsoAgLib::iVtObjectNumberVariable_c* variableReference;
  uint8_t value;
  IsoAgLib::Enabled enabled;
  iVtObjectInputBoolean_s(
		IsoAgLib::ObjectID ID,
		IsoAgLib::Colour backgroundColour,
		uint16_t width,
		IsoAgLib::iVtObjectFontAttributes_c *foregroundColour,
		IsoAgLib::iVtObjectNumberVariable_c *variableReference,
		uint8_t value,
		IsoAgLib::Enabled enabled);
};

    vtObjectInputBoolean_c::iVtObjectInputBoolean_s::iVtObjectInputBoolean_s(
            IsoAgLib::ObjectID ID,
            IsoAgLib::Colour backgroundColour,
            uint16_t width,
            IsoAgLib::iVtObjectFontAttributes_c *foregroundColour,
            IsoAgLib::iVtObjectNumberVariable_c *variableReference,
            uint8_t value,
			IsoAgLib::Enabled enabled)
            : iVtObject_s(ID)
            , iVtObjectwMacro_s(ID)
            , backgroundColour(backgroundColour)
            , width(width)
            , foregroundColour(foregroundColour)
            , variableReference(variableReference)
            , value(value)
            , enabled(enabled)
    {}

uint16_t
vtObjectInputBoolean_c::stream(uint8_t* destMemory,
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
      destMemory [2] = VT_OBJECT_TYPE_INPUT_BOOLEAN; // Object Type = Input Boolean
      destMemory [3] = getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [4] = width & 0xFF;
      destMemory [5] = width >> 8;
      destMemory [6] = vtObject_a->foregroundColour->getID() & 0xFF;
      destMemory [7] = vtObject_a->foregroundColour->getID() >> 8;
      if (vtObject_a->variableReference != NULL) {
        destMemory [8] = vtObject_a->variableReference->getID() & 0xFF;
        destMemory [9] = vtObject_a->variableReference->getID() >> 8;
      } else {
        destMemory [8] = 0xFF;
        destMemory [9] = 0xFF;
      }
      destMemory [10] = vtObject_a->value;
      destMemory [11] = vtObject_a->enabled;
      destMemory [12] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 13;
      curBytes += 13;
    }

    MACRO_streamEventMacro(13);
    return curBytes;
}


IsoAgLib::ObjectID vtObjectInputBoolean_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}



uint32_t
vtObjectInputBoolean_c::fitTerminal() const
{
  return 13+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectInputBoolean_c::updateEnable(IsoAgLib::Enabled aui8_enOrDis)
{
	vtObject_a->enabled = aui8_enOrDis;
}


void
vtObjectInputBoolean_c::setValue(bool newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (vtObject_a->variableReference == NULL) {
    if (b_updateObject)
    	vtObject_a->value = newValue;
    getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, newValue?1:0, 0x00, 0x00, 0x00, b_enableReplaceOfCmd);
  }
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
IsoAgLib::Colour
vtObjectInputBoolean_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(BackgroundColour);
  return vtObject_a->backgroundColour;
}

uint16_t
vtObjectInputBoolean_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(Width);
  return vtObject_a->width;
}

IsoAgLib::iVtObjectFontAttributes_c*
vtObjectInputBoolean_c::updateForegroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(ForegroundColour);
  return vtObject_a->foregroundColour;
}

IsoAgLib::iVtObjectNumberVariable_c*
vtObjectInputBoolean_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(VariableReference);
  return vtObject_a->variableReference;
}

/** these attributes are in parentheses in the spec, so commented out here
uint8_t
vtObjectInputBoolean_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), value), sizeof(iVtObjectInputBoolean_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), value), sizeof(iVtObjectInputBoolean_s));
}

uint8_t
vtObjectInputBoolean_c::updateEnabled(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), enabled), sizeof(iVtObjectInputBoolean_s), 6);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), enabled), sizeof(iVtObjectInputBoolean_s));
}
*/

void
vtObjectInputBoolean_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case BackgroundColour:  vtObject_a->backgroundColour  = convertLittleEndianStringColour( pui8_attributeValue); break;
    case Width:             vtObject_a->width             = convertLittleEndianStringUi16(   pui8_attributeValue); break;
    //case ForegroundColour:  vtObject_a->foregroundColour  = convertLittleEndianStringUi16(pui8_attributeValue); break; //TODO
    //case VariableReference: vtObject_a->variableReference = convertLittleEndianStringUi16(pui8_attributeValue); break; //TODO
    /** these attributes are in parentheses in the spec, so commented out here
    case 5: vtObject_a->value), sizeof(iVtObjectInputBoolean_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: vtObject_a->enabled), sizeof(iVtObjectInputBoolean_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
    vtObjectInputBoolean_c::vtObjectInputBoolean_c(
    		vtObjectInputBoolean_c::iVtObjectInputBoolean_s *vtObjectInputBooleanSROM, multiton ai_multitonInst)
            :vtObject_c(ai_multitonInst)
    		,vtObject_a(vtObjectInputBooleanSROM)
    {}



    void vtObjectInputBoolean_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->backgroundColour = newValue;
    	setAttribute(BackgroundColour, getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectInputBoolean_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->width = newValue;
    	setAttribute(Width, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputBoolean_c::setForegroundColour(IsoAgLib::iVtObjectFontAttributes_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->foregroundColour = newValue;
    	setAttribute(ForegroundColour, newValue->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectInputBoolean_c::setVariableReference(IsoAgLib::iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->variableReference = newValue;
    	setAttribute(VariableReference, newValue->getID(), b_enableReplaceOfCmd);
    }

    vtObjectInputBoolean_c::vtObjectInputBoolean_c(
    		multiton ai_multitonInst,
			IsoAgLib::ObjectID ID,
			IsoAgLib::Colour backgroundColour,
			uint16_t width,
			IsoAgLib::iVtObjectFontAttributes_c *foregroundColour,
			IsoAgLib::iVtObjectNumberVariable_c *variableReference,
			uint8_t value,
			IsoAgLib::Enabled enabled)
    :vtObjectInputBoolean_c(
    		new iVtObjectInputBoolean_s(
    				ID,
    				backgroundColour,
    				width,
    				foregroundColour,
    				variableReference,
    				value,
    				enabled),
    		ai_multitonInst)
    {

    }

    IsoAgLib::Enabled vtObjectInputBoolean_c::getEnabled() {
        return vtObject_a->enabled;}

    bool vtObjectInputBoolean_c::enable(bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObject_c::able (IsoAgLib::enabled, b_updateObject, b_enableReplaceOfCmd); }

    bool vtObjectInputBoolean_c::disable(bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObject_c::able (IsoAgLib::disabled, b_updateObject, b_enableReplaceOfCmd); }


    vtObjectInputBoolean_c::~vtObjectInputBoolean_c() = default;


} // __IsoAgLib

#endif
