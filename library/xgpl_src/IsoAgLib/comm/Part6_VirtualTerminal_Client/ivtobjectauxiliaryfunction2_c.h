/*
  ivtobjectauxiliaryfunction2_c.h

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
#ifndef IVTOBJECTAUXILIARYFUNCTION2_C_H
#define IVTOBJECTAUXILIARYFUNCTION2_C_H

#include "impl/vtobjectauxiliaryfunction2_c.h"


#ifdef CONFIG_USE_VTOBJECT_auxiliaryfunction2

namespace IsoAgLib {

class iVtObjectAuxiliaryFunction2_c : public __IsoAgLib::vtObjectAuxiliaryFunction2_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_AUXILIARY_FUNCTION_2; }

  ~iVtObjectAuxiliaryFunction2_c(){};



  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectAuxiliaryFunction2_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectAuxiliaryFunction2_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }

  // This function overrides the FunctionType as defined in the object pool.
  // Only use it once at startup (after the object pool was initAndRegistered)
  // It will only modify the object, not send out any command, so it needs
  // to be done before the object pool upload takes place. (At best before the main loop runs)
  void overrideFunctionType_onlyAtStartup(uint8_t newValue) {
    vtObjectAuxiliaryFunction2_c::overrideFunctionType_onlyAtStartup(newValue);
  }

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectAuxiliaryFunction2_c::updateObjectType(); }
  */
#endif

  //! @return true, if AUX2 input is assigned
  bool isAssigned() const
  { return __IsoAgLib::vtObjectAuxiliaryFunction2_c::isAssigned(); }

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif // CONFIG_USE_VTOBJECT_auxiliaryfunction2

#endif // IVTOBJECTAUXILIARYFUNCTION2_C_H
