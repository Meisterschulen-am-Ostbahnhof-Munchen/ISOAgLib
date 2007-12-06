/***************************************************************************
                          vtobjectoutputstring_c.cpp
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
#include "vtobjectoutputstring_c.h"
#include "../ivtobjectoutputstring_c.h"

#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectstringvariable_c.h"
#include "isoterminal_c.h"
#include <IsoAgLib/util/impl/util_funcs.h>

namespace IsoAgLib {
  // implement here a normal constructor/destructor, as the compiler dislikes inlining of that simple
  // constructor/destructor direct in scope of iVtObjectOutputString_c
  iVtObjectOutputString_c::iVtObjectOutputString_c() {}
  iVtObjectOutputString_c::~iVtObjectOutputString_c(){}
};

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// Operation : stream
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectOutputString_c::stream(uint8_t* destMemory,
                               uint16_t maxBytes,
                               objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectOutputString_a
#define MACRO_vtObjectTypeS iVtObjectOutputString_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 11; // Object Type = Output String
      if ((s_properties.flags & FLAG_ORIGIN_SKM) || p_parentButtonObject) {
        destMemory [3] = (((uint32_t) vtObjectOutputString_a->width*factorM)/factorD) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectOutputString_a->width*factorM)/factorD) >> 8;
        destMemory [5] = (((uint32_t) vtObjectOutputString_a->height*factorM)/factorD) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectOutputString_a->height*factorM)/factorD) >> 8;
      } else {
        destMemory [3] = (((uint32_t) vtObjectOutputString_a->width*vtDimension)/opDimension) & 0xFF;
        destMemory [4] = (((uint32_t) vtObjectOutputString_a->width*vtDimension)/opDimension) >> 8;
        destMemory [5] = (((uint32_t) vtObjectOutputString_a->height*vtDimension)/opDimension) & 0xFF;
        destMemory [6] = (((uint32_t) vtObjectOutputString_a->height*vtDimension)/opDimension) >> 8;
      }
      destMemory [7] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (vtObjectOutputString_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [8] = vtObjectOutputString_a->fontAttributes->getID() & 0xFF;
      destMemory [9] = vtObjectOutputString_a->fontAttributes->getID() >> 8;
      destMemory [10] = vtObjectOutputString_a->options;
      if (vtObjectOutputString_a->variableReference != NULL) {
        destMemory [11] = vtObjectOutputString_a->variableReference->getID() & 0xFF;
        destMemory [12] = vtObjectOutputString_a->variableReference->getID() >> 8;
      } else {
        destMemory [11] = 0xFF;
        destMemory [12] = 0xFF;
      }
      destMemory [13] = vtObjectOutputString_a->horizontalJustification;
      destMemory [14] = vtObjectOutputString_a->length & 0xFF;
      destMemory [15] = vtObjectOutputString_a->length >> 8;
      sourceOffset += 16;
      curBytes += 16;
    }

    while ((sourceOffset >= 16U) && (sourceOffset < (16U+vtObjectOutputString_a->length)) && ((curBytes+1) <= maxBytes)) {
      if (vtObjectOutputString_a->value == NULL)
        destMemory [curBytes] = 0x00;
      else
        destMemory [curBytes] = vtObjectOutputString_a->value [sourceOffset-16];
      curBytes++;
      sourceOffset++;
    }

    if ((sourceOffset == (16U + vtObjectOutputString_a->length)) && ((curBytes+1) <= maxBytes)) {
      destMemory [curBytes] = vtObjectOutputString_a->numberOfMacrosToFollow;
      curBytes++;
      sourceOffset++;
    }

    MACRO_streamEventMacro(17U+vtObjectOutputString_a->length);
    return curBytes;
}

// Operation : vtObjectOutputString_c
vtObjectOutputString_c::vtObjectOutputString_c() {}
vtObjectOutputString_c::~vtObjectOutputString_c() {}

// Operation : size
uint32_t
vtObjectOutputString_c::fitTerminal() const
{
  MACRO_localVars;
  return 17+vtObjectOutputString_a->length+vtObjectOutputString_a->numberOfMacrosToFollow*2;
}

// Operation : setOriginSKM
//! @param b_SKM:
void
vtObjectOutputString_c::setOriginSKM(bool b_SKM)
{
  MACRO_localVars;
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    vtObjectOutputString_a->fontAttributes->setOriginSKM (b_SKM);
  }
}

void
vtObjectOutputString_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn)
{
  MACRO_localVars;
  if (p_btn)
  {
    p_parentButtonObject = p_btn;
    vtObjectOutputString_a->fontAttributes->setOriginBTN (p_btn);
  }
}

#ifdef USE_VT_UNICODE_SUPPORT
// Operation : setValueCopyUTF8
//! @param newValue:
//! @param b_updateObject:
void
vtObjectOutputString_c::setValueCopyUTF8 (const char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  const uint16_t cui16_strLen = (uint16_t)CNAMESPACE::strlen (newValue);
  char* pc_iso8859 = new char [cui16_strLen+1];

  const uint8_t cui8_fontType = get_vtObjectOutputString_a()->fontAttributes->get_vtObjectFontAttributes_a().fontType;

  convertStringUnicodeTo8859 (newValue, cui16_strLen, pc_iso8859, cui8_fontType);

  setValueCopy (pc_iso8859, b_updateObject, b_enableReplaceOfCmd);
  delete[] pc_iso8859;
}
#endif

// Operation : setValueCopy
//! @param newValue:
//! @param b_updateObject:
void
vtObjectOutputString_c::setValueCopy(const char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  if (vtObjectOutputString_a->variableReference != NULL) {
    // register error!!
    return;
  }

  if (b_updateObject) {
    // check if not already RAM string buffer?
    if (!(s_properties.flags & FLAG_STRING_IN_RAM)) {
      s_properties.flags |= FLAG_STRING_IN_RAM;
      // create new String buffer with same length as original one, as the size can't be changed !!
      char *newStringBuffer = new (char [get_vtObjectOutputString_a()->length+1]);
      saveValueP (MACRO_getStructOffset(get_vtObjectOutputString_a(), value), sizeof(iVtObjectOutputString_s), (IsoAgLib::iVtObject_c*) newStringBuffer);
    }
    char *dest = get_vtObjectOutputString_a()->value;
    const char *src = newValue;
    int copyLen = (CNAMESPACE::strlen (newValue) <= get_vtObjectOutputString_a()->length) ? CNAMESPACE::strlen (newValue) : get_vtObjectOutputString_a()->length;
    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    for (; i<get_vtObjectOutputString_a()->length; i++) *dest++ = ' ';
    *dest = 0x00; // 0-termiante!
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeStringValue (this, newValue, get_vtObjectOutputString_a()->length, b_enableReplaceOfCmd);
}

// Operation : setValueRef
//! @param newValue:
//! @param b_updateObject:
void
vtObjectOutputString_c::setValueRef(const char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  if (vtObjectOutputString_a->variableReference != NULL) {
    // register error!!
    return;
  }

  if (b_updateObject) {
    // delete RAM_String first, before we lose the pointer!
    if (s_properties.flags & FLAG_STRING_IN_RAM) {
      delete (get_vtObjectOutputString_a()->value);
      s_properties.flags &= ~FLAG_STRING_IN_RAM;
    }

    saveValueP (MACRO_getStructOffset(get_vtObjectOutputString_a(), value), sizeof(iVtObjectOutputString_s), (IsoAgLib::iVtObject_c*) newValue);
  }

  setStringToStream( newValue ); // use MultiSendStreamer with mpc_stringToStream set!
  uint16_t ui16_tempLen = 0;
  if (newValue != NULL ) ui16_tempLen = (CNAMESPACE::strlen (newValue) <= get_vtObjectOutputString_a()->length) ? CNAMESPACE::strlen (newValue) : get_vtObjectOutputString_a()->length;
  setStrLenToSend( ui16_tempLen );
  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeStringValue (this, b_enableReplaceOfCmd);
}

/** obsolete, now no more checking for length != 0, set VaRef anyway!
// Operation : setVariableReference
//! @param newVariable:
//! @param b_updateObject:
void
vtObjectOutputString_c::setVariableReference(IsoAgLib::iVtObjectStringVariable_c* newVariable, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  if (vtObjectOutputString_a->length != 0) {
    // register error!!
    return;
  }

  if (b_updateObject) saveValueP (MACRO_getStructOffset(get_vtObjectOutputString_a(), variableReference),  sizeof(iVtObjectOutputString_s), newVariable);

  uint16_t newVariableID = newVariable ? (newVariable->getID()) : 0xFFFF;
  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeAttribute (this, 6, (newVariableID & 0xFF), (newVariableID >> 8), 0, 0, b_enableReplaceOfCmd);
}
*/

