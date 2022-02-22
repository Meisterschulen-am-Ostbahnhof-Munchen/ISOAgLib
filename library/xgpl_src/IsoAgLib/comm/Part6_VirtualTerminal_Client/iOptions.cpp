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


	iVtObjectNumberOptions::~iVtObjectNumberOptions() = default;

	iVtObjectNumberOptions::iVtObjectNumberOptions()
	:iVtObjectNumberOptions(ioallOptionsOff)
	{}

	iVtObjectNumberOptions::iVtObjectNumberOptions(iVtObjectNumberOptions_e options)
	:options(options)
	{}

	iVtObjectNumberOptions::iVtObjectNumberOptions(
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


	iVtObjectNumberOptions2::~iVtObjectNumberOptions2() = default;

	iVtObjectNumberOptions2::iVtObjectNumberOptions2()
	:iVtObjectNumberOptions2(o2allOptionsOff)
	{}


	iVtObjectNumberOptions2::iVtObjectNumberOptions2(iVtObjectNumberOptions2_e options)
	:options(options)
	{}

	iVtObjectNumberOptions2::iVtObjectNumberOptions2(
	Enabled 		  enabled,
	RealTimeDataInput realTimeDataInput
	)
	:options(o2allOptionsOff)//first all off.
	{
		bits.enabled           = enabled;
		bits.realTimeDataInput = realTimeDataInput;

	}

	iVtObjectMeterOptions::~iVtObjectMeterOptions() = default;


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


    iVtObjectStringOptions::~iVtObjectStringOptions() = default;

    iVtObjectStringOptions::iVtObjectStringOptions()
    :iVtObjectStringOptions(soallOptionsOff)
    {

    }

    iVtObjectStringOptions::iVtObjectStringOptions(
    		iVtObjectStringOptions_e options)
    :options(options)
    {

    }

    iVtObjectStringOptions::iVtObjectStringOptions(
    		Transparent transparent,
			AutoWrap autoWrap,
			WrapOnHyphen wrapOnHyphen)
    :iVtObjectStringOptions(soallOptionsOff)
    {
    	bits.transparent  = transparent;
		bits.autoWrap     = autoWrap;
		bits.wrapOnHyphen = wrapOnHyphen;
    }

    iVtObjectArchedBarGraphOptions::~iVtObjectArchedBarGraphOptions() = default;

    iVtObjectArchedBarGraphOptions::iVtObjectArchedBarGraphOptions()
            :iVtObjectArchedBarGraphOptions(allDrawArchedBarGraph)
    {}

    iVtObjectArchedBarGraphOptions::iVtObjectArchedBarGraphOptions(
            iVtObjectArchedBarGraphOptions_e options)
            :options(options)
            {}

    iVtObjectArchedBarGraphOptions::iVtObjectArchedBarGraphOptions(
            DrawBorder drawBorder,
            DrawATargetLine drawATargetLine,
            BarGraphType barGraphType,
            DeflectionDirection deflectionDirection)
            :iVtObjectArchedBarGraphOptions(allDrawArchedBarGraph)
            {
                bits.drawBorder          = drawBorder;
                bits.drawATargetLine     = drawATargetLine;
                bits.barGraphType        = barGraphType;
                bits.deflectionDirection = deflectionDirection;
            }
}

