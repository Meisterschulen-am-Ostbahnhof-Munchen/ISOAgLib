/***************************************************************************
                             vtobjectmeter_c.cpp
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

#include "vtobjectmeter_c.h"
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
vtObjectMeter_c::stream(uint8_t* destMemory,
                                 uint16_t maxBytes,
                                 uint16_t sourceOffset)
{ // ~X2C
#define MACRO_vtObjectTypeA vtObjectMeter_a
#define MACRO_vtObjectTypeS iVtObjectMeter_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 17; // Object Type = Meter
      if (flags & FLAG_ORIGIN_SKM) { // no need to check for p_parentButtonObject as this object can't be nested in a button!
        destMemory [3] = (((uint32_t) vtObjectMeter_a->width*factorM)/factorD) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectMeter_a->width*factorM)/factorD) >> 8;
      } else {
        destMemory [3] = (((uint32_t) vtObjectMeter_a->width*vtDimension)/opDimension) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectMeter_a->width*vtDimension)/opDimension) >> 8;
      }
      destMemory [5] = __IsoAgLib::getIsoTerminalInstance4Comm().getUserClippedColor (vtObjectMeter_a->needleColour, this, IsoAgLib::NeedleColour);
      destMemory [6] = __IsoAgLib::getIsoTerminalInstance4Comm().getUserClippedColor (vtObjectMeter_a->borderColour, this, IsoAgLib::BorderColour);
      destMemory [7] = __IsoAgLib::getIsoTerminalInstance4Comm().getUserClippedColor (vtObjectMeter_a->arcAndTickColour, this, IsoAgLib::ArcAndTickColour);
      destMemory [8] = vtObjectMeter_a->options;
      destMemory [9] = vtObjectMeter_a->numberOfTicks;
      destMemory [10] = vtObjectMeter_a->startAngle;
      destMemory [11] = vtObjectMeter_a->endAngle;
      destMemory [12] = vtObjectMeter_a->minValue & 0xFF;
      destMemory [13] = vtObjectMeter_a->minValue >> 8;
      destMemory [14] = vtObjectMeter_a->maxValue & 0xFF;
      destMemory [15] = vtObjectMeter_a->maxValue >> 8;
      if (vtObjectMeter_a->variableReference != NULL) {
        destMemory [16] = vtObjectMeter_a->variableReference->getID() & 0xFF;
        destMemory [17] = vtObjectMeter_a->variableReference->getID() >> 8;
      } else {
        destMemory [16] = 0xFF;
        destMemory [17] = 0xFF;
      }
      destMemory [18] = vtObjectMeter_a->value & 0xFF;
      destMemory [19] = vtObjectMeter_a->value >> 8;

      destMemory [20] = vtObjectMeter_a->numberOfMacrosToFollow;
      sourceOffset += 21;
      curBytes += 21;
    }

    MACRO_streamEventMacro(21);
    return curBytes;
} // -X2C

// //////////////////////////////// +X2C Operation 123 : vtObjectMeter_c
vtObjectMeter_c::vtObjectMeter_c()
{ // ~X2C
} // -X2C

// //////////////////////////////// +X2C Operation 201 : size
uint32_t
vtObjectMeter_c::fitTerminal()
{ // ~X2C
  MACRO_localVars;
  return 21+vtObjectMeter_a->numberOfMacrosToFollow*2;
} // -X2C

// //////////////////////////////// +X2C Operation 250 : setValue
//! Parameter:
//! @param newValue:
//! @param b_updateObject:
void
vtObjectMeter_c::setValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{ // ~X2C
  if (get_vtObjectMeter_a()->variableReference == NULL) {
    if (b_updateObject) saveValue16 (MACRO_getStructOffset(get_vtObjectMeter_a(), value), sizeof(iVtObjectMeter_s), newValue);

    __IsoAgLib::getIsoTerminalInstance4Comm().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, 0x00, 0x00, b_enableReplaceOfCmd);
  }
} // -X2C

} // end namespace __IsoAgLib
