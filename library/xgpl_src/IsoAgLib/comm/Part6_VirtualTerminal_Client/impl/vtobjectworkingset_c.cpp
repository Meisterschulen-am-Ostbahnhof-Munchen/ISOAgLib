/*
  vtobjectworkingset_c.cpp

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
#include "vtobjectworkingset_c.h"

#include "vtclient_c.h"
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"


namespace __IsoAgLib {



struct vtObjectWorkingSet_c::iVtObjectWorkingSet_s : iVtObjectObject_s, iVtObjectwMacro_s, iVtObjectLanguages_s {
  IsoAgLib::Colour backgroundColour;
  uint8_t selectable;
  IsoAgLib::iVtObjectMask_c* activeMask; // data or alarm mask
  explicit iVtObjectWorkingSet_s(
		IsoAgLib::ObjectID ID,
		IsoAgLib::Colour backgroundColour,
  		uint8_t selectable,
		IsoAgLib::iVtObjectMask_c* activeMask // data or alarm mask
  	  	  )
    : iVtObject_s(ID)
    , iVtObjectObject_s(ID)
    , iVtObjectwMacro_s(ID)
    , iVtObjectLanguages_s(ID)
    , backgroundColour(backgroundColour)
    , selectable(selectable)
	, activeMask(activeMask)
  {}
};

int16_t
vtObjectWorkingSet_c::stream(uint8_t* destMemory,
                             uint16_t maxBytes,
                             objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectWorkingSet_a
#define MACRO_vtObjectTypeS iVtObjectWorkingSet_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
        destMemory [0] = vtObject_a->ID & 0xFF;
        destMemory [1] = vtObject_a->ID >> 8;
        destMemory [2] = 0; // Object Type = Working Set
        destMemory [3] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
        destMemory [4] = vtObject_a->selectable;
        if (vtObject_a->activeMask != NULL) {
            destMemory [5] = vtObject_a->activeMask->getID() & 0xFF;
            destMemory [6] = vtObject_a->activeMask->getID() >> 8;
        } else {
            destMemory [5] = 0; // using 0x00 here as 0xFFFF is NOT allowed
            destMemory [6] = 0; // using 0x00 here as 0xFFFF is NOT allowed
        }
        destMemory [7] = vtObject_a->numberOfObjectsToFollow;
        destMemory [8] = vtObject_a->numberOfMacrosToFollow;
        destMemory [9] = vtObject_a->numberOfLanguagesToFollow;

        sourceOffset += 10;
        curBytes += 10;
    }

    MACRO_streamObjectXYcenteredInSoftKey(10);
    uint16_t tempOffset = 10+vtObject_a->numberOfObjectsToFollow*6;
    MACRO_streamEventMacro(tempOffset);
    tempOffset = tempOffset + vtObject_a->numberOfMacrosToFollow*2;
    MACRO_streamLanguages(tempOffset);

    return curBytes;
}


IsoAgLib::ObjectID vtObjectWorkingSet_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

vtObjectWorkingSet_c::vtObjectWorkingSet_c(
		int ai_multitonInst,
		IsoAgLib::ObjectID ID,
		IsoAgLib::Colour backgroundColour,
  		uint8_t selectable,
		IsoAgLib::iVtObjectMask_c* activeMask // data or alarm mask
  		)
	:vtObjectWorkingSet_c(
			new iVtObjectWorkingSet_s(
					ID,
					backgroundColour,
					selectable,
					activeMask),
			ai_multitonInst)
{
}

vtObjectWorkingSet_c::vtObjectWorkingSet_c(iVtObjectWorkingSet_s* vtObjectWorkingSetSROM , int ai_multitonInst)
	: vtObject_c((iVtObject_s*) vtObjectWorkingSetSROM , ai_multitonInst)
	, vtObject_a(vtObjectWorkingSetSROM)

{}

vtObjectWorkingSet_c::iVtObjectWorkingSet_s* vtObjectWorkingSet_c::get_vtObjectWorkingSet_a() {
	return vtObject_a;
}



void vtObjectWorkingSet_c::Append(iVtObject_c* const vtObject, int16_t x, int16_t y)
{
	vtObject_a->Append(vtObject, x, y);
}


uint32_t
vtObjectWorkingSet_c::fitTerminal() const
{
  return 10+vtObject_a->numberOfObjectsToFollow*6+vtObject_a->numberOfMacrosToFollow*2+vtObject_a->numberOfLanguagesToFollow*2;
}


void
vtObjectWorkingSet_c::updateSelectable( uint8_t newSelectable )
{
	vtObject_a->selectable = newSelectable;
}

void
vtObjectWorkingSet_c::changeActiveMask(IsoAgLib::iVtObjectMask_c* apc_vtObjectMask, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject)
	  vtObject_a->activeMask = apc_vtObjectMask;

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeActiveMask( this, apc_vtObjectMask, b_enableReplaceOfCmd);
}

void
vtObjectWorkingSet_c::changeBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject)
	  vtObject_a->backgroundColour = newValue;

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeBackgroundColour (this, newValue, b_enableReplaceOfCmd);
}

bool
vtObjectWorkingSet_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObject_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObject_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectWorkingSet_a(), objectsToFollow), sizeof(iVtObjectWorkingSet_s), b_enableReplaceOfCmd);
}

bool
vtObjectWorkingSet_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObject_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObject_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectWorkingSet_a(), objectsToFollow), sizeof(iVtObjectWorkingSet_s), b_enableReplaceOfCmd, SoftKeyOffset);
}

bool
vtObjectWorkingSet_c::controlAudioDevice (uint8_t aui8_repetitions, uint16_t aui16_frequency, uint16_t aui16_onTime, uint16_t aui16_offTime)
{
  return __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandControlAudioDevice (aui8_repetitions, aui16_frequency, aui16_onTime, aui16_offTime);
}

bool
vtObjectWorkingSet_c::setAudioVolume (uint8_t aui8_volume)
{
  return __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandSetAudioVolume (aui8_volume);
}

bool
vtObjectWorkingSet_c::setColourMapOrPalette (uint16_t aui16_objectId)
{
  bool result = true;
  if( __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getVersion() >= 4 )
  {
    result = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandSetColourMapOrPalette (aui16_objectId);
  }
  return result;
}

void
vtObjectWorkingSet_c::setOriginSKM(bool /*b_SKM*/)
{
  s_properties.flags |= FLAG_ORIGIN_SKM; // WS Descriptor has to fit inside a SoftKey!
  for (int i=0; i<vtObject_a->numberOfObjectsToFollow; i++) {
    vtObject_a->objectsToFollow[i].vtObject->setOriginSKM (true);
  }
}
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
/** these attributes are in parentheses in the spec, so commented out here
uint8_t
vtObjectWorkingSet_c::updateBackgroundColour (bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectWorkingSet_a(), backgroundColour), sizeof(iVtObjectWorkingSet_s), 1);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectWorkingSet_a(), backgroundColour), sizeof(iVtObjectWorkingSet_s));
}

uint8_t
vtObjectWorkingSet_c::updateSelectable(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectWorkingSet_a(), selectable), sizeof(iVtObjectWorkingSet_s), 2);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectWorkingSet_a(), selectable), sizeof(iVtObjectWorkingSet_s));
}

uint16_t
vtObjectWorkingSet_c::updateActiveMask(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectWorkingSet_a(), activeMask), sizeof(iVtObjectWorkingSet_s), 3);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectWorkingSet_a(), activeMask), sizeof(iVtObjectWorkingSet_s));
}
*/

void
vtObjectWorkingSet_c::saveReceivedAttribute (uint8_t attrID, uint8_t* /*pui8_attributeValue*/)
{
  switch (attrID)
  {
    /** these attributes are in parentheses in the spec, so commented out here
    case 1: saveValue8(MACRO_getStructOffset(get_vtObjectWorkingSet_a(), backgroundColour), sizeof(iVtObjectWorkingSet_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 2: saveValue8(MACRO_getStructOffset(get_vtObjectWorkingSet_a(), selectable), sizeof(iVtObjectWorkingSet_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 3: saveValue16(MACRO_getStructOffset(get_vtObjectWorkingSet_a(), activeMask), sizeof(iVtObjectWorkingSet_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    */
    default: break;
  }
}

#endif





} // __IsoAgLib
