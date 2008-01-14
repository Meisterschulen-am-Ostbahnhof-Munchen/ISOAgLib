/***************************************************************************
                          vtobjectfontattributes_c.cpp
                             -------------------
    begin                : Don Sep 4 2003
    copyright            : (C) 2003 by Martin Wodok
    email                : m.wodok@osb-ag:de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

#include "../ivtobjectfontattributes_c.h"
#include "vtobjectfontattributes_c.h"
#include "isoterminal_c.h"
#include "../ivtobjectbutton_c.h"

namespace IsoAgLib {
// implement here a normal constructor and functions, as the compiler dislikes inlining of that simple
// cconstructor/functions direct in scope of iVtObjectFontAttributes_c
  iVtObjectFontAttributes_c::iVtObjectFontAttributes_c() : vtObjectFontAttributes_c() {}

  iVtObjectFontAttributes_c::~iVtObjectFontAttributes_c() {}

  uint16_t
      iVtObjectFontAttributes_c::getScaledWidthHeight()
  {
    return vtObjectFontAttributes_c::getScaledWidthHeight();
  }
}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


uint8_t vtObjectFontAttributes_c::marr_font2PixelDimensionTableW [15] = {6,  8,  8, 12, 16, 16, 24, 32, 32, 48, 64, 64, 96,128,128};
uint8_t vtObjectFontAttributes_c::marr_font2PixelDimensionTableH [15] = {8,  8, 12, 16, 16, 24, 32, 32, 48, 64, 64, 96,128,128,192};


// //////////////////////////////// +X2C Operation 168 : stream
//! Parameter:
//! @param destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectFontAttributes_c::stream(uint8_t* destMemory,
                                 uint16_t maxBytes,
                                 objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectFontAttributes_a
#define MACRO_vtObjectTypeS iVtObjectFontAttributes_s
  MACRO_streamLocalVars

  if (sourceOffset == 0) { // dump out constant sized stuff
    destMemory [0] = vtObject_a->ID & 0xFF;
    destMemory [1] = vtObject_a->ID >> 8;
    destMemory [2] = 23; // Object Type = Font Attributes
    destMemory [3] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (vtObjectFontAttributes_a->fontColour, this, IsoAgLib::FontColour);
    destMemory [4] = mui8_fontSizeScaled; // size() must have been called before to prepare!!!!
    destMemory [5] = vtObjectFontAttributes_a->fontType;
    destMemory [6] = vtObjectFontAttributes_a->fontStyle & __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getVtServerInst().getVtCapabilities()->fontTypes;
    destMemory [7] = vtObjectFontAttributes_a->numberOfMacrosToFollow;
    sourceOffset += 8;
    curBytes += 8;
  }

  MACRO_streamEventMacro(8);
  return curBytes;
}

// Operation : vtObjectFontAttributes_c
vtObjectFontAttributes_c::vtObjectFontAttributes_c()
: mui8_fontSizeScaled( 0xFF ) // set mui8_fontSizeScaled to "not yet calculated"
{}


vtObjectFontAttributes_c::~vtObjectFontAttributes_c()
{}


// Operation : size
uint32_t
vtObjectFontAttributes_c::fitTerminal() const
{
  MACRO_localVars;

  mui8_fontSizeScaled = 0xFF;
  // Recalc mui8_fontSizeScaled (with 0xFF it will re-calc the font size)
  calcScaledFontDimension();

  return 8+vtObjectFontAttributes_a->numberOfMacrosToFollow*2;
}

// Operation : getScaledWidthHeight
uint16_t
vtObjectFontAttributes_c::getScaledWidthHeight()
{
  calcScaledFontDimension(); // idempotent! method doesn't calc more than once, so no problem to call...

  if (mui8_fontSizeScaled >= 15)
    return ((0<<6) | (0));
  else
    return ((marr_font2PixelDimensionTableW [mui8_fontSizeScaled] << 8) | (marr_font2PixelDimensionTableH [mui8_fontSizeScaled]));
}

// Operation : calcScaledFontDimension
void
vtObjectFontAttributes_c::calcScaledFontDimension() const
{
  MACRO_localVars;
  MACRO_scaleLocalVars;
  MACRO_scaleSKLocalVars;

  // you can call it idempotent!!
  if (mui8_fontSizeScaled != 0xFF)
    return; // already calculated

  mui8_fontSizeScaled = vtObjectFontAttributes_a->fontSize;
  if (mui8_fontSizeScaled > (15-1)) mui8_fontSizeScaled = (15-1);

  uint32_t width, height;
  uint8_t wIndex=0, hIndex=0;
  if ((s_properties.flags & FLAG_ORIGIN_SKM) || p_parentButtonObject) {
    width = (((uint32_t) factorM * (marr_font2PixelDimensionTableW [mui8_fontSizeScaled]) <<10)/factorD); // (8 bit shifted fixed floating)
    height= (((uint32_t) factorM * (marr_font2PixelDimensionTableH [mui8_fontSizeScaled]) <<10)/factorD); // (8 bit shifted fixed floating)
  } else {
    width = (((uint32_t) vtDimension * (marr_font2PixelDimensionTableW [mui8_fontSizeScaled]) <<10)/opDimension); // (8 bit shifted fixed floating)
    height= (((uint32_t) vtDimension * (marr_font2PixelDimensionTableH [mui8_fontSizeScaled]) <<10)/opDimension); // (8 bit shifted fixed floating)
  }

  /** @todo DISCUSSION maybe keep aspect ratio?? Make it a user-flag on registerIsoObjectPool? Or put it into the objects itself?? */
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
  while (!(__IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getVtServerInst().getVtFontSizes() & (1 << mui8_fontSizeScaled))) {
    mui8_fontSizeScaled--; // try a smaller font, but "6x8" should be there in any way, 'cause we set it in processMsg!!
  }
}

