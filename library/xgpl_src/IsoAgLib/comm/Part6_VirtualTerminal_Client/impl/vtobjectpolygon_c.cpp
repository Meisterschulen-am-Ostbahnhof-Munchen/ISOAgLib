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

#ifdef CONFIG_USE_VTOBJECT_polygon
#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectfillattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


enum vtObjectPolygon_c::AttributeID:uint8_t
{
	Width                   = 1,
	Height                  = 2,
	LineAttributes          = 3,
	FillAttributes          = 4,
	aPolygonType            = 5,
};



struct vtObjectPolygon_c::iVtObjectPolygon_s: iVtObjectwMacro_s {
	uint16_t width;
	uint16_t height;
	IsoAgLib::iVtObjectLineAttributes_c* lineAttributes;
	IsoAgLib::iVtObjectFillAttributes_c* fillAttributes;
	IsoAgLib::PolygonType polygonType;
	uint8_t numberOfPoints;
	const IsoAgLib::repeat_x_y_s *pointsToFollow;
	iVtObjectPolygon_s(
			IsoAgLib::ObjectID ID,
			uint16_t width,
			uint16_t height,
			IsoAgLib::iVtObjectLineAttributes_c* lineAttributes,
			IsoAgLib::iVtObjectFillAttributes_c* fillAttributes,
			IsoAgLib::PolygonType polygonType,
			uint8_t numberOfPoints,
			const IsoAgLib::repeat_x_y_s *pointsToFollow);


};

    vtObjectPolygon_c::iVtObjectPolygon_s::iVtObjectPolygon_s(
            IsoAgLib::ObjectID ID,
            uint16_t width,
            uint16_t height,
            IsoAgLib::iVtObjectLineAttributes_c *lineAttributes,
            IsoAgLib::iVtObjectFillAttributes_c *fillAttributes,
            IsoAgLib::PolygonType polygonType,
            uint8_t numberOfPoints,
            const IsoAgLib::repeat_x_y_s *pointsToFollow)
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


    uint16_t
vtObjectPolygon_c::stream(uint8_t* destMemory,
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
      destMemory [2] = VT_OBJECT_TYPE_POLYGON; // Object Type = Polygon
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = vtObject_a->lineAttributes->getID() & 0xFF;
      destMemory [8] = vtObject_a->lineAttributes->getID() >> 8;

      if (vtObject_a->fillAttributes != nullptr) {
        destMemory [9] = vtObject_a->fillAttributes->getID() & 0xFF;
        destMemory [10] = vtObject_a->fillAttributes->getID() >> 8;
      } else {
        destMemory [9] = 0xFF;
        destMemory [10] = 0xFF;
      }

      destMemory [11] = vtObject_a->polygonType;
      destMemory [12] = vtObject_a->numberOfPoints;
      destMemory [13] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 14;
      curBytes += 14;
    }
    MACRO_streamPolygonPoints(14);

#ifdef CONFIG_POLYGON_DEBUG
	#include "esp_log.h"
    static const char * const TAG = "PolygonPoints";


#endif

    MACRO_streamEventMacro(14U+vtObject_a->numberOfPoints * 4U);
    return curBytes;
}



IsoAgLib::ObjectID vtObjectPolygon_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

uint32_t
vtObjectPolygon_c::fitTerminal() const
{
  return 14+vtObject_a->numberOfPoints*4+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectPolygon_c::setOriginSKM(bool b_SKM)
{
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    vtObject_a->lineAttributes->setOriginSKM (b_SKM);
  }
}

void
vtObjectPolygon_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
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
vtObjectPolygon_c::updateWidth(bool b_SendRequest)
{
	  if (b_SendRequest)
	    getAttribute(Width);
	  return vtObject_a->width;
}

uint16_t
vtObjectPolygon_c::updateHeight(bool b_SendRequest)
{
	  if (b_SendRequest)
	    getAttribute(Height);
	  return vtObject_a->height;
}

