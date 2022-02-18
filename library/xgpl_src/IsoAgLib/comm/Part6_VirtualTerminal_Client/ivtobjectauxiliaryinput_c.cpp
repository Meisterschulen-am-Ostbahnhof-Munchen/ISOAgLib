/*
 * ivtobjectauxiliaryinput_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */



#include "ivtobjectauxiliaryinput_c.h"


namespace IsoAgLib {


    uint16_t iVtObjectAuxiliaryInput_c::objectType() { return VT_OBJECT_TYPE_AUXILIARY_INPUT_1; }

    bool iVtObjectAuxiliaryInput_c::moveChildLocation(IsoAgLib::iVtObject_c *apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectAuxiliaryInput_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectAuxiliaryInput_c::setChildPosition(IsoAgLib::iVtObject_c *apc_childObject, int16_t dx, int16_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectAuxiliaryInput_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectAuxiliaryInput_c::getObjectType() const { return objectType(); }
} // IsoAgLib


