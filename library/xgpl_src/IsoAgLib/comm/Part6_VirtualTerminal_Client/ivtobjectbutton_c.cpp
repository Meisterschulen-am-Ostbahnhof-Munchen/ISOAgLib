
/*
 * ivtobjectbutton_c.cpp
 *
 *  Created on: 28.01.2022
 *      Author: franz
 */


#include "ivtobjectbutton_c.h"

namespace IsoAgLib {


	iVtObjectButton_c::iVtObjectButton_c(
			int ai_multitonInst,
			ObjectID ID,
			uint16_t width,
			uint16_t height,
			uint8_t backgroundColour,
			uint8_t borderColour,
			uint8_t keyCode,
			uint8_t options,
			uint8_t numberOfMacrosToFollow,
			const repeat_event_iVtObjectMacro_s *macrosToFollow)
	: vtObjectButton_c(
			  new iVtObjectButton_s(
						ID,
						width,
						height,
						backgroundColour,
						borderColour,
						keyCode,
						options,
						numberOfMacrosToFollow,
						macrosToFollow)
					  , ai_multitonInst)
	{}


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

    void iVtObjectButton_c::setBackgroundColour(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectButton_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectButton_c::moveChildLocation(IsoAgLib::iVtObject_c *apc_childObject, int8_t dx, int8_t dy,
                                              bool b_updateObject) {
        return vtObjectButton_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
    }

    uint16_t iVtObjectButton_c::getObjectType() const { return objectType(); }



#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
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



    uint8_t iVtObjectButton_c::updateBackgroundColour(bool b_SendRequest) {
        return vtObjectButton_c::updateBackgroundColour(b_SendRequest);
    }

    uint8_t iVtObjectButton_c::updateBorderColour(bool b_SendRequest) {
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

    void iVtObjectButton_c::setBorderColour(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectButton_c::setBorderColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectButton_c::v4setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectButton_c::v4setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectButton_c::v4select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }

    bool iVtObjectButton_c::v4enable(bool b_updateObject, bool b_enableReplaceOfCmd) { return vtObject_c::able (1, b_updateObject, b_enableReplaceOfCmd); }

    bool iVtObjectButton_c::v4disable(bool b_updateObject, bool b_enableReplaceOfCmd) { return vtObject_c::able (0, b_updateObject, b_enableReplaceOfCmd); }

    iVtObjectButton_c::iVtObjectButton_c(const iVtObject_c::iVtObjectButton_s *vtObjectButtonSROM, int ai_multitonInst)
            :vtObjectButton_c(vtObjectButtonSROM , ai_multitonInst)
    {}

    uint16_t iVtObjectButton_c::objectType() { return VT_OBJECT_TYPE_BUTTON; }


}
