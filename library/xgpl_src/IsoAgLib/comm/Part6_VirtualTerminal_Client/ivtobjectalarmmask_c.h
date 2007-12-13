/***************************************************************************
                            ivtobjectalarmmask_c.h
                             -------------------
    begin                : Don Sep 4 2003
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
#ifndef IVTOBJECTALARMMASK_C_H
#define IVTOBJECTALARMMASK_C_H

#include "impl/vtobjectalarmmask_c.h"

#if not defined PRJ_ISO_TERMINAL_OBJECT_SELECTION1 || defined USE_VTOBJECT_alarmmask
#include "ivtobjectsoftkeymask_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 76 : iVtObjectAlarmMask_c
//!  Stereotype: Klasse
class iVtObjectAlarmMask_c : public __IsoAgLib::vtObjectAlarmMask_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectAlarmMaskSROM:
  //! @param b_initPointer:
  void init(const iVtObjectAlarmMask_s* vtObjectAlarmMaskSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectAlarmMask_c::init (vtObjectAlarmMaskSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }
  ~iVtObjectAlarmMask_c(){}
  //  Operation: get_vtObjectAlarmMask_a
  const iVtObjectAlarmMask_s& get_vtObjectAlarmMask_a() { return *vtObjectAlarmMask_c::get_vtObjectAlarmMask_a(); }


  //  Operation: setBackgroundColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  //! @param b_enableReplaceOfCmd:
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectAlarmMask_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSoftKeyMask
  //! Parameter:
  //! @param newSoftKeyMask:
  //! @param b_updateObject:
  void setSoftKeyMask(iVtObjectSoftKeyMask_c* newSoftKeyMask, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectAlarmMask_c::setSoftKeyMask (newSoftKeyMask, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setPriority
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setPriority(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectAlarmMask_c::setPriority (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setAcousticSignal
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setAcousticSignal(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectAlarmMask_c::setAcousticSignal (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: moveChildLocation
  //! Parameter:
  //! @param apc_object:
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  bool moveChildLocation(iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectAlarmMask_c::moveChildLocation (apc_childObject, dx, dy, b_updateObject);
  }

  //  Operation: setChildPosition

  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectAlarmMask_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const {
    return vtObjectAlarmMask_c::updateObjectType();
  }
   */

  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectAlarmMask_c::updateBackgroundColour(b_SendRequest);
  }

  uint16_t updateSoftKeyMask(bool b_SendRequest=false) {
    return vtObjectAlarmMask_c::updateSoftKeyMask(b_SendRequest);
  }

  uint8_t updatePriority(bool b_SendRequest=false) {
    return vtObjectAlarmMask_c::updatePriority(b_SendRequest);
  }

  uint8_t updateAcousticSignal(bool b_SendRequest=false) {
    return vtObjectAlarmMask_c::updateAcousticSignal(b_SendRequest);
  }
#endif
};

} // end of namespace IsoAgLib

#endif
#endif
