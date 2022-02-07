/*
  vtobjectoutputstring_c.cpp

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

#include "../ivtobjectoutputstring_c.h"
#include "vtobjectoutputstring_c.h"


#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectstringvariable_c.h"
#include "../ivtobjectmacro_c.h"
#include <IsoAgLib/util/impl/util_funcs.h>
#include "vtclient_c.h"



namespace __IsoAgLib {



struct vtObjectOutputString_c::iVtObjectOutputString_s : iVtObjectString_s, iVtObjectwMacro_s {
  uint16_t width;
  uint16_t height;
  IsoAgLib::Colour backgroundColour;
  IsoAgLib::iVtObjectFontAttributes_c* fontAttributes;
  uint8_t options;
  IsoAgLib::iVtObjectStringVariable_c* variableReference;
  uint8_t horizontalJustification;
  uint16_t length;
  char* value; /* size length+1 (0-termination intern!) */
  explicit iVtObjectOutputString_s(
		IsoAgLib::ObjectID ID,
  	    uint16_t width,
  	    uint16_t height,
		IsoAgLib::Colour backgroundColour,
		IsoAgLib::iVtObjectFontAttributes_c* fontAttributes,
  	    uint8_t options,
		IsoAgLib::iVtObjectStringVariable_c* variableReference,
  	    uint8_t horizontalJustification,
  	    uint16_t length,
  	    char* value /* size length+1 (0-termination intern!) */
  		)
  : iVtObject_s(ID)
  , iVtObjectString_s(ID)
  , iVtObjectwMacro_s(ID)
  , width(width)
  , height(height)
  , backgroundColour(backgroundColour)
  , fontAttributes(fontAttributes)
  , options(options)
  , variableReference(variableReference)
  , horizontalJustification(horizontalJustification)
  , length(length)
  , value(value) /* size length+1 (0-termination intern!) */
	{}
};




int16_t
vtObjectOutputString_c::stream(uint8_t* destMemory,
                               uint16_t maxBytes,
                               objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectOutputString_a
#define MACRO_vtObjectTypeS iVtObjectOutputString_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObject_a->width;
    uint32_t height = (uint32_t)vtObject_a->height;
    MACRO_scaleSizeI32(width, height);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 11; // Object Type = Output String
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [8] = vtObject_a->fontAttributes->getID() & 0xFF;
      destMemory [9] = vtObject_a->fontAttributes->getID() >> 8;
      destMemory [10] = vtObject_a->options;
      if (vtObject_a->variableReference != NULL) {
        destMemory [11] = vtObject_a->variableReference->getID() & 0xFF;
        destMemory [12] = vtObject_a->variableReference->getID() >> 8;
      } else {
        destMemory [11] = 0xFF;
        destMemory [12] = 0xFF;
      }
      destMemory [13] = vtObject_a->horizontalJustification;
      destMemory [14] = vtObject_a->length & 0xFF;
      destMemory [15] = vtObject_a->length >> 8;
      sourceOffset += 16;
      curBytes += 16;
    }

    while ((sourceOffset >= 16U) && (sourceOffset < (16U+vtObject_a->length)) && ((curBytes+1) <= maxBytes)) {
      if (vtObject_a->value == NULL)
        destMemory [curBytes] = 0x00;
      else
        destMemory [curBytes] = vtObject_a->value [sourceOffset-16];
      curBytes++;
      sourceOffset++;
    }

    if ((sourceOffset == (16U + vtObject_a->length)) && ((curBytes+1) <= maxBytes)) {
      destMemory [curBytes] = vtObject_a->numberOfMacrosToFollow;
      curBytes++;
      sourceOffset++;
    }

    MACRO_streamEventMacro(17U+vtObject_a->length);
    return curBytes;
}

IsoAgLib::ObjectID vtObjectOutputString_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}






vtObjectOutputString_c::vtObjectOutputString_c(
		int ai_multitonInst,
		IsoAgLib::ObjectID ID,
		uint16_t width,
		uint16_t height,
		IsoAgLib::Colour backgroundColour,
		IsoAgLib::iVtObjectFontAttributes_c *fontAttributes,
		uint8_t options,
		IsoAgLib::iVtObjectStringVariable_c *variableReference,
		uint8_t horizontalJustification,
		char *value)
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
					strlen(value),
					value),
			ai_multitonInst)
{
}

