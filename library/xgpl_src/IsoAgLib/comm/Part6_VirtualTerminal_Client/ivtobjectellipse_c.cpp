/*
 * ivtobjectellipse_c.cpp
 *
 *  Created on: 30.01.2022
 *      Author: franz
 */


#include "ivtobjectellipse_c.h"


#ifdef CONFIG_USE_VTOBJECT_ellipse

namespace IsoAgLib {


    uint16_t iVtObjectEllipse_c::objectType() { return VT_OBJECT_TYPE_ELLIPSE; }



    iVtObjectEllipse_c::iVtObjectEllipse_c(
    		iVtClientObjectPool_c *pool,
			ObjectID ID,
    		iVtObjectLineAttributes_c *lineAttributes,
			uint16_t width,
    		uint16_t height,
			uint8_t ellipseType,
			uint8_t startAngle,
    		uint8_t endAngle,
			iVtObjectFillAttributes_c *fillAttributes)
    :vtObjectEllipse_c(
			pool->getAiMultitonInst(),
			ID,
    		lineAttributes,
			width,
    		height,
			ellipseType,
			startAngle,
    		endAngle,
			fillAttributes)
	{
		pool->Append(this);
	}



    void iVtObjectEllipse_c::setLineAttributes(iVtObjectLineAttributes_c *newLineAttributes, bool b_updateObject,
                                               bool b_enableReplaceOfCmd) {
        vtObjectEllipse_c::setLineAttributes (newLineAttributes, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectEllipse_c::getObjectType() const { return objectType(); }

    void iVtObjectEllipse_c::setWidth(uint16_t newWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectEllipse_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectEllipse_c::setHeight(uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectEllipse_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectEllipse_c::setEllipseType(uint8_t newEllipseType, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectEllipse_c::setEllipseType (newEllipseType, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectEllipse_c::setStartAngle(uint8_t newStartAngle, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectEllipse_c::setStartAngle (newStartAngle, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectEllipse_c::setEndAngle(uint8_t newEndAngle, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectEllipse_c::setEndAngle (newEndAngle, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectEllipse_c::setFillAttributes(iVtObjectFillAttributes_c *newFillAttributes, bool b_updateObject,
                                               bool b_enableReplaceOfCmd) {
        vtObjectEllipse_c::setFillAttributes (newFillAttributes, b_updateObject, b_enableReplaceOfCmd);
    }

    void
    iVtObjectEllipse_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectEllipse_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES



    iVtObjectLineAttributes_c* iVtObjectEllipse_c::updateLineAttributes(bool b_SendRequest) {
        return vtObjectEllipse_c::updateLineAttributes(b_SendRequest);
    }

    uint16_t iVtObjectEllipse_c::updateWidth(bool b_SendRequest) {
        return vtObjectEllipse_c::updateWidth(b_SendRequest);
    }

    uint16_t iVtObjectEllipse_c::updateHeight(bool b_SendRequest) {
        return vtObjectEllipse_c::updateHeight(b_SendRequest);
    }

    uint8_t iVtObjectEllipse_c::updateEllipseType(bool b_SendRequest) {
        return vtObjectEllipse_c::updateEllipseType(b_SendRequest);
    }

    uint8_t iVtObjectEllipse_c::updateStartAngle(bool b_SendRequest) {
        return vtObjectEllipse_c::updateStartAngle(b_SendRequest);
    }

    uint8_t iVtObjectEllipse_c::updateEndAngle(bool b_SendRequest) {
        return vtObjectEllipse_c::updateEndAngle(b_SendRequest);
    }

    iVtObjectFillAttributes_c* iVtObjectEllipse_c::updateFillAttributes(bool b_SendRequest) {
        return vtObjectEllipse_c::updateFillAttributes(b_SendRequest);
    }

    iVtObjectEllipse_c::~iVtObjectEllipse_c() = default;

#endif






} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_ellipse
