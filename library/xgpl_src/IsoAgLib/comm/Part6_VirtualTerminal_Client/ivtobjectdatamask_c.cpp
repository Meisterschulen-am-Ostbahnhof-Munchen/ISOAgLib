//
// Created by franz on 28.01.22.
//


#include "ivtobjectdatamask_c.h"

namespace IsoAgLib {


	iVtObjectDataMask_c::iVtObjectDataMask_c() {}

    iVtObjectDataMask_c::iVtObjectDataMask_c(const iVtObject_c::iVtObjectDataMask_s *vtObjectDataMaskSROM,
                                             int ai_multitonInst)
            : vtObjectDataMask_c(vtObjectDataMaskSROM , ai_multitonInst)
    {}

    iVtObjectDataMask_c::iVtObjectDataMask_c(
    		int ai_multitonInst,
			uint16_t ID,
			uint8_t backgroundColour,
			iVtObjectSoftKeyMask_c *softKeyMask,
			uint8_t numberOfObjectsToFollow,
			const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *objectsToFollow,
			uint8_t numberOfMacrosToFollow,
			const repeat_event_iVtObjectMacro_s *macrosToFollow)
    : iVtObjectDataMask_c(
    		new iVtObjectDataMask_s(
    				ID,
    				backgroundColour,
    				softKeyMask,
    				numberOfObjectsToFollow,
    				objectsToFollow,
    				numberOfMacrosToFollow,
    				macrosToFollow),
			ai_multitonInst)
    {

    }
}
