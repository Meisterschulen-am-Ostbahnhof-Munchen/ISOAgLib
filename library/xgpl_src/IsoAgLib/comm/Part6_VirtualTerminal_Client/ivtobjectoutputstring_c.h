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


#ifdef CONFIG_USE_VTOBJECT_outputstring

#include "ivtclient_c.h"

namespace IsoAgLib {

class iVtObjectOutputString_c : public __IsoAgLib::vtObjectOutputString_c
{
public:
  ~iVtObjectOutputString_c() override;
  iVtObjectOutputString_c() = delete;

  static ObjectType objectType();



  explicit iVtObjectOutputString_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID = autoID,
  	    uint16_t width = 50,
  	    uint16_t height= 20,
  	    Colour backgroundColour = WHITE,
  	    iVtObjectFontAttributes_c* fontAttributes = nullptr,
		iVtObjectStringOptions options = iVtObjectStringOptions(soallOptionsOff),
  	    iVtObjectStringVariable_c* variableReference = nullptr,
  	    Justification justification = Justification(TopLeft),
  	    char* value = nullptr /* size length+1 (0-termination intern!) */
  		);


  void setValueCopy(char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;

#ifdef USE_VT_UNICODE_SUPPORT
  void setValueCopyUTF8(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  void setValueCopyUTF16(const char* newValue, uint16_t length, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#endif

  void setValueRef(char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  void setVariableReference(iVtObjectStringVariable_c* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  const char* getString() override;
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setBackgroundColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setFontAttributes(iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setOptions(iVtObjectStringOptions newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setJustification(Justification newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectOutputString_c::updateObjectType(); }
   */

  uint16_t updateWidth(bool b_SendRequest=false) override;
  uint16_t updateHeight(bool b_SendRequest=false) override;
  Colour updateBackgroundColour(bool b_SendRequest=false) override;
  iVtObjectFontAttributes_c* updateFontAttributes(bool b_SendRequest=false) override;
  iVtObjectStringOptions updateOptions(bool b_SendRequest=false) override;
  iVtObjectStringVariable_c* updateVariableReference(bool b_SendRequest=false) override;
  Justification updateJustification(bool b_SendRequest=false) override;
#endif

  ObjectType getObjectType() const override;
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_outputstring

#endif //IVTOBJECTOUTPUTSTRING_C_H
