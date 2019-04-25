/*
  vtobjectcolourmap_c.cpp

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

#include "vtobjectcolourmap_c.h"

#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

#define min(a,b) (a<b)?a:b

const uint8_t vtObjectColourMap_c::pgheaderSize = 5;

int16_t
vtObjectColourMap_c::stream(uint8_t* destMemory,
                            uint16_t maxBytes,
                            objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectColourMap_a
#define MACRO_vtObjectTypeS iVtObjectColourMap_s
    MACRO_streamLocalVars;

    HUGE_MEM uint8_t const * colourMapArray = vtObjectColourMap_a->colourMapArray;

    uint16_t const numOfColourIdxToFollow = vtObjectColourMap_a->numOfColourIdxToFollow;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = VT_OBJECT_TYPE_COLOUR_MAP;
      destMemory [3] = numOfColourIdxToFollow & 0xFF;
      destMemory [4] = numOfColourIdxToFollow >> 8;

      sourceOffset  += pgheaderSize;
      curBytes      += pgheaderSize;
    }

    while ( (sourceOffset >= pgheaderSize) &&
            (sourceOffset < (pgheaderSize + numOfColourIdxToFollow)) &&
            ((curBytes+1) <= maxBytes) )
    {
      destMemory [curBytes] = colourMapArray [sourceOffset-pgheaderSize];
      curBytes++;
      sourceOffset++;
    }

    return curBytes;
}

vtObjectColourMap_c::vtObjectColourMap_c() {}

uint32_t
vtObjectColourMap_c::fitTerminal() const
{
  MACRO_localVars;
  return pgheaderSize + vtObjectColourMap_a->numOfColourIdxToFollow;
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES


void
vtObjectColourMap_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectColourMap_a(),
                        numOfColourIdxToFollow),
                        sizeof(iVtObjectColourMap_s),
                        convertLittleEndianStringUi16(pui8_attributeValue));
    break;

    default: break;
  }
}
#endif // USE_ISO_TERMINAL_GETATTRIBUTES

} // __IsoAgLib
