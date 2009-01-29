/***************************************************************************
                           ivtobjectworkingset_c.h
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

#ifndef IVTOBJECTWORKINGSET_C_H
#define IVTOBJECTWORKINGSET_C_H


// +X2C includes
#include "impl/vtobjectworkingset_c.h"
#include "ivtobjectmask_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 66 : iVtObjectWorkingSet_c
//!  Stereotype: Klasse
class iVtObjectWorkingSet_c : public __IsoAgLib::vtObjectWorkingSet_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectWorkingSetSROM:
  //! @param b_initPointer:
  void init(const iVtObjectWorkingSet_s* vtObjectWorkingSetSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectWorkingSet_c::init (vtObjectWorkingSetSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectWorkingSet_a
  const iVtObjectWorkingSet_s& get_vtObjectWorkingSet_a() { return *vtObjectWorkingSet_c::get_vtObjectWorkingSet_a(); }

  /// Operation: changeActiveMask
  ///
  /// @param apc_iVtObjectMask
  /// @param b_updateObject
  /// @param b_enableReplaceOfCmd
  void changeActiveMask(iVtObjectMask_c* apc_iVtObjectMask, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectWorkingSet_c::changeActiveMask (apc_iVtObjectMask, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: changeBackgroundColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void changeBackgroundColour(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectWorkingSet_c::changeBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: moveChildLocation
  //! Parameter:
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  //! @param b_updateObject:
  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectWorkingSet_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  //  Operation: setChildPosition
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectWorkingSet_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: controlAudioDevice
  //! @param aui8_repetitions
  //! @param aui16_frequency
  //! @param aui16_onTime
  //! @param aui16_offTime
  bool controlAudioDevice (uint8_t aui8_repetitions, uint16_t aui16_frequency, uint16_t aui16_onTime, uint16_t aui16_offTime) {
    return vtObjectWorkingSet_c::controlAudioDevice (aui8_repetitions, aui16_frequency, aui16_onTime, aui16_offTime);
  }

  //  Operation: setAudioVolume
  //! @param aui8_volume The Volume given in percent. Range 0..100
  bool setAudioVolume (uint8_t aui8_volume) {
    return vtObjectWorkingSet_c::setAudioVolume (aui8_volume);
  }

  // ///////////////////////// getter for attributes
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const {
    return vtObjectWorkingSet_c::updateObjectType();
  }

  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectWorkingSet_c::updateBackgroundColour(b_SendRequest);
  }

  uint8_t updateSelectable(bool b_SendRequest=false) {
    return vtObjectWorkingSet_c::updateSelectable(b_SendRequest);
  }

  uint16_t updateActiveMask(bool b_SendRequest=false) {
    return vtObjectWorkingSet_c::updateActiveMask(b_SendRequest);
  }
  */
};

} // end of namespace IsoAgLib

#endif
