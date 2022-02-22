/*
 * iOptions.h
 *
 *  Created on: 20.02.2022
 *      Author: franz
 */

#ifndef COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IOPTIONS_H_
#define COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IOPTIONS_H_



namespace IsoAgLib {


	//Enabled is used in many cases.
	/**
	 * Bit 0 = Enabled. If TRUE, the object shall be
	 * enabled. If FALSE, the object is disabled.
	 */
	enum Enabled : bool
	{
		disabled = false,
		enabled  = true,
	};

	/**
	 * Bit 1 = real time data input. b If TRUE, the
	 * value shall be transmitted to the ECU as it is
     * being changed (see 4.2).
     */
	enum RealTimeDataInput : bool
	{
		realTimeDataInputOff = false,
		realTimeDataInputOn  = true,
	};


	enum Reserved : bool
	{
		setToZero = false,
	};



	//Transparent. need to see where used !
	/**
	 * Bit 0 = Transparent. If TRUE, the input field is
	 * displayed with background showing through
     * instead of using the background colour
     * attribute.
	 */
	enum Transparent : bool
	{
	    opaque      = false,
		transparent = true,
	};

	/**
	 * Bit 1 = Display leading zeros. If TRUE, Fill left
	 * to width of field with zeros.
	 */
	enum DisplayLeadingZeros : bool
	{
		noDisplayLeadingZeros      = false,
	    displayLeadingZeros        = true,
	};


	/**
	 * Bit 2 = Display zero as blank if this bit is
	 * TRUE. When this option bit is set, a blank field
	 * is displayed if and only if the value of the
	 * object is exactly zero.
	 *
	 * Except when the field is blank, the VT shall
	 * always display at least one digit before the
	 * decimal point (examples: 2.2, 0.2).
	 *
	 */
	enum DisplayZeroAsBlank : bool
	{
		noDisplayZeroAsBlank    = false,
	    displayZeroAsBlank 		= true,
	};


	/**
	 * Bit 3 = Truncate. If TRUE, the value shall be
	 * truncated to the specified number of decimals.
	 * Otherwise, it shall be rounded off to the
	 * specified number of decimals. a b
	 *
	 * The designer should account for a unary
	 * minus sign with respect to leading zeros and
	 * the field width.
	 */
	enum Truncate : bool
	{
		rounded      = false,
	    truncate     = true,
	};






	/**
	 * Bit 0 = Draw Arc
	 */
	enum DrawArc : bool
	{
		dontDrawArc = false,
		doDrawArc   = true,
	};

	/**
	 * Bit 1 = Draw Border
	 */
	enum  DrawBorder : bool
	{
		dontDrawBorder = false,
		doDrawBorder   = true,
	};

	/**
	 * Bit 2 = Draw Ticks
	 */
	enum DrawTicks : bool
	{
		dontDrawTicks = false,
		doDrawTicks   = true,
	};

	/**
	 * Bit 3 = Deflection Direction.
	 * 0 = From minimum to maximum, anticlockwise.
	 * 1 = From minimum to maximum, clockwise.
	 */
	enum DeflectionDirection : bool
	{
		anticlockwise = false,
		clockwise     = true,
	};

	/**
	 * Bit 1 = Auto-Wrap. If TRUE, Auto-Wrapping
	 * rules apply (see 4.6.16.4).
	 */
	enum AutoWrap : bool
	{
		noAutoWrap = false,
	    autoWrap   = true,
	};

	/**
	 * Bit 2 = Wrap on Hyphen. If TRUE, Auto-
	 * Wrapping can occur between a hyphen
	 * (2D 16 ) and the following character (see
	 * 4.6.16.4). Wrap on Hyphen is a modifier to
	 * the Auto-Wrap option and is applied only if
	 * the Auto-Wrap option is TRUE and ignored if
	 * the Auto-Wrap option is FALSE. a
	 */

	enum WrapOnHyphen : bool
	{
		noWrapOnHyphen = false,
	    wrapOnHyphen   = true,
	};




	/**
	 * Bit 0 = Draw Border
	 */


	/**
	 * Bit 1 = Draw a target line
	 */
	enum DrawATargetLine : bool
	{
		drawNoTargetLine  = false,
		drawATargetLine   = true,
	};


	/**
	 * Bit 2 = Not used
	 */

	/**
	 * Bit 3 = bar graph type. If this bit is FALSE (0),
	 * bar graph is filled. If this bit is TRUE (1), the bar
	 * graph is not filled but rather shows the current
	 * value as a single line at the proper position
	 * within the bar graph.
	 */
	enum BarGraphType : bool
	{
		filled      = false,
		notFilled   = true,
	};

