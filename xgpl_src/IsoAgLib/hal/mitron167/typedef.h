/***************************************************************************
                          typedef.h  -  system dependent basic typedef's
                             -------------------
    begin                : Thu Aug 08 2002
    copyright            : (C) 2002 - 2004 Dipl.-Inform. Achim Spangler
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
#ifndef _TYPEDEF_MITRON167_H
#define _TYPEDEF_MITRON167_H

/* --- GLOBAL DEFINES CONSTANTS --- */
#define USER_WATCHDOG           0      /* Define for User-Watchdog */
#define DATA_CHANGED            1      /* Returnvalue from config_wd */

#define FALSE             0
#define TRUE              1
#define HIGH              1
#define LOW               0

#define ON                1
#define OFF               0

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned int uint16_t;
typedef int int16_t;
typedef unsigned long uint32_t;
typedef long int32_t;

// NOTE: The mitron167 can NOT compile this because it does NOT understand what a long long is.
//       At least this was the case with Tasking version 7.5 r 6.
//typedef unsigned long long int uint64_t;
//typedef long long int int64_t;

typedef uint8_t boolean;

namespace __HAL {

/** Union uint8_t, uint16_t Zugriff (2 Byte) */
typedef union   {
  uint8_t  b[2];
  uint16_t  w;
} tBw;

/** Union uint8_t, uint16_t, uint32_t Zugriff (4 Byte) */
typedef union   {
  uint8_t  b[4];
  uint16_t  w[2];
  uint32_t dw;
} tDw;

typedef struct
{
  int32_t  l1ms;
  uint16_t  w1us;
} tTime;


typedef struct
{
  uint8_t bWDmaxTime;
  uint8_t bWDminTime;
  uint8_t bUDmax;
  uint8_t bUDmin;
  uint8_t bConfigRelais;
  uint8_t bConfigReset;
} tWDConfig;

typedef struct
{
  uint8_t bHardware;
  uint8_t bEESize;
  uint16_t wRAMSize;
  uint16_t wROMSize;
  uint8_t bReset;
  uint16_t wL0Err;
} tSystem;

} // End of namespace __HAL


#endif
