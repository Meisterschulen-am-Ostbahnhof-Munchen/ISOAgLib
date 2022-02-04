/*
  ivtobjectlineattributes_c.h

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
#ifndef IVTOBJECTLINEATTRIBUTES_C_H
#define IVTOBJECTLINEATTRIBUTES_C_H

#include "impl/vtobjectlineattributes_c.h"


namespace IsoAgLib {

class iVtObjectLineAttributes_c : public __IsoAgLib::vtObjectLineAttributes_c
{
public:
  static uint16_t objectType();

  iVtObjectLineAttributes_c(
		iVtClientObjectPool_c* pool,
  		ObjectID ID = autoID,
  		uint8_t lineColour = 0,
		uint8_t lineWidth = 1,
		uint16_t lineArt = 65535);

  void setLineColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setLineWidth(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setLineArt(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setLineAttributes(Colour newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectLineAttributes_c::updateObjectType(); }
  */

  uint8_t updateLineColour(bool b_SendRequest=false);
  uint8_t updateLineWidth(bool b_SendRequest=false);
  uint16_t updateLineArt(bool b_SendRequest=false);
#endif

  virtual uint16_t getObjectType() const;
};

} // IsoAgLib

#endif
