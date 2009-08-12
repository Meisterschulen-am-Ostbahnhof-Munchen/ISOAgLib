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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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

#ifdef USE_VTOBJECT_outputnumber
#include <IsoAgLib/util/impl/util_funcs.h>
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// Operation : stream
//! @param destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectOutputNumber_c::stream(uint8_t* destMemory,
                               uint16_t maxBytes,
                               objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectOutputNumber_a
#define MACRO_vtObjectTypeS iVtObjectOutputNumber_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 12; // Object Type = Output Number
      if ((s_properties.flags & FLAG_ORIGIN_SKM) || p_parentButtonObject) {
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
      destMemory [7] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (vtObjectOutputNumber_a->backgroundColour, this, IsoAgLib::BackgroundColour);
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

      uint32_t offset = *((uint32_t*)&(vtObjectOutputNumber_a->offset));
      destMemory [17] = (offset) & 0xFF;
      destMemory [18] = (offset >> 8) & 0xFF;
      destMemory [19] = (offset >> 16) & 0xFF;
      destMemory [20] = (offset >> 24) & 0xFF;

      __IsoAgLib::floatVar2LittleEndianStream (&vtObjectOutputNumber_a->scale, &destMemory[21]);

      destMemory [25] = vtObjectOutputNumber_a->numberOfDecimals;
      destMemory [26] = vtObjectOutputNumber_a->format;
      destMemory [27] = vtObjectOutputNumber_a->horizontalJustification;
      destMemory [28] = vtObjectOutputNumber_a->numberOfMacrosToFollow;
      sourceOffset += 29;
      curBytes += 29;
    }

    MACRO_streamEventMacro(29);
    return curBytes;
}

// Operation : vtObjectOutputNumber_c
vtObjectOutputNumber_c::vtObjectOutputNumber_c() {}

// Operation : size
uint32_t
vtObjectOutputNumber_c::fitTerminal() const
{
  MACRO_localVars;
  return 29+vtObjectOutputNumber_a->numberOfMacrosToFollow*2;
}

// Operation : setValue
//! @param newValue:
//! @param b_updateObject:
void
vtObjectOutputNumber_c::setValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (get_vtObjectOutputNumber_a()->variableReference == NULL) {
    // Save Value if requested
    if (b_updateObject) saveValue32 (MACRO_getStructOffset(get_vtObjectOutputNumber_a(), value), sizeof(iVtObjectOutputNumber_s), newValue);

    // Send Value update
    __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, (newValue >> 16) & 0xFF, newValue >> 24, b_enableReplaceOfCmd);
  }
}

// Operation : setOriginSKM
//! @param b_SKM:
void
vtObjectOutputNumber_c::setOriginSKM(bool b_SKM)
{
  MACRO_localVars;
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    vtObjectOutputNumber_a->fontAttributes->setOriginSKM (b_SKM);
  }
}

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

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectOutputNumber_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), width), sizeof(iVtObjectOutputNumber_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), width), sizeof(iVtObjectOutputNumber_s));
}

uint16_t
vtObjectOutputNumber_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), height), sizeof(iVtObjectOutputNumber_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), height), sizeof(iVtObjectOutputNumber_s));
}

uint8_t
vtObjectOutputNumber_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), backgroundColour), sizeof(iVtObjectOutputNumber_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), backgroundColour), sizeof(iVtObjectOutputNumber_s));
}

uint16_t
vtObjectOutputNumber_c::updateFontAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), fontAttributes), sizeof(iVtObjectOutputNumber_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), fontAttributes), sizeof(iVtObjectOutputNumber_s));
}

uint8_t
vtObjectOutputNumber_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), options), sizeof(iVtObjectOutputNumber_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), options), sizeof(iVtObjectOutputNumber_s));
}

uint16_t
vtObjectOutputNumber_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), variableReference), sizeof(iVtObjectOutputNumber_s), 6);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), variableReference), sizeof(iVtObjectOutputNumber_s));
}

int32_t
vtObjectOutputNumber_c::updateOffset(bool b_SendRequest)
{
  if (b_SendRequest)
    return getSignedValue32GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), offset), sizeof(iVtObjectOutputNumber_s), 7);
  else
    return getSignedValue32(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), offset), sizeof(iVtObjectOutputNumber_s));
}

float
vtObjectOutputNumber_c::updateScale(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValueFloatGetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), scale), sizeof(iVtObjectOutputNumber_s), 8);
  else
    return getValueFloat(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), scale), sizeof(iVtObjectOutputNumber_s));
}

uint8_t
vtObjectOutputNumber_c::updateNumberOfDecimals(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), numberOfDecimals), sizeof(iVtObjectOutputNumber_s), 9);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), numberOfDecimals), sizeof(iVtObjectOutputNumber_s));
}

uint8_t
vtObjectOutputNumber_c::updateFormat(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), format), sizeof(iVtObjectOutputNumber_s), 10);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), format), sizeof(iVtObjectOutputNumber_s));
}

uint8_t
vtObjectOutputNumber_c::updateJustification(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), horizontalJustification), sizeof(iVtObjectOutputNumber_s), 11);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), horizontalJustification), sizeof(iVtObjectOutputNumber_s));
}

/** that attribute is in parentheses in the spec, so commented out here
uint32_t
vtObjectOutputNumber_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue32GetAttribute(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), value), sizeof(iVtObjectOutputNumber_s), 12);
  else
    return getValue32(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), value), sizeof(iVtObjectOutputNumber_s));
}
*/

void
vtObjectOutputNumber_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), width), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), height), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), backgroundColour), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), fontAttributes), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), options), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue16(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), variableReference), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 7: saveSignedValue32(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), offset), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringI32(pui8_attributeValue)); break;
    case 8: saveValueFloat(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), scale), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringFloat(pui8_attributeValue)); break;
    case 9: saveValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), numberOfDecimals), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 10: saveValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), format), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 11: saveValue8(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), horizontalJustification), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    /** that attribute is in parentheses in the spec, so commented out here
    case 12: saveValue32(MACRO_getStructOffset(get_vtObjectOutputNumber_a(), value), sizeof(iVtObjectOutputNumber_s), convertLittleEndianStringUi32(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
} // end of namespace __IsoAgLib
#endif
