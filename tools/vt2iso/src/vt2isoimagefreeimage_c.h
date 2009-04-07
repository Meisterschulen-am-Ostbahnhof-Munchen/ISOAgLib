/***************************************************************************
                          vt2isoimagefreeimage_c.h  -  image handling
                          for vt2iso with paintlib library
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
#ifndef VT2ISOIMAGEFREEIMAGE_C_H
#define VT2ISOIMAGEFREEIMAGE_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <FreeImage.h>

#include "vt2isoimagebase_c.h"

// ---------------------------------------------------------------------------
//  Concrete class for image access with Paintlib
// ---------------------------------------------------------------------------
class Vt2IsoImageFreeImage_c : public Vt2IsoImageBase_c
{
 public:
 	/** constructor which can directly open the corresponding file */
	Vt2IsoImageFreeImage_c( const char* filename = NULL );
	
	void printLicenseText();
	
	virtual void close( void );
 	/** open the given bitmap file and guarantee
		that the pixels can be accessed by RGB
		( i.e. convert if indexed )
		@return true -> opened and converted correct
	*/
	virtual bool openBitmap( const char* filename );
 protected:
	/** deliver R-value of bitmap at given position */
	virtual unsigned int getR( unsigned int aui_x, unsigned int aui_y );
	/** deliver G-value of bitmap at given position */
	virtual unsigned int getG( unsigned int aui_x, unsigned int aui_y );
	/** deliver B-value of bitmap at given position */
	virtual unsigned int getB( unsigned int aui_x, unsigned int aui_y );

  int getPaletteIndex (unsigned int aui_x, unsigned int aui_y);

 private:
	/** check and adapt scanline */
	void checkUpdateScanline( unsigned int aui_y );

	FIBITMAP *bitmap;
	BYTE *scanline;
	int bytespp;
	int i_curScanLineY;
	static bool b_FreeImageLibInited;
  
  bool mb_palettized;
  int mi_colourMismatch; // <0 for NO MISMATCH, 0..255 for the colour index mismatching!
};

#endif
