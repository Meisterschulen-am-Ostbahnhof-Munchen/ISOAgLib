/***************************************************************************
                          vtobjectlinearbargraph_c.cpp
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

#include "vtobjectlinearbargraph_c.h"
#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


// //////////////////////////////// +X2C Operation 120 : stream
//! Parameter:
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectLinearBarGraph_c::stream(uint8_t* destMemory,
                                 uint16_t maxBytes,
                                 uint16_t sourceOffset)
{ // ~X2C
#define MACRO_vtObjectTypeA vtObjectLinearBarGraph_a
#define MACRO_vtObjectTypeS iVtObjectLinearBarGraph_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 18; // Object Type = Linear Bar Graph
      if (flags & FLAG_ORIGIN_SKM) {
        destMemory [3] = (((uint32_t) vtObjectLinearBarGraph_a->width * factor) >> 20) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectLinearBarGraph_a->width * factor) >> 20) >> 8;
        destMemory [5] = (((uint32_t) vtObjectLinearBarGraph_a->height * factor) >> 20) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectLinearBarGraph_a->height * factor) >> 20) >> 8;
      } else {
        destMemory [3] = (((uint32_t) vtObjectLinearBarGraph_a->width*vtDimension)/opDimension) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectLinearBarGraph_a->width*vtDimension)/opDimension) >> 8;
        destMemory [5] = (((uint32_t) vtObjectLinearBarGraph_a->height*vtDimension)/opDimension) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectLinearBarGraph_a->height*vtDimension)/opDimension) >> 8;
      }
      destMemory [7] = vtObjectLinearBarGraph_a->colour;
      destMemory [8] = vtObjectLinearBarGraph_a->targetLineColour;
      destMemory [9] = vtObjectLinearBarGraph_a->options;
      destMemory [10] = vtObjectLinearBarGraph_a->numberOfTicks;
      destMemory [11] = vtObjectLinearBarGraph_a->minValue & 0xFF;
      destMemory [12] = vtObjectLinearBarGraph_a->minValue >> 8;
      destMemory [13] = vtObjectLinearBarGraph_a->maxValue & 0xFF;
      destMemory [14] = vtObjectLinearBarGraph_a->maxValue >> 8;
      if (vtObjectLinearBarGraph_a->variableReference != NULL) {
        destMemory [15] = vtObjectLinearBarGraph_a->variableReference->getID() & 0xFF;
        destMemory [16] = vtObjectLinearBarGraph_a->variableReference->getID() >> 8;
      } else {
        destMemory [15] = 0xFF;
        destMemory [16] = 0xFF;
      }
      destMemory [17] = vtObjectLinearBarGraph_a->value & 0xFF;
      destMemory [18] = vtObjectLinearBarGraph_a->value >> 8;
      if (vtObjectLinearBarGraph_a->targetValueVariableReference != NULL) {
        destMemory [19] = vtObjectLinearBarGraph_a->targetValueVariableReference->getID() & 0xFF;
        destMemory [20] = vtObjectLinearBarGraph_a->targetValueVariableReference->getID() >> 8;
      } else {
        destMemory [19] = 0xFF;
        destMemory [20] = 0xFF;
      }
      destMemory [21] = vtObjectLinearBarGraph_a->targetValue & 0xFF;
      destMemory [22] = vtObjectLinearBarGraph_a->targetValue >> 8;
      destMemory [23] = vtObjectLinearBarGraph_a->numberOfMacrosToFollow;
      sourceOffset += 24;
      curBytes += 24;
    }

    MACRO_streamEventMacro(24);
    return curBytes;
} // -X2C


// //////////////////////////////// +X2C Operation 123 : vtObjectLinearBarGraph_c
vtObjectLinearBarGraph_c::vtObjectLinearBarGraph_c()
{ // ~X2C
} // -X2C

// //////////////////////////////// +X2C Operation 201 : size
uint32_t
vtObjectLinearBarGraph_c::fitTerminal()
{ // ~X2C
  MACRO_localVars;
  return 24+vtObjectLinearBarGraph_a->numberOfMacrosToFollow*2;
} // -X2C

// //////////////////////////////// +X2C Operation 250 : setValue
//! Parameter:
//! @param newValue:
//! @param b_updateObject:
void
vtObjectLinearBarGraph_c::setValue(uint16_t newValue, bool b_updateObject)
{ // ~X2C
  if (get_vtObjectLinearBarGraph_a()->variableReference == NULL) {
    if (b_updateObject) saveValue16 (MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), value), sizeof(iVtObjectLinearBarGraph_s), newValue);

    __IsoAgLib::getIsoTerminalInstance().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, 0x00, 0x00);
  }
} // -X2C

void
vtObjectLinearBarGraph_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), width),  sizeof(iVtObjectLinearBarGraph_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), height), sizeof(iVtObjectLinearBarGraph_s), newHeight);
  }
  __IsoAgLib::getIsoTerminalInstance().sendCommandChangeSize (this, newWidth, newHeight);
}

} // end of namespace __IsoAgLib
