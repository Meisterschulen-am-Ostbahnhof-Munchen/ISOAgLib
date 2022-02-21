/*
 * ivtobjectinputstring_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectinputstring_c.h"


namespace IsoAgLib {

iVtObjectInputString_c::iVtObjectInputString_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID,
		uint16_t width,
		uint16_t height,
		Colour backgroundColour,
		iVtObjectFontAttributes_c *fontAttributes,
		iVtObjectInputAttributes_c* inputAttributes,
		iVtObjectStringOptions options,
		iVtObjectStringVariable_c *variableReference,
		Justification justification,
		char *value,
		Enabled enabled)
        :vtObjectInputString_c(
        		pool->getAiMultitonInst(),
                ID,
                width,
                height,
                backgroundColour,
                fontAttributes,
				inputAttributes,
                options,
                variableReference,
				justification,
                value, /* size length+1 (0-termination intern!) */
				enabled
        )
{
	pool->Append(this);
}

    ObjectType iVtObjectInputString_c::objectType() { return VT_OBJECT_TYPE_INPUT_STRING; }

    const char *iVtObjectInputString_c::getString() { return vtObjectInputString_c::getString(); }

    void iVtObjectInputString_c::setValueRef(char *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputString_c::setValueRef (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputString_c::setValueCopy(char *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputString_c::setValueCopy (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputString_c::setVariableReference(iVtObjectStringVariable_c *newVariable, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputString_c::setVariableReference (newVariable, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputString_c::setBackgroundColour(Colour colorValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputString_c::setBackgroundColour (colorValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputString_c::setWidth(uint16_t newWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputString_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputString_c::setHeight(uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputString_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputString_c::setFontAttributes(iVtObjectFontAttributes_c *newFontAttributes, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputString_c::setFontAttributes (newFontAttributes, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputString_c::setInputAttributes(iVtObjectInputAttributes_c *newInputAttributes, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputString_c::setInputAttributes (newInputAttributes, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputString_c::setOptions(iVtObjectStringOptions newOptions, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputString_c::setOptions (newOptions, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputString_c::setJustification(Justification newJustification, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputString_c::setJustification (newJustification, b_updateObject, b_enableReplaceOfCmd);
    }

    iVtObjectInputString_c::~iVtObjectInputString_c() = default;


    bool iVtObjectInputString_c::enable(bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObject_c::able (enabled , b_updateObject, b_enableReplaceOfCmd); }

    bool iVtObjectInputString_c::disable(bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObject_c::able (disabled , b_updateObject, b_enableReplaceOfCmd); }

    Enabled iVtObjectInputString_c::getEnabled() {
        return vtObjectInputString_c::getEnabled(); }

    bool iVtObjectInputString_c::select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }

    uint16_t iVtObjectInputString_c::updateWidth(bool b_SendRequest) {
        return vtObjectInputString_c::updateWidth(b_SendRequest);
    }

    uint16_t iVtObjectInputString_c::updateHeight(bool b_SendRequest) {
        return vtObjectInputString_c::updateHeight(b_SendRequest);
    }

    Colour iVtObjectInputString_c::updateBackgroundColour(bool b_SendRequest) {
        return vtObjectInputString_c::updateBackgroundColour(b_SendRequest);
    }

    iVtObjectFontAttributes_c *iVtObjectInputString_c::updateFontAttributes(bool b_SendRequest) {
        return vtObjectInputString_c::updateFontAttributes(b_SendRequest);
    }

    iVtObjectInputAttributes_c *iVtObjectInputString_c::updateInputAttributes(bool b_SendRequest) {
        return vtObjectInputString_c::updateInputAttributes(b_SendRequest);
    }

    iVtObjectStringOptions iVtObjectInputString_c::updateOptions(bool b_SendRequest) {
        return vtObjectInputString_c::updateOptions(b_SendRequest);
    }

    iVtObjectStringVariable_c *iVtObjectInputString_c::updateVariableReference(bool b_SendRequest) {
        return vtObjectInputString_c::updateVariableReference(b_SendRequest);
    }

    Justification iVtObjectInputString_c::updateJustification(bool b_SendRequest) {
        return vtObjectInputString_c::updateJustification(b_SendRequest);
    }

    ObjectType iVtObjectInputString_c::getObjectType() const { return objectType(); }

} // IsoAgLib



