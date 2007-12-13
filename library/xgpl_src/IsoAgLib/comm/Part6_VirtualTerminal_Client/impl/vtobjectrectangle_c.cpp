/***************************************************************************
                            vtobjectrectangle_c.cpp
                             -------------------
    begin                : Don Sep 4 2003
    copyright            : (C) 2003-2004 by Martin Wodok / Bradford Cox
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
#include "vtobjectrectangle_c.h"

#if not defined PRJ_ISO_TERMINAL_OBJECT_SELECTION1 || defined USE_VTOBJECT_rectangle
#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectfillattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
// Operation : stream
//! @param destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectRectangle_c::stream(uint8_t* destMemory,
                            uint16_t maxBytes,
                            objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectRectangle_a
#define MACRO_vtObjectTypeS iVtObjectRectangle_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 14; // Object Type = Rectangle
      destMemory [3] = vtObjectRectangle_a->lineAttributes->getID() & 0xFF;
      destMemory [4] = vtObjectRectangle_a->lineAttributes->getID() >> 8;
      if ((s_properties.flags & FLAG_ORIGIN_SKM) || p_parentButtonObject) {
        destMemory [5] = (((uint32_t) vtObjectRectangle_a->width*factorM)/factorD) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectRectangle_a->width*factorM)/factorD) >> 8;
        destMemory [7] = (((uint32_t) vtObjectRectangle_a->height*factorM)/factorD) & 0xFF;
        destMemory [8] = (((uint32_t) vtObjectRectangle_a->height*factorM)/factorD) >> 8;
      } else {
        destMemory [5] = (((uint32_t) vtObjectRectangle_a->width*vtDimension)/opDimension) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectRectangle_a->width*vtDimension)/opDimension) >> 8;
        destMemory [7] = (((uint32_t) vtObjectRectangle_a->height*vtDimension)/opDimension) & 0xFF;
        destMemory [8] = (((uint32_t) vtObjectRectangle_a->height*vtDimension)/opDimension) >> 8;

    }
      destMemory [9] = vtObjectRectangle_a->lineSuppression;

    if (vtObjectRectangle_a->fillAttributes != NULL)
    {
        destMemory [10] = vtObjectRectangle_a->fillAttributes->getID() & 0xFF;
        destMemory [11] = vtObjectRectangle_a->fillAttributes->getID() >> 8;
      } else {
        destMemory [10] = 0xFF;
        destMemory [11] = 0xFF;
      }

      destMemory [12] = vtObjectRectangle_a->numberOfMacrosToFollow;
      sourceOffset += 13;
      curBytes += 13;
    }

    MACRO_streamEventMacro(13);
    return curBytes;
}

// Operation : vtObjectRectangle_c
vtObjectRectangle_c::vtObjectRectangle_c() {}

// Operation : fitTerminal
uint32_t
vtObjectRectangle_c::fitTerminal() const
{
  MACRO_localVars;
  return 13+vtObjectRectangle_a->numberOfMacrosToFollow*2;
}


// Operation : setOriginSKM
void
vtObjectRectangle_c::setOriginSKM(bool b_SKM)
{
  MACRO_localVars;
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    vtObjectRectangle_a->lineAttributes->setOriginSKM (b_SKM);
  }
}

void
vtObjectRectangle_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectRectangle_a(), width),  sizeof(iVtObjectRectangle_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectRectangle_a(), height), sizeof(iVtObjectRectangle_s), newHeight);
  }
  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectRectangle_c::updateLineAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineAttributes), sizeof(iVtObjectRectangle_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineAttributes), sizeof(iVtObjectRectangle_s));
}

uint16_t
vtObjectRectangle_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectRectangle_a(), width), sizeof(iVtObjectRectangle_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), width), sizeof(iVtObjectRectangle_s));
}

uint16_t
vtObjectRectangle_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectRectangle_a(), height), sizeof(iVtObjectRectangle_s), 3);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), height), sizeof(iVtObjectRectangle_s));
}

uint8_t
vtObjectRectangle_c::updateLineSuppression(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineSuppression), sizeof(iVtObjectRectangle_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineSuppression), sizeof(iVtObjectRectangle_s));
}

uint16_t
vtObjectRectangle_c::updateFillAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectRectangle_a(), fillAttributes), sizeof(iVtObjectRectangle_s), 5);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), fillAttributes), sizeof(iVtObjectRectangle_s));
}

void
vtObjectRectangle_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineAttributes), sizeof(iVtObjectRectangle_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), width), sizeof(iVtObjectRectangle_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), height), sizeof(iVtObjectRectangle_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectRectangle_a(), lineSuppression), sizeof(iVtObjectRectangle_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 5: saveValue16(MACRO_getStructOffset(get_vtObjectRectangle_a(), fillAttributes), sizeof(iVtObjectRectangle_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    default: break;
  }
}
#endif

} // end namespace __IsoAgLib
#endif
