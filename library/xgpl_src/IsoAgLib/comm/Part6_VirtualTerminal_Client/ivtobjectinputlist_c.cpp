/*
 * ivtobjectinputlist_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectinputlist_c.h"


namespace IsoAgLib {


    iVtObjectInputList_c::iVtObjectInputList_c(
    		iVtClientObjectPool_c *pool,
			ObjectID ID,
			uint16_t width,
			uint16_t height,
			iVtObjectNumberVariable_c *variableReference,
			uint8_t value,
			uint8_t options)
    	    :vtObjectInputList_c(
    				pool->getAiMultitonInst(),
    				ID,
					width,
					height,
					variableReference,
					value,
					options)
    		{
    			pool->Append(this);
    		}
} // IsoAgLib



