/*
  vtobjectdatamask_c.cpp

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

#include "vtobjectdatamask_c.h"

#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

int16_t
vtObjectDataMask_c::stream(uint8_t* destMemory,
                           uint16_t maxBytes,
                           objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectDataMask_a
#define MACRO_vtObjectTypeS iVtObjectDataMask_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObjectDataMask_a->ID & 0xFF;
      destMemory [1] = vtObjectDataMask_a->ID >> 8;
      destMemory [2] = 1; // Object Type = Data Mask
      destMemory [3] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObjectDataMask_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      if (vtObjectDataMask_a->softKeyMask != NULL) {
          destMemory [4] = vtObjectDataMask_a->softKeyMask->getID() & 0xFF;
          destMemory [5] = vtObjectDataMask_a->softKeyMask->getID() >> 8;
      } else {
          destMemory [4] = 0xFF;
          destMemory [5] = 0xFF;
      }
      destMemory [6] = vtObjectDataMask_a->numberOfObjectsToFollow;
      destMemory [7] = vtObjectDataMask_a->numberOfMacrosToFollow;
      sourceOffset += 8;
      curBytes += 8;
    }

    MACRO_DAMstreamObjectXY(8);
    MACRO_streamEventMacro(8U+vtObjectDataMask_a->numberOfObjectsToFollow*6U);

    return curBytes;
}



vtObjectDataMask_c::vtObjectDataMask_c(const iVtObjectDataMask_s* vtObjectDataMaskSROM , int ai_multitonInst)
: iVtObjectMask_c((iVtObjectMask_s*)vtObjectDataMaskSROM , ai_multitonInst)
{}



uint32_t
vtObjectDataMask_c::fitTerminal() const
{
  MACRO_localVars;
  return 8+vtObjectDataMask_a->numberOfObjectsToFollow*6+vtObjectDataMask_a->numberOfMacrosToFollow*2;
}


void
vtObjectDataMask_c::setSoftKeyMask(IsoAgLib::iVtObjectSoftKeyMask_c* newSoftKeyMask, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) saveValueP (MACRO_getStructOffset(get_vtObjectDataMask_a(), softKeyMask), sizeof(iVtObjectDataMask_s), newSoftKeyMask);

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSoftKeyMask(
    this, 1 /* "Type: Data Mask" */, (newSoftKeyMask == NULL) ? 0xFFFF : newSoftKeyMask->getID(), b_enableReplaceOfCmd );
}

bool
vtObjectDataMask_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObjectDataMask_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObjectDataMask_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectDataMask_a(), objectsToFollow), sizeof(iVtObjectDataMask_s), b_enableReplaceOfCmd);
}

bool
vtObjectDataMask_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObjectDataMask_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObjectDataMask_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectDataMask_a(), objectsToFollow), sizeof(iVtObjectDataMask_s), b_enableReplaceOfCmd, DataAlarmMaskOffset);
}


bool
vtObjectDataMask_c::lockUnlockMask( bool b_lockMask, uint16_t ui16_lockTimeOut )
{
  bool result = true;
  if( __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getVersion() >= 4 )
  {
    result = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandLockUnlockMask( this, b_lockMask, ui16_lockTimeOut );
  }
  return result;
}


void vtObjectDataMask_c::Append(iVtObject_c* const vtObject, int16_t x, int16_t y)
{
	MACRO_localVars;
	vtObjectDataMask_a->Append(vtObject, x, y);
}




#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint8_t
vtObjectDataMask_c::updateBackgroundColour (bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectDataMask_a(), backgroundColour), sizeof(iVtObjectDataMask_s), 1);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectDataMask_a(), backgroundColour), sizeof(iVtObjectDataMask_s));
}

uint16_t
vtObjectDataMask_c::updateSoftKeyMask(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectDataMask_a(), softKeyMask), sizeof(iVtObjectDataMask_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectDataMask_a(), softKeyMask), sizeof(iVtObjectDataMask_s));
}

void
vtObjectDataMask_c::saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue8(MACRO_getStructOffset(get_vtObjectDataMask_a(), backgroundColour), sizeof(iVtObjectDataMask_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectDataMask_a(), softKeyMask), sizeof(iVtObjectDataMask_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    default: break;
  }
}
#endif

IsoAgLib::iVtObject_c::iVtObjectDataMask_s *vtObjectDataMask_c::get_vtObjectDataMask_a() { return dynamic_cast<iVtObjectDataMask_s *>(&(get_vtObject_a())); }



} // __IsoAgLib
