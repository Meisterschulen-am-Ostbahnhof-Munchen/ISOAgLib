/*
  vtobjectbutton_c.cpp

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

#include "vtobjectbutton_c.h"

#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"



namespace __IsoAgLib {



struct vtObjectButton_c::iVtObjectButton_s : iVtObjectObject_s, iVtObjectwMacro_s {
  uint16_t width;
  uint16_t height;
  IsoAgLib::Colour backgroundColour;
  IsoAgLib::Colour borderColour;
  uint8_t keyCode;
  uint8_t options;
  explicit iVtObjectButton_s(
		IsoAgLib::ObjectID ID,
  	    uint16_t width,
  	    uint16_t height,
		IsoAgLib::Colour backgroundColour,
		IsoAgLib::Colour borderColour,
  	    uint8_t keyCode,
  	    uint8_t options)
  : iVtObject_s(ID)
  , iVtObjectObject_s(ID)
  , iVtObjectwMacro_s(ID)
  , width(width)
  , height(height)
  , backgroundColour(backgroundColour)
  , borderColour(borderColour)
  , keyCode(keyCode)
  , options(options)
  {}
};



int16_t
vtObjectButton_c::stream(uint8_t* destMemory,
                         uint16_t maxBytes,
                         objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectButton_a
#define MACRO_vtObjectTypeS iVtObjectButton_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObjectButton_a->width;
    uint32_t height = (uint32_t)vtObjectButton_a->height;
    MACRO_scaleSizeI32(width, height);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 6; // Object Type = Button
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObjectButton_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [8] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObjectButton_a->borderColour, this, IsoAgLib::BorderColour);
      destMemory [9] = vtObjectButton_a->keyCode;
      destMemory [10] = vtObjectButton_a->options;
      destMemory [11] = vtObjectButton_a->numberOfObjectsToFollow;
      destMemory [12] = vtObjectButton_a->numberOfMacrosToFollow;
      sourceOffset += 13;
      curBytes += 13;
    }

    MACRO_streamObjectXYcenteredInButton(13);
    MACRO_streamEventMacro(13U+vtObjectButton_a->numberOfObjectsToFollow*6U);
    return curBytes;
}


IsoAgLib::ObjectID vtObjectButton_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


vtObjectButton_c::vtObjectButton_c(
  int ai_multitonInst,
	IsoAgLib::ObjectID ID,
	uint16_t width,
	uint16_t height,
	IsoAgLib::Colour backgroundColour,
	IsoAgLib::Colour borderColour,
	uint8_t keyCode,
	uint8_t options)
:vtObjectButton_c(
		new iVtObjectButton_s(
			ID,
			width,
			height,
			backgroundColour,
			borderColour,
			keyCode,
			options),
		ai_multitonInst)
{}


vtObjectButton_c::vtObjectButton_c(vtObjectButton_c::iVtObjectButton_s *vtObjectButtonSROM,
                                   int ai_multitonInst)
        : vtObject_c(ai_multitonInst)
		, vtObject_a(vtObjectButtonSROM)
{}



vtObjectButton_c::iVtObjectButton_s *vtObjectButton_c::get_vtObjectButton_a() {
	return vtObject_a;
}



uint32_t
vtObjectButton_c::fitTerminal() const
{
  MACRO_localVars;
  return 13+vtObjectButton_a->numberOfObjectsToFollow*6+vtObjectButton_a->numberOfMacrosToFollow*2;
}

void
vtObjectButton_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectButton_a(), width),  sizeof(iVtObjectButton_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectButton_a(), height), sizeof(iVtObjectButton_s), newHeight);
  }

  scaleSize( newWidth, newHeight );

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize(
    this, newWidth, newHeight, b_enableReplaceOfCmd );
}

bool
vtObjectButton_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObjectButton_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObjectButton_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectButton_a(), objectsToFollow), sizeof(iVtObjectButton_s), b_enableReplaceOfCmd);
}

bool
vtObjectButton_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObjectButton_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObjectButton_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectButton_a(), objectsToFollow), sizeof(iVtObjectButton_s), b_enableReplaceOfCmd, NoOffset);
}

void
vtObjectButton_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* /*p_btn*/)
{
  MACRO_localVars;
  p_parentButtonObject = (IsoAgLib::iVtObjectButton_c*)this;
  for (int i=0; i<vtObjectButton_a->numberOfObjectsToFollow; i++) {
    vtObjectButton_a->objectsToFollow[i].vtObject->setOriginBTN (p_parentButtonObject);
  }
}


    void vtObjectButton_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), width) : 0, sizeof(iVtObjectButton_s), 1, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectButton_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), height) : 0, sizeof(iVtObjectButton_s), 2, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectButton_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), backgroundColour) : 0, sizeof(iVtObjectButton_s), 3, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectButton_c::setBorderColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), borderColour) : 0, sizeof(iVtObjectButton_s), 4, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BorderColour), b_enableReplaceOfCmd);
    }

    void vtObjectButton_c::setKeyCode(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), keyCode) : 0, sizeof(iVtObjectButton_s), 5, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectButton_c::v4setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), options) : 0, sizeof(iVtObjectButton_s), 6, newValue, newValue, b_enableReplaceOfCmd);
    }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectButton_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), width), sizeof(iVtObjectButton_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectButton_a(), width), sizeof(iVtObjectButton_s));
}

uint16_t
vtObjectButton_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), height), sizeof(iVtObjectButton_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectButton_a(), height), sizeof(iVtObjectButton_s));
}

uint8_t
vtObjectButton_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), backgroundColour), sizeof(iVtObjectButton_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectButton_a(), backgroundColour), sizeof(iVtObjectButton_s));
}

uint8_t
vtObjectButton_c::updateBorderColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), borderColour), sizeof(iVtObjectButton_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectButton_a(), borderColour), sizeof(iVtObjectButton_s));
}

uint8_t
vtObjectButton_c::updateKeyCode(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), keyCode), sizeof(iVtObjectButton_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectButton_a(), keyCode), sizeof(iVtObjectButton_s));
}

uint8_t
vtObjectButton_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), options), sizeof(iVtObjectButton_s), 6);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectButton_a(), options), sizeof(iVtObjectButton_s));
}

void
vtObjectButton_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectButton_a(), width), sizeof(iVtObjectButton_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectButton_a(), height), sizeof(iVtObjectButton_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectButton_a(), backgroundColour), sizeof(iVtObjectButton_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectButton_a(), borderColour), sizeof(iVtObjectButton_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectButton_a(), keyCode), sizeof(iVtObjectButton_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue8(MACRO_getStructOffset(get_vtObjectButton_a(), options), sizeof(iVtObjectButton_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    default: break;
  }
}
#endif

void vtObjectButton_c::Append(iVtObject_c * const vtObject, int16_t x, int16_t y) {
	get_vtObjectButton_a()->Append(vtObject, x, y);
}

    uint16_t vtObjectButton_c::getHeight() const {
        return vtObject_a->height;
    }

    uint16_t vtObjectButton_c::getWidth() const {
        return vtObject_a->width;
    }

} // __IsoAgLib
