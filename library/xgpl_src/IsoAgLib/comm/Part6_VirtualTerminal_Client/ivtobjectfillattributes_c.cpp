/*
 * ivtobjectfillattributes_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectfillattributes_c.h"

#ifdef CONFIG_USE_VTOBJECT_fillattributes

namespace IsoAgLib {


    iVtObjectFillAttributes_c::~iVtObjectFillAttributes_c() = default;

    uint16_t iVtObjectFillAttributes_c::objectType() { return VT_OBJECT_TYPE_FILL_ATTRIBUTES; }
} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_fillattributes



