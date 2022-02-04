/*
  ivtobjectfontattributes_c.h

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
#ifndef IVTOBJECTFONTATTRIBUTES_C_H
#define IVTOBJECTFONTATTRIBUTES_C_H

#include "impl/vtobjectfontattributes_c.h"


namespace IsoAgLib {

class iVtObjectFontAttributes_c : public __IsoAgLib::vtObjectFontAttributes_c
{
public:
  iVtObjectFontAttributes_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID = autoID,
		Colour fontColour = BLACK,
		uint8_t fontSize = 1,
		uint8_t fontType = 0, // always =0 ISO_LATIN_1
		uint8_t fontStyle = 0);

  static uint16_t objectType();

  virtual ~iVtObjectFontAttributes_c();


  uint16_t getScaledWidthHeight();

  void setFontColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFontSize(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFontType(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFontStyle(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFontAttributes(uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectFontAttributes_c::updateObjectType(); }
  */
  uint8_t updateFontColour(bool b_SendRequest=false);
  uint8_t updateFontSize(bool b_SendRequest=false);
  uint8_t updateFontType(bool b_SendRequest=false);
  uint8_t updateFontStyle(bool b_SendRequest=false);
#endif

  uint16_t getObjectType() const override;
};

} // IsoAgLib

#endif
