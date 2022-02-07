/*
  vtobjectrectangle_c.cpp

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

#include "vtobjectrectangle_c.h"


#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectfillattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


struct vtObjectRectangle_c::iVtObjectRectangle_s: iVtObjectwMacro_s {
	IsoAgLib::iVtObjectLineAttributes_c *lineAttributes;
	uint16_t width;
	uint16_t height;
	uint8_t lineSuppression;
	IsoAgLib::iVtObjectFillAttributes_c *fillAttributes;
	iVtObjectRectangle_s(
			IsoAgLib::ObjectID ID,
			IsoAgLib::iVtObjectLineAttributes_c *lineAttributes,
			uint16_t width,
			uint16_t height,
			uint8_t lineSuppression,
			IsoAgLib::iVtObjectFillAttributes_c *fillAttributes)
    : iVtObject_s(ID)
	, iVtObjectwMacro_s(ID)
	, lineAttributes(lineAttributes)
	, width(width)
	, height(height)
	, lineSuppression(lineSuppression)
	, fillAttributes(fillAttributes)
	{
	}


};

int16_t
vtObjectRectangle_c::stream(uint8_t* destMemory,
                            uint16_t maxBytes,
                            objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectRectangle_a
#define MACRO_vtObjectTypeS iVtObjectRectangle_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObjectRectangle_a->width;
    uint32_t height = (uint32_t)vtObjectRectangle_a->height;
    MACRO_scaleSizeI32(width, height);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 14; // Object Type = Rectangle
      destMemory [3] = vtObjectRectangle_a->lineAttributes->getID() & 0xFF;
      destMemory [4] = vtObjectRectangle_a->lineAttributes->getID() >> 8;
      destMemory [5] = width & 0xFF;
      destMemory [6] = width >> 8;
      destMemory [7] = height & 0xFF;
      destMemory [8] = height >> 8;
      destMemory [9] = vtObjectRectangle_a->lineSuppression;

    if (vtObjectRectangle_a->fillAttributes != NULL)
    {
        destMemory [10] = vtObjectRectangle_a->fillAttributes->getID() & 0xFF;
        destMemory [11] = vtObjectRectangle_a->fillAttributes->getID() >> 8;
      } else {
        destMemory [10] = 0xFF;
        destMemory [11] = 0xFF;
      }

      destMemory [12] = vtObjectRectangle_a->numberOfMacrosToFollow;
      sourceOffset += 13;
      curBytes += 13;
    }

    MACRO_streamEventMacro(13);
    return curBytes;
}





uint32_t
vtObjectRectangle_c::fitTerminal() const
{
  MACRO_localVars;
  return 13+vtObjectRectangle_a->numberOfMacrosToFollow*2;
}


void
vtObjectRectangle_c::setOriginSKM(bool b_SKM)
{
  MACRO_localVars;
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    vtObjectRectangle_a->lineAttributes->setOriginSKM (b_SKM);
  }
}

void
vtObjectRectangle_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectRectangle_a(), width),  sizeof(iVtObjectRectangle_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectRectangle_a(), height), sizeof(iVtObjectRectangle_s), newHeight);
  }

  scaleSize( newWidth, newHeight );

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectRectangle_c::updateLineAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineAttributes), sizeof(iVtObjectRectangle_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineAttributes), sizeof(iVtObjectRectangle_s));
}

uint16_t
vtObjectRectangle_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectRectangle_a(), width), sizeof(iVtObjectRectangle_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), width), sizeof(iVtObjectRectangle_s));
}

uint16_t
vtObjectRectangle_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectRectangle_a(), height), sizeof(iVtObjectRectangle_s), 3);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), height), sizeof(iVtObjectRectangle_s));
}

uint8_t
vtObjectRectangle_c::updateLineSuppression(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineSuppression), sizeof(iVtObjectRectangle_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineSuppression), sizeof(iVtObjectRectangle_s));
}

uint16_t
vtObjectRectangle_c::updateFillAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectRectangle_a(), fillAttributes), sizeof(iVtObjectRectangle_s), 5);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), fillAttributes), sizeof(iVtObjectRectangle_s));
}

void
vtObjectRectangle_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineAttributes), sizeof(iVtObjectRectangle_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), width), sizeof(iVtObjectRectangle_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), height), sizeof(iVtObjectRectangle_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineSuppression), sizeof(iVtObjectRectangle_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 5: saveValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), fillAttributes), sizeof(iVtObjectRectangle_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    default: break;
  }
}
#endif
    vtObjectRectangle_c::vtObjectRectangle_c(iVtObjectRectangle_s *vtObjectRectangleSROM,
                                             int ai_multitonInst)
            :vtObject_c((iVtObject_s*) vtObjectRectangleSROM , ai_multitonInst)
    		,vtObject_a(vtObjectRectangleSROM)
    {}

    vtObjectRectangle_c::iVtObjectRectangle_s *vtObjectRectangle_c::get_vtObjectRectangle_a()
    {
    	return vtObject_a;
    }

    void
    vtObjectRectangle_c::setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c *newLineAttributes, bool b_updateObject,
                                           bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectRectangle_a(), lineAttributes) : 0, sizeof(iVtObjectRectangle_s), 1 /* "Line Attribute" */, (IsoAgLib::iVtObject_c*) newLineAttributes, b_enableReplaceOfCmd);
    }

    void vtObjectRectangle_c::setWidth(uint16_t newWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectRectangle_a(), width) : 0, sizeof(iVtObjectRectangle_s), 2 /* "Width" */, newWidth, b_enableReplaceOfCmd);
    }

    void vtObjectRectangle_c::setHeight(uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectRectangle_a(), height) : 0, sizeof(iVtObjectRectangle_s), 3 /* "Height" */, newHeight, b_enableReplaceOfCmd);
    }

    void vtObjectRectangle_c::setLineSuppression(uint8_t newLineSupressionValue, bool b_updateObject,
                                                 bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectRectangle_a(), lineSuppression) : 0, sizeof(iVtObjectRectangle_s), 4 /* "Line Suppression" */, newLineSupressionValue, newLineSupressionValue, b_enableReplaceOfCmd);
    }

    void
    vtObjectRectangle_c::setFillAttributes(IsoAgLib::iVtObjectFillAttributes_c *newFillAttributes, bool b_updateObject,
                                           bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectRectangle_a(), fillAttributes) : 0, sizeof(iVtObjectRectangle_s), 5 /* "Fill Attributes" */, (IsoAgLib::iVtObject_c*) newFillAttributes, b_enableReplaceOfCmd);
    }


} // __IsoAgLib


