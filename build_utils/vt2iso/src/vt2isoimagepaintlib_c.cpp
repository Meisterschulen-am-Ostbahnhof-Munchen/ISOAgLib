/***************************************************************************
                          vt2isoimagepaintlib_c.cpp  -  image handling
                          for vt2is with paintlib library
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
#include "vt2isoimagepaintlib_c.h"



/** constructor which can directly open the corresponding file */
Vt2IsoImagePaintlib_c::Vt2IsoImagePaintlib_c( const char* filename )
	: Vt2IsoImageBase_c( ), i_curScanLineY( -1 )
{
	if ( filename != NULL )
	{
		if ( openBitmap( filename ) ) getOptimalBwThreshold();
	}
}
/** open the given bitmap file and guarantee
	that the pixels can be accessed by RGB
	( i.e. convert if indexed )
	@return true -> opened and converted correct
*/
bool Vt2IsoImagePaintlib_c::openBitmap( const char* filename )
{
    bool result = false;
	reset();
	i_curScanLineY = -1;
	
    plDecoder.MakeBmpFromFile (filename, &plBitmap);
	if (1)
    {
        result = true;
        printf ("Opening %s...\n", filename);

	    // Get the addresses of the bitmap lines
	    pLineArray = plBitmap.GetLineArray32();

	    ui_width  = plBitmap.GetWidth();
	    ui_height = plBitmap.GetHeight();
	    i_currentThreshold = 128;
    }
    return result;
}
void Vt2IsoImagePaintlib_c::close( void )
{ // simply reset
	reset();
}

/** check and adopt scanline */
void Vt2IsoImagePaintlib_c::checkUpdateScanline( unsigned int rui_y )
{
	if ( i_curScanLineY != rui_y )
	{ // read scanline for given y
		pLine = pLineArray[rui_y];
	}
}
/** deliver R-value of bitmap at given position */
unsigned int Vt2IsoImagePaintlib_c::getR( unsigned int rui_x, unsigned int rui_y )
{
	if ( ( rui_x >= ui_width ) || ( rui_y >= ui_height ) ) return 0;
	checkUpdateScanline( rui_y );
	return pLine[ rui_x ].GetR();
}

/** deliver G-value of bitmap at given position */
unsigned int Vt2IsoImagePaintlib_c::getG( unsigned int rui_x, unsigned int rui_y )
{
	if ( ( rui_x >= ui_width ) || ( rui_y >= ui_height ) ) return 0;
	checkUpdateScanline( rui_y );
	return pLine[ rui_x ].GetG();
}

/** deliver B-value of bitmap at given position */
unsigned int Vt2IsoImagePaintlib_c::getB( unsigned int rui_x, unsigned int rui_y )
{
	if ( ( rui_x >= ui_width ) || ( rui_y >= ui_height ) ) return 0;
	checkUpdateScanline( rui_y );
	return pLine[ rui_x ].GetB();
}
