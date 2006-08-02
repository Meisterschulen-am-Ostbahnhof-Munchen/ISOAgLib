/***************************************************************************
                          vtobjectpicturegraphic_c.cpp
                             -------------------
    begin                : Mon Jun 30 14:41:54 2003
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

#include "vtobjectpicturegraphic_c.h"
#include "../ivtobjectbutton_c.h"
//#include "../ivtobjectmacro_c.h"
#include "isoterminal_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {



#if 0
#define helperForDifferentSizes(a,b,c,f,g,h,x,y,z,optionander) \
        if (vtDimension < 400) { \
          numberOfBytesInRawData = vtObjectPictureGraphic_a->a; \
          rawData = vtObjectPictureGraphic_a->x; \
          options = (vtObjectPictureGraphic_a->f & 0x03) + ((vtObjectPictureGraphic_a->f & optionander) ? 0x04 : 0x00); \
        } else if (vtDimension < 480) { \
          numberOfBytesInRawData = vtObjectPictureGraphic_a->b; \
          rawData = vtObjectPictureGraphic_a->y; \
          options = (vtObjectPictureGraphic_a->g & 0x03) + ((vtObjectPictureGraphic_a->g & optionander) ? 0x04 : 0x00); \
        } else { \
          numberOfBytesInRawData = vtObjectPictureGraphic_a->c; \
          rawData = vtObjectPictureGraphic_a->z; \
          options = (vtObjectPictureGraphic_a->h & 0x03) + ((vtObjectPictureGraphic_a->h & optionander) ? 0x04 : 0x00); \
        }

#define helperForDifferentSizesSizeOnly(a,b,c) \
        if (vtDimension < 400) { \
          numberOfBytesInRawData = vtObjectPictureGraphic_a->a; \
        } else if (vtDimension < 480) { \
          numberOfBytesInRawData = vtObjectPictureGraphic_a->b; \
        } else { \
          numberOfBytesInRawData = vtObjectPictureGraphic_a->c; \
        }
//#else
#define helperForDifferentSizes(a,b,f,g,x,y,optionander) \
        if ( ( vtDimension < 480) || ( optionander == 0x04 ) ) { \
          numberOfBytesInRawData = vtObjectPictureGraphic_a->a; \
          rawData = vtObjectPictureGraphic_a->x; \
          options = (vtObjectPictureGraphic_a->f & 0x03) + ((vtObjectPictureGraphic_a->f & optionander) ? 0x04 : 0x00); \
        } else { \
          numberOfBytesInRawData = vtObjectPictureGraphic_a->b; \
          rawData = vtObjectPictureGraphic_a->y; \
          options = (vtObjectPictureGraphic_a->g & 0x03) + ((vtObjectPictureGraphic_a->g & optionander) ? 0x04 : 0x00); \
        }

#define helperForDifferentSizesSizeOnly(a,b,col) \
        if ( ( vtDimension < 480) || ( col == 0x0 ) ) { \
          numberOfBytesInRawData = vtObjectPictureGraphic_a->a; \
        } else { \
          numberOfBytesInRawData = vtObjectPictureGraphic_a->b; \
        }
#endif

#define min(a,b) (a<b)?a:b

#define MACRO_helperForDifferentSizesSizeOnly(a) \
          numberOfBytesInRawData = vtObjectPictureGraphic_a->a;

#define MACRO_helperForDifferentSizes(a,f,x,optionander) \
          MACRO_helperForDifferentSizesSizeOnly(a) \
          rawData = vtObjectPictureGraphic_a->x; \
          options = (vtObjectPictureGraphic_a->f & 0x03) + ((vtObjectPictureGraphic_a->f & optionander) ? 0x04 : 0x00); /* get the right RLE 1/4/8 bit to bit 2 when streaming! */

#define MACRO_calculate_ui8_graphicType \
          ui8_graphicType = ( min (__IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getVtServerInst().getVtCapabilities()->hwGraphicType, vtObjectPictureGraphic_a->format) ); \
          /* If 16-color bitmap is not specified, take the 2-color version. -That's the only exception! */ \
          if ((ui8_graphicType == 1) && (vtObjectPictureGraphic_a->rawData1 == NULL)) ui8_graphicType = 0;

#define MACRO_CheckFixedBitmapsLoop_start \
    /* See if we have colorDepth of VT */ \
    uint8_t vtDepth = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getVtServerInst().getVtCapabilities()->hwGraphicType; \
    /* Check for 100%-matching fixedBitmaps first */ \
    bool b_foundFixedBitmap = false; \
    for (int fixNr=0; fixNr<vtObjectPictureGraphic_a->numberOfFixedBitmapsToFollow; fixNr++) { \
      /* Matching means only match in WIDTH, not (yet) HEIGHT - and in Colourdepth! */ \
      if ( (width == vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].actualWidth) \
      && (vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].formatoptions >> 6 == vtDepth) ) {
// here user can insert code into the loop.
#define MACRO_CheckFixedBitmapsLoop_end \
        b_foundFixedBitmap = true; \
        break; \
      } \
    }

#if defined (NO_BITMAP_SCALING)
  #define MACRO_calculateRequestedSize \
    uint16_t width = vtObjectPictureGraphic_a->width;
