/*
  vtobjectgraphicscontext_c.cpp

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

#include "vtobjectgraphicscontext_c.h"
#ifdef USE_ISO_TERMINAL_GRAPHICCONTEXT

#include "../ivtobjectpicturegraphic_c.h"
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectfillattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"
#include <IsoAgLib/util/convert.h>
#include <IsoAgLib/util/iassert.h>



namespace __IsoAgLib {

struct vtObjectGraphicsContext_c::iVtObjectGraphicsContext_s: iVtObject_s {
	uint16_t viewportWidth;
	uint16_t viewportHeight;
	int16_t viewportX;       //!< Upper left corner of the viewport
	int16_t viewportY;
	uint16_t canvasWidth;
	uint16_t canvasHeight;
	float viewportZoom;    //!< Zoom value [-32.0F ... 32.0F]
	int16_t cursorX;
	int16_t cursorY;
	IsoAgLib::Colour foregroundColour;
	IsoAgLib::Colour backgroundColour;
	IsoAgLib::iVtObjectFontAttributes_c *fontAttributes;
	IsoAgLib::iVtObjectLineAttributes_c *lineAttributes;
	IsoAgLib::iVtObjectFillAttributes_c *fillAttributes;
	uint8_t format; //!< 0 => 1 bit/pixel, 1 => 4 bit/pixel, 2 => 8 bit/pixel
	uint8_t options; //!< Bit 0 transparent flag; Bit 1 line, font, fill colour usage
	IsoAgLib::Colour transparencyColour;  //!< Colour to be shown transparent.
	iVtObjectGraphicsContext_s(
			IsoAgLib::ObjectID ID,
			uint16_t viewportWidth,
			uint16_t viewportHeight,
			int16_t viewportX,
			int16_t viewportY,
			uint16_t canvasWidth,
			uint16_t canvasHeight,
			float viewportZoom,
			int16_t cursorX,
			int16_t cursorY,
			IsoAgLib::Colour foregroundColour,
			IsoAgLib::Colour backgroundColour,
			IsoAgLib::iVtObjectFontAttributes_c *fontAttributes,
			IsoAgLib::iVtObjectLineAttributes_c *lineAttributes,
			IsoAgLib::iVtObjectFillAttributes_c *fillAttributes,
			uint8_t format,
			uint8_t options,
			IsoAgLib::Colour transparencyColour)
	: iVtObject_s(ID)
	, viewportWidth(viewportWidth)
	, viewportHeight(viewportHeight)
	, viewportX(viewportX)
	, viewportY(viewportY)
	, canvasWidth(canvasWidth)
	, canvasHeight(canvasHeight)
	, viewportZoom(viewportZoom)
	, cursorX(cursorX)
	, cursorY(cursorY)
	, foregroundColour(foregroundColour)
	, backgroundColour(backgroundColour)
	, fontAttributes(fontAttributes)
	, lineAttributes(lineAttributes)
	, fillAttributes(fillAttributes)
	, format(format)
	, options(options)
	, transparencyColour(transparencyColour)
	{
	}
};




int16_t
vtObjectGraphicsContext_c::stream(uint8_t* destMemory, uint16_t maxBytes, objRange_t sourceOffset)
{
  iVtObjectGraphicsContext_s* pc_vtOGC_a = get_vtObjectGraphicsContext_a();

  if (sourceOffset == 0) { // dump out constant sized stuff
    // Check precondition
    // (Not allways because we have no exception handling for 16-bit systems implemented).
    isoaglib_assert(maxBytes >= mi_totalSize);
    (void)maxBytes;

    uint8_t* p = destMemory;
    number2LittleEndianString( uint16_t(vtObject_a->ID), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint8_t(e_objectType), p ); p += sizeof(uint8_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->viewportWidth), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->viewportHeight), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->viewportX), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->viewportY), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->canvasWidth), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->canvasHeight), p ); p += sizeof(uint16_t);
    floatVar2LittleEndianStream( &(pc_vtOGC_a->viewportZoom), p ); p += sizeof(float);

    number2LittleEndianString( uint16_t(pc_vtOGC_a->cursorX), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->cursorY), p ); p += sizeof(uint16_t);
    *(p++) = getVtClientInstance4Comm().getClientByID(s_properties.clientId).getUserConvertedColor(
                                    pc_vtOGC_a->foregroundColour, this, IsoAgLib::Colour);
    *(p++) = getVtClientInstance4Comm().getClientByID(s_properties.clientId).getUserConvertedColor(
                                    pc_vtOGC_a->backgroundColour, this, IsoAgLib::BackgroundColour);

    if (pc_vtOGC_a->fontAttributes)
      number2LittleEndianString( uint16_t(pc_vtOGC_a->fontAttributes->getID()), p );
    else
      number2LittleEndianString( uint16_t(0xFFFF), p );
    p += sizeof(uint16_t);

    if (pc_vtOGC_a->lineAttributes)
      number2LittleEndianString( uint16_t(pc_vtOGC_a->lineAttributes->getID()), p );
    else
      number2LittleEndianString( uint16_t(0xFFFF), p );
    p += sizeof(uint16_t);

    if (pc_vtOGC_a->fillAttributes)
      number2LittleEndianString( uint16_t(pc_vtOGC_a->fillAttributes->getID()), p );
    else
      number2LittleEndianString( uint16_t(0xFFFF), p );
    p += sizeof(uint16_t);

    number2LittleEndianString( uint8_t(pc_vtOGC_a->format), p ); p += sizeof(uint8_t);
    number2LittleEndianString( uint8_t(pc_vtOGC_a->options), p ); p += sizeof(uint8_t);
    *(p++) = getVtClientInstance4Comm().getClientByID(s_properties.clientId).getUserConvertedColor(
                                    pc_vtOGC_a->transparencyColour, this, IsoAgLib::TransparencyColour );

    // Check postcondition
    isoaglib_assert((destMemory + mi_totalSize) == p);

    return mi_totalSize;
  }
  return 0;
}


IsoAgLib::ObjectID vtObjectGraphicsContext_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

vtObjectGraphicsContext_c::vtObjectGraphicsContext_c() {}

vtObjectGraphicsContext_c::vtObjectGraphicsContext_c(iVtObjectGraphicsContext_s* vtObjectGraphicsContextSROM , int ai_multitonInst)
:vtObject_c((iVtObject_s*) vtObjectGraphicsContextSROM , ai_multitonInst)
,vtObject_a(vtObjectGraphicsContextSROM)
{}


iVtObjectGraphicsContext_s* vtObjectGraphicsContext_c::get_vtObjectGraphicsContext_a( void ) {
  return vtObject_a;
}


void
vtObjectGraphicsContext_c::setGraphicsCursor( int16_t ai16_x, int16_t ai16_y,
                                              bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), ai16_x );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), ai16_y );
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandSetGraphicsCursor(
              this, ai16_x, ai16_y, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::setForegroundColour( uint8_t newValue,
                                                bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue8 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), foregroundColour),
      sizeof(iVtObjectGraphicsContext_s), newValue);
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandSetForegroundColour(
              this, newValue, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::setBackgroundColour( uint8_t newValue,
                                                bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue8 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), backgroundColour),
      sizeof(iVtObjectGraphicsContext_s), newValue);
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandSetBackgroundColour(
              this, newValue, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::setLineAttributes( const IsoAgLib::iVtObjectLineAttributes_c* const newLineAttributes,
                                              bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValueP (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), lineAttributes),
      sizeof(iVtObjectGraphicsContext_s), newLineAttributes);
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandSetGCLineAttributes(
              this, newLineAttributes, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::setFillAttributes( const IsoAgLib::iVtObjectFillAttributes_c* const newFillAttributes,
                                              bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValueP (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), fillAttributes),
      sizeof(iVtObjectGraphicsContext_s), newFillAttributes);
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandSetGCFillAttributes(
              this, newFillAttributes, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::setFontAttributes( const IsoAgLib::iVtObjectFontAttributes_c* const newFontAttributes,
                                              bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValueP (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), fontAttributes),
      sizeof(iVtObjectGraphicsContext_s), newFontAttributes);
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandSetGCFontAttributes(
              this, newFontAttributes, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::eraseRectangle( int16_t ai16_x, int16_t ai16_y,
                                           bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), ai16_x );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), ai16_y );
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandEraseRectangle(
              this, ai16_x, ai16_y, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawPoint( bool /*b_updateObject*/, bool b_enableReplaceOfCmd)
{
  // No change of object => b_updateObject ignored.

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandDrawPoint(
              this, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawLine( int16_t ai16_x, int16_t ai16_y,
                                     bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), ai16_x );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), ai16_y );
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandDrawLine(
              this, ai16_x, ai16_y, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawRectangle( int16_t ai16_x, int16_t ai16_y,
                                          bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), ai16_x );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), ai16_y );
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandDrawRectangle(
              this, ai16_x, ai16_y, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawClosedEllipse( int16_t ai16_x, int16_t ai16_y,
                                              bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), ai16_x );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), ai16_y );
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandDrawClosedEllipse(
              this, ai16_x, ai16_y, b_enableReplaceOfCmd);
}

