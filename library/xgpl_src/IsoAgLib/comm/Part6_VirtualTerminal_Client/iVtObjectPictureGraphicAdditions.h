/*
 * iVtObjectPictureGraphicAdditions.h
 *
 *  Created on: 23.02.2022
 *      Author: franz
 */

#ifndef COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTPICTUREGRAPHICADDITIONS_H_
#define COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTPICTUREGRAPHICADDITIONS_H_


struct repeat_rawData_rawBytes_actWidth_actHeight_formatoptions_s {
  const uint8_t* rawData;
  uint32_t numberOfBytesInRawData;
  uint16_t actualWidth;
  uint16_t actualHeight;
  uint8_t formatoptions; /* format is bit 8+7, options is bit 2-0 (NO RLE1/4/8 stuff here!! */
};


#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTPICTUREGRAPHICADDITIONS_H_ */
