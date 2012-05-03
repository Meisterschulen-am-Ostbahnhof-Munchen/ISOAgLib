/*
  config.h: central configuration definitions

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef __UTIL_CONFIG_HEADER_H__
#define __UTIL_CONFIG_HEADER_H__

#include <IsoAgLib/isoaglib_config.h>


#define STANDARD_CAN_IDENT uint16_t
#define EXTENDED_CAN_IDENT uint32_t

/* ********************************* */
/* *** declaration of some types *** */
/* ********************************* */

/**
  @TODO TaskControllerClient cleanup!
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
