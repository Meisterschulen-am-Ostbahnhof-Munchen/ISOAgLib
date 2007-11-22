/***************************************************************************
                          config.h  - central configuration
                                                    definitions
                             -------------------
    begin                : Sun Jul 3 2001
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



#ifndef __MASTER_CONFIG_HEADER_H__
#define __MASTER_CONFIG_HEADER_H__

#include "../typedef.h"
#include "compiler_adaptation.h"


/* ****************************************** */
/* * include of standard header files */
/* ****************************************** */

/* include of system specific headers */
/* (not headers with BIOS function adaptors) */
#include <IsoAgLib/hal/config.h>

#define STANDARD_CAN_IDENT uint16_t
#define EXTENDED_CAN_IDENT uint32_t

/* ********************************* */
/* *** declaration of some types *** */
/* ********************************* */
/**
  union to access 4byte process data values in the same 4byte
  memory as int32_t, uint32_t, float as they are exclusively used
*/
union processDataVal
{
  int32_t i32_val;
  uint32_t ulVal;
#ifdef USE_FLOAT_DATA_TYPE
  float f_val;
#endif
};
/**
  enumeration to define type of process data value:
  int32_t, uint32_t, float
*/
enum proc_valType_t {i32_val = 0, ui32_val = 2, cmdVal = 3
#if defined(USE_FLOAT_DATA_TYPE)
  , float_val = 1
#endif
  };
/**
  enumeration to define special cmds in he 4byte process data value
*/
enum proc_specCmd_t {setpointReleaseCmd = 1, setpointErrCmd = 2, noVal_32s = 4, errVal_32s = 8};

/* ****************************************** */
/* * declaration of HW independent constants  */
/* ****************************************** */

// define if exception handling should be used
// (f.e. signalling bad dynamic array access)
// check dependent on EXCEPTION use configured for STL
#if defined(__EXCEPTIONS) || defined(__STL_USE_EXCEPTIONS)
  #define __USE_EXCEPTIONS
#endif

// define some declaration constants for exceptions
#ifdef __USE_EXCEPTIONS
  #define THROW_BAD_ALLOC throw bad_alloc();
  #define THROW_CONT_EL_NONEXIST
  #define THROW_PRECOND_VIOLATION
#else
  #define THROW_BAD_ALLOC
  #define THROW_CONT_EL_NONEXIST
  #define THROW_PRECOND_VIOLATION
#endif



// special Scheduler_c values
#define SETPOINT_RELEASE_COMMAND  NO_VAL_32S
#define SETPOINT_ERROR_COMMAND  ERROR_VAL_32S


#define NO_VAL_32S ((int32_t)0x80000000l)
#define NO_VAL_16S ((int16_t)0x8000)
#define NO_VAL_8S ((int8_t)0x80)
#define NO_VAL_32 ((uint32_t)0xFFFFFFFFUL)
#define NO_VAL_16 ((uint16_t)0xFFFFu)
#define NO_VAL_8 ((uint8_t)0xFFu)

#define MIN_32S ((int32_t)-2147483647)
#define MIN_16S ((int16_t)-32767)
#define MIN_8S ((int8_t)-127)

#define MAX_32S ((int32_t)2147483647)
#define MAX_16S ((int16_t)32767)
#define MAX_8S ((int8_t)127)

#define MAX_32 ((uint32_t)0xFFFFFFFFUL)
#define MAX_16 ((uint16_t)0xFFFF)
#define MAX_8 ((uint8_t)0xFF)

#define ERROR_VAL_32S ((int32_t)0x80000001l)
#define ERROR_VAL_16S ((int16_t)0x8001)
#define ERROR_VAL_8S ((int8_t)0x81)
#define ERROR_VAL_32 0xFFFFFFFEUL
#define ERROR_VAL_16 0xFFFEu
#define ERROR_VAL_8 0xFEu

#endif