vtObjectOutputString_c::vtObjectOutputString_c(iVtObjectOutputString_s* vtObjectOutputStringSROM , int ai_multitonInst)
: iVtObjectString_c((iVtObjectString_s*)vtObjectOutputStringSROM , ai_multitonInst)
, vtObject_a(vtObjectOutputStringSROM)
{}



uint32_t
vtObjectOutputString_c::fitTerminal() const
{
  return 17+vtObject_a->length+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectOutputString_c::setOriginSKM(bool b_SKM)
{
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    vtObject_a->fontAttributes->setOriginSKM (b_SKM);
  }
}

void
vtObjectOutputString_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn)
{
  if (p_btn)
  {
    p_parentButtonObject = p_btn;
    vtObject_a->fontAttributes->setOriginBTN (p_btn);
  }
}

#ifdef USE_VT_UNICODE_SUPPORT
void
vtObjectOutputString_c::setValueCopyUTF8 (const char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  const uint16_t cui16_strLen = (uint16_t)CNAMESPACE::strlen (newValue);
  char* pc_iso8859 = new char [cui16_strLen+1];

  // TODO !
  const uint8_t cui8_fontType = 0;
  //cui8_fontType = get_vtObjectOutputString_a()->fontAttributes->get_vtObjectFontAttributes_a().fontType;

  convertStringUnicodeTo8859 (newValue, cui16_strLen, pc_iso8859, cui8_fontType);

  setValueCopy (pc_iso8859, b_updateObject, b_enableReplaceOfCmd);
  delete[] pc_iso8859;
}

void
vtObjectOutputString_c::setValueCopyUTF16 (const char* newValue, uint16_t length, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  int copyLen = ( length <= get_vtObjectOutputString_a()->length) ? length : get_vtObjectOutputString_a()->length;

  // UTF-16 string must be a multiple of 2 bytes long.
  if( copyLen % 2 )
   copyLen--;

  if (b_updateObject) {
    // check if not already RAM string buffer?
    if (!(s_properties.flags & FLAG_STRING_IN_RAM)) {
      s_properties.flags |= FLAG_STRING_IN_RAM;
      // create new String buffer with same length as original one, as the size can't be changed !!
      char *newStringBuffer = new char [get_vtObjectOutputString_a()->length+1];
      saveValueP (MACRO_getStructOffset(get_vtObjectOutputString_a(), value), sizeof(iVtObjectOutputString_s), (IsoAgLib::iVtObject_c*) newStringBuffer);
    }
    char *dest = get_vtObjectOutputString_a()->value;
    const char *src = newValue;

    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    spacePadBomUTF16( dest, copyLen, get_vtObjectOutputString_a()->length );
  }

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValueUTF16 (getID(), newValue, copyLen, b_enableReplaceOfCmd);

}
#endif


void
vtObjectOutputString_c::setValueCopy(char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (vtObject_a->variableReference != NULL) {
    // register error!!
    return;
  }

  if (b_updateObject) {
    // check if not already RAM string buffer?
    if (!(s_properties.flags & FLAG_STRING_IN_RAM)) {
      s_properties.flags |= FLAG_STRING_IN_RAM;
      // create new String buffer with same length as original one, as the size can't be changed !!
      char *newStringBuffer = new char [get_vtObjectOutputString_a()->length+1];
      vtObject_a->value = newStringBuffer;
    }
    char *dest = get_vtObjectOutputString_a()->value;
    const char *src = newValue;
    int copyLen = (CNAMESPACE::strlen (newValue) <= get_vtObjectOutputString_a()->length) ? CNAMESPACE::strlen (newValue) : get_vtObjectOutputString_a()->length;
    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    for (; i<get_vtObjectOutputString_a()->length; i++) *dest++ = ' ';
    *dest = 0x00; // 0-termiante!
  }

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValue (this, newValue, get_vtObjectOutputString_a()->length, b_enableReplaceOfCmd);
}


