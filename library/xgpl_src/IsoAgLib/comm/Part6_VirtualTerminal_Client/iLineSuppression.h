/*
 * iLineSuppression.h
 *
 *  Created on: 21.02.2022
 *      Author: franz
 */

#ifndef COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_ILINESUPPRESSION_H_
#define COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_ILINESUPPRESSION_H_

namespace IsoAgLib {


/**
 * Bit 0 = 1 = Suppress Top Line (smallest Y value)
 */
enum SuppressTopLine : bool
{
	showTopLine     = false,
	suppressTopLine = true,
};
/**
 * Bit 1 = 1 = Suppress Right Side Line (largest X value)
 */
enum SuppressRightSideLine : bool
{
	showRightSideLine     = false,
	suppressRightSideLine = true,
};
/**
 * Bit 2 = 1 = Suppress Bottom Line (largest Y value)
 */
enum SuppressBottomLine : bool
{
	showBottomLine     = false,
	suppressBottomLine = true,
};
/**
 * Bit 3 = 1 = Suppress Left Side Line (smallest X value)
 */
enum SuppressLeftSideLine : bool
{
	showLeftSideLine     = false,
	suppressLeftSideLine = true,
};

enum SuppressOtherBit : bool
{
	resevedLine     = false,
};


enum LineSuppression_e : uint8_t
{
	ClosedRectangle = 0b00000000,
};



union LineSuppression {
	LineSuppression_e lineSuppression;
	struct __attribute__ ((packed))
	{
		SuppressTopLine       top    : 1;
		SuppressRightSideLine right  : 1;
		SuppressBottomLine    bottom : 1;
		SuppressLeftSideLine  left   : 1;

		SuppressOtherBit  reserved04 : 1;
		SuppressOtherBit  reserved05 : 1;
		SuppressOtherBit  reserved06 : 1;
		SuppressOtherBit  reserved07 : 1;
	} bits ;
	~LineSuppression();
	explicit LineSuppression(LineSuppression_e lineSuppression = ClosedRectangle);
	explicit LineSuppression(
			SuppressTopLine       top    = showTopLine,
			SuppressRightSideLine right  = showRightSideLine,
			SuppressBottomLine    bottom = showBottomLine,
			SuppressLeftSideLine  left   = showLeftSideLine);
};




}




#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_ILINESUPPRESSION_H_ */
