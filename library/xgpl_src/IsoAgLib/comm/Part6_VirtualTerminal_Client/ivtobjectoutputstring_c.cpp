/*
 * ivtobjectoutputstring_c.cpp
 *
 *  Created on: 29.01.2022
 *      Author: franz
 */


#include "ivtobjectoutputstring_c.h"

namespace IsoAgLib {


    iVtObjectOutputString_c::iVtObjectOutputString_c(
    		iVtClientObjectPool_c* pool,
			ObjectID ID,
			uint16_t width,
			uint16_t height,
			Colour backgroundColour,
			iVtObjectFontAttributes_c *fontAttributes,
			uint8_t options,
			iVtObjectStringVariable_c *variableReference,
			uint8_t horizontalJustification,
			char *value)
            :vtObjectOutputString_c(
            		pool->getAiMultitonInst(),
                    ID,
                    width,
                    height,
                    backgroundColour,
                    fontAttributes,
                    options,
                    variableReference,
                    horizontalJustification,
                    value /* size length+1 (0-termination intern!) */
            )
	{
    	pool->Append(this);
	}

    uint16_t iVtObjectOutputString_c::objectType() { return VT_OBJECT_TYPE_OUTPUT_STRING; }

    uint16_t iVtObjectOutputString_c::getObjectType() const { return objectType(); }

    void iVtObjectOutputString_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject,
                                          bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputString_c::setHorizontalJustification(uint8_t newValue, bool b_updateObject,
                                                             bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setHorizontalJustification (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputString_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputString_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputString_c::setValueCopy(char *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setValueCopy (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputString_c::setValueRef(char *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setValueRef (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputString_c::setVariableReference(iVtObjectStringVariable_c *newValue, bool b_updateObject,
                                                       bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    const char *iVtObjectOutputString_c::getString() { return vtObjectOutputString_c::getString(); }

    void iVtObjectOutputString_c::setBackgroundColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void
    iVtObjectOutputString_c::setFontAttributes(iVtObjectFontAttributes_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setFontAttributes (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputString_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
    }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES


    uint16_t iVtObjectOutputString_c::updateWidth(bool b_SendRequest) {
        return vtObjectOutputString_c::updateWidth(b_SendRequest);
    }

    uint16_t iVtObjectOutputString_c::updateHeight(bool b_SendRequest) {
        return vtObjectOutputString_c::updateHeight(b_SendRequest);
    }

    uint8_t iVtObjectOutputString_c::updateBackgroundColour(bool b_SendRequest) {
        return vtObjectOutputString_c::updateBackgroundColour(b_SendRequest);
    }

    uint16_t iVtObjectOutputString_c::updateFontAttributes(bool b_SendRequest) {
        return vtObjectOutputString_c::updateFontAttributes(b_SendRequest);
    }

    uint8_t iVtObjectOutputString_c::updateOptions(bool b_SendRequest) {
        return vtObjectOutputString_c::updateOptions(b_SendRequest);
    }

    uint16_t iVtObjectOutputString_c::updateVariableReference(bool b_SendRequest) {
        return vtObjectOutputString_c::updateVariableReference(b_SendRequest);
    }

    uint8_t iVtObjectOutputString_c::updateJustification(bool b_SendRequest) {
        return vtObjectOutputString_c::updateJustification(b_SendRequest);
    }

#endif



#ifdef USE_VT_UNICODE_SUPPORT
    void iVtObjectOutputString_c::setValueCopyUTF8(const char *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setValueCopyUTF8 (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputString_c::setValueCopyUTF16(const char *newValue, uint16_t length, bool b_updateObject,
                                                    bool b_enableReplaceOfCmd) {
        vtObjectOutputString_c::setValueCopyUTF16 (newValue, length, b_updateObject, b_enableReplaceOfCmd);
    }
#endif



}

