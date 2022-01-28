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
  iVtObjectOutputString_c(){}
  ~iVtObjectOutputString_c(){}

  static uint16_t objectType() { return VT_OBJECT_TYPE_OUTPUT_STRING; }


  iVtObjectOutputString_c(const iVtObjectOutputString_s* vtObjectOutputStringSROM , int ai_multitonInst)
  : __IsoAgLib::vtObjectOutputString_c(vtObjectOutputStringSROM , ai_multitonInst)
  {}



  iVtObjectOutputString_c(
		int ai_multitonInst,
  	    uint16_t ID = 0,
  	    uint16_t width = 50,
  	    uint16_t height= 20,
  	    uint8_t backgroundColour = 1,
  	    iVtObjectFontAttributes_c* fontAttributes = nullptr,
  	    uint8_t options = 0,
  	    iVtObjectStringVariable_c* variableReference = nullptr,
  	    uint8_t horizontalJustification = 0,
  	    uint16_t length = 0,
  	    char* value = nullptr, /* size length+1 (0-termination intern!) */
  	    uint8_t numberOfMacrosToFollow = 0,
  	    const repeat_event_iVtObjectMacro_s* macrosToFollow = nullptr
  		)
  :vtObjectOutputString_c(
		  new iVtObjectOutputString_s(
			  	    ID,
			  	    width,
			  	    height,
			  	    backgroundColour,
			  	    fontAttributes,
			  	    options,
			  	    variableReference,
			  	    horizontalJustification,
			  	    length,
			  	    value, /* size length+1 (0-termination intern!) */
			  	    numberOfMacrosToFollow,
			  	    macrosToFollow
				  )
  	  	  ,ai_multitonInst)
  {}





  void init(const iVtObjectOutputString_s* vtObjectOutputStringSROM , int ai_multitonInst)
  {
     vtObjectOutputString_c::init (vtObjectOutputStringSROM , ai_multitonInst);
   }


  const iVtObjectOutputString_s& get_vtObjectOutputString_a() { return *vtObjectOutputString_c::get_vtObjectOutputString_a(); }

  void setValueCopy(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setValueCopy (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef USE_VT_UNICODE_SUPPORT
  void setValueCopyUTF8(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setValueCopyUTF8 (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setValueCopyUTF16(const char* newValue, uint16_t length, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setValueCopyUTF16 (newValue, length, b_updateObject, b_enableReplaceOfCmd);
  }
#endif

  void setValueRef(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setValueRef (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setVariableReference(iVtObjectStringVariable_c* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false)  {
    vtObjectOutputString_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  const char* getString() { return vtObjectOutputString_c::getString(); }


  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setFontAttributes(iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setFontAttributes (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setHorizontalJustification (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputString_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectOutputString_c::updateObjectType(); }
   */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateHeight(b_SendRequest);
  }

  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateBackgroundColour(b_SendRequest);
  }

  uint16_t updateFontAttributes(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateFontAttributes(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateOptions(b_SendRequest);
  }

  uint16_t updateVariableReference(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateVariableReference(b_SendRequest);
  }

  uint8_t updateJustification(bool b_SendRequest=false) {
    return vtObjectOutputString_c::updateJustification(b_SendRequest);
  }
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif

