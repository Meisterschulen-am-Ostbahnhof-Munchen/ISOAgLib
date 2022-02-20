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
	enum Enabled : bool
	{
		disabled = false,
		enabled  = true,
	};


	enum RealTimeDataInput : bool
	{
		realTimeDataInputOff = false,
		realTimeDataInputOn  = true,
	};


	enum Reserved : bool
	{
		setToZero = false,
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



};









#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IOPTIONS_H_ */
