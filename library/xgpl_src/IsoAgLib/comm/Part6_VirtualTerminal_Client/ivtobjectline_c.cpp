/*
 * ivtobjectline_c.cpp
 *
 *  Created on: 29.01.2022
 *      Author: franz
 */



#include "ivtobjectline_c.h"


#ifdef CONFIG_USE_VTOBJECT_line

namespace IsoAgLib {


	ObjectType iVtObjectLine_c::objectType() { return VT_OBJECT_TYPE_LINE; }


    iVtObjectLine_c::~iVtObjectLine_c() = default;

iVtObjectLine_c::iVtObjectLine_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID,
		iVtObjectLineAttributes_c *lineAttributes,
		uint16_t width,
		uint16_t height,
		uint8_t lineDirection)
:vtObjectLine_c(
		pool->getAiMultitonInst(),
        ID,
		lineAttributes,
		width,
		height,
		lineDirection)
	{
		pool->Append(this);
	}

    void iVtObjectLine_c::setLineAttributes(iVtObjectLineAttributes_c *newValue, bool b_updateObject,
                                            bool b_enableReplaceOfCmd) {
        vtObjectLine_c::setLineAttributes(newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLine_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLine_c::setWidth(newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLine_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLine_c::setHeight(newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLine_c::setLineDirection(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLine_c::setLineDirection(newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void
    iVtObjectLine_c::setEndPoint(uint16_t newWidth, uint16_t newHeight, uint8_t newLineDirection, bool b_updateObject,
                                 bool b_enableReplaceOfCmd) {
        vtObjectLine_c::setEndPoint(newWidth, newHeight, newLineDirection, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLine_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLine_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    ObjectType iVtObjectLine_c::getObjectType() const { return objectType(); }


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES


    iVtObjectLineAttributes_c* iVtObjectLine_c::updateLineAttributes(bool b_SendRequest) {
        return vtObjectLine_c::updateLineAttributes(b_SendRequest);
    }

    uint16_t iVtObjectLine_c::updateWidth(bool b_SendRequest) {
        return vtObjectLine_c::updateWidth(b_SendRequest);
    }

    uint16_t iVtObjectLine_c::updateHeight(bool b_SendRequest) {
        return vtObjectLine_c::updateHeight(b_SendRequest);
    }

    uint8_t iVtObjectLine_c::updateLineDirection(bool b_SendRequest) {
        return vtObjectLine_c::updateLineDirection(b_SendRequest);
    }


#endif


} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_line

