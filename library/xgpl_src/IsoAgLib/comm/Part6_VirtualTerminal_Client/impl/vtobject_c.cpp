/*
  vtobject_c.cpp

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

#include "vtobject_c.h"
#include "../ivtobject_c.h"
#include "../ivtobjectbutton_c.h"

#include "vtclient_c.h"
#include "vtclientconnection_c.h"
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/util/iassert.h>





namespace __IsoAgLib {



vtObject_c::vtObject_c(int ai_multitonInst)
:iVtObject_c()
{
  // typical double init is caught in objectpool-class's init-call!
  s_properties.flags &= ~FLAG_STRING_IN_RAM;
  s_properties.flags &= ~FLAG_OBJECTS2FOLLOW_IN_RAM;
  // NOTE: If objects were modified using b_updateObject==true,
  // then these modifications are GONE and result in a MEMORY LEAK
  // This is subject to be correctly fixed in some later version, if needed!

  setMultitonInst( ai_multitonInst );
}

vtObject_c::~vtObject_c() = default;

void
vtObject_c::setAttribute(uint8_t attrID, uint32_t newValue, bool b_enableReplaceOfCmd)
{
  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeAttribute(
    this, attrID, newValue & 0xFF, (newValue >> 8) & 0xFF, (newValue >> 16) & 0xFF, newValue >> 24, b_enableReplaceOfCmd );
}


void
vtObject_c::setAttributeFloat(uint8_t attrID, float newValue, bool b_enableReplaceOfCmd)
{
#ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  uint32_t ui32_convertedFloat;
  floatVar2LittleEndianStream (&newValue, &ui32_convertedFloat);
#else
  uint8_t ui8_convertedFloat[4];
  floatVar2LittleEndianStream (&newValue, &ui8_convertedFloat);
  uint32_t ui32_convertedFloat = __IsoAgLib::convertLittleEndianStringUi32(ui8_convertedFloat);
#endif

  setAttribute (attrID, ui32_convertedFloat, b_enableReplaceOfCmd);
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
void
vtObject_c::getAttribute(uint8_t attrID, bool b_enableReplaceOfCmd)
{
  //__IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).sendCommandGetAttributeValue (this, attrID, b_enableReplaceOfCmd);
}
#endif





void
vtObject_c::scaleSize( uint16_t &width, uint16_t &height ) const
{
#ifndef USE_VT_CLIENT_OLD_UNSCALED_SIZE_COMMANDS
  MACRO_scaleLocalVars
  MACRO_scaleSKLocalVars

  uint32_t scaledWidth = uint32_t( width );
  uint32_t scaledHeight = uint32_t( height );
  MACRO_scaleSizeI32(scaledWidth,scaledHeight)
  width = uint16_t( scaledWidth );
  height = uint16_t( scaledHeight );
#else
  ;
#endif
}
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
// //////////////////////////////// get(Signed)Value(8/16/32)GetAttribute
uint8_t
vtObject_c::getValue8GetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_enableReplaceOfCmd) {
  getAttribute (ui8_ind, b_enableReplaceOfCmd);
  return getValue8 (ui16_structOffset, ui16_structLen);
}
uint16_t
vtObject_c::getValue16GetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_enableReplaceOfCmd) {
  getAttribute (ui8_ind, b_enableReplaceOfCmd);
  return getValue16 (ui16_structOffset, ui16_structLen);
}
uint32_t
vtObject_c::getValue32GetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_enableReplaceOfCmd) {
  getAttribute (ui8_ind, b_enableReplaceOfCmd);
  return getValue32 (ui16_structOffset, ui16_structLen);
}
int8_t
vtObject_c::getSignedValue8GetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_enableReplaceOfCmd) {
  getAttribute (ui8_ind, b_enableReplaceOfCmd);
  return getSignedValue8 (ui16_structOffset, ui16_structLen);
}
int16_t
vtObject_c::getSignedValue16GetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_enableReplaceOfCmd) {
  getAttribute (ui8_ind, b_enableReplaceOfCmd);
  return getSignedValue16 (ui16_structOffset, ui16_structLen);
}
int32_t
vtObject_c::getSignedValue32GetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_enableReplaceOfCmd) {
  getAttribute (ui8_ind, b_enableReplaceOfCmd);
  return getSignedValue32 (ui16_structOffset, ui16_structLen);
}
float
vtObject_c::getValueFloatGetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_enableReplaceOfCmd) {
  getAttribute (ui8_ind, b_enableReplaceOfCmd);
  return getValueFloat (ui16_structOffset, ui16_structLen);
}
#endif


bool
vtObject_c::genericChangeChildLocationPosition (bool ab_isLocation, IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen)
{
  bool foundAtLeastOnce = false;
  // Find the child object in question
  for(uint8_t i = 0; i < numObjectsToFollow; i++) {
    if (childObject->getID() == objectsToFollow[i].vtObject->getID()) {
      if (b_updateObject) {
        if (ab_isLocation) {
          objectsToFollow[i].x = objectsToFollow[i].x + dx;
          objectsToFollow[i].y = objectsToFollow[i].y + dy;
        } else {
          objectsToFollow[i].x = dx;
          objectsToFollow[i].y = dy;
        }
      }
      foundAtLeastOnce = true; // Object was child object, so its position could be changed
    }
  }
  return foundAtLeastOnce; // Object was not child object
}


// Valid Range for (scaled) dx and dy in the ISO-command: -127..0..+128
// Function will fail if the SCALED relative movement is out of the -127..+128 range!
// The application needs to be aware of this and in case of a negative return value this
// movement is not possible in one command so it's up to the application to divide it into
// multiple ChangeChildLocation commands - or simply use the ChangeChildPosition instead,
// which is typically better anyway because it avoids rounding/truncation problems!
bool
vtObject_c::genericChangeChildLocation (IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_enableReplaceOfCmd)
{
  MACRO_scaleLocalVars
  MACRO_scaleSKLocalVars

  int dx32 = int32_t( dx );
  int dy32 = int32_t( dy );
#ifndef USE_VT_CLIENT_OLD_UNSCALED_CHILD_POSITIONING_COMMANDS
  MACRO_scaleI32(dx32,dy32)
#endif

  VtClientConnection_c &connection = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId);

  if ((dx32 < -127) || (dx32 > 128)
   || (dy32 < -127) || (dy32 > 128)) return false;

  bool b_result = genericChangeChildLocationPosition (true, childObject, dx, dy, b_updateObject, numObjectsToFollow, objectsToFollow, ui16_structOffset, ui16_structLen);
  if (b_result)
    connection.commandHandler().sendCommandChangeChildLocation(
      this, childObject, int16_t( dx32 ), int16_t( dy32 ), b_enableReplaceOfCmd );

  return b_result;
}


bool
vtObject_c::genericChangeChildPosition (IsoAgLib::iVtObject_c* childObject, int16_t x, int16_t y, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_enableReplaceOfCmd, OffsetMode_en offsetMode)
{
  MACRO_scaleLocalVars
  MACRO_scaleSKLocalVars

  int x32 = int32_t( x );
  int y32 = int32_t( y );
#ifndef USE_VT_CLIENT_OLD_UNSCALED_CHILD_POSITIONING_COMMANDS
  MACRO_scaleI32(x32,y32)
#endif
  switch( offsetMode )
  {
  case DataAlarmMaskOffset:
    x32 += vtOffsetX;
    y32 += vtOffsetY;
    break;

  case SoftKeyOffset:
    {
      const int16_t centerX = (vtSoftKeyWidth -  ((opSoftKeyWidth *factorM)/factorD)) >>1;
      const int16_t centerY = (vtSoftKeyHeight - ((opSoftKeyHeight*factorM)/factorD)) >>1;
      x32 += centerX + skOffsetX;
      y32 += centerY + skOffsetY;
    } break;

  case NoOffset:
    ;
  }

  VtClientConnection_c &connection = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId);

  bool b_result = genericChangeChildLocationPosition (false, childObject, x, y, b_updateObject, numObjectsToFollow, objectsToFollow, ui16_structOffset, ui16_structLen);
  if (b_result)
    connection.commandHandler().sendCommandChangeChildPosition(
      this, childObject, int16_t( x32 ), int16_t( y32 ), b_enableReplaceOfCmd );

  return b_result;
}


bool
vtObject_c::able (uint8_t enOrDis, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject)
    updateEnable (enOrDis);

  return __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommand(
    0xA1 /* Command: Command --- Parameter: Enable/Disable Object */,
	getID() & 0xFF, getID() >> 8, enOrDis, 0xFF, 0xFF, 0xFF, 0xFF, b_enableReplaceOfCmd );
}


bool
vtObject_c::select(uint8_t selectOrActivate)
{
  return __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandSelectInputObject(
    getID(), (selectOrActivate == 0), true );
}
    bool
    vtObject_c::isOmittedFromUpload() const
    {
        return (s_properties.flags & FLAG_OMIT_OBJECT) != 0;
    }

    void vtObject_c::updateEnable(uint8_t) {}

} // __IsoAgLib
