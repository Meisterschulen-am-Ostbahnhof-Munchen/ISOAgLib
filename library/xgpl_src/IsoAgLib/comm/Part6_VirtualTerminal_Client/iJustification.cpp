/*
 * iJustification.cpp
 *
 *  Created on: 20.02.2022
 *      Author: franz
 */


#include <cstdint>
#include "iJustification.h"

namespace IsoAgLib {


}

Justification::~Justification() = default;

Justification::Justification(
        HorizontalJustification horizontalJustification,
        VerticalJustification verticalJustification)
	:justification(TopLeft)
{
	bits.horizontalJustification = horizontalJustification;
	bits.verticalJustification   = verticalJustification;

}

Justification::Justification(AllJustification justification)
:justification(justification)
{

}
