/***************************************************************************
                          vtobject_c.cpp
                             -------------------
    begin                : Don Sep 4 2003
    copyright            : (C) 2003 by Martin Wodok
    email                : m.wodok@osb-ag:de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

#include "vtobject_c.h"
#include "isoterminal_c.h"

#include <IsoAgLib/util/impl/util_funcs.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {



// //////////////////////////////// +X2C Operation 783 : setAttribute
//! Parameter:
//! @param attrID: Attribute ID of the object's attribute
//! @param newValue: New Value of the attribute
void
vtObject_c::setAttribute(uint8_t attrID, uint32_t newValue, bool b_enableReplaceOfCmd)
{ // ~X2C
  __IsoAgLib::getIsoTerminalInstance4Comm().sendCommandChangeAttribute (this, attrID, newValue & 0xFF, (newValue >> 8) & 0xFF, (newValue >> 16) & 0xFF, newValue >> 24, b_enableReplaceOfCmd);
} // -X2C

void
vtObject_c::setAttributeFloat(uint8_t attrID, float newValue, bool b_enableReplaceOfCmd)
{ // ~X2C
  uint32_t ui32_convertedFloat;
  float2Int (&newValue, &ui32_convertedFloat);
  setAttribute (attrID, ui32_convertedFloat, b_enableReplaceOfCmd);
} // -X2C



void
vtObject_c::createRamStructIfNotYet (uint16_t ui16_structLen)
{ // Do we have to generate a RAM copy of our struct (to save the value), or has this already be done?
  if (!(flags & FLAG_IN_RAM)) {
    void* old=vtObject_a;
    vtObject_a = (iVtObject_s*) new (uint8_t [ui16_structLen]);
    CNAMESPACE::memcpy (vtObject_a, old, ui16_structLen);
    flags |= FLAG_IN_RAM;
  }
}


// //////////////////////////////// saveValue(8/16/32)
void
vtObject_c::saveValue8 (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_newValue)
{
  createRamStructIfNotYet (ui16_structLen);
  ((uint8_t *)vtObject_a) [ui16_structOffset] = ui8_newValue;
}
void
vtObject_c::saveValue16 (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint16_t ui16_newValue)
{
  createRamStructIfNotYet (ui16_structLen);
  * ((uint16_t*) (((uint8_t *)vtObject_a)+ui16_structOffset)) = ui16_newValue;
}
void
vtObject_c::saveValue32 (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint32_t ui32_newValue)
{
  createRamStructIfNotYet (ui16_structLen);
  * ((uint32_t*) (((uint8_t *)vtObject_a)+ui16_structOffset)) = ui32_newValue;
}
void
vtObject_c::saveValueFloat (uint16_t ui16_structOffset, uint16_t ui16_structLen, float f_newValue)
{
  createRamStructIfNotYet (ui16_structLen);
  * ((float*) (((uint8_t *)vtObject_a)+ui16_structOffset)) = f_newValue;
}
void
vtObject_c::saveValueP (uint16_t ui16_structOffset, uint16_t ui16_structLen, IsoAgLib::iVtObject_c* p_newValue)
{
  createRamStructIfNotYet (ui16_structLen);
  * ((IsoAgLib::iVtObject_c**) (((uint8_t *)vtObject_a)+ui16_structOffset)) = p_newValue;
}

// //////////////////////////////// saveValue(8/16/32)SetAttribute
void
vtObject_c::saveValue8SetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, uint8_t ui8_newValue, uint8_t ui8_newValueSend, bool b_enableReplaceOfCmd) {
  if (ui16_structOffset != 0) saveValue8 (ui16_structOffset, ui16_structLen, ui8_newValue);
  setAttribute (ui8_ind, (uint32_t) ui8_newValueSend, b_enableReplaceOfCmd);
}
void
vtObject_c::saveValue16SetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, uint16_t ui16_newValue, bool b_enableReplaceOfCmd) {
  if (ui16_structOffset != 0) saveValue16 (ui16_structOffset, ui16_structLen, ui16_newValue);
  setAttribute (ui8_ind, (uint32_t) ui16_newValue, b_enableReplaceOfCmd);
}
void
vtObject_c::saveValue32SetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, uint32_t ui32_newValue, bool b_enableReplaceOfCmd) {
  if (ui16_structOffset != 0) saveValue32 (ui16_structOffset, ui16_structLen, ui32_newValue);
  setAttribute (ui8_ind, ui32_newValue, b_enableReplaceOfCmd);
}
void
vtObject_c::saveValueFloatSetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, float f_newValue, bool b_enableReplaceOfCmd) {
  if (ui16_structOffset != 0) saveValueFloat (ui16_structOffset, ui16_structLen, f_newValue);
  setAttributeFloat (ui8_ind, f_newValue, b_enableReplaceOfCmd);
}
void
vtObject_c::saveValuePSetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, IsoAgLib::iVtObject_c* p_newValue, bool b_enableReplaceOfCmd) {
  if (ui16_structOffset != 0) saveValueP (ui16_structOffset, ui16_structLen, p_newValue);
  setAttribute (ui8_ind, (p_newValue == NULL) ? 65535 : p_newValue->getID(), b_enableReplaceOfCmd);
}

bool
vtObject_c::genericChangeChildLocationPosition (bool rb_isLocation, IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen)
{ // ~X2C
  // Find the child object in question
  for(int8_t i = 0; i < numObjectsToFollow; i++) {
    if (childObject->getID() == objectsToFollow[i].vtObject->getID()) {
      if (b_updateObject) {
        // Check if RAM version of objectsToFollow already exists?
        /**** he following could be called "createObjectsToFollowRamStructIfNotYet" ****/
        if (!(flags & FLAG_OBJECTS2FOLLOW_IN_RAM)) {
          // Copy objectsToFollow structure! and use new pointer afterwards!!
          void* romObjectsToFollow = objectsToFollow;
          objectsToFollow = new (IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s [numObjectsToFollow]);
          CNAMESPACE::memcpy (objectsToFollow, romObjectsToFollow, sizeof (IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s) * numObjectsToFollow);
          // saveValue will check itself if general structure is already in RAM and can be altered
          saveValueP(ui16_structOffset, ui16_structLen, (IsoAgLib::iVtObject_c *)objectsToFollow);
          flags |= FLAG_OBJECTS2FOLLOW_IN_RAM;
        }
        if (rb_isLocation) {
          objectsToFollow[i].x = objectsToFollow[i].x + dx;
          objectsToFollow[i].y = objectsToFollow[i].y + dy;
        } else {
          objectsToFollow[i].x = dx;
          objectsToFollow[i].y = dy;
        }
      }
      return true; // Object was child object, so its position could be changed --> return TRUE!
    }
  }
  return false; // Object was not child object --> return FALSE!
} // -X2C

