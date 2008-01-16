/***************************************************************************
                         ivtobjectfontattributes_c.h
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

#ifndef IVTOBJECTFONTATTRIBUTES_C_H
#define IVTOBJECTFONTATTRIBUTES_C_H


// +X2C includes
#include "impl/vtobjectfontattributes_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 167 : iVtObjectFontAttributes_c
//!  Stereotype: Klasse
class iVtObjectFontAttributes_c : public __IsoAgLib::vtObjectFontAttributes_c
{

public:
  iVtObjectFontAttributes_c();

  virtual ~iVtObjectFontAttributes_c();
  /// Operation: init
  ///
  /// @param vtObjectFontAttributesSROM
  /// @param b_initPointer
  void init(const iVtObjectFontAttributes_s* vtObjectFontAttributesSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectFontAttributes_c::init (vtObjectFontAttributesSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  };

  //  Operation: get_vtObjectFontAttributes_a
  const iVtObjectFontAttributes_s& get_vtObjectFontAttributes_a() { return *vtObjectFontAttributes_c::get_vtObjectFontAttributes_a(); };

  //  Operation: getScaledWidthHeight
  uint16_t getScaledWidthHeight();

  /// Operation: setFontColour
  ///
  /// @param newValue
  /// @param b_updateObject
  void setFontColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectFontAttributes_c::setFontColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  };

  /// Operation: setFontSize
  ///
  /// @param newValue
  /// @param b_updateObject
  void setFontSize(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectFontAttributes_c::setFontSize (newValue, b_updateObject, b_enableReplaceOfCmd);
  };

  /// Operation: setFontType
  ///
  /// @param newValue
  /// @param b_updateObject
  void setFontType(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectFontAttributes_c::setFontType(newValue, b_updateObject, b_enableReplaceOfCmd);
  };

  /// Operation: setFontStyle
  ///
  /// @param newValue
  /// @param b_updateObject
  void setFontStyle(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectFontAttributes_c::setFontStyle(newValue, b_updateObject, b_enableReplaceOfCmd);
  };

  /// Operation: setFontAttributes
  ///
  /// @param newFontColour
  /// @param newFontSize
  /// @param newFontType
  /// @param newFontStyle
  /// @param b_updateObject
  void setFontAttributes(uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false){
    vtObjectFontAttributes_c::setFontAttributes(newFontColour, newFontSize, newFontType, newFontStyle, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectFontAttributes_c::updateObjectType(); }
  */

  uint8_t updateFontColour(bool b_SendRequest=false) {
    return vtObjectFontAttributes_c::updateFontColour(b_SendRequest);
  }

  uint8_t updateFontSize(bool b_SendRequest=false) {
    return vtObjectFontAttributes_c::updateFontSize(b_SendRequest);
  }

  uint8_t updateFontType(bool b_SendRequest=false) {
    return vtObjectFontAttributes_c::updateFontType(b_SendRequest);
  }

  uint8_t updateFontStyle(bool b_SendRequest=false) {
    return vtObjectFontAttributes_c::updateFontStyle(b_SendRequest);
  }
#endif
};

} // end of namespace IsoAgLib

#endif
