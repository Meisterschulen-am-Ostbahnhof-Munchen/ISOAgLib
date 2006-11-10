/***************************************************************************
                           ivtobjectinputlist_c.h
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

#ifndef IVTOBJECTINPUTLIST_C_H
#define IVTOBJECTINPUTLIST_C_H


// +X2C includes
#include "impl/vtobjectinputlist_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 93 : iVtObjectInputList_c
//!  Stereotype: Klasse
class iVtObjectInputList_c : public __IsoAgLib::vtObjectInputList_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectInputListSROM:
  //! @param b_initPointer:
  void init(const iVtObjectInputList_s* vtObjectInputListSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectInputList_c::init (vtObjectInputListSROM SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA);
  }

  //  Operation: get_vtObjectInputList_a
  const iVtObjectInputList_s& get_vtObjectInputList_a() { return *vtObjectInputList_c::get_vtObjectInputList_a(); }

  //  Operation: getListItem
  //! Parameter:
  //! @param xth:
  iVtObject_c* getListItem(uint8_t xth) { return vtObjectInputList_c::getListItem (xth); }

  //  Operation: getNumberOfListItems
  uint8_t getNumberOfListItems() { return vtObjectInputList_c::getNumberOfListItems(); }

  //  Operation: setValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputList_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setItem
  //! Parameter:
  //! @param rui8_index:
  //! @param rpc_object:
  void setItem(uint8_t rui8_index, iVtObject_c* rpc_object, bool b_enableReplaceOfCmd=false) {
    vtObjectInputList_c::setItem (rui8_index, rpc_object, b_enableReplaceOfCmd);
  }

  //  Operation: setWidth
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputList_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputList_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setVariableReference(iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputList_c::setVariableReference(newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputList_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

    //  Operation: enable
  //! Parameter:
  //! @param b_updateObject:
  bool enable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) { return vtObject_c::able (1, b_updateObject, b_enableReplaceOfCmd); }

  //  Operation: disable
  //! Parameter:
  //! @param b_updateObject:
  bool disable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) { return vtObject_c::able (0, b_updateObject, b_enableReplaceOfCmd); }

  //  Operation: select
  bool select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }

  
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputList_c::updateObjectType(); }
  */
  
  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectInputList_c::updateWidth(b_SendRequest);
  }
  
  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectInputList_c::updateHeight(b_SendRequest);
  }

  uint16_t updateVariableReference(bool b_SendRequest=false) {
    return vtObjectInputList_c::updateVariableReference(b_SendRequest);
  }

  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateValue(bool b_SendRequest=false) {
    return vtObjectInputList_c::getValue(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectInputList_c::updateOptions(b_SendRequest);
  }
  */

}; // ~X2C

} // end of namespace IsoAgLib

#endif // -X2C