// only using int16_t because ISO's offset is -127 and hence the range is -127..0..+128 :-(((
bool
vtObject_c::genericChangeChildLocation (IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_enableReplaceOfCmd)
{
  if (dx > 128) dx = 128;  /** @todo throw a warning here?! log to Err_c ?! */
  if (dy < -127)dy = -127; /** @todo throw a warning here?! log to Err_c ?! */

  bool b_result = genericChangeChildLocationPosition (true, childObject, dx, dy, b_updateObject, numObjectsToFollow, objectsToFollow, ui16_structOffset, ui16_structLen);
  if (b_result) __IsoAgLib::getIsoTerminalInstance4Comm().sendCommandChangeChildLocation (this, childObject, dx, dy, b_enableReplaceOfCmd);
  return b_result;
}

bool
vtObject_c::genericChangeChildPosition (IsoAgLib::iVtObject_c* childObject, int16_t x, int16_t y, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_enableReplaceOfCmd)
{
  bool b_result = genericChangeChildLocationPosition (false, childObject, x, y, b_updateObject, numObjectsToFollow, objectsToFollow, ui16_structOffset, ui16_structLen);
  if (b_result) __IsoAgLib::getIsoTerminalInstance4Comm().sendCommandChangeChildPosition (this, childObject, x, y, b_enableReplaceOfCmd);
  return b_result;
}

// //////////////////////////////// +X2C Operation 216 : disable
//! Parameter:
//! @param b_updateObject:
bool
vtObject_c::able (uint8_t enOrDis, bool b_updateObject, bool b_enableReplaceOfCmd)
{ // ~X2C
  if (b_updateObject) {
    updateEnable (enOrDis);
  }
  return __IsoAgLib::getIsoTerminalInstance4Comm().sendCommand (0xA1 /* Command: Command --- Parameter: Enable/Disable Object */,
                                                          vtObject_a->ID & 0xFF, vtObject_a->ID >> 8,
                                                          enOrDis,
                                                          0xFF, 0xFF, 0xFF, 0xFF, 1000, b_enableReplaceOfCmd);
} // -X2C

// //////////////////////////////// +X2C Operation 218 : select
bool
vtObject_c::select()
{ // ~X2C
  return __IsoAgLib::getIsoTerminalInstance4Comm().sendCommand (0xA2 /* Command: Command --- Parameter: Select Input Object */,
                                                          vtObject_a->ID & 0xFF, vtObject_a->ID >> 8,
                                                          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 1000, true);
} // -X2C

} // end of namespace __IsoAgLib
