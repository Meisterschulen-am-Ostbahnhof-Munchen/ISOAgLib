/*
 * iFillType.h
 *
 *  Created on: 15.02.2022
 *      Author: franz
 */

#ifndef COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IFILLTYPE_H_
#define COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IFILLTYPE_H_


namespace IsoAgLib {

//Fill type

enum FillType : uint8_t {

	noFill      = 0, // no fill
	lineColour  = 1, // fill with line colour
	fillColour  = 2, // fill with specified colour in fill colour attribute
	fillPattern = 3, // fill with pattern given by fill pattern attribute
};


}


#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IFILLTYPE_H_ */
