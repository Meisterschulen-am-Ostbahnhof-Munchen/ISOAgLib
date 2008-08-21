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
#include <iomanip>
#include <vt2iso.hpp>


typedef unsigned char fiuint8_t;
typedef unsigned int fiuint16_t;

extern BGR_s vtColourTable[256];

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


void Vt2IsoImageFreeImage_c::printLicenseText()
{
  std::cout << "This software uses the FreeImage open source image library. See http://freeimage.sourceforge.net for details." << std::endl
            << "FreeImage is used under the (GNU GPL or FIPL), version (license version). " << std::endl;
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

 if (vt2iso_c::isVerbose())
   std::cout << std::endl << "Opening " << filename << std::endl;
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

 if (FreeImage_GetBPP(bitmap) > 8)
 { // convert to 32-Bit standard bitmap ONLY if NOT palettized!
   FIBITMAP *tmp = FreeImage_ConvertTo32Bits(bitmap);
   FreeImage_Unload(bitmap);
   bitmap = tmp;
 }

 ui_width  = FreeImage_GetWidth(bitmap);
 ui_height = FreeImage_GetHeight(bitmap);
 // Calculate the number of bytes per pixel (3 for 24-bit or 4 for 32-bit)
 bytespp = FreeImage_GetLine(bitmap) / FreeImage_GetWidth(bitmap);

 mb_palettized = false;

 if (FreeImage_GetBPP(bitmap) <= 8)
 {
   mi_colourMismatch = -1; // default: no colour mismatches...

   for (int i=0; i<(16+216); i++)
   { // check all colours if the bitmap uses the ISO-palette
     if ( (vtColourTable[i].bgrRed != FreeImage_GetPalette (bitmap)[i].rgbRed)
       || (vtColourTable[i].bgrGreen != FreeImage_GetPalette (bitmap)[i].rgbGreen)
       || (vtColourTable[i].bgrBlue != FreeImage_GetPalette (bitmap)[i].rgbBlue) )
     {
       mi_colourMismatch = i;
       break;
     }
   }
   mb_palettized = true;
   if (vt2iso_c::isVerbose())
     std::cout << " palettized (depth="<<FreeImage_GetBPP(bitmap)<<"). ";
   return true;
 }
 else
 {
   if (vt2iso_c::isVerbose())
     std::cout << " as RGB (depth="<<FreeImage_GetBPP(bitmap)<<"). ";
   return true;
 }
}

// this function is only called for the 8bit case!
int Vt2IsoImageFreeImage_c::getPaletteIndex (unsigned int aui_x, unsigned int aui_y)
{
  if (mb_palettized)
  {
    // do this check here, because in case we only use 4bit bitmap, we don't have to care for the palette matching...
    if (mi_colourMismatch >= 0)
    {
      std::cerr << "*** COULDN'T LOAD BITMAP: WRONG PALETTE. See (first) mismatching colour #"<<mi_colourMismatch<<" below. Please use the ISO11783-Part 6 (VT)-Palette for bitmaps you have saved palettized and use in 8bit-mode. Use 'vt2iso -p' to generate an .act file and resample your bitmap to use this palette! ***" << std::endl;
      std::cerr << "HAS TO BE | you had" << std::hex << std::setfill('0');
      for (int i=0; i<(16+216); i++)
      {
        if ((i % 8) == 0) std::cerr << std::endl;
        else std::cerr << "     ";
        std::cerr << std::setw(2) << fiuint16_t(vtColourTable[i].bgrRed) << std::setw(2) << fiuint16_t(vtColourTable[i].bgrGreen) << std::setw(2) << fiuint16_t(vtColourTable[i].bgrBlue);
        if (i == mi_colourMismatch) std::cerr << "*|*"; else std::cerr << " | ";
        std::cerr << std::setw(2) << fiuint16_t(FreeImage_GetPalette (bitmap)[i].rgbRed) << std::setw(2) << fiuint16_t(FreeImage_GetPalette (bitmap)[i].rgbGreen) << std::setw(2) << fiuint16_t(FreeImage_GetPalette (bitmap)[i].rgbBlue);
      }
      std::cerr << std::endl;
      b_isInvalidPalette = true;
      return -1;
    }

    fiuint8_t idx;
    FreeImage_GetPixelIndex (bitmap, aui_x, (ui_height - 1) - aui_y, &idx);
    return idx;
  }
  else return -1;
}