void
vtObjectFontAttributes_c::setFontAttributes(uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_updateObject, bool b_enableReplaceOfCmd) {
  if (b_updateObject) {
    saveValue8 (MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontColour), sizeof(iVtObjectFontAttributes_s), __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newFontColour, this, IsoAgLib::FontColour));
    saveValue8 (MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontSize),   sizeof(iVtObjectFontAttributes_s), newFontSize);
    saveValue8 (MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontType),   sizeof(iVtObjectFontAttributes_s), newFontType);
    saveValue8 (MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontStyle),  sizeof(iVtObjectFontAttributes_s), newFontStyle);
  }
  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeFontAttributes (this, __IsoAgLib::getIsoTerminalInstance().getClientByID (s_properties.clientId).getUserClippedColor (newFontColour, this, IsoAgLib::FontColour), newFontSize, newFontType, newFontStyle, b_enableReplaceOfCmd);
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint8_t
vtObjectFontAttributes_c::updateFontColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontColour), sizeof(iVtObjectFontAttributes_s), 1);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontColour), sizeof(iVtObjectFontAttributes_s));
}

uint8_t
vtObjectFontAttributes_c::updateFontSize(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontSize), sizeof(iVtObjectFontAttributes_s), 2);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontSize), sizeof(iVtObjectFontAttributes_s));
}

uint8_t
vtObjectFontAttributes_c::updateFontType(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontType), sizeof(iVtObjectFontAttributes_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontType), sizeof(iVtObjectFontAttributes_s));
}

uint8_t
vtObjectFontAttributes_c::updateFontStyle(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontStyle), sizeof(iVtObjectFontAttributes_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontStyle), sizeof(iVtObjectFontAttributes_s));
}

void
vtObjectFontAttributes_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue8(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontColour), sizeof(iVtObjectFontAttributes_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 2: saveValue8(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontSize), sizeof(iVtObjectFontAttributes_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontType), sizeof(iVtObjectFontAttributes_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontStyle), sizeof(iVtObjectFontAttributes_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    default: break;
  }
}
#endif
} // end of namespace __IsoAgLib
