/***************************************************************************
                          vtobjectgraphicscontext_c.cpp
                             -------------------
    begin                : Mon Jun 30 14:41:54 2003
    copyright            : (C) 2006 by Martina Winkler
    email                : m.winkler@osb-ag:de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#include "vtobjectgraphicscontext_c.h"

#include "../ivtobjectpicturegraphic_c.h"
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectfillattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "isoterminal_c.h"
#include <IsoAgLib/util/convert.h>

// Make sure not such macro is used.
#undef MACRO_vtObjectTypeA
#undef MACRO_vtObjectTypeS

namespace __IsoAgLib {
//! @param destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
//! @todo REQUEST Not data mask scaling (done by client) supported for graphics context.
int16_t
vtObjectGraphicsContext_c::stream(uint8_t* destMemory, uint16_t maxBytes, objRange_t sourceOffset)
{
  iVtObjectGraphicsContext_s* pc_vtOGC_a = get_vtObjectGraphicsContext_a();

  if (sourceOffset == 0) { // dump out constant sized stuff
    // Check precondition
    // (Not allways because we have no exception handling for 16-bit systems implemented).
#if defined(DEBUG) && defined(SYSTEM_PC)
    if (maxBytes < mi_totalSize) { abort(); }
#else
    maxBytes = maxBytes;  // Prevent from warning.
#endif

    uint8_t* p = destMemory;
    number2LittleEndianString( uint16_t(vtObject_a->ID), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint8_t(e_objectType), p ); p += sizeof(uint8_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->viewportWidth), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->viewportHeight), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->viewportX), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->viewportY), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->canvasWidth), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->canvasHeight), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint8_t(pc_vtOGC_a->viewportZoom), p ); p += sizeof(uint8_t);

    number2LittleEndianString( uint16_t(pc_vtOGC_a->cursorX), p ); p += sizeof(uint16_t);
    number2LittleEndianString( uint16_t(pc_vtOGC_a->cursorY), p ); p += sizeof(uint16_t);
    *(p++) = getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).getUserClippedColor(
                                    pc_vtOGC_a->foregroundColour, this, IsoAgLib::Colour);
    *(p++) = getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).getUserClippedColor(
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
    *(p++) = getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).getUserClippedColor(
                                    pc_vtOGC_a->transparencyColour, this, IsoAgLib::TransparencyColour );

    // Check postcondition
    // (Not allways because we have no exception handling for 16-bit systems implemented).
#if defined(DEBUG) && defined(SYSTEM_PC)
    if ((destMemory+mi_totalSize) != p) { abort(); }
#endif

    return mi_totalSize;
  }
  return 0;
}

vtObjectGraphicsContext_c::vtObjectGraphicsContext_c() {}

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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandSetGraphicsCursor(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandSetForegroundColour(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandSetBackgroundColour(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandSetGCLineAttributes(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandSetGCFillAttributes(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandSetGCFontAttributes(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandEraseRectangle(
              this, ai16_x, ai16_y, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawPoint( bool /*b_updateObject*/, bool b_enableReplaceOfCmd)
{
  // No change of object => b_updateObject ignored.

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawPoint(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawLine(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawRectangle(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawClosedEllipse(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawPolygon(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawText(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandPanViewport(
              this, ai16_x, ai16_y, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::zoomViewport( int8_t newValue,
                                         bool b_updateObject, bool b_enableReplaceOfCmd)
{
  // Check precondition as defined in ISO
  if ((-32 > newValue) || (newValue > 32)) { return; }

  if (b_updateObject) {
    saveSignedValue8 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportZoom),
      sizeof(iVtObjectGraphicsContext_s), newValue);
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandZoomViewport(
              this, newValue, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::panAndZoomViewport( int16_t ai16_x, int16_t ai16_y, int8_t newValue,
                                               bool b_updateObject, bool b_enableReplaceOfCmd)
{
  // Check precondition as defined in ISO
  if ((-32 > newValue) || (newValue > 32)) { return; }

  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportX),
      sizeof(iVtObjectGraphicsContext_s), ai16_x );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportY),
      sizeof(iVtObjectGraphicsContext_s), ai16_y );
    saveSignedValue8 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportZoom),
      sizeof(iVtObjectGraphicsContext_s), newValue);
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandPanAndZoomViewport(
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandChangeViewportSize(
              this, newWidth, newHeight, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawVtObject( const iVtObject_c* const newVtObject,
                                         bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    //! @todo REQUEST The object size is not know and thus the cursor is not moved for b_updateObject case.
    uint16_t ui16_totalHeight = 1;
    uint16_t ui16_totalWidth = 1;

    int16_t i16_x = get_vtObjectGraphicsContext_a()->cursorX - 1 + ui16_totalWidth;
    int16_t i16_y = get_vtObjectGraphicsContext_a()->cursorY - 1 + ui16_totalHeight;

    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), i16_x );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), i16_y );
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawVtObject(
              this, newVtObject, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::copyCanvas2PictureGraphic( const IsoAgLib::iVtObjectPictureGraphic_c* const pc_iVtObjectPictureGraphic,
                                                      bool /*b_updateObject*/, bool b_enableReplaceOfCmd)
{
  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandCopyCanvas2PictureGraphic(
              this, pc_iVtObjectPictureGraphic, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::copyViewport2PictureGraphic( const IsoAgLib::iVtObjectPictureGraphic_c* const pc_iVtObjectPictureGraphic,
                                                        bool /*b_updateObject*/, bool b_enableReplaceOfCmd)
{
  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandCopyViewport2PictureGraphic(
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
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportZoom), sizeof(iVtObjectGraphicsContext_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportZoom), sizeof(iVtObjectGraphicsContext_s));
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
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportZoom), sizeof(iVtObjectGraphicsContext_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
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
} // namespace __IsoAgLib
