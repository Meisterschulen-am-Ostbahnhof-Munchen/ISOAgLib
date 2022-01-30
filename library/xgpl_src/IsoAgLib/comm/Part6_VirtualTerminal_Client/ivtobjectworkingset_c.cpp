/*
 * ivtobjectworkingset_c.cpp
 *
 *  Created on: 28.01.2022
 *      Author: franz
 */


#include "ivtobjectworkingset_c.h"

namespace IsoAgLib {

	iVtObjectWorkingSet_c::iVtObjectWorkingSet_c(
			iVtClientObjectPool_c* pool,
			ObjectID ID,
			uint8_t backgroundColour,
			uint8_t selectable,
			iVtObjectMask_c *activeMask,
			uint8_t numberOfLanguagesToFollow,
			const repeat_vtLanguage_s *languagesToFollow)
	:vtObjectWorkingSet_c(
			pool->getAiMultitonInst(),
				ID,
				backgroundColour,
				selectable,
				activeMask,
				numberOfLanguagesToFollow,
				languagesToFollow)
		{
			pool->Append(this);
		}

    void iVtObjectWorkingSet_c::updateSelectable(uint8_t newSelectable) {
        vtObjectWorkingSet_c::updateSelectable( newSelectable );
    }

    void iVtObjectWorkingSet_c::changeActiveMask(iVtObjectMask_c *apc_iVtObjectMask, bool b_updateObject,
                                                 bool b_enableReplaceOfCmd) {
        vtObjectWorkingSet_c::changeActiveMask (apc_iVtObjectMask, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectWorkingSet_c::changeBackgroundColour(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectWorkingSet_c::changeBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectWorkingSet_c::moveChildLocation(IsoAgLib::iVtObject_c *apc_childObject, int8_t dx, int8_t dy,
                                                  bool b_updateObject) {
        return vtObjectWorkingSet_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
    }

    bool iVtObjectWorkingSet_c::setChildPosition(IsoAgLib::iVtObject_c *apc_childObject, int16_t dx, int16_t dy,
                                                 bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectWorkingSet_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    bool
    iVtObjectWorkingSet_c::controlAudioDevice(uint8_t aui8_repetitions, uint16_t aui16_frequency, uint16_t aui16_onTime,
                                              uint16_t aui16_offTime) {
        return vtObjectWorkingSet_c::controlAudioDevice (aui8_repetitions, aui16_frequency, aui16_onTime, aui16_offTime);
    }

    bool iVtObjectWorkingSet_c::setAudioVolume(uint8_t aui8_volume) {
        return vtObjectWorkingSet_c::setAudioVolume (aui8_volume);
    }

    bool iVtObjectWorkingSet_c::setColourMapOrPalette(uint16_t aui16_objectId) {
        return vtObjectWorkingSet_c::setColourMapOrPalette (aui16_objectId);
    }

    uint16_t iVtObjectWorkingSet_c::getObjectType() const { return objectType(); }

    uint16_t iVtObjectWorkingSet_c::objectType() { return VT_OBJECT_TYPE_WORKING_SET; }


}
