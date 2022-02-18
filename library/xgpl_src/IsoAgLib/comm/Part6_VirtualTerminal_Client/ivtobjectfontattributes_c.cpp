/*
 * ivtobjectfontattributes_c.cpp
 *
 *  Created on: 28.01.2022
 *      Author: franz
 */

#include "ivtobjectfontattributes_c.h"

namespace IsoAgLib {
	// implement here a normal constructor and functions, as the compiler dislikes inlining of that simple
	// cconstructor/functions direct in scope of iVtObjectFontAttributes_c

	iVtObjectFontAttributes_c::~iVtObjectFontAttributes_c() = default;

	uint16_t iVtObjectFontAttributes_c::getScaledWidthHeight() {
		return vtObjectFontAttributes_c::getScaledWidthHeight();
	}



	iVtObjectFontAttributes_c::iVtObjectFontAttributes_c(
			iVtClientObjectPool_c* pool,
			ObjectID ID,
			Colour fontColour,
			Font fontSize,
			uint8_t fontType, // always =0 ISO_LATIN_1
			uint8_t fontStyle)
	:vtObjectFontAttributes_c(
			        pool->getAiMultitonInst(),
					ID,
					fontColour,
					fontSize,
					fontType, // always =0 ISO_LATIN_1
					fontStyle)
	    {
	    	pool->Append(this);
	    }


    uint16_t iVtObjectFontAttributes_c::objectType() { return VT_OBJECT_TYPE_FONT_ATTRIBUTES; }

    uint16_t iVtObjectFontAttributes_c::getObjectType() const { return objectType(); }

    void iVtObjectFontAttributes_c::setFontColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectFontAttributes_c::setFontColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectFontAttributes_c::setFontSize(Font newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectFontAttributes_c::setFontSize (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectFontAttributes_c::setFontType(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectFontAttributes_c::setFontType(newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectFontAttributes_c::setFontStyle(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectFontAttributes_c::setFontStyle(newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectFontAttributes_c::setFontAttributes(Colour newFontColour, Font newFontSize, uint8_t newFontType,
                                                      uint8_t newFontStyle, bool b_updateObject,
                                                      bool b_enableReplaceOfCmd) {
        vtObjectFontAttributes_c::setFontAttributes(newFontColour, newFontSize, newFontType, newFontStyle, b_updateObject, b_enableReplaceOfCmd);
    }
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
    uint8_t iVtObjectFontAttributes_c::updateFontColour(bool b_SendRequest) {
        return vtObjectFontAttributes_c::updateFontColour(b_SendRequest);
    }

    uint8_t iVtObjectFontAttributes_c::updateFontSize(bool b_SendRequest) {
        return vtObjectFontAttributes_c::updateFontSize(b_SendRequest);
    }

    uint8_t iVtObjectFontAttributes_c::updateFontType(bool b_SendRequest) {
        return vtObjectFontAttributes_c::updateFontType(b_SendRequest);
    }

    uint8_t iVtObjectFontAttributes_c::updateFontStyle(bool b_SendRequest) {
        return vtObjectFontAttributes_c::updateFontStyle(b_SendRequest);
    }


#endif

}
