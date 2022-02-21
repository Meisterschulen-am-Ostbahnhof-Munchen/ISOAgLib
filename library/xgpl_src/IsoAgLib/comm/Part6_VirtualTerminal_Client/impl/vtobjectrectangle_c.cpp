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

#ifdef CONFIG_USE_VTOBJECT_rectangle
#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectfillattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


enum vtObjectRectangle_c::AttributeID:uint8_t
{
	LineAttributes   = 1,
	Width            = 2,
	Height           = 3,
	aLineSuppression = 4,
	FillAttributes   = 5,
};


struct vtObjectRectangle_c::iVtObjectRectangle_s: iVtObjectwMacro_s {
	IsoAgLib::iVtObjectLineAttributes_c *lineAttributes;
	uint16_t width;
	uint16_t height;
	IsoAgLib::LineSuppression lineSuppression;
	IsoAgLib::iVtObjectFillAttributes_c *fillAttributes;
	iVtObjectRectangle_s(
			IsoAgLib::ObjectID ID,
			IsoAgLib::iVtObjectLineAttributes_c *lineAttributes,
			uint16_t width,
			uint16_t height,
			IsoAgLib::LineSuppression lineSuppression,
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

uint16_t
vtObjectRectangle_c::stream(uint8_t* destMemory,
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
      destMemory [2] = VT_OBJECT_TYPE_RECTANGLE; // Object Type = Rectangle
      destMemory [3] = vtObject_a->lineAttributes->getID() & 0xFF;
      destMemory [4] = vtObject_a->lineAttributes->getID() >> 8;
      destMemory [5] = width & 0xFF;
      destMemory [6] = width >> 8;
      destMemory [7] = height & 0xFF;
      destMemory [8] = height >> 8;
      destMemory [9] = vtObject_a->lineSuppression.lineSuppression;

    if (vtObject_a->fillAttributes != NULL)
    {
        destMemory [10] = vtObject_a->fillAttributes->getID() & 0xFF;
        destMemory [11] = vtObject_a->fillAttributes->getID() >> 8;
      } else {
        destMemory [10] = 0xFF;
        destMemory [11] = 0xFF;
      }

      destMemory [12] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 13;
      curBytes += 13;
    }

    MACRO_streamEventMacro(13);
    return curBytes;
}


IsoAgLib::ObjectID vtObjectRectangle_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectRectangle_c::fitTerminal() const
{
  return 13+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectRectangle_c::setOriginSKM(bool b_SKM)
{
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    vtObject_a->lineAttributes->setOriginSKM (b_SKM);
  }
}

void
vtObjectRectangle_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	  vtObject_a->width = newWidth;
	  vtObject_a->height = newHeight;
  }

  scaleSize( newWidth, newHeight );

  getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
IsoAgLib::iVtObjectLineAttributes_c *
vtObjectRectangle_c::updateLineAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(LineAttributes);
  return vtObject_a->lineAttributes;
}

uint16_t
vtObjectRectangle_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(Width);
  return vtObject_a->width;
}

uint16_t
vtObjectRectangle_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(Height);
  return vtObject_a->height;
}

IsoAgLib::LineSuppression
vtObjectRectangle_c::updateLineSuppression(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(aLineSuppression);
  return vtObject_a->lineSuppression;
}

IsoAgLib::iVtObjectFillAttributes_c *
vtObjectRectangle_c::updateFillAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(FillAttributes);
  return vtObject_a->fillAttributes;
}

void
vtObjectRectangle_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    //case LineAttributes:  vtObject_a->lineAttributes  = convertLittleEndianStringUi16(pui8_attributeValue); break;
    case Width:           vtObject_a->width           = convertLittleEndianStringUi16(pui8_attributeValue); break;
    case Height:          vtObject_a->height          = convertLittleEndianStringUi16(pui8_attributeValue); break;
    case aLineSuppression: vtObject_a->lineSuppression.lineSuppression = (IsoAgLib::LineSuppression_e)convertLittleEndianStringUi8( pui8_attributeValue); break;
    //case FillAttributes:  vtObject_a->fillAttributes  = convertLittleEndianStringUi16(pui8_attributeValue); break;
    default: break;
  }
}
#endif
    vtObjectRectangle_c::vtObjectRectangle_c(iVtObjectRectangle_s *vtObjectRectangleSROM,
                                             multiton ai_multitonInst)
            :vtObject_c(ai_multitonInst)
    		,vtObject_a(vtObjectRectangleSROM)
    {}



    void
    vtObjectRectangle_c::setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c *newLineAttributes, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->lineAttributes = newLineAttributes;
    	setAttribute ( LineAttributes /* "Line Attribute" */, newLineAttributes->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectRectangle_c::setWidth(uint16_t newWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
        MACRO_scaleLocalVars
        MACRO_scaleSKLocalVars

        uint32_t scaledDim = uint32_t( newWidth );
      #ifndef USE_VT_CLIENT_OLD_UNSCALED_SIZE_COMMANDS
        MACRO_scaleDimension( scaledDim )
      #endif

    	if (b_updateObject)
    		vtObject_a->height = newWidth;
        setAttribute ( Width /* "Width" */, scaledDim, b_enableReplaceOfCmd);
    }

    void vtObjectRectangle_c::setHeight(uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        MACRO_scaleLocalVars
        MACRO_scaleSKLocalVars

        uint32_t scaledDim = uint32_t( newHeight );
      #ifndef USE_VT_CLIENT_OLD_UNSCALED_SIZE_COMMANDS
        MACRO_scaleDimension( scaledDim )
      #endif

    	if (b_updateObject)
    		vtObject_a->height = newHeight;
    	setAttribute ( Height /* "Height" */, scaledDim, b_enableReplaceOfCmd);
    }

    void vtObjectRectangle_c::setLineSuppression(IsoAgLib::LineSuppression  newLineSupressionValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->lineSuppression = newLineSupressionValue;
    	setAttribute ( aLineSuppression /* "Line Suppression" */, newLineSupressionValue.lineSuppression, b_enableReplaceOfCmd);
    }

    void
    vtObjectRectangle_c::setFillAttributes(IsoAgLib::iVtObjectFillAttributes_c *newFillAttributes, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fillAttributes = newFillAttributes;
    	setAttribute ( FillAttributes /* "Fill Attributes" */, newFillAttributes->getID(), b_enableReplaceOfCmd);
    }

    vtObjectRectangle_c::vtObjectRectangle_c(
            multiton ai_multitonInst,
            IsoAgLib::ObjectID ID,
            IsoAgLib::iVtObjectLineAttributes_c *lineAttributes,
            uint16_t width,
            uint16_t height,
            IsoAgLib::LineSuppression lineSuppression,
            IsoAgLib::iVtObjectFillAttributes_c *fillAttributes)
	: vtObjectRectangle_c(
		new iVtObjectRectangle_s(
	        ID,
			lineAttributes,
			width,
			height,
			lineSuppression,
			fillAttributes),
			ai_multitonInst)
	{
	}

    vtObjectRectangle_c::~vtObjectRectangle_c() = default;


} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_rectangle
