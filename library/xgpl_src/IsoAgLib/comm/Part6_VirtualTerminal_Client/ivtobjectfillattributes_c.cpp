/*
 * ivtobjectfillattributes_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectfillattributes_c.h"

#include <cstdint>

#include "ivtclientobjectpool_c.h"
#include "ivttypes.h"

#ifdef CONFIG_USE_VTOBJECT_fillattributes

namespace IsoAgLib {


    iVtObjectFillAttributes_c::~iVtObjectFillAttributes_c() = default;

    ObjectType iVtObjectFillAttributes_c::objectType() { return VT_OBJECT_TYPE_FILL_ATTRIBUTES; }

    void iVtObjectFillAttributes_c::setFillType(FillType newFillType, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectFillAttributes_c::setFillType(newFillType, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectFillAttributes_c::setFillColour(Colour newFillColour, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectFillAttributes_c::setFillColour(newFillColour, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectFillAttributes_c::setFillPattern(iVtObjectPictureGraphic_c *newFillPatternObject, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectFillAttributes_c::setFillPattern(newFillPatternObject, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectFillAttributes_c::setFillAttributes(FillType newFillType, Colour newFillColour, iVtObjectPictureGraphic_c *newFillPattern, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectFillAttributes_c::setFillAttributes(newFillType, newFillColour, newFillPattern, b_updateObject, b_enableReplaceOfCmd);
    }

    FillType iVtObjectFillAttributes_c::updateFillType(bool b_SendRequest) {
        return vtObjectFillAttributes_c::updateFillType(b_SendRequest);
    }

    Colour iVtObjectFillAttributes_c::updateFillColour(bool b_SendRequest) {
        return vtObjectFillAttributes_c::updateFillColour(b_SendRequest);
    }

    iVtObjectPictureGraphic_c *iVtObjectFillAttributes_c::updateFillPattern(bool b_SendRequest) {
        return vtObjectFillAttributes_c::updateFillPattern(b_SendRequest);
    }

    ObjectType iVtObjectFillAttributes_c::getObjectType() const { return objectType(); }

iVtObjectFillAttributes_c::iVtObjectFillAttributes_c(
		iVtClientObjectPool_c *pool,
		ObjectID ID,
		FillType fillType,
		Colour fillColour,
		iVtObjectPictureGraphic_c *fillPatternObject)
:vtObjectFillAttributes_c(
		        pool->getAiMultitonInst(),
				ID,
				fillType,
				fillColour,
				fillPatternObject)
    {
    	pool->Append(this);
    }

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_fillattributes



