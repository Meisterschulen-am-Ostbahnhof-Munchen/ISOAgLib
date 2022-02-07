/*
  vtobjectline_c.cpp

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

#include "vtobjectline_c.h"


#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {



struct vtObjectLine_c::iVtObjectLine_s : iVtObjectwMacro_s {
  IsoAgLib::iVtObjectLineAttributes_c* lineAttributes;
  uint16_t width;
  uint16_t height;
  uint8_t lineDirection;
  explicit iVtObjectLine_s(
		  IsoAgLib::ObjectID ID,
		  IsoAgLib::iVtObjectLineAttributes_c *lineAttributes,
			uint16_t width,
			uint16_t height,
			uint8_t lineDirection)
  : iVtObject_s(ID)
  , iVtObjectwMacro_s(ID)
  , lineAttributes(lineAttributes)
  , width(width)
  , height(height)
  , lineDirection(lineDirection)
  {}
};

int16_t
vtObjectLine_c::stream(uint8_t* destMemory,
                       uint16_t maxBytes,
                       objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectLine_a
#define MACRO_vtObjectTypeS iVtObjectLine_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObject_a->width;
    uint32_t height = (uint32_t)vtObject_a->height;
    MACRO_scaleSizeI32(width, height);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 13; // Object Type = Line
      destMemory [3] = vtObject_a->lineAttributes->getID() & 0xFF;
      destMemory [4] = vtObject_a->lineAttributes->getID() >> 8;
      destMemory [5] = width & 0xFF;
      destMemory [6] = width >> 8;
      destMemory [7] = height & 0xFF;
      destMemory [8] = height >> 8;
      destMemory [9] = vtObject_a->lineDirection;
      destMemory [10] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 11;
      curBytes += 11;
    }

    MACRO_streamEventMacro(11);
    return curBytes;
}


IsoAgLib::ObjectID vtObjectLine_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


vtObjectLine_c::vtObjectLine_c(
		int ai_multitonInst,
		IsoAgLib::ObjectID ID,
		IsoAgLib::iVtObjectLineAttributes_c *lineAttributes,
		uint16_t width,
		uint16_t height,
		uint8_t lineDirection)
	: vtObjectLine_c(
		new iVtObjectLine_s(
	        ID,
			lineAttributes,
			width,
			height,
			lineDirection),
			ai_multitonInst)
{
}



vtObjectLine_c::vtObjectLine_c(vtObjectLine_c::iVtObjectLine_s *vtObjectLineSROM, int ai_multitonInst)
        : vtObject_c((iVtObject_s*) vtObjectLineSROM , ai_multitonInst)
		, vtObject_a(vtObjectLineSROM)
{}

vtObjectLine_c::iVtObjectLine_s *vtObjectLine_c::get_vtObjectLine_a() {
	return vtObject_a;
}


uint32_t
vtObjectLine_c::fitTerminal() const
{
  return 11+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectLine_c::setOriginSKM(bool b_SKM)
{
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    vtObject_a->lineAttributes->setOriginSKM (b_SKM);
  }
}

void
vtObjectLine_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	  vtObject_a->width = newWidth;
	  vtObject_a->height = newHeight;
  }

  scaleSize( newWidth, newHeight );

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

void
vtObjectLine_c::setEndPoint (uint16_t newWidth, uint16_t newHeight, uint8_t newLineDirection, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	  vtObject_a->width = newWidth;
	  vtObject_a->height = newHeight;
	  vtObject_a->lineDirection = newLineDirection;
  }

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeEndPoint (this, newWidth, newHeight, newLineDirection, b_enableReplaceOfCmd);
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectLine_c::updateLineAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectLine_a(), lineAttributes), sizeof(iVtObjectLine_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectLine_a(), lineAttributes), sizeof(iVtObjectLine_s));
}

uint16_t
vtObjectLine_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectLine_a(), width), sizeof(iVtObjectLine_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectLine_a(), width), sizeof(iVtObjectLine_s));
}

uint16_t
vtObjectLine_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectLine_a(), height), sizeof(iVtObjectLine_s), 3);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectLine_a(), height), sizeof(iVtObjectLine_s));
}

uint8_t
vtObjectLine_c::updateLineDirection(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectLine_a(), lineDirection), sizeof(iVtObjectLine_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectLine_a(), lineDirection), sizeof(iVtObjectLine_s));
}

void
vtObjectLine_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectLine_a(), lineAttributes), sizeof(iVtObjectLine_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectLine_a(), width), sizeof(iVtObjectLine_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue16(MACRO_getStructOffset(get_vtObjectLine_a(), height), sizeof(iVtObjectLine_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectLine_a(), lineDirection), sizeof(iVtObjectLine_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    default: break;
  }
}
#endif


    void vtObjectLine_c::setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c *newValue, bool b_updateObject,
                                           bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLine_a(), lineAttributes) : 0, sizeof(iVtObjectLine_s), 1, (IsoAgLib::iVtObject_c*) newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLine_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLine_a(), width) : 0, sizeof(iVtObjectLine_s), 2, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLine_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLine_a(), height) : 0, sizeof(iVtObjectLine_s), 3, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectLine_c::setLineDirection(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLine_a(), lineDirection) : 0, sizeof(iVtObjectLine_s), 4, newValue, newValue, b_enableReplaceOfCmd);
    }



} // __IsoAgLib


