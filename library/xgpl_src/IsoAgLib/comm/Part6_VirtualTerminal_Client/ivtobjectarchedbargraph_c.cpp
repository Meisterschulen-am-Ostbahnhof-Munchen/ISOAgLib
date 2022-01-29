/*
 * ivtobjectarchedbargraph_c.cpp
 *
 *  Created on: 29.01.2022
 *      Author: franz
 */

#include "ivtobjectarchedbargraph_c.h"

namespace IsoAgLib {

	uint16_t iVtObjectArchedBarGraph_c::objectType() {
		return VT_OBJECT_TYPE_ARCHED_BAR_GRAPH;
	}

iVtObjectArchedBarGraph_c::iVtObjectArchedBarGraph_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID,
		uint16_t width,
		uint16_t height,
		uint8_t colour,
		uint8_t targetLineColour,
		uint8_t options,
		uint8_t startAngle,
		uint8_t endAngle,
		uint16_t barGraphWidth,
		uint16_t minValue,
		uint16_t maxValue,
		iVtObject_c *variableReference,
		uint16_t value,
		iVtObject_c *targetValueVariableReference,
		uint16_t targetValue)
	:vtObjectArchedBarGraph_c(
			new iVtObjectArchedBarGraph_s(
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
					targetValue),
	pool->getAiMultitonInst())
	{
		pool->Append(this);
	}

    void iVtObjectArchedBarGraph_c::setValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setWidth(uint16_t newWidth, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setHeight(uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setColour(uint8_t newColour, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setColour (newColour, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setTargetLineColour(uint8_t newTargetLineColour, bool b_updateObject,
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

    void iVtObjectArchedBarGraph_c::setBarGraphWidth(uint16_t newBarGraphWidth, bool b_updateObject,
                                                     bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setBarGraphWidth (newBarGraphWidth, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setMinValue(uint16_t newMin, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setMinValue (newMin, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectArchedBarGraph_c::setMaxValue(uint16_t newMax, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectArchedBarGraph_c::setMaxValue (newMax, b_updateObject, b_enableReplaceOfCmd);
    }

}


