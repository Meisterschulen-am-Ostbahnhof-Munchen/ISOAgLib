/***************************************************************************
                          vtobjectoutputnumber_c.cpp
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

#include "vtobjectoutputnumber_c.h"
#include <IsoAgLib/util/impl/util_funcs.h>
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


// //////////////////////////////// +X2C Operation 125 : stream
//! Parameter:
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectOutputNumber_c::stream(uint8_t* destMemory,
                               uint16_t maxBytes,
                               objRange_t sourceOffset)
{ // ~X2C
#define MACRO_vtObjectTypeA vtObjectOutputNumber_a
#define MACRO_vtObjectTypeS iVtObjectOutputNumber_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 12; // Object Type = Output Number
      if ((flags & FLAG_ORIGIN_SKM) || p_parentButtonObject) {
        destMemory [3] = (((uint32_t) vtObjectOutputNumber_a->width*factorM)/factorD) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectOutputNumber_a->width*factorM)/factorD) >> 8;
        destMemory [5] = (((uint32_t) vtObjectOutputNumber_a->height*factorM)/factorD) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectOutputNumber_a->height*factorM)/factorD) >> 8;
      } else {
        destMemory [3] = (((uint32_t) vtObjectOutputNumber_a->width*vtDimension)/opDimension) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectOutputNumber_a->width*vtDimension)/opDimension) >> 8;
        destMemory [5] = (((uint32_t) vtObjectOutputNumber_a->height*vtDimension)/opDimension) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectOutputNumber_a->height*vtDimension)/opDimension) >> 8;
      }
      destMemory [7] = __IsoAgLib::getIsoTerminalInstance4Comm().getUserClippedColor (vtObjectOutputNumber_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [8] = vtObjectOutputNumber_a->fontAttributes->getID() & 0xFF;
      destMemory [9] = vtObjectOutputNumber_a->fontAttributes->getID() >> 8;
      destMemory [10] = vtObjectOutputNumber_a->options;
      if (vtObjectOutputNumber_a->variableReference != NULL) {
        destMemory [11] = vtObjectOutputNumber_a->variableReference->getID() & 0xFF;
        destMemory [12] = vtObjectOutputNumber_a->variableReference->getID() >> 8;
      } else {
        destMemory [11] = 0xFF;
        destMemory [12] = 0xFF;
      }
      destMemory [13] = (vtObjectOutputNumber_a->value) & 0xFF;
      destMemory [14] = (vtObjectOutputNumber_a->value >> 8) & 0xFF;
      destMemory [15] = (vtObjectOutputNumber_a->value >> 16) & 0xFF;
      destMemory [16] = (vtObjectOutputNumber_a->value >> 24) & 0xFF;

      destMemory [17] = (vtObjectOutputNumber_a->offset) & 0xFF;
      destMemory [18] = (vtObjectOutputNumber_a->offset >> 8) & 0xFF;
      destMemory [19] = (vtObjectOutputNumber_a->offset >> 16) & 0xFF;
      destMemory [20] = (vtObjectOutputNumber_a->offset >> 24) & 0xFF;

      __IsoAgLib::float2Int (&vtObjectOutputNumber_a->scale, &destMemory[21]);

      destMemory [25] = vtObjectOutputNumber_a->numberOfDecimals;
      destMemory [26] = vtObjectOutputNumber_a->format;
      destMemory [27] = vtObjectOutputNumber_a->horizontalJustification;
      destMemory [28] = vtObjectOutputNumber_a->numberOfMacrosToFollow;
      sourceOffset += 29;
      curBytes += 29;
    }

    MACRO_streamEventMacro(29);
    return curBytes;
} // -X2C


// //////////////////////////////// +X2C Operation 128 : vtObjectOutputNumber_c
vtObjectOutputNumber_c::vtObjectOutputNumber_c()
{ // ~X2C
} // -X2C

// //////////////////////////////// +X2C Operation 199 : size
uint32_t
vtObjectOutputNumber_c::fitTerminal()
{ // ~X2C
  MACRO_localVars;
  return 29+vtObjectOutputNumber_a->numberOfMacrosToFollow*2;
} // -X2C



// //////////////////////////////// +X2C Operation 208 : setValue
//! Parameter:
//! @param newValue:
//! @param b_updateObject:
void
vtObjectOutputNumber_c::setValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{ // ~X2C
  if (get_vtObjectOutputNumber_a()->variableReference == NULL) {
    // Save Value if requested
    if (b_updateObject) saveValue32 (MACRO_getStructOffset(get_vtObjectOutputNumber_a(), value), sizeof(iVtObjectOutputNumber_s), newValue);

    // Send Value update
    __IsoAgLib::getIsoTerminalInstance4Comm().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, (newValue >> 16) & 0xFF, newValue >> 24, b_enableReplaceOfCmd);
  }
} // -X2C

// //////////////////////////////// +X2C Operation 234 : setOriginSKM
//! Parameter:
//! @param b_SKM:
void
vtObjectOutputNumber_c::setOriginSKM(bool b_SKM)
{ // ~X2C
  MACRO_localVars;
  if (b_SKM) {
    flags |= FLAG_ORIGIN_SKM;
    vtObjectOutputNumber_a->fontAttributes->setOriginSKM (b_SKM);
  }
} // -X2C

//! Parameter:
//! @param p_btn:
void
vtObjectOutputNumber_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn)
{
  MACRO_localVars;
  if (p_btn)
  {
    p_parentButtonObject = p_btn;
    vtObjectOutputNumber_a->fontAttributes->setOriginBTN (p_btn);
  }
}

void
vtObjectOutputNumber_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectOutputNumber_a(), width),  sizeof(iVtObjectOutputNumber_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectOutputNumber_a(), height), sizeof(iVtObjectOutputNumber_s), newHeight);
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

} // end of namespace __IsoAgLib
