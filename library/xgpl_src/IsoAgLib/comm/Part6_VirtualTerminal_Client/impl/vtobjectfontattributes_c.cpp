/*
  vtobjectfontattributes_c.cpp

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

#include "../ivtobjectfontattributes_c.h"
#include "vtobjectfontattributes_c.h"
#include "vtclient_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"




namespace __IsoAgLib {


struct vtObjectFontAttributes_c::iVtObjectFontAttributes_s : iVtObjectwMacro_s {
  IsoAgLib::Colour fontColour;
  IsoAgLib::Font fontSize;
  uint8_t fontType; // always =0 ISO_LATIN_1
  uint8_t fontStyle;
  explicit iVtObjectFontAttributes_s(
		IsoAgLib::ObjectID ID,
		IsoAgLib::Colour fontColour,
		IsoAgLib::Font fontSize,
  	    uint8_t fontType, // always =0 ISO_LATIN_1
  	    uint8_t fontStyle)
  : iVtObject_s(ID)
  , iVtObjectwMacro_s(ID)
  , fontColour(fontColour)
  , fontSize(fontSize)
  , fontType(fontType) // always =0 ISO_LATIN_1
  , fontStyle(fontStyle)
  {}
};


uint8_t vtObjectFontAttributes_c::marr_font2PixelDimensionTableW [15] = {6,  8,  8, 12, 16, 16, 24, 32, 32, 48, 64, 64, 96,128,128};
uint8_t vtObjectFontAttributes_c::marr_font2PixelDimensionTableH [15] = {8,  8, 12, 16, 16, 24, 32, 32, 48, 64, 64, 96,128,128,192};



int16_t
vtObjectFontAttributes_c::stream(uint8_t* destMemory,
                                 uint16_t maxBytes,
                                 objRange_t sourceOffset)
{
  uint16_t curBytes=0; /* current bytes written */

  if (sourceOffset == 0) { // dump out constant sized stuff
    destMemory [0] = vtObject_a->ID & 0xFF;
    destMemory [1] = vtObject_a->ID >> 8;
    destMemory [2] = 23; // Object Type = Font Attributes
    destMemory [3] = getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->fontColour, this, IsoAgLib::FontColour);
    destMemory [4] = mui8_fontSizeScaled; // size() must have been called before to prepare!!!!
    destMemory [5] = vtObject_a->fontType;
    destMemory [6] = vtObject_a->fontStyle & getVtClientInstance4Comm().getClientByID (s_properties.clientId).getVtServerInst().getVtCapabilities().fontTypes;
    destMemory [7] = vtObject_a->numberOfMacrosToFollow;
    sourceOffset += 8;
    curBytes += 8;
  }

  MACRO_streamEventMacro(8);
  return curBytes;
}

IsoAgLib::ObjectID vtObjectFontAttributes_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


vtObjectFontAttributes_c::vtObjectFontAttributes_c(
		multiton ai_multitonInst,
		IsoAgLib::ObjectID ID,
		IsoAgLib::Colour fontColour,
		IsoAgLib::Font fontSize,
		uint8_t fontType, // always =0 ISO_LATIN_1
		uint8_t fontStyle)
: vtObjectFontAttributes_c(
		new iVtObjectFontAttributes_s(
				ID,
				fontColour,
				fontSize,
				fontType, // always =0 ISO_LATIN_1
				fontStyle),
		ai_multitonInst)
{
}

vtObjectFontAttributes_c::~vtObjectFontAttributes_c() = default;


vtObjectFontAttributes_c::vtObjectFontAttributes_c(iVtObjectFontAttributes_s* vtObject_c , multiton ai_multitonInst)
: __IsoAgLib::vtObject_c(ai_multitonInst)
, vtObject_a(vtObject_c)
, mui8_fontSizeScaled( 0xFF ) // set mui8_fontSizeScaled to "not yet calculated"
{
}






uint32_t
vtObjectFontAttributes_c::fitTerminal() const
{
  mui8_fontSizeScaled = 0xFF;
  // Recalc mui8_fontSizeScaled (with 0xFF it will re-calc the font size)
  calcScaledFontDimension();

  return 8+vtObject_a->numberOfMacrosToFollow*2;
}


uint16_t
vtObjectFontAttributes_c::getScaledWidthHeight()
{
  calcScaledFontDimension(); // idempotent! method doesn't calc more than once, so no problem to call...

  if (mui8_fontSizeScaled >= 15)
    return ((0<<6) | (0));
  else
    return ((marr_font2PixelDimensionTableW [mui8_fontSizeScaled] << 8) | (marr_font2PixelDimensionTableH [mui8_fontSizeScaled]));
}


