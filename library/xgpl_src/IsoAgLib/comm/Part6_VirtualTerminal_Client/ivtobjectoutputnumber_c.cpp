/*
 * ivtobjectoutputnumber_c.cpp
 *
 *  Created on: 30.01.2022
 *      Author: franz
 */


#include "ivtobjectoutputnumber_c.h"



#ifdef CONFIG_USE_VTOBJECT_outputnumber


namespace IsoAgLib {


iVtObjectOutputNumber_c::iVtObjectOutputNumber_c(
	iVtClientObjectPool_c* pool,
	ObjectID ID,
	uint16_t width,
	uint16_t height,
	Colour backgroundColour,
	iVtObjectFontAttributes_c *fontAttributes,
	iVtObjectNumberOptions options,
	iVtObjectNumberVariable_c *variableReference,
	uint32_t value,
	int32_t offset,
	float scale,
	uint8_t numberOfDecimals,
	uint8_t format,
	Justification justification)
    	    :vtObjectOutputNumber_c(
    				pool->getAiMultitonInst(),
    				ID,
    				width,
    				height,
    				backgroundColour,
    				fontAttributes,
    				options,
    				variableReference,
    				value,
    				offset,
    				scale,
    				numberOfDecimals,
    				format,
    				justification)
    		{
    			pool->Append(this);
    		}



	ObjectType iVtObjectOutputNumber_c::objectType() { return VT_OBJECT_TYPE_OUTPUT_NUMBER; }

    void iVtObjectOutputNumber_c::setValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setBackgroundColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setFontAttributes(iVtObjectFontAttributes_c *newValue, bool b_updateObject,
                                                    bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setFontAttributes (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setOptions(iVtObjectNumberOptions newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setVariableReference(iVtObjectNumberVariable_c *newValue, bool b_updateObject,
                                                       bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    ObjectType iVtObjectOutputNumber_c::getObjectType() const { return objectType(); }

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

    void iVtObjectOutputNumber_c::setJustification(Justification newValue, bool b_updateObject,
                                                             bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setJustification (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectOutputNumber_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject,
                                          bool b_enableReplaceOfCmd) {
        vtObjectOutputNumber_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
    uint16_t iVtObjectOutputNumber_c::updateWidth(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateWidth(b_SendRequest);
    }

    uint16_t iVtObjectOutputNumber_c::updateHeight(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateHeight(b_SendRequest);
    }

    Colour iVtObjectOutputNumber_c::updateBackgroundColour(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateBackgroundColour(b_SendRequest);
    }

    iVtObjectFontAttributes_c* iVtObjectOutputNumber_c::updateFontAttributes(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateFontAttributes(b_SendRequest);
    }

    iVtObjectNumberOptions iVtObjectOutputNumber_c::updateOptions(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateOptions(b_SendRequest);
    }

    iVtObjectNumberVariable_c *iVtObjectOutputNumber_c::updateVariableReference(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateVariableReference(b_SendRequest);
    }

    int32_t iVtObjectOutputNumber_c::updateOffset(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateOffset(b_SendRequest);
    }

    float iVtObjectOutputNumber_c::updateScale(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateScale(b_SendRequest);
    }

    uint8_t iVtObjectOutputNumber_c::updateNumberOfDecimals(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateNumberOfDecimals(b_SendRequest);
    }

    uint8_t iVtObjectOutputNumber_c::updateFormat(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateFormat(b_SendRequest);
    }

    IsoAgLib::Justification iVtObjectOutputNumber_c::updateJustification(bool b_SendRequest) {
        return vtObjectOutputNumber_c::updateJustification(b_SendRequest);
    }

    iVtObjectOutputNumber_c::~iVtObjectOutputNumber_c() = default;

#endif

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_outputnumber
