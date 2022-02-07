/*
  vtobjectinputstring_c.cpp

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

#include "../ivtobjectinputstring_c.h"
#include "vtobjectinputstring_c.h"


#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectinputattributes_c.h"
#include "../ivtobjectstringvariable_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"
#include <IsoAgLib/util/impl/util_funcs.h>


namespace __IsoAgLib {



struct vtObjectInputString_c::iVtObjectInputString_s: iVtObjectString_s, iVtObjectwMacro_s {
		uint16_t width;
		uint16_t height;
		IsoAgLib::Colour backgroundColour;
		IsoAgLib::iVtObjectFontAttributes_c *fontAttributes;
		IsoAgLib::iVtObjectInputAttributes_c *inputAttributes;
		uint8_t options;
		IsoAgLib::iVtObjectStringVariable_c *variableReference;
		uint8_t horizontalJustification;
		uint16_t length;
		char *value;
		uint8_t enabled;
		iVtObjectInputString_s(
				IsoAgLib::ObjectID ID,
				uint16_t width,
				uint16_t height,
				IsoAgLib::Colour backgroundColour,
				IsoAgLib::iVtObjectFontAttributes_c *fontAttributes,
				IsoAgLib::iVtObjectInputAttributes_c *inputAttributes,
				uint8_t options,
				IsoAgLib::iVtObjectStringVariable_c *variableReference,
				uint8_t horizontalJustification,
				uint16_t length,
				char *value,
				uint8_t enabled)
		: iVtObject_s(ID)
		, iVtObjectString_s(ID)
		, iVtObjectwMacro_s(ID)
		, width(width)
		, height(height)
		, backgroundColour(backgroundColour)
		, fontAttributes(fontAttributes)
		, inputAttributes(inputAttributes)
		, options(options)
		, variableReference(variableReference)
		, horizontalJustification(horizontalJustification)
		, length(length)
		, value(value)
		, enabled(enabled)
		{
		}
	};

int16_t
vtObjectInputString_c::stream(uint8_t* destMemory,
                               uint16_t maxBytes,
                               objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectInputString_a
#define MACRO_vtObjectTypeS iVtObjectInputString_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObjectInputString_a->width;
    uint32_t height = (uint32_t)vtObjectInputString_a->height;
    MACRO_scaleSizeI32(width, height);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 8; // Object Type = Input String
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObjectInputString_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [8] = vtObjectInputString_a->fontAttributes->getID() & 0xFF;
      destMemory [9] = vtObjectInputString_a->fontAttributes->getID() >> 8;
      if (vtObjectInputString_a->inputAttributes != NULL)
      {
        destMemory [10] = vtObjectInputString_a->inputAttributes->getID() & 0xFF;
        destMemory [11] = vtObjectInputString_a->inputAttributes->getID() >> 8;
      }
      else
      {
        destMemory [10] = 0xFF;
        destMemory [11] = 0xFF;
      }
      destMemory [12] = vtObjectInputString_a->options;
      if (vtObjectInputString_a->variableReference != NULL) {
        destMemory [13] = vtObjectInputString_a->variableReference->getID() & 0xFF;
        destMemory [14] = vtObjectInputString_a->variableReference->getID() >> 8;
      } else {
        destMemory [13] = 0xFF;
        destMemory [14] = 0xFF;
      }
      destMemory [15] = vtObjectInputString_a->horizontalJustification;
      destMemory [16] = static_cast<uint8_t>(vtObjectInputString_a->length);  // Length not greater than 255!
      sourceOffset += 17;
      curBytes += 17;
    }

    while ((sourceOffset >= 17U) && (sourceOffset < (17U+vtObjectInputString_a->length)) && ((curBytes+1) <= maxBytes)) {
      if (vtObjectInputString_a->value == NULL)
        destMemory [curBytes] = 0x00;
      else
        destMemory [curBytes] = vtObjectInputString_a->value [sourceOffset-17];
      curBytes++;
      sourceOffset++;
    }

    if ((sourceOffset == (17U + vtObjectInputString_a->length)) && ((curBytes+1) <= maxBytes)) {
      destMemory [curBytes] = vtObjectInputString_a->enabled;
      curBytes++;
      sourceOffset++;
      destMemory [curBytes] = vtObjectInputString_a->numberOfMacrosToFollow;
      curBytes++;
      sourceOffset++;
    }
    MACRO_streamEventMacro(19U+vtObjectInputString_a->length);
    return curBytes;
}

IsoAgLib::ObjectID vtObjectInputString_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectInputString_c::fitTerminal() const
{
  MACRO_localVars;
  return 19+vtObjectInputString_a->length+vtObjectInputString_a->numberOfMacrosToFollow*2;
}


void
vtObjectInputString_c::updateEnable(uint8_t aui8_enOrDis)
{
  saveValue8 (MACRO_getStructOffset(get_vtObjectInputString_a(), enabled), sizeof(iVtObjectInputString_s), aui8_enOrDis);
}


#ifdef USE_VT_UNICODE_SUPPORT
// Operation : setValueCopyUTF8
//! @param newValue:
//! @param b_updateObject:
void
vtObjectInputString_c::setValueCopyUTF8 (const char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  const uint16_t cui16_strLen = (uint16_t)CNAMESPACE::strlen (newValue);
  char* pc_iso8859 = new char [cui16_strLen+1];

  //TODO
  const uint8_t cui8_fontType = 0;
  //cui8_fontType = get_vtObjectInputString_a()->fontAttributes->get_vtObjectFontAttributes_a().fontType;

  convertStringUnicodeTo8859 (newValue, cui16_strLen, pc_iso8859, cui8_fontType);

  setValueCopy (pc_iso8859, b_updateObject, b_enableReplaceOfCmd);
  delete[] pc_iso8859;
}

void
vtObjectInputString_c::setValueCopyUTF16 (const char* newValue, uint16_t length, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  int copyLen = ( length <= get_vtObjectInputString_a()->length) ? length : get_vtObjectInputString_a()->length;

  // UTF-16 string must be a multiple of 2 bytes long.
  if( copyLen % 2 )
   copyLen--;

  if (b_updateObject) {
    // check if not already RAM string buffer?
    if (!(s_properties.flags & FLAG_STRING_IN_RAM)) {
      s_properties.flags |= FLAG_STRING_IN_RAM;
      // create new String buffer with same length as original one, as the size can't be changed !!
      char *newStringBuffer = new char [get_vtObjectInputString_a()->length+1];
      saveValueP (MACRO_getStructOffset(get_vtObjectInputString_a(), value), sizeof(iVtObjectInputString_s), (IsoAgLib::iVtObject_c*) newStringBuffer);
    }
    char *dest = get_vtObjectInputString_a()->value;
    const char *src = newValue;

    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    spacePadBomUTF16( dest, copyLen, get_vtObjectInputString_a()->length );
  }

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValueUTF16 (getID(), newValue, copyLen, b_enableReplaceOfCmd);

}
#endif


void
vtObjectInputString_c::setValueCopy(const char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  if (vtObjectInputString_a->variableReference != NULL) {
    // register error!!
    return;
  }

  if (b_updateObject) {
    // check if not already RAM string buffer?
    if (!(s_properties.flags & FLAG_STRING_IN_RAM)) {
      s_properties.flags |= FLAG_STRING_IN_RAM;
      // create new String buffer with same length as original one, as the size can't be changed !!
      char *newStringBuffer = new char [get_vtObjectInputString_a()->length+1];
      saveValueP (MACRO_getStructOffset(get_vtObjectInputString_a(), value), sizeof(iVtObjectInputString_s), (IsoAgLib::iVtObject_c*)newStringBuffer);
    }
    char *dest = get_vtObjectInputString_a()->value;
    const char *src = newValue;
    int copyLen = (CNAMESPACE::strlen (newValue) <= get_vtObjectInputString_a()->length) ? CNAMESPACE::strlen (newValue) : get_vtObjectInputString_a()->length;
    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    for (; i<get_vtObjectInputString_a()->length; i++) *dest++ = ' ';
    *dest = 0x00; // 0-termiante!
  }

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValue (this, newValue, get_vtObjectInputString_a()->length, b_enableReplaceOfCmd);
}


void
vtObjectInputString_c::setValueRef(const char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  if (vtObjectInputString_a->variableReference != NULL) {
    // register error!!
    return;
  }

  if (b_updateObject) {
    // delete RAM_String first, before we lose the pointer!
    if (s_properties.flags & FLAG_STRING_IN_RAM) {
      delete (get_vtObjectInputString_a()->value);
      s_properties.flags &= ~FLAG_STRING_IN_RAM;
    }

    saveValueP (MACRO_getStructOffset(get_vtObjectInputString_a(), value),  sizeof(iVtObjectInputString_s), (IsoAgLib::iVtObject_c*)newValue);
  }

  uint16_t ui16_tempLen = 0;
  if (newValue != NULL ) ui16_tempLen = uint16_t( (CNAMESPACE::strlen (newValue) <= get_vtObjectInputString_a()->length) ? CNAMESPACE::strlen (newValue) : get_vtObjectInputString_a()->length );
  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValueRef( this, newValue, ui16_tempLen, b_enableReplaceOfCmd );
}



const char*
vtObjectInputString_c::getString()
{
  return get_vtObjectInputString_a()->value;
}

void
vtObjectInputString_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	  vtObject_a->width = newWidth;
	  vtObject_a->height = newHeight;
  }

  scaleSize( newWidth, newHeight );

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize(
    this, newWidth, newHeight, b_enableReplaceOfCmd );
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectInputString_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputString_a(), width), sizeof(iVtObjectInputString_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), width), sizeof(iVtObjectInputString_s));
}

uint16_t
vtObjectInputString_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputString_a(), height), sizeof(iVtObjectInputString_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), height), sizeof(iVtObjectInputString_s));
}

uint8_t
vtObjectInputString_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputString_a(), backgroundColour), sizeof(iVtObjectInputString_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputString_a(), backgroundColour), sizeof(iVtObjectInputString_s));
}

uint16_t
vtObjectInputString_c::updateFontAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputString_a(), fontAttributes), sizeof(iVtObjectInputString_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), fontAttributes), sizeof(iVtObjectInputString_s));
}

uint16_t
vtObjectInputString_c::updateInputAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputString_a(), inputAttributes), sizeof(iVtObjectInputString_s), 5);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), inputAttributes), sizeof(iVtObjectInputString_s));
}

uint8_t
vtObjectInputString_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputString_a(), options), sizeof(iVtObjectInputString_s), 6);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), options), sizeof(iVtObjectInputString_s));
}

uint16_t
vtObjectInputString_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputString_a(), variableReference), sizeof(iVtObjectInputString_s), 7);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), variableReference), sizeof(iVtObjectInputString_s));
}

uint8_t
vtObjectInputString_c::updateJustification(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputString_a(), horizontalJustification), sizeof(iVtObjectInputString_s), 8);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), horizontalJustification), sizeof(iVtObjectInputString_s));
}

/** that attribute is in parentheses in the spec, so commented out here
uint8_t
vtObjectInputString_c::updateEnabled(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputString_a(), enabled), sizeof(iVtObjectInputString_s), 9);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), enabled), sizeof(iVtObjectInputString_s));
}
*/

