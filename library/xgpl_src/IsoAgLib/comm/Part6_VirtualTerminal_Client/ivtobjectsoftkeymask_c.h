/*
  ivtobjectsoftkeymask_c.h

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
#ifndef IVTOBJECTSOFTKEYMASK_C_H
#define IVTOBJECTSOFTKEYMASK_C_H

#include "impl/vtobjectsoftkeymask_c.h"

#ifdef CONFIG_USE_VTOBJECT_softkeymask


namespace IsoAgLib {

class iVtObjectSoftKeyMask_c : public __IsoAgLib::vtObjectSoftKeyMask_c
{
public:
  static ObjectType objectType() { return VT_OBJECT_TYPE_SOFT_KEY_MASK; }

  iVtObjectSoftKeyMask_c() = delete;
  explicit iVtObjectSoftKeyMask_c(
			iVtClientObjectPool_c* pool,
			ObjectID ID = autoID,
			Colour backgroundColour = CYAN);


  virtual ~iVtObjectSoftKeyMask_c() override;


  void setBackgroundColour(Colour newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES  
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const {
    return vtObjectSoftKeyMask_c::updateObjectType();
  }
  */
  
  Colour updateBackgroundColour(bool b_SendRequest=false) override;
#endif

  ObjectType getObjectType() const override;
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_softkeymask

#endif //IVTOBJECTSOFTKEYMASK_C_H
