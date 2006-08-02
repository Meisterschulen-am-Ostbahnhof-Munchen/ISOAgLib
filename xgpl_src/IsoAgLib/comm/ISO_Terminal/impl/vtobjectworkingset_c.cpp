/***************************************************************************
                          vtobjectworkingset_c.cpp
                             -------------------
    begin                : Mon Jun 30 14:41:54 2003
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

#include "vtobjectworkingset_c.h"
#include "isoterminal_c.h"
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


// //////////////////////////////// +X2C Operation 67 : stream
//! Parameter:
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectWorkingSet_c::stream(uint8_t* destMemory,
                             uint16_t maxBytes,
                             objRange_t sourceOffset)
{ // ~X2C
#define MACRO_vtObjectTypeA vtObjectWorkingSet_a
#define MACRO_vtObjectTypeS iVtObjectWorkingSet_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
        destMemory [0] = vtObjectWorkingSet_a->ID & 0xFF;
        destMemory [1] = vtObjectWorkingSet_a->ID >> 8;
        destMemory [2] = 0; // Object Type = Working Set
        destMemory [3] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (vtObjectWorkingSet_a->backgroundColour, this, IsoAgLib::BackgroundColour);
        destMemory [4] = vtObjectWorkingSet_a->selectable;
        if (vtObjectWorkingSet_a->activeMask != NULL) {
            destMemory [5] = vtObjectWorkingSet_a->activeMask->getID() & 0xFF;
            destMemory [6] = vtObjectWorkingSet_a->activeMask->getID() >> 8;
        } else {
            destMemory [5] = 0; // using 0x00 here as 0xFFFF is NOT allowed
            destMemory [6] = 0; // using 0x00 here as 0xFFFF is NOT allowed
        }
        destMemory [7] = vtObjectWorkingSet_a->numberOfObjectsToFollow;
        destMemory [8] = vtObjectWorkingSet_a->numberOfMacrosToFollow;
        destMemory [9] = vtObjectWorkingSet_a->numberOfLanguagesToFollow;

        sourceOffset += 10;
        curBytes += 10;
    }

    MACRO_streamObjectXYcenteredInSoftKey(10);
    uint16_t tempOffset = 10+vtObjectWorkingSet_a->numberOfObjectsToFollow*6;
    MACRO_streamEventMacro(tempOffset);
    tempOffset = tempOffset + vtObjectWorkingSet_a->numberOfMacrosToFollow*2;
    MACRO_streamLanguages(tempOffset);

    return curBytes;
} // -X2C

// //////////////////////////////// +X2C Operation 70 : vtObjectWorkingSet_c
vtObjectWorkingSet_c::vtObjectWorkingSet_c()
{ // ~X2C
} // -X2C

// //////////////////////////////// +X2C Operation 188 : size
uint32_t
vtObjectWorkingSet_c::fitTerminal()
{ // ~X2C
  MACRO_localVars;
  return 10+vtObjectWorkingSet_a->numberOfObjectsToFollow*6+vtObjectWorkingSet_a->numberOfMacrosToFollow*2+vtObjectWorkingSet_a->numberOfLanguagesToFollow*2;
} // -X2C

// //////////////////////////////// +X2C Operation 224 : changeActiveMask
//! Parameter:
//! @param rpc_vtObjectMask:
//! @param b_updateObject:
void
vtObjectWorkingSet_c::changeActiveMask(IsoAgLib::iVtObjectMask_c* rpc_vtObjectMask, bool b_updateObject, bool b_enableReplaceOfCmd)
{ // ~X2C
  if (b_updateObject) saveValueP (MACRO_getStructOffset(get_vtObjectWorkingSet_a(), activeMask), sizeof(iVtObjectWorkingSet_s), rpc_vtObjectMask);

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommand (173 /* Command: Command --- Parameter: Change Active Mask */,
                                                   vtObject_a->ID & 0xFF, vtObject_a->ID >> 8,
                                                   rpc_vtObjectMask->getID() & 0xFF, rpc_vtObjectMask->getID() >> 8,
                                                   0xFF, 0xFF, 0xFF, 2000, b_enableReplaceOfCmd);
} // -X2C

void
vtObjectWorkingSet_c::changeBackgroundColour(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{ // ~X2C
  if (b_updateObject) saveValue8 (MACRO_getStructOffset(get_vtObjectWorkingSet_a(), backgroundColour), sizeof(iVtObjectWorkingSet_s), newValue);

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeBackgroundColour (this, newValue, b_enableReplaceOfCmd);
} // -X2C

bool
vtObjectWorkingSet_c::moveChildLocation(IsoAgLib::iVtObject_c* rpc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  return genericChangeChildLocation (rpc_childObject, dx, dy, b_updateObject, vtObjectWorkingSet_a->numberOfObjectsToFollow, (IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *) vtObjectWorkingSet_a->objectsToFollow, MACRO_getStructOffset(get_vtObjectWorkingSet_a(), objectsToFollow), sizeof(iVtObjectWorkingSet_s), b_enableReplaceOfCmd);
}

bool
vtObjectWorkingSet_c::setChildPosition(IsoAgLib::iVtObject_c* rpc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  return genericChangeChildPosition (rpc_childObject, x, y, b_updateObject, vtObjectWorkingSet_a->numberOfObjectsToFollow, (IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *) vtObjectWorkingSet_a->objectsToFollow, MACRO_getStructOffset(get_vtObjectWorkingSet_a(), objectsToFollow), sizeof(iVtObjectWorkingSet_s), b_enableReplaceOfCmd);
}

bool
vtObjectWorkingSet_c::controlAudioDevice (uint8_t rui8_repetitions, uint16_t rui16_frequency, uint16_t rui16_onTime, uint16_t rui16_offTime)
{
  return __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandControlAudioDevice (rui8_repetitions, rui16_frequency, rui16_onTime, rui16_offTime);
}

bool
vtObjectWorkingSet_c::setAudioVolume (uint8_t rui8_volume)
{
  return __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandSetAudioVolume (rui8_volume);
}


// //////////////////////////////// +X2C Operation 235 : setOriginSKM
//! Parameter:
//! @param b_SKM:
void
vtObjectWorkingSet_c::setOriginSKM(bool /*b_SKM*/)
{ // ~X2C
  MACRO_localVars;
  s_properties.flags |= FLAG_ORIGIN_SKM; // WS Descriptor has to fit inside a SoftKey!
  for (int i=0; i<vtObjectWorkingSet_a->numberOfObjectsToFollow; i++) {
    vtObjectWorkingSet_a->objectsToFollow[i].vtObject->setOriginSKM (true);
  }
} // -X2C

} // end of namespace __IsoAgLib
