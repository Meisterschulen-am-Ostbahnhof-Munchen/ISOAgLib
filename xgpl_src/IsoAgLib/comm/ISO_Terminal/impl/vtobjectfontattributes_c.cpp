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

#include "vtobjectfontattributes_c.h"
#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {



// //////////////////////////////// +X2C Operation 168 : stream
//! Parameter:
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectFontAttributes_c::stream(uint8_t* destMemory,
                                 uint16_t maxBytes,
                                 uint16_t sourceOffset)
{ // ~X2C
#define MACRO_vtObjectTypeA vtObjectFontAttributes_a
#define MACRO_vtObjectTypeS iVtObjectFontAttributes_s
  MACRO_streamLocalVars;

  if (sourceOffset == 0) { // dump out constant sized stuff
    destMemory [0] = vtObject_a->ID & 0xFF;
    destMemory [1] = vtObject_a->ID >> 8;
    destMemory [2] = 23; // Object Type = Font Attributes
    destMemory [3] = __IsoAgLib::getIsoTerminalInstance().getUserClippedColor (vtObjectFontAttributes_a->fontColour);
    destMemory [4] = ui8_fontSizeScaled; // size() must have been called before to prepare!!!!
    destMemory [5] = vtObjectFontAttributes_a->fontType; // fix ISO_LATIN_1
    destMemory [6] = vtObjectFontAttributes_a->fontStyle;
    destMemory [7] = vtObjectFontAttributes_a->numberOfMacrosToFollow;
    sourceOffset += 8;
    curBytes += 8;
  }

  MACRO_streamEventMacro(8);
  return curBytes;
} // -X2C

// //////////////////////////////// +X2C Operation 171 : vtObjectFontAttributes_c
vtObjectFontAttributes_c::vtObjectFontAttributes_c()
: ui8_fontSizeScaled( 0xFF ) // set ui8_fontSizeScaled to "not yet calculated"
{ // ~X2C
} // -X2C

// //////////////////////////////// +X2C Operation 205 : size
uint32_t
vtObjectFontAttributes_c::fitTerminal()
{ // ~X2C
  MACRO_localVars;
  
  ui8_fontSizeScaled = 0xFF;
  calcScaledFontDimension(); // with 0xFF it will re-calc the font size!
  
  return 8+vtObjectFontAttributes_a->numberOfMacrosToFollow*2;
} // -X2C

// //////////////////////////////// +X2C Operation 281 : getScaledWidthHeight
uint16_t
vtObjectFontAttributes_c::getScaledWidthHeight()
{ // ~X2C
  static uint16_t font2PixelDimensionTable [15] = {
    (  6<<8) | (  8),
    (  8<<8) | (  8),
    (  8<<8) | ( 12),
    ( 12<<8) | ( 16),
    ( 16<<8) | ( 16),
    ( 16<<8) | ( 24),
    ( 24<<8) | ( 32),
    ( 32<<8) | ( 32),
    ( 32<<8) | ( 48),
    ( 48<<8) | ( 64),
    ( 64<<8) | ( 64),
    ( 64<<8) | ( 96),
    ( 96<<8) | (128),
    (128<<8) | (128),
    (128<<8) | (192)
  };

  calcScaledFontDimension(); // idempotent! method doesn't calc more than once, so no problem to call...

  if (ui8_fontSizeScaled >= 15)
    return ((0<<6) | (0));
  else
    return (font2PixelDimensionTable [ui8_fontSizeScaled]);
} // -X2C


// //////////////////////////////// +X2C Operation 282 : calcScaledFontDimension
void
vtObjectFontAttributes_c::calcScaledFontDimension()
{ // ~X2C
  MACRO_localVars;
  MACRO_scaleLocalVars;
  MACRO_scaleSKLocalVars;

  // you can call it idempotent!!
  if (ui8_fontSizeScaled != 0xFF)
    return; // already calculated

  ui8_fontSizeScaled = vtObjectFontAttributes_a->fontSize;
  uint32_t scale;
  if (flags & FLAG_ORIGIN_SKM) {
    scale = factor; 
  } else {
    scale = (((uint32_t) vtDimension * 0x100000)/opDimension); // (20 bit shifted fixed floating)
  }
  while ((scale >= 0x200000) && (ui8_fontSizeScaled <= 11)) { // 2.00000
    ui8_fontSizeScaled += 3; // double font size
    scale >>= 1; // doubled, now see if we still have x2 or x1.33
  }
  while ((scale >= 0x180000) && (ui8_fontSizeScaled <= 11)) { // 1.50000
    switch (ui8_fontSizeScaled % 3) {
      case 0:
        ui8_fontSizeScaled += 2;
        break;
      case 1:
        ui8_fontSizeScaled += 1;
        scale >>= 1;
        break;
      case 2:
        ui8_fontSizeScaled += 1;
        break;
    }
    scale *= 2; scale /= 3;
  }
  while ((scale >= 0x155555) && ((ui8_fontSizeScaled % 3) == 0)) {
    ui8_fontSizeScaled += 1;
    scale *= 3; scale /= 4;
  }
  // now check if the font is available?
  while (!(__IsoAgLib::getIsoTerminalInstance().getVtCapabilities()->fontSizes & (1 << ui8_fontSizeScaled))) {
    ui8_fontSizeScaled--; // try a smaller font, but "6x8" should be there in any way, 'cause we set it in processMsg!!
  }
} // -X2C

void
vtObjectFontAttributes_c::setFontAttributes(uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_updateObject) {
  if (b_updateObject) {
    saveValue8 (MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontColour), sizeof(iVtObjectFontAttributes_s), newFontColour);
    saveValue8 (MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontSize),   sizeof(iVtObjectFontAttributes_s), newFontSize);
    saveValue8 (MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontType),   sizeof(iVtObjectFontAttributes_s), newFontType);
    saveValue8 (MACRO_getStructOffset(get_vtObjectFontAttributes_a(), fontStyle),  sizeof(iVtObjectFontAttributes_s), newFontStyle);
  }
  __IsoAgLib::getIsoTerminalInstance().sendCommandChangeFontAttributes (this, newFontColour, newFontSize, newFontType, newFontStyle);
}


} // end of namespace __IsoAgLib
