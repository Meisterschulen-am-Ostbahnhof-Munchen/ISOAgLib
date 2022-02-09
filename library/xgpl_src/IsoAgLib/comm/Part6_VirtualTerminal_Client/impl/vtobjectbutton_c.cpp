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

#ifdef CONFIG_USE_VTOBJECT_button
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
    uint16_t curBytes=0; /* current bytes written */;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObject_a->width;
    uint32_t height = (uint32_t)vtObject_a->height;
    MACRO_scaleSizeI32(width, height);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 6; // Object Type = Button
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [8] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->borderColour, this, IsoAgLib::BorderColour);
      destMemory [9] = vtObject_a->keyCode;
      destMemory [10] = vtObject_a->options;
      destMemory [11] = vtObject_a->numberOfObjectsToFollow;
      destMemory [12] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 13;
      curBytes += 13;
    }

    MACRO_streamObjectXYcenteredInButton(13);
    MACRO_streamEventMacro(13U+vtObject_a->numberOfObjectsToFollow*6U);
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
  return 13+vtObject_a->numberOfObjectsToFollow*6+vtObject_a->numberOfMacrosToFollow*2;
}

void
vtObjectButton_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    vtObject_a->width = newWidth;
    vtObject_a->height = newHeight;
  }

  scaleSize( newWidth, newHeight );

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize(
    this, newWidth, newHeight, b_enableReplaceOfCmd );
}

bool
vtObjectButton_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObject_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObject_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectButton_a(), objectsToFollow), sizeof(iVtObjectButton_s), b_enableReplaceOfCmd);
}

bool
vtObjectButton_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObject_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObject_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectButton_a(), objectsToFollow), sizeof(iVtObjectButton_s), b_enableReplaceOfCmd, NoOffset);
}


void
vtObjectButton_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* /*p_btn*/)
{
  p_parentButtonObject = (IsoAgLib::iVtObjectButton_c*)this;
  for (int i=0; i<vtObject_a->numberOfObjectsToFollow; i++) {
    vtObject_a->objectsToFollow[i].vtObject->setOriginBTN (p_parentButtonObject);
  }
}



    void vtObjectButton_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        MACRO_scaleLocalVars
        MACRO_scaleSKLocalVars

        uint32_t scaledDim = uint32_t( newValue );
      #ifndef USE_VT_CLIENT_OLD_UNSCALED_SIZE_COMMANDS
        MACRO_scaleDimension( scaledDim )
      #endif

    	if (b_updateObject)
    		vtObject_a->width = newValue;
        setAttribute (1, scaledDim, b_enableReplaceOfCmd);
    }

    void vtObjectButton_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        MACRO_scaleLocalVars
        MACRO_scaleSKLocalVars

        uint32_t scaledDim = uint32_t( newValue );
      #ifndef USE_VT_CLIENT_OLD_UNSCALED_SIZE_COMMANDS
        MACRO_scaleDimension( scaledDim )
      #endif

    	if (b_updateObject)
    		vtObject_a->height = newValue;
        setAttribute (2, scaledDim, b_enableReplaceOfCmd);
    }

    void vtObjectButton_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->backgroundColour = newValue;
	setAttribute(3, __IsoAgLib::getVtClientInstance4Comm().getClientByID(s_properties.clientId).getUserConvertedColor(newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectButton_c::setBorderColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->borderColour = newValue;
    	setAttribute(4, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BorderColour), b_enableReplaceOfCmd);
    }

    void vtObjectButton_c::setKeyCode(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->keyCode = newValue;
    	setAttribute(5, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectButton_c::v4setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->options = newValue;
    	setAttribute(6, newValue, b_enableReplaceOfCmd);
    }

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectButton_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(1);
  return vtObject_a->width;
}

uint16_t
vtObjectButton_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
	  getAttribute (2);
  return vtObject_a->height;

}

uint8_t
vtObjectButton_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(3);
  return vtObject_a->backgroundColour;
}

uint8_t
vtObjectButton_c::updateBorderColour(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(4);
  return vtObject_a->borderColour;
}

uint8_t
vtObjectButton_c::updateKeyCode(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(5);
  return vtObject_a->keyCode;
}

uint8_t
vtObjectButton_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(6);
  return vtObject_a->options;
}

void
vtObjectButton_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: vtObject_a->width            = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case 2: vtObject_a->height           = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case 3: vtObject_a->backgroundColour = convertLittleEndianStringColour(pui8_attributeValue); break;
    case 4: vtObject_a->borderColour     = convertLittleEndianStringColour(pui8_attributeValue); break;
    case 5: vtObject_a->keyCode          = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case 6: vtObject_a->options          = convertLittleEndianStringUi8(   pui8_attributeValue); break;
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

#endif //CONFIG_USE_VTOBJECT_button
