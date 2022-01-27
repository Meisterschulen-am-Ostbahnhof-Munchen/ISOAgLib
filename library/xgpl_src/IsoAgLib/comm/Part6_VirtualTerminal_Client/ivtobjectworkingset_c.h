/*
  ivtobjectworkingset_c.h

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTWORKINGSET_C_H
#define IVTOBJECTWORKINGSET_C_H

#include "impl/vtobjectworkingset_c.h"
#include "ivtobjectmask_c.h"


namespace IsoAgLib {

class iVtObjectWorkingSet_c : public __IsoAgLib::vtObjectWorkingSet_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_WORKING_SET; }

  iVtObjectWorkingSet_c(const iVtObjectWorkingSet_s* vtObjectWorkingSetSROM , int ai_multitonInst)
  :__IsoAgLib::vtObjectWorkingSet_c(vtObjectWorkingSetSROM , ai_multitonInst)
  {}

  void init(const iVtObjectWorkingSet_s* vtObjectWorkingSetSROM , int ai_multitonInst) {
    vtObjectWorkingSet_c::init (vtObjectWorkingSetSROM , ai_multitonInst);
  }

  const iVtObjectWorkingSet_s& get_vtObjectWorkingSet_a() { return *vtObjectWorkingSet_c::get_vtObjectWorkingSet_a(); }

  void updateSelectable( uint8_t newSelectable ) {
    vtObjectWorkingSet_c::updateSelectable( newSelectable );
  }

  void changeActiveMask(iVtObjectMask_c* apc_iVtObjectMask, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectWorkingSet_c::changeActiveMask (apc_iVtObjectMask, b_updateObject, b_enableReplaceOfCmd);
  }

  void changeBackgroundColour(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectWorkingSet_c::changeBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectWorkingSet_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectWorkingSet_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }

  bool controlAudioDevice (uint8_t aui8_repetitions, uint16_t aui16_frequency, uint16_t aui16_onTime, uint16_t aui16_offTime) {
    return vtObjectWorkingSet_c::controlAudioDevice (aui8_repetitions, aui16_frequency, aui16_onTime, aui16_offTime);
  }

  //! @param aui8_volume The Volume given in percent. Range 0..100
  bool setAudioVolume (uint8_t aui8_volume) {
    return vtObjectWorkingSet_c::setAudioVolume (aui8_volume);
  }

  bool setColourMapOrPalette (uint16_t aui16_objectId) {
    return vtObjectWorkingSet_c::setColourMapOrPalette (aui16_objectId);
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

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif
