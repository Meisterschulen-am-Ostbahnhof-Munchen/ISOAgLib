/*
  ivtobjectauxiliarycontroldesignatorobjectpointer_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTAUXILIARYCONTROLDESIGNATOROBJECTPOINTER_C_H
#define IVTOBJECTAUXILIARYCONTROLDESIGNATOROBJECTPOINTER_C_H

#include "impl/vtobjectauxiliarypointer_c.h"


#ifdef USE_VTOBJECT_auxiliarycontroldesignatorobjectpointer

namespace IsoAgLib {

class iVtObjectAuxiliaryControlDesignatorObjectPointer_c : public __IsoAgLib::vtObjectAuxiliaryControlDesignatorObjectPointer_c
{
public:
  ~iVtObjectAuxiliaryControlDesignatorObjectPointer_c(){};

  void init(const iVtObjectAuxiliaryControlDesignatorObjectPointer_s* vtObjectAuxiliaryControlDesignatorObjectPointerSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) { vtObject_c::init ((iVtObject_s*) vtObjectAuxiliaryControlDesignatorObjectPointerSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); };

  const iVtObjectAuxiliaryControlDesignatorObjectPointer_s& get_vtObjectAuxiliaryControlDesignatorObjectPointer_a() { return *vtObjectAuxiliaryControlDesignatorObjectPointer_c::get_vtObjectAuxiliaryControlDesignatorObjectPointer_a(); }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputAttributes_c::updateObjectType(); }
  
  uint16_t updateValue(bool b_SendRequest=false) {
    return vtObjectInputAttributes_c::getValue(b_SendRequest);
  }
  */
#endif
};

} // IsoAgLib

#endif

#endif
