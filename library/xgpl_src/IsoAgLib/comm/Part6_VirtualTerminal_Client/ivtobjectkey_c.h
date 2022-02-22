/*
  ivtobjectkey_c.h

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
#ifndef IVTOBJECTKEY_C_H
#define IVTOBJECTKEY_C_H

#include "impl/vtobjectkey_c.h"


#ifdef CONFIG_USE_VTOBJECT_key

namespace IsoAgLib {

class iVtObjectKey_c : public __IsoAgLib::vtObjectKey_c
{
public:

	iVtObjectKey_c() = delete;

	~iVtObjectKey_c() override;

	iVtObjectKey_c(
	    iVtClientObjectPool_c* pool,
		ObjectID ID = autoID,
		Colour backgroundColour = OLIVE,
		uint8_t keyCode = 0);







  static ObjectType objectType() { return VT_OBJECT_TYPE_KEY; }


  void setBackgroundColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectKey_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //! @param newValue: Range 1-255, KeyCode 0 is reserved for KeyCode:"ESC"
  void setKeyCode(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectKey_c::setKeyCode (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  bool moveChildLocation(iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectKey_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }

  bool setChildPosition(iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectKey_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const {
  return vtObjectKey_c::updateObjectType();
  }
   */

  Colour updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectKey_c::updateBackgroundColour(b_SendRequest);
  }

  uint8_t updateKeyCode(bool b_SendRequest=false) {
    return vtObjectKey_c::updateKeyCode(b_SendRequest);
  }
#endif

  virtual ObjectType getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_key

#endif //IVTOBJECTKEY_C_H
