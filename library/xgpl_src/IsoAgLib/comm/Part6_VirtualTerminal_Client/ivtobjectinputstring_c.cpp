/*
 * ivtobjectinputstring_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectinputstring_c.h"


namespace IsoAgLib {

iVtObjectInputString_c::iVtObjectInputString_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID,
		uint16_t width,
		uint16_t height,
		Colour backgroundColour,
		iVtObjectFontAttributes_c *fontAttributes,
		iVtObjectInputAttributes_c* inputAttributes,
		iVtObjectStringOptions options,
		iVtObjectStringVariable_c *variableReference,
		Justification justification,
		char *value,
		uint8_t enabled)
        :vtObjectInputString_c(
        		pool->getAiMultitonInst(),
                ID,
                width,
                height,
                backgroundColour,
                fontAttributes,
				inputAttributes,
                options,
                variableReference,
				justification,
                value, /* size length+1 (0-termination intern!) */
				enabled
        )
{
	pool->Append(this);
}

} // IsoAgLib



