/***************************************************************************
                             ivtobjectmeter_c.h
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


#ifndef IVTOBJECTMETER_C_H
#define IVTOBJECTMETER_C_H


// +X2C includes
#include "impl/vtobjectmeter_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 119 : iVtObjectmeter_c
//!  Stereotype: Klasse
class iVtObjectMeter_c : public __IsoAgLib::vtObjectMeter_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectmeterSROM: 
  //! @param b_initPointer: 
  void init(const iVtObjectMeter_s* vtObjectMeterSROM) {
    vtObjectMeter_c::init (vtObjectMeterSROM);
  };

  //  Operation: get_vtObjectMeter_a
  iVtObjectMeter_s* get_vtObjectMeter_a() { return vtObjectMeter_c::get_vtObjectMeter_a(); };

  //  Operation: setWidth
  //! Parameter:
  //! @param newValue: 
  //! @param b_updateObject: 
  void setWidth(uint16_t newValue, bool b_updateObject= false) {
    vtObjectMeter_c::setWidth (newValue, b_updateObject);
  };

  //  Operation: setNeedleColour
  //! Parameter:
  //! @param newValue: 
  //! @param b_updateObject: 
  void setNeedleColour(uint8_t newValue, bool b_updateObject= false) {
    vtObjectMeter_c::setNeedleColour (newValue, b_updateObject);
  };

  //  Operation: setBorderColour
  //! Parameter:
  //! @param newValue: 
  //! @param b_updateObject: 
  void setBorderColour(uint8_t newValue, bool b_updateObject= false) {
    vtObjectMeter_c::setBorderColour (newValue, b_updateObject);
  };

  //  Operation: setArcAndTickColour
  //! Parameter:
  //! @param newValue: 
  //! @param b_updateObject: 
  void setArcAndTickColour(uint8_t newValue, bool b_updateObject= false) {
    vtObjectMeter_c::setArcAndTickColour (newValue, b_updateObject);
  };

  //  Operation: setOptions
  //! Parameter:
  //! @param newValue: 
  //! @param b_updateObject: 
  void setOptions(uint8_t newValue, bool b_updateObject= false) {
    vtObjectMeter_c::setOptions (newValue, b_updateObject);
  };

  //  Operation: setNumberOfTicks
  //! Parameter:
  //! @param newValue: 
  //! @param b_updateObject: 
  void setNumberOfTicks(uint8_t newValue, bool b_updateObject= false) {
    vtObjectMeter_c::setNumberOfTicks (newValue, b_updateObject);
  };
  
  //  Operation: setStartAngle
  //! Parameter:
  //! @param newValue: 
  //! @param b_updateObject: 
  void setStartAngle(uint8_t newValue, bool b_updateObject= false) {
    vtObjectMeter_c::setStartAngle (newValue, b_updateObject);
  };

  //  Operation: setEndAngle
  //! Parameter:
  //! @param newValue: 
  //! @param b_updateObject: 
  void setEndAngle(uint8_t newValue, bool b_updateObject= false) {
    vtObjectMeter_c::setEndAngle (newValue, b_updateObject);
  };

  //  Operation: setMin
  //! Parameter:
  //! @param newMin: 
  //! @param b_updateObject: 
  void setMin(uint32_t newMin, bool b_updateObject= false) {
    vtObjectMeter_c::setMin (newMin, b_updateObject);
  };

  //  Operation: setMax
  //! Parameter:
  //! @param newMax: 
  //! @param b_updateObject: 
  void setMax(uint32_t newMax, bool b_updateObject= false) {
    vtObjectMeter_c::setMax (newMax, b_updateObject);
  };
 
  //  Operation: setValue
  //! Parameter:
  //! @param newValue: 
  //! @param b_updateObject: 
  void setValue(uint32_t newValue, bool b_updateObject= false) {
    vtObjectMeter_c::setValue (newValue, b_updateObject);
  };

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newValue: 
  //! @param b_updateObject: 
  void setVariableReference(iVtObject_c* newValue, bool b_updateObject= false) {
    vtObjectMeter_c::setVariableReference (newValue, b_updateObject);
  };

}; // ~X2C

} // end namespace IsoAgLib

#endif // -X2C
