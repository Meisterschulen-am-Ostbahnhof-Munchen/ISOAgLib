/*
  vt2isoimagepaintlib_c.cpp: image handling for vt2is with
    paintlib library

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

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


int Vt2IsoImagePaintlib_c::getPaletteIndex (unsigned int aui_x, unsigned int aui_y)
{
  /** @todo ON REQUEST: See if we can retrieve the palette index via paintlib, too.. Note: Paintlib is not supported anymore! */
  return -1;
}


/** check and adapt scanline */
void Vt2IsoImagePaintlib_c::checkUpdateScanline( unsigned int aui_y )
{
	if ( i_curScanLineY != aui_y )
	{ // read scanline for given y
		pLine = pLineArray[aui_y];
	}
}
/** deliver R-value of bitmap at given position */
unsigned int Vt2IsoImagePaintlib_c::getR( unsigned int aui_x, unsigned int aui_y )
{
	if ( ( aui_x >= ui_width ) || ( aui_y >= ui_height ) ) return 0;
	checkUpdateScanline( aui_y );
	return pLine[ aui_x ].GetR();
}

/** deliver G-value of bitmap at given position */
unsigned int Vt2IsoImagePaintlib_c::getG( unsigned int aui_x, unsigned int aui_y )
{
	if ( ( aui_x >= ui_width ) || ( aui_y >= ui_height ) ) return 0;
	checkUpdateScanline( aui_y );
	return pLine[ aui_x ].GetG();
}

/** deliver B-value of bitmap at given position */
unsigned int Vt2IsoImagePaintlib_c::getB( unsigned int aui_x, unsigned int aui_y )
{
	if ( ( aui_x >= ui_width ) || ( aui_y >= ui_height ) ) return 0;
	checkUpdateScanline( aui_y );
	return pLine[ aui_x ].GetB();
}
