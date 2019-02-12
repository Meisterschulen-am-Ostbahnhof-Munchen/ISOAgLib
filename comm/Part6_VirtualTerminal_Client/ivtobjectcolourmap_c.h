/*
  ivtobjectcolourmap_c.h

  (C) Copyright 2009 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTCOLOURMAP_C_H
#define IVTOBJECTCOLOURMAP_C_H

#include "impl/vtobjectcolourmap_c.h"


namespace IsoAgLib {


class iVtObjectColourMap_c : public __IsoAgLib::vtObjectColourMap_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_COLOUR_MAP; }

  void init(const iVtObjectColourMap_s* vtObjectColourMapSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectColourMap_c::init (vtObjectColourMapSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  const iVtObjectColourMap_s& get_vtObjectColourMap_a() { return *vtObjectColourMap_c::get_vtObjectColourMap_a(); }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectColourMap_c::updateObjectType(); }
  */

  uint16_t updateNumOfColourIdxToFollow(bool b_SendRequest=false) {
    return vtObjectColourMap_c::updateNumOfColourIdxToFollow(b_SendRequest);
  }

#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif
