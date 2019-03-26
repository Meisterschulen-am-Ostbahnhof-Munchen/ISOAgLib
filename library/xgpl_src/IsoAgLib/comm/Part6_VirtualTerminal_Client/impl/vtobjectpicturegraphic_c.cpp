/*
  vtobjectpicturegraphic_c.cpp

  (C) Copyright 2009 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "vtobjectpicturegraphic_c.h"

#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


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
          ui8_graphicType = ( min (__IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getVtServerInst().getVtCapabilities().hwGraphicType, vtObjectPictureGraphic_a->format) ); \
          /* If 16-color bitmap is not specified, take the 2-color version. -That's the only exception! */ \
          if ((ui8_graphicType == 1) && (vtObjectPictureGraphic_a->rawData1 == NULL)) ui8_graphicType = 0;

#define MACRO_CheckFixedBitmapsLoop_start \
    /* See if we have colorDepth of VT */ \
    uint8_t vtDepth = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getVtServerInst().getVtCapabilities().hwGraphicType; \
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

#define MACRO_calculateRequestedSize \
  uint16_t width; \
  if ((s_properties.flags & FLAG_ORIGIN_SKM) || p_parentButtonObject) { \
    width = (((uint32_t) vtObjectPictureGraphic_a->width * factorM)/factorD); \
  } else { \
    width = (((uint32_t) vtObjectPictureGraphic_a->width * vtDimension) /opDimension); \
  }


int16_t
vtObjectPictureGraphic_c::stream(uint8_t* destMemory, uint16_t maxBytes, objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectPictureGraphic_a
#define MACRO_vtObjectTypeS iVtObjectPictureGraphic_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint8_t ui8_graphicType = 0;

    const HUGE_MEM uint8_t* rawData = NULL;
    uint32_t numberOfBytesInRawData = 0;
    uint16_t actualWidth = 0;
    uint16_t actualHeight = 0;
    uint8_t options = 0;

    MACRO_calculateRequestedSize

    MACRO_CheckFixedBitmapsLoop_start
        rawData = vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].rawData;
        numberOfBytesInRawData = vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].numberOfBytesInRawData;
        actualWidth = vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].actualWidth;
        actualHeight = vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].actualHeight;
        /* format is bit 8+7, options is bit 2-0 (NO RLE1/4/8 stuff here!! */
        options = vtObjectPictureGraphic_a->fixedBitmapsToFollow [fixNr].formatoptions & 0x7;
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

    // Get a ref to the vtClient, so that we can convert colours by calling getUserConvertedColor() over and over
    VtClientConnection_c& vtClient = __IsoAgLib::getVtClientInstance4Comm().getClientByID(s_properties.clientId);
    const uint32_t pgheaderSize = 17;

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
      destMemory [11] = vtClient.getUserConvertedColor(vtObjectPictureGraphic_a->transparencyColour, this, IsoAgLib::TransparencyColour);
      destMemory [12] = (numberOfBytesInRawData) & 0xFF;
      destMemory [13] = (numberOfBytesInRawData >> 8) & 0xFF;
      destMemory [14] = (numberOfBytesInRawData >> 16) & 0xFF;
      destMemory [15] = (numberOfBytesInRawData >> 24) & 0xFF;
      destMemory [16] = vtObjectPictureGraphic_a->numberOfMacrosToFollow;

      sourceOffset += pgheaderSize;
      curBytes += pgheaderSize;
    }

#ifdef CONFIG_VT_CLIENT_PICTURE_GRAPHIC_COLOUR_CONVERSION
    // Colour Operation is for b/w picture graphics only!
    // let's get color 0 (black) and color 1 (white) converted by the application
    // and decide then what to do with these colors in case of a b/w terminal (graphicType==0)
    uint8_t colourOperation = 0;
    enum ColourOperation_e
    {
                    // W B (White, Black) - 1 if that colour changed its value during the conversion
          NoChange  // 0 0 = 0 - No change (leave all bits alone)
        , SetToZero // 1 0 = 1 - 0 (set all bits to 0=black)
        , SetToOne  // 0 1 = 2 - 1 (set all bits to 1=white)
        , Toggle    // 1 1 = 3 - XOR (toggle all bits with XOR)
    };

    if( ui8_graphicType == 0 )
    {
        for( uint8_t colourCode=0; colourCode<2;++colourCode )
        {
            uint8_t convertedColour = vtClient.getUserConvertedColor( colourCode, this, IsoAgLib::PictureGraphicColour );

            if( convertedColour != colourCode )
            {
                // If the colour toggled (from 1 to 0 or 0 to 1 which are the only possibilities for 1-bit color)
                // then set that colour's bit (see table under ColourOperation_e)
                // Black is bit 0, White is bit 1 (that's the reason for the shift below)
                colourOperation |= (1 << colourCode);
            }
        }
    }
