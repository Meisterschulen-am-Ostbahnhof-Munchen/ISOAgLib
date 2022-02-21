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

#include "vtobjectinputstring_c.h"

#ifdef CONFIG_USE_VTOBJECT_inputstring
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"
#include <IsoAgLib/util/impl/util_funcs.h>


namespace __IsoAgLib {

enum vtObjectInputString_c::AttributeID:uint8_t
{
	Width                   = 1,
	Height                  = 2,
	BackgroundColour        = 3,
	FontAttributes          = 4,
	InputAttributes         = 5,
	Options                 = 6,
	VariableReference       = 7,
	Justification           = 8,
};



struct vtObjectInputString_c::iVtObjectInputString_s: iVtObjectString_s, iVtObjectwMacro_s {
		uint16_t width;
		uint16_t height;
		IsoAgLib::Colour backgroundColour;
		IsoAgLib::iVtObjectFontAttributes_c* fontAttributes;
		IsoAgLib::iVtObjectInputAttributes_c* inputAttributes;
		IsoAgLib::iVtObjectStringOptions options;
		IsoAgLib::iVtObjectStringVariable_c* variableReference;
		IsoAgLib::Justification justification;
		IsoAgLib::Enabled enabled;
		iVtObjectInputString_s(
				IsoAgLib::ObjectID ID,
				uint16_t width,
				uint16_t height,
				IsoAgLib::Colour backgroundColour,
				IsoAgLib::iVtObjectFontAttributes_c* fontAttributes,
				IsoAgLib::iVtObjectInputAttributes_c* inputAttributes,
				IsoAgLib::iVtObjectStringOptions options,
				IsoAgLib::iVtObjectStringVariable_c* variableReference,
				IsoAgLib::Justification justification,
				char *value,
				IsoAgLib::Enabled enabled)
		: iVtObject_s(ID)
		, iVtObjectString_s(ID, value)
		, iVtObjectwMacro_s(ID)
		, width(width)
		, height(height)
		, backgroundColour(backgroundColour)
		, fontAttributes(fontAttributes)
		, inputAttributes(inputAttributes)
		, options(options)
		, variableReference(variableReference)
		, justification(justification)
		, enabled(enabled)
		{
		}
	};

uint16_t
vtObjectInputString_c::stream(uint8_t* destMemory,
                               uint16_t maxBytes,
                               objRange_t sourceOffset)
{
    uint16_t curBytes=0; /* current bytes written */;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObject_a->width;
    uint32_t height = (uint32_t)vtObject_a->height;
    MACRO_scaleSizeI32(width, height);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = VT_OBJECT_TYPE_INPUT_STRING; // Object Type = Input String
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [8] = vtObject_a->fontAttributes->getID() & 0xFF;
      destMemory [9] = vtObject_a->fontAttributes->getID() >> 8;
      if (vtObject_a->inputAttributes != nullptr)
      {
        destMemory [10] = vtObject_a->inputAttributes->getID() & 0xFF;
        destMemory [11] = vtObject_a->inputAttributes->getID() >> 8;
      }
      else
      {
        destMemory [10] = 0xFF;
        destMemory [11] = 0xFF;
      }
      destMemory [12] = vtObject_a->options.options;
      if (vtObject_a->variableReference != nullptr) {
        destMemory [13] = vtObject_a->variableReference->getID() & 0xFF;
        destMemory [14] = vtObject_a->variableReference->getID() >> 8;
      } else {
        destMemory [13] = 0xFF;
        destMemory [14] = 0xFF;
      }
      destMemory [15] = vtObject_a->justification.justification;
      destMemory [16] = static_cast<uint8_t>(vtObject_a->length);  // Length not greater than 255!
      sourceOffset += 17;
      curBytes += 17;
    }

    while ((sourceOffset >= 17U) && (sourceOffset < (17U+vtObject_a->length)) && ((curBytes+1) <= maxBytes)) {
      if (vtObject_a->value == nullptr)
        destMemory [curBytes] = 0x00;
      else
        destMemory [curBytes] = vtObject_a->value [sourceOffset-17];
      curBytes++;
      sourceOffset++;
    }

    if ((sourceOffset == (17U + vtObject_a->length)) && ((curBytes+1) <= maxBytes)) {
      destMemory [curBytes] = vtObject_a->enabled;
      curBytes++;
      sourceOffset++;
      destMemory [curBytes] = vtObject_a->numberOfMacrosToFollow;
      curBytes++;
      sourceOffset++;
    }
    MACRO_streamEventMacro(19U+vtObject_a->length);
    return curBytes;
}




