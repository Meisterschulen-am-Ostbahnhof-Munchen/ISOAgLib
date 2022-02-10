/*
  vtobjectlineattributes_c.cpp

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

#include "vtobjectlineattributes_c.h"

#ifdef CONFIG_USE_VTOBJECT_lineattributes

#include "vtclient_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"


namespace __IsoAgLib {


enum vtObjectLineAttributes_c::AttributeID:uint8_t
{
	LineColour = 1,
    LineWidth  = 2,
    LineArt    = 3,
};


struct vtObjectLineAttributes_c::iVtObjectLineAttributes_s: iVtObjectwMacro_s {
	IsoAgLib::Colour lineColour;
	uint8_t lineWidth;
	uint16_t lineArt;
	explicit iVtObjectLineAttributes_s(
			IsoAgLib::ObjectID ID,
			IsoAgLib::Colour lineColour,
			uint8_t lineWidth,
			uint16_t lineArt)
    : iVtObject_s(ID)
	, iVtObjectwMacro_s(ID)
	, lineColour(lineColour)
	, lineWidth(lineWidth)
	, lineArt(lineArt) {
	}
};

int16_t
vtObjectLineAttributes_c::stream(uint8_t* destMemory,
                                 uint16_t maxBytes,
                                 objRange_t sourceOffset)
{
    uint16_t curBytes=0; /* current bytes written */;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObject_a->lineWidth;
    MACRO_scaleDimension(width);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 24; // Object Type = Line Attributes
      destMemory [3] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->lineColour, this, IsoAgLib::LineColour);
      destMemory [4] = width & 0xFF;
      destMemory [5] = vtObject_a->lineArt & 0xFF;
      destMemory [6] = vtObject_a->lineArt >> 8;

      destMemory [7] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 8;
      curBytes += 8;
    }

    MACRO_streamEventMacro(8);
    return curBytes;
}


IsoAgLib::ObjectID vtObjectLineAttributes_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


vtObjectLineAttributes_c::vtObjectLineAttributes_c(
		int ai_multitonInst,
		IsoAgLib::ObjectID ID,
		IsoAgLib::Colour lineColour,
		uint8_t lineWidth,
		uint16_t lineArt)
:vtObjectLineAttributes_c(
		new iVtObjectLineAttributes_s(
        ID,
		lineColour,
		lineWidth,
		lineArt),
		ai_multitonInst)
{
}


    vtObjectLineAttributes_c::vtObjectLineAttributes_c(
    		vtObjectLineAttributes_c::iVtObjectLineAttributes_s *vtObjectLineAttributesSROM, int ai_multitonInst)
            : vtObject_c(ai_multitonInst)
    		, vtObject_a(vtObjectLineAttributesSROM)
    {}



    vtObjectLineAttributes_c::~vtObjectLineAttributes_c() = default;




uint32_t
vtObjectLineAttributes_c::fitTerminal() const
{
  return 8+vtObject_a->numberOfMacrosToFollow*2;
}

void
vtObjectLineAttributes_c::setLineAttributes(IsoAgLib::Colour newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_updateObject, bool b_enableReplaceOfCmd) {
  if (b_updateObject) {
	vtObject_a->lineColour = getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newLineColour, this, IsoAgLib::LineColour);
    vtObject_a->lineWidth = newLineWidth;
	vtObject_a->lineArt = newLineArt;
  }
  getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeLineAttributes (this, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newLineColour, this, IsoAgLib::LineColour), newLineWidth, newLineArt, b_enableReplaceOfCmd);
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint8_t
vtObjectLineAttributes_c::updateLineColour(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(LineColour);
  return vtObject_a->lineColour;
}

uint8_t
vtObjectLineAttributes_c::updateLineWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(LineWidth);
  return vtObject_a->lineWidth;
}

uint16_t
vtObjectLineAttributes_c::updateLineArt(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(LineArt);
  return vtObject_a->lineArt;
}

void
vtObjectLineAttributes_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case LineColour: vtObject_a->lineColour = convertLittleEndianStringColour(pui8_attributeValue); break;
    case LineWidth: vtObject_a->lineWidth  = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case LineArt: vtObject_a->lineArt    = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    default: break;
  }
}
#endif
    void vtObjectLineAttributes_c::setLineColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->lineColour = newValue;
        setAttribute(LineColour, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::LineColour), b_enableReplaceOfCmd);
    }

    void vtObjectLineAttributes_c::setLineWidth(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->lineWidth = newValue;
        setAttribute(LineWidth, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLineAttributes_c::setLineArt(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->lineArt = newValue;
        setAttribute(LineArt, newValue, b_enableReplaceOfCmd);
    }






} // __IsoAgLib

#endif // CONFIG_USE_VTOBJECT_lineattributes


