/*
 * iLineSuppression.cpp
 *
 *  Created on: 21.02.2022
 *      Author: franz
 */

#include <cstdint>
#include "iLineSuppression.h"

namespace IsoAgLib {

LineSuppression::~LineSuppression() = default;

LineSuppression::LineSuppression(LineSuppression_e lineSuppression)
:lineSuppression(lineSuppression)
{
}

LineSuppression::LineSuppression(
		SuppressTopLine top,
		SuppressRightSideLine right,
		SuppressBottomLine bottom,
		SuppressLeftSideLine left)
	:lineSuppression(ClosedRectangle)
{
	bits.top    = top;
	bits.right  = right;
	bits.bottom = bottom;
	bits.left   = left;
}

}
