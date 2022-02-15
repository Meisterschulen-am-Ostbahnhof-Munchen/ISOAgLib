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

#ifdef CONFIG_USE_VTOBJECT_colourmap

#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


struct vtObjectColourMap_c::iVtObjectColourMap_s: iVtObject_s {
	uint16_t numOfColourIdxToFollow;
	const uint8_t *colourMapArray;
	iVtObjectColourMap_s(
			IsoAgLib::ObjectID ID,
			uint16_t numOfColourIdxToFollow,
			const uint8_t *colourMapArray)
	: iVtObject_s(ID)
	, numOfColourIdxToFollow(numOfColourIdxToFollow)
	, colourMapArray(colourMapArray)
	{
	}


};

#define min(a,b) (a<b)?a:b

const uint8_t vtObjectColourMap_c::pgheaderSize = 5;

int16_t
vtObjectColourMap_c::stream(uint8_t* destMemory,
                            uint16_t maxBytes,
                            objRange_t sourceOffset)
{
    uint16_t curBytes=0; /* current bytes written */;

    uint8_t const * colourMapArray = vtObject_a->colourMapArray;

    uint16_t const numOfColourIdxToFollow = vtObject_a->numOfColourIdxToFollow;

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

IsoAgLib::ObjectID vtObjectColourMap_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectColourMap_c::fitTerminal() const
{
  return pgheaderSize + vtObject_a->numOfColourIdxToFollow;
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES


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
#endif // CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES



    vtObjectColourMap_c::vtObjectColourMap_c(vtObjectColourMap_c::iVtObjectColourMap_s *vtObjectColourMapSROM,
                                             int ai_multitonInst)
            :vtObject_c(ai_multitonInst)
    		,vtObject_a(vtObjectColourMapSROM)
    {}



} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_colourmap
