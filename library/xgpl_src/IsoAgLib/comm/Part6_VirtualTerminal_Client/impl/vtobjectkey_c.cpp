/*
  vtobjectkey_c.cpp

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

#include "vtobjectkey_c.h"


#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


struct vtObjectKey_c::iVtObjectKey_s: iVtObjectObject_s, iVtObjectwMacro_s {
	IsoAgLib::Colour backgroundColour;
	uint8_t keyCode;
	iVtObjectKey_s(
			IsoAgLib::ObjectID ID,
			IsoAgLib::Colour backgroundColour,
			uint8_t keyCode)
	: iVtObject_s(ID)
	, iVtObjectObject_s(ID)
	, iVtObjectwMacro_s(ID)
	, backgroundColour(backgroundColour)
	, keyCode(keyCode)
	{
	}
};

int16_t
vtObjectKey_c::stream(uint8_t* destMemory,
                      uint16_t maxBytes,
                      objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectKey_a
#define MACRO_vtObjectTypeS iVtObjectKey_s
    uint16_t curBytes=0; /* current bytes written */;
    MACRO_scaleLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 5; // Object Type = Key
      destMemory [3] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [4] = vtObject_a->keyCode;
      destMemory [5] = vtObject_a->numberOfObjectsToFollow;
      destMemory [6] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 7;
      curBytes += 7;
    }

    MACRO_streamObjectXYcenteredInSoftKey(7);
    MACRO_streamEventMacro(7U+vtObject_a->numberOfObjectsToFollow*6U);
    return curBytes;
}


IsoAgLib::ObjectID vtObjectKey_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectKey_c::fitTerminal() const
{
  return 7+vtObject_a->numberOfObjectsToFollow*6+vtObject_a->numberOfMacrosToFollow*2;
}

bool
vtObjectKey_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObject_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObject_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectKey_a(), objectsToFollow), sizeof(iVtObjectKey_s), b_enableReplaceOfCmd);
}

bool
vtObjectKey_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObject_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObject_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectKey_a(), objectsToFollow), sizeof(iVtObjectKey_s), b_enableReplaceOfCmd, SoftKeyOffset);
}


void
vtObjectKey_c::setOriginSKM(bool /*b_SKM*/)
{
  for (int i=0; i<vtObject_a->numberOfObjectsToFollow; i++) {
    vtObject_a->objectsToFollow[i].vtObject->setOriginSKM (true);
  }
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint8_t
vtObjectKey_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectKey_a(), backgroundColour), sizeof(iVtObjectKey_s), 1);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectKey_a(), backgroundColour), sizeof(iVtObjectKey_s));
}

uint8_t
vtObjectKey_c::updateKeyCode(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectKey_a(), keyCode), sizeof(iVtObjectKey_s), 2);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectKey_a(), keyCode), sizeof(iVtObjectKey_s));
}

void
vtObjectKey_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue8(MACRO_getStructOffset(get_vtObjectKey_a(), backgroundColour), sizeof(iVtObjectKey_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 2: saveValue8(MACRO_getStructOffset(get_vtObjectKey_a(), keyCode), sizeof(iVtObjectKey_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    default: break;
  }
}
#endif
    vtObjectKey_c::vtObjectKey_c(vtObjectKey_c::iVtObjectKey_s *vtObjectKeySROM, int ai_multitonInst)
            :vtObject_c((iVtObject_s*) vtObjectKeySROM , ai_multitonInst)
    		,vtObject_a(vtObjectKeySROM)
    {}

    vtObjectKey_c::iVtObjectKey_s *vtObjectKey_c::get_vtObjectKey_a()
    {
    	return vtObject_a;
    }

    void vtObjectKey_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectKey_a(), backgroundColour) : 0, sizeof(iVtObjectKey_s), 1, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectKey_c::setKeyCode(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectKey_a(), keyCode) : 0, sizeof(iVtObjectKey_s), 2, newValue, newValue, b_enableReplaceOfCmd);
    }


} // __IsoAgLib


