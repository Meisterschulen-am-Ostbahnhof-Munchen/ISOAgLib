/***************************************************************************
                          vt2isoimagebase_c.h  -  image handling
                          for vt2iso - base class
                             -------------------
    begin                : Fri Aug 20 2004
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 ***************************************************************************/
#ifndef VT2ISOIMAGEBASE_C_H
#define VT2ISOIMAGEBASE_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */

// ---------------------------------------------------------------------------
//  Abstract base class for image access
// ---------------------------------------------------------------------------
	static const unsigned int color16table [16] [3] = {
	{0x00,0x00,0x00},
	{0xFF,0xFF,0xFF},
	{0x00,0x99,0x00},
	{0x00,0x99,0x99},
	{0x99,0x00,0x00},
	{0x99,0x00,0x99},
	{0x99,0x99,0x00},
	{0xCC,0xCC,0xCC},
	{0x99,0x99,0x99},
	{0x00,0x00,0xFF},
	{0x00,0xFF,0x00},
	{0x00,0xFF,0xFF},
	{0xFF,0x00,0x00},
	{0xFF,0x00,0xFF},
	{0xFF,0xFF,0x00},
	{0x00,0x00,0x99}};

class Vt2IsoImageBase_c
{
 public:
 	/** constructor which can directly open the corresponding file */
	Vt2IsoImageBase_c( void );
	void reset( void );
	void resetLengths( void );
	virtual void close( void );
 	/** open the given bitmap file and guarantee
		that the pixels can be accessed by RGB
		( i.e. convert if indexed )
		@return true -> opened and converted correct
	*/
	virtual bool openBitmap( const char* filename ) = 0;
	/** deliver the widht of current opened bitmap ( 0 == no open bitmap ) */
	unsigned int getWidth( void ) const { return ui_width;};
	/** deliver the height of current opened bitmap ( 0 == no open bitmap ) */
	unsigned int getHeight( void ) const { return ui_height;};
	/** deliver the b/w thresholded value at given bitmap position
		( calculate the optimal bitmap threshold if not yet defined )
	 */
	unsigned int get1BitPixel( unsigned int rui_x, unsigned int rui_y );
	/** get the ISO virtual table indexed bitmap value for 4Bit ( 16color ) target bitmap */
	unsigned int get4BitPixel( unsigned int rui_x, unsigned int rui_y );
	/** get the ISO virtual table indexed bitmap value for 8Bit ( 256color ) target bitmap */
	unsigned int get8BitPixel( unsigned int rui_x, unsigned int rui_y );

	/** write the Bitmap to the given buffer and return amount of written Bytes */
	unsigned int write1BitBitmap( unsigned char* pui_bitmap, unsigned int rui_maxSize = ( 480 * 480 ) );
	/** write the Bitmap to the given buffer and return amount of written Bytes */
	unsigned int write4BitBitmap( unsigned char* pui_bitmap, unsigned int rui_maxSize = ( 480 * 480 ) );
	/** write the Bitmap to the given buffer and return amount of written Bytes */
	unsigned int write8BitBitmap( unsigned char* pui_bitmap, unsigned int rui_maxSize = ( 480 * 480 ) );
	unsigned int objRawBitmapBytes [3];
 protected:
	/** deliver R-value of bitmap at given position */
	virtual unsigned int getR( unsigned int rui_x, unsigned int rui_y ) = 0;
	/** deliver G-value of bitmap at given position */
	virtual unsigned int getG( unsigned int rui_x, unsigned int rui_y ) = 0;
	/** deliver B-value of bitmap at given position */
	virtual unsigned int getB( unsigned int rui_x, unsigned int rui_y ) = 0;

	/** calculate the optimal threshold for conversion to b/w */
	void getOptimalBwThreshold( void );
	/** deliver 4Bit palette value */
	unsigned int indexfromtable (unsigned int red, unsigned int green, unsigned int blue);
	/** dliver 16Bit Palette value for given R/G/B values */
	unsigned int rgbtopalette16 (unsigned int red, unsigned int green, unsigned int blue);
	unsigned int componenttoindex6 (unsigned int component);
	unsigned int componentto09CF (unsigned int comp);
	unsigned int componentto09F (unsigned int comp);

	#if 0
	/** get 4Bit palette pixel value for selected pixel */
	unsigned int get4BitPalettePixel( unsigned int rui_x, unsigned int rui_y ) = 0;
	/** get 8Bit palette pixel value for selected pixel */
	unsigned int get8BitPalettePixel( unsigned int rui_x, unsigned int rui_y ) = 0;
	#endif
	/** current threshold for b/w derive - default -1 == not yet calculated */
	int i_currentThreshold;
	/** width of bitmap */
	unsigned int ui_width;
	/** height of bitmap */
	unsigned int ui_height;
};

#endif
