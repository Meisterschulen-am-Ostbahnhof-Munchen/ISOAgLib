/*
  vtobjectpolygon_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTPOLYGON_C_H
#define VTOBJECTPOLYGON_C_H

#include <IsoAgLib/typedef.h>

#ifdef USE_VTOBJECT_polygon
#include "vtobject_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectPolygon_c : public vtObject_c
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
  //! @param vtObjectpolygonSROM:
  //! @param b_initPointer:
  void init(const iVtObjectPolygon_s* vtObjectPolygonSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectPolygonSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectPolygon_a
  iVtObjectPolygon_s* get_vtObjectPolygon_a() { return (iVtObjectPolygon_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectPolygon_c
  vtObjectPolygon_c();

  //  Operation: fitTerminal
  uint32_t fitTerminal() const;

  //  Operation: setWidth
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectPolygon_a(), width) : 0, sizeof(iVtObjectPolygon_s), 1 /* "Width" */, newWidth, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectPolygon_a(), height) : 0, sizeof(iVtObjectPolygon_s), 2 /* "Height" */, newHeight, b_enableReplaceOfCmd);
  }

  //  Operation: setLineAttribute
  //! @param newLineAttribute:
  //! @param b_updateObject:
  void setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectPolygon_a(), lineAttributes) : 0, sizeof(iVtObjectPolygon_s), 3 /* "Line Attribute" */, (IsoAgLib::iVtObject_c*) newLineAttributes, b_enableReplaceOfCmd);
  }

  //  Operation: setFillAttributes
  //! @param newFillAttributes:
  //! @param b_updateObject:
  void setFillAttributes(IsoAgLib::iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectPolygon_a(), fillAttributes) : 0, sizeof(iVtObjectPolygon_s), 4 /* "Fill Attribute" */, (IsoAgLib::iVtObject_c*) newFillAttributes, b_enableReplaceOfCmd);
  }

  //  Operation: setPolygonType
  //! @param newPolygonType:
  //! @param b_updateObject:
  void setPolygonType(uint8_t newPolygonType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectPolygon_a(), polygonType) : 0, sizeof(iVtObjectPolygon_s), 5 /* "Polygon Type" */, newPolygonType, newPolygonType, b_enableReplaceOfCmd);
  }

  //  Operation: setOriginSKM
  //! @param b_SKM:
  void setOriginSKM(bool b_SKM);

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 16; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);

  uint16_t updateHeight(bool b_SendRequest=false);

  uint16_t updateLineAttributes(bool b_SendRequest=false);

  uint16_t updateFillAttributes(bool b_SendRequest=false);

  uint8_t updatePolygonType(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // end namepsace __IsoAgLibb

#endif
#endif