IsoAgLib::iVtObjectLineAttributes_c*
vtObjectPolygon_c::updateLineAttributes(bool b_SendRequest)
{
	  if (b_SendRequest)
	    getAttribute(LineAttributes);
	  return vtObject_a->lineAttributes;
}

IsoAgLib::iVtObjectFillAttributes_c*
vtObjectPolygon_c::updateFillAttributes(bool b_SendRequest)
{
	  if (b_SendRequest)
	    getAttribute(FillAttributes);
	  return vtObject_a->fillAttributes;
}

IsoAgLib::PolygonType
vtObjectPolygon_c::updatePolygonType(bool b_SendRequest)
{
	  if (b_SendRequest)
	    getAttribute(aPolygonType);
	  return vtObject_a->polygonType;
}
void
vtObjectPolygon_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case Width:          vtObject_a->width          = convertLittleEndianStringUi16(pui8_attributeValue); break;
    case Height:         vtObject_a->height         = convertLittleEndianStringUi16(pui8_attributeValue); break;
    //case LineAttributes: vtObject_a->lineAttributes = convertLittleEndianStringUi16(pui8_attributeValue); break; //TODO
    //case FillAttributes: vtObject_a->fillAttributes = convertLittleEndianStringUi16(pui8_attributeValue); break; //TODO
    case aPolygonType:    vtObject_a->polygonType    = (IsoAgLib::PolygonType)convertLittleEndianStringUi8(pui8_attributeValue); break;
    default: break;
  }
}
#endif
    vtObjectPolygon_c::vtObjectPolygon_c(iVtObjectPolygon_s *vtObjectPolygonSROM,
                                         multiton ai_multitonInst)
            :vtObject_c(ai_multitonInst)
    		,vtObject_a(vtObjectPolygonSROM)
    {}

    void vtObjectPolygon_c::setWidth(uint16_t newWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->width = newWidth;
    	setAttribute(Width /* "Width" */, newWidth, b_enableReplaceOfCmd);
    }

    void vtObjectPolygon_c::setHeight(uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->height = newHeight;
    	setAttribute(Height /* "Height" */, newHeight, b_enableReplaceOfCmd);
    }

    void
    vtObjectPolygon_c::setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c *newLineAttributes, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->lineAttributes = newLineAttributes;
    	setAttribute(LineAttributes /* "Line Attributes" */, newLineAttributes->getID(), b_enableReplaceOfCmd);
    }



    void
    vtObjectPolygon_c::setFillAttributes(IsoAgLib::iVtObjectFillAttributes_c *newFillAttributes, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fillAttributes = newFillAttributes;
    	setAttribute(FillAttributes /* "Fill Attributes" */, newFillAttributes->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectPolygon_c::setPolygonType(IsoAgLib::PolygonType newPolygonType, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->polygonType = newPolygonType;
    	setAttribute( aPolygonType /* "Polygon Type" */, newPolygonType, b_enableReplaceOfCmd);
    }

    vtObjectPolygon_c::vtObjectPolygon_c(
            multiton ai_multitonInst,
			IsoAgLib::ObjectID ID,
			uint16_t width, uint16_t height,
			IsoAgLib::iVtObjectLineAttributes_c *lineAttributes,
			IsoAgLib::iVtObjectFillAttributes_c *fillAttributes,
			IsoAgLib::PolygonType polygonType,
			uint8_t numberOfPoints,
			const IsoAgLib::repeat_x_y_s *pointsToFollow)
    :vtObjectPolygon_c(
    		new iVtObjectPolygon_s(
    				ID,
    				width,
    				height,
    				lineAttributes,
    				fillAttributes,
    				polygonType,
    				numberOfPoints,
    				pointsToFollow),
				ai_multitonInst)
    {

    }

    vtObjectPolygon_c::~vtObjectPolygon_c() = default;


} // __IsoAgLib

#endif
