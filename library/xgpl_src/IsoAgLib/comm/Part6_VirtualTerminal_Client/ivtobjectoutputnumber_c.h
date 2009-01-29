/***************************************************************************
                          ivtobjectoutputnumber_c.h
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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

#ifndef IVTOBJECTOUTPUTNUMBER_C_H
#define IVTOBJECTOUTPUTNUMBER_C_H


// +X2C includes
#include "impl/vtobjectoutputnumber_c.h"

#ifdef USE_VTOBJECT_outputnumber
#include "ivtobjectfontattributes_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 124 : iVtObjectOutputNumber_c
//!  Stereotype: Klasse
class iVtObjectOutputNumber_c : public __IsoAgLib::vtObjectOutputNumber_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectOutputNumberSROM:
  //! @param b_initPointer:
  void init(const iVtObjectOutputNumber_s* vtObjectOutputNumberSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectOutputNumber_c::init (vtObjectOutputNumberSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectOutputNumber_a
  const iVtObjectOutputNumber_s& get_vtObjectOutputNumber_a() { return *vtObjectOutputNumber_c::get_vtObjectOutputNumber_a(); }

  //  Operation: setValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) {
    vtObjectOutputNumber_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setWidth
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setBackgroundColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setFontAttributes
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setFontAttributes(iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setFontAttributes (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setOptions
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setVariableReference(iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setOffset
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setOffset(int32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setOffset (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setScale
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setScale(float newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setScale (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setNumberOfDecimals
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setNumberOfDecimals(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setNumberOfDecimals (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setFormat
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setFormat(bool newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setFormat (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHorizontalJustification
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setHorizontalJustification (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputNumber_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }


#ifdef USE_ISO_TERMINAL_GETATTRIBUTES  
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectOutputNumber_c::updateObjectType(); }
  */
  
  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateHeight(b_SendRequest);
  }

  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateBackgroundColour(b_SendRequest);
  }

  uint16_t updateFontAttributes(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateFontAttributes(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateOptions(b_SendRequest);
  }

  uint16_t updateVariableReference(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateVariableReference(b_SendRequest);
  }

  int32_t updateOffset(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateOffset(b_SendRequest);
  }

  float updateScale(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateScale(b_SendRequest);
  }

  uint8_t updateNumberOfDecimals(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateNumberOfDecimals(b_SendRequest);
  }

  uint8_t updateFormat(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateFormat(b_SendRequest);
  }

  uint8_t updateJustification(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateJustification(b_SendRequest);
  }

  /** that attribute is in parentheses in the spec, so commented out here
  uint32_t updateValue(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::getValue(b_SendRequest);
  }
  */
#endif
}; // ~X2C

} // end of namespace IsoAgLib

#endif // -X2C
#endif
