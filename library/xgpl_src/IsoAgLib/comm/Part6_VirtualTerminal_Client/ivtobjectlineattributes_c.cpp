/*
 * ivtobjectlineattributes_c.cpp
 *
 *  Created on: 29.01.2022
 *      Author: franz
 */



#include "ivtobjectlineattributes_c.h"

namespace IsoAgLib {


    uint16_t iVtObjectLineAttributes_c::objectType() { return VT_OBJECT_TYPE_LINE_ATTRIBUTES; }

iVtObjectLineAttributes_c::iVtObjectLineAttributes_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID,
		Colour lineColour,
		uint8_t lineWidth,
		uint16_t lineArt)
:vtObjectLineAttributes_c(
		pool->getAiMultitonInst(),
        ID,
		lineColour,
		lineWidth,
		lineArt)
	{
		pool->Append(this);
	}

    void iVtObjectLineAttributes_c::setLineColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLineAttributes_c::setLineColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLineAttributes_c::setLineWidth(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLineAttributes_c::setLineWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLineAttributes_c::setLineArt(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLineAttributes_c::setLineArt (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLineAttributes_c::setLineAttributes(Colour newLineColour, uint8_t newLineWidth, uint16_t newLineArt,
                                                      bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLineAttributes_c::setLineAttributes (newLineColour, newLineWidth, newLineArt, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectLineAttributes_c::getObjectType() const { return objectType(); }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
    /** that attribute is in parentheses in the spec, so commented out here
    uint8_t updateObjectType() const { return vtObjectLineAttributes_c::updateObjectType(); }
    */
    uint8_t iVtObjectLineAttributes_c::updateLineColour(bool b_SendRequest) {
        return vtObjectLineAttributes_c::updateLineColour(b_SendRequest);
    }

    uint8_t iVtObjectLineAttributes_c::updateLineWidth(bool b_SendRequest) {
        return vtObjectLineAttributes_c::updateLineWidth(b_SendRequest);
    }

    uint16_t iVtObjectLineAttributes_c::updateLineArt(bool b_SendRequest) {
        return vtObjectLineAttributes_c::updateLineArt(b_SendRequest);
    }

#endif


}

