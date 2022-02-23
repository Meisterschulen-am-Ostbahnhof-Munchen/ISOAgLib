/*
 * ivtobjectpicturegraphic_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectpicturegraphic_c.h"

#ifdef CONFIG_USE_VTOBJECT_picturegraphic

namespace IsoAgLib {

iVtObjectPictureGraphic_c::~iVtObjectPictureGraphic_c() = default;

iVtObjectPictureGraphic_c::iVtObjectPictureGraphic_c(
		iVtClientObjectPool_c *pool,
		ObjectID ID,
		uint16_t width,
		uint16_t actualWidth,
		uint16_t actualHeight,
		PictureGraphicFormat format,
		iVtObjectPictureGraphicOptions options,
		Colour transparencyColour,
		uint32_t numberOfBytesInRawData0,
		const uint8_t *rawData0,
		uint32_t numberOfBytesInRawData1,
		const uint8_t *rawData1,
		uint32_t numberOfBytesInRawData2,
		const uint8_t *rawData2,
		uint8_t numberOfFixedBitmapsToFollow,
		const repeat_rawData_rawBytes_actWidth_actHeight_formatoptions_s *fixedBitmapsToFollow)
:vtObjectPictureGraphic_c(
		pool->getAiMultitonInst(),
		ID,
		width,
		actualWidth,
		actualHeight,
		format,
		options,
		transparencyColour,
		numberOfBytesInRawData0,
		rawData0,
		numberOfBytesInRawData1,
		rawData1,
		numberOfBytesInRawData2,
		rawData2,
		numberOfFixedBitmapsToFollow,
		fixedBitmapsToFollow)
{
	pool->Append(this);
}

    ObjectType iVtObjectPictureGraphic_c::objectType() { return VT_OBJECT_TYPE_PICTURE_GRAPHIC; }

    void iVtObjectPictureGraphic_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectPictureGraphic_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectPictureGraphic_c::setOptions(iVtObjectPictureGraphicOptions newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectPictureGraphic_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectPictureGraphic_c::setTransparencyColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectPictureGraphic_c::setTransparencyColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectPictureGraphic_c::setRawData0(uint8_t *newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth, uint16_t aui16_actHeight, uint16_t aui16_width) {
        vtObjectPictureGraphic_c::setRawData0 (newValue, aui32_size, ab_rle, aui16_actWidth, aui16_actHeight, aui16_width);
    }

    void iVtObjectPictureGraphic_c::setRawData1(uint8_t *newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth, uint16_t aui16_actHeight, uint16_t aui16_width) {
        vtObjectPictureGraphic_c::setRawData1 (newValue, aui32_size, ab_rle, aui16_actWidth, aui16_actHeight, aui16_width);
    }

    void iVtObjectPictureGraphic_c::setRawData2(uint8_t *newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth, uint16_t aui16_actHeight, uint16_t aui16_width) {
        vtObjectPictureGraphic_c::setRawData2 (newValue, aui32_size, ab_rle, aui16_actWidth, aui16_actHeight, aui16_width);
    }

    uint16_t iVtObjectPictureGraphic_c::updateWidth(bool b_SendRequest) {
        return vtObjectPictureGraphic_c::updateWidth(b_SendRequest);
    }

    iVtObjectPictureGraphicOptions iVtObjectPictureGraphic_c::updateOptions(bool b_SendRequest) {
        return vtObjectPictureGraphic_c::updateOptions(b_SendRequest);
    }

    Colour iVtObjectPictureGraphic_c::updateTransparencyColour(bool b_SendRequest) {
        return vtObjectPictureGraphic_c::updateTransparencyColour(b_SendRequest);
    }

    ObjectType iVtObjectPictureGraphic_c::getObjectType() const { return objectType(); }

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_picturegraphic


