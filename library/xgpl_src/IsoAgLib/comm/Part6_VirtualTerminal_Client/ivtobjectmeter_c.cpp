/*
 * ivtobjectmeter_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectmeter_c.h"

namespace IsoAgLib {

iVtObjectMeter_c::~iVtObjectMeter_c() = default;

iVtObjectMeter_c::iVtObjectMeter_c(
		iVtClientObjectPool_c* pool,
  		ObjectID ID,
		uint16_t width,
		Colour needleColour,
		Colour borderColour,
		Colour arcAndTickColour,
		iVtObjectMeterOptions options,
		uint8_t numberOfTicks,
		uint8_t startAngle,
		uint8_t endAngle,
		uint16_t minValue,
		uint16_t maxValue,
		iVtObjectNumberVariable_c *variableReference,
		uint16_t value)
:vtObjectMeter_c(
		pool->getAiMultitonInst(),
  		ID,
		width,
		needleColour,
		borderColour,
		arcAndTickColour,
		options,
		numberOfTicks,
		startAngle,
		endAngle,
		minValue,
		maxValue,
		variableReference,
		value)
{
	pool->Append(this);
}

    void iVtObjectMeter_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectMeter_c::setNeedleColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setNeedleColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectMeter_c::setBorderColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setBorderColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectMeter_c::setArcAndTickColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setArcAndTickColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectMeter_c::setOptions(iVtObjectMeterOptions newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectMeter_c::setNumberOfTicks(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setNumberOfTicks (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectMeter_c::setStartAngle(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setStartAngle (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectMeter_c::setEndAngle(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setEndAngle (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectMeter_c::setMin(uint16_t newMin, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setMin (newMin, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectMeter_c::setMax(uint16_t newMax, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setMax (newMax, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectMeter_c::setValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectMeter_c::setVariableReference(iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectMeter_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectMeter_c::updateWidth(bool b_SendRequest) {
        return vtObjectMeter_c::updateWidth(b_SendRequest);
    }

    uint8_t iVtObjectMeter_c::updateNeedleColour(bool b_SendRequest) {
        return vtObjectMeter_c::updateNeedleColour(b_SendRequest);
    }

    uint8_t iVtObjectMeter_c::updateBorderColour(bool b_SendRequest) {
        return vtObjectMeter_c::updateBorderColour(b_SendRequest);
    }

    uint8_t iVtObjectMeter_c::updateArcAndTickColour(bool b_SendRequest) {
        return vtObjectMeter_c::updateArcAndTickColour(b_SendRequest);
    }

    iVtObjectMeterOptions iVtObjectMeter_c::updateOptions(bool b_SendRequest) {
        return vtObjectMeter_c::updateOptions(b_SendRequest);
    }

    uint8_t iVtObjectMeter_c::updateNumberOfTicks(bool b_SendRequest) {
        return vtObjectMeter_c::updateNumberOfTicks(b_SendRequest);
    }

    uint8_t iVtObjectMeter_c::updateStartAngle(bool b_SendRequest) {
        return vtObjectMeter_c::updateStartAngle(b_SendRequest);
    }

    uint8_t iVtObjectMeter_c::updateEndAngle(bool b_SendRequest) {
        return vtObjectMeter_c::updateEndAngle(b_SendRequest);
    }

    uint16_t iVtObjectMeter_c::updateMinValue(bool b_SendRequest) {
        return vtObjectMeter_c::updateMinValue(b_SendRequest);
    }

    uint16_t iVtObjectMeter_c::updateMaxValue(bool b_SendRequest) {
        return vtObjectMeter_c::updateMaxValue(b_SendRequest);
    }

    iVtObjectNumberVariable_c *iVtObjectMeter_c::updateVariableReference(bool b_SendRequest) {
        return vtObjectMeter_c::updateVariableReference(b_SendRequest);
    }

    ObjectType iVtObjectMeter_c::getObjectType() const { return objectType(); }


} // IsoAgLib



