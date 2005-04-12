/***************************************************************************
                            ivtobjectellipse_c.h
                             -------------------
    begin                : Don Sep 4 2003
    copyright            : (C) 2003-2004 by Martin Wodok / Bradforx Cox
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


#ifndef IVTOBJECTELLIPSE_C_H
#define IVTOBJECTELLIPSE_C_H


// +X2C includes
#include "impl/vtobjectellipse_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 119 : vtObjectellipse_c
//!  Stereotype: Klasse
class iVtObjectEllipse_c : public __IsoAgLib::vtObjectEllipse_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectellipseSROM:
  //! @param b_initPointer:
  void init(const iVtObjectEllipse_s* vtObjectellipseSROM) {
    vtObjectEllipse_c::init (vtObjectellipseSROM);
  };

  //  Operation: get_vtObjectmeter_a
  iVtObjectEllipse_s* get_vtObjectEllipse_a() { return vtObjectEllipse_c::get_vtObjectEllipse_a(); };

  //  Operation: setLineAttributes
  //! Parameter:
  //! @param newLineAttributes:
  //! @param b_updateObject:
  void setLineAttributes(iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false) {
    vtObjectEllipse_c::setLineAttributes (newLineAttributes, b_updateObject);
  };

  //  Operation: setWidth
  //! Parameter:
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newWidth, bool b_updateObject=false) {
    vtObjectEllipse_c::setWidth (newWidth, b_updateObject);
  };

  //  Operation: setHeight
  //! Parameter:
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newHeight, bool b_updateObject=false) {
    vtObjectEllipse_c::setHeight (newHeight, b_updateObject);
  };

  //  Operation: setEllipseType
  //! Parameter:
  //! @param newEllipseType:
  //! @param b_updateObject:
  void setEllipseType(uint8_t newEllipseType, bool b_updateObject=false) {
    vtObjectEllipse_c::setEllipseType (newEllipseType, b_updateObject);
  };

  //  Operation: setStartAngle
  //! Parameter:
  //! @param newStartAngle:
  //! @param b_updateObject:
  void setStartAngle(uint8_t newStartAngle, bool b_updateObject=false) {
    vtObjectEllipse_c::setStartAngle (newStartAngle, b_updateObject);
  };

  //  Operation: setEndAngle
  //! Parameter:
  //! @param newEndAngle:
  //! @param b_updateObject:
  void setEndAngle(uint8_t newEndAngle, bool b_updateObject=false) {
    vtObjectEllipse_c::setEndAngle (newEndAngle, b_updateObject);
  };

  //  Operation: setFillAttributes
  //! Parameter:
  //! @param newFillAttribute:
  //! @param b_updateObject:
  void setFillAttributes(iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false) {
    vtObjectEllipse_c::setFillAttributes (newFillAttributes, b_updateObject);
  };

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false) {
    vtObjectEllipse_c::setSize(newWidth, newHeight, b_updateObject);
  };

}; // ~X2C

} // end namespace IsoAgLib

#endif // -X2C
