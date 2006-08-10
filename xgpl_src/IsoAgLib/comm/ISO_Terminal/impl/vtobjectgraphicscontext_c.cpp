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
#include "vtobjectgraphicscontext_c.h"
#include "isoterminal_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// //////////////////////////////// +X2C Operation 150 : stream
//! Parameter:
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectGraphicsContext_c::stream(uint8_t* destMemory, uint16_t /*maxBytes*/, objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectGraphicsContext_a
#define MACRO_vtObjectTypeS iVtObjectGraphicsContext_s
    MACRO_streamLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      uint16_t ui16_tmp;
      destMemory [0]  = vtObject_a->ID & 0xFF;
      destMemory [1]  = vtObject_a->ID >> 8;
      destMemory [2]  = 54; //!< @todo Not defined in ISO. Need to adjust. Object Type = Graphics Context
      destMemory [3]  = vtObjectGraphicsContext_a->viewportWidth & 0xFF;
      destMemory [4]  = vtObjectGraphicsContext_a->viewportWidth >> 8;
      destMemory [5]  = vtObjectGraphicsContext_a->viewportHeight & 0xFF;
      destMemory [6]  = vtObjectGraphicsContext_a->viewportHeight >> 8;
      ui16_tmp = vtConvert_n::unsignValue16( vtObjectGraphicsContext_a->viewportX );
      destMemory [7]  = ui16_tmp & 0xFF;
      destMemory [8]  = ui16_tmp >> 8;
      ui16_tmp = vtConvert_n::unsignValue16( vtObjectGraphicsContext_a->viewportY );
      destMemory [9]  = ui16_tmp & 0xFF;
      destMemory [10] = ui16_tmp >> 8;
      destMemory [11] = vtObjectGraphicsContext_a->canvasWidth & 0xFF;
      destMemory [12] = vtObjectGraphicsContext_a->canvasWidth >> 8;
      destMemory [13] = vtObjectGraphicsContext_a->canvasHeight & 0xFF;
      destMemory [14] = vtObjectGraphicsContext_a->canvasHeight >> 8;
      destMemory [15] = vtConvert_n::unsignValue8( vtObjectGraphicsContext_a->viewportZoom );
      ui16_tmp = vtConvert_n::unsignValue16( vtObjectGraphicsContext_a->cursorX );
      destMemory [16]  = ui16_tmp & 0xFF;
      destMemory [17]  = ui16_tmp >> 8;
      ui16_tmp = vtConvert_n::unsignValue16( vtObjectGraphicsContext_a->cursorY );
      destMemory [18]  = ui16_tmp & 0xFF;
      destMemory [19]  = ui16_tmp >> 8;
      destMemory [20] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).getUserClippedColor(
                                    vtObjectGraphicsContext_a->foregroundColour, this, IsoAgLib::Colour);
      destMemory [21] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).getUserClippedColor(
                                    vtObjectGraphicsContext_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [22] = (vtObjectGraphicsContext_a->fontAttributes)? (vtObjectGraphicsContext_a->fontAttributes->getID() & 0xFF) : 0xFF;
      destMemory [23] = (vtObjectGraphicsContext_a->fontAttributes)? (vtObjectGraphicsContext_a->fontAttributes->getID() >> 8) : 0xFF;
      destMemory [24] = (vtObjectGraphicsContext_a->lineAttributes)? (vtObjectGraphicsContext_a->lineAttributes->getID() & 0xFF) : 0xFF;
      destMemory [25] = (vtObjectGraphicsContext_a->lineAttributes)? (vtObjectGraphicsContext_a->lineAttributes->getID() >> 8) : 0xFF;
      destMemory [26] = (vtObjectGraphicsContext_a->fillAttributes)? (vtObjectGraphicsContext_a->fillAttributes->getID() & 0xFF) : 0xFF;
      destMemory [27] = (vtObjectGraphicsContext_a->fillAttributes)? (vtObjectGraphicsContext_a->fillAttributes->getID() >> 8) : 0xFF;
      destMemory [28] = vtObjectGraphicsContext_a->format;
      destMemory [29] = vtObjectGraphicsContext_a->options;
      destMemory [30] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID(s_properties.clientId).getUserClippedColor(
                                    vtObjectGraphicsContext_a->transparencyColour, this, IsoAgLib::TransparencyColour);

      sourceOffset += 31;
      curBytes     += 31;
    }

    return curBytes;
}

// //////////////////////////////// +X2C Operation 153 : vtObjectGraphicsContext_c
vtObjectGraphicsContext_c::vtObjectGraphicsContext_c()
{ // ~X2C
} // -X2C

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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandSetGraphicsCursor(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandSetForegroundColour(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandSetBackgroundColour(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandSetGCLineAttributes(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandSetGCFillAttributes(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandSetGCFontAttributes(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandEraseRectangle(
              this, rc_point, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawPoint( bool /*b_updateObject*/, bool b_enableReplaceOfCmd)
{
  // No change of object => b_updateObject ignored.

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandDrawPoint(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandDrawLine(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandDrawRectangle(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandDrawClosedEllipse(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandDrawPolygon(
              this, cnt, rc_point, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawText( uint8_t type, uint8_t cnt, const char* rpc_string,
                                     bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    /** @todo set the cursor to the bottom right corner of the last character */
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandDrawText (
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandPanViewPort(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandZoomViewPort(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandPanViewPort(
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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeViewPortSize(
              this, newWidth, newHeight, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::drawVtObject( const iVtObject_c* const newVtObject,
                                         bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    /** @todo set the cursor to the bottom right corner of drawn object */
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandDrawVtObject(
              this, newVtObject, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::copyCanvas2PictureGraphic( const IsoAgLib::iVtObjectPictureGraphic_c* const pc_VtObjectPictureGraphic,
                                                      bool /*b_updateObject*/, bool b_enableReplaceOfCmd)
{
  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandCopyCanvas2PictureGraphic(
              this, pc_VtObjectPictureGraphic, b_enableReplaceOfCmd);
}

void
vtObjectGraphicsContext_c::copyViewport2PictureGraphic( const IsoAgLib::iVtObjectPictureGraphic_c* const pc_VtObjectPictureGraphic,
                                                        bool /*b_updateObject*/, bool b_enableReplaceOfCmd)
{
  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandCopyViewport2PictureGraphic(
              this, pc_VtObjectPictureGraphic, b_enableReplaceOfCmd);
}

} // end of namespace __IsoAgLib

