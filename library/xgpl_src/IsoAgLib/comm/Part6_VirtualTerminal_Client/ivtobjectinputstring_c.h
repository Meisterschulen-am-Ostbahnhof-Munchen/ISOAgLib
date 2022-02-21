/*
  ivtobjectinputstring_c.h

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
#ifndef IVTOBJECTINPUTSTRING_C_H
#define IVTOBJECTINPUTSTRING_C_H

#include "impl/vtobjectinputstring_c.h"


#ifdef CONFIG_USE_VTOBJECT_inputstring

namespace IsoAgLib {

class iVtObjectInputString_c : public __IsoAgLib::vtObjectInputString_c
{
public:

	~iVtObjectInputString_c() override;
	iVtObjectInputString_c() = delete;

	  explicit iVtObjectInputString_c(
			iVtClientObjectPool_c* pool,
			ObjectID ID = autoID,
	  	    uint16_t width = 50,
	  	    uint16_t height= 20,
	  	    Colour backgroundColour = WHITE,
	  	    iVtObjectFontAttributes_c* fontAttributes = nullptr,
			iVtObjectInputAttributes_c* inputAttributes = nullptr,
			iVtObjectStringOptions options = iVtObjectStringOptions(soallOptionsOff),
	  	    iVtObjectStringVariable_c* variableReference = nullptr,
	  	    Justification justification = Justification(TopLeft),
	  	    char* value = nullptr, /* size length+1 (0-termination intern!) */
			Enabled enabled = Enabled::enabled
	  		);



  static ObjectType objectType();
  const char* getString() override;
  void setValueRef(char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setValueCopy(char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

#ifdef USE_VT_UNICODE_SUPPORT
  void setValueCopyUTF8(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setValueCopyUTF8 (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setValueCopyUTF16(const char* newValue, uint16_t length, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputString_c::setValueCopyUTF16 (newValue, length, b_updateObject, b_enableReplaceOfCmd);
  }
#endif

  void setVariableReference(iVtObjectStringVariable_c* newVariable, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setBackgroundColour(Colour colorValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setFontAttributes(iVtObjectFontAttributes_c* newFontAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setInputAttributes(iVtObjectInputAttributes_c* newInputAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setOptions(iVtObjectStringOptions newOptions, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setJustification(Justification newJustification, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  bool enable(bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  bool disable(bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  Enabled getEnabled() override;
  bool select(uint8_t selectOrActivate) override;
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputString_c::updateObjectType(); }
  */
  
  uint16_t updateWidth(bool b_SendRequest=false) override;
  uint16_t updateHeight(bool b_SendRequest=false) override;
  Colour updateBackgroundColour(bool b_SendRequest=false) override;
  iVtObjectFontAttributes_c* updateFontAttributes(bool b_SendRequest=false) override;
  iVtObjectInputAttributes_c* updateInputAttributes(bool b_SendRequest=false) override;
  iVtObjectStringOptions updateOptions(bool b_SendRequest=false) override;
  iVtObjectStringVariable_c* updateVariableReference(bool b_SendRequest=false) override;
  Justification updateJustification(bool b_SendRequest=false) override;

  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateEnabled(bool b_SendRequest=false) {
    return vtObjectInputString_c::updateEnabled(b_SendRequest);
  }
  */
#endif

  ObjectType getObjectType() const override;
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_inputstring

#endif //IVTOBJECTINPUTSTRING_C_H