vtObjectInputString_c::vtObjectInputString_c(
		multiton ai_multitonInst,
		IsoAgLib::ObjectID ID,
	    uint16_t width,
	    uint16_t height,
		IsoAgLib::Colour backgroundColour,
		IsoAgLib::iVtObjectFontAttributes_c* fontAttributes,
		IsoAgLib::iVtObjectInputAttributes_c* inputAttributes,
		IsoAgLib::iVtObjectStringOptions options,
		IsoAgLib::iVtObjectStringVariable_c* variableReference,
		IsoAgLib::Justification justification,
	    char* value, /* size length+1 (0-termination intern!) */
		IsoAgLib::Enabled enabled
		)
:vtObjectInputString_c(
		new iVtObjectInputString_s(
				ID,
				width,
				height,
				backgroundColour,
				fontAttributes,
				inputAttributes,
				options,
				variableReference,
				justification,
				value,
				enabled),
		ai_multitonInst)
{
}









IsoAgLib::ObjectID vtObjectInputString_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectInputString_c::fitTerminal() const
{
  return 19+vtObject_a->length+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectInputString_c::updateEnable(IsoAgLib::Enabled aui8_enOrDis)
{
	vtObject_a->enabled = aui8_enOrDis;
}


#ifdef USE_VT_UNICODE_SUPPORT
// Operation : setValueCopyUTF8
//! @param newValue:
//! @param b_updateObject:
void
vtObjectInputString_c::setValueCopyUTF8 (const char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  const uint16_t cui16_strLen = (uint16_t)std::strlen (newValue);
  char* pc_iso8859 = new char [cui16_strLen+1];

  //TODO
  const uint8_t cui8_fontType = 0;
  //cui8_fontType = vtObject_a->fontAttributes->get_vtObjectFontAttributes_a().fontType;

  convertStringUnicodeTo8859 (newValue, cui16_strLen, pc_iso8859, cui8_fontType);

  setValueCopy (pc_iso8859, b_updateObject, b_enableReplaceOfCmd);
  delete[] pc_iso8859;
}

void
vtObjectInputString_c::setValueCopyUTF16 (const char* newValue, uint16_t length, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  int copyLen = ( length <= vtObject_a->length) ? length : vtObject_a->length;

  // UTF-16 string must be a multiple of 2 bytes long.
  if( copyLen % 2 )
   copyLen--;

  if (b_updateObject) {
    // check if not already RAM string buffer?
    if (!(s_properties.flags & FLAG_STRING_IN_RAM)) {
      s_properties.flags |= FLAG_STRING_IN_RAM;
      // create new String buffer with same length as original one, as the size can't be changed !!
      char *newStringBuffer = new char [vtObject_a->length+1];
      saveValueP (MACRO_getStructOffset(get_vtObjectInputString_a(), value = (IsoAgLib::iVtObject_c*) newStringBuffer);
    }
    char *dest = vtObject_a->value;
    const char *src = newValue;

    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    spacePadBomUTF16( dest, copyLen, vtObject_a->length );
  }

  getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValueUTF16 (getID(), newValue, copyLen, b_enableReplaceOfCmd);

}
#endif


void
vtObjectInputString_c::setValueCopy(char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (vtObject_a->variableReference != nullptr) {
    // register error!!
    return;
  }

  if (b_updateObject) {
      // create new String buffer with same length as original one, as the size can't be changed !!
      char *newStringBuffer = new char [vtObject_a->length+1];
      vtObject_a->value = newStringBuffer;
    char *dest = vtObject_a->value;
    const char *src = newValue;
    int copyLen = (std::strlen (newValue) <= vtObject_a->length) ? std::strlen (newValue) : vtObject_a->length;
    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    for (; i<vtObject_a->length; i++) *dest++ = ' ';
    *dest = 0x00; // 0-terminate!
  }

  getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValue (this, newValue, vtObject_a->length, b_enableReplaceOfCmd);
}


void
vtObjectInputString_c::setValueRef(char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (vtObject_a->variableReference != nullptr) {
    // register error!!
    return;
  }

  if (b_updateObject) {
    // delete RAM_String first, before we lose the pointer!
      delete (vtObject_a->value);
    vtObject_a->value = newValue;
  }

  uint16_t ui16_tempLen = 0;
  if (newValue != nullptr ) ui16_tempLen = uint16_t( (std::strlen (newValue) <= vtObject_a->length) ? std::strlen (newValue) : vtObject_a->length );
  getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValueRef( this, newValue, ui16_tempLen, b_enableReplaceOfCmd );
}



const char*
vtObjectInputString_c::getString()
{
  return vtObject_a->value;
}

void
vtObjectInputString_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	  vtObject_a->width = newWidth;
	  vtObject_a->height = newHeight;
  }

  scaleSize( newWidth, newHeight );

  getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize(
    this, newWidth, newHeight, b_enableReplaceOfCmd );
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectInputString_c::updateWidth(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Width);
	return vtObject_a->width;
}

