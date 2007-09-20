/***************************************************************************
                         ivtobjectpicturegraphic_c.h
                             -------------------
    begin                : Mon Jun 30 14:41:54 2003
    copyright            : (C) 2003 by Martin Wodok
    email                : m.wodok@osb-ag.de
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
#ifndef IVTOBJECTPICTUREGRAPHIC_C_H
#define IVTOBJECTPICTUREGRAPHIC_C_H

#include "impl/vtobjectpicturegraphic_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//!  Stereotype: Klasse
class iVtObjectPictureGraphic_c : public __IsoAgLib::vtObjectPictureGraphic_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectPictureGraphicSROM:
  //! @param b_initPointer:
  void init(const iVtObjectPictureGraphic_s* vtObjectPictureGraphicSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectPictureGraphic_c::init (vtObjectPictureGraphicSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectPictureGraphic_a
  const iVtObjectPictureGraphic_s& get_vtObjectPictureGraphic_a() { return *vtObjectPictureGraphic_c::get_vtObjectPictureGraphic_a(); }

  //  Operation: setWidth
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPictureGraphic_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setOptions
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPictureGraphic_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setTransparencyColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setTransparencyColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPictureGraphic_c::setTransparencyColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setRawData0 (HUGE_MEM uint8_t* newValue, uint32_t rui32_size, bool rb_rle, uint16_t rui16_actWidth=0xFFFF, uint16_t rui16_actHeight=0xFFFF, uint16_t rui16_width=0xFFFF) {
    vtObjectPictureGraphic_c::setRawData0 (newValue, rui32_size, rb_rle, rui16_actWidth, rui16_actHeight, rui16_width);
  }
  void setRawData1 (HUGE_MEM uint8_t* newValue, uint32_t rui32_size, bool rb_rle, uint16_t rui16_actWidth=0xFFFF, uint16_t rui16_actHeight=0xFFFF, uint16_t rui16_width=0xFFFF) {
    vtObjectPictureGraphic_c::setRawData1 (newValue, rui32_size, rb_rle, rui16_actWidth, rui16_actHeight, rui16_width);
  }
  void setRawData2 (HUGE_MEM uint8_t* newValue, uint32_t rui32_size, bool rb_rle, uint16_t rui16_actWidth=0xFFFF, uint16_t rui16_actHeight=0xFFFF, uint16_t rui16_width=0xFFFF) {
    vtObjectPictureGraphic_c::setRawData2 (newValue, rui32_size, rb_rle, rui16_actWidth, rui16_actHeight, rui16_width);
  }

  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectPictureGraphic_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateWidth(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateOptions(b_SendRequest);
  }

  uint8_t updateTransparencyColour(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateTransparencyColour(b_SendRequest);
  }

  /** these attributes are in parentheses in the spec, so commented out here
  uint16_t updateActualWidth(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateActualWidth(b_SendRequest);
  }

  uint16_t updateActualHeight(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateActualHeight(b_SendRequest);
  }

  uint8_t updateFormat(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateFormat(b_SendRequest);
  }
  */
};

} // end of namespace IsoAgLib

#endif
