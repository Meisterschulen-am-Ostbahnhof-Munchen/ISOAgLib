/*
  vtobjectinputattributes_c.cpp

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

#include "vtobjectinputattributes_c.h"

#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {



struct vtObjectInputAttributes_c::iVtObjectInputAttributes_s : iVtObjectwMacro_s {
  uint8_t validationType;
  uint8_t length;
  char* validationString;
  iVtObjectInputAttributes_s(
		IsoAgLib::ObjectID ID,
  		uint8_t validationType,
		uint8_t length,
		char *validationString)
  : iVtObject_s(ID)
  , iVtObjectwMacro_s(ID)
  , validationType(validationType)
  , length(length)
  , validationString(validationString)
  {}
};


int16_t
vtObjectInputAttributes_c::stream(uint8_t* destMemory,
                                  uint16_t maxBytes,
                                  objRange_t sourceOffset)
{
    uint16_t curBytes=0; /* current bytes written */;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 26; // Object Type = Input Attributes
      destMemory [3] = vtObject_a->validationType;

      destMemory [4] = vtObject_a->length;
      sourceOffset += 5;
      curBytes += 5;
    }

    while ((sourceOffset >= 5U) && (sourceOffset < (5U+vtObject_a->length)) && ((curBytes+1) <= maxBytes))
    {
      if (vtObject_a->validationString == NULL)
          destMemory [curBytes] = 0x00;
      else
          destMemory [curBytes] = vtObject_a->validationString [sourceOffset-5];

      curBytes++;
      sourceOffset++;
    }

    if ((sourceOffset == (5U + vtObject_a->length)) && ((curBytes+1) <= maxBytes))
    {
      destMemory [curBytes] = vtObject_a->numberOfMacrosToFollow;
      curBytes++;
      sourceOffset++;
    }
    MACRO_streamEventMacro(6U+vtObject_a->length);
    return curBytes;
}


vtObjectInputAttributes_c::~vtObjectInputAttributes_c() = default;

IsoAgLib::ObjectID vtObjectInputAttributes_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectInputAttributes_c::fitTerminal() const
{
  return 6+vtObject_a->length+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectInputAttributes_c::setValidationStringCopy(const char* newValidationString, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    char *dest = vtObject_a->validationString;
    const char *src = newValidationString;
    int copyLen = (CNAMESPACE::strlen (newValidationString) <= vtObject_a->length) ? CNAMESPACE::strlen (newValidationString) : vtObject_a->length;
    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    for (; i<vtObject_a->length; i++) *dest++ = ' ';
    *dest = 0x00; // 0-termiante!
  }

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValue (this, newValidationString, vtObject_a->length, b_enableReplaceOfCmd);
}


void
vtObjectInputAttributes_c::setValidationStringRef(char* newValidationString, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    // delete RAM_String first, before we lose the pointer!
      delete (vtObject_a->validationString);

    vtObject_a->validationString = newValidationString;
  }

  const uint16_t ui16_newValStrLen = uint16_t( CNAMESPACE::strlen (newValidationString) );
  const uint16_t ui16_inAttrLen = uint16_t( vtObject_a->length );
  const uint16_t ui16_tempLen = (ui16_newValStrLen <= ui16_inAttrLen) ? ui16_newValStrLen : ui16_inAttrLen;
  __IsoAgLib::getVtClientInstance4Comm().getClientByID(s_properties.clientId).commandHandler().sendCommandChangeStringValueRef(this, newValidationString, ui16_tempLen, b_enableReplaceOfCmd);
}



const char*
vtObjectInputAttributes_c::getString()
{
  return vtObject_a->validationString;
}
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
/** that attribute is in parentheses in the spec, so commented out here
uint8_t
vtObjectInputAttributes_c::updateValidationType(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputAttributes_a(), validationType), sizeof(iVtObjectInputAttributes_s), 1);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputAttributes_a(), validationType), sizeof(iVtObjectInputAttributes_s));
}
*/

void
vtObjectInputAttributes_c::saveReceivedAttribute(uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/)
{
  /** that attribute is in parentheses in the spec, so commented out here
  if (attrID == 1)
    saveValue8(MACRO_getStructOffset(get_vtObjectInputAttributes_a(), validationType), sizeof(iVtObjectInputAttributes_s), convertLittleEndianStringUi8(pui8_attributeValue));
  */
}
#endif
    vtObjectInputAttributes_c::vtObjectInputAttributes_c(
            vtObjectInputAttributes_c::iVtObjectInputAttributes_s *vtObjectInputAttributesSROM, int ai_multitonInst)
            :iVtObjectString_c(ai_multitonInst)
    		,vtObject_a(vtObjectInputAttributesSROM)
    {}



} // __IsoAgLib
