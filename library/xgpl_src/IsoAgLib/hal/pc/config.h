/*
  config.h: system dependent configs, defines and includes

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
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
#ifndef FLOAT_WORD_ORDER
  #if defined(SYSTEM_A5) || defined(SYSTEM_MCC)
    #define FLOAT_WORD_ORDER BYTE_HI_LO
  #elif defined(OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN)
    #define FLOAT_WORD_ORDER WORD_LO_HI
  #endif
#endif

#if (!defined( OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN ) && !defined( OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN ) )
#  ifdef SYSTEM_A1
   /* we know that this cpu is little endian */
#    define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
#  elif defined(SYSTEM_A5)
   /* we know that this cpu is little endian */
#    define OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN
#  elif defined(SYSTEM_PC_VC)
   /* we know that the Microsoft Visual C++ platform is little endian */
#    define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
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









