/*
  ivtobjectoutputstring_c.h

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
#ifndef IVTOBJECTOUTPUTSTRING_C_H
#define IVTOBJECTOUTPUTSTRING_C_H

#include "impl/vtobjectoutputstring_c.h"



#include "ivtclient_c.h"

namespace IsoAgLib {

class iVtObjectOutputString_c : public __IsoAgLib::vtObjectOutputString_c
{
public:
  ~iVtObjectOutputString_c() override = default;

  static uint16_t objectType();



  iVtObjectOutputString_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID = autoID,
  	    uint16_t width = 50,
  	    uint16_t height= 20,
  	    uint8_t backgroundColour = 1,
  	    iVtObjectFontAttributes_c* fontAttributes = nullptr,
  	    uint8_t options = 0,
  	    iVtObjectStringVariable_c* variableReference = nullptr,
  	    uint8_t horizontalJustification = 0,
  	    char* value = nullptr /* size length+1 (0-termination intern!) */
  		);


  void setValueCopy(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;

#ifdef USE_VT_UNICODE_SUPPORT
  void setValueCopyUTF8(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  void setValueCopyUTF16(const char* newValue, uint16_t length, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#endif

  void setValueRef(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setVariableReference(iVtObjectStringVariable_c* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  const char* getString();
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setFontAttributes(iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectOutputString_c::updateObjectType(); }
   */

  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  uint8_t updateBackgroundColour(bool b_SendRequest=false);
  uint16_t updateFontAttributes(bool b_SendRequest=false);
  uint8_t updateOptions(bool b_SendRequest=false);
  uint16_t updateVariableReference(bool b_SendRequest=false);
  uint8_t updateJustification(bool b_SendRequest=false);
#endif

  virtual uint16_t getObjectType() const;
};

} // IsoAgLib

#endif

