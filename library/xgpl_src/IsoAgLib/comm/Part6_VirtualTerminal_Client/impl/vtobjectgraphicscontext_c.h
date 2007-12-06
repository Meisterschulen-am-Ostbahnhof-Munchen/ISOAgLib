/***************************************************************************
                          vtobjectgraphicscontext_c.h
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
#ifndef VTOBJECTGRAPHICSCONTEXT_C_H
#define VTOBJECTGRAPHICSCONTEXT_C_H

#include "vtobject_c.h"
#include "isoterminal_c.h"

namespace __IsoAgLib {
//! VT client object for graphics context.
class vtObjectGraphicsContext_c : public vtObject_c
{
public:
  //! ISO related IDs
  enum ID_t {
     //! Object type ID as defined by ISO App.B
     e_objectType               = 36
     //! Command ID as defined by ISO App.F
    ,e_commandID                = 0xB8  // dec. 184
     //! Graphics context sub command ID as in ISO App.F
    ,e_setGraphicsCursorCmdID   = 0
    ,e_setForegroundColourCmdID = 1
    ,e_setBackgroundColourCmdID = 2
    ,e_setLineAttributeCmdID    = 3
    ,e_setFillAttributeCmdID    = 4
    ,e_setFontAttributeCmdID    = 5
    ,e_eraseRectangleCmdID      = 6
    ,e_drawPointCmdID           = 7
    ,e_drawLineCmdID            = 8
    ,e_drawRectangleCmdID       = 9
    ,e_drawClosedEllipseCmdID   = 10
    ,e_drawPolygonCmdID         = 11
    ,e_drawTextCmdID            = 12
    ,e_panViewportCmdID         = 13
    ,e_zoomViewportCmdID        = 14
    ,e_panAndZoomViewportCmdID  = 15
    ,e_changeViewportSizeCmdID  = 16
    ,e_drawVTObjectCmdID        = 17
    ,e_copyCanvasToPictureGraphicCmdID   = 18
    ,e_copyViewportToPictureGraphicCmdID = 19
  };

  //! @param destMemory
  //! @param maxBytes Maximum bytes that can be streamed out without upload-buffer overrun.
  //! @param sourceOffset
  int16_t stream(uint8_t* destMemory, uint16_t maxBytes, objRange_t sourceOffset);

  //  Operation: init
  //! @param vtObjectGraphicsContextSROM:
  //! @param b_initPointer:
  void init(const iVtObjectGraphicsContext_s* vtObjectGraphicsContextSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObject_c::init ((iVtObject_s*) vtObjectGraphicsContextSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectGraphicsContext_a
  inline iVtObjectGraphicsContext_s* get_vtObjectGraphicsContext_a( void ) {
    return (iVtObjectGraphicsContext_s *)&(get_vtObject_a());
  }

  //  Operation: vtObjectGraphicsContext_c
  vtObjectGraphicsContext_c( void );

  //! Give total size of object including header and attributes.
  uint32_t fitTerminal( void ) const { return mi_totalSize; }

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setGraphicsCursor(const IsoAgLib::iVtPoint_c& ac_point,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void setForegroundColour(uint8_t newValue,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void setBackgroundColour(uint8_t newValue,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void setLineAttributes(const IsoAgLib::iVtObjectLineAttributes_c* const newLineAttributes,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void setFillAttributes(const IsoAgLib::iVtObjectFillAttributes_c* const newFillAttributes,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void setFontAttributes(const IsoAgLib::iVtObjectFontAttributes_c* const newFontAttributes,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void eraseRectangle(const IsoAgLib::iVtPoint_c& ac_point,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void drawPoint(
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void drawLine(const IsoAgLib::iVtPoint_c& ac_point,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void drawRectangle(const IsoAgLib::iVtPoint_c& ac_point,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void drawClosedEllipse(const IsoAgLib::iVtPoint_c& ac_point,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void drawPolygon(uint16_t cnt, const IsoAgLib::iVtPoint_c* const aps_data,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void drawText(uint8_t type, uint8_t cnt, const char* apc_string,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void panViewport(const IsoAgLib::iVtPoint_c& ac_point,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void zoomViewport(int8_t newValue,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void panAndZoomViewport(const IsoAgLib::iVtPoint_c& ac_point, int8_t newValue,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void changeViewportSize(uint16_t newWidth, uint16_t newHeight,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void drawVtObject(const IsoAgLib::iVtObject_c* const p_VtObject,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void copyCanvas2PictureGraphic(const IsoAgLib::iVtObjectPictureGraphic_c* const pc_VtObjectPictureGraphic,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void copyViewport2PictureGraphic(const IsoAgLib::iVtObjectPictureGraphic_c* const pc_VtObjectPictureGraphic,
       bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_GETATTRIBUTE
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 36; }
  */

  uint16_t updateViewportWidth(bool b_SendRequest=false);

  uint16_t updateViewportHeight(bool b_SendRequest=false);

  uint16_t updateViewportXPos(bool b_SendRequest=false);

  uint16_t updateViewportYPos(bool b_SendRequest=false);

  uint8_t updateViewPortZoom(bool b_SendRequest=false);

  uint16_t updateCursorXPos(bool b_SendRequest=false);

  uint16_t updateCursorYPos(bool b_SendRequest=false);

  uint8_t updateForegroundColour(bool b_SendRequest=false);

  uint8_t updateBackgroundColour(bool b_SendRequest=false);

  uint16_t updateFontAttributes(bool b_SendRequest=false);

  uint16_t updateLineAttributes(bool b_SendRequest=false);

  uint16_t updateFillAttributes(bool b_SendRequest=false);

  uint8_t updateFormat(bool b_SendRequest=false);

  uint8_t updateOptions(bool b_SendRequest=false);

  uint8_t updateTransparencyColour(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
private:
  //! Total size of Graphics Context attributes.
  static const unsigned mi_attributesSize = (
    sizeof(uint16_t) +
    sizeof(uint16_t) +
    sizeof(int16_t)*2 +
    sizeof(uint16_t) +
    sizeof(uint16_t) +
    sizeof(int8_t) +
    sizeof(int16_t)*2 +
    sizeof(uint8_t) +
    sizeof(uint8_t) +
    sizeof(uint16_t) +
    sizeof(uint16_t) +
    sizeof(uint16_t) +
    sizeof(uint8_t) +
    sizeof(uint8_t) +
    sizeof(uint8_t) );

  //! Total size of Graphics Context object including header
  static const unsigned mi_totalSize = 3+mi_attributesSize;
};

} // namespace __IsoAgLib

#endif // VTOBJECTGRAPHICSCONTEXT_C_H
