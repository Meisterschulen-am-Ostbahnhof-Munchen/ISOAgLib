/***************************************************************************
                         ivtobjectlinearbargraph_c.h
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

#ifndef IVTOBJECTLINEARBARGRAPH_C_H
#define IVTOBJECTLINEARBARGRAPH_C_H

// +X2C includes
#include "impl/vtobjectlinearbargraph_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 119 : iVtObjectLinearBarGraph_c
//!  Stereotype: Klasse
class iVtObjectLinearBarGraph_c : public __IsoAgLib::vtObjectLinearBarGraph_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectLinearBarGraphSROM:
  //! @param b_initPointer:
  void init(const iVtObjectLinearBarGraph_s* vtObjectLinearBarGraphSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectLinearBarGraph_c::init (vtObjectLinearBarGraphSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectLinearBarGraph_a
  const iVtObjectLinearBarGraph_s& get_vtObjectLinearBarGraph_a() { return *vtObjectLinearBarGraph_c::get_vtObjectLinearBarGraph_a(); }

  //  Operation: setValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) {
    vtObjectLinearBarGraph_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setWidth
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setTargetLineColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setTargetLineColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setTargetLineColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setOptions
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setNumberOfTicks
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setNumberOfTicks(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setNumberOfTicks (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setMinValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setMinValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setMinValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setMaxValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setMaxValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setMaxValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setVariableReference(iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setTargetValueVariableReference
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setTargetValueVariableReference(iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setTargetValueVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setTargetValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setTargetValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setTargetValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectLinearBarGraph_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateHeight(b_SendRequest);
  }

  uint8_t updateColour(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateColour(b_SendRequest);
  }

  uint8_t updateTargetLineColour(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateTargetLineColour(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateOptions(b_SendRequest);
  }

  uint8_t updateNumberOfTicks(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateNumberOfTicks(b_SendRequest);
  }

  uint16_t updateMinValue(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateMinValue(b_SendRequest);
  }

  uint16_t updateMaxValue(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateMaxValue(b_SendRequest);
  }

  uint16_t updateVariableReference(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateVariableReference(b_SendRequest);
  }

  uint16_t updateTargetValueVariableReference(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateTargetValueVariableReference(b_SendRequest);
  }

  uint16_t updateTargetValue(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateTargetValue(b_SendRequest);
  }

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::getValue(b_SendRequest);
  }
  */
};

} // end of namespace IsoAgLib

#endif
