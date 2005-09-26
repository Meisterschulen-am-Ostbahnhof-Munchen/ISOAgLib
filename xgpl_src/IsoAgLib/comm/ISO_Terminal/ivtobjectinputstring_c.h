/***************************************************************************
                          ivtobjectinputstring_c.h
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


#ifndef IVTOBJECTINPUTSTRING_C_H
#define IVTOBJECTINPUTSTRING_C_H


// +X2C includes
#include "impl/vtobjectinputstring_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 109 : iVtObjectInputString_c
//!  Stereotype: Klasse
class iVtObjectInputString_c : public __IsoAgLib::vtObjectInputString_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectInputStringSROM:
  //! @param b_initPointer:
  void init(const iVtObjectInputString_s* vtObjectInputStringSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectInputString_c::init (vtObjectInputStringSROM SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA);
  };

  //  Operation: get_vtObjectInputString_a
  iVtObjectInputString_s* get_vtObjectInputString_a() { return vtObjectInputString_c::get_vtObjectInputString_a(); };

  //  Operation: getString
  const char* getString() { return vtObjectInputString_c::getString(); };


  //  Operation: setValueRef
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValueRef(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputString_c::setValueRef (newValue, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setValueCopy
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopy(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputString_c::setValueCopy (newValue, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newVariable:
  //! @param b_updateObject:
  void setVariableReference(iVtObjectStringVariable_c* newVariable, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputString_c::setVariableReference (newVariable, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: changeBackgroundColor
  //! Parameter:
  //! @param colorValue:
  //! @param b_updateObject:
  void changeBackgroundColour(uint8_t colorValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputString_c::setBackgroundColour (colorValue, b_updateObject, b_enableReplaceOfCmd);
  };

  // Operation: setWidth
  //! Parameter:
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputString_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
  };

  // Operation: setHeight
  //! Parameter:
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputString_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
  };

  // Operation: setFontAttributes
  //! Parameter:
  //! @param newFontAttributes:
  //! @param b_updateObject:
  void setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c* newFontAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputString_c::setFontAttributes (newFontAttributes, b_updateObject, b_enableReplaceOfCmd);
  };

  // Operation: setInputAttributes
  //! Parameter:
  //! @param newInputAttributes:
  //! @param b_updateObject:
  void setInputAttributes(IsoAgLib::iVtObjectInputAttributes_c* newInputAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputString_c::setInputAttributes (newInputAttributes, b_updateObject, b_enableReplaceOfCmd);
  };

  // Operation: setOptions
  //! Parameter:
  //! @param newOptions:
  //! @param b_updateObject:
  void setOptions(uint8_t newOptions, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputString_c::setOptions (newOptions, b_updateObject, b_enableReplaceOfCmd);
  };

  // Operation: setHorizontalJustification
  //! Parameter:
  //! @param newHorizontalJustification:
  //! @param b_updateObject:
  void setHorizontalJustification(uint8_t newHorizontalJustification, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputString_c::setHorizontalJustification (newHorizontalJustification, b_updateObject, b_enableReplaceOfCmd);
  };

    //  Operation: enable
  //! Parameter:
  //! @param b_updateObject:
  bool enable(bool b_updateObject= false, bool b_enableReplaceOfCmd=true) { return vtObject_c::able (1, b_updateObject, b_enableReplaceOfCmd); };

  //  Operation: disable
  //! Parameter:
  //! @param b_updateObject:
  bool disable(bool b_updateObject= false, bool b_enableReplaceOfCmd=true) { return vtObject_c::able (0, b_updateObject, b_enableReplaceOfCmd); };

  //  Operation: select
  bool select() { return vtObject_c::select(); }

private:

}; // ~X2C

} // end namespace IsoAgLib

#endif // -X2C
