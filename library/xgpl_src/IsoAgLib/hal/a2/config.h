/*
  config.h:  system dependent configs, defines and includes

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _HAL_A2_CONFIG_H_
#define _HAL_A2_CONFIG_H_


#define SYSTEM_A2


// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
#define HAL_CAN_MAX_BUS_NR 1

/** decide if ESX has a babyboard, which should also be init at startup */
//#define _INIT_BABYBOARD_

/** define uint16_t order of float: WORD_LO_HI, WORD_HI_LO */
#define FLOAT_WORD_ORDER WORD_HI_LO

/** define size of int */
#define SIZEOF_INT 2

/** define suitable keyword for huge memory type */
#define HUGE_MEM huge
/** the preprocessor can only distinguish between undefined or defined
  * constant for conditional compilation
  * -> use USE_HUGE_MEM
  */
#define USE_HUGE_MEM

#define MAX_EEPROM_SEGMENT_SIZE 40

/** define max number of write try-cycle to EEPROM
    ( one cycle: write value, re-read, compare )
  */
#define MAX_EEPROM_WRITE_TRY_CYCLE_CNT 5

// basic period of task manager (scheduler)= const * 500 us = 1 ms
#define T_TASK_BASIC 2
/* Initialisierung Watchdog 0 */
/* 128 ms                     */
#define WD_MAX_TIME   254
// 255
/* 0 ms                       */
#define WD_MIN_TIME    50
/* 16.7 V                     */
#define UD_MAX        233
/* 7.2 V                      */
#define UD_MIN        100
/* Relais wird bei allen Fehlern abgeschaltet */
#define CONFIG_RELAIS 0x1F
//#define CONFIG_RELAIS 0x0F
/* (b 0001 0000) Reset bei WD Verzug      */
#define  CONFIG_RESET 0x10
//#define  CONFIG_RESET 0x00

/** limit of digit value from getAdc as feedback input from a digital output
  * which decides between interpretation as High or Low
  * (needed for interpretation of status BITS)
  */
#define PWM_ADC_HIGH_THRESHOLD 500

#endif
