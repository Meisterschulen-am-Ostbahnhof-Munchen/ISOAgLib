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

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_picturegraphic


