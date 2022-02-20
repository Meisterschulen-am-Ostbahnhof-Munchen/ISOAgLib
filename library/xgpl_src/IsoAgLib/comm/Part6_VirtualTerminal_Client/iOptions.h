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
		iVtObjectInputListOptions();
		~iVtObjectInputListOptions();
		iVtObjectInputListOptions(iVtObjectInputListOptions_e options);
		iVtObjectInputListOptions(Enabled 			          enabled);
	};



	enum iVtObjectInputNumberOptions_e : uint8_t
	{
		ioallOptionsOff = 0b00000000,
	};

	union iVtObjectInputNumberOptions{
		iVtObjectInputNumberOptions_e options;
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
		iVtObjectInputNumberOptions();
		~iVtObjectInputNumberOptions();
		iVtObjectInputNumberOptions(iVtObjectInputNumberOptions_e options);
	};




	enum iVtObjectInputNumberOptions2_e : uint8_t
	{
		o2allOptionsOff = 0b00000000,
	};


	union iVtObjectInputNumberOptions2 {
		iVtObjectInputNumberOptions2_e options;
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
		iVtObjectInputNumberOptions2();
		~iVtObjectInputNumberOptions2();
		iVtObjectInputNumberOptions2(iVtObjectInputNumberOptions2_e options);
		iVtObjectInputNumberOptions2(Enabled 			            enabled);
	};


};









#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IOPTIONS_H_ */
