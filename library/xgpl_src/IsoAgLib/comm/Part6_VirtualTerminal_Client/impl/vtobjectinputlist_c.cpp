/***************************************************************************
                          vtobjectinputlist_c.cpp
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
#include "vtobjectinputlist_c.h"

#if not defined PRJ_ISO_TERMINAL_OBJECT_SELECTION1 || defined USE_VTOBJECT_inputlist
#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
// Operation : stream
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectInputList_c::stream(uint8_t* destMemory,
                            uint16_t maxBytes,
                            objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectInputList_a
#define MACRO_vtObjectTypeS iVtObjectInputList_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 10; // Object Type = Input List
      destMemory [3] = (((uint32_t) vtObjectInputList_a->width*vtDimension)/opDimension) & 0xFF;
      destMemory [4] = (((uint32_t) vtObjectInputList_a->width*vtDimension)/opDimension) >> 8;
      destMemory [5] = (((uint32_t) vtObjectInputList_a->height*vtDimension)/opDimension) & 0xFF;
      destMemory [6] = (((uint32_t) vtObjectInputList_a->height*vtDimension)/opDimension) >> 8;
      if (vtObjectInputList_a->variableReference != NULL) {
        destMemory [7] = vtObjectInputList_a->variableReference->getID() & 0xFF;
        destMemory [8] = vtObjectInputList_a->variableReference->getID() >> 8;
      } else {
        destMemory [7] = 0xFF;
        destMemory [8] = 0xFF;
      }
      destMemory [9] = vtObjectInputList_a->value;

      destMemory [10] = vtObjectInputList_a->numberOfObjectsToFollow;
      destMemory [11] = vtObjectInputList_a->enabled;
      destMemory [12] = vtObjectInputList_a->numberOfMacrosToFollow;
      sourceOffset += 13;
      curBytes += 13;
    }

    MACRO_streamObject(13);
    MACRO_streamEventMacro(13U+vtObjectInputList_a->numberOfObjectsToFollow*2U);
    return curBytes;
}


// Operation : vtObjectInputList_c
vtObjectInputList_c::vtObjectInputList_c() {}

// Operation : getListItem
IsoAgLib::iVtObject_c*
vtObjectInputList_c::getListItem(uint8_t xth)
{
  return ((iVtObjectInputList_s *) vtObject_a)->objectsToFollow[xth].vtObject;
}

// Operation : getNumberOfListItems
uint8_t
vtObjectInputList_c::getNumberOfListItems()
{
    return ((iVtObjectInputList_s *) vtObject_a)->numberOfObjectsToFollow;
}

// Operation : fitTerminal
uint32_t
vtObjectInputList_c::fitTerminal() const
{
  MACRO_localVars;
  return 13+vtObjectInputList_a->numberOfObjectsToFollow*2+vtObjectInputList_a->numberOfMacrosToFollow*2;
}

// Operation : updateEnable
//! @param b_enableOrDisable:
void
vtObjectInputList_c::updateEnable(uint8_t aui8_enOrDis)
{
  saveValue8 (MACRO_getStructOffset(get_vtObjectInputList_a(), enabled), sizeof(iVtObjectInputList_s), aui8_enOrDis);
}

// Operation : setValue
//! @param newValue:
//! @param b_updateObject:
void
vtObjectInputList_c::setValue(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (get_vtObjectInputList_a()->variableReference == NULL) {
    if (b_updateObject) saveValue8 (MACRO_getStructOffset(get_vtObjectInputList_a(), value), sizeof(iVtObjectInputList_s), newValue);

    __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeNumericValue (this, newValue, 0x00, 0x00, 0x00, b_enableReplaceOfCmd);
  }
}

// Operation : setItem
//! @param aui8_index:
//! @param apc_object:
void
vtObjectInputList_c::setItem(uint8_t aui8_index, IsoAgLib::iVtObject_c* apc_object, bool b_enableReplaceOfCmd)
{
  uint8_t hi, lo;
  if (apc_object == NULL) {
    lo = 0xFF;
    hi = 0xFF;
  } else {
    lo = apc_object->getID() & 0xFF;
    hi = apc_object->getID() >> 8;
  }
  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommand (177 /* Command: Command --- Parameter: Change List Item */,
                                                   this->getID() & 0xFF,
                                                   this->getID() >> 8,
                                                   aui8_index,
                                                   lo,
                                                   hi,
                                                   0xFF,
                                                   0xFF,
                                                   DEF_TimeOut_NormalCommand /* timeout value */,
                                                   b_enableReplaceOfCmd
                                                   );
}

void
vtObjectInputList_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectInputList_a(), width),  sizeof(iVtObjectInputList_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectInputList_a(), height), sizeof(iVtObjectInputList_s), newHeight);
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectInputList_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputList_a(), width), sizeof(iVtObjectInputList_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), width), sizeof(iVtObjectInputList_s));
}

uint16_t
vtObjectInputList_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputList_a(), height), sizeof(iVtObjectInputList_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), height), sizeof(iVtObjectInputList_s));
}

uint16_t
vtObjectInputList_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputList_a(), variableReference), sizeof(iVtObjectInputList_s), 3);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), variableReference), sizeof(iVtObjectInputList_s));
}

/** these attributes are in parentheses in the spec, so commented out here
uint8_t
vtObjectInputList_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputList_a(), value), sizeof(iVtObjectInputList_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputList_a(), value), sizeof(iVtObjectInputList_s));
}

uint8_t
vtObjectInputList_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputList_a(), enabled), sizeof(iVtObjectInputList_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputList_a(), enabled), sizeof(iVtObjectInputList_s));
}
*/

void
vtObjectInputList_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), width), sizeof(iVtObjectInputList_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), height), sizeof(iVtObjectInputList_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), variableReference), sizeof(iVtObjectInputList_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    /** these attributes are in parentheses in the spec, so commented out here
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectInputList_a(), value), sizeof(iVtObjectInputList_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectInputList_a(), enabled), sizeof(iVtObjectInputList_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
} // end of namespace __IsoAgLib
#endif
