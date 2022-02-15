/*
 * ivtobjectarchedbargraph_c.cpp
 *
 *  Created on: 29.01.2022
 *      Author: franz
 */

#include "ivtobjectarchedbargraph_c.h"


#ifdef CONFIG_USE_VTOBJECT_archedbargraph

namespace IsoAgLib {

	uint16_t iVtObjectArchedBarGraph_c::objectType() {
		return VT_OBJECT_TYPE_ARCHED_BAR_GRAPH;
	}

iVtObjectArchedBarGraph_c::iVtObjectArchedBarGraph_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID,
		uint16_t width,
		uint16_t height,
		Colour colour,
		Colour targetLineColour,
		uint8_t options,
		uint8_t startAngle,
		uint8_t endAngle,
		uint16_t barGraphWidth,
		uint16_t minValue,
		uint16_t maxValue,
		iVtObjectNumberVariable_c *variableReference,
		uint16_t value,
		iVtObjectNumberVariable_c *targetValueVariableReference,
		uint16_t targetValue)
	:vtObjectArchedBarGraph_c(
					pool->getAiMultitonInst(),
					ID,
					width,
					height,
					colour,
					targetLineColour,
					options,
					startAngle,
					endAngle,
					barGraphWidth,
					minValue,
					maxValue,
					variableReference,
					value,
					targetValueVariableReference,
					targetValue)
	{
		pool->Append(this);
	}

    iVtObjectArchedBarGraph_c::~iVtObjectArchedBarGraph_c() = default;

    void iVtObjectArchedBarGraph_c::setValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setWidth(uint16_t newWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setHeight(uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setColour(Colour newColour, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setColour (newColour, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setTargetLineColour(Colour newTargetLineColour, bool b_updateObject,
                                                        bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setTargetLineColour (newTargetLineColour, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setOptions(uint8_t newOptions, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setOptions (newOptions, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setStartAngle(uint8_t newStartAngle, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setStartAngle (newStartAngle, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setEndAngle(uint8_t newEndAngle, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setEndAngle (newEndAngle, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setBarGraphWidth(uint16_t newBarGraphWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setBarGraphWidth (newBarGraphWidth, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setMinValue(uint16_t newMin, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setMinValue (newMin, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setMaxValue(uint16_t newMax, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setMaxValue (newMax, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setVariableReference(iVtObjectNumberVariable_c *newVariableReference, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setVariableReference (newVariableReference, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setTargetValueVariableReference(iVtObjectNumberVariable_c *newTargetValueVariableReference, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setTargetValueVariableReference (newTargetValueVariableReference, b_updateObject, b_enableReplaceOfCmd);
    }

    void
    iVtObjectArchedBarGraph_c::setTargetValue(uint16_t newTargetValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setTargetValue (newTargetValue, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectArchedBarGraph_c::getObjectType() const { return objectType(); }

    void iVtObjectArchedBarGraph_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES


    uint16_t iVtObjectArchedBarGraph_c::updateWidth(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateWidth(b_SendRequest);
    }

    uint16_t iVtObjectArchedBarGraph_c::updateHeight(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateHeight(b_SendRequest);
    }

    Colour iVtObjectArchedBarGraph_c::updateColour(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateColour(b_SendRequest);
    }

    Colour iVtObjectArchedBarGraph_c::updateTargetLineColour(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateTargetLineColour(b_SendRequest);
    }

    uint8_t iVtObjectArchedBarGraph_c::updateOptions(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateOptions(b_SendRequest);
    }

    uint8_t iVtObjectArchedBarGraph_c::updateStartAngle(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateStartAngle(b_SendRequest);
    }

    uint8_t iVtObjectArchedBarGraph_c::updateEndAngle(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateEndAngle(b_SendRequest);
    }

    uint16_t iVtObjectArchedBarGraph_c::updateBarGraphWidth(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateBarGraphWidth(b_SendRequest);
    }

    uint16_t iVtObjectArchedBarGraph_c::updateMinValue(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateMinValue(b_SendRequest);
    }

    uint16_t iVtObjectArchedBarGraph_c::updateMaxValue(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateMaxValue(b_SendRequest);
    }

    IsoAgLib::iVtObjectNumberVariable_c * iVtObjectArchedBarGraph_c::updateVariableReference(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateVariableReference(b_SendRequest);
    }

    IsoAgLib::iVtObjectNumberVariable_c * iVtObjectArchedBarGraph_c::updateTargetValueVariableReference(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateTargetValueVariableReference(b_SendRequest);
    }

    uint16_t iVtObjectArchedBarGraph_c::updateTargetValue(bool b_SendRequest) {
        return vtObjectArchedBarGraph_c::updateTargetValue(b_SendRequest);
    }


#endif

} // IsoAgLib

#endif // CONFIG_USE_VTOBJECT_archedbargraph


