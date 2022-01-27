/*
  vtobjectauxiliarypointer_c.h - actually
  "vtobjectauxiliarycontroldesignatorobjectpointer_c.h" but shortened
  because of problems with the filesystem's name/path-lengths.

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
#ifndef VTOBJECTAUXILIARYCONTROLDESIGNATOROBJECTPOINTER_C_H
#define VTOBJECTAUXILIARYCONTROLDESIGNATOROBJECTPOINTER_C_H

#include <IsoAgLib/isoaglib_config.h>




#include "vtobject_c.h"
#include "vttypes.h"
#include "vtclientconnection_c.h"

namespace __IsoAgLib {

class vtObjectAuxiliaryControlDesignatorObjectPointer_c : public vtObject_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  void init(const iVtObjectAuxiliaryControlDesignatorObjectPointer_s* vtObjectAuxiliaryControlDesignatorObjectPointerSROM , int ai_multitonInst)
  {vtObject_c::init((iVtObject_s *)vtObjectAuxiliaryControlDesignatorObjectPointerSROM , ai_multitonInst);}

  iVtObjectAuxiliaryControlDesignatorObjectPointer_s* get_vtObjectAuxiliaryControlDesignatorObjectPointer_a() { return (iVtObjectAuxiliaryControlDesignatorObjectPointer_s *)&(get_vtObject_a()); }

  vtObjectAuxiliaryControlDesignatorObjectPointer_c();
  ~vtObjectAuxiliaryControlDesignatorObjectPointer_c() {}

  uint32_t fitTerminal() const;

  void setAuxiliaryObjectId(IsoAgLib::iVtObject_c* apc_newObject, bool b_updateObject= false, bool b_enableReplaceOfCmd=false)
  {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectAuxiliaryControlDesignatorObjectPointer_a(), value) : 0, sizeof(iVtObjectAuxiliaryControlDesignatorObjectPointer_s), 2 /* "auxiliary object ID Attribute" */, apc_newObject, b_enableReplaceOfCmd);
  }


#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 33; }
  */
  
  uint16_t updateValue(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif


