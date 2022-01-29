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
  static uint16_t objectType();

  iVtObjectWorkingSet_c(const iVtObjectWorkingSet_s* vtObjectWorkingSetSROM , int ai_multitonInst);

  iVtObjectWorkingSet_c(
		int ai_multitonInst,
		ObjectID ID = autoID,
  		uint8_t backgroundColour = 0,
  		uint8_t selectable = 1,
  		iVtObjectMask_c* activeMask = nullptr, // data or alarm mask
  		uint8_t numberOfLanguagesToFollow = 0,
  		const repeat_vtLanguage_s* languagesToFollow = nullptr);



  const iVtObjectWorkingSet_s& get_vtObjectWorkingSet_a();

  void updateSelectable( uint8_t newSelectable );

  void changeActiveMask(iVtObjectMask_c* apc_iVtObjectMask, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  void changeBackgroundColour(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false);

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  bool controlAudioDevice (uint8_t aui8_repetitions, uint16_t aui16_frequency, uint16_t aui16_onTime, uint16_t aui16_offTime);

  //! @param aui8_volume The Volume given in percent. Range 0..100
  bool setAudioVolume (uint8_t aui8_volume);

  bool setColourMapOrPalette (uint16_t aui16_objectId);

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

  virtual uint16_t getObjectType() const;
};

} // IsoAgLib

#endif
