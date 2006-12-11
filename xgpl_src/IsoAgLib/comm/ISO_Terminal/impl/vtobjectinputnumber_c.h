/***************************************************************************
                          vtobjectinputnumber_c.h
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
#ifndef VTOBJECTINPUTNUMBER_C_H
#define VTOBJECTINPUTNUMBER_C_H

#include "vtobject_c.h"
#include "isoterminal_c.h"
#include "vtclientservercommunication_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectInputNumber_c : public vtObject_c
{
public:
  //  Operation: stream
  //! @param:destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  //  Operation: init
  //! @param vtObjectInputNumberSROM:
  //! @param b_initPointer:
  void init(const iVtObjectInputNumber_s* vtObjectInputNumberSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectInputNumberSROM SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA); }

  //  Operation: get_vtObjectInputNumber_a
  iVtObjectInputNumber_s* get_vtObjectInputNumber_a() { return (iVtObjectInputNumber_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectInputNumber_c
  vtObjectInputNumber_c();

  //  Operation: size
  uint32_t fitTerminal() const;

  //  Operation: setValue
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), width) : 0, sizeof(iVtObjectInputNumber_s), 1, newValue, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), height) : 0, sizeof(iVtObjectInputNumber_s), 2, newValue, b_enableReplaceOfCmd);
  }

  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), backgroundColour) : 0, sizeof(iVtObjectInputNumber_s), 3, newValue, __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
  }

  void setFontAttributes(IsoAgLib::iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), fontAttributes) : 0, sizeof(iVtObjectInputNumber_s), 4, newValue, b_enableReplaceOfCmd);
  }

  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), options) : 0, sizeof(iVtObjectInputNumber_s), 5, newValue, newValue, b_enableReplaceOfCmd);
  }

  void setVariableReference(IsoAgLib::iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), variableReference) : 0, sizeof(iVtObjectInputNumber_s), 6, (iVtObject_c*)newValue, b_enableReplaceOfCmd);
  }

  void setMinValue(uint32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue32SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), minValue) : 0, sizeof(iVtObjectInputNumber_s), 7, newValue, b_enableReplaceOfCmd);
  }

  void setMaxValue(uint32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue32SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), maxValue) : 0, sizeof(iVtObjectInputNumber_s), 8, newValue, b_enableReplaceOfCmd);
  }

  void setOffset(int32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue32SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), offset) : 0, sizeof(iVtObjectInputNumber_s), 9, (uint32_t) newValue, b_enableReplaceOfCmd);
  }

  void setScale(float newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValueFloatSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), scale) : 0, sizeof(iVtObjectInputNumber_s), 10, newValue, b_enableReplaceOfCmd);
  }

  void setNumberOfDecimals(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), numberOfDecimals) : 0, sizeof(iVtObjectInputNumber_s), 11, newValue, newValue, b_enableReplaceOfCmd);
  }

  void setFormat(bool newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), format) : 0, sizeof(iVtObjectInputNumber_s), 12, (newValue) ? 1 : 0, (newValue) ? 1 : 0, b_enableReplaceOfCmd);
  }

  void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputNumber_a(), horizontalJustification) : 0, sizeof(iVtObjectInputNumber_s), 13, newValue, newValue, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 9; }
   */

  uint16_t updateWidth(bool b_SendRequest=false);

  uint16_t updateHeight(bool b_SendRequest=false);

  uint8_t updateBackgroundColour(bool b_SendRequest=false);

  uint16_t updateFontAttributes(bool b_SendRequest=false);

  uint8_t updateOptions(bool b_SendRequest=false);

  uint16_t updateVariableReference(bool b_SendRequest=false);

  uint32_t updateMinValue(bool b_SendRequest=false);

  uint32_t updateMaxValue(bool b_SendRequest=false);

  int32_t updateOffset(bool b_SendRequest=false);

  float updateScale(bool b_SendRequest=false);

  uint8_t updateNumberOfDecimals(bool b_SendRequest=false);

  uint8_t updateFormat(bool b_SendRequest=false);

  uint8_t updateJustification(bool b_SendRequest=false);

  /** these attributes are in parentheses in the spec, so commented out here
  uint32_t updateValue(bool b_SendRequest=false);

  uint8_t update2ndOptionsByte(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
private:
  //  Operation: updateEnable
  //! @param b_enableOrDisable:
  void updateEnable(uint8_t rui8_enOrDis);
};

} // end of namespace __IsoAgLib

#endif
