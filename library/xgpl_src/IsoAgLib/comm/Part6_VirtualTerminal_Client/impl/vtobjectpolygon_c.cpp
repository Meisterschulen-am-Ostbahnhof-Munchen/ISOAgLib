/*
  vtobjectpolygon_c.cpp

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

#include "vtobjectpolygon_c.h"


#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectfillattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


struct repeat_x_y_s {
  int16_t x;
  int16_t y;
};


struct vtObjectPolygon_c::iVtObjectPolygon_s: iVtObjectwMacro_s {
	uint16_t width;
	uint16_t height;
	IsoAgLib::iVtObjectLineAttributes_c *lineAttributes;
	IsoAgLib::iVtObjectFillAttributes_c *fillAttributes;
	uint8_t polygonType;
	uint8_t numberOfPoints;
	const repeat_x_y_s *pointsToFollow;
	iVtObjectPolygon_s(
			IsoAgLib::ObjectID ID,
			uint16_t width,
			uint16_t height,
			IsoAgLib::iVtObjectLineAttributes_c *lineAttributes,
			IsoAgLib::iVtObjectFillAttributes_c *fillAttributes,
			uint8_t polygonType,
			uint8_t numberOfPoints,
			const repeat_x_y_s *pointsToFollow)
    : iVtObject_s(ID)
	, iVtObjectwMacro_s(ID)
	, width(width)
	, height(height)
	, lineAttributes(lineAttributes)
	, fillAttributes(fillAttributes)
	, polygonType(polygonType)
	, numberOfPoints(numberOfPoints)
	, pointsToFollow(pointsToFollow)
	{
	}


};


int16_t
vtObjectPolygon_c::stream(uint8_t* destMemory,
                          uint16_t maxBytes,
                          objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectPolygon_a
#define MACRO_vtObjectTypeS iVtObjectPolygon_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObjectPolygon_a->width;
    uint32_t height = (uint32_t)vtObjectPolygon_a->height;
    MACRO_scaleSizeI32(width, height);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 16; // Object Type = Polygon
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = vtObjectPolygon_a->lineAttributes->getID() & 0xFF;
      destMemory [8] = vtObjectPolygon_a->lineAttributes->getID() >> 8;

      if (vtObjectPolygon_a->fillAttributes != NULL) {
        destMemory [9] = vtObjectPolygon_a->fillAttributes->getID() & 0xFF;
        destMemory [10] = vtObjectPolygon_a->fillAttributes->getID() >> 8;
      } else {
        destMemory [9] = 0xFF;
        destMemory [10] = 0xFF;
      }

      destMemory [11] = vtObjectPolygon_a->polygonType;
      destMemory [12] = vtObjectPolygon_a->numberOfPoints;
      destMemory [13] = vtObjectPolygon_a->numberOfMacrosToFollow;
      sourceOffset += 14;
      curBytes += 14;
    }
    MACRO_streamPolygonPoints(14);

    MACRO_streamEventMacro(14U+vtObjectPolygon_a->numberOfPoints * 4U);
    return curBytes;
}





uint32_t
vtObjectPolygon_c::fitTerminal() const
{
  MACRO_localVars;
  return 14+vtObjectPolygon_a->numberOfPoints*4+vtObjectPolygon_a->numberOfMacrosToFollow*2;
}


void
vtObjectPolygon_c::setOriginSKM(bool b_SKM)
{
  MACRO_localVars;
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    vtObjectPolygon_a->lineAttributes->setOriginSKM (b_SKM);
  }
}

void
vtObjectPolygon_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectPolygon_a(), width),  sizeof(iVtObjectPolygon_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectPolygon_a(), height), sizeof(iVtObjectPolygon_s), newHeight);
  }

  scaleSize( newWidth, newHeight );

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectPolygon_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectPolygon_a(), width), sizeof(iVtObjectPolygon_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectPolygon_a(), width), sizeof(iVtObjectPolygon_s));
}

uint16_t
vtObjectPolygon_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectPolygon_a(), height), sizeof(iVtObjectPolygon_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectPolygon_a(), height), sizeof(iVtObjectPolygon_s));
}

uint16_t
vtObjectPolygon_c::updateLineAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectPolygon_a(), lineAttributes), sizeof(iVtObjectPolygon_s), 3);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectPolygon_a(), lineAttributes), sizeof(iVtObjectPolygon_s));
}

uint16_t
vtObjectPolygon_c::updateFillAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectPolygon_a(), fillAttributes), sizeof(iVtObjectPolygon_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectPolygon_a(), fillAttributes), sizeof(iVtObjectPolygon_s));
}

uint8_t
vtObjectPolygon_c::updatePolygonType(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectPolygon_a(), polygonType), sizeof(iVtObjectPolygon_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectPolygon_a(), polygonType), sizeof(iVtObjectPolygon_s));
}

void
vtObjectPolygon_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectPolygon_a(), width), sizeof(iVtObjectPolygon_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectPolygon_a(), height), sizeof(iVtObjectPolygon_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue16(MACRO_getStructOffset(get_vtObjectPolygon_a(), lineAttributes), sizeof(iVtObjectPolygon_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 4: saveValue16(MACRO_getStructOffset(get_vtObjectPolygon_a(), fillAttributes), sizeof(iVtObjectPolygon_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectPolygon_a(), polygonType), sizeof(iVtObjectPolygon_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    default: break;
  }
}
#endif
    vtObjectPolygon_c::vtObjectPolygon_c(iVtObjectPolygon_s *vtObjectPolygonSROM,
                                         int ai_multitonInst)
            :vtObject_c((iVtObject_s*) vtObjectPolygonSROM , ai_multitonInst)
    		,vtObject_a(vtObjectPolygonSROM)
    {}

    void vtObjectPolygon_c::setWidth(uint16_t newWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectPolygon_a(), width) : 0, sizeof(iVtObjectPolygon_s), 1 /* "Width" */, newWidth, b_enableReplaceOfCmd);
    }

    void vtObjectPolygon_c::setHeight(uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectPolygon_a(), height) : 0, sizeof(iVtObjectPolygon_s), 2 /* "Height" */, newHeight, b_enableReplaceOfCmd);
    }

    void
    vtObjectPolygon_c::setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c *newLineAttributes, bool b_updateObject,
                                         bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectPolygon_a(), lineAttributes) : 0, sizeof(iVtObjectPolygon_s), 3 /* "Line Attribute" */, (IsoAgLib::iVtObject_c*) newLineAttributes, b_enableReplaceOfCmd);
    }

    vtObjectPolygon_c::iVtObjectPolygon_s *vtObjectPolygon_c::get_vtObjectPolygon_a()
    {
    	return vtObject_a;
    }

    void
    vtObjectPolygon_c::setFillAttributes(IsoAgLib::iVtObjectFillAttributes_c *newFillAttributes, bool b_updateObject,
                                         bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectPolygon_a(), fillAttributes) : 0, sizeof(iVtObjectPolygon_s), 4 /* "Fill Attribute" */, (IsoAgLib::iVtObject_c*) newFillAttributes, b_enableReplaceOfCmd);
    }

    void vtObjectPolygon_c::setPolygonType(uint8_t newPolygonType, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectPolygon_a(), polygonType) : 0, sizeof(iVtObjectPolygon_s), 5 /* "Polygon Type" */, newPolygonType, newPolygonType, b_enableReplaceOfCmd);
    }


} // __IsoAgLib


