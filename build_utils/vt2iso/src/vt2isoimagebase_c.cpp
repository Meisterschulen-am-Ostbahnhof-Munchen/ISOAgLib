/***************************************************************************
                          vt2isoimagebase_c.cpp  -  image handling
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
#include "vt2isoimagebase_c.h"
#include <iostream>


/** constructor which can directly open the corresponding file */
Vt2IsoImageBase_c::Vt2IsoImageBase_c( void )
  : i_currentThreshold( -1 ), ui_width( 0 ), ui_height( 0 )
{
}

void Vt2IsoImageBase_c::close( void )
{
}

void Vt2IsoImageBase_c::reset( void )
{
  i_currentThreshold = -1;
  ui_width = 0;
  ui_height = 0;
}


void Vt2IsoImageBase_c::resetLengths( void )
{
  objRawBitmapBytes[0] = 0;
  objRawBitmapBytes[1] = 0;
  objRawBitmapBytes[2] = 0;
}


/** deliver the b/w thresholded value at given bitmap position
  ( calculate the optimal bitmap threshold if not yet defined )
  */
unsigned int Vt2IsoImageBase_c::get1BitPixel( unsigned int rui_x, unsigned int rui_y )
{
  if ( i_currentThreshold < 0 )  getOptimalBwThreshold();
  return ( int( ( getR( rui_x, rui_y ) + getG( rui_x, rui_y ) + getB( rui_x, rui_y ) ) / 3 ) >= i_currentThreshold )?1U:0U;
}

/** get the ISO virtual table indexed bitmap value for 4Bit ( 16color ) target bitmap */
unsigned int Vt2IsoImageBase_c::get4BitPixel( unsigned int rui_x, unsigned int rui_y )
{
  return rgbtopalette16 ( getR( rui_x, rui_y ), getG( rui_x, rui_y ), getB( rui_x, rui_y ) );
}

/** get the ISO virtual table indexed bitmap value for 8Bit ( 256color ) target bitmap */
unsigned int Vt2IsoImageBase_c::get8BitPixel( unsigned int rui_x, unsigned int rui_y )
{
  int idx = getPaletteIndex (rui_x, rui_y);
  if (idx >= 0)
  { // we're palettized!
    // 0..255 possible - directly taken out of the bitmap!
    return idx;
  }
  else if (idx == -1)
  { // we're NOT palettized, calculate a palette index!
    idx = 16 + ( componenttoindex6 ( getR( rui_x, rui_y ) )*36 )
             + ( componenttoindex6 ( getG( rui_x, rui_y ) )*6  )
             + ( componenttoindex6 ( getB( rui_x, rui_y ) )    );
    // 16..231 possible - mapped to this area!
    switch (idx)
    { // now try to map down those colors that exactly match to the range 0..15!
      // because those colours will be used in the 16-color version, too. This makes it easier for the transparency color then!
      case  16: idx = 0; break;
      case 231: idx = 1; break;
      case  34: idx = 2; break;
      case  37: idx = 3; break;
      case 124: idx = 4; break;
      case 127: idx = 5; break;
      case 142: idx = 6; break;
      case 188: idx = 7; break;
      case 145: idx = 8; break;
      case  21: idx = 9; break;
      case  46: idx =10; break;
      case  51: idx =11; break;
      case 196: idx =12; break;
      case 201: idx =13; break;
      case 226: idx =14; break;
      case  19: idx =15; break;
    }
    return idx;
  }
  else if (idx == -2)
  { // we were palettized, but the color table didn't match!
    exit (1);
  }
  else return 0; // make compiler happy!
}

/** write the Bitmap to the given buffer and return amount of written Bytes */
unsigned int Vt2IsoImageBase_c::write1BitBitmap( unsigned char* pui_bitmap, unsigned int rui_maxSize )
{
  objRawBitmapBytes [0] = 0;
  if ( ( i_currentThreshold < 0 ) || ( i_currentThreshold == 128 ) ) getOptimalBwThreshold();

  unsigned int roundedWidth = ( ( getWidth() + 7U ) & ( 0xFFFFFFFFU - 7U ) );

  for (unsigned int ui_y=0; ui_y< getHeight(); ui_y++) {
    for (unsigned int ui_x=0; ui_x < roundedWidth; ui_x+=8) {
      unsigned int pixel1PaletteIndex = get1BitPixel( ui_x, ui_y );
      unsigned int pixel2PaletteIndex = get1BitPixel( ui_x+1, ui_y );
      unsigned int pixel3PaletteIndex = get1BitPixel( ui_x+2, ui_y );
      unsigned int pixel4PaletteIndex = get1BitPixel( ui_x+3, ui_y );
      unsigned int pixel5PaletteIndex = get1BitPixel( ui_x+4, ui_y );;
      unsigned int pixel6PaletteIndex = get1BitPixel( ui_x+5, ui_y );
      unsigned int pixel7PaletteIndex = get1BitPixel( ui_x+6, ui_y );
      unsigned int pixel8PaletteIndex = get1BitPixel( ui_x+7, ui_y );
      unsigned int byte = (pixel1PaletteIndex << 7) + (pixel2PaletteIndex << 6) + (pixel3PaletteIndex << 5) + (pixel4PaletteIndex << 4)
                + (pixel5PaletteIndex << 3) + (pixel6PaletteIndex << 2) + (pixel7PaletteIndex << 1) + pixel8PaletteIndex;
      // avoid overflow
      if ( objRawBitmapBytes [0] > rui_maxSize ) return 0;

      pui_bitmap [objRawBitmapBytes [0]] = byte;
      objRawBitmapBytes [0]++;
    }
  } // iterate loop
  return objRawBitmapBytes [0];
}


/** write the Bitmap to the given buffer and return amount of written Bytes */
unsigned int Vt2IsoImageBase_c::write4BitBitmap( unsigned char* pui_bitmap, unsigned int rui_maxSize )
{
  objRawBitmapBytes [1] = 0;
  unsigned int roundedWidth = ( ( getWidth() + 1U ) & ( 0xFFFFFFFFU - 1U ) );

  for ( unsigned int ui_y=0; ui_y< getHeight(); ui_y++) {
    for (unsigned int ui_x=0; ui_x<roundedWidth; ui_x+=2) {
      unsigned int pixel1PaletteIndex = get4BitPixel( ui_x, ui_y );
      unsigned int pixel2PaletteIndex = get4BitPixel( ui_x+1, ui_y );
      unsigned int byte = (pixel1PaletteIndex << 4) + pixel2PaletteIndex;
      // avoid overflow
      if ( objRawBitmapBytes [1] > rui_maxSize ) return 0;

      pui_bitmap [objRawBitmapBytes [1]] = byte;
      objRawBitmapBytes [1]++;
    }
  } // iterate loop
  return objRawBitmapBytes [1];
}

/** write the Bitmap to the given buffer and return amount of written Bytes */
unsigned int Vt2IsoImageBase_c::write8BitBitmap( unsigned char* pui_bitmap, unsigned int rui_maxSize )
{
  objRawBitmapBytes [2] = 0;
  for ( unsigned int ui_y=0; ui_y< getHeight(); ui_y++) {
    for ( unsigned int ui_x=0; ui_x < getWidth(); ui_x++) {
      unsigned int pixelPaletteIndex = get8BitPixel( ui_x, ui_y );
      // avoid overflow
      if ( objRawBitmapBytes [2] > rui_maxSize ) return 0;

      pui_bitmap [objRawBitmapBytes [2]] = pixelPaletteIndex;
      objRawBitmapBytes [2]++;
    }
  } // iterate loop
  return objRawBitmapBytes [2];
}

/** deliver 4Bit palette value */
unsigned int Vt2IsoImageBase_c::indexfromtable (unsigned int red, unsigned int green, unsigned int blue)
{
  for (int i=0; i<16; i++) {
    if ((color16table [i] [0] == red) && (color16table [i] [1] == green) && (color16table [i] [2] == blue))
      return i;
  }
  return 0; // shouldn't happen!!
}


/** dliver 16Bit Palette value for given R/G/B values */
unsigned int Vt2IsoImageBase_c::rgbtopalette16 (unsigned int red, unsigned int green, unsigned int blue)
{
  red = componentto09CF (red);
  green = componentto09CF (green);
  blue = componentto09CF (blue);
  if ((red <= 0x99) && (green <= 0x99) && (blue <= 0x99)) return indexfromtable (red, green, blue); // 0x00s & 0x99s
  if (((red == 0xFF) || (red == 0x00)) && ((green == 0xFF) || (green == 0x00)) && ((blue == 0xFF) || (blue == 0x00))) return indexfromtable (red, green, blue); // 0x00s & 0xFFs
  if ((red == 0xCC) && (green == 0xCC) && (blue == 0xCC)) return 7; // special-case silver 0xCC 0xCC 0xCC
  red = componentto09F (red);
  green = componentto09F (green);
  blue = componentto09F (blue);
  if ((red == 0xFF) || (green == 0xFF) || (blue == 0xFF)) { // at least one component is 0xFF, so set the others to 0x00
    if (red != 0xFF) red = 0x00;
    if (green != 0xFF) green = 0x00;
    if (blue != 0xFF) blue = 0x00;
  }
  return indexfromtable (red, green, blue);
}


/** calculate the optimal threshold for conversion to b/w */
void Vt2IsoImageBase_c::getOptimalBwThreshold( void )
{
  int threshold128AllWhite = ( ( getWidth() * getHeight() ) / 8 ),
  threshold128AllBlack = ( ( getWidth() * getHeight() ) / 8 ),
  thresholdOptimalAllWhite = ( ( getWidth() * getHeight() ) / 8 ),
  thresholdOptimalAllBlack = ( ( getWidth() * getHeight() ) / 8 ),
  thresholdLoopAllWhite = 0,
  thresholdLoopAllBlack = 0,
  optimalThreshold = 0;
  unsigned int roundedWidth = ( ( getWidth() + 7U ) & ( 0xFFFFFFFFU - 7U ) );
  
  for (unsigned int threshold = 32; threshold <= 224; threshold += 16 ) {
  thresholdLoopAllWhite = 0;
  thresholdLoopAllBlack = 0;
  for (unsigned int ui_y=0; ui_y<getHeight(); ui_y++) {
    for (unsigned int ui_x=0; ui_x<roundedWidth; ui_x+=8) {
      unsigned int pixel1PaletteIndex = ( ( ( getR( ui_x, ui_y ) + getG( ui_x, ui_y ) + getB( ui_x, ui_y ) ) / 3 ) >= threshold )?1:0;
      unsigned int pixel2PaletteIndex = ( ( ( getR( ui_x+1, ui_y ) + getG( ui_x+1, ui_y ) + getB( ui_x+1, ui_y ) ) / 3 ) >= threshold )?1:0;
      unsigned int pixel3PaletteIndex = ( ( ( getR( ui_x+2, ui_y ) + getG( ui_x+2, ui_y ) + getB( ui_x+2, ui_y ) ) / 3 ) >= threshold )?1:0;
      unsigned int pixel4PaletteIndex = ( ( ( getR( ui_x+3, ui_y ) + getG( ui_x+3, ui_y ) + getB( ui_x+3, ui_y ) ) / 3 ) >= threshold )?1:0;
      unsigned int pixel5PaletteIndex = ( ( ( getR( ui_x+4, ui_y ) + getG( ui_x+4, ui_y ) + getB( ui_x+4, ui_y ) ) / 3 ) >= threshold )?1:0;
      unsigned int pixel6PaletteIndex = ( ( ( getR( ui_x+5, ui_y ) + getG( ui_x+5, ui_y ) + getB( ui_x+5, ui_y ) ) / 3 ) >= threshold )?1:0;
      unsigned int pixel7PaletteIndex = ( ( ( getR( ui_x+6, ui_y ) + getG( ui_x+6, ui_y ) + getB( ui_x+6, ui_y ) ) / 3 ) >= threshold )?1:0;
      unsigned int pixel8PaletteIndex = ( ( ( getR( ui_x+7, ui_y ) + getG( ui_x+7, ui_y ) + getB( ui_x+7, ui_y ) ) / 3 ) >= threshold )?1:0;
      unsigned int byte = (pixel1PaletteIndex << 7) + (pixel2PaletteIndex << 6) + (pixel3PaletteIndex << 5) + (pixel4PaletteIndex << 4)
                + (pixel5PaletteIndex << 3) + (pixel6PaletteIndex << 2) + (pixel7PaletteIndex << 1) + pixel8PaletteIndex;
      if ( byte == 0 ) thresholdLoopAllWhite++;
      else if ( byte == 0xFF ) thresholdLoopAllBlack++;
    } // loop one columns of one line
  } // iterate loop for all lines
  if ( ( thresholdLoopAllWhite + thresholdLoopAllBlack ) < ( thresholdOptimalAllWhite + thresholdOptimalAllBlack ) ) {
    // new optimum found
    thresholdOptimalAllWhite = thresholdLoopAllWhite;
    thresholdOptimalAllBlack = thresholdLoopAllBlack;
    optimalThreshold = threshold;
  }
  if ( threshold == 128 ) {
    // currently 128 - default - analysed
    threshold128AllWhite = thresholdLoopAllWhite;
    threshold128AllBlack = thresholdLoopAllBlack;
  }
  }
  // now decide about optimal threshold -> use 128 if optimum is not BETTER
  if ( ( thresholdOptimalAllWhite + thresholdOptimalAllBlack ) < ( threshold128AllWhite + threshold128AllBlack ) ) {
  // optimal threshold is really better
    std::cerr << "Found Optimal Threshold: " << optimalThreshold << " which has only " << thresholdOptimalAllWhite << " all white and "
      << thresholdOptimalAllBlack << " all black entries" << std::endl;
  i_currentThreshold = optimalThreshold;
  }
  else {
  // no better threshold than standard 128 found
  std::cerr << "Default 128 Threshold is optimal as it has only " << threshold128AllWhite << " all white and "
      << threshold128AllBlack << " all black entries\n"
      << "Optimal Threshold has " << thresholdOptimalAllWhite << " allWhite and " << thresholdOptimalAllBlack
      << " all black"
      << std::endl;
  i_currentThreshold = 128;
  }
}

unsigned int Vt2IsoImageBase_c::componenttoindex6 (unsigned int component)
{
  if (component < (0x33 - 0x33/2)) return 0;
  if (component < (0x66 - 0x33/2)) return 1;
  if (component < (0x99 - 0x33/2)) return 2;
  if (component < (0xCC - 0x33/2)) return 3;
  if (component < (0xFF - 0x33/2)) return 4;
  return 0x5;
}


unsigned int Vt2IsoImageBase_c::componentto09CF (unsigned int comp)
{
  if (comp < (0x99/2)) return 0x00;
  if (comp < (0xCC-0x99)/2+0x99) return 0x99;
  if (comp < (0xFF-0xCC)/2+0xCC) return 0xCC;
  return 0xFF;
}

unsigned int Vt2IsoImageBase_c::componentto09F (unsigned int comp)
{
  if (comp < (0x99/2)) return 0x00;
  if (comp < (0xFF-0x99)/2+0x99) return 0xCC;
  return 0xFF;
}
