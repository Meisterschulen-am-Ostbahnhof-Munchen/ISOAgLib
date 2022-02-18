/*
 * ivtobjectcolourmap_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */



#include "ivtobjectcolourmap_c.h"


namespace IsoAgLib {


    iVtObjectColourMap_c::~iVtObjectColourMap_c() = default;

    uint16_t iVtObjectColourMap_c::objectType() { return VT_OBJECT_TYPE_COLOUR_MAP; }

    uint16_t iVtObjectColourMap_c::updateNumOfColourIdxToFollow(bool b_SendRequest) {
        //return vtObjectColourMap_c::updateNumOfColourIdxToFollow(b_SendRequest);
        return 0;
    }

    uint16_t iVtObjectColourMap_c::getObjectType() const { return objectType(); }
} // IsoAgLib




