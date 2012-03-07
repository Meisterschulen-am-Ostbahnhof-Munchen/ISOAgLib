/*
  vt2isoimagepaintlib_c.h: image handling for vt2iso with
    paintlib library

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef VT2ISOIMAGEPAINTLIB_C_H
#define VT2ISOIMAGEPAINTLIB_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#ifdef WIN32
	#include <paintlib/common/planydec.h>
	#include <paintlib/common/planybmp.h>
#else
	#include <paintlib/planydec.h>
	#include <paintlib/planybmp.h>
#endif

#include "vt2isoimagebase_c.h"

// ---------------------------------------------------------------------------
//  Concrete class for image access with Paintlib
// ---------------------------------------------------------------------------
class Vt2IsoImagePaintlib_c : public Vt2IsoImageBase_c
{
 public:
 	/** constructor which can directly open the corresponding file */
	Vt2IsoImagePaintlib_c( const char* filename = NULL );
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

 private:
	/** check and adapt scanline */
	void checkUpdateScanline( unsigned int aui_y );

	// Paintlib Stuff
	PLAnyPicDecoder plDecoder;
	PLAnyBmp plBitmap;
	PLPixel32 ** pLineArray;
	PLPixel32  * pLine;
	int i_curScanLineY;
};

#endif
