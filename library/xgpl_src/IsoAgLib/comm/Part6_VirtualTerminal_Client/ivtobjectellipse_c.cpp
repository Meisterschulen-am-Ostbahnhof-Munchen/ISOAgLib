/*
 * ivtobjectellipse_c.cpp
 *
 *  Created on: 30.01.2022
 *      Author: franz
 */


#include "ivtobjectellipse_c.h"


namespace IsoAgLib {


    uint16_t iVtObjectEllipse_c::objectType() { return VT_OBJECT_TYPE_ELLIPSE; }

    iVtObjectEllipse_c::iVtObjectEllipse_c(iVtObject_c::iVtObjectEllipse_s *vtObjectellipseSROM,
                                           int ai_multitonInst)
            :vtObjectEllipse_c(vtObjectellipseSROM , ai_multitonInst)
    {}

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


#ifdef USE_ISO_TERMINAL_GETATTRIBUTES



    uint16_t iVtObjectEllipse_c::updateLineAttributes(bool b_SendRequest) {
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

    uint16_t iVtObjectEllipse_c::updateFillAttributes(bool b_SendRequest) {
        return vtObjectEllipse_c::updateFillAttributes(b_SendRequest);
    }
#endif


	const iVtObject_c::iVtObjectEllipse_s& iVtObjectEllipse_c::get_vtObjectEllipse_a() {
		return *vtObjectEllipse_c::get_vtObjectEllipse_a();
	}


}
