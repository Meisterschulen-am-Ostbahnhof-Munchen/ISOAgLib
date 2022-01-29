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
	iVtObjectFontAttributes_c::iVtObjectFontAttributes_c() :
			vtObjectFontAttributes_c() {
	}

	iVtObjectFontAttributes_c::~iVtObjectFontAttributes_c() {
	}

	uint16_t iVtObjectFontAttributes_c::getScaledWidthHeight() {
		return vtObjectFontAttributes_c::getScaledWidthHeight();
	}

	iVtObjectFontAttributes_c::iVtObjectFontAttributes_c(const iVtObjectFontAttributes_s *vtObjectFontAttributesSROM, int ai_multitonInst) :
			vtObjectFontAttributes_c(vtObjectFontAttributesSROM, ai_multitonInst) {

	}

	void iVtObjectFontAttributes_c::init(const iVtObjectFontAttributes_s *vtObjectFontAttributesSROM, int ai_multitonInst) {
		vtObjectFontAttributes_c::init(vtObjectFontAttributesSROM, ai_multitonInst);
	}
	;

	iVtObjectFontAttributes_c::iVtObjectFontAttributes_c(
			int ai_multitonInst,
			ObjectID ID,
			uint8_t fontColour,
			uint8_t fontSize,
			uint8_t fontType, // always =0 ISO_LATIN_1
			uint8_t fontStyle)
	:vtObjectFontAttributes_c(
			new iVtObjectFontAttributes_s(
					ID,
					fontColour,
					fontSize,
					fontType, // always =0 ISO_LATIN_1
					fontStyle),
				ai_multitonInst)
	{
	}

}
