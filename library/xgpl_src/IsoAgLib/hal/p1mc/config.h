/*
  config.h: system dependent configs, defines and includes

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _HAL_P1MC_CONFIG_H_
#define _HAL_P1MC_CONFIG_H_


#define SYSTEM_P1MC


#define HAL_CAN_BITRATE_LIST {10, 20, 40, 50, 100, 125, 250, 500, 1000}
#define HAL_CAN_BITRATE_CNT 9
/** index for "LastMsgObj" where all messages are stored, which map through the special lastMsgMask */
#define HAL_CAN_LAST_MSG_OBJ_NR 14
#define HAL_CAN_MIN_SEND_OBJ 0
#define HAL_CAN_MAX_SEND_OBJ 13
// the send msg objects take the lowest possible msg ob numbers
// ==> take 2 as min msg obj nr
#define HAL_CAN_MIN_REC_OBJ 1
#define HAL_CAN_MAX_REC_OBJ 14

/// define list of allowed speed settings
#define HAL_RS232_BAUDRATE_LIST {75, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200}
#define HAL_RS232_BITRATE_CNT 10



// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
#define HAL_CAN_MAX_BUS_NR 2

/** define uint16_t order of float: WORD_LO_HI, WORD_HI_LO or BYTE_HI_LO*/
#define FLOAT_WORD_ORDER BYTE_HI_LO
/* we know that this cpu is little endian */
//#define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN 1
#define USE_LITTLE_ENDIAN_CPU 1

/** define size of int */
#define SIZEOF_INT 2

/** define suitable keyword for huge memory type */
#define HUGE_MEM
//#define HUGE_MEM huge
/** the preprocessor can only distinguish between undefined or defined
  * constant for conditional compilation
  * -> use USE_HUGE_MEM
  */
#define USE_HUGE_MEM

/** define suitable  keyword for near memory type */
#define NEAR_MEM      _near
#define USE_NEAR_MEM  _near

#define MAX_EEPROM_SEGMENT_SIZE 40

/** define max number of write try-cycle to EEPROM
    ( one cycle: write value, re-read, compare )
  */
#define MAX_EEPROM_WRITE_TRY_CYCLE_CNT 5

// basic period of task manager (scheduler)= const * 500 us = 1 ms
#define T_TASK_BASIC 2

// the P1MC processor modul has no static power line
// where check of D-Plus can be used
#define NO_CAN_EN_CHECK

#endif
