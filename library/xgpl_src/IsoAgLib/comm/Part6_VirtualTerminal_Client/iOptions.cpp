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
{}

iVtObjectInputListOptions::~iVtObjectInputListOptions() = default;

iVtObjectInputListOptions::iVtObjectInputListOptions(iVtObjectInputListOptions_e options)
:options(options)
{}

iVtObjectInputListOptions::iVtObjectInputListOptions(Enabled enabled)
{
	options = allOptionsOff; //first all off.
	bits.enabled = enabled; //then set this bit.
}

}