#endif

    while ((sourceOffset >= pgheaderSize) && (sourceOffset < (pgheaderSize+numberOfBytesInRawData)) && ((curBytes+1) <= maxBytes))
    {
#ifdef CONFIG_VT_CLIENT_PICTURE_GRAPHIC_COLOUR_CONVERSION
        if( sourceOffset < (pgheaderSize + (vtObjectPictureGraphic_a->numberOfMacrosToFollow << 1)) )
        {
            // Copy over the macros
            // 2 bytes for each macro defined, so the end of the macros is
            // sourceOffset + (pgheaderSize + (vtObjectPictureGraphic_a->numberOfMacrosToFollow << 1))
            destMemory [curBytes] = rawData [sourceOffset-pgheaderSize];
        }
        else
        {
            // If this is a RunLength Encoded 2-byte pair (count, colour), and this is not the colour byte
            if ((options & 0x04) && (sourceOffset & 0x01))
            {
                // just return the byte without conversion
                destMemory[curBytes] = rawData[sourceOffset - pgheaderSize];
            }
            else
            {
                // Otherwise, either it is not RLE encoded
                // or this is the colour (2nd) byte of the 2-byte pair
                // So, yes, we need to convert the color

                // Try to do colour conversion on the bitmap if necessary (during upload)
                switch (ui8_graphicType)
                {
                case 2: // 8-bit
                    destMemory[curBytes] = vtClient.getUserConvertedColor(rawData[sourceOffset - pgheaderSize], this, IsoAgLib::PictureGraphicColour);
                    break;

                case 1: // 4-bit - Convert only 4 bits at a time (2 function calls)
                    // Convert 2 color codes (YES, convert color)
                    destMemory[curBytes] =
                        (
                            vtClient.getUserConvertedColor(rawData[sourceOffset - pgheaderSize] & 0x0F, this, IsoAgLib::PictureGraphicColour)			// Colour in Bottom 4 bits
                        |
                            (vtClient.getUserConvertedColor((rawData[sourceOffset - pgheaderSize] & 0xF0) >> 4, this, IsoAgLib::PictureGraphicColour)	// Colour in Top 4 bits
                             << 4)
                        );
                    break;

                case 0: // 1-bit - Convert all 8-bits at once
                    switch (colourOperation)
                    {
                    case NoChange:
                        destMemory[curBytes] = rawData[sourceOffset - pgheaderSize];
                        break;

                    case SetToZero:
                        destMemory[curBytes] = 0;
                        break;

                    case SetToOne:
                        destMemory[curBytes] = 0xFF;
                        break;

                    case Toggle:
                        destMemory[curBytes] = rawData[sourceOffset - pgheaderSize] ^ 0xFF;
                        break;
                    }
                    break;

                default:
                    isoaglib_assert( !"Error - Unknown ui8_graphicType, wrong object pool data!" );
                    break;
                }
            }
        }
#else
      destMemory [curBytes] = rawData [sourceOffset-pgheaderSize];
#endif

      curBytes++;
      sourceOffset++;
    }

    MACRO_streamEventMacro(pgheaderSize+numberOfBytesInRawData);

    return curBytes;
}

vtObjectPictureGraphic_c::vtObjectPictureGraphic_c() {}

uint32_t
vtObjectPictureGraphic_c::fitTerminal() const
{
  MACRO_localVars;
  MACRO_scaleLocalVars;
  MACRO_scaleSKLocalVars;

  uint32_t numberOfBytesInRawData = 0;
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
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectPictureGraphic_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), width), sizeof(iVtObjectPictureGraphic_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), width), sizeof(iVtObjectPictureGraphic_s));
}

uint8_t
vtObjectPictureGraphic_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), options), sizeof(iVtObjectPictureGraphic_s), 2);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), options), sizeof(iVtObjectPictureGraphic_s));
}

uint8_t
vtObjectPictureGraphic_c::updateTransparencyColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), transparencyColour), sizeof(iVtObjectPictureGraphic_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), transparencyColour), sizeof(iVtObjectPictureGraphic_s));
}

/** these attributes are in parentheses in the spec, so commented out here
uint16_t
vtObjectPictureGraphic_c::updateActualWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), actualWidth), sizeof(iVtObjectPictureGraphic_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), actualWidth), sizeof(iVtObjectPictureGraphic_s));
}

uint16_t
vtObjectPictureGraphic_c::updateActualHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), actualHeight), sizeof(iVtObjectPictureGraphic_s), 5);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), actualHeight), sizeof(iVtObjectPictureGraphic_s));
}

uint8_t
vtObjectPictureGraphic_c::updateFormat(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), format), sizeof(iVtObjectPictureGraphic_s), 6);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), format), sizeof(iVtObjectPictureGraphic_s));
}
*/

void
vtObjectPictureGraphic_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), width), sizeof(iVtObjectPictureGraphic_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue8(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), options), sizeof(iVtObjectPictureGraphic_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), transparencyColour), sizeof(iVtObjectPictureGraphic_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    /** these attributes are in parentheses in the spec, so commented out here
    case 4: saveValue16(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), actualWidth), sizeof(iVtObjectPictureGraphic_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 5: saveValue16(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), actualHeight), sizeof(iVtObjectPictureGraphic_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 6: saveValue8(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), format), sizeof(iVtObjectPictureGraphic_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif

} // __IsoAgLib
