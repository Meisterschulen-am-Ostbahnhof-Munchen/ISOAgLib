/***************************************************************************
                          vtobjectstringvariable_c.cpp
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

#include "vtobjectstringvariable_c.h"
#include "isoterminal_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {



// //////////////////////////////// +X2C Operation 162 : stream
//! Parameter:
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectStringVariable_c::stream(uint8_t* destMemory,
                                 uint16_t maxBytes,
                                 objRange_t sourceOffset)
{ // ~X2C
#define MACRO_vtObjectTypeA vtObjectStringVariable_a
#define MACRO_vtObjectTypeS iVtObjectStringVariable_s
  MACRO_streamLocalVars;

  if (sourceOffset == 0) { // dump out constant sized stuff
    destMemory [0] = vtObject_a->ID & 0xFF;
    destMemory [1] = vtObject_a->ID >> 8;
    destMemory [2] = 22; // Object Type = Integer Variable
    destMemory [3] = vtObjectStringVariable_a->length & 0xFF;
    destMemory [4] = vtObjectStringVariable_a->length >> 8;
    curBytes += 5;
    sourceOffset += 5;
  }

  while ((sourceOffset >= 5U) && (sourceOffset < (5U+vtObjectStringVariable_a->length)) && ((curBytes+1) <= maxBytes)) {
    destMemory [curBytes] = vtObjectStringVariable_a->value [sourceOffset-5];
    curBytes++;
    sourceOffset++;
  }

  return curBytes;
} // -X2C

// //////////////////////////////// +X2C Operation 165 : vtObjectStringVariable_c
vtObjectStringVariable_c::vtObjectStringVariable_c()
{ // ~X2C
} // -X2C

// //////////////////////////////// +X2C Operation 204 : size
uint32_t
vtObjectStringVariable_c::fitTerminal()
{ // ~X2C
  MACRO_localVars;
  return 5+vtObjectStringVariable_a->length;
} // -X2C

// //////////////////////////////// +X2C Operation 236 : setValueCopy
//! Parameter:
//! @param newValue:
//! @param b_updateObject:
void
vtObjectStringVariable_c::setValueCopy(char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{ // ~X2C
  if (b_updateObject) {
    // check if not already RAM string buffer?
    if (!(flags & FLAG_STRING_IN_RAM)) {
      flags |= FLAG_STRING_IN_RAM;
      // create new String buffer with same length as original one, as the size can't be changed !!
      char *newStringBuffer = new (char [get_vtObjectStringVariable_a()->length+1]);
      saveValueP (MACRO_getStructOffset(get_vtObjectStringVariable_a(), value), sizeof(iVtObjectStringVariable_s), (IsoAgLib::iVtObject_c*) newStringBuffer);
    }
    char *dest = get_vtObjectStringVariable_a()->value;
    const char *src = newValue;
    int copyLen = (CNAMESPACE::strlen (newValue) <= get_vtObjectStringVariable_a()->length) ? CNAMESPACE::strlen (newValue) : get_vtObjectStringVariable_a()->length;
    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    for (; i<get_vtObjectStringVariable_a()->length; i++) *dest++ = ' ';
    *dest = 0x00; // 0-termiante!
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().sendCommandChangeStringValue (this, newValue, get_vtObjectStringVariable_a()->length, b_enableReplaceOfCmd);
} // -X2C

// //////////////////////////////// +X2C Operation 236 : setValue
//! Parameter:
//! @param newValue:
//! @param b_updateObject:
void
vtObjectStringVariable_c::setValueRef(const char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{ // ~X2C
  if (b_updateObject) {
    // delete RAM_String first, before we lose the pointer!
    if (flags & FLAG_STRING_IN_RAM) {
      delete (get_vtObjectStringVariable_a()->value);
      flags &= ~FLAG_STRING_IN_RAM;
    }

    saveValueP (MACRO_getStructOffset(get_vtObjectStringVariable_a(), value), sizeof(iVtObjectStringVariable_s), (IsoAgLib::iVtObject_c*) newValue);
  }

  setStringToStream( newValue ); // use MultiSendStreamer with pc_stringToStream set!
  uint16_t ui16_tempLen = 0;
  if (newValue != NULL ) ui16_tempLen = (CNAMESPACE::strlen (newValue) <= get_vtObjectStringVariable_a()->length) ? CNAMESPACE::strlen (newValue) : get_vtObjectStringVariable_a()->length;
  setStrLenToSend( ui16_tempLen );
  __IsoAgLib::getIsoTerminalInstance4Comm().sendCommandChangeStringValue (this, b_enableReplaceOfCmd);
} // -X2C


// //////////////////////////////// +X2C Operation 248 : getString
const char*
vtObjectStringVariable_c::getString()
{ // ~X2C
  return get_vtObjectStringVariable_a()->value;
} // -X2C

} // end of namespace __IsoAgLib
