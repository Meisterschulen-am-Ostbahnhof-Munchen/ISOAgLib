/*
 * ivtobjectpolygon_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectpolygon_c.h"

namespace IsoAgLib {


    iVtObjectPolygon_c::~iVtObjectPolygon_c() = default;

    iVtObjectPolygon_c::iVtObjectPolygon_c(
    		iVtClientObjectPool_c *pool,
			ObjectID ID,
			uint16_t width,
			uint16_t height,
			iVtObjectLineAttributes_c *lineAttributes,
			iVtObjectFillAttributes_c *fillAttributes,
			PolygonType polygonType,
			uint8_t numberOfPoints,
			const repeat_x_y_s *pointsToFollow)
    :vtObjectPolygon_c(
    		pool->getAiMultitonInst(),
			ID,
			width,
			height,
			lineAttributes,
			fillAttributes,
			polygonType,
			numberOfPoints,
			pointsToFollow)
 	{
 		pool->Append(this);
 	}
} // IsoAgLib

