/*
 * ivtobjectlineattributes_c.cpp
 *
 *  Created on: 29.01.2022
 *      Author: franz
 */



#include "ivtobjectlineattributes_c.h"

namespace IsoAgLib {


    uint16_t iVtObjectLineAttributes_c::objectType() { return VT_OBJECT_TYPE_LINE_ATTRIBUTES; }

iVtObjectLineAttributes_c::iVtObjectLineAttributes_c(
		int ai_multitonInst,
		ObjectID ID,
		uint8_t lineColour,
		uint8_t lineWidth,
		uint16_t lineArt)
:vtObjectLineAttributes_c(
new iVtObjectLineAttributes_s(
        ID,
		lineColour,
		lineWidth,
		lineArt)
	,ai_multitonInst)
{}

}

