/*
  vtobjectalarmmask_c.cpp

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

#include "vtobjectalarmmask_c.h"


#include "vtclient_c.h"
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectmacro_c.h"


namespace __IsoAgLib {



struct vtObjectAlarmMask_c::iVtObjectAlarmMask_s : iVtObjectMask_s {
  IsoAgLib::Colour backgroundColour;
  IsoAgLib::iVtObjectSoftKeyMask_c* softKeyMask;
  uint8_t priority;
  uint8_t acousticSignal;
  explicit iVtObjectAlarmMask_s(
		  IsoAgLib::ObjectID ID,
		  IsoAgLib::Colour backgroundColour,
		  IsoAgLib::iVtObjectSoftKeyMask_c *softKeyMask,
		  uint8_t priority,
          uint8_t acousticSignal)
  : iVtObject_s(ID)
  , iVtObjectMask_s(ID)
  , backgroundColour(backgroundColour)
  , softKeyMask(softKeyMask)
  , priority(priority)
  , acousticSignal(acousticSignal)
  {}
};

int16_t
vtObjectAlarmMask_c::stream(uint8_t* destMemory,
                            uint16_t maxBytes,
                            objRange_t sourceOffset)
{
    uint16_t curBytes=0; /* current bytes written */;
    MACRO_scaleLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 2; // Object Type = Alarm Mask
      destMemory [3] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      if (vtObject_a->softKeyMask != NULL) {
          destMemory [4] = vtObject_a->softKeyMask->getID() & 0xFF;
          destMemory [5] = vtObject_a->softKeyMask->getID() >> 8;
      } else {
          destMemory [4] = 0xFF;
          destMemory [5] = 0xFF;
      }
      destMemory [6] = vtObject_a->priority;
      destMemory [7] = vtObject_a->acousticSignal;
      destMemory [8] = vtObject_a->numberOfObjectsToFollow;
      destMemory [9] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 10;
      curBytes += 10;
    }

    MACRO_DAMstreamObjectXY(10);
    MACRO_streamEventMacro(10U+vtObject_a->numberOfObjectsToFollow*6U);
    return curBytes;
}



IsoAgLib::ObjectID vtObjectAlarmMask_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectAlarmMask_c::fitTerminal() const
{
	return 10 + vtObject_a->numberOfObjectsToFollow * 6	+ vtObject_a->numberOfMacrosToFollow * 2;
}


void
vtObjectAlarmMask_c::setSoftKeyMask(IsoAgLib::iVtObjectSoftKeyMask_c* newSoftKeyMask,
                                    bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject)
	  vtObject_a->softKeyMask = newSoftKeyMask;

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSoftKeyMask(
    this, 2 /* "Type: Alarm Mask" */, (newSoftKeyMask == NULL) ? 0xFFFF : newSoftKeyMask->getID(), b_enableReplaceOfCmd );
}

/// No cmdReplacing here, as it's a relative command!!
bool
vtObjectAlarmMask_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject)
{
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObject_a->numberOfObjectsToFollow, vtObject_a->objectsToFollow, MACRO_getStructOffset(get_vtObjectAlarmMask_a(), objectsToFollow), sizeof(iVtObjectAlarmMask_s), false);
}

bool
vtObjectAlarmMask_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObject_a->numberOfObjectsToFollow, vtObject_a->objectsToFollow, MACRO_getStructOffset(get_vtObjectAlarmMask_a(), objectsToFollow), sizeof(iVtObjectAlarmMask_s), b_enableReplaceOfCmd, DataAlarmMaskOffset);
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint8_t
vtObjectAlarmMask_c::updateBackgroundColour (bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), backgroundColour), sizeof(iVtObjectAlarmMask_s), 1);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), backgroundColour), sizeof(iVtObjectAlarmMask_s));
}

uint16_t
vtObjectAlarmMask_c::updateSoftKeyMask(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), softKeyMask), sizeof(iVtObjectAlarmMask_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), softKeyMask), sizeof(iVtObjectAlarmMask_s));
}

uint8_t
vtObjectAlarmMask_c::updatePriority (bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), priority), sizeof(iVtObjectAlarmMask_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), priority), sizeof(iVtObjectAlarmMask_s));
}

uint8_t
vtObjectAlarmMask_c::updateAcousticSignal(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), acousticSignal), sizeof(iVtObjectAlarmMask_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), acousticSignal), sizeof(iVtObjectAlarmMask_s));
}

void
vtObjectAlarmMask_c::saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue8(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), backgroundColour), sizeof(iVtObjectAlarmMask_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), softKeyMask), sizeof(iVtObjectAlarmMask_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), priority), sizeof(iVtObjectAlarmMask_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectAlarmMask_a(), acousticSignal), sizeof(iVtObjectAlarmMask_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    default: break;
  }
}
#endif


vtObjectAlarmMask_c::iVtObjectAlarmMask_s *vtObjectAlarmMask_c::get_vtObjectAlarmMask_a() {
	return vtObject_a;
}

    void vtObjectAlarmMask_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectAlarmMask_a(), backgroundColour) : 0, sizeof(iVtObjectAlarmMask_s), 1, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

    void vtObjectAlarmMask_c::setPriority(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectAlarmMask_a(), priority) : 0, sizeof(iVtObjectAlarmMask_s), 3, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectAlarmMask_c::setAcousticSignal(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectAlarmMask_a(), acousticSignal) : 0, sizeof(iVtObjectAlarmMask_s), 4, newValue, newValue, b_enableReplaceOfCmd);
    }



    vtObjectAlarmMask_c::vtObjectAlarmMask_c(
  		  int ai_multitonInst,
  		  IsoAgLib::ObjectID ID,
  		  IsoAgLib::Colour backgroundColour,
  		  IsoAgLib::iVtObjectSoftKeyMask_c *softKeyMask,
  		  uint8_t priority,
  		  uint8_t acousticSignal)
    :vtObjectAlarmMask_c(
    		new iVtObjectAlarmMask_s(
    		  		  ID,
    		  		  backgroundColour,
    		  		  softKeyMask,
    		  		  priority,
    		  		  acousticSignal
    				),
					ai_multitonInst
  		  )
    {

    }

    vtObjectAlarmMask_c::vtObjectAlarmMask_c(
    		vtObjectAlarmMask_c::iVtObjectAlarmMask_s *vtObjectAlarmMaskSROM,
            int ai_multitonInst)
            : iVtObjectMask_c((iVtObjectMask_s*) vtObjectAlarmMaskSROM, ai_multitonInst)
    		, vtObject_a(vtObjectAlarmMaskSROM)
    {}


} // end of namespace __IsoAgLib