//! You need to supply valid pointers for api16_x/api16_y - no checks are done in here...
void
vtObjectGraphicsContext_c::drawPolygon( uint16_t cnt, const int16_t* api16_x, const int16_t* api16_y,
                                        bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), api16_x[cnt-1] );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), api16_y[cnt-1] );
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandDrawPolygon(
              this, cnt, api16_x, api16_y, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawText( uint8_t type, uint8_t cnt, const char* apc_string,
                                     bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    uint16_t ui16_widthHeight = get_vtObjectGraphicsContext_a()->fontAttributes->getScaledWidthHeight();
    uint16_t ui16_totalHeight = ui16_widthHeight & 0xFF;
    uint16_t ui16_totalWidth = uint16_t(ui16_widthHeight >> 8) * uint16_t(cnt);

    int16_t i16_x = get_vtObjectGraphicsContext_a()->cursorX - 1 + ui16_totalWidth;
    int16_t i16_y = get_vtObjectGraphicsContext_a()->cursorY - 1 + ui16_totalHeight;

    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), i16_x );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), i16_y );
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandDrawText(
              this, type, cnt, apc_string, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::panViewport( int16_t ai16_x, int16_t ai16_y,
                                        bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportX),
      sizeof(iVtObjectGraphicsContext_s), ai16_x );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportY),
      sizeof(iVtObjectGraphicsContext_s), ai16_y );
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandPanViewport(
              this, ai16_x, ai16_y, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::zoomViewport( float newValue,
                                         bool b_updateObject, bool b_enableReplaceOfCmd)
{
  // Check precondition as defined in ISO
  if ((-32.0F > newValue) || (newValue > 32.0F)) { return; }

  if (b_updateObject) {
    saveValueFloat (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportZoom),
      sizeof(iVtObjectGraphicsContext_s), newValue);
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandZoomViewport(
              this, newValue, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::panAndZoomViewport( int16_t ai16_x, int16_t ai16_y, float newValue,
                                               bool b_updateObject, bool b_enableReplaceOfCmd)
{
  // Check precondition as defined in ISO
  if ((-32.0F > newValue) || (newValue > 32.0F)) { return; }

  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportX),
      sizeof(iVtObjectGraphicsContext_s), ai16_x );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportY),
      sizeof(iVtObjectGraphicsContext_s), ai16_y );
    saveValueFloat (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportZoom),
      sizeof(iVtObjectGraphicsContext_s), newValue);
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandPanAndZoomViewport(
              this, ai16_x, ai16_y, newValue, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::changeViewportSize( uint16_t newWidth, uint16_t newHeight,
                                               bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportWidth),
      sizeof(iVtObjectGraphicsContext_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportHeight),
      sizeof(iVtObjectGraphicsContext_s), newHeight);
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandChangeViewportSize(
              this, newWidth, newHeight, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawVtObject( const iVtObject_c* const newVtObject,
                                         bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    //! @todo ON REQUEST Revision4 The object size is not know and thus the cursor is not moved for b_updateObject case.
    uint16_t ui16_totalHeight = 1;
    uint16_t ui16_totalWidth = 1;

    int16_t i16_x = get_vtObjectGraphicsContext_a()->cursorX - 1 + ui16_totalWidth;
    int16_t i16_y = get_vtObjectGraphicsContext_a()->cursorY - 1 + ui16_totalHeight;

    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), i16_x );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), i16_y );
  }

  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandDrawVtObject(
              this, newVtObject, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::copyCanvas2PictureGraphic( const IsoAgLib::iVtObjectPictureGraphic_c* const pc_iVtObjectPictureGraphic,
                                                      bool /*b_updateObject*/, bool b_enableReplaceOfCmd)
{
  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandCopyCanvas2PictureGraphic(
              this, pc_iVtObjectPictureGraphic, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::copyViewport2PictureGraphic( const IsoAgLib::iVtObjectPictureGraphic_c* const pc_iVtObjectPictureGraphic,
                                                        bool /*b_updateObject*/, bool b_enableReplaceOfCmd)
{
  getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandCopyViewport2PictureGraphic(
              this, pc_iVtObjectPictureGraphic, b_enableReplaceOfCmd);
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectGraphicsContext_c::updateViewportWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportWidth), sizeof(iVtObjectGraphicsContext_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportWidth), sizeof(iVtObjectGraphicsContext_s));
}

uint16_t
vtObjectGraphicsContext_c::updateViewportHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportHeight), sizeof(iVtObjectGraphicsContext_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportHeight), sizeof(iVtObjectGraphicsContext_s));
}

