/***************************************************************************
                         vtobjectinputattributes_c.cpp
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



#include "vtobjectinputattributes_c.h"
#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {



// //////////////////////////////// +X2C Operation 168 : stream
//! Parameter:
//! @param destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectInputAttributes_c::stream(uint8_t* destMemory,
                                  uint16_t maxBytes,
                                  objRange_t sourceOffset)
{ // ~X2C
#define MACRO_vtObjectTypeA vtObjectInputAttributes_a
#define MACRO_vtObjectTypeS iVtObjectInputAttributes_s
    MACRO_streamLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 26; // Object Type = Input Attributes
      destMemory [3] = vtObjectInputAttributes_a->validationType;

      destMemory [4] = vtObjectInputAttributes_a->length;
      sourceOffset += 5;
      curBytes += 5;
    }

    while ((sourceOffset >= 5U) && (sourceOffset < (5U+vtObjectInputAttributes_a->length)) && ((curBytes+1) <= maxBytes))
    {
      if (vtObjectInputAttributes_a->validationString == NULL)
      {
          destMemory [curBytes] = 0x00;
      }
      else
      {
          destMemory [curBytes] = vtObjectInputAttributes_a->validationString [sourceOffset-5];
      }
      curBytes++;
      sourceOffset++;
    }

    if ((sourceOffset == (5U + vtObjectInputAttributes_a->length)) && ((curBytes+1) <= maxBytes))
    {
      destMemory [curBytes] = vtObjectInputAttributes_a->numberOfMacrosToFollow;
      curBytes++;
      sourceOffset++;
    }
    MACRO_streamEventMacro(6U+vtObjectInputAttributes_a->length);
    return curBytes;
} // -X2C

// //////////////////////////////// +X2C Operation 171 : vtObjectInputAttributes_c
vtObjectInputAttributes_c::vtObjectInputAttributes_c()
{
}

// //////////////////////////////// +X2C Operation 205 : size
uint32_t
vtObjectInputAttributes_c::fitTerminal() const
{
  MACRO_localVars;
  return 6+vtObjectInputAttributes_a->length+vtObjectInputAttributes_a->numberOfMacrosToFollow*2;
}

// //////////////////////////////// +X2C Operation 250 : setValidationStringCopy
//! Parameter:
//! @param newValidationString:
//! @param b_updateObject:
void
vtObjectInputAttributes_c::setValidationStringCopy(const char* newValidationString, bool b_updateObject, bool b_enableReplaceOfCmd)
{ // ~X2C
  if (b_updateObject) {
    // check if not already RAM string buffer?
    if (!(s_properties.flags & FLAG_STRING_IN_RAM)) {
      s_properties.flags |= FLAG_STRING_IN_RAM;
      // create new String buffer with same length as original one, as the size can't be changed !!
      char *newStringBuffer = new (char [get_vtObjectInputAttributes_a()->length+1]);
      saveValueP (MACRO_getStructOffset(get_vtObjectInputAttributes_a(), validationString), sizeof(iVtObjectInputAttributes_s), (IsoAgLib::iVtObject_c*) newStringBuffer);
    }
    char *dest = get_vtObjectInputAttributes_a()->validationString;
    const char *src = newValidationString;
    int copyLen = (CNAMESPACE::strlen (newValidationString) <= get_vtObjectInputAttributes_a()->length) ? CNAMESPACE::strlen (newValidationString) : get_vtObjectInputAttributes_a()->length;
    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    for (; i<get_vtObjectInputAttributes_a()->length; i++) *dest++ = ' ';
    *dest = 0x00; // 0-termiante!
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeStringValue (this, newValidationString, get_vtObjectInputAttributes_a()->length, b_enableReplaceOfCmd);
} // -X2C

// //////////////////////////////// +X2C Operation 250 : setValidationStringRef
//! Parameter:
//! @param newValidationString:
//! @param b_updateObject:
void
vtObjectInputAttributes_c::setValidationStringRef(const char* newValidationString, bool b_updateObject, bool b_enableReplaceOfCmd)
{ // ~X2C
  if (b_updateObject) {
    // delete RAM_String first, before we lose the pointer!
    if (s_properties.flags & FLAG_STRING_IN_RAM) {
      delete (get_vtObjectInputAttributes_a()->validationString);
      s_properties.flags &= ~FLAG_STRING_IN_RAM;
    }

    saveValueP (MACRO_getStructOffset(get_vtObjectInputAttributes_a(), validationString), sizeof(iVtObjectInputAttributes_s), (IsoAgLib::iVtObject_c*) newValidationString);
  }

  setStringToStream( newValidationString ); // use MultiSendStreamer with pc_stringToStream set!
  const uint16_t ui16_tempLen = (CNAMESPACE::strlen (newValidationString) <= get_vtObjectInputAttributes_a()->length) ? CNAMESPACE::strlen (newValidationString) : get_vtObjectInputAttributes_a()->length;
  setStrLenToSend( ui16_tempLen );
  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeStringValue (this, b_enableReplaceOfCmd);
} // -X2C


// //////////////////////////////// +X2C Operation 247 : getString
const char*
vtObjectInputAttributes_c::getString()
{ // ~X2C
  return get_vtObjectInputAttributes_a()->validationString;
} // -X2C
} // end namespace __IsoAgLib
