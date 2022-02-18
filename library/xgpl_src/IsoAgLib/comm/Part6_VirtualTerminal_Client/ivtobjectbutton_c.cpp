
/*
 * ivtobjectbutton_c.cpp
 *
 *  Created on: 28.01.2022
 *      Author: franz
 */


#include "ivtobjectbutton_c.h"

#ifdef CONFIG_USE_VTOBJECT_button

namespace IsoAgLib {


	iVtObjectButton_c::iVtObjectButton_c(
			iVtClientObjectPool_c* pool,
			ObjectID ID,
			uint16_t width,
			uint16_t height,
			Colour backgroundColour,
			Colour borderColour,
			uint8_t keyCode,
			uint8_t options)
	: vtObjectButton_c(
						pool->getAiMultitonInst(),
						ID,
						width,
						height,
						backgroundColour,
						borderColour,
						keyCode,
						options)
	{
		pool->Append(this);
	}


    void
    iVtObjectButton_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectButton_c::setSize (newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectButton_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectButton_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectButton_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectButton_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectButton_c::setBackgroundColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectButton_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectButton_c::moveChildLocation(IsoAgLib::iVtObject_c *apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectButton_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectButton_c::getObjectType() const {
        return objectType();
    }



#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
    // ///////////////////////// getter for attributes
    /** that attribute is in parentheses in the spec, so commented out here
    uint8_t updateObjectType() const { return vtObjectButton_c::updateObjectType(); }
    */
    uint16_t iVtObjectButton_c::updateWidth(bool b_SendRequest) {
        return vtObjectButton_c::updateWidth(b_SendRequest);
    }



    uint16_t iVtObjectButton_c::updateHeight(bool b_SendRequest) {
        return vtObjectButton_c::updateHeight(b_SendRequest);
    }

    IsoAgLib::Colour iVtObjectButton_c::updateBackgroundColour(bool b_SendRequest) {
        return vtObjectButton_c::updateBackgroundColour(b_SendRequest);
    }

    IsoAgLib::Colour iVtObjectButton_c::updateBorderColour(bool b_SendRequest) {
        return vtObjectButton_c::updateBorderColour(b_SendRequest);
    }

    uint8_t iVtObjectButton_c::updateOptions(bool b_SendRequest) {
        return vtObjectButton_c::updateOptions(b_SendRequest);
    }

    uint8_t iVtObjectButton_c::updateKeyCode(bool b_SendRequest) {
        return vtObjectButton_c::updateKeyCode(b_SendRequest);
    }

#endif

    bool iVtObjectButton_c::setChildPosition(IsoAgLib::iVtObject_c *apc_childObject, int16_t dx, int16_t dy,
                                             bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectButton_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectButton_c::setKeyCode(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectButton_c::setKeyCode (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectButton_c::setBorderColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectButton_c::setBorderColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectButton_c::v4setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectButton_c::v4setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectButton_c::v4select(uint8_t selectOrActivate) {
    	return vtObject_c::select(selectOrActivate);
    }

    bool iVtObjectButton_c::v4enable(bool b_updateObject, bool b_enableReplaceOfCmd) {
    	return vtObject_c::able (1, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectButton_c::v4disable(bool b_updateObject, bool b_enableReplaceOfCmd) {
    	return vtObject_c::able (0, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectButton_c::objectType() {
    	return VT_OBJECT_TYPE_BUTTON;
    }

    iVtObjectButton_c::~iVtObjectButton_c() = default;

}

#endif //CONFIG_USE_VTOBJECT_button
