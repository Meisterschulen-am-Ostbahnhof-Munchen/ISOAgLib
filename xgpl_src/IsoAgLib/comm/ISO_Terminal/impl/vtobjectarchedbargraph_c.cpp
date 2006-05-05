/***************************************************************************
                          vtobjectarchedbargraph_c.cpp
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



#include "vtobjectarchedbargraph_c.h"
#include "isoterminal_c.h"
#include "../ivtobjectbutton_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


// //////////////////////////////// +X2C Operation 120 : stream
//! Parameter:
//! @param destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectArchedBarGraph_c::stream(uint8_t* destMemory,
                                 uint16_t maxBytes,
                                 objRange_t sourceOffset)
{ // ~X2C
#define MACRO_vtObjectTypeA vtObjectArchedBarGraph_a
#define MACRO_vtObjectTypeS iVtObjectArchedBarGraph_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 19; // Object Type = Arched Bar Graph
      if ((flags & FLAG_ORIGIN_SKM) || p_parentButtonObject) {
        destMemory [3] = (((uint32_t) vtObjectArchedBarGraph_a->width*factorM)/factorD) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectArchedBarGraph_a->width*factorM)/factorD) >> 8;
        destMemory [5] = (((uint32_t) vtObjectArchedBarGraph_a->height*factorM)/factorD) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectArchedBarGraph_a->height*factorM)/factorD) >> 8;
      } else {
        destMemory [3] = (((uint32_t) vtObjectArchedBarGraph_a->width*vtDimension)/opDimension) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectArchedBarGraph_a->width*vtDimension)/opDimension) >> 8;
        destMemory [5] = (((uint32_t) vtObjectArchedBarGraph_a->height*vtDimension)/opDimension) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectArchedBarGraph_a->height*vtDimension)/opDimension) >> 8;
      }
      destMemory [7] = __IsoAgLib::getIsoTerminalInstance4Comm().getUserClippedColor (vtObjectArchedBarGraph_a->colour, this, IsoAgLib::Colour);
      destMemory [8] = __IsoAgLib::getIsoTerminalInstance4Comm().getUserClippedColor (vtObjectArchedBarGraph_a->targetLineColour, this, IsoAgLib::TargetLineColour);
      destMemory [9] = vtObjectArchedBarGraph_a->options;
      destMemory [10] = vtObjectArchedBarGraph_a->startAngle;
      destMemory [11] = vtObjectArchedBarGraph_a->endAngle;
      if ((flags & FLAG_ORIGIN_SKM) || p_parentButtonObject)
      {
        destMemory [12] = (((uint32_t) vtObjectArchedBarGraph_a->barGraphWidth*factorM)/factorD) & 0xFF;
        destMemory [13] = (((uint32_t) vtObjectArchedBarGraph_a->barGraphWidth*factorM)/factorD) >> 8;
      }
      else
      {
        destMemory [12] = (((uint32_t) vtObjectArchedBarGraph_a->barGraphWidth*vtDimension)/opDimension) & 0xFF;
        destMemory [13] = (((uint32_t) vtObjectArchedBarGraph_a->barGraphWidth*vtDimension)/opDimension) >> 8;
      }
      if ( (vtObjectArchedBarGraph_a->barGraphWidth > 0) &&
           (destMemory [12] == 0) && (destMemory [13] == 0))
      { // if the arch had a linewidth > 0, then don't allow downscaling to let it become 0!
        destMemory [12] = 0x01;
      //destMemory [13] = 0x00; // is already set to 0 as checked in the if-statement!
      }
      destMemory [14] = vtObjectArchedBarGraph_a->minValue & 0xFF;
      destMemory [15] = vtObjectArchedBarGraph_a->minValue >> 8;
      destMemory [16] = vtObjectArchedBarGraph_a->maxValue & 0xFF;
      destMemory [17] = vtObjectArchedBarGraph_a->maxValue >> 8;
      if (vtObjectArchedBarGraph_a->variableReference != NULL) {
        destMemory [18] = vtObjectArchedBarGraph_a->variableReference->getID() & 0xFF;
        destMemory [19] = vtObjectArchedBarGraph_a->variableReference->getID() >> 8;
      } else {
        destMemory [18] = 0xFF;
        destMemory [19] = 0xFF;
      }
      destMemory [20] = vtObjectArchedBarGraph_a->value & 0xFF;
      destMemory [21] = vtObjectArchedBarGraph_a->value >> 8;
      if (vtObjectArchedBarGraph_a->targetValueVariableReference != NULL) {
        destMemory [22] = vtObjectArchedBarGraph_a->targetValueVariableReference->getID() & 0xFF;
        destMemory [23] = vtObjectArchedBarGraph_a->targetValueVariableReference->getID() >> 8;
      } else {
        destMemory [22] = 0xFF;
        destMemory [23] = 0xFF;
      }
      destMemory [24] = vtObjectArchedBarGraph_a->targetValue & 0xFF;
      destMemory [25] = vtObjectArchedBarGraph_a->targetValue >> 8;
      destMemory [26] = vtObjectArchedBarGraph_a->numberOfMacrosToFollow;
      sourceOffset += 27;
      curBytes += 27;
    }

    MACRO_streamEventMacro(27);
    return curBytes;
} // -X2C

// //////////////////////////////// +X2C Operation 123 : vtObjectArchedBarGraph_c
vtObjectArchedBarGraph_c::vtObjectArchedBarGraph_c()
{ // ~X2C
} // -X2C

// //////////////////////////////// +X2C Operation 201 : size
uint32_t
vtObjectArchedBarGraph_c::fitTerminal()
{ // ~X2C
  MACRO_localVars;
  return 27+vtObjectArchedBarGraph_a->numberOfMacrosToFollow*2;
} // -X2C

// //////////////////////////////// +X2C Operation 250 : setValue
//! Parameter:
//! @param newValue:
//! @param b_updateObject:
void
vtObjectArchedBarGraph_c::setValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{ // ~X2C
  if (get_vtObjectArchedBarGraph_a()->variableReference == NULL) {
    if (b_updateObject) saveValue16 (MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), value), sizeof(iVtObjectArchedBarGraph_s), newValue);

    __IsoAgLib::getIsoTerminalInstance4Comm().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, 0x00, 0x00, b_enableReplaceOfCmd);
  }
} // -X2C


void
vtObjectArchedBarGraph_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), width),  sizeof(iVtObjectArchedBarGraph_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), height), sizeof(iVtObjectArchedBarGraph_s), newHeight);
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

} // end namespace __IsoAgLib