uint16_t
vtObjectInputString_c::updateHeight(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Height);
	return vtObject_a->height;
}

IsoAgLib::Colour
vtObjectInputString_c::updateBackgroundColour(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(BackgroundColour);
	return vtObject_a->backgroundColour;
}

IsoAgLib::iVtObjectFontAttributes_c*
vtObjectInputString_c::updateFontAttributes(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(FontAttributes);
	return vtObject_a->fontAttributes;
}

IsoAgLib::iVtObjectInputAttributes_c*
vtObjectInputString_c::updateInputAttributes(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(InputAttributes);
	return vtObject_a->inputAttributes;
}

IsoAgLib::iVtObjectStringOptions
vtObjectInputString_c::updateOptions(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Options);
	return vtObject_a->options;
}

IsoAgLib::iVtObjectStringVariable_c*
vtObjectInputString_c::updateVariableReference(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(VariableReference);
	return vtObject_a->variableReference;
}

IsoAgLib::Justification
vtObjectInputString_c::updateJustification(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Justification);
	return vtObject_a->justification;
}

/** that attribute is in parentheses in the spec, so commented out here
uint8_t
vtObjectInputString_c::updateEnabled(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputString_a(), enabled = 9);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputString_a(), enabled), sizeof(iVtObjectInputString_s));
}
*/

void
vtObjectInputString_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case Width:                   vtObject_a->width                   = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case Height:                  vtObject_a->height                  = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case BackgroundColour:        vtObject_a->backgroundColour        = convertLittleEndianStringColour(pui8_attributeValue); break;
    //case FontAttributes:          vtObject_a->fontAttributes          = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    //case InputAttributes:         vtObject_a->inputAttributes         = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case Options:                 vtObject_a->options.options         = (IsoAgLib::iVtObjectStringOptions_e)convertLittleEndianStringUi8(   pui8_attributeValue); break;
    //case VariableReference:       vtObject_a->variableReference       = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case Justification: vtObject_a->justification.justification = (IsoAgLib::AllJustification)convertLittleEndianStringUi8(   pui8_attributeValue); break;
    /** that attribute is in parentheses in the spec, so commented out here
    case 9:  saveValue8(MACRO_getStructOffset(get_vtObjectInputString_a(), enabled = convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
    vtObjectInputString_c::vtObjectInputString_c(
    		vtObjectInputString_c::iVtObjectInputString_s *vtObjectInputStringSROM, multiton ai_multitonInst)
            :iVtObjectString_c(ai_multitonInst)
    		,vtObject_a(vtObjectInputStringSROM)
    {}



    void vtObjectInputString_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->width = newValue;
    	setAttribute(Width, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->height = newValue;
    	setAttribute(Height, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->backgroundColour = newValue;
    	setAttribute(BackgroundColour, getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c *newFontAttributes, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fontAttributes = newFontAttributes;
    	setAttribute (FontAttributes, newFontAttributes->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setInputAttributes(IsoAgLib::iVtObjectInputAttributes_c *newInputAttributes, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->inputAttributes = newInputAttributes;
    	setAttribute (InputAttributes, newInputAttributes->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setOptions(IsoAgLib::iVtObjectStringOptions newOptions, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->options = newOptions;
    	setAttribute (Options, newOptions.options, b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setVariableReference(IsoAgLib::iVtObjectStringVariable_c *newVariableRef, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->variableReference = newVariableRef;
    	setAttribute (VariableReference, newVariableRef->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectInputString_c::setJustification(IsoAgLib::Justification newJustification, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->justification = newJustification;
    	setAttribute (Justification, newJustification.justification, b_enableReplaceOfCmd);
    }

    vtObjectInputString_c::~vtObjectInputString_c() = default;


    IsoAgLib::Enabled vtObjectInputString_c::getEnabled() {
        return vtObject_a->enabled;
    }

} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_inputstring
