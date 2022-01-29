/*
  ivtobjectauxiliaryinput_c.h

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
#ifndef IVTOBJECTAUXILIARYINPUT_C_H
#define IVTOBJECTAUXILIARYINPUT_C_H

#include "impl/vtobjectauxiliaryinput_c.h"



namespace IsoAgLib {

class iVtObjectAuxiliaryInput_c : public __IsoAgLib::vtObjectAuxiliaryInput_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_AUXILIARY_INPUT_1; }

  iVtObjectAuxiliaryInput_c(const iVtObjectAuxiliaryInput_s* vtObjectAuxiliaryInputSROM , int ai_multitonInst)
  :vtObjectAuxiliaryInput_c(vtObjectAuxiliaryInputSROM , ai_multitonInst)
  {}

  const iVtObjectAuxiliaryInput_s& get_vtObjectAuxiliaryInput_a() { return *vtObjectAuxiliaryInput_c::get_vtObjectAuxiliaryInput_a(); }

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectAuxiliaryInput_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectAuxiliaryInput_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectAuxiliaryInput_c::updateObjectType(); }
  */
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif

