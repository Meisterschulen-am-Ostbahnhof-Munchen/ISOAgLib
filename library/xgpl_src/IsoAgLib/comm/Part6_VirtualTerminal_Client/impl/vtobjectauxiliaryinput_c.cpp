/*
  vtobjectauxiliaryinput_c.cpp

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
#include "vtobjectauxiliaryinput_c.h"

#ifdef CONFIG_USE_VTOBJECT_auxiliaryinput
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {




struct vtObjectAuxiliaryInput_c::iVtObjectAuxiliaryInput_s: iVtObjectObject_s {
	IsoAgLib::Colour backgroundColour;
	uint8_t functionType;
	uint8_t inputId;
	iVtObjectAuxiliaryInput_s(
			IsoAgLib::ObjectID ID,
			IsoAgLib::Colour backgroundColour,
			uint8_t functionType,
			uint8_t inputId)
    : iVtObject_s(ID)
	, iVtObjectObject_s(ID)
	, backgroundColour(backgroundColour)
	, functionType(functionType)
	, inputId(inputId)
	{
	}


};




int16_t
vtObjectAuxiliaryInput_c::stream(uint8_t* destMemory, uint16_t maxBytes, objRange_t sourceOffset)
{
  uint16_t curBytes=0; /* current bytes written */;
  MACRO_scaleLocalVars;
  if (sourceOffset == 0) { // dump out constant sized stuff
    destMemory [0] = vtObject_a->ID & 0xFF;
    destMemory [1] = vtObject_a->ID >> 8;
    destMemory [2] = 30; // Object Type = Auxiliary Input
    destMemory [3] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
    destMemory [4] = vtObject_a->functionType;
    destMemory [5] = vtObject_a->inputId;
    destMemory [6] = vtObject_a->numberOfObjectsToFollow;
    sourceOffset += 7;
    curBytes += 7;
  }

  MACRO_streamObjectXYcenteredInSoftKey(7);

  return curBytes;
}


IsoAgLib::ObjectID vtObjectAuxiliaryInput_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

uint32_t
vtObjectAuxiliaryInput_c::fitTerminal() const
{
  return 7+vtObject_a->numberOfObjectsToFollow*6;
}

bool
vtObjectAuxiliaryInput_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObject_a->numberOfObjectsToFollow, vtObject_a->objectsToFollow);
}

bool
vtObjectAuxiliaryInput_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObject_a->numberOfObjectsToFollow, vtObject_a->objectsToFollow, b_enableReplaceOfCmd, SoftKeyOffset);
}

// Operation : setOriginSKM
//! @param b_SKM:
void
vtObjectAuxiliaryInput_c::setOriginSKM(bool /* b_SKM */)
{
  for (int i=0; i<vtObject_a->numberOfObjectsToFollow; i++) {
    vtObject_a->objectsToFollow[i].vtObject->setOriginSKM (true);
  }
}

    vtObjectAuxiliaryInput_c::vtObjectAuxiliaryInput_c(
            iVtObjectAuxiliaryInput_s *vtObjectAuxiliaryInputSROM, multiton ai_multitonInst)
            :vtObject_c(ai_multitonInst)
    		,vtObject_a(vtObjectAuxiliaryInputSROM)
    {}

    vtObjectAuxiliaryInput_c::~vtObjectAuxiliaryInput_c() = default;


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
    void vtObjectAuxiliaryInput_c::saveReceivedAttribute(uint8_t, uint8_t *) {}
#endif
} // __IsoAgLib
#endif //CONFIG_USE_VTOBJECT_auxiliaryinput
