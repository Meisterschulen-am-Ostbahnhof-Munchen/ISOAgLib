/*
 * iOptions.cpp
 *
 *  Created on: 20.02.2022
 *      Author: franz
 */


#include <cstdint>
#include "iOptions.h"

namespace IsoAgLib {

	iVtObjectInputListOptions::iVtObjectInputListOptions()
	:options(allOptionsOff)
	{}

	iVtObjectInputListOptions::~iVtObjectInputListOptions() = default;

	iVtObjectInputListOptions::iVtObjectInputListOptions(iVtObjectInputListOptions_e options)
	:options(options)
	{}

	iVtObjectInputListOptions::iVtObjectInputListOptions(Enabled enabled)
	:options(allOptionsOff)//first all off.
	{
		bits.enabled = enabled; //then set this bit.
	}

	iVtObjectInputNumberOptions::iVtObjectInputNumberOptions()
	:options(ioallOptionsOff)
	{}

	iVtObjectInputNumberOptions::~iVtObjectInputNumberOptions() = default;

	iVtObjectInputNumberOptions::iVtObjectInputNumberOptions(iVtObjectInputNumberOptions_e options)
	:options(options)
	{}



	iVtObjectInputNumberOptions2::iVtObjectInputNumberOptions2()
	:options(o2allOptionsOff)
	{}

	iVtObjectInputNumberOptions2::~iVtObjectInputNumberOptions2() = default;

	iVtObjectInputNumberOptions2::iVtObjectInputNumberOptions2(iVtObjectInputNumberOptions2_e options)
	:options(options)
	{}

	iVtObjectInputNumberOptions2::iVtObjectInputNumberOptions2(Enabled enabled)
	:options(o2allOptionsOff)//first all off.
	{
		bits.enabled = enabled; //then set this bit.
	}

}

