/*
 * iJustification.h
 *
 *  Created on: 20.02.2022
 *      Author: franz
 */

#ifndef COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IJUSTIFICATION_H_
#define COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IJUSTIFICATION_H_

namespace IsoAgLib {



/**
 *
 * Horizontal Justification Value of Bits 0–1
 * 0 = Position Left
 * 1 = Position Middle
 * 2 = Position Right
 * 3 = Reserved
 *
 */

enum HorizontalJustification : uint8_t
{
	HorizontalLeft   = 0,
	HorizontalMiddle = 1,
	HorizontalRight  = 2,
};

/**
 *
 * Vertical Justification Value of Bits 2–3 b
 * 0 = Position Top
 * 1 = Position Middle
 * 2 = Position Bottom
 * 3 = Reserved
 *
 */

enum VerticalJustification : uint8_t
{
	VerticalTop    = 0,
	VerticalMiddle = 1,
	VerticalBottom = 2,
};



enum ReservedJustification : uint8_t
{
	setToZeroJustification = 0,
};




enum AllJustification : uint8_t
{
	TopLeft = 0b00000000,
};


union Justification {
	AllJustification justification;
	struct __attribute__ ((packed))
	{
		HorizontalJustification horizontalJustification : 2;
		VerticalJustification 	verticalJustification   : 2;
		ReservedJustification   reserved02              : 2;
		ReservedJustification   reserved03              : 2;

	} bits ;
	Justification();
	explicit Justification(AllJustification justification);
	explicit Justification(
			HorizontalJustification horizontalJustification = HorizontalLeft ,
			VerticalJustification 	verticalJustification   = VerticalTop);
	~Justification();
};

}

#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IJUSTIFICATION_H_ */