uint16_t
vtObjectGraphicsContext_c::updateViewportXPos(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportX), sizeof(iVtObjectGraphicsContext_s), 3);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportX), sizeof(iVtObjectGraphicsContext_s));
}

uint16_t
vtObjectGraphicsContext_c::updateViewportYPos(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportY), sizeof(iVtObjectGraphicsContext_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportY), sizeof(iVtObjectGraphicsContext_s));
}

uint8_t
vtObjectGraphicsContext_c::updateViewPortZoom(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValueFloatGetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportZoom), sizeof(iVtObjectGraphicsContext_s), 5);
  else
    return getValueFloat(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportZoom), sizeof(iVtObjectGraphicsContext_s));
}

uint16_t
vtObjectGraphicsContext_c::updateCursorXPos(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX), sizeof(iVtObjectGraphicsContext_s), 6);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY), sizeof(iVtObjectGraphicsContext_s));
}

uint16_t
vtObjectGraphicsContext_c::updateCursorYPos(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportY), sizeof(iVtObjectGraphicsContext_s), 7);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportY), sizeof(iVtObjectGraphicsContext_s));
}

uint8_t vtObjectGraphicsContext_c::updateForegroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), foregroundColour), sizeof(iVtObjectGraphicsContext_s), 8);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), foregroundColour), sizeof(iVtObjectGraphicsContext_s));
}

