/*
  typedef.h: system dependent basic typedef's

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _TYPEDEF_PC_H
#define _TYPEDEF_PC_H

/* --- GLOBAL DEFINES CONSTANTS --- */
#define USER_WATCHDOG           0      /* Define for User-Watchdog */
#define DATA_CHANGED            1      /* Returnvalue from config_wd */

#ifndef FALSE
  #define FALSE             0
#endif
#ifndef TRUE
  #define TRUE              1
#endif
#define HIGH              1
#define LOW               0

#define ON                1
#define OFF               0

//#if !defined(SYSTEM_PC_VC) && !defined(WINCE)
#if !defined(WINCE)
  #include <stdint.h>
#else
  typedef unsigned char uint8_t;
  typedef signed char int8_t;
  typedef unsigned short uint16_t;
  typedef short int16_t;
  typedef unsigned int uint32_t;
  typedef int int32_t;
  typedef unsigned long long int uint64_t;
  typedef long int long int64_t;
#endif

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
