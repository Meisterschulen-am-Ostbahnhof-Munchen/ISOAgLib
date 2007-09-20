/***************************************************************************
                             ivtobjectbutton_c.h
                             -------------------
    begin                : Don Sep 4 2003
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

#ifndef IVTOBJECTBUTTON_C_H
#define IVTOBJECTBUTTON_C_H


// +X2C includes
#include "impl/vtobjectbutton_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 98 : iVtObjectButton_c
//!  Stereotype: Klasse
class iVtObjectButton_c : public __IsoAgLib::vtObjectButton_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectButtonSROM:
  //! @param b_initPointer:
  void init(const iVtObjectButton_s* vtObjectButtonSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectButton_c::init (vtObjectButtonSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectButton_a
  const iVtObjectButton_s& get_vtObjectButton_a();




  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setSize (newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setWidth
  //! Parameter:
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setBackgroundColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setBorderColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setBorderColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setBorderColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setKeyCode
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setKeyCode(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setKeyCode (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef REVISION3_CLIENT
  //  Operation: setOptions
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }
#endif

  //  Operation: moveChildLocation
  //! Parameter:
  //! @param rpc_childObject:
  //! @param dx:
  //! @param dy:
  bool moveChildLocation(IsoAgLib::iVtObject_c* rpc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectButton_c::moveChildLocation(rpc_childObject, dx, dy, b_updateObject);
  }

  //  Operation: setChildPosition
  //! @param rpc_childObject:
  //! @param dx:
  //! @param dy:
  bool setChildPosition(IsoAgLib::iVtObject_c* rpc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectButton_c::setChildPosition(rpc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }

  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectButton_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectButton_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectButton_c::updateHeight(b_SendRequest);
  }

  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectButton_c::updateBackgroundColour(b_SendRequest);
  }

  uint8_t updateBorderColour(bool b_SendRequest=false) {
    return vtObjectButton_c::updateBorderColour(b_SendRequest);
  }

  uint8_t updateKeyCode(bool b_SendRequest=false) {
    return vtObjectButton_c::updateKeyCode(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectButton_c::updateOptions(b_SendRequest);
  }
};

} // end of namespace IsoAgLib

#endif
