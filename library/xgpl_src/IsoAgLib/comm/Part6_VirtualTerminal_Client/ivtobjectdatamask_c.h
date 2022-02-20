/*
  ivtobjectdatamask_c.h

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
#ifndef IVTOBJECTDATAMASK_C_H
#define IVTOBJECTDATAMASK_C_H

#include "impl/vtobjectdatamask_c.h"


namespace IsoAgLib {

class iVtObjectDataMask_c : public __IsoAgLib::vtObjectDataMask_c
{
private:
    iVtObjectDataMask_c() = delete;

public:
  static ObjectType objectType();

  ~iVtObjectDataMask_c() override;

  explicit iVtObjectDataMask_c(
		  iVtClientObjectPool_c* pool
		, ObjectID ID = autoID
  	    , Colour backgroundColour = BLACK
#ifdef CONFIG_USE_VTOBJECT_softkeymask
  	    , iVtObjectSoftKeyMask_c* softKeyMask = nullptr
#endif
		);


#ifdef CONFIG_USE_VTOBJECT_softkeymask
  void setSoftKeyMask(iVtObjectSoftKeyMask_c* newSoftKeyMask, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
#endif
  void setBackgroundColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  bool moveChildLocation(iVtObject_c* apc_childObject, int16_t dx,  int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  bool setChildPosition( iVtObject_c* apc_childObject, int16_t  x,  int16_t y,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const {
    return vtObjectDataMask_c::updateObjectType();
  }
  */

  Colour updateBackgroundColour(bool b_SendRequest=false) override;
  iVtObjectSoftKeyMask_c* updateSoftKeyMask(bool b_SendRequest=false) override;
#endif

  ObjectType getObjectType() const override;
};

} // IsoAgLib

#endif //IVTOBJECTDATAMASK_C_H
