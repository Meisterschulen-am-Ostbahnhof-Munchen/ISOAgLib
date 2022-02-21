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

	~iVtObjectPolygon_c() override;


	explicit iVtObjectPolygon_c(
				iVtClientObjectPool_c* pool,
				ObjectID ID = autoID,
				uint16_t width = 32,
				uint16_t height = 32,
				iVtObjectLineAttributes_c* lineAttributes = nullptr,
				iVtObjectFillAttributes_c* fillAttributes = nullptr,
				PolygonType polygonType = PolygonType::Convex,
				uint8_t numberOfPoints = 0,
				repeat_x_y_s *pointsToFollow = nullptr);


  static ObjectType objectType();
  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setLineAttributes(iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setFillAttributes(iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setPolygonType(IsoAgLib::PolygonType newPolygonType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectPolygon_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) override;
  uint16_t updateHeight(bool b_SendRequest=false) override;
  iVtObjectLineAttributes_c* updateLineAttributes(bool b_SendRequest=false) override;
  iVtObjectFillAttributes_c* updateFillAttributes(bool b_SendRequest=false) override;
  IsoAgLib::PolygonType updatePolygonType(bool b_SendRequest=false) override;
#endif
  ObjectType getObjectType() const override;
};

} // IsoAgLibb

#endif //CONFIG_USE_VTOBJECT_polygon

#endif //IVTOBJECTPOLYGON_C_H
