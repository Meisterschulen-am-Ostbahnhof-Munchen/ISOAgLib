/***************************************************************************
                          vtobjectbutton_c.cpp
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
#include "vtobjectbutton_c.h"

#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "isoterminal_c.h"

namespace IsoAgLib {
  // implement here a normal functions, as the compiler dislikes inlining of that simple functions
  // direct in scope of iVtObjectButton_s
  const iVtObjectButton_c::iVtObjectButton_s& iVtObjectButton_c::get_vtObjectButton_a()
  {
    return *vtObjectButton_c::get_vtObjectButton_a();
  }

}


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
// Operation : stream
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectButton_c::stream(uint8_t* destMemory,
                         uint16_t maxBytes,
                         objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectButton_a
#define MACRO_vtObjectTypeS iVtObjectButton_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 6; // Object Type = Button
      destMemory [3] = (((uint32_t) vtObjectButton_a->width*vtDimension)/opDimension) & 0xFF;
      destMemory [4] = (((uint32_t) vtObjectButton_a->width*vtDimension)/opDimension) >> 8;
      destMemory [5] = (((uint32_t) vtObjectButton_a->height*vtDimension)/opDimension) & 0xFF;
      destMemory [6] = (((uint32_t) vtObjectButton_a->height*vtDimension)/opDimension) >> 8;
      destMemory [7] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (vtObjectButton_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [8] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (vtObjectButton_a->borderColour, this, IsoAgLib::BorderColour);
      destMemory [9] = vtObjectButton_a->keyCode;
      destMemory [10] = vtObjectButton_a->options;
      destMemory [11] = vtObjectButton_a->numberOfObjectsToFollow;
      destMemory [12] = vtObjectButton_a->numberOfMacrosToFollow;
      sourceOffset += 13;
      curBytes += 13;
    }

    MACRO_streamObjectXYcenteredInButton(13);
    MACRO_streamEventMacro(13U+vtObjectButton_a->numberOfObjectsToFollow*6U);
    return curBytes;
}


// Operation : vtObjectButton_c
vtObjectButton_c::vtObjectButton_c() {}

// Operation : size
uint32_t
vtObjectButton_c::fitTerminal() const
{
  MACRO_localVars;
  return 13+vtObjectButton_a->numberOfObjectsToFollow*6+vtObjectButton_a->numberOfMacrosToFollow*2;
}

void
vtObjectButton_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectButton_a(), width),  sizeof(iVtObjectButton_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectButton_a(), height), sizeof(iVtObjectButton_s), newHeight);
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeSize(this, newWidth, newHeight, b_enableReplaceOfCmd);
}

bool
vtObjectButton_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObjectButton_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObjectButton_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectButton_a(), objectsToFollow), sizeof(iVtObjectButton_s), b_enableReplaceOfCmd);
}

bool
vtObjectButton_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObjectButton_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObjectButton_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectButton_a(), objectsToFollow), sizeof(iVtObjectButton_s), b_enableReplaceOfCmd);
}

void
vtObjectButton_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* /*p_btn*/)
{
  MACRO_localVars;
  p_parentButtonObject = (IsoAgLib::iVtObjectButton_c*)this;
  for (int i=0; i<vtObjectButton_a->numberOfObjectsToFollow; i++) {
    vtObjectButton_a->objectsToFollow[i].vtObject->setOriginBTN (p_parentButtonObject);
  }
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectButton_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), width), sizeof(iVtObjectButton_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectButton_a(), width), sizeof(iVtObjectButton_s));
}

uint16_t
vtObjectButton_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), height), sizeof(iVtObjectButton_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectButton_a(), height), sizeof(iVtObjectButton_s));
}

uint8_t
vtObjectButton_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), backgroundColour), sizeof(iVtObjectButton_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectButton_a(), backgroundColour), sizeof(iVtObjectButton_s));
}

uint8_t
vtObjectButton_c::updateBorderColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), borderColour), sizeof(iVtObjectButton_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectButton_a(), borderColour), sizeof(iVtObjectButton_s));
}

uint8_t
vtObjectButton_c::updateKeyCode(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), keyCode), sizeof(iVtObjectButton_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectButton_a(), keyCode), sizeof(iVtObjectButton_s));
}

uint8_t
vtObjectButton_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectButton_a(), options), sizeof(iVtObjectButton_s), 6);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectButton_a(), options), sizeof(iVtObjectButton_s));
}

void
vtObjectButton_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectButton_a(), width), sizeof(iVtObjectButton_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectButton_a(), height), sizeof(iVtObjectButton_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectButton_a(), backgroundColour), sizeof(iVtObjectButton_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectButton_a(), borderColour), sizeof(iVtObjectButton_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectButton_a(), keyCode), sizeof(iVtObjectButton_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue8(MACRO_getStructOffset(get_vtObjectButton_a(), options), sizeof(iVtObjectButton_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    default: break;
  }
}
#endif
} // end of namespace __IsoAgLib
