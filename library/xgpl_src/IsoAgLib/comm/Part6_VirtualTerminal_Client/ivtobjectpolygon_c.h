/*
  ivtobjectpolygon_c.h

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTPOLYGON_C_H
#define IVTOBJECTPOLYGON_C_H

#include "impl/vtobjectpolygon_c.h"


#ifdef CONFIG_USE_VTOBJECT_polygon

namespace IsoAgLib {

class iVtObjectPolygon_c : public __IsoAgLib::vtObjectPolygon_c
{
public:

	iVtObjectPolygon_c() = delete;

	~iVtObjectPolygon_c();


	iVtObjectPolygon_c(
				iVtClientObjectPool_c* pool,
				ObjectID ID = autoID,
				uint16_t width = 32,
				uint16_t height = 32,
				iVtObjectLineAttributes_c* lineAttributes = nullptr,
				iVtObjectFillAttributes_c* fillAttributes = nullptr,
				PolygonType polygonType = PolygonType::Convex,
				uint8_t numberOfPoints = 0,
				const repeat_x_y_s *pointsToFollow = nullptr);


  static ObjectType objectType() { return VT_OBJECT_TYPE_POLYGON; }


  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  void setLineAttributes(iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setLineAttributes (newLineAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  void setFillAttributes(iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setFillAttributes (newFillAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  void setPolygonType(IsoAgLib::PolygonType newPolygonType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setPolygonType (newPolygonType, b_updateObject, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectPolygon_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateHeight(b_SendRequest);
  }

  iVtObjectLineAttributes_c* updateLineAttributes(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateLineAttributes(b_SendRequest);
  }

  iVtObjectFillAttributes_c* updateFillAttributes(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateFillAttributes(b_SendRequest);
  }

  IsoAgLib::PolygonType updatePolygonType(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updatePolygonType(b_SendRequest);
  }
#endif

  virtual ObjectType getObjectType() const { return objectType(); }
};

} // IsoAgLibb

#endif //CONFIG_USE_VTOBJECT_polygon

#endif //IVTOBJECTPOLYGON_C_H
