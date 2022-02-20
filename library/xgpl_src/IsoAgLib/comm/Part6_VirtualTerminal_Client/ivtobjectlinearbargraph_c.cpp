/*
 * ivtobjectlinearbargraph_c.cpp
 *
 *  Created on: 30.01.2022
 *      Author: franz
 */


#include "ivtobjectlinearbargraph_c.h"


#ifdef CONFIG_USE_VTOBJECT_linearbargraph

namespace IsoAgLib {


	ObjectType iVtObjectLinearBarGraph_c::objectType() { return VT_OBJECT_TYPE_LINEAR_BAR_GRAPH; }

    iVtObjectLinearBarGraph_c::iVtObjectLinearBarGraph_c(
    		iVtClientObjectPool_c* pool,
    		ObjectID ID,
    		uint16_t width,
			uint16_t height,
			Colour colour,
    		Colour targetLineColour,
			uint8_t options,
			uint8_t numberOfTicks,
    		uint16_t minValue,
			uint16_t maxValue,
			iVtObjectNumberVariable_c *variableReference,
    		uint16_t value,
			iVtObjectNumberVariable_c *targetValueVariableReference,
    		uint16_t targetValue)
    : vtObjectLinearBarGraph_c(
    		pool->getAiMultitonInst(),
    		ID,
    		width,
			height,
			colour,
    		targetLineColour,
			options,
			numberOfTicks,
    		minValue,
			maxValue,
			variableReference,
    		value,
			targetValueVariableReference,
    		targetValue)
    {
		pool->Append(this);
    }

    iVtObjectLinearBarGraph_c::~iVtObjectLinearBarGraph_c() = default;

    void iVtObjectLinearBarGraph_c::setValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLinearBarGraph_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLinearBarGraph_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLinearBarGraph_c::setColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void
    iVtObjectLinearBarGraph_c::setTargetLineColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setTargetLineColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLinearBarGraph_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLinearBarGraph_c::setNumberOfTicks(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setNumberOfTicks (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLinearBarGraph_c::setMinValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setMinValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLinearBarGraph_c::setMaxValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setMaxValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLinearBarGraph_c::setVariableReference(iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLinearBarGraph_c::setTargetValueVariableReference(iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setTargetValueVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLinearBarGraph_c::setTargetValue(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setTargetValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectLinearBarGraph_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject,
                                            bool b_enableReplaceOfCmd) {
        vtObjectLinearBarGraph_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES


    uint16_t iVtObjectLinearBarGraph_c::updateWidth(bool b_SendRequest) {
        return vtObjectLinearBarGraph_c::updateWidth(b_SendRequest);
    }


    uint16_t iVtObjectLinearBarGraph_c::updateHeight(bool b_SendRequest) {
        return vtObjectLinearBarGraph_c::updateHeight(b_SendRequest);
    }

    Colour iVtObjectLinearBarGraph_c::updateColour(bool b_SendRequest) {
        return vtObjectLinearBarGraph_c::updateColour(b_SendRequest);
    }

    Colour iVtObjectLinearBarGraph_c::updateTargetLineColour(bool b_SendRequest) {
        return vtObjectLinearBarGraph_c::updateTargetLineColour(b_SendRequest);
    }

    uint8_t iVtObjectLinearBarGraph_c::updateOptions(bool b_SendRequest) {
        return vtObjectLinearBarGraph_c::updateOptions(b_SendRequest);
    }

    uint8_t iVtObjectLinearBarGraph_c::updateNumberOfTicks(bool b_SendRequest) {
        return vtObjectLinearBarGraph_c::updateNumberOfTicks(b_SendRequest);
    }

    uint16_t iVtObjectLinearBarGraph_c::updateMinValue(bool b_SendRequest) {
        return vtObjectLinearBarGraph_c::updateMinValue(b_SendRequest);
    }

    uint16_t iVtObjectLinearBarGraph_c::updateMaxValue(bool b_SendRequest) {
        return vtObjectLinearBarGraph_c::updateMaxValue(b_SendRequest);
    }

    iVtObjectNumberVariable_c* iVtObjectLinearBarGraph_c::updateVariableReference(bool b_SendRequest) {
        return vtObjectLinearBarGraph_c::updateVariableReference(b_SendRequest);
    }

    iVtObjectNumberVariable_c* iVtObjectLinearBarGraph_c::updateTargetValueVariableReference(bool b_SendRequest) {
        return vtObjectLinearBarGraph_c::updateTargetValueVariableReference(b_SendRequest);
    }

    uint16_t iVtObjectLinearBarGraph_c::updateTargetValue(bool b_SendRequest) {
        return vtObjectLinearBarGraph_c::updateTargetValue(b_SendRequest);
    }
#endif


    ObjectType iVtObjectLinearBarGraph_c::getObjectType() const { return objectType(); }



} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_linearbargraph
