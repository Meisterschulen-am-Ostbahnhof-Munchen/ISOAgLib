/*
  ivtobjectauxiliaryinput2_c.h

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
#ifndef IVTOBJECTAUXILIARYINPUT2_C_H
#define IVTOBJECTAUXILIARYINPUT2_C_H

#include "impl/vtobjectauxiliaryinput2_c.h"


#ifdef CONFIG_USE_VTOBJECT_auxiliaryinput2

namespace IsoAgLib {

class iVtObjectAuxiliaryInput2_c : public __IsoAgLib::vtObjectAuxiliaryInput2_c
{
public:
  static uint16_t objectType();

  typedef  __IsoAgLib::vtObjectAuxiliaryInput2_c::StateForLearnMode_en StateForLearnMode_en;


  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectAuxiliaryInput2_c::updateObjectType(); }
  */
#endif //CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES

  /**
    set new value and trigger AUX2 input state message
    @paramter aen_stateForLearnMode:
       - default value: the active state is calculated from aui16_value1 != 0
       - StateForLearnMode_SetActive: set state to activated
       - StateForLearnMode_SetInactive: set state to not activated
    @return true if range of value matches the expected range for this function type
  */
  bool setValue(uint16_t aui16_value1, uint16_t aui16_value2 = 0, StateForLearnMode_en aen_stateForLearnMode = StateForLearnMode_CalculateInternally) override;

  uint16_t getObjectType() const override;
};

} // IsoAgLib

#endif // CONFIG_USE_VTOBJECT_auxiliaryinput2

#endif // IVTOBJECTAUXILIARYINPUT2_C_H
