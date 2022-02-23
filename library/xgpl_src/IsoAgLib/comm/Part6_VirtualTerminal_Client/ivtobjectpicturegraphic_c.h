/*
  ivtobjectpicturegraphic_c.h

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTPICTUREGRAPHIC_C_H
#define IVTOBJECTPICTUREGRAPHIC_C_H

#include "impl/vtobjectpicturegraphic_c.h"

#ifdef CONFIG_USE_VTOBJECT_picturegraphic

namespace IsoAgLib {


class iVtObjectPictureGraphic_c : public __IsoAgLib::vtObjectPictureGraphic_c
{
public:

	iVtObjectPictureGraphic_c() = delete;
	~iVtObjectPictureGraphic_c() override;


	explicit iVtObjectPictureGraphic_c(
			    iVtClientObjectPool_c* pool,
				ObjectID ID = autoID,
				uint16_t width = 100,
				uint16_t actualWidth = 100,
				uint16_t actualHeight = 100,
				PictureGraphicFormat format = Colour8bit,
				iVtObjectPictureGraphicOptions options = iVtObjectPictureGraphicOptions(allDrawPicture),
				Colour transparencyColour = WHITE,
				uint32_t numberOfBytesInRawData0 = 0,
				const uint8_t *rawData0 = nullptr,
				uint32_t numberOfBytesInRawData1 = 0,
				const uint8_t *rawData1 = nullptr,
				uint32_t numberOfBytesInRawData2 = 0,
				const uint8_t *rawData2 = nullptr,
				uint8_t numberOfFixedBitmapsToFollow = 0,
				const repeat_rawData_rawBytes_actWidth_actHeight_formatoptions_s *fixedBitmapsToFollow = nullptr);




  static ObjectType objectType();
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setOptions(iVtObjectPictureGraphicOptions newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setTransparencyColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  void setRawData0 (uint8_t* newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth=0xFFFF, uint16_t aui16_actHeight=0xFFFF, uint16_t aui16_width=0xFFFF) override;
  void setRawData1 (uint8_t* newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth=0xFFFF, uint16_t aui16_actHeight=0xFFFF, uint16_t aui16_width=0xFFFF) override;
  void setRawData2 (uint8_t* newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth=0xFFFF, uint16_t aui16_actHeight=0xFFFF, uint16_t aui16_width=0xFFFF) override;
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectPictureGraphic_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) override;

  iVtObjectPictureGraphicOptions updateOptions(bool b_SendRequest=false) override;

  Colour updateTransparencyColour(bool b_SendRequest=false) override;

  /** these attributes are in parentheses in the spec, so commented out here
  uint16_t updateActualWidth(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateActualWidth(b_SendRequest);
  }

  uint16_t updateActualHeight(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateActualHeight(b_SendRequest);
  }

  uint8_t updateFormat(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateFormat(b_SendRequest);
  }
  */
#endif

  ObjectType getObjectType() const override;
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_picturegraphic

#endif //IVTOBJECTPICTUREGRAPHIC_C_H
