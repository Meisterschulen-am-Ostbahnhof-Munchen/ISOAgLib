/*
  ivtobjectnumbervariable_c.h

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
#ifndef IVTOBJECTNUMBERVARIABLE_C_H
#define IVTOBJECTNUMBERVARIABLE_C_H

#include "impl/vtobjectnumbervariable_c.h"


#ifdef CONFIG_USE_VTOBJECT_numbervariable

namespace IsoAgLib {

class iVtObjectNumberVariable_c : public __IsoAgLib::vtObjectNumberVariable_c
{
public:


	iVtObjectNumberVariable_c() = delete;

	~iVtObjectNumberVariable_c();

	iVtObjectNumberVariable_c(
			iVtClientObjectPool_c* pool,
			ObjectID ID,
			uint32_t value);







  static ObjectType objectType() { return VT_OBJECT_TYPE_NUMBER_VARIABLE; }

  void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) override {
    vtObjectNumberVariable_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd); }


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES  
  // ///////////////////////// getter for attributes
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectNumberVariable_c::updateObjectType(); }
  
  uint32_t updateValue(bool b_SendRequest=false) {
    return vtObjectNumberVariable_c::getValue(b_SendRequest);
  }
  */
#endif

  virtual ObjectType getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_numbervariable

#endif //IVTOBJECTNUMBERVARIABLE_C_H
