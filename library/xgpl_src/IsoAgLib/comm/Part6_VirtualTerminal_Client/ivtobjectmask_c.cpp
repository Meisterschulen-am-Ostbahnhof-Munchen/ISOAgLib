/*
 * ivtobjectmask_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */


#include "ivtobjectmask_c.h"

namespace IsoAgLib {


    iVtObjectMask_c::~iVtObjectMask_c() = default;

    iVtObjectMask_c::iVtObjectMask_c(const iVtObject_c::iVtObjectMask_s *vtObjectDataMaskSROM, multiton ai_multitonInst)
            : vtObject_c(ai_multitonInst)
    {}
} // IsoAgLib


