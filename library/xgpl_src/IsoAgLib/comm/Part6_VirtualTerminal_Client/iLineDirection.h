/*
 * iLineDirection.h
 *
 *  Created on: 20.02.2022
 *      Author: franz
 */

#ifndef COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_ILINEDIRECTION_H_
#define COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_ILINEDIRECTION_H_



namespace IsoAgLib {

/**
 *
 *
 * 0 = Line is drawn from top left to bottom right
 * of enclosing virtual rectangle
 * 1 = Line is drawn from bottom left to top right
 * of enclosing virtual rectangle
 *
 *
 *
 */


	enum LineDirection : uint8_t
	{
		sink  = 0,
		rise = 1,
	};

};


#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_ILINEDIRECTION_H_ */