void
vtObjectFontAttributes_c::calcScaledFontDimension() const
{
  MACRO_scaleLocalVars;
  MACRO_scaleSKLocalVars;

  // you can call it idempotent!!
  if (mui8_fontSizeScaled != 0xFF)
    return; // already calculated

  mui8_fontSizeScaled = vtObject_a->fontSize;
  if (mui8_fontSizeScaled > (15-1)) mui8_fontSizeScaled = (15-1);

  uint32_t width, height;
  uint8_t wIndex=0, hIndex=0;
  if (
		  (s_properties.flags & FLAG_ORIGIN_SKM)
#ifdef CONFIG_USE_VTOBJECT_button
		  or
		  p_parentButtonObject
#endif //CONFIG_USE_VTOBJECT_button
	 )
  {
    width = (((uint32_t) factorM * (marr_font2PixelDimensionTableW [mui8_fontSizeScaled]) <<10)/factorD); // (8 bit shifted fixed floating)
    height= (((uint32_t) factorM * (marr_font2PixelDimensionTableH [mui8_fontSizeScaled]) <<10)/factorD); // (8 bit shifted fixed floating)
  } else {
    width = (((uint32_t) vtDimension * (marr_font2PixelDimensionTableW [mui8_fontSizeScaled]) <<10)/opDimension); // (8 bit shifted fixed floating)
    height= (((uint32_t) vtDimension * (marr_font2PixelDimensionTableH [mui8_fontSizeScaled]) <<10)/opDimension); // (8 bit shifted fixed floating)
  }

  /** @todo SOON-174 maybe keep aspect ratio?? Make it a user-flag on registerIsoObjectPool? Or put it into the objects itself?? */
  // now get the lower possible size...
  int i, j;
  for (i=14; i>=0; i--) {
    if (((uint32_t (marr_font2PixelDimensionTableW [i])) << 10) <= width) {
      wIndex = i;
      break;
    }
  }
  for (j=14; j>=0; j--) {
    if (((uint32_t (marr_font2PixelDimensionTableH [j])) << 10) <= height) {
      hIndex = j;
      break;
    }
  }
  if ((i < 0) || (j < 0))
  { // too small font, smaller than 6x8... ==> take 6x8
    mui8_fontSizeScaled = 0;
  }
  else
  { // match indices together... take the lowest one, that'll do!
    if (wIndex < hIndex)
      mui8_fontSizeScaled = wIndex;
    else
      mui8_fontSizeScaled = hIndex;
  }

  /// Always check if the font is available!
  while (!(getVtClientInstance4Comm().getClientByID (s_properties.clientId).getVtServerInst().getVtFontSizes() & (1 << mui8_fontSizeScaled))) {
    mui8_fontSizeScaled--; // try a smaller font, but "6x8" should be there in any way, 'cause we set it in processMsg!!
  }
}

void
vtObjectFontAttributes_c::setFontAttributes(IsoAgLib::Colour newFontColour, IsoAgLib::Font newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    vtObject_a->fontColour = getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newFontColour, this, IsoAgLib::FontColour);
    vtObject_a->fontSize  =newFontSize;
    vtObject_a->fontType  =newFontType;
    vtObject_a->fontStyle =newFontStyle;
  }
  VtClientConnection_c& vtCSC = getVtClientInstance4Comm().getClientByID (s_properties.clientId);
  vtCSC.commandHandler().sendCommandChangeFontAttributes(
    this, vtCSC.getUserConvertedColor (newFontColour, this, IsoAgLib::FontColour),
    newFontSize, newFontType, newFontStyle, b_enableReplaceOfCmd );
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint8_t
vtObjectFontAttributes_c::updateFontColour(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(1);
	return vtObject_a->fontColour;
}

uint8_t
vtObjectFontAttributes_c::updateFontSize(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(2);
	return vtObject_a->fontSize;
}

uint8_t
vtObjectFontAttributes_c::updateFontType(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(3);
	return vtObject_a->fontType;
}

uint8_t
vtObjectFontAttributes_c::updateFontStyle(bool b_SendRequest)
{
	if (b_SendRequest)
		getAttribute(4);
	return vtObject_a->fontStyle;
}

void
vtObjectFontAttributes_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: vtObject_a->fontColour = convertLittleEndianStringColour(pui8_attributeValue); break;
    case 2: vtObject_a->fontSize   = convertLittleEndianStringFont(  pui8_attributeValue); break;
    case 3: vtObject_a->fontType   = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case 4: vtObject_a->fontStyle  = convertLittleEndianStringUi8(   pui8_attributeValue); break;
    default: break;
  }
}
#endif //CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES




    void vtObjectFontAttributes_c::setFontColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fontColour = newValue;
    	setAttribute (1, getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::FontColour), b_enableReplaceOfCmd);
    }

    void vtObjectFontAttributes_c::setFontSize(IsoAgLib::Font newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fontSize = newValue;
    	setAttribute (2, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectFontAttributes_c::setFontType(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fontType = newValue;
    	setAttribute (3, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectFontAttributes_c::setFontStyle(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fontStyle = newValue;
    	setAttribute (4, newValue, b_enableReplaceOfCmd);
    }





} // __IsoAgLib

