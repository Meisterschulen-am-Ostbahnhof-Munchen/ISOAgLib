/*
  ivtobjectworkingset_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
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
  void init(const iVtObjectWorkingSet_s* vtObjectWorkingSetSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectWorkingSet_c::init (vtObjectWorkingSetSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
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
