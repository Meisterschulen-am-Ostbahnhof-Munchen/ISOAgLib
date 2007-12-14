/***************************************************************************
                          vtobjectobjectpointer_c.cpp
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
#include "vtobjectobjectpointer_c.h"

#ifdef USE_VTOBJECT_objectpointer
#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// Operation : stream
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectObjectPointer_c::stream(uint8_t* destMemory,
                                uint16_t /*maxBytes*/,
                                objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectObjectPointer_a
#define MACRO_vtObjectTypeS iVtObjectObjectPointer_s
    MACRO_localVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 27; // Object Type = Object Pointer
      if (vtObjectObjectPointer_a->value != NULL) {
        destMemory [3] = vtObjectObjectPointer_a->value->getID() & 0xFF;
        destMemory [4] = vtObjectObjectPointer_a->value->getID() >> 8;
      } else {
        destMemory [3] = 0xFF;
        destMemory [4] = 0xFF;
      }
      return 5;
    }
    return 0;
}


// Operation : vtObjectObjectPointer_c
vtObjectObjectPointer_c::vtObjectObjectPointer_c() {}

// Operation : size
uint32_t
vtObjectObjectPointer_c::fitTerminal() const
{
  return 5;
}

// Operation : setValue
//! @param apc_newObject:
//! @param b_updateObject:
void
vtObjectObjectPointer_c::setValue(IsoAgLib::iVtObject_c* apc_newObject, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) saveValueP (MACRO_getStructOffset(get_vtObjectObjectPointer_a(), value),  sizeof(iVtObjectObjectPointer_s), apc_newObject);

  if (apc_newObject != NULL) __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeNumericValue (this, apc_newObject->getID() & 0xFF, (apc_newObject->getID() >> 8) & 0xFF, 0x00, 0x00, b_enableReplaceOfCmd);
  else                       __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeNumericValue (this, 0xFF, 0xFF, 0x00, 0x00, b_enableReplaceOfCmd);
}

void
vtObjectObjectPointer_c::setOriginSKM(bool b_SKM)
{
  MACRO_localVars;
  if (vtObjectObjectPointer_a->value != NULL) {
    vtObjectObjectPointer_a->value->setOriginSKM (b_SKM);
  }
}

void
vtObjectObjectPointer_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn)
{
  MACRO_localVars;
  if (p_btn) p_parentButtonObject = p_btn;
  if (vtObjectObjectPointer_a->value != NULL) {
    vtObjectObjectPointer_a->value->setOriginBTN (p_btn);
  }
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
/** that attribute is in parentheses in the spec, so commented out here
uint16_t
vtObjectObjectPointer_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectObjectPointer_a(), value), sizeof(iVtObjectObjectPointer_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectObjectPointer_a(), value), sizeof(iVtObjectObjectPointer_s));
}
*/

void
vtObjectObjectPointer_c::saveReceivedAttribute(uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/)
{
  /** that attribute is in parentheses in the spec, so commented out here
  case 1: saveValue16(MACRO_getStructOffset(get_vtObjectObjectPointer_a(), value), sizeof(iVtObjectObjectPointer_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
  default: break;
  */
}
#endif
} // end of namespace __IsoAgLib
#endif
