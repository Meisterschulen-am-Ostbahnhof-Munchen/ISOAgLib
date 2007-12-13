/***************************************************************************
                         ivtobjectgraphicscontext_c.h
                             -------------------
    begin                : Mon Jun 30 14:41:54 2003
    copyright            : (C) 2006 by Martina Winkler
    email                : m.winkler@osb-ag.de
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
 * the main author Achim Spangler by a.spangler@osb-ag.de                  *
 ***************************************************************************/
#ifndef IVTOBJECTGRAPHICSCONTEXT_C_H
#define IVTOBJECTGRAPHICSCONTEXT_C_H

#include "impl/vtobjectgraphicscontext_c.h"

#if not defined PRJ_ISO_TERMINAL_OBJECT_SELECTION1 || defined USE_VTOBJECT_graphicscontext

namespace IsoAgLib {

class iVtObjectGraphicsContext_c : public __IsoAgLib::vtObjectGraphicsContext_c
{
public:
  //  Operation: init
  //! @param vtObjectGraphicsContextSROM
  //! @param b_initPointer
  void init(const iVtObjectGraphicsContext_s* vtObjectGraphicsContextSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectGraphicsContext_c::init (vtObjectGraphicsContextSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  };

  //  Operation: get_vtObjectGraphicsContext_a
  const iVtObjectGraphicsContext_s& get_vtObjectGraphicsContext_a() {
    return *vtObjectGraphicsContext_c::get_vtObjectGraphicsContext_a();
  };

  //! This command alters the graphics cursor X/Y attributes of the object (Sub-Command ID 0).
  //! @param ac_point Position of cursor to be set
  //! @param b_updateObject Keep track of object changes (by use of RAM copy instead of static ROM image).
  //! @param b_enableReplaceOfCmd Allow command queue optimization by replacing allready queued commands
  //!   by currently received.
  void setGraphicsCursor(const iVtPoint_c& ac_point,
                         bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::setGraphicsCursor( ac_point, b_updateObject, b_enableReplaceOfCmd);
  }

  //! This command modifies the foreground colour attribute (Sub-Command ID 1).
  //! The graphics cursor is not moved.
  //! @param newValue Colour value to be set.
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void setForegroundColour(uint8_t newValue,
                           bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::setForegroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //! This command modifies the background colour attribute (Sub-Command ID 2).
  //! The graphics cursor is not moved.
  //! @param newValue Colour value to be set.
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void setBackgroundColour(uint8_t newValue,
                           bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //! This command modifies the line object attribute (Sub-Command ID 3).
  //! All drawing commands that follow use the new attribute value.
  //! For line suppression, set the object ID to 65535.
  //! The graphics cursor is not moved.
  //! @param newLineAttributes
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void setLineAttributes(const iVtObjectLineAttributes_c* const newLineAttributes,
                         bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::setLineAttributes (newLineAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  //! This command modifies the fill object attribute (Sub-Command ID 4).
  //! All drawing commands that follow use the new attribute value.
  //! For no filling, set the object ID to 65535.
  //! The graphics cursor is not moved.
  //! @param newFillAttributes
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void setFillAttributes(const iVtObjectFillAttributes_c* const newFillAttributes,
                         bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::setFillAttributes (newFillAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  //! This command modifies the font object attribute (Sub-Command ID 5).
  //! All drawing commands that follow use the new attribute value.
  //! If text is not being used, the object ID can be set to 65535.
  //! The graphics cursor is not moved.
  //! @param newFontAttributes
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void setFontAttributes(const iVtObjectFontAttributes_c* const newFontAttributes,
                         bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::setFontAttributes (newFontAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  //! Fills the rectangle from the graphics cursor to the defined end pixel
  //! using the current background colour (Sub-Command ID 6).
  //! For this command, the Fill Attributes Object is not used regardless of
  //! the state of Options bit 1.
  //! The graphics cursor is moved to the defined end pixel.
  //! @param ac_point End pixel position
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void eraseRectangle(const iVtPoint_c& ac_point,
                      bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::eraseRectangle( ac_point, b_updateObject, b_enableReplaceOfCmd);
  }

  //! Sets the pixel at the graphics cursor to the foreground colour (Sub-Command ID 7).
  //! The graphics cursor is not moved.
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void drawPoint(bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::drawPoint( b_updateObject, b_enableReplaceOfCmd);
  }

  //! Draws a line from the graphics cursor to the defined end pixel using the foreground colour (Sub-Command ID 8).
  //! The Line Object drawing rules apply with respect to the end pixel location and line attributes.
  //! The graphics cursor is moved to the defined end pixel.
  //! @param ac_point End pixel position
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void drawLine(const iVtPoint_c& ac_point,
                bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::drawLine( ac_point, b_updateObject, b_enableReplaceOfCmd);
  }

  //! Draws a rectangle from the graphics cursor to the defined end pixel (Sub-Command ID 9).
  //! The Rectangle Object drawing rules apply.
  //! If a line attribute object is currently defined, the border is drawn.
  //! If a fill attribute object is currently defined, the rectangle is filled.
  //! The graphics cursor is moved to the defined end pixel.
  //! @param ac_point End pixel position
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void drawRectangle(const iVtPoint_c& ac_point,
                     bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::drawRectangle( ac_point, b_updateObject, b_enableReplaceOfCmd);
  }

  //! Draws a closed ellipse bounded by the rectangle defined by the current graphics cursor
  //! location and the end pixel given (Sub-Command ID 10).
  //! The Ellipse Object drawing rules apply.
  //! If a line attribute object is currently defined, the border is drawn.
  //! If a fill attribute object is currently defined, the circle is filled.
  //! The graphics cursor is moved to the defined end pixel.
  //! @param ac_point End pixel position
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void drawClosedEllipse(const iVtPoint_c& ac_point,
                         bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::drawClosedEllipse( ac_point, b_updateObject, b_enableReplaceOfCmd);
  }

  //! Draws a polygon from the graphics cursor to the first point,
  //! then to the second point and so on (Sub-Command ID 11).
  //! If the data does not close the polygon, no automatic closing is performed and filling is ignored.
  //! Foreground colour is used for the border colour.
  //! The Polygon Object drawing rules apply.
  //! If a line attribute object is currently defined, the border is drawn.
  //! If a fill object is currently defined and the polygon is closed, the polygon is filled.
  //! The graphics cursor is moved to the last point in the list.
  //! @param cnt Number of additional points
  //! @param aps_data Pointer to array of polygon points
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void drawPolygon(uint16_t cnt, const iVtPoint_c* const apc_data,
                   bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::drawPolygon (cnt, apc_data, b_updateObject, b_enableReplaceOfCmd);
  }

  //! Draws the given text using the font attribute object (Sub-Command ID 12).
  //! Any flashing bits in the Font style of the font attribute object are ignored.
  //! If opaque (type == 1), the background colour attribute is used.
  //! The graphics cursor is moved to the bottom right corner of the extent of the text.
  //! @param type Type of text background drawing (0 == opaque, 1 == transparent).
  //! @param cnt Number of characters to be drawn
  //! @param apc_string Pointer to character array to be drawn
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void drawText(uint8_t type, uint8_t cnt, const char* apc_string,
                bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::drawText( type, cnt, apc_string, b_updateObject, b_enableReplaceOfCmd);
  }

  //! This command modifies the viewport X and Y attributes and forces a redraw of the object (Sub-Command ID 13).
  //! This allows "panning" of the underlying object contents.
  //! The graphics cursor is not moved.
  //! @param ac_point Viewport position
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void panViewport(const iVtPoint_c& ac_point,
                   bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::panViewport( ac_point, b_updateObject, b_enableReplaceOfCmd);
  }

  //! This command allows magnification of the viewport contents (Sub-Command ID 14).
  //! See "ISO11783 part 6 Graphics Context" section on zooming for meaning of the zoom value.
  //! The graphics cursor is not moved.
  //! @param newValue Zoom value
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void zoomViewport(int8_t newValue,
                    bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::zoomViewport( newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //! @todo REQUEST (to be done when GCO gets used..) Coordinates need to be signed.
  //! This command allows both panning and zooming at the same time
  //! by combining commands 13 and 14 (Sub-Command ID 15).
  //! @param ac_point Viewport position
  //! @param newValue Zoom value
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void panAndZoomViewport(const iVtPoint_c& ac_point, int8_t newValue,
                          bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::panAndZoomViewport( ac_point, newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //! This command changes the size of the viewport and can be compared to
  //! the normal Change Size command (Sub-Command ID 15).
  //! Note that the size of the object (i.e. the memory used) cannot be changed.
  //! The graphics cursor is not moved.
  //! @param newWidth
  //! @param newHeight
  //! @param b_updateObject:
  //! @param b_enableReplaceOfCmd
  void changeViewportSize(uint16_t newWidth, uint16_t newHeight,
                          bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::changeViewportSize (newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  //! This command draws the VT Object at the current graphics cursor location (top left corner)(Sub-Command ID 16).
  //! Any drawable object may be specified with the exception of the Graphics Context
  //! object itselve or any object that contains this Graphics Context object
  //! (circular references are not allowed).
  //! The object shall be drawn using the current value and state of that object at the time
  //! the command was specified (for instance, enabled or disabled).
  //! However, a focus indicator shall not be drawn even if the specified object (or any
  //! child object) has focus at that time.
  //! Also, if the object is being edited by the user, it shall be drawn as if it is not
  //! being edited, using the last accepted value of the object (not a temporary value
  //! that the user is still entering). The graphics cursor is moved to the bottom right
  //! corner of the object that was drawn.  Normal VT Object transparency rules apply
  //! when drawing the VT Object onto the canvas.
  //! Any colours outside of the colours allowed by this Graphics Context Object shall be treated as transparent.
  //! @todo REQUEST (to be done when Graphics Context Object gets used) The object size is not know and thus the cursor is not moved for b_updateObject case.
  //! @param iVtObject Pointer to the VT Object to be drawn
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void drawVtObject(const iVtObject_c* const iVtObject,
                    bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::drawVtObject( iVtObject, b_updateObject, b_enableReplaceOfCmd);
  }

  //! This command copies the current canvas of the Graphics Context Object into the
  //! Picture Graphic Object (Sub-Command ID 17).
  //! If the Picture Graphic is smaller than the canvas,
  //! then it shall be clipped to fit within the Picture Graphic.
  //! If the Picture Graphic is larger than the canvas, then the extra pixels in the
  //! Picture Graphic are not changed.
  //! Colours in the Canvas that are set to the transparency colour in the Graphics
  //! Context Object are not copied and the corresponding pixels in the Picture Graphic
  //! are not changed.
  //! The picture graphic must have at least the same number of colours as the Graphics
  //! Context Object.
  //! Any colours outside of the colours allowed by this Picture Graphic Object shall
  //! be treated as transparent.
  //! @param iVtObjectPictureGraphic Picture Graphic Object to copy canvas to.
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void copyCanvas2PictureGraphic(const iVtObjectPictureGraphic_c* const iVtObjectPictureGraphic,
                                 bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::copyCanvas2PictureGraphic (iVtObjectPictureGraphic, b_updateObject, b_enableReplaceOfCmd);
  }

  //! This command copies the current Viewport (zoomed or panned) of the Graphics
  //! Context Object into the Picture Graphic Object (Sub-Command ID 18).
  //! If the Picture Graphic is smaller than the Viewport, then it shall be clipped
  //! to fit within the Picture Graphic.
  //! If the Picture Graphic is larger than the Viewport, then the extra pixels in
  //! the Picture Graphic are not changed.
  //! Colours in the Viewport that are set to the transparency colour in the Graphics
  //! Context Object are not copied and the corresponding pixels in the Picture Graphic
  //! are not changed.
  //! The picture graphic must have at least the same number of colours as the Graphics
  //! Context Object.
  //! Any colours outside of the colours allowed by this Picture Graphic Object shall
  //! be treated as transparent.
  //! @param iVtObjectPictureGraphicPicture Graphic Object to copy viewport to.
  //! @param b_updateObject
  //! @param b_enableReplaceOfCmd
  void copyViewport2PictureGraphic(const iVtObjectPictureGraphic_c* const iVtObjectPictureGraphic,
                                   bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectGraphicsContext_c::copyViewport2PictureGraphic (iVtObjectPictureGraphic, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectGraphicsContext_c::updateObjectType(); }
  */

  uint16_t updateViewportWidth(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateViewportWidth(b_SendRequest);
  }

  uint16_t updateViewportHeight(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateViewportHeight(b_SendRequest);
  }

  uint16_t updateViewportXPos(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateViewportXPos(b_SendRequest);
  }

  uint16_t updateViewportYPos(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateViewportYPos(b_SendRequest);
  }

  uint8_t updateViewPortZoom(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateViewPortZoom(b_SendRequest);
  }

  uint16_t updateCursorXPos(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateCursorXPos(b_SendRequest);
  }

  uint16_t updateCursorYPos(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateCursorYPos(b_SendRequest);
  }

  uint8_t updateForegroundColour(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateForegroundColour(b_SendRequest);
  }

  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateBackgroundColour(b_SendRequest);
  }

  uint16_t updateFontAttributes(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateFontAttributes(b_SendRequest);
  }

  uint16_t updateLineAttributes(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateLineAttributes(b_SendRequest);
  }

  uint16_t updateFillAttributes(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateFillAttributes(b_SendRequest);
  }

  uint8_t updateFormat(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateFormat(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateOptions(b_SendRequest);
  }

  uint8_t updateTransparencyColour(bool b_SendRequest=false) {
    return vtObjectGraphicsContext_c::updateTransparencyColour(b_SendRequest);
  }
#endif
}; // class iVtObjectGraphicsContext_c

} // namespace IsoAgLib

#endif // IVTOBJECTGRAPHICSCONTEXT_C_H
#endif