void
vtObjectOutputString_c::setValueRef(char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (vtObject_a->variableReference != NULL) {
    // register error!!
    return;
  }

  if (b_updateObject) {
    // delete RAM_String first, before we lose the pointer!
    if (s_properties.flags & FLAG_STRING_IN_RAM) {
      delete (get_vtObjectOutputString_a()->value);
      s_properties.flags &= ~FLAG_STRING_IN_RAM;
    }

    vtObject_a->value = newValue;
  }

  uint16_t ui16_tempLen = 0;
  if (newValue != NULL ) ui16_tempLen = uint16_t( (CNAMESPACE::strlen (newValue) <= get_vtObjectOutputString_a()->length) ? CNAMESPACE::strlen (newValue) : get_vtObjectOutputString_a()->length );
  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValueRef ( this, newValue, ui16_tempLen, b_enableReplaceOfCmd);
}

/** obsolete, now no more checking for length != 0, set VaRef anyway!
void
vtObjectOutputString_c::setVariableReference(IsoAgLib::iVtObjectStringVariable_c* newVariable, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (vtObject_a->length != 0) {
    // register error!!
    return;
  }

  if (b_updateObject) saveValueP (MACRO_getStructOffset(get_vtObjectOutputString_a(), variableReference),  sizeof(iVtObjectOutputString_s), newVariable);

  uint16_t newVariableID = newVariable ? (newVariable->getID()) : 0xFFFF;
  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeAttribute (this, 6, (newVariableID & 0xFF), (newVariableID >> 8), 0, 0, b_enableReplaceOfCmd);
}
*/

void
vtObjectOutputString_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	  vtObject_a->width = newWidth;
	  vtObject_a->height = newHeight;
  }

  scaleSize( newWidth, newHeight );

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}


const char*
vtObjectOutputString_c::getString()
{
  return get_vtObjectOutputString_a()->value;
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectOutputString_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), width), sizeof(iVtObjectOutputString_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), width), sizeof(iVtObjectOutputString_s));
}

uint16_t
vtObjectOutputString_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), height), sizeof(iVtObjectOutputString_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), height), sizeof(iVtObjectOutputString_s));
}

uint8_t
vtObjectOutputString_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), backgroundColour), sizeof(iVtObjectOutputString_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), backgroundColour), sizeof(iVtObjectOutputString_s));
}

uint16_t
vtObjectOutputString_c::updateFontAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), fontAttributes), sizeof(iVtObjectOutputString_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), fontAttributes), sizeof(iVtObjectOutputString_s));
}

uint8_t
vtObjectOutputString_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), options), sizeof(iVtObjectOutputString_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), options), sizeof(iVtObjectOutputString_s));
}

uint16_t
vtObjectOutputString_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), variableReference), sizeof(iVtObjectOutputString_s), 6);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), variableReference), sizeof(iVtObjectOutputString_s));
}

uint8_t
vtObjectOutputString_c::updateJustification(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), horizontalJustification), sizeof(iVtObjectOutputString_s), 7);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), horizontalJustification), sizeof(iVtObjectOutputString_s));
}

void
vtObjectOutputString_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), width), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), height), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), backgroundColour), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), fontAttributes), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), options), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), variableReference), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 7: saveValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), horizontalJustification), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    default: break;
  }
}

#endif

vtObjectOutputString_c::iVtObjectOutputString_s *vtObjectOutputString_c::get_vtObjectOutputString_a()
{
	return vtObject_a;
}

    void vtObjectOutputString_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), width) : 0, sizeof(iVtObjectOutputString_s), 1, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputString_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), height) : 0, sizeof(iVtObjectOutputString_s), 2, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputString_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), backgroundColour) : 0, sizeof(iVtObjectOutputString_s), 3, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectOutputString_c::setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c *newValue, bool b_updateObject,
                                                   bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), fontAttributes) : 0, sizeof(iVtObjectOutputString_s), 4, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputString_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), options) : 0, sizeof(iVtObjectOutputString_s), 5, newValue, newValue, b_enableReplaceOfCmd);
    }

    void
    vtObjectOutputString_c::setVariableReference(IsoAgLib::iVtObjectStringVariable_c *newValue, bool b_updateObject,
                                                 bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), variableReference) : 0, sizeof(iVtObjectOutputString_s), 6, (iVtObject_c*)newValue, b_enableReplaceOfCmd);
    }

    void
    vtObjectOutputString_c::setHorizontalJustification(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputString_a(), horizontalJustification) : 0, sizeof(iVtObjectOutputString_s), 7, newValue, newValue, b_enableReplaceOfCmd);
    }





} //__IsoAgLib


