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

#ifdef CONFIG_USE_VTOBJECT_outputstring
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectstringvariable_c.h"
#include "../ivtobjectmacro_c.h"
#include <IsoAgLib/util/impl/util_funcs.h>
#include "vtclient_c.h"



namespace __IsoAgLib {



enum vtObjectOutputString_c::AttributeID:uint8_t
{

    Width                   = 1,
    Height                  = 2,
	BackgroundColour        = 3,
	FontAttributes          = 4,
	Options                 = 5,
	VariableReference       = 6,
	HorizontalJustification = 7,
};


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
    uint16_t curBytes=0; /* current bytes written */;
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
: iVtObjectString_c(ai_multitonInst)
, vtObject_a(vtObjectOutputStringSROM)
{}

vtObjectOutputString_c::~vtObjectOutputString_c() = default;

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

#ifdef CONFIG_USE_VTOBJECT_button
void
vtObjectOutputString_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn)
{
  if (p_btn)
  {
    p_parentButtonObject = p_btn;
    vtObject_a->fontAttributes->setOriginBTN (p_btn);
  }
}
#endif //CONFIG_USE_VTOBJECT_button

#ifdef USE_VT_UNICODE_SUPPORT
void
vtObjectOutputString_c::setValueCopyUTF8 (const char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  const uint16_t cui16_strLen = (uint16_t)CNAMESPACE::strlen (newValue);
  char* pc_iso8859 = new char [cui16_strLen+1];

  // TODO !
  const uint8_t cui8_fontType = 0;
  //cui8_fontType = vtObject_a->fontAttributes->get_vtObjectFontAttributes_a().fontType;

  convertStringUnicodeTo8859 (newValue, cui16_strLen, pc_iso8859, cui8_fontType);

  setValueCopy (pc_iso8859, b_updateObject, b_enableReplaceOfCmd);
  delete[] pc_iso8859;
}

void
vtObjectOutputString_c::setValueCopyUTF16 (const char* newValue, uint16_t length, bool b_updateObject, bool b_enableReplaceOfCmd)
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
      saveValueP (MACRO_getStructOffset(get_vtObjectOutputString_a(), value), sizeof(iVtObjectOutputString_s), (IsoAgLib::iVtObject_c*) newStringBuffer);
    }
    char *dest = vtObject_a->value;
    const char *src = newValue;

    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    spacePadBomUTF16( dest, copyLen, vtObject_a->length );
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
    char *dest = vtObject_a->value;
    const char *src = newValue;
    int copyLen = (CNAMESPACE::strlen (newValue) <= vtObject_a->length) ? CNAMESPACE::strlen (newValue) : vtObject_a->length;
    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    for (; i<vtObject_a->length; i++) *dest++ = ' ';
    *dest = 0x00; // 0-termiante!
  }

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValue (this, newValue, vtObject_a->length, b_enableReplaceOfCmd);
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
      delete (vtObject_a->value);

    vtObject_a->value = newValue;
  }

  uint16_t ui16_tempLen = 0;
  if (newValue != NULL ) ui16_tempLen = uint16_t( (CNAMESPACE::strlen (newValue) <= vtObject_a->length) ? CNAMESPACE::strlen (newValue) : vtObject_a->length );
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
  return vtObject_a->value;
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectOutputString_c::updateWidth(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Width);
	return vtObject_a->width;
}

uint16_t
vtObjectOutputString_c::updateHeight(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Height);
	return vtObject_a->height;
}

IsoAgLib::Colour
vtObjectOutputString_c::updateBackgroundColour(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(BackgroundColour);
	return vtObject_a->backgroundColour;
}

uint16_t
vtObjectOutputString_c::updateFontAttributes(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(FontAttributes);
	return vtObject_a->backgroundColour;
}

uint8_t
vtObjectOutputString_c::updateOptions(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(Options);
	return vtObject_a->options;
}

IsoAgLib::iVtObjectStringVariable_c*
vtObjectOutputString_c::updateVariableReference(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(VariableReference);
	return vtObject_a->variableReference;
}

uint8_t
vtObjectOutputString_c::updateJustification(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(HorizontalJustification);
	return vtObject_a->horizontalJustification;
}


void
vtObjectOutputString_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case Width: vtObject_a->width                   = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case Height: vtObject_a->height                  = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case BackgroundColour: vtObject_a->backgroundColour        = convertLittleEndianStringColour(pui8_attributeValue); break;
    //case FontAttributes: vtObject_a->fontAttributes          = convertLittleEndianStringUi16(  pui8_attributeValue); break; //TODO
    case Options: vtObject_a->options                 = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    //case VariableReference: vtObject_a->variableReference       = convertLittleEndianStringUi16(  pui8_attributeValue); break; //TODO
    case HorizontalJustification: vtObject_a->horizontalJustification = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    default: break;
  }
}

#endif


    void vtObjectOutputString_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	  MACRO_scaleLocalVars
    	  MACRO_scaleSKLocalVars

    	  uint32_t scaledDim = uint32_t( newValue );
    	#ifndef USE_VT_CLIENT_OLD_UNSCALED_SIZE_COMMANDS
    	  MACRO_scaleDimension( scaledDim )
    	#endif

      	if (b_updateObject)
      		vtObject_a->width = newValue;
    	  setAttribute (Width, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputString_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
  	  MACRO_scaleLocalVars
  	  MACRO_scaleSKLocalVars

  	  uint32_t scaledDim = uint32_t( newValue );
  	#ifndef USE_VT_CLIENT_OLD_UNSCALED_SIZE_COMMANDS
  	  MACRO_scaleDimension( scaledDim )
  	#endif

    	if (b_updateObject)
    		vtObject_a->height = newValue;
  	  setAttribute (Height, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectOutputString_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->backgroundColour = newValue;
    	setAttribute (BackgroundColour, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectOutputString_c::setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fontAttributes = newValue;
    	setAttribute (FontAttributes, (newValue == NULL) ? 65535 : newValue->getID(), b_enableReplaceOfCmd);
    }

    void vtObjectOutputString_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->options = newValue;
    	setAttribute (Options, newValue, b_enableReplaceOfCmd);
    }

    void
    vtObjectOutputString_c::setVariableReference(IsoAgLib::iVtObjectStringVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->variableReference = newValue;
    	setAttribute (VariableReference, (newValue == NULL) ? 65535 : newValue->getID(), b_enableReplaceOfCmd);
    }

    void
    vtObjectOutputString_c::setHorizontalJustification(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->horizontalJustification = newValue;
    	setAttribute (HorizontalJustification, newValue, b_enableReplaceOfCmd);
    }





} //__IsoAgLib

#endif
