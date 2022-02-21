/*
 * ivttypes.cpp
 *
 *  Created on: 28.01.2022
 *      Author: franz
 */


#include "ivttypes.h"

namespace IsoAgLib {

    repeat_iVtObject_x_y_s::repeat_iVtObject_x_y_s(
    		iVtObject_c *vtObject,
			int16_t x,
			int16_t y)
      : vtObject(vtObject)
	  , x(x)
	  , y(y)
    {}

    repeat_iVtObjectFontAttributes_row_col_s::repeat_iVtObjectFontAttributes_row_col_s(
    		iVtObjectFontAttributes_c *vtObjectBlockFont,
			int16_t row,
			int16_t col)
	  : vtObjectBlockFont(vtObjectBlockFont)
	  , row(row)
	  , col(col)
    {}

    iAux2InputData::iAux2InputData()
            : name(),
              modelIdentificationCode(0xFFFF),
              uid(0xFFFF),
              preferredAssignmentMatched(false), // a matching input object is found in an active joy stick ECU
              preserve(false)                    // if CONFIG_MAX_AUX2_PREFERRED_ASSIGNMENT_PER_FUNCTION is reached
    // => old preferred assignments candidates with preserve == false will be removed
    {}

    iAux2InputData::iAux2InputData(const iIsoName_c &a_name, uint16_t a_modelIdentificationCode, uint16_t a_uid, bool a_preserve)
            : name(a_name),
              modelIdentificationCode(a_modelIdentificationCode),
              uid(a_uid),
              preferredAssignmentMatched(false),
              preserve(a_preserve)
    {}

    bool iAux2InputData::operator==(const iAux2InputData &arc_ref) const {
        // - check for preferredAssignmentMatched and preserve is not necessary
        if ( (name != arc_ref.name) ||
             (modelIdentificationCode != arc_ref.modelIdentificationCode) ||
             (uid != arc_ref.uid) )
            return false;
        else
            return true;
    }

    bool iAux2InputData::operator!=(const iAux2InputData &arc_ref) const {
        return !(operator==(arc_ref));
    }

    iAux2FunctionInputAssignment_t::iAux2FunctionInputAssignment_t()
    : functionUid()
    , input();
    {}
}
