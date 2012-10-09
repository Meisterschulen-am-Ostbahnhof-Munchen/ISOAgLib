/***************************************************************************
                          ivtobjectarchedbargraph_c.h
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


#ifndef IVTOBJECTARCHEDBARGRAPH_C_H
#define IVTOBJECTARCHEDBARGRAPH_C_H


// +X2C includes
#include "impl/vtobjectarchedbargraph_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {



//  +X2C Class 119 : vtObjectArchedBarGraph_c
//!  Stereotype: Klasse
class iVtObjectArchedBarGraph_c : public __IsoAgLib::vtObjectArchedBarGraph_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectArchedBarGraphSROM:
  //! @param b_initPointer:
  void init(const iVtObjectArchedBarGraph_s* vtObjectArchedBarGraphSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectArchedBarGraph_c::init (vtObjectArchedBarGraphSROM SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA);
  };

  //  Operation: get_vtObjectArchedBarGraph_a
  iVtObjectArchedBarGraph_s* get_vtObjectArchedBarGraph_a() { return vtObjectArchedBarGraph_c::get_vtObjectArchedBarGraph_a(); }
  ~iVtObjectArchedBarGraph_c(){};

  //  Operation: setValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) {
    vtObjectArchedBarGraph_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setWidth
  //! Parameter:
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newWidth, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setHeight
  //! Parameter:
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newHeight, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setColour
  //! Parameter:
  //! @param newColour:
  //! @param b_updateObject:
  void setColour(uint8_t newColour, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setColour (newColour, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setTargetLineColour
  //! Parameter:
  //! @param newTargetLineColour:
  //! @param b_updateObject:
  void setTargetLineColour(uint8_t newTargetLineColour, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setTargetLineColour (newTargetLineColour, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setOptions
  //! Parameter:
  //! @param newOptions:
  //! @param b_updateObject:
  void setOptions(uint8_t newOptions, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setOptions (newOptions, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setStartAngle
  //! Parameter:
  //! @param newStartAngle:
  //! @param b_updateObject:
  void setStartAngle(uint8_t newStartAngle, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setStartAngle (newStartAngle, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setEndAngle
  //! Parameter:
  //! @param newEndAngle:
  //! @param b_updateObject:
  void setEndAngle(uint8_t newEndAngle, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setEndAngle (newEndAngle, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setBarGraphWidth
  //! Parameter:
  //! @param newBarGraphWidth:
  //! @param b_updateObject:
  void setBarGraphWidth(uint16_t newBarGraphWidth, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setBarGraphWidth (newBarGraphWidth, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setMinValue
  //! Parameter:
  //! @param newMin:
  //! @param b_updateObject:
  void setMinValue(uint32_t newMin, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setMinValue (newMin, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setMaxValue
  //! Parameter:
  //! @param newMax:
  //! @param b_updateObject:
  void setMaxValue(uint32_t newMax, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setMaxValue (newMax, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newVariableReference:
  //! @param b_updateObject:
  void setVariableReference(iVtObject_c* newVariableReference, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setVariableReference (newVariableReference, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setTargetValueVariableReference
  //! Parameter:
  //! @param newTargetValueVariableReference:
  //! @param b_updateObject:
  void setTargetValueVariableReference(iVtObject_c* newTargetValueVariableReference, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setTargetValueVariableReference (newTargetValueVariableReference, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setTargetValue
  //! Parameter:
  //! @param newTargetValue:
  //! @param b_updateObject:
  void setTargetValue(uint16_t newTargetValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setTargetValue (newTargetValue, b_updateObject, b_enableReplaceOfCmd);
  };

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectArchedBarGraph_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

}; // ~X2C

} // end namespace __IsoAgLib

#endif // -X2C
