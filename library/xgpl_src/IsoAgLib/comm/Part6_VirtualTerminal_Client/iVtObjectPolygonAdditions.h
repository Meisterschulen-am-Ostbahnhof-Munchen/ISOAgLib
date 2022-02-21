/*
 * iVtObjectPolygonAdditions.h
 *
 *  Created on: 21.02.2022
 *      Author: franz
 */

#ifndef COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTPOLYGONADDITIONS_H_
#define COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTPOLYGONADDITIONS_H_


namespace IsoAgLib {




struct repeat_x_y_s {
  uint16_t x;
  uint16_t y;
};


/**
 *
 * Polygon type. The first three types are useful
 * only if the polygon is to be filled. VT designer
 * may choose to implement only a complex fill
 * algorithm since it will work with all types.
 * Polygon type can only be changed from open
 * to not open or from not open to open.
 * 0 = Convex. On any given horizontal line, only two points on the polygon are encountered.
 * 1 = Non-Convex. On any given horizontal line, more than two points on the polygon edges can be encountered but the polygon edges do not cross.
 * 2 = Complex. Similar to Non-convex but edges cross. Uses Complex Fill Algorithm.
 * 3 = Open. This type cannot be filled.
 *
 *
 */
enum PolygonType : uint8_t
{
	Convex    = 0,
	NonConvex = 1,
	Complex   = 2,
	Open      = 3,
};



}



#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTPOLYGONADDITIONS_H_ */
