/*
 * ivtobjectauxiliaryfunction2_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */


#include "ivtobjectauxiliaryfunction2_c.h"

namespace IsoAgLib {


    iVtObjectAuxiliaryFunction2_c::~iVtObjectAuxiliaryFunction2_c() = default;

    bool iVtObjectAuxiliaryFunction2_c::moveChildLocation(iVtObject_c *apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectAuxiliaryFunction2_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectAuxiliaryFunction2_c::setChildPosition(iVtObject_c *apc_childObject, int16_t dx, int16_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectAuxiliaryFunction2_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectAuxiliaryFunction2_c::overrideFunctionType_onlyAtStartup(uint8_t newValue) {
        vtObjectAuxiliaryFunction2_c::overrideFunctionType_onlyAtStartup(newValue);
    }

    bool iVtObjectAuxiliaryFunction2_c::isAssigned() const { return vtObjectAuxiliaryFunction2_c::isAssigned(); }

    uint16_t iVtObjectAuxiliaryFunction2_c::getObjectType() const { return objectType(); }

    uint16_t iVtObjectAuxiliaryFunction2_c::objectType() { return VT_OBJECT_TYPE_AUXILIARY_FUNCTION_2; }
} // IsoAgLib


