/*
  ivtobjectcontainer_c.cpp

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
#ifndef IVTOBJECTCONTAINER_C_H
#define IVTOBJECTCONTAINER_C_H

#include "impl/vtobjectcontainer_c.h"


#ifdef USE_VTOBJECT_container

namespace IsoAgLib {

class iVtObjectContainer_c : public __IsoAgLib::vtObjectContainer_c
{
public:
  ~iVtObjectContainer_c();

  static uint16_t objectType() { return VT_OBJECT_TYPE_CONTAINER; }



  void hide(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) { vtObjectContainer_c::hideShow (0, b_updateObject, b_enableReplaceOfCmd); }
  void show(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) { vtObjectContainer_c::hideShow (1, b_updateObject, b_enableReplaceOfCmd); }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectContainer_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectContainer_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectContainer_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const {
    return vtObjectContainer_c::updateObjectType();
  }


  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectContainer_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectContainer_c::updateHeight(b_SendRequest);
  }

  uint8_t updateHidden(bool b_SendRequest=false) {
    return vtObjectContainer_c::updateHidden(b_SendRequest);
  }
  */
#endif //CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif //USE_VTOBJECT_container

#endif //IVTOBJECTCONTAINER_C_H
