/***************************************************************************
                          vt2isoimagefreeimage_c.cpp  -  image handling
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
#if defined( WIN32 ) && defined( __GNUC__ )
 // MinGW as gcc compiler on Win32 platform
 // include header to get DWORD and corresponding types
 #include <windef.h>
 #include <wingdi.h>
#endif

#include "vt2isoimagefreeimage_c.h"
#include <iostream>

bool Vt2IsoImageFreeImage_c::b_FreeImageLibInited = false;

/** constructor which can directly open the corresponding file */
Vt2IsoImageFreeImage_c::Vt2IsoImageFreeImage_c( const char* filename )
 : Vt2IsoImageBase_c( ), i_curScanLineY( -1 )
{
 if ( ! b_FreeImageLibInited )
 {
  b_FreeImageLibInited = true;
  FreeImage_Initialise();
 }
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
bool Vt2IsoImageFreeImage_c::openBitmap( const char* filename )
{
 reset();
 i_curScanLineY = -1;

 std::cout << "Opening " << filename << std::endl;
 FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

 // check the file signature and deduce its format
 // (the second argument is currently not used by FreeImage)
 fif = FreeImage_GetFileType(filename, 0);
 if(fif == FIF_UNKNOWN) {
  // no signature ?
  // try to guess the file format from the file extension
  fif = FreeImage_GetFIFFromFilename(filename);
  std::cerr << "Dateityp nicht bekannt" << std::endl;
 }
 // check that the plugin has reading capabilities ...
 if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
  // ok, let's load the file
  bitmap = FreeImage_Load(fif, filename, 0);
 }
 else
 { // hardcore - try with BMP
  std::cerr << "Try with BMP" << std::endl;
  bitmap = FreeImage_Load(FIF_BMP, filename, 0);
  if ( bitmap == NULL )
  { // next hardcoretry with png
    std::cerr << "Try with PNG" << std::endl;
    bitmap = FreeImage_Load(FIF_PNG, filename, 0);
  }
  if ( bitmap == NULL )
  { // next hardcoretry with jpg
    std::cerr << "Try with JPEG" << std::endl;
    bitmap = FreeImage_Load(FIF_JPEG, filename, 0);
  }
  if ( bitmap == NULL )
  {
   std::cerr << "Fehler beim Laden" << std::endl;
   return false;
  }
  else std::cerr << "Erfolgreich mit BMP geladen" << std::endl;
 }

 // convert to 32-Bit standard bitmap
 FIBITMAP *tmp = FreeImage_ConvertTo32Bits(bitmap);
 FreeImage_Unload(bitmap);
 bitmap = tmp;

 ui_width  = FreeImage_GetWidth(bitmap);
 ui_height = FreeImage_GetHeight(bitmap);
 // Calculate the number of bytes per pixel (3 for 24-bit or 4 for 32-bit)
 bytespp = FreeImage_GetLine(bitmap) / FreeImage_GetWidth(bitmap);
 return true;
}
void Vt2IsoImageFreeImage_c::close( void )
{ // unload bitmap from memory
 FreeImage_Unload(bitmap);
 reset();
}
/** check and adapt scanline */
void Vt2IsoImageFreeImage_c::checkUpdateScanline( unsigned int rui_y )
{
 if ( i_curScanLineY != rui_y )
 { // read scanline for given y
  // ( FreeImage library documentation states, that first scanline in memory is
  //   bottommost -> i.e. upsidedown in relation to other modellings
  //    -> change direction back to usual with ( ui_height - rui_y ) )
  scanline = FreeImage_GetScanLine(bitmap, ( (ui_height - 1) - rui_y ) );
  i_curScanLineY = rui_y;
 }
}

/** deliver R-value of bitmap at given position */
unsigned int Vt2IsoImageFreeImage_c::getR( unsigned int rui_x, unsigned int rui_y )
{
 if ( ( rui_x >= ui_width ) || ( rui_y >= ui_height ) ) return 0;
 #if defined( WIN32 )
 RGBQUAD temp_pixel;
 // ( FreeImage library documentation states, that first scanline in memory is
 //   bottommost -> i.e. upsidedown in relation to other modellings
 //    -> change direction back to usual with ( ui_height - rui_y ) )
 FreeImage_GetPixelColor(bitmap, rui_x, ( (ui_height - 1) - rui_y ), &temp_pixel);
 return temp_pixel.rgbRed;
 #else
 checkUpdateScanline( rui_y );
 return scanline[FI_RGBA_RED + ( bytespp * rui_x )];
 #endif
}

/** deliver G-value of bitmap at given position */
unsigned int Vt2IsoImageFreeImage_c::getG( unsigned int rui_x, unsigned int rui_y )
{
 if ( ( rui_x >= ui_width ) || ( rui_y >= ui_height ) ) return 0;
 #if defined( WIN32 )
 RGBQUAD temp_pixel;
 // ( FreeImage library documentation states, that first scanline in memory is
 //   bottommost -> i.e. upsidedown in relation to other modellings
 //    -> change direction back to usual with ( ui_height - rui_y ) )
 FreeImage_GetPixelColor(bitmap, rui_x, ( (ui_height - 1) - rui_y ), &temp_pixel);
 return temp_pixel.rgbGreen;
 #else
 checkUpdateScanline( rui_y );
 return scanline[FI_RGBA_GREEN + ( bytespp * rui_x )];
 #endif
}

/** deliver B-value of bitmap at given position */
unsigned int Vt2IsoImageFreeImage_c::getB( unsigned int rui_x, unsigned int rui_y )
{
 if ( ( rui_x >= ui_width ) || ( rui_y >= ui_height ) ) return 0;
 #if defined( WIN32 )
 RGBQUAD temp_pixel;
 // ( FreeImage library documentation states, that first scanline in memory is
 //   bottommost -> i.e. upsidedown in relation to other modellings
 //    -> change direction back to usual with ( ui_height - rui_y ) )
 FreeImage_GetPixelColor(bitmap, rui_x, ( (ui_height - 1) - rui_y ), &temp_pixel);
 return temp_pixel.rgbBlue;
 #else
 checkUpdateScanline( rui_y );
 return scanline[FI_RGBA_BLUE + ( bytespp * rui_x )];
 #endif
}
