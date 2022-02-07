/*
  vtobjectmacro_c.cpp

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

#include "vtobjectmacro_c.h"

#include "vtclient_c.h"


namespace __IsoAgLib {


//TODO!! ID of Macro must be 0..127 or so ..

struct vtObjectMacro_c::iVtObjectMacro_s: iVtObject_s {
	uint16_t numBytesToFollow;
	const uint8_t *commandsToFollow;
	iVtObjectMacro_s(
			IsoAgLib::ObjectID ID,
			uint16_t numBytesToFollow,
			const uint8_t *commandsToFollow)
	: iVtObject_s(ID)
	, numBytesToFollow(numBytesToFollow)
	, commandsToFollow(commandsToFollow)
	{
	}
};



int16_t
vtObjectMacro_c::stream(uint8_t* destMemory,
                        uint16_t maxBytes,
                        objRange_t sourceOffset)
{ // ~X2C
#define MACRO_vtObjectTypeA vtObjectMacro_a
#define MACRO_vtObjectTypeS iVtObjectMacro_s
    MACRO_streamLocalVars;

    uint16_t numBytesToFollow = vtObjectMacro_a->commandsToFollow ? vtObjectMacro_a->numBytesToFollow : 0;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 28; // Object Type = Macro
      destMemory [3] = numBytesToFollow & 0xFF;
      destMemory [4] = numBytesToFollow >> 8;
      curBytes += 5;
      sourceOffset += 5;
    }

    while ((sourceOffset >= 5U) && (sourceOffset < (5U+numBytesToFollow)) && ((curBytes+1) <= maxBytes))
    {
      destMemory [curBytes] = vtObjectMacro_a->commandsToFollow [sourceOffset-5];
      curBytes++;
      sourceOffset++;
    }
    return curBytes;
}


IsoAgLib::ObjectID vtObjectMacro_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}



uint32_t
vtObjectMacro_c::fitTerminal() const
{
  MACRO_localVars;
  return 5 + (vtObjectMacro_a->commandsToFollow ? vtObjectMacro_a->numBytesToFollow : 0);
}

    vtObjectMacro_c::vtObjectMacro_c(vtObjectMacro_c::iVtObjectMacro_s *vtObjectMacroSROM,
                                     int ai_multitonInst)
            :vtObject_c((iVtObject_s*) vtObjectMacroSROM , ai_multitonInst)
    		,vtObject_a(vtObjectMacroSROM)
    {}

    vtObjectMacro_c::iVtObjectMacro_s *vtObjectMacro_c::get_vtObjectMacro_a()
    {
    	return vtObject_a;
    }

    void vtObjectMacro_c::saveReceivedAttribute(uint8_t, uint8_t *) {}

} // __IsoAgLib
