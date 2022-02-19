/*
 * ivtobjectauxiliaryfunction_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectauxiliaryfunction_c.h"

namespace IsoAgLib {


    iVtObjectAuxiliaryFunction_c::~iVtObjectAuxiliaryFunction_c() = default;

    uint16_t iVtObjectAuxiliaryFunction_c::objectType() { return VT_OBJECT_TYPE_AUXILIARY_FUNCTION_1; }

    bool iVtObjectAuxiliaryFunction_c::moveChildLocation(iVtObject_c *apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectAuxiliaryFunction_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectAuxiliaryFunction_c::setChildPosition(iVtObject_c *apc_childObject, int16_t dx, int16_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectAuxiliaryFunction_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectAuxiliaryFunction_c::getObjectType() const { return objectType(); }
} // IsoAgLib



