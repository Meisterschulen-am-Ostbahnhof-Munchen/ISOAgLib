/***************************************************************************
                          vtobjectcontainer_c.cpp
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
#include "vtobjectcontainer_c.h"
#include "../ivtobjectcontainer_c.h"

#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "isoterminal_c.h"

namespace IsoAgLib {
  // implement here a normal constructor/destructor, as the compiler dislikes inlining of that simple
  // constructor/destructor direct in scope of iVtObjectContainer_c
  iVtObjectContainer_c::iVtObjectContainer_c() {}
  iVtObjectContainer_c::~iVtObjectContainer_c() {}
};

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
// Operation : stream
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectContainer_c::stream(uint8_t* destMemory,
                            uint16_t maxBytes,
                            objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectContainer_a
#define MACRO_vtObjectTypeS iVtObjectContainer_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 3; // Object Type = Container
      if ((s_properties.flags & FLAG_ORIGIN_SKM) || p_parentButtonObject) {
        destMemory [3] = (((uint32_t) vtObjectContainer_a->width*factorM)/factorD) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectContainer_a->width*factorM)/factorD) >> 8;
        destMemory [5] = (((uint32_t) vtObjectContainer_a->height*factorM)/factorD) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectContainer_a->height*factorM)/factorD) >> 8;
      } else {
        destMemory [3] = (((uint32_t) vtObjectContainer_a->width*vtDimension)/opDimension) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectContainer_a->width*vtDimension)/opDimension) >> 8;
        destMemory [5] = (((uint32_t) vtObjectContainer_a->height*vtDimension)/opDimension) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectContainer_a->height*vtDimension)/opDimension) >> 8;
      }
      destMemory [7] = vtObjectContainer_a->hidden;
      destMemory [8] = vtObjectContainer_a->numberOfObjectsToFollow;
      destMemory [9] = vtObjectContainer_a->numberOfMacrosToFollow;

      sourceOffset += 10;
      curBytes += 10;
    }

    MACRO_streamObjectXY(10);
    MACRO_streamEventMacro(10U+vtObjectContainer_a->numberOfObjectsToFollow*6U);
    return curBytes;
}

// Operation : vtObjectContainer_c
vtObjectContainer_c::vtObjectContainer_c() {}

// Operation : size
uint32_t
vtObjectContainer_c::fitTerminal() const
{
  MACRO_localVars;
  return 10+vtObjectContainer_a->numberOfObjectsToFollow*6+vtObjectContainer_a->numberOfMacrosToFollow*2;
}


// Operation : hideShow
//! @param b_hideOrShow:
//! @param b_updateObject:
void
vtObjectContainer_c::hideShow(uint8_t b_hideOrShow, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) saveValue8 (MACRO_getStructOffset(get_vtObjectContainer_a(), hidden), sizeof(iVtObjectContainer_s), (!b_hideOrShow)&0x01);

   __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandHideShow (this, b_hideOrShow,b_enableReplaceOfCmd);
}

void
vtObjectContainer_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectContainer_a(), width),  sizeof(iVtObjectContainer_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectContainer_a(), height), sizeof(iVtObjectContainer_s), newHeight);
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

bool
vtObjectContainer_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{

  MACRO_localVars;
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObjectContainer_a->numberOfObjectsToFollow, (IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *) vtObjectContainer_a->objectsToFollow, MACRO_getStructOffset(get_vtObjectContainer_a(), objectsToFollow), sizeof(iVtObjectContainer_s), b_enableReplaceOfCmd);
}

bool
vtObjectContainer_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObjectContainer_a->numberOfObjectsToFollow, (IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *) vtObjectContainer_a->objectsToFollow, MACRO_getStructOffset(get_vtObjectContainer_a(), objectsToFollow), sizeof(iVtObjectContainer_s), b_enableReplaceOfCmd);
}

void
vtObjectContainer_c::setOriginSKM(bool b_SKM)
{
  MACRO_localVars;
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    for (int i=0; i<vtObjectContainer_a->numberOfObjectsToFollow; i++) {
      vtObjectContainer_a->objectsToFollow[i].vtObject->setOriginSKM (b_SKM);
    }
  }
}

void
vtObjectContainer_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn)
{
  MACRO_localVars;
  if (p_btn) p_parentButtonObject = p_btn;
  for (int i=0; i<vtObjectContainer_a->numberOfObjectsToFollow; i++) {
    vtObjectContainer_a->objectsToFollow[i].vtObject->setOriginBTN (p_btn);
  }
}

/** these attributes are in parentheses in the spec, so commented out here
uint16_t
vtObjectContainer_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectContainer_a(), width), sizeof(iVtObjectContainer_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectContainer_a(), width), sizeof(iVtObjectContainer_s));
}

uint16_t
vtObjectContainer_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectContainer_a(), height), sizeof(iVtObjectContainer_s), 2);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectContainer_a(), height), sizeof(iVtObjectContainer_s));
}

uint8_t
vtObjectContainer_c::updateHidden(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectContainer_a(), hidden), sizeof(iVtObjectContainer_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectContainer_a(), hidden), sizeof(iVtObjectContainer_s));
}
*/

void
vtObjectContainer_c::saveReceivedAttribute(uint8_t attrID, uint8_t* /*pui8_attributeValue*/)
{
  switch (attrID)
  {
    /** these attributes are in parentheses in the spec, so commented out here
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectContainer_a(), width), sizeof(iVtObjectContainer_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectContainer_a(), height), sizeof(iVtObjectContainer_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectContainer_a(), hidden), sizeof(iVtObjectContainer_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
} // end of namespace __IsoAgLib
