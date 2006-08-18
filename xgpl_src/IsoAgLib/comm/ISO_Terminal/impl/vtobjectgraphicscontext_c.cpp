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

#include "../ivtobjectpicturegraphic_c.h"
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectfillattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "vtobjectgraphicscontext_c.h"
#include "isoterminal_c.h"
#include <IsoAgLib/convert.h>

// Make shure not such macro is used.
#undef MACRO_vtObjectTypeA
#undef MACRO_vtObjectTypeS

namespace __IsoAgLib {

//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectGraphicsContext_c::stream(uint8_t* destMemory, uint16_t maxBytes, objRange_t sourceOffset)
{
  iVtObjectGraphicsContext_s* pc_vtOGC_a = get_vtObjectGraphicsContext_a();

  if (sourceOffset == 0) { // dump out constant sized stuff
    // Check precondition
#ifdef SYSTEM_PC && DEBUG
    if (maxBytes < i_totalSize) { abort(); }
#endif

    uint8_t* p = destMemory;
    *(p++) = convert_n::getByte( vtObject_a->ID,             0 );
    *(p++) = convert_n::getByte( vtObject_a->ID,             1 );
    *(p++) = u_objectType;
    *(p++) = convert_n::getByte( pc_vtOGC_a->viewportWidth,  0 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->viewportWidth,  1 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->viewportHeight, 0 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->viewportHeight, 1 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->viewportX,      0 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->viewportX,      1 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->viewportY,      0 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->viewportY,      1 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->canvasWidth,    0 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->canvasWidth,    1 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->canvasHeight,   0 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->canvasHeight,   1 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->viewportZoom,   0 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->cursorX,        0 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->cursorX,        1 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->cursorY,        0 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->cursorY,        1 );
    *(p++) = getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).getUserClippedColor(
                                    pc_vtOGC_a->foregroundColour, this, IsoAgLib::Colour);
    *(p++) = getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).getUserClippedColor(
                                    pc_vtOGC_a->backgroundColour, this, IsoAgLib::BackgroundColour);
    *(p++) = (pc_vtOGC_a->fontAttributes)? (pc_vtOGC_a->fontAttributes->getID() & 0xFF) : 0xFF;
    *(p++) = (pc_vtOGC_a->fontAttributes)? (pc_vtOGC_a->fontAttributes->getID() >> 8) : 0xFF;
    *(p++) = (pc_vtOGC_a->lineAttributes)? (pc_vtOGC_a->lineAttributes->getID() & 0xFF) : 0xFF;
    *(p++) = (pc_vtOGC_a->lineAttributes)? (pc_vtOGC_a->lineAttributes->getID() >> 8) : 0xFF;
    *(p++) = (pc_vtOGC_a->fillAttributes)? (pc_vtOGC_a->fillAttributes->getID() & 0xFF) : 0xFF;
    *(p++) = (pc_vtOGC_a->fillAttributes)? (pc_vtOGC_a->fillAttributes->getID() >> 8) : 0xFF;
    *(p++) = convert_n::getByte( pc_vtOGC_a->format,         0 );
    *(p++) = convert_n::getByte( pc_vtOGC_a->options,        0 );
    *(p++) = getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).getUserClippedColor(
                                    pc_vtOGC_a->transparencyColour, this, IsoAgLib::TransparencyColour );

    // Check postcondition
#ifdef SYSTEM_PC && DEBUG
    if ((destMemory+i_totalSize) != p) { abort(); }
#endif

    return i_totalSize;
  }
  return 0;
}

// //////////////////////////////// +X2C Operation 153 : vtObjectGraphicsContext_c
vtObjectGraphicsContext_c::vtObjectGraphicsContext_c()
{
}

void
vtObjectGraphicsContext_c::setGraphicsCursor( const IsoAgLib::iVtPoint_c& rc_point,
                                              bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX), 
      sizeof(iVtObjectGraphicsContext_s), rc_point.getX() );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getY() );
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandSetGraphicsCursor(
              this, rc_point, b_enableReplaceOfCmd);
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
vtObjectGraphicsContext_c::eraseRectangle( const IsoAgLib::iVtPoint_c& rc_point,
                                           bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getX() );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getY() );
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandEraseRectangle(
              this, rc_point, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawPoint( bool /*b_updateObject*/, bool b_enableReplaceOfCmd)
{
  // No change of object => b_updateObject ignored.

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawPoint(
              this, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawLine( const IsoAgLib::iVtPoint_c& rc_point,
                                     bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getX() );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getY() );
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawLine(
              this, rc_point, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawRectangle( const IsoAgLib::iVtPoint_c& rc_point,
                                          bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getX() );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getY() );
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawRectangle(
              this, rc_point, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawClosedEllipse( const IsoAgLib::iVtPoint_c& rc_point,
                                              bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getX() );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getY() );
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawClosedEllipse(
              this, rc_point, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawPolygon( uint16_t cnt, const IsoAgLib::iVtPoint_c* const rc_point,
                                        bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorX),
      sizeof(iVtObjectGraphicsContext_s), rc_point[cnt-1].getX() );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), cursorY),
      sizeof(iVtObjectGraphicsContext_s), rc_point[cnt-1].getY() );
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawPolygon(
              this, cnt, rc_point, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawText( uint8_t type, uint8_t cnt, const char* rpc_string,
                                     bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    /** @todo set the cursor to the bottom right corner of the last character */
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandDrawText (
              this, type, cnt, rpc_string, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::panViewport( const IsoAgLib::iVtPoint_c& rc_point,
                                        bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportX),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getX() );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportY),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getY() );
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandPanViewPort(
              this, rc_point, b_enableReplaceOfCmd);
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandZoomViewPort(
              this, newValue, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::panAndZoomViewport( const IsoAgLib::iVtPoint_c& rc_point, int8_t newValue,
                                               bool b_updateObject, bool b_enableReplaceOfCmd)
{
  // Check precondition as defined in ISO
  if ((-32 > newValue) || (newValue > 32)) { return; }

  if (b_updateObject) {
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportX),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getX() );
    saveSignedValue16 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportY),
      sizeof(iVtObjectGraphicsContext_s), rc_point.getY() );
    saveSignedValue8 (MACRO_getStructOffset(get_vtObjectGraphicsContext_a(), viewportZoom),
      sizeof(iVtObjectGraphicsContext_s), newValue);
  }

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandPanViewPort(
              this, rc_point, b_enableReplaceOfCmd);
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

  getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).sendCommandChangeViewPortSize(
              this, newWidth, newHeight, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawVtObject( const iVtObject_c* const newVtObject,
                                         bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    /** @todo set the cursor to the bottom right corner of drawn object */
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

} // namespace __IsoAgLib

