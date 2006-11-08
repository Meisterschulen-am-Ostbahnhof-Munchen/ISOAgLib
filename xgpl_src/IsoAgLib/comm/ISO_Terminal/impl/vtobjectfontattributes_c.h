/***************************************************************************
                          vtobjectfontattributes_c.h
                             -------------------
    begin                : Mon Jun 30 14:41:54 2003
    copyright            : (C) 2003 by Martin Wodok
    email                : m.wodok@osb-ag:de
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

#ifndef VTOBJECTFONTATTRIBUTES_C_H
#define VTOBJECTFONTATTRIBUTES_C_H


#include "vtobject_c.h"
#include "isoterminal_c.h"
#include "vtclientservercommunication_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


//  +X2C Class 167 : vtObjectFontAttributes_c
//!  Stereotype: Klasse
class vtObjectFontAttributes_c : public vtObject_c
{

public:

  //  Operation: stream
  //! Parameter:
  //! @param:destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  //  Operation: init
  //! Parameter:
  //! @param vtObjectFontAttributesSROM:
  //! @param b_initPointer:
  void init(const iVtObjectFontAttributes_s* vtObjectFontAttributesSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) { vtObject_c::init ((iVtObject_s*) vtObjectFontAttributesSROM SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA);};

  //  Operation: get_vtObjectFontAttributes_a
  iVtObjectFontAttributes_s* get_vtObjectFontAttributes_a() { return (iVtObjectFontAttributes_s *)vtObject_a; }

  //  Operation: vtObjectFontAttributes_c
  vtObjectFontAttributes_c();
  virtual ~vtObjectFontAttributes_c(){};
  //  Operation: size
  uint32_t fitTerminal() const;

  //  Operation: getScaledWidthHeight
  uint16_t getScaledWidthHeight();

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setFontColour(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontColour) :
        0, sizeof(iVtObjectFontAttributes_s), 1, newValue, __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newValue, this, IsoAgLib::FontColour), b_enableReplaceOfCmd);
  }

  void setFontSize(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontSize) : 0, sizeof(iVtObjectFontAttributes_s), 2, newValue, newValue, b_enableReplaceOfCmd);
  }

  void setFontType(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontType) : 0, sizeof(iVtObjectFontAttributes_s), 3, newValue, newValue, b_enableReplaceOfCmd);
  }

  void setFontStyle(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontStyle) : 0, sizeof(iVtObjectFontAttributes_s), 4, newValue, newValue, b_enableReplaceOfCmd);
  }

  void setFontAttributes(uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

private:

  static uint8_t font2PixelDimensionTableW [15];
  static uint8_t font2PixelDimensionTableH [15];

  //! Calculate font size and cache in ui8_fontSizeScaled
  void calcScaledFontDimension() const;

  //! Chached font size calculated by calcScaledFontDimension()
  mutable uint8_t ui8_fontSizeScaled;

};

} // end of namespace __IsoAgLib

#endif // VTOBJECTFONTATTRIBUTES_C_H

