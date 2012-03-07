/*
  vtobjectellipse_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTELLIPSE_C_H
#define VTOBJECTELLIPSE_C_H

#include <IsoAgLib/hal/hal_typedef.h>

#ifdef USE_VTOBJECT_ellipse
#include "vtobject_c.h"
#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectfillattributes_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectEllipse_c : public vtObject_c
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
  //! @param vtObjectellipseSROM:
  //! @param b_initPointer:
  void init(const iVtObjectEllipse_s* vtObjectellipseSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectellipseSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectmeter_a
  iVtObjectEllipse_s* get_vtObjectEllipse_a() { return (iVtObjectEllipse_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectEllipse_c
  vtObjectEllipse_c();

  //  Operation: fitTerminal
  uint32_t fitTerminal() const;

  //  Operation: setLineAttributes
  //! @param newLineAttribute:
  //! @param b_updateObject:
  void setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectEllipse_a(), lineAttributes) : 0, sizeof(iVtObjectEllipse_s), 1 /* "Line Attributes" */, newLineAttributes, b_enableReplaceOfCmd);
  }

  //  Operation: setWidth
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectEllipse_a(), width) : 0, sizeof(iVtObjectEllipse_s), 2 /* "Width" */, newWidth, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectEllipse_a(), height) : 0, sizeof(iVtObjectEllipse_s), 3 /* "Height" */, newHeight, b_enableReplaceOfCmd);
  }

  //  Operation: setEllipseType
  //! @param newEllipseType:
  //! @param b_updateObject:
  void setEllipseType(uint8_t newEllipseType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectEllipse_a(), ellipseType) : 0, sizeof(iVtObjectEllipse_s), 4 /* "Ellipse Type" */, newEllipseType, newEllipseType, b_enableReplaceOfCmd);
  }

  //  Operation: setStartAngle
  //! @param newStartAngle:
  //! @param b_updateObject:
  void setStartAngle(uint8_t newStartAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectEllipse_a(), startAngle) : 0, sizeof(iVtObjectEllipse_s), 5 /* "Start Angle" */, newStartAngle, newStartAngle, b_enableReplaceOfCmd);
  }

  //  Operation: setEndAngle
  //! @param newEndAngle:
  //! @param b_updateObject:
  void setEndAngle(uint8_t newEndAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectEllipse_a(), endAngle) : 0, sizeof(iVtObjectEllipse_s), 6 /* "End Angle" */, newEndAngle, newEndAngle, b_enableReplaceOfCmd);
  }

  //  Operation: setFillAttributes
  //! @param newFillAttribute:
  //! @param b_updateObject:
  void setFillAttributes(IsoAgLib::iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectEllipse_a(), fillAttributes) : 0, sizeof(iVtObjectEllipse_s), 7 /* "Fill Attributes" */, newFillAttributes, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  //  Operation: setOriginSKM
  //! @param b_SKM:
  void setOriginSKM(bool b_SKM);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 15; }
  */

  uint16_t updateLineAttributes(bool b_SendRequest=false);

  uint16_t updateWidth(bool b_SendRequest=false);

  uint16_t updateHeight(bool b_SendRequest=false);

  uint8_t updateEllipseType(bool b_SendRequest=false);

  uint8_t updateStartAngle(bool b_SendRequest=false);

  uint8_t updateEndAngle(bool b_SendRequest=false);

  uint16_t updateFillAttributes(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // end namespace __IsoAgLib

#endif
#endif
