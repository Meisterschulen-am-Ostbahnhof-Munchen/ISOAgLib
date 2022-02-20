/*
 * ivtobjectauxiliaryinput2_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */


#include "ivtobjectauxiliaryinput2_c.h"

namespace IsoAgLib {

	ObjectType iVtObjectAuxiliaryInput2_c::objectType() { return VT_OBJECT_TYPE_AUXILIARY_INPUT_2; }

    bool iVtObjectAuxiliaryInput2_c::moveChildLocation(iVtObject_c *apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectAuxiliaryInput2_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectAuxiliaryInput2_c::setChildPosition(iVtObject_c *apc_childObject, int16_t dx, int16_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectAuxiliaryInput2_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectAuxiliaryInput2_c::setValue(uint16_t aui16_value1, uint16_t aui16_value2, iVtObjectAuxiliaryInput2_c::StateForLearnMode_en aen_stateForLearnMode) {
        return vtObjectAuxiliaryInput2_c::setValue(aui16_value1, aui16_value2, aen_stateForLearnMode);
    }

    ObjectType iVtObjectAuxiliaryInput2_c::getObjectType() const { return objectType(); }


} // IsoAgLib


