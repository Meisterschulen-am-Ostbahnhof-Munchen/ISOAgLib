/*
 * ivtobjectline_c.cpp
 *
 *  Created on: 29.01.2022
 *      Author: franz
 */



#include "ivtobjectline_c.h"

namespace IsoAgLib {


    uint16_t iVtObjectLine_c::objectType() { return VT_OBJECT_TYPE_LINE; }

iVtObjectLine_c::iVtObjectLine_c(
		int ai_multitonInst,
		ObjectID ID,
		iVtObjectLineAttributes_c *lineAttributes,
		uint16_t width,
		uint16_t height,
		uint8_t lineDirection)
:vtObjectLine_c(
new iVtObjectLine_s(
        ID,
		lineAttributes,
		width,
		height,
		lineDirection)
	,ai_multitonInst)
{}

}