	/**
	 * 	Bit 4 = Deflection of the bar graph around the arc.
	 * 	0 = anticlockwise
	 * 	    and
	 * 	1 = clockwise
	 */



	/**
	 * 	Orientation and direction of the bar graph:
	 * 	Bit 4 = Axis orientation.
	 * 	0 = vertical (increasing values move parallel to the Y axis with constant X),
	 * 	1 = horizontal (increasing values move parallel to the X axis with constant Y)
	 */


	enum AxisOrientation : bool
	{
		vertical     = false,
		horizontal   = true,
	};


	/*
	 *
	 * 	Bit 5 = Direction.
	 * 	0 = Grows negative (left or down).
	 * 	1 = Grows positive (right or up).
	 *
	 *
	 */

	enum Direction : bool
	{
		growsNegative   = false,
		growsPositive   = true,
	};




	enum iVtObjectLinearBarGraphOptions_e : uint8_t
	{
		allDrawLinearBarGraph = 0b00110111,
	};



	union iVtObjectLinearBarGraphOptions {
		iVtObjectLinearBarGraphOptions_e options;
		struct __attribute__ ((packed))
		{
			DrawBorder 	        drawBorder          : 1;
			DrawATargetLine     drawATargetLine     : 1;
			DrawTicks           drawTicks           : 1;
			BarGraphType        barGraphType        : 1;

			AxisOrientation     axisOrientation     : 1;
			Direction           direction           : 1;
			Reserved            reserved06          : 1;
			Reserved            reserved07          : 1;
		} bits ;
		~iVtObjectLinearBarGraphOptions();
		iVtObjectLinearBarGraphOptions();
		explicit iVtObjectLinearBarGraphOptions(
				iVtObjectLinearBarGraphOptions_e options         = allDrawLinearBarGraph);
		explicit iVtObjectLinearBarGraphOptions(
				DrawBorder 	        drawBorder          = DrawBorder::doDrawBorder,
				DrawATargetLine     drawATargetLine     = DrawATargetLine::drawATargetLine,
				DrawTicks           drawTicks           = DrawTicks::doDrawTicks,
				BarGraphType        barGraphType        = BarGraphType::filled,
				AxisOrientation     axisOrientation     = AxisOrientation::horizontal,
				Direction           direction           = Direction::growsPositive
				);
	};







	enum iVtObjectArchedBarGraphOptions_e : uint8_t
	{
		allDrawArchedBarGraph = 0b00010011,
	};



	union iVtObjectArchedBarGraphOptions {
		iVtObjectArchedBarGraphOptions_e options;
		struct __attribute__ ((packed))
		{
			DrawBorder 	        drawBorder          : 1;
			DrawATargetLine     drawATargetLine     : 1;
			Reserved            reserved02          : 1;
			BarGraphType        barGraphType        : 1;

			DeflectionDirection deflectionDirection : 1;
			Reserved            reserved05          : 1;
			Reserved            reserved06          : 1;
			Reserved            reserved07          : 1;
		} bits ;
		~iVtObjectArchedBarGraphOptions();
		iVtObjectArchedBarGraphOptions();
		explicit iVtObjectArchedBarGraphOptions(
				iVtObjectArchedBarGraphOptions_e options         = allDrawArchedBarGraph);
		explicit iVtObjectArchedBarGraphOptions(
				DrawBorder 	        drawBorder          = DrawBorder::doDrawBorder,
				DrawATargetLine     drawATargetLine     = DrawATargetLine::drawATargetLine,
				BarGraphType        barGraphType        = BarGraphType::filled,
				DeflectionDirection deflectionDirection = DeflectionDirection::clockwise
				);
	};







	enum iVtObjectMeterOptions_e : uint8_t
	{
		allDrawMeter = 0b00001111,
	};



	union iVtObjectMeterOptions {
		iVtObjectMeterOptions_e options;
		struct __attribute__ ((packed))
		{
			DrawArc 			drawArc             : 1;
			DrawBorder 	        drawBorder          : 1;
			DrawTicks           drawTicks           : 1;
			DeflectionDirection deflectionDirection : 1;

			Reserved            reserved04          : 1;
			Reserved            reserved05          : 1;
			Reserved            reserved06          : 1;
			Reserved            reserved07          : 1;
		} bits ;
		~iVtObjectMeterOptions();
		iVtObjectMeterOptions();
		explicit iVtObjectMeterOptions(
				iVtObjectMeterOptions_e options         = allDrawMeter);
		explicit iVtObjectMeterOptions(
				DrawArc 			drawArc             = DrawArc::doDrawArc,
				DrawBorder 	        drawBorder          = DrawBorder::doDrawBorder,
				DrawTicks           drawTicks           = DrawTicks::doDrawTicks,
				DeflectionDirection deflectionDirection = DeflectionDirection::clockwise
				);
	};


