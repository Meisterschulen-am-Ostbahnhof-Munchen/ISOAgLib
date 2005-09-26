/***************************************************************************
                        ivtobjectfillattributes_c.h
                             -------------------
    begin                : Don Sep 4 2003
    copyright            : (C) 2003-2004 by Martin Wodok / Bradford Cox
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


#ifndef IVTOBJECTFILLATTRIBUTES_C_H
#define IVTOBJECTFILLATTRIBUTES_C_H


// +X2C includes
#include "impl/vtobjectfillattributes_c.h"
#include "ivtobjectpicturegraphic_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 167 : iVtObjectFillAttributes_c
//!  Stereotype: Klasse
class iVtObjectFillAttributes_c : public __IsoAgLib::vtObjectFillAttributes_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectFillAttributesSROM:
  //! @param b_initPointer:
  void init(const iVtObjectFillAttributes_s* vtObjectFillAttributesSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectFillAttributes_c::init (vtObjectFillAttributesSROM SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA);
  };

  //  Operation: get_vtObjectFillAttributes_a
  iVtObjectFillAttributes_s* get_vtObjectFillAttributes_a() { return vtObjectFillAttributes_c::get_vtObjectFillAttributes_a(); };


  //  Operation: setFillType
  //! Parameter:
  //! @param newFillType:
  //! @param b_updateObject:
  void setFillType(uint8_t newFillType, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectFillAttributes_c::setFillType(newFillType, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setFillColour
  //! Parameter:
  //! @param newFillColour:
  //! @param b_updateObject:
  void setFillColour(uint8_t newFillColour, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectFillAttributes_c::setFillColour(newFillColour, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setFillPattern
  //! Parameter:
  //! @param newFillPatternObject:
  //! @param b_updateObject:
  void setFillPattern(iVtObjectPictureGraphic_c* newFillPatternObject, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectFillAttributes_c::setFillPattern(newFillPatternObject, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: changeFillAttributes
  //! Parameter:
  //! @param newFillType:
  //! @param newFillColour:
  //! @param newFillPatternObject:
  //! @param b_updateObject:
  void setFillAttributes(uint8_t newFillType, uint8_t newFillColour, iVtObjectPictureGraphic_c* newFillPattern, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectFillAttributes_c::setFillAttributes(newFillType, newFillColour, newFillPattern, b_updateObject, b_enableReplaceOfCmd);
  };

}; // ~X2C

} // end namespace IsoAgLib

#endif // -X2C
