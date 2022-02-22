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

    ObjectType iVtObjectNumberVariable_c::objectType() { return VT_OBJECT_TYPE_NUMBER_VARIABLE; }

    void iVtObjectNumberVariable_c::setValue(uint32_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectNumberVariable_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd); }

    ObjectType iVtObjectNumberVariable_c::getObjectType() const { return objectType(); }
} // IsoAgLib