	enum iVtObjectInputListOptions_e : uint8_t
	{
		allOptionsOff = 0b00000000,
	};



	union iVtObjectInputListOptions {
		iVtObjectInputListOptions_e options;
		struct __attribute__ ((packed))
		{
			Enabled 			enabled           : 1;
			RealTimeDataInput 	realTimeDataInput : 1;
			Reserved            reserved02        : 1;
			Reserved            reserved03        : 1;

			Reserved            reserved04        : 1;
			Reserved            reserved05        : 1;
			Reserved            reserved06        : 1;
			Reserved            reserved07        : 1;
		} bits ;
		~iVtObjectInputListOptions();
		iVtObjectInputListOptions();
		explicit iVtObjectInputListOptions(iVtObjectInputListOptions_e options = allOptionsOff);
		explicit iVtObjectInputListOptions(
				Enabled 		  enabled           = Enabled::enabled,
				RealTimeDataInput realTimeDataInput = RealTimeDataInput::realTimeDataInputOff
				);
	};



	enum iVtObjectNumberOptions_e : uint8_t
	{
		ioallOptionsOff = 0b00000000,
	};

	union iVtObjectNumberOptions{
		iVtObjectNumberOptions_e options;
		struct __attribute__ ((packed))
		{
			Transparent 		transparent         : 1;
			DisplayLeadingZeros displayLeadingZeros : 1;
			DisplayZeroAsBlank  displayZeroAsBlank  : 1;
			Truncate            truncate            : 1;

			Reserved            reserved04          : 1;
			Reserved            reserved05          : 1;
			Reserved            reserved06          : 1;
			Reserved            reserved07          : 1;
		} bits ;
		~iVtObjectNumberOptions();
		iVtObjectNumberOptions();
		explicit iVtObjectNumberOptions(
				iVtObjectNumberOptions_e options = ioallOptionsOff);
		explicit iVtObjectNumberOptions(
				Transparent 		transparent         = Transparent::opaque,
				DisplayLeadingZeros displayLeadingZeros = DisplayLeadingZeros::noDisplayLeadingZeros,
				DisplayZeroAsBlank  displayZeroAsBlank  = DisplayZeroAsBlank::noDisplayZeroAsBlank,
				Truncate            truncate            = Truncate::rounded
				);
	};


	enum iVtObjectStringOptions_e : uint8_t
	{
		soallOptionsOff = 0b00000000,
	};

	union iVtObjectStringOptions{
		iVtObjectStringOptions_e options;
		struct __attribute__ ((packed))
		{
			Transparent  transparent  : 1;
			AutoWrap     autoWrap     : 1;
			WrapOnHyphen wrapOnHyphen : 1;
			Reserved     reserved03   : 1;

			Reserved     reserved04   : 1;
			Reserved     reserved05   : 1;
			Reserved     reserved06   : 1;
			Reserved     reserved07   : 1;
		} bits{} ;
		~iVtObjectStringOptions();
		iVtObjectStringOptions();
		explicit iVtObjectStringOptions(
				iVtObjectStringOptions_e options = soallOptionsOff);
		explicit iVtObjectStringOptions(
				Transparent   transparent   = Transparent::opaque,
				AutoWrap      autoWrap 	    = AutoWrap::noAutoWrap,
				WrapOnHyphen  wrapOnHyphen  = WrapOnHyphen::noWrapOnHyphen
				);
	};




	enum iVtObjectNumberOptions2_e : uint8_t
	{
		o2allOptionsOff = 0b00000000,
	};


	union iVtObjectNumberOptions2 {
		iVtObjectNumberOptions2_e options;
		struct __attribute__ ((packed))
		{
			Enabled 			enabled           : 1;
			RealTimeDataInput 	realTimeDataInput : 1;
			Reserved            reserved02        : 1;
			Reserved            reserved03        : 1;

			Reserved            reserved04        : 1;
			Reserved            reserved05        : 1;
			Reserved            reserved06        : 1;
			Reserved            reserved07        : 1;
		} bits ;
		~iVtObjectNumberOptions2();
		iVtObjectNumberOptions2();
		explicit iVtObjectNumberOptions2(
				iVtObjectNumberOptions2_e options = o2allOptionsOff);
		explicit iVtObjectNumberOptions2(
				Enabled 		  enabled           = Enabled::enabled,
				RealTimeDataInput realTimeDataInput = RealTimeDataInput::realTimeDataInputOff
				);
	};


}









#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IOPTIONS_H_ */
