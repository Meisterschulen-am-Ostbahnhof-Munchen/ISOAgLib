/***************************************************************************
                         ivtobjectoutputstring_c.h
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

#ifndef IVTOBJECTOUTPUTSTRING_C_H
#define IVTOBJECTOUTPUTSTRING_C_H


// +X2C includes
#include "impl/vtobjectoutputstring_c.h"
#include "ivtobjectstringvariable_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {



//  +X2C Class 109 : iVtObjectOutputString_c
//!  Stereotype: Klasse
class iVtObjectOutputString_c : public __IsoAgLib::vtObjectOutputString_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectOutputStringSROM:
  //! @param b_initPointer:
  void init(const iVtObjectOutputString_s* vtObjectOutputStringSROM) {
    vtObjectOutputString_c::init (vtObjectOutputStringSROM);
  };

  //  Operation: get_vtObjectOutputString_a
  iVtObjectOutputString_s* get_vtObjectOutputString_a() { return vtObjectOutputString_c::get_vtObjectOutputString_a(); };


  //  Operation: setValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopy(const char* newValue, bool b_updateObject= false) {
    vtObjectOutputString_c::setValueCopy (newValue, b_updateObject);
  };

  //  Operation: setValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValueRef(const char* newValue, bool b_updateObject= false) {
    vtObjectOutputString_c::setValueRef (newValue, b_updateObject);
  };

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newVariable:
  //! @param b_updateObject:
  void setVariableReference(iVtObjectStringVariable_c* newValue, bool b_updateObject= false)  {
    vtObjectOutputString_c::setVariableReference (newValue, b_updateObject);
  };

  //  Operation: getString
  const char* getString() { return vtObjectOutputString_c::getString(); };


  //  Operation: setWidth
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject=false) {
    vtObjectOutputString_c::setWidth (newValue, b_updateObject);
  };

  //  Operation: setHeight
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setHeight(uint16_t newValue, bool b_updateObject=false) {
    vtObjectOutputString_c::setHeight (newValue, b_updateObject);
  };

  //  Operation: setBackgroundColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false) {
    vtObjectOutputString_c::setBackgroundColour (newValue, b_updateObject);
  };

  //  Operation: setFontAttributes
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setFontAttributes(iVtObject_c* newValue, bool b_updateObject=false) {
    vtObjectOutputString_c::setFontAttributes (newValue, b_updateObject);
  };

  //  Operation: setOptions
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setOptions(uint8_t newValue, bool b_updateObject=false) {
    vtObjectOutputString_c::setOptions (newValue, b_updateObject);
  };

  //  Operation: setHorizontalJustification
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false) {
    vtObjectOutputString_c::setHorizontalJustification (newValue, b_updateObject);
  };

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false) {
    vtObjectOutputString_c::setSize(newWidth, newHeight, b_updateObject);
  };

}; // ~X2C

} // end of namespace IsoAgLib

#endif // -X2C
