/*
 * ivtobjectinputnumber_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectinputnumber_c.h"


namespace IsoAgLib {


    iVtObjectInputNumber_c::~iVtObjectInputNumber_c() = default;

    iVtObjectInputNumber_c::iVtObjectInputNumber_c(
    		iVtClientObjectPool_c *pool,
			ObjectID ID, uint16_t width,
			uint16_t height,
			Colour backgroundColour,
			iVtObjectFontAttributes_c *fontAttributes,
			uint8_t options,
			iVtObjectNumberVariable_c *variableReference,
			uint32_t value,
			uint32_t minValue, uint32_t maxValue,
			int32_t offset,
			float scale,
			uint8_t numberOfDecimals,
			uint8_t format,
			uint8_t horizontalJustification,
			uint8_t secondOptionsByte)
    {

    }
} // IsoAgLib



