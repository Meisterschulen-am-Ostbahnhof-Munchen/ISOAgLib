/***************************************************************************
                          vtobjectinputstring_c.h
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
#ifndef VTOBJECTINPUTSTRING_C_H
#define VTOBJECTINPUTSTRING_C_H

#include "../ivtobjectstring_c.h"
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectinputattributes_c.h"
#include "../ivtobjectstringvariable_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectInputString_c : public IsoAgLib::iVtObjectString_c
{
public:
  //  Operation: stream
  //! @param destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  //  Operation: init
  //! @param vtObjectInputStringSROM:
  //! @param b_initPointer:
  void init(const iVtObjectInputString_s* vtObjectInputStringSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectInputStringSROM SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA); }

  //  Operation: get_vtObjectInputString_a
  iVtObjectInputString_s* get_vtObjectInputString_a() { return (iVtObjectInputString_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectInputString_c
  vtObjectInputString_c();

  //  Operation: fitTerminal
  uint32_t fitTerminal() const;

  //  Operation: getString
  const char* getString();

  //  Operation: setValueRef
  //! @param newValue:
  //! @param b_updateObject:
  void setValueRef(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  //  Operation: setValueCopy
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopy(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

#ifdef USE_VT_UNICODE_SUPPORT
  //  Operation: setValueCopyUTF8
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopyUTF8(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
#endif

  //  Operation: setSize
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  // Operation: setWidth
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), width) : 0, sizeof(iVtObjectInputString_s), 1 /* "Width" */, newValue, b_enableReplaceOfCmd);
  }

  // Operation: setHeight
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), height) : 0, sizeof(iVtObjectInputString_s), 2 /* "Height" */, newValue, b_enableReplaceOfCmd);
  }

  //  Operation: setBackgroundColor
   //! @param colorValue:
  //! @param b_updateObject:
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), backgroundColour) : 0, sizeof(iVtObjectInputString_s), 3 /* "Background Colour" */, newValue, __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
  }

  // Operation: setFontAttributes
  //! @param newFontAttributes:
  //! @param b_updateObject:
  void setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c* newFontAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), fontAttributes) : 0, sizeof(iVtObjectInputString_s), 4 /* "Font Attributes" */, newFontAttributes, b_enableReplaceOfCmd);
  }

  // Operation: setInputAttributes
  //! @param newInputAttributes:
  //! @param b_updateObject:
  void setInputAttributes(IsoAgLib::iVtObjectInputAttributes_c* newInputAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), inputAttributes) : 0, sizeof(iVtObjectInputString_s), 5 /* "Input Attributes" */, newInputAttributes, b_enableReplaceOfCmd);
  }

  // Operation: setOptions
  //! @param newOptions:
  //! @param b_updateObject:
  void setOptions(uint8_t newOptions, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), options) : 0, sizeof(iVtObjectInputString_s), 6 /* "Options" */, newOptions, newOptions, b_enableReplaceOfCmd);
  }

  // Operation: setVariableReference
  //! @param newVariableRef:
  //! @param b_updateObject:
  void setVariableReference(IsoAgLib::iVtObjectStringVariable_c* newVariableRef, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), variableReference) : 0, sizeof(iVtObjectInputString_s), 7 /* "Variable Reference" */, newVariableRef, b_enableReplaceOfCmd);
  }

  // Operation: setHorizontalJustification
  //! @param newHorizontalJustification:
  //! @param b_updateObject:
  void setHorizontalJustification(uint8_t newHorizontalJustification, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputString_a(), horizontalJustification) : 0, sizeof(iVtObjectInputString_s), 8 /* "Horizontal justification" */, newHorizontalJustification, newHorizontalJustification, b_enableReplaceOfCmd);
  }

  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 8; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);

  uint16_t updateHeight(bool b_SendRequest=false);

  uint8_t updateBackgroundColour(bool b_SendRequest=false);

  uint16_t updateFontAttributes(bool b_SendRequest=false);

  uint16_t updateInputAttributes(bool b_SendRequest=false);

  uint8_t updateOptions(bool b_SendRequest=false);

  uint16_t updateVariableReference(bool b_SendRequest=false);

  uint8_t updateJustification(bool b_SendRequest=false);

  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateEnabled(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);

private:
  //  Operation: updateEnable
  //! @param b_enableOrDisable:
  void updateEnable(uint8_t rui8_enOrDis);
};

} // end namespace __IsoAgLib

#endif
