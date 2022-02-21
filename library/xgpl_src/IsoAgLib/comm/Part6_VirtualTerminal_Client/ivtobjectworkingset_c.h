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
  static ObjectType objectType();


  explicit iVtObjectWorkingSet_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID = autoID,
  		Colour backgroundColour = BLACK,
  		uint8_t selectable = 1,
  		iVtObjectMask_c* activeMask = nullptr // data or alarm mask
		);

  iVtObjectWorkingSet_c() = delete;
  ~iVtObjectWorkingSet_c() override;


  void updateSelectable( uint8_t newSelectable ) override;
  void changeActiveMask(iVtObjectMask_c* apc_iVtObjectMask, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void changeBackgroundColour(Colour newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  bool moveChildLocation(iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  bool setChildPosition(iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  bool controlAudioDevice (uint8_t aui8_repetitions, uint16_t aui16_frequency, uint16_t aui16_onTime, uint16_t aui16_offTime) override;

  //! @param aui8_volume The Volume given in percent. Range 0..100
  bool setAudioVolume (uint8_t aui8_volume) override;
  bool setColourMapOrPalette (uint16_t aui16_objectId) override;

  // ///////////////////////// getter for attributes
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const {
    return vtObjectWorkingSet_c::updateObjectType();
  }

  Colour updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectWorkingSet_c::updateBackgroundColour(b_SendRequest);
  }

  uint8_t updateSelectable(bool b_SendRequest=false) {
    return vtObjectWorkingSet_c::updateSelectable(b_SendRequest);
  }

  uint16_t updateActiveMask(bool b_SendRequest=false) {
    return vtObjectWorkingSet_c::updateActiveMask(b_SendRequest);
  }
  */

  ObjectType getObjectType() const override;
};

} // IsoAgLib

#endif
