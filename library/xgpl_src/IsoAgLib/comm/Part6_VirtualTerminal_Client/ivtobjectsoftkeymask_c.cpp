/*
 * ivtobjectsoftkeymask_c.cpp
 *
 *  Created on: 04.02.2022
 *      Author: franz
 */

#include "ivtobjectsoftkeymask_c.h"

namespace IsoAgLib {


    iVtObjectSoftKeyMask_c::~iVtObjectSoftKeyMask_c() = default;

    iVtObjectSoftKeyMask_c::iVtObjectSoftKeyMask_c(
            iVtClientObjectPool_c *pool,
            ObjectID ID,
            Colour backgroundColour)
    :vtObjectSoftKeyMask_c(
    		pool->getAiMultitonInst(),
			ID,
			backgroundColour)
	{
    	pool->Append(this);
	}

    void iVtObjectSoftKeyMask_c::setBackgroundColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectSoftKeyMask_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    Colour iVtObjectSoftKeyMask_c::updateBackgroundColour(bool b_SendRequest) {
        return vtObjectSoftKeyMask_c::updateBackgroundColour(b_SendRequest);
    }

    ObjectType iVtObjectSoftKeyMask_c::getObjectType() const { return objectType(); }


} // IsoAgLib



