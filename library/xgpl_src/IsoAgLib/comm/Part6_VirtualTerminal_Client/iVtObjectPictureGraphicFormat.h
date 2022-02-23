/*
 * iVtObjectPictureGraphicFormat.h
 *
 *  Created on: 23.02.2022
 *      Author: franz
 */

#ifndef COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTPICTUREGRAPHICFORMAT_H_
#define COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTPICTUREGRAPHICFORMAT_H_



namespace IsoAgLib {

/**
 *
 * Picture graphic type:
 * 0 = Monochrome; 8 pixels per byte. Each bit represents a colour palette index of 0 or 1.
 * (“White” colour can vary with display hardware.)
 * 1 = 4 bit colour; 2 colour pixels per byte. Each
 * nibble (4 bits) represents a colour palette
 * index of 0 to 15.
 * 2 = 8 bit colour; 1 colour pixel per byte. Each
 * byte represents a colour palette index of 0 to
 * 255.
 * See Table A.4.
 *
 *
 *
 */


	enum PictureGraphicFormat : uint8_t
	{
		Monochrome  = 0,
		Colour4bit  = 1,
		Colour8bit  = 2,
	};

};

#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTPICTUREGRAPHICFORMAT_H_ */
