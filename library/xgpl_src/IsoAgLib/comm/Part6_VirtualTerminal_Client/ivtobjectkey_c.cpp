/*
 * ivtobjectkey_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */


#include "ivtobjectkey_c.h"

namespace IsoAgLib {


    iVtObjectKey_c::~iVtObjectKey_c() = default;

    iVtObjectKey_c::iVtObjectKey_c(
            iVtClientObjectPool_c *pool,
            ObjectID ID,
            Colour backgroundColour,
            uint8_t keyCode)
            :vtObjectKey_c(
            pool->getAiMultitonInst(),
            ID,
            backgroundColour,
            keyCode)
    {
        pool->Append(this);
    }
} // IsoAgLib



