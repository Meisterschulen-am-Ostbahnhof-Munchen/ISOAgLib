/*
 * ivtobjectnumbervariable_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */


#include "ivtobjectnumbervariable_c.h"

namespace IsoAgLib {


    iVtObjectNumberVariable_c::~iVtObjectNumberVariable_c() = default;

    iVtObjectNumberVariable_c::iVtObjectNumberVariable_c(
            iVtClientObjectPool_c *pool,
            ObjectID ID,
            uint32_t value)
            :vtObjectNumberVariable_c(
            pool->getAiMultitonInst(),
            ID,
            value)
    {
        pool->Append(this);
    }
} // IsoAgLib




