/*
  config.h: system dependent configs, defines and includes

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _HAL_DJ1_CONFIG_H_
#define _HAL_DJ1_CONFIG_H_

// include target independent configs
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/compiler_adaptation.h>

// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
#define HAL_CAN_MAX_BUS_NR 2

// If not allready defined in compiler_adaptation.h ...
#ifndef STL_NAMESPACE
  #define STL_NAMESPACE std
#endif

/** define uint16_t order of float: WORD_LO_HI, WORD_HI_LO */
#define FLOAT_WORD_ORDER WORD_HI_LO
/* we know that this cpu is little endian */
#define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN

/** define size of int */
#define SIZEOF_INT 2

/** define suitable keyword for huge memory type */
#define HUGE_MEM huge
/** the preprocessor can only distinguish between undefined or defined
  * constant for conditional compilation
  * -> use USE_HUGE_MEM
  */
#define USE_HUGE_MEM

/** define suitable  keyword for near memory type */
#define NEAR_MEM      _near
#define USE_NEAR_MEM  _near
#define MAX_EEPROM_SEGMENT_SIZE 64

/** define max number of write try-cycle to EEPROM
    ( one cycle: write value, re-read, compare )
  */
#define MAX_EEPROM_WRITE_TRY_CYCLE_CNT 5

// basic period of task manager (scheduler)= const * 500 us = 1 ms
#if !defined( T_TASK_BASIC )
	#define T_TASK_BASIC 2
#endif
/* Initialisierung Watchdog 0 */
/* Initialize Watchdog 0 */
/* 128 ms                     */
#define WD_MAX_TIME   WD_TIME_100_MS

/** limit of digit value from getAdc as feedback input from a digital output
  * which decides between interpretation as High or Low
  * (needed for interpretation of status BITS)
  */
#define PWM_ADC_HIGH_THRESHOLD 500

#endif
