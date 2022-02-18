/*
 * ivtobjectalarmmask_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */


#include "ivtobjectalarmmask_c.h"

namespace IsoAgLib {


    iVtObjectAlarmMask_c::~iVtObjectAlarmMask_c() = default;

    void iVtObjectAlarmMask_c::setBackgroundColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectAlarmMask_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectAlarmMask_c::setSoftKeyMask(iVtObjectSoftKeyMask_c *newSoftKeyMask, bool b_updateObject,
                                              bool b_enableReplaceOfCmd) {
        vtObjectAlarmMask_c::setSoftKeyMask (newSoftKeyMask, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectAlarmMask_c::setPriority(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectAlarmMask_c::setPriority (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectAlarmMask_c::setAcousticSignal(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectAlarmMask_c::setAcousticSignal (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    bool
    iVtObjectAlarmMask_c::moveChildLocation(iVtObject_c *apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectAlarmMask_c::moveChildLocation (apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectAlarmMask_c::setChildPosition(IsoAgLib::iVtObject_c *apc_childObject, int16_t dx, int16_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectAlarmMask_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    IsoAgLib::Colour iVtObjectAlarmMask_c::updateBackgroundColour(bool b_SendRequest) {
        return vtObjectAlarmMask_c::updateBackgroundColour(b_SendRequest);
    }

    IsoAgLib::iVtObjectSoftKeyMask_c *iVtObjectAlarmMask_c::updateSoftKeyMask(bool b_SendRequest) {
        return vtObjectAlarmMask_c::updateSoftKeyMask(b_SendRequest);
    }

    uint16_t iVtObjectAlarmMask_c::getObjectType() const { return objectType(); }

    uint8_t iVtObjectAlarmMask_c::updateAcousticSignal(bool b_SendRequest) {
        return vtObjectAlarmMask_c::updateAcousticSignal(b_SendRequest);
    }

    uint8_t iVtObjectAlarmMask_c::updatePriority(bool b_SendRequest) {
        return vtObjectAlarmMask_c::updatePriority(b_SendRequest);
    }
} // IsoAgLib



