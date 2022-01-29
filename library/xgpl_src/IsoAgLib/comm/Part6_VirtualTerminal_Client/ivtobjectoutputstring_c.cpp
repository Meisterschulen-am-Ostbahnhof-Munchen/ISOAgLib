/*
 * ivtobjectoutputstring_c.cpp
 *
 *  Created on: 29.01.2022
 *      Author: franz
 */


#include "ivtobjectoutputstring_c.h"

namespace IsoAgLib {


    iVtObjectOutputString_c::iVtObjectOutputString_c(
    		int ai_multitonInst,
			ObjectID ID,
			uint16_t width,
			uint16_t height,
			uint8_t backgroundColour,
			iVtObjectFontAttributes_c *fontAttributes,
			uint8_t options,
			iVtObjectStringVariable_c *variableReference,
			uint8_t horizontalJustification,
			uint16_t length,
			char *value)
            :vtObjectOutputString_c(
            new iVtObjectOutputString_s(
                    ID,
                    width,
                    height,
                    backgroundColour,
                    fontAttributes,
                    options,
                    variableReference,
                    horizontalJustification,
                    length,
                    value /* size length+1 (0-termination intern!) */
            )
            ,ai_multitonInst)
    {}
}