void
vtObjectInputString_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), width), sizeof(iVtObjectInputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), height), sizeof(iVtObjectInputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectInputString_a(), backgroundColour), sizeof(iVtObjectInputString_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), fontAttributes), sizeof(iVtObjectInputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 5: saveValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), inputAttributes), sizeof(iVtObjectInputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 6: saveValue8(MACRO_getStructOffset(get_vtObjectInputString_a(), options), sizeof(iVtObjectInputString_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 7: saveValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), variableReference), sizeof(iVtObjectInputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 8: saveValue8(MACRO_getStructOffset(get_vtObjectInputString_a(), horizontalJustification), sizeof(iVtObjectInputString_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    /** that attribute is in parentheses in the spec, so commented out here
    case 9:  saveValue8(MACRO_getStructOffset(get_vtObjectInputString_a(), enabled), sizeof(iVtObjectInputString_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
    vtObjectInputString_c::vtObjectInputString_c(
    		vtObjectInputString_c::iVtObjectInputString_s *vtObjectInputStringSROM, int ai_multitonInst)
            :iVtObjectString_c((iVtObjectString_s*)vtObjectInputStringSROM , ai_multitonInst)
    		,vtObject_a(vtObjectInputStringSROM)
    {}

    vtObjectInputString_c::iVtObjectInputString_s *vtObjectInputString_c::get_vtObjectInputString_a()
    {
    	return vtObject_a;
    }

    void vtObjectInputString_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), width) : 0, sizeof(iVtObjectInputString_s), 1 /* "Width" */, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), height) : 0, sizeof(iVtObjectInputString_s), 2 /* "Height" */, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), backgroundColour) : 0, sizeof(iVtObjectInputString_s), 3 /* "Background Colour" */, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c *newFontAttributes,
                                                  bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), fontAttributes) : 0, sizeof(iVtObjectInputString_s), 4 /* "Font Attributes" */, newFontAttributes, b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setInputAttributes(IsoAgLib::iVtObjectInputAttributes_c *newInputAttributes,
                                                   bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), inputAttributes) : 0, sizeof(iVtObjectInputString_s), 5 /* "Input Attributes" */, newInputAttributes, b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setOptions(uint8_t newOptions, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), options) : 0, sizeof(iVtObjectInputString_s), 6 /* "Options" */, newOptions, newOptions, b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setVariableReference(IsoAgLib::iVtObjectStringVariable_c *newVariableRef,
                                                     bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), variableReference) : 0, sizeof(iVtObjectInputString_s), 7 /* "Variable Reference" */, newVariableRef, b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setHorizontalJustification(uint8_t newHorizontalJustification, bool b_updateObject,
                                                           bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), horizontalJustification) : 0, sizeof(iVtObjectInputString_s), 8 /* "Horizontal justification" */, newHorizontalJustification, newHorizontalJustification, b_enableReplaceOfCmd);
    }






} // __IsoAgLib


