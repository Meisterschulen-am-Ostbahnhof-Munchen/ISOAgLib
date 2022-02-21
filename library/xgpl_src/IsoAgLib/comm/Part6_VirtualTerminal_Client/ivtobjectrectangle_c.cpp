/*
 * ivtobjectrectangle_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */


#include "ivtobjectrectangle_c.h"

namespace IsoAgLib {


    ObjectType iVtObjectRectangle_c::objectType() { return VT_OBJECT_TYPE_RECTANGLE; }

    iVtObjectRectangle_c::~iVtObjectRectangle_c() = default;

    iVtObjectRectangle_c::iVtObjectRectangle_c(
            iVtClientObjectPool_c *pool,
            ObjectID ID, iVtObjectLineAttributes_c *lineAttributes,
            uint16_t width,
            uint16_t height,
            LineSuppression lineSuppression,
            iVtObjectFillAttributes_c *fillAttributes)
    :vtObjectRectangle_c(
    		pool->getAiMultitonInst(),
            ID,
    		lineAttributes,
    		width,
    		height,
			lineSuppression,
			fillAttributes)
    	{
    		pool->Append(this);
    	}






} // IsoAgLib
