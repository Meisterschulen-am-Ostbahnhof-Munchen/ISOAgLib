/***************************************************************************
                          config.h  -  system dependent configs, defines and includes
                             -------------------
    begin                : Thu Jul 29 1999
    copyright            : (C) 1999 - 2004 Dipl.-Inform. Achim Spangler
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
#ifndef SYSTEM_DEFINES_PC_H
#define SYSTEM_DEFINES_PC_H

// include target independent configs
#include <IsoAgLib/isoaglib_config.h>

// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
#define HAL_CAN_MAX_BUS_NR 3

// If not allready defined in compiler_adaptation.h ...
#ifndef STL_NAMESPACE
  #define STL_NAMESPACE std
#endif


/** define uint16_t order of float: WORD_LO_HI, BYTE_HI_LO, WORD_HI_LO */
#if defined(SYSTEM_A5) || defined(SYSTEM_MCC)
  #define FLOAT_WORD_ORDER BYTE_HI_LO
#elif defined(OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN)
  #define FLOAT_WORD_ORDER WORD_LO_HI
#endif

#if (!defined( OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN ) && !defined( OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN ) )
#  ifdef SYSTEM_A1
   /* we know that this cpu is little endian */
#    define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
#  elif defined(SYSTEM_A5)
   /* we know that this cpu is little endian */
#    define OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN
#  endif
#endif

/** define size of int */
#define SIZEOF_INT 4

/** don't use CanEn check */
// #define NO_CAN_EN_CHECK

/** define suitable keyword for huge memory type */
#define HUGE_MEM
/** the preprocessor can only distinguish between undefined or defined
  * constant for conditional compilation
  * -> use USE_HUGE_MEM
  */
// #define USE_HUGE_MEM

/** define suitable  keyword for near memory type */
#define NEAR_MEM
#define USE_NEAR_MEM


#define MAX_EEPROM_SEGMENT_SIZE 40

/** define max number of write try-cycle to EEPROM
    ( one cycle: write value, re-read, compare )
  */
#define MAX_EEPROM_WRITE_TRY_CYCLE_CNT 5

/// choose operating system of PC (windows, linux)
#if defined( _WIN32 )
	#define PC_OS_Win32
#else
	#define PC_OS_Linux
#endif

/// choose if real RS232 should be used (otherwise simuation via file)
//#define USE_REAL_RS232

#ifdef PC_OS_Win32
/// choose if real CAN cardX should be used on Win32
//#define USE_CAN_CARD_X
#endif

// basic period of task manager (scheduler)= const * 500 us = 1 ms
#define T_TASK_BASIC 2
/* Initialisierung Watchdog 0 */
#define WD_MAX_TIME      0//200        /* 128 ms                    */
#define WD_MIN_TIME      0      /* 0 ms                      */
#define UD_MAX           233    /* 16.7 V                    */
#define UD_MIN           100    /* 7.2 V                    */
#define CONFIG_RELAIS    255    /* Relais wird bei allen Fehlern abgeschaltet */
#define  CONFIG_RESET    0x10   /* (b 0001 0000) Reset bei WD Verzug      */

/** @def SYSTEM_PC_VC_YN
  * set this define to YES if IsoAgLib is used for a PC with microsoft Visual C++ compiler
  * (some parts of C++ are not correct interpreted by Visual C++ )
  */
#ifndef SYSTEM_PC_VC_YN
  #define SYSTEM_PC_VC_YN NO
#endif
#if SYSTEM_PC_VC_YN == YES && !defined(SYSTEM_PC_VC)
  #define SYSTEM_PC_VC
#endif

#endif









