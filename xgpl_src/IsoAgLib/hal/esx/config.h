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
#ifndef _HAL_ESX_CONFIG_H_
#define _HAL_ESX_CONFIG_H_

// include target independent configs
#include <Application_Config/isoaglib_config.h>
#include <IsoAgLib/util/compiler_adaptation.h>

// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
#define HAL_CAN_MAX_BUS_NR 1

// If not allready defined in compiler_adaptation.h ...
#ifndef STL_NAMESPACE
  #define STL_NAMESPACE std
#endif

/** decide if ESX has a babyboard, which should also be init at startup */
//#define _INIT_BABYBOARD_

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
#define WD_MIN_TIME    0
/* 16.7 V                     */
#define UD_MAX        255
/* 7.2 V                      */
#define UD_MIN        85
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
