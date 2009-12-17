/*
  typedef.h:  system dependent basic typedef's

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
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


#ifdef HANDLE_FAST_DATATYPE_AS_STRICT
/**  SIZEOF_INT = 2 */
/* Signed.  */
typedef signed char     int_fast8_t;
typedef int             int_fast16_t;
typedef long            int_fast32_t;
//typedef long long int   int_fast64_t;

/* Unsigned.  */
typedef unsigned char           uint_fast8_t;
typedef unsigned int            uint_fast16_t;
typedef unsigned long           uint_fast32_t;
//typedef unsigned long long int  uint_fast64_t;

#endif // end HANDLE_FAST_DATATYPE_AS_STRICT

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