uint8_t
vtObjectGraphicsContext_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), backgroundColour), sizeof(iVtObjectGraphicsContext_s), 9);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), backgroundColour), sizeof(iVtObjectGraphicsContext_s));
}

uint16_t
vtObjectGraphicsContext_c::updateFontAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), fontAttributes), sizeof(iVtObjectGraphicsContext_s), 10);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), fontAttributes), sizeof(iVtObjectGraphicsContext_s));
}

uint16_t
vtObjectGraphicsContext_c::updateLineAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), lineAttributes), sizeof(iVtObjectGraphicsContext_s), 11);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), lineAttributes), sizeof(iVtObjectGraphicsContext_s));
}

uint16_t
vtObjectGraphicsContext_c::updateFillAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), fillAttributes), sizeof(iVtObjectGraphicsContext_s), 12);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), fillAttributes), sizeof(iVtObjectGraphicsContext_s));
}

uint8_t
vtObjectGraphicsContext_c::updateFormat(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), format), sizeof(iVtObjectGraphicsContext_s), 13);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), format), sizeof(iVtObjectGraphicsContext_s));
}

uint8_t
vtObjectGraphicsContext_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), options), sizeof(iVtObjectGraphicsContext_s), 14);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), options), sizeof(iVtObjectGraphicsContext_s));
}

uint8_t
vtObjectGraphicsContext_c::updateTransparencyColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), transparencyColour), sizeof(iVtObjectGraphicsContext_s), 15);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), transparencyColour), sizeof(iVtObjectGraphicsContext_s));
}

void
vtObjectGraphicsContext_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportWidth), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportHeight), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportX), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 4: saveValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportY), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 5: saveValueFloat(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportZoom), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringFloat(pui8_attributeValue)); break;
    case 6: saveValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 7: saveValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportY), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 8: saveValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), foregroundColour), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 9: saveValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), backgroundColour), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 10: saveValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), fontAttributes), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 11: saveValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), lineAttributes), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 12: saveValue16(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), fillAttributes), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 13: saveValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), format), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 14: saveValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), options), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 15: saveValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), transparencyColour), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    default: break;
  }
}

#endif

} // __IsoAgLib

#endif
