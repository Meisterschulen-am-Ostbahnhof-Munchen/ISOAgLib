/*
 * ivttypes.cpp
 *
 *  Created on: 28.01.2022
 *      Author: franz
 */


#include "ivttypes.h"

namespace IsoAgLib {


    repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s(
            iVtObject_c *vtObject,
			short x,
			short y,
			iVtObjectFontAttributes_c *vtObjectBlockFont,
			short row,
			short col)
            : vtObject(vtObject)
			  , x(x)
			  , y(y)
			  , vtObjectBlockFont(vtObjectBlockFont)
			  , row(row)
			  , col(col)
		{}
}
