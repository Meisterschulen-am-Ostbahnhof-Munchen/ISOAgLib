/*
 * ivtobjectmeter_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectmeter_c.h"

namespace IsoAgLib {

iVtObjectMeter_c::~iVtObjectMeter_c() {
}

iVtObjectMeter_c::iVtObjectMeter_c(
		iVtClientObjectPool_c* pool,
  		ObjectID ID,
		uint16_t width,
		Colour needleColour,
		Colour borderColour,
		Colour arcAndTickColour,
		iVtObjectMeterOptions options,
		uint8_t numberOfTicks,
		uint8_t startAngle,
		uint8_t endAngle,
		uint16_t minValue,
		uint16_t maxValue,
		iVtObjectNumberVariable_c *variableReference,
		uint16_t value)
:vtObjectMeter_c(
		pool->getAiMultitonInst(),
  		ID,
		width,
		needleColour,
		borderColour,
		arcAndTickColour,
		options,
		numberOfTicks,
		startAngle,
		endAngle,
		minValue,
		maxValue,
		variableReference,
		value)
{
	pool->Append(this);
}


} // IsoAgLib



