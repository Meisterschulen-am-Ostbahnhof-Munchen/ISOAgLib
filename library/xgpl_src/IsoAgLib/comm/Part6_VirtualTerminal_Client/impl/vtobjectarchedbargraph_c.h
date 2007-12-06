/***************************************************************************
                          vtobjectarchedbargraph_c.h
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
#ifndef VTOBJECTARCHEDBARGRAPH_C_H
#define VTOBJECTARCHEDBARGRAPH_C_H

// includes
#include "vtobject_c.h"
#include "isoterminal_c.h"
#include "vtclientservercommunication_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
// Class : vtObjectArchedBarGraph_c
class vtObjectArchedBarGraph_c : public vtObject_c
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
  //! @param vtObjectArchedBarGraphSROM:
  //! @param b_initPointer:
  void init(const iVtObjectArchedBarGraph_s* vtObjectArchedBarGraphSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectArchedBarGraphSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectArchedBarGraph_a
  iVtObjectArchedBarGraph_s* get_vtObjectArchedBarGraph_a() { return (iVtObjectArchedBarGraph_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectArchedBarGraph_c
  vtObjectArchedBarGraph_c();

  ~vtObjectArchedBarGraph_c() {}

  //  Operation: size
  uint32_t fitTerminal() const;

  void setValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true);

  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), width) : 0, sizeof(iVtObjectArchedBarGraph_s), 1 /* "Width" */, newWidth, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), height) : 0, sizeof(iVtObjectArchedBarGraph_s), 2 /* "Height" */, newHeight, b_enableReplaceOfCmd);
  }

  void setColour(uint8_t newColour, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), colour) : 0, sizeof(iVtObjectArchedBarGraph_s), 3 /* "Colour" */, newColour, __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newColour, this, IsoAgLib::Colour), b_enableReplaceOfCmd);
  }

  void setTargetLineColour(uint8_t newTargetLineColour, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetLineColour) : 0, sizeof(iVtObjectArchedBarGraph_s), 4 /* "Target Line Colour" */, newTargetLineColour, __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newTargetLineColour, this, IsoAgLib::TargetLineColour), b_enableReplaceOfCmd);
  }

  void setOptions(uint8_t newOptions, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), options) : 0, sizeof(iVtObjectArchedBarGraph_s), 5 /* "Options" */, newOptions, newOptions, b_enableReplaceOfCmd);
  }

  void setStartAngle(uint8_t newStartAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), startAngle) : 0, sizeof(iVtObjectArchedBarGraph_s), 6 /* "Start Angle" */, newStartAngle, newStartAngle, b_enableReplaceOfCmd);
  }

  void setEndAngle(uint8_t newEndAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), endAngle) : 0, sizeof(iVtObjectArchedBarGraph_s), 7 /* "End Angle" */, newEndAngle, newEndAngle, b_enableReplaceOfCmd);
  }

  void setBarGraphWidth(uint16_t newBarGraphWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), barGraphWidth) : 0, sizeof(iVtObjectArchedBarGraph_s), 8 /* "Bar Graph Width" */, newBarGraphWidth, b_enableReplaceOfCmd);
  }

  void setMinValue(uint16_t newMin, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), minValue) : 0, sizeof(iVtObjectArchedBarGraph_s), 9 /* "Min value" */, newMin, b_enableReplaceOfCmd);
  }

  void setMaxValue(uint16_t newMax, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), maxValue) : 0, sizeof(iVtObjectArchedBarGraph_s), 10 /* "Max value" */, newMax, b_enableReplaceOfCmd);
  }

  void setVariableReference(IsoAgLib::iVtObject_c* newVariableReference, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), variableReference) : 0, sizeof(iVtObjectArchedBarGraph_s), 11 /* "Variable Reference" */, newVariableReference, b_enableReplaceOfCmd);
  }

  void setTargetValueVariableReference(IsoAgLib::iVtObject_c* newTargetValueVariableReference, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValueVariableReference) : 0, sizeof(iVtObjectArchedBarGraph_s), 12 /* "Target Value Variable Reference" */, newTargetValueVariableReference, b_enableReplaceOfCmd);
  }

  void setTargetValue(uint16_t newTargetValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValue) : 0, sizeof(iVtObjectArchedBarGraph_s), 13 /* "Target Value" */, newTargetValue, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_GETATTRIBUTE
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 19; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);

  uint16_t updateHeight(bool b_SendRequest=false);

  uint8_t updateColour(bool b_SendRequest=false);

  uint8_t updateTargetLineColour(bool b_SendRequest=false);

  uint8_t updateOptions(bool b_SendRequest=false);

  uint8_t updateStartAngle(bool b_SendRequest=false);

  uint8_t updateEndAngle(bool b_SendRequest=false);

  uint16_t updateBarGraphWidth(bool b_SendRequest=false);

  uint16_t updateMinValue(bool b_SendRequest=false);

  uint16_t updateMaxValue(bool b_SendRequest=false);

  uint16_t updateVariableReference(bool b_SendRequest=false);

  uint16_t updateTargetValueVariableReference(bool b_SendRequest=false);

  uint16_t updateTargetValue(bool b_SendRequest=false);

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // end namespace __IsoAgLib

#endif
