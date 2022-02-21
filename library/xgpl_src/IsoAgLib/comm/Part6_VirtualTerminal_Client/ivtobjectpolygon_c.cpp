/*
 * ivtobjectpolygon_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectpolygon_c.h"

namespace IsoAgLib {


    iVtObjectPolygon_c::~iVtObjectPolygon_c() = default;

    iVtObjectPolygon_c::iVtObjectPolygon_c(
    		iVtClientObjectPool_c *pool,
			ObjectID ID,
			uint16_t width,
			uint16_t height,
			iVtObjectLineAttributes_c *lineAttributes,
			iVtObjectFillAttributes_c *fillAttributes,
			PolygonType polygonType,
			repeat_x_y_s *pointsToFollow)
    :vtObjectPolygon_c(
    		pool->getAiMultitonInst(),
			ID,
			width,
			height,
			lineAttributes,
			fillAttributes,
			polygonType,
			pointsToFollow)
 	{
 		pool->Append(this);
 	}

    ObjectType iVtObjectPolygon_c::objectType() { return VT_OBJECT_TYPE_POLYGON; }

    void iVtObjectPolygon_c::setWidth(uint16_t newWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectPolygon_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectPolygon_c::setHeight(uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectPolygon_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectPolygon_c::setLineAttributes(iVtObjectLineAttributes_c *newLineAttributes, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectPolygon_c::setLineAttributes (newLineAttributes, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectPolygon_c::setFillAttributes(iVtObjectFillAttributes_c *newFillAttributes, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectPolygon_c::setFillAttributes (newFillAttributes, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectPolygon_c::setPolygonType(IsoAgLib::PolygonType newPolygonType, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectPolygon_c::setPolygonType (newPolygonType, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectPolygon_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectPolygon_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectPolygon_c::updateWidth(bool b_SendRequest) {
        return vtObjectPolygon_c::updateWidth(b_SendRequest);
    }

    uint16_t iVtObjectPolygon_c::updateHeight(bool b_SendRequest) {
        return vtObjectPolygon_c::updateHeight(b_SendRequest);
    }

    iVtObjectLineAttributes_c *iVtObjectPolygon_c::updateLineAttributes(bool b_SendRequest) {
        return vtObjectPolygon_c::updateLineAttributes(b_SendRequest);
    }

    iVtObjectFillAttributes_c *iVtObjectPolygon_c::updateFillAttributes(bool b_SendRequest) {
        return vtObjectPolygon_c::updateFillAttributes(b_SendRequest);
    }

    IsoAgLib::PolygonType iVtObjectPolygon_c::updatePolygonType(bool b_SendRequest) {
        return vtObjectPolygon_c::updatePolygonType(b_SendRequest);
    }

    ObjectType iVtObjectPolygon_c::getObjectType() const { return objectType(); }


} // IsoAgLib

