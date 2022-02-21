/*
 * iOptions.cpp
 *
 *  Created on: 20.02.2022
 *      Author: franz
 */


#include <cstdint>
#include "iOptions.h"

namespace IsoAgLib {


iVtObjectInputListOptions::~iVtObjectInputListOptions() = default;

iVtObjectInputListOptions::iVtObjectInputListOptions()
:iVtObjectInputListOptions(allOptionsOff)
{}

iVtObjectInputListOptions::iVtObjectInputListOptions(iVtObjectInputListOptions_e options)
:options(options)
{}


iVtObjectInputListOptions::iVtObjectInputListOptions(
Enabled 		  enabled,
RealTimeDataInput realTimeDataInput
)
:options(allOptionsOff)//first all off.
{
	bits.enabled           = enabled;
	bits.realTimeDataInput = realTimeDataInput;

}


iVtObjectInputNumberOptions::~iVtObjectInputNumberOptions() = default;

iVtObjectInputNumberOptions::iVtObjectInputNumberOptions()
:iVtObjectInputNumberOptions(ioallOptionsOff)
{}

iVtObjectInputNumberOptions::iVtObjectInputNumberOptions(iVtObjectInputNumberOptions_e options)
:options(options)
{}

iVtObjectInputNumberOptions::iVtObjectInputNumberOptions(
		Transparent transparent,
		DisplayLeadingZeros displayLeadingZeros,
		DisplayZeroAsBlank displayZeroAsBlank,
		Truncate truncate)
:options(ioallOptionsOff)//first all off.
{
	bits.transparent         = transparent;
	bits.displayLeadingZeros = displayLeadingZeros;
	bits.displayZeroAsBlank  = displayZeroAsBlank;
	bits.truncate            = truncate;
}


iVtObjectInputNumberOptions2::~iVtObjectInputNumberOptions2() = default;

iVtObjectInputNumberOptions2::iVtObjectInputNumberOptions2()
:iVtObjectInputNumberOptions2(o2allOptionsOff)
{}


iVtObjectInputNumberOptions2::iVtObjectInputNumberOptions2(iVtObjectInputNumberOptions2_e options)
:options(options)
{}

iVtObjectInputNumberOptions2::iVtObjectInputNumberOptions2(
Enabled 		  enabled,
RealTimeDataInput realTimeDataInput
)
:options(o2allOptionsOff)//first all off.
{
	bits.enabled           = enabled;
	bits.realTimeDataInput = realTimeDataInput;

}

iVtObjectMeterOptions::~iVtObjectMeterOptions() {
}


iVtObjectMeterOptions::iVtObjectMeterOptions()
:iVtObjectMeterOptions(allDrawMeter)
{}

iVtObjectMeterOptions::iVtObjectMeterOptions(
		iVtObjectMeterOptions_e options)
:options(options)
{}

iVtObjectMeterOptions::iVtObjectMeterOptions(
		DrawArc drawArc,
		DrawBorder drawBorder,
		DrawTicks drawTicks,
		DeflectionDirection deflectionDirection)
:options(allDrawMeter)//first all off.
{
	bits.drawArc             = drawArc;
	bits.drawBorder          = drawBorder;
	bits.drawTicks           = drawTicks;
	bits.deflectionDirection = deflectionDirection;
}



}

