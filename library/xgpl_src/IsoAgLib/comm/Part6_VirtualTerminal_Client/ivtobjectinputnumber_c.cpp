/*
 * ivtobjectinputnumber_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectinputnumber_c.h"


namespace IsoAgLib {


    iVtObjectInputNumber_c::~iVtObjectInputNumber_c() = default;

    iVtObjectInputNumber_c::iVtObjectInputNumber_c(
    		iVtClientObjectPool_c *pool,
			ObjectID ID,
			uint16_t width,
			uint16_t height,
			Colour backgroundColour,
			iVtObjectFontAttributes_c *fontAttributes,
			iVtObjectInputNumberOptions options,
			iVtObjectNumberVariable_c *variableReference,
			uint32_t value,
			uint32_t minValue,
			uint32_t maxValue,
			int32_t offset,
			float scale,
			uint8_t numberOfDecimals,
			uint8_t format,
			uint8_t justification,
			iVtObjectInputNumberOptions2 secondOptionsByte)
    :vtObjectInputNumber_c(
			pool->getAiMultitonInst(),
			ID,
			width,
			height,
			backgroundColour,
			fontAttributes,
			options,
			variableReference,
			value,
			minValue,
			maxValue,
			offset,
			scale,
			numberOfDecimals,
			format,
			justification,
			secondOptionsByte)
	{
		pool->Append(this);
	}

    ObjectType iVtObjectInputNumber_c::objectType() { return VT_OBJECT_TYPE_INPUT_NUMBER; }

    void iVtObjectInputNumber_c::setValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setBackgroundColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setFontAttributes(iVtObjectFontAttributes_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setFontAttributes (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setOptions(iVtObjectInputNumberOptions newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setVariableReference(iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setMinValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setMinValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setMaxValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setMaxValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setOffset(int32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setOffset (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setScale(float newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setScale (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setNumberOfDecimals(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setNumberOfDecimals (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setFormat(bool newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setFormat (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setHorizontalJustification(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setHorizontalJustification (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setSecondOptionsByte(iVtObjectInputNumberOptions2 newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setSecondOptionsByte (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputNumber_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputNumber_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectInputNumber_c::enable(bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObject_c::able (enabled , b_updateObject, b_enableReplaceOfCmd); }

    bool iVtObjectInputNumber_c::disable(bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObject_c::able (disabled , b_updateObject, b_enableReplaceOfCmd); }

    Enabled iVtObjectInputNumber_c::getEnabled() {
        return vtObjectInputNumber_c::getEnabled(); }

    bool iVtObjectInputNumber_c::select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }

    uint16_t iVtObjectInputNumber_c::updateWidth(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateWidth(b_SendRequest);
    }

    uint16_t iVtObjectInputNumber_c::updateHeight(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateHeight(b_SendRequest);
    }

    Colour iVtObjectInputNumber_c::updateBackgroundColour(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateBackgroundColour(b_SendRequest);
    }

    iVtObjectFontAttributes_c *iVtObjectInputNumber_c::updateFontAttributes(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateFontAttributes(b_SendRequest);
    }

    iVtObjectInputNumberOptions iVtObjectInputNumber_c::updateOptions(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateOptions(b_SendRequest);
    }

    iVtObjectNumberVariable_c *iVtObjectInputNumber_c::updateVariableReference(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateVariableReference(b_SendRequest);
    }

    uint32_t iVtObjectInputNumber_c::updateMinValue(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateMinValue(b_SendRequest);
    }

    uint32_t iVtObjectInputNumber_c::updateMaxValue(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateMaxValue(b_SendRequest);
    }

    int32_t iVtObjectInputNumber_c::updateOffset(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateOffset(b_SendRequest);
    }

    float iVtObjectInputNumber_c::updateScale(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateScale(b_SendRequest);
    }

    uint8_t iVtObjectInputNumber_c::updateNumberOfDecimals(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateNumberOfDecimals(b_SendRequest);
    }

    uint8_t iVtObjectInputNumber_c::updateFormat(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateFormat(b_SendRequest);
    }

    uint8_t iVtObjectInputNumber_c::updateJustification(bool b_SendRequest) {
        return vtObjectInputNumber_c::updateJustification(b_SendRequest);
    }

    ObjectType iVtObjectInputNumber_c::getObjectType() const { return objectType(); }
} // IsoAgLib



