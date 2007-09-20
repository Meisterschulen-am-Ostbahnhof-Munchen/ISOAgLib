/***************************************************************************
                         ivtobjectlineattributes_c.h
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

#ifndef IVTOBJECTLINEATTRIBUTES_C_H
#define IVTOBJECTLINEATTRIBUTES_C_H

// +X2C includes
#include "impl/vtobjectlineattributes_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 172 : iVtObjectLineAttributes_c
//!  Stereotype: Klasse
class iVtObjectLineAttributes_c : public __IsoAgLib::vtObjectLineAttributes_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectLineAttributesSROM:
  //! @param b_initPointer:
  void init(const iVtObjectLineAttributes_s* vtObjectLineAttributesSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectLineAttributes_c::init (vtObjectLineAttributesSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectLineAtrtibutes_a
  const iVtObjectLineAttributes_s& get_vtObjectLineAttributes_a() { return *vtObjectLineAttributes_c::get_vtObjectLineAttributes_a (); }

  //  Operation: setLineColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setLineColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLineAttributes_c::setLineColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setLineWidth
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setLineWidth(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLineAttributes_c::setLineWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setLineArt
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setLineArt(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLineAttributes_c::setLineArt (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setLineAttributes
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setLineAttributes(uint16_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLineAttributes_c::setLineAttributes (newLineColour, newLineWidth, newLineArt, b_updateObject, b_enableReplaceOfCmd);
  }

  
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectLineAttributes_c::updateObjectType(); }
  */

  uint8_t updateLineColour(bool b_SendRequest=false) {
    return vtObjectLineAttributes_c::updateLineColour(b_SendRequest);
  }

  uint8_t updateLineWidth(bool b_SendRequest=false) {
    return vtObjectLineAttributes_c::updateLineWidth(b_SendRequest);
  }

  uint16_t updateLineArt(bool b_SendRequest=false) {
    return vtObjectLineAttributes_c::updateLineArt(b_SendRequest);
  }

}; // ~X2C

} // end of namespace IsoAgLib

#endif // -X2C
