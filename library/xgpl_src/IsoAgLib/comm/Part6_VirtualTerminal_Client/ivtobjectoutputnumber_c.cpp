/*
 * ivtobjectoutputnumber_c.cpp
 *
 *  Created on: 30.01.2022
 *      Author: franz
 */


#include "ivtobjectoutputnumber_c.h"


namespace IsoAgLib {
    uint16_t iVtObjectOutputNumber_c::objectType() { return VT_OBJECT_TYPE_OUTPUT_NUMBER; }

    void iVtObjectOutputNumber_c::setValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setBackgroundColour(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setFontAttributes(iVtObjectFontAttributes_c *newValue, bool b_updateObject,
                                                    bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setFontAttributes (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setVariableReference(iVtObjectNumberVariable_c *newValue, bool b_updateObject,
                                                       bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectOutputNumber_c::getObjectType() const { return objectType(); }

    void iVtObjectOutputNumber_c::setOffset(int32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setOffset (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setScale(float newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setScale (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setNumberOfDecimals(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setNumberOfDecimals (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setFormat(bool newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setFormat (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setHorizontalJustification(uint8_t newValue, bool b_updateObject,
                                                             bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setHorizontalJustification (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject,
                                          bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectOutputNumber_c::updateWidth(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateWidth(b_SendRequest);
    }

    uint16_t iVtObjectOutputNumber_c::updateHeight(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateHeight(b_SendRequest);
    }

    uint8_t iVtObjectOutputNumber_c::updateBackgroundColour(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateBackgroundColour(b_SendRequest);
    }

    uint16_t iVtObjectOutputNumber_c::updateFontAttributes(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateFontAttributes(b_SendRequest);
    }


}