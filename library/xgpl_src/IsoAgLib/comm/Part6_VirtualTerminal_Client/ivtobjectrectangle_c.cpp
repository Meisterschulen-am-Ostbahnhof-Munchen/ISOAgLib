/*
 * ivtobjectrectangle_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */


#include "ivtobjectrectangle_c.h"

namespace IsoAgLib {


    ObjectType iVtObjectRectangle_c::objectType() { return VT_OBJECT_TYPE_RECTANGLE; }

    iVtObjectRectangle_c::~iVtObjectRectangle_c() = default;

    iVtObjectRectangle_c::iVtObjectRectangle_c(
            iVtClientObjectPool_c *pool,
            ObjectID ID, iVtObjectLineAttributes_c *lineAttributes,
            uint16_t width,
            uint16_t height,
            LineSuppression lineSuppression,
            iVtObjectFillAttributes_c *fillAttributes)
    :vtObjectRectangle_c(
    		pool->getAiMultitonInst(),
            ID,
    		lineAttributes,
    		width,
    		height,
			lineSuppression,
			fillAttributes)
    	{
    		pool->Append(this);
    	}

    void iVtObjectRectangle_c::setWidth(uint16_t newWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectRectangle_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectRectangle_c::setHeight(uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectRectangle_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectRectangle_c::setLineAttributes(iVtObjectLineAttributes_c *newLineAttributes, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectRectangle_c::setLineAttributes (newLineAttributes, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectRectangle_c::setLineSuppression(LineSuppression newLineSupressionValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectRectangle_c::setLineSuppression (newLineSupressionValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectRectangle_c::setFillAttributes(iVtObjectFillAttributes_c *newFillAttribute, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectRectangle_c::setFillAttributes (newFillAttribute, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectRectangle_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectRectangle_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    iVtObjectLineAttributes_c *iVtObjectRectangle_c::updateLineAttributes(bool b_SendRequest) {
        return vtObjectRectangle_c::updateLineAttributes(b_SendRequest);
    }

    uint16_t iVtObjectRectangle_c::updateWidth(bool b_SendRequest) {
        return vtObjectRectangle_c::updateWidth(b_SendRequest);
    }

    uint16_t iVtObjectRectangle_c::updateHeight(bool b_SendRequest) {
        return vtObjectRectangle_c::updateHeight(b_SendRequest);
    }

    LineSuppression iVtObjectRectangle_c::updateLineSuppression(bool b_SendRequest) {
        return vtObjectRectangle_c::updateLineSuppression(b_SendRequest);
    }

    iVtObjectFillAttributes_c *iVtObjectRectangle_c::updateFillAttributes(bool b_SendRequest) {
        return vtObjectRectangle_c::updateFillAttributes(b_SendRequest);
    }

    ObjectType iVtObjectRectangle_c::getObjectType() const { return objectType(); }


} // IsoAgLib