#else
  #define MACRO_calculateRequestedSize \
    uint16_t width; \
    if ((s_properties.flags & FLAG_ORIGIN_SKM) || p_parentButtonObject) { \
      width = (((uint32_t) vtObjectPictureGraphic_a->width * factorM)/factorD); \
    } else { \
      width = (((uint32_t) vtObjectPictureGraphic_a->width * vtDimension) /opDimension); \
    }
#endif


// //////////////////////////////// +X2C Operation 150 : stream
//! Parameter:
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectPictureGraphic_c::stream(uint8_t* destMemory, uint16_t maxBytes, objRange_t sourceOffset)
{ // ~X2C
#define MACRO_vtObjectTypeA vtObjectPictureGraphic_a
#define MACRO_vtObjectTypeS iVtObjectPictureGraphic_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint8_t ui8_graphicType;

    const HUGE_MEM uint8_t* rawData;
    uint32_t numberOfBytesInRawData;
    uint16_t actualWidth;
    uint16_t actualHeight;
    uint8_t options;

    MACRO_calculateRequestedSize

    MACRO_CheckFixedBitmapsLoop_start
        rawData = vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].rawData;
        numberOfBytesInRawData = vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].numberOfBytesInRawData;
        actualWidth = vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].actualWidth;
        actualHeight = vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].actualHeight;
        options = vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].formatoptions & 0x7; /* format is bit 8+7, options is bit 2-0 (NO RLE1/4/8 stuff here!! */
        ui8_graphicType = vtDepth;
    MACRO_CheckFixedBitmapsLoop_end

    if (!b_foundFixedBitmap) {
      // See what we have as standard...
      actualWidth = vtObjectPictureGraphic_a->actualWidth;
      actualHeight = vtObjectPictureGraphic_a->actualHeight;
      MACRO_calculate_ui8_graphicType
      switch (ui8_graphicType) {
        case 2:  MACRO_helperForDifferentSizes (numberOfBytesInRawData2, options, rawData2, 0x10) break;
        case 1:  MACRO_helperForDifferentSizes (numberOfBytesInRawData1, options, rawData1, 0x08) break;
        case 0:
        default: MACRO_helperForDifferentSizes (numberOfBytesInRawData0, options, rawData0, 0x04) break;
      }
    }

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 20; // Object Type = Picture Graphic
      destMemory [3] = width & 0xFF; // calculated above
      destMemory [4] = width >> 8;   // dependent on FLAG_ORIGIN_SKM
      destMemory [5] = actualWidth & 0xFF;
      destMemory [6] = actualWidth >> 8;
      destMemory [7] = actualHeight & 0xFF;
      destMemory [8] = actualHeight >> 8;
      destMemory [9] = ui8_graphicType;
      destMemory [10] = options;
      destMemory [11] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (vtObjectPictureGraphic_a->transparencyColour, this, IsoAgLib::TransparencyColour);
      destMemory [12] = (numberOfBytesInRawData) & 0xFF;
      destMemory [13] = (numberOfBytesInRawData >> 8) & 0xFF;
      destMemory [14] = (numberOfBytesInRawData >> 16) & 0xFF;
      destMemory [15] = (numberOfBytesInRawData >> 24) & 0xFF;
      destMemory [16] = vtObjectPictureGraphic_a->numberOfMacrosToFollow;

      sourceOffset += 17;
      curBytes += 17;
    }

    while ((sourceOffset >= 17) && (sourceOffset < (17+numberOfBytesInRawData)) && ((curBytes+1) <= maxBytes)) {
      destMemory [curBytes] = rawData [sourceOffset-17];
      curBytes++;
      sourceOffset++;
    }

    MACRO_streamEventMacro(17+numberOfBytesInRawData);

    return curBytes;
} // -X2C

// //////////////////////////////// +X2C Operation 153 : vtObjectPictureGraphic_c
vtObjectPictureGraphic_c::vtObjectPictureGraphic_c()
{ // ~X2C
} // -X2C

// //////////////////////////////// +X2C Operation 202 : size
uint32_t
vtObjectPictureGraphic_c::fitTerminal()
{ // ~X2C
  MACRO_localVars;
  MACRO_scaleLocalVars;
  MACRO_scaleSKLocalVars;

  uint32_t numberOfBytesInRawData;
  uint8_t ui8_graphicType;

  MACRO_calculateRequestedSize

  MACRO_CheckFixedBitmapsLoop_start
      numberOfBytesInRawData = vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].numberOfBytesInRawData;
  MACRO_CheckFixedBitmapsLoop_end

  if (!b_foundFixedBitmap) {
    MACRO_calculate_ui8_graphicType
    switch (ui8_graphicType) {
      case 2:  MACRO_helperForDifferentSizesSizeOnly (numberOfBytesInRawData2) break;
      case 1:  MACRO_helperForDifferentSizesSizeOnly (numberOfBytesInRawData1) break;
      case 0:
      default: MACRO_helperForDifferentSizesSizeOnly (numberOfBytesInRawData0) break;
    }
  }

  return 17+numberOfBytesInRawData+vtObjectPictureGraphic_a->numberOfMacrosToFollow*2;
} // -X2C

} // end of namespace __IsoAgLib