void Vt2IsoImageFreeImage_c::close( void )
{ // unload bitmap from memory
 FreeImage_Unload(bitmap);
 reset();
}


/** check and adapt scanline */
void Vt2IsoImageFreeImage_c::checkUpdateScanline( unsigned int aui_y )
{
 if ( i_curScanLineY != int(aui_y) )
 { // read scanline for given y
  // ( FreeImage library documentation states, that first scanline in memory is
  //   bottommost -> i.e. upsidedown in relation to other modellings
  //    -> change direction back to usual with ( ui_height - aui_y ) )
  scanline = FreeImage_GetScanLine(bitmap, ( (ui_height - 1) - aui_y ) );
  i_curScanLineY = aui_y;
 }
}

/** deliver R-value of bitmap at given position */
unsigned int Vt2IsoImageFreeImage_c::getR( unsigned int aui_x, unsigned int aui_y )
{
 if ( ( aui_x >= ui_width ) || ( aui_y >= ui_height ) ) return 0;

 if (mb_palettized)
 { // we can't raw-access the bitmap buffer with RGB, we need to get the RGB via the palette-index's colour
   fiuint8_t idx;
   FreeImage_GetPixelIndex (bitmap, aui_x, (ui_height - 1) - aui_y, &idx);
   return vtColourTable[idx].bgrRed;
 }
 else
 {
  #if defined( WIN32 )
  RGBQUAD temp_pixel;
  // ( FreeImage library documentation states, that first scanline in memory is
  //   bottommost -> i.e. upsidedown in relation to other modellings
  //    -> change direction back to usual with ( ui_height - aui_y ) )
  FreeImage_GetPixelColor(bitmap, aui_x, ( (ui_height - 1) - aui_y ), &temp_pixel);
  return temp_pixel.rgbRed;
  #else
  checkUpdateScanline( aui_y );
  return scanline[FI_RGBA_RED + ( bytespp * aui_x )];
  #endif
 }
}

/** deliver G-value of bitmap at given position */
unsigned int Vt2IsoImageFreeImage_c::getG( unsigned int aui_x, unsigned int aui_y )
{
 if (mb_palettized)
 { // we can't raw-access the bitmap buffer with RGB, we need to get the RGB via the palette-index's colour
   fiuint8_t idx;
   FreeImage_GetPixelIndex (bitmap, aui_x, (ui_height - 1) - aui_y, &idx);
   return vtColourTable[idx].bgrGreen;
 }
 else
 {
  if ( ( aui_x >= ui_width ) || ( aui_y >= ui_height ) ) return 0;
  #if defined( WIN32 )
  RGBQUAD temp_pixel;
  // ( FreeImage library documentation states, that first scanline in memory is
  //   bottommost -> i.e. upsidedown in relation to other modellings
  //    -> change direction back to usual with ( ui_height - aui_y ) )
  FreeImage_GetPixelColor(bitmap, aui_x, ( (ui_height - 1) - aui_y ), &temp_pixel);
  return temp_pixel.rgbGreen;
  #else
  checkUpdateScanline( aui_y );
  return scanline[FI_RGBA_GREEN + ( bytespp * aui_x )];
  #endif
 }
}

/** deliver B-value of bitmap at given position */
unsigned int Vt2IsoImageFreeImage_c::getB( unsigned int aui_x, unsigned int aui_y )
{
 if (mb_palettized)
 { // we can't raw-access the bitmap buffer with RGB, we need to get the RGB via the palette-index's colour
   fiuint8_t idx;
   FreeImage_GetPixelIndex (bitmap, aui_x, (ui_height - 1) - aui_y, &idx);
   return vtColourTable[idx].bgrBlue;
 }
 else
 {
  if ( ( aui_x >= ui_width ) || ( aui_y >= ui_height ) ) return 0;
  #if defined( WIN32 )
  RGBQUAD temp_pixel;
  // ( FreeImage library documentation states, that first scanline in memory is
  //   bottommost -> i.e. upsidedown in relation to other modellings
  //    -> change direction back to usual with ( ui_height - aui_y ) )
  FreeImage_GetPixelColor(bitmap, aui_x, ( (ui_height - 1) - aui_y ), &temp_pixel);
  return temp_pixel.rgbBlue;
  #else
  checkUpdateScanline( aui_y );
  return scanline[FI_RGBA_BLUE + ( bytespp * aui_x )];
  #endif
 }
}