void
vtObjectOutputString_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectOutputString_a(), width),  sizeof(iVtObjectOutputString_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectOutputString_a(), height), sizeof(iVtObjectOutputString_s), newHeight);
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

// Operation : getString
const char*
vtObjectOutputString_c::getString()
{
  return get_vtObjectOutputString_a()->value;
}

#ifdef USE_GETATTRIBUTE
uint16_t
vtObjectOutputString_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), width), sizeof(iVtObjectOutputString_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), width), sizeof(iVtObjectOutputString_s));
}

uint16_t
vtObjectOutputString_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), height), sizeof(iVtObjectOutputString_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), height), sizeof(iVtObjectOutputString_s));
}

uint8_t
vtObjectOutputString_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), backgroundColour), sizeof(iVtObjectOutputString_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), backgroundColour), sizeof(iVtObjectOutputString_s));
}

uint16_t
vtObjectOutputString_c::updateFontAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), fontAttributes), sizeof(iVtObjectOutputString_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), fontAttributes), sizeof(iVtObjectOutputString_s));
}

uint8_t
vtObjectOutputString_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), options), sizeof(iVtObjectOutputString_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), options), sizeof(iVtObjectOutputString_s));
}

uint16_t
vtObjectOutputString_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), variableReference), sizeof(iVtObjectOutputString_s), 6);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), variableReference), sizeof(iVtObjectOutputString_s));
}

uint8_t
vtObjectOutputString_c::updateJustification(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputString_a(), horizontalJustification), sizeof(iVtObjectOutputString_s), 7);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), horizontalJustification), sizeof(iVtObjectOutputString_s));
}

void
vtObjectOutputString_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), width), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), height), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), backgroundColour), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), fontAttributes), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), options), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue16(MACRO_getStructOffset(get_vtObjectOutputString_a(), variableReference), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 7: saveValue8(MACRO_getStructOffset(get_vtObjectOutputString_a(), horizontalJustification), sizeof(iVtObjectOutputString_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    default: break;
  }
}
#endif
} // end of namespace __IsoAgLib
