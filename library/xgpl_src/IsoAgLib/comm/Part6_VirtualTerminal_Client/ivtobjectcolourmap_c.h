/*
  ivtobjectcolourmap_c.h

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
#ifndef IVTOBJECTCOLOURMAP_C_H
#define IVTOBJECTCOLOURMAP_C_H

#include "impl/vtobjectcolourmap_c.h"


namespace IsoAgLib {


class iVtObjectColourMap_c : public __IsoAgLib::vtObjectColourMap_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_COLOUR_MAP; }


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectColourMap_c::updateObjectType(); }
  */

  uint16_t updateNumOfColourIdxToFollow(bool b_SendRequest=false) {
    //return vtObjectColourMap_c::updateNumOfColourIdxToFollow(b_SendRequest);
	  return 0;
  }

#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif
