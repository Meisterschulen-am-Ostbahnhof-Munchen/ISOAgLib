/*
  ivtobjectmacro_c.h

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
#ifndef IVTOBJECTMACRO_C_H
#define IVTOBJECTMACRO_C_H

#include "impl/vtobjectmacro_c.h"


namespace IsoAgLib {

class iVtObjectMacro_c : public __IsoAgLib::vtObjectMacro_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_MACRO; }

  void init(const iVtObjectMacro_s* vtObjectMacroSROM , int ai_multitonInst) {
    vtObjectMacro_c::init (vtObjectMacroSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  const iVtObjectMacro_s& get_vtObjectMacro_a() { return *vtObjectMacro_c::get_vtObjectMacro_a(); }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES  
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectMacro_c::updateObjectType(); }
  */
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif
