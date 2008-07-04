/***************************************************************************
                          convert.h  - central conversion functions
                             -------------------
    begin                : Thu Aug 08 2002
    copyright            : (C) 2006 Dipl.-Inform. Manfred H.Guenzl
    email                : Manfred.Guenzl@osb-ag:de
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
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

#ifndef __CONVERT_H__
#define __CONVERT_H__

#include <IsoAgLib/typedef.h>


/** Common convert helpers (e.g. type conversion with type check).
  */
namespace convert_n
{
// Casting signed/unsigned by pointer not by value:

  //! Convert any integer to unsigned by cast (not by value) even if allready.
  inline uint8_t  castUI( int8_t  value ) { return *((uint8_t* )(&value)); }
  inline uint16_t castUI( int16_t value ) { return *((uint16_t*)(&value)); }
  inline uint32_t castUI( int32_t value ) { return *((uint32_t*)(&value)); }
  inline uint8_t  castUI(uint8_t  value ) { return value; }
  inline uint16_t castUI(uint16_t value ) { return value; }
  inline uint32_t castUI(uint32_t value ) { return value; }
  //! Convert any integer to signed by cast (not by value) even if allready.
  inline  int8_t  castI( uint8_t  value ) { return *(( int8_t* )(&value)); }
  inline  int16_t castI( uint16_t value ) { return *(( int16_t*)(&value)); }
  inline  int32_t castI( uint32_t value ) { return *(( int32_t*)(&value)); }
  inline  int8_t  castI(  int8_t  value ) { return value; }
  inline  int16_t castI(  int16_t value ) { return value; }
  inline  int32_t castI(  int32_t value ) { return value; }

  //! Template to get n-th significant byte of integer (no matter if signed or not).
  template<class T> inline uint8_t getByte(   T value, int n ) {
    return (castUI(value) >> (8*n)) & 0xFFU; }
  //! Template to get n-th significant nibble of integer (no matter if signed or not).
  template<class T> inline uint8_t getNibble( T value, int n ) {
    return (castUI(value) >> (4*n)) & 0xFU; }
};

#endif // __CONVERT_H__
