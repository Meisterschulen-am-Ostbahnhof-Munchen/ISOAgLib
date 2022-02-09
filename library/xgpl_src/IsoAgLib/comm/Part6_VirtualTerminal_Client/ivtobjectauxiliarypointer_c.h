/*
  ivtobjectauxiliarypointer_c.h

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
#ifndef IVTOBJECTAUXILIARYCONTROLDESIGNATOROBJECTPOINTER_C_H
#define IVTOBJECTAUXILIARYCONTROLDESIGNATOROBJECTPOINTER_C_H

#include "impl/vtobjectauxiliarypointer_c.h"


#ifdef CONFIG_USE_VTOBJECT_auxiliarycontroldesignatorobjectpointer

namespace IsoAgLib {

class iVtObjectAuxiliaryControlDesignatorObjectPointer_c : public __IsoAgLib::vtObjectAuxiliaryControlDesignatorObjectPointer_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_AUXILIARY_POINTER; }

  ~iVtObjectAuxiliaryControlDesignatorObjectPointer_c(){}

  void setAuxiliaryObjectId(iVtObject_c* apc_newObject, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectAuxiliaryControlDesignatorObjectPointer_c::setAuxiliaryObjectId (apc_newObject, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputAttributes_c::updateObjectType(); }
  
  uint16_t updateValue(bool b_SendRequest=false) {
    return vtObjectInputAttributes_c::getValue(b_SendRequest);
  }
  */
#endif //CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  
  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif // CONFIG_USE_VTOBJECT_auxiliarycontroldesignatorobjectpointer

#endif // IVTOBJECTAUXILIARYCONTROLDESIGNATOROBJECTPOINTER_C_H
