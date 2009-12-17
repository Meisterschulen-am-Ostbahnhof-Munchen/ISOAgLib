/*
  typedef.h:  system dependent basic typedef's

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TYPEDEF_A2_H
#define TYPEDEF_A2_H

#if defined(__TSW_CPP__)
 #include <cstddef>
 #include <cstring>
 typedef unsigned int size_t;
#endif

#ifdef SYSTEM_A2
#include <string.h> //Brian Wei, 8/31/06
extern "C"	char *  strchr   ( const char *, int );
extern "C"	char *  strncat  ( char *, const char *, size_t );
extern "C"	int     strncmp  ( const char *, const char *, size_t );
extern "C"	int     strcmp   ( const char *, const char * );
extern "C"  int    sprintf  ( char *, const char *, ... );
extern "C"  unsigned int  strlen   ( const char * );
_ext_C	_USMLIB	void *  memcpy   ( void *, const void *, size_t );
_ext_C	_USMLIB	void *  memmove  ( void *, const void *, size_t );
_ext_C	_USMLIB	void *  memchr   ( const void *, int, size_t );
_ext_C	_USMLIB	void *  memset   ( void *, int, size_t );
_ext_C	_USMLIB	int     memcmp   ( const void *, const void *, size_t );
#endif


typedef unsigned char uint8_t;
typedef unsigned char byte;
typedef char int8_t;
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

#endif //  end HANDLE_FAST_DATATYPE_AS_STRICT

//typedef unsigned long long int uint64_t;
//typedef long long int int64_t;

#endif

//BW, 4/18/06, copy from ESX bios header file "xos20esx.h"
/*--------------------------------------------------------------------------.DA*
  Name:         def00typ.h
  Description:  common defines and type definitions
  Version:      06.12.2001 /Se                            Copyrights (c) STW
  --------------------------------------------------------------------------.DE*/
#ifndef __DEF00TYP_H
#define __DEF00TYP_H

   /* --- GLOBAL DEFINES AND CONSTANTS --- */
   #define FALSE             0
   #define TRUE              1

   #define HIGH              1
   #define LOW               0

   #define ON                1
   #define OFF               0

   #define CODE              const  /*04.05.99*/

   // anpassen an Compiler V7.0r1
   #ifdef NULL
      #undef  NULL
      #define NULL           (0L)
   #endif

   /* --- GLOBAL TYPDEFINITIONS --- */
   typedef unsigned int      word;
   typedef unsigned char     byte;
   typedef unsigned long     dword;
   typedef unsigned char     boolean;

   typedef void (*pfFunction)(void); /* typdefinitions for indirect functioncall */

   typedef struct           /* Struct Time */
   {
      long  l1ms;
      word  w1us;
   } tTime;

   typedef union            /* Union byte, word Zugriff (2 Byte) */
   {
      byte  b[2];
      word  w;
   } tBw;

   typedef union            /* Union byte, word, dword Zugriff (4 Byte) */
   {
      byte  b[4];
      word  w[2];
      dword dw;
   } tDw;

   typedef struct           /* Struct Identification for Software */
   {
      byte abProgname[8];
      byte abVersion[5];
      byte bTyp;
      int  iOption;
   } tIdent;

   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */

/*--------------------------------------------------------------------------.DA*
  Name:           xx2_0sys.h
  Description:    Systemfunctions for ESX
  Version: 19.01.99 /BA /Se                               Copyrights (c) STW
  --------------------------------------------------------------------------.DE*/
   /* --- GLOBAL DEFINES CONSTANTS --- */
   #define USER_WATCHDOG           0      /* Define for User-Watchdog */
   #define DATA_CHANGED            1      /* Returnvalue from config_wd */

   /* --- GLOBAL TYPDEFINITIONS --- */
   typedef struct
   {
      byte bHardware;
      byte bCPUType;
      byte bCPU_freq;
      word wRAMSize;
      word wROMSize;
      byte bEESize;
      word wSFCType;
      byte abBabyboard1[8];
      byte abBabyboard2[8];
      word wOutputType;
      word wL0_Err;
      byte bEEWriteSegmentSize;
      word wPICTick;
      byte bReset;
   } tSystem;

   typedef struct
   {
      byte bWDmaxTime;
      byte bWDminTime;
      byte bUDmax;
      byte bUDmin;
      byte bConfigRelais;
      byte bConfigReset;
   } tWDConfig;


#endif


//BW, copy from ESX bios header file "xos20err.h"

/*--------------------------------------------------------------------------.DA*
  Name:         err00167.h
  Description:  Defines for errorhandler
  Version: 15.02.99 /BA /Se /Fi                           Copyrights (c) STW
  --------------------------------------------------------------------------.DE*/
#ifndef __ERR00167_H__
   #define __ERR00167_H__

   /*--- PROTOTYPES ---*/
   extern void init_errorhandler(byte bSwitch); /* 0:= errorhandler inactiv; 1:= activ*/

   /* --- GLOBAL DEFINES CONSTANTS --- */
   /* Module */
   #define M_ESX            ((byte)0 * (byte)0x40)
   #define M_BB1            ((byte)1 * (byte)0x40)
   #define M_BB2            ((byte)2 * (byte)0x40)
   #define M_LEVEL0         ((byte)3 * (byte)0x40)

   /* Source */
   #define S_SYS            ((byte)0 * (byte)0x10)
   #define S_USER           ((byte)1 * (byte)0x10)
   #define S_APPL           ((byte)2 * (byte)0x10)

   /* Area */
   #define A_ANALOG         ((byte)0 * (byte)0x01)
   #define A_OSYS           ((byte)1 * (byte)0x01)
   #define A_EEPROM         ((byte)2 * (byte)0x01)
   #define A_FLS            ((byte)3 * (byte)0x01)
   #define A_DIGIN          ((byte)4 * (byte)0x01)
   #define A_DIGOUT         ((byte)5 * (byte)0x01)
   #define A_SYS            ((byte)6 * (byte)0x01)
   #define A_TSYS           ((byte)7 * (byte)0x01)
   #define A_CAN            ((byte)8 * (byte)0x01)
   #define A_RS232          ((byte)9 * (byte)0x01)
   #define A_SCU            ((byte)10* (byte)0x01)
   #define A_BB             ((byte)11* (byte)0x01)
   #define A_PID            ((byte)12* (byte)0x01)
   #define A_FPRG           ((byte)13* (byte)0x01)

   /* Detail */
   #define D_CAN0           ((byte)1 * (byte)0x40)
   #define D_CAN1           ((byte)2 * (byte)0x40)

   /* Cause */
   #define C_NO_ERR         ((int)( 0 )* (int)0x0001)
   #define C_UNKNOWN_ERR    ((int)(-1 )* (int)0x0001)
   #define C_WARN           ((int)(-2 )* (int)0x0001)
   #define C_DEFAULT        ((int)(-3 )* (int)0x0001)
   #define C_BUSY           ((int)(-4 )* (int)0x0001)
   #define C_RANGE          ((int)(-5 )* (int)0x0001)
   #define C_OVERFLOW       ((int)(-6 )* (int)0x0001)
   #define C_RD_WR          ((int)(-7 )* (int)0x0001)
   #define C_NOACT          ((int)(-8 )* (int)0x0001)
   #define C_COM            ((int)(-9 )* (int)0x0001)
   #define C_CONFIG         ((int)(-10)* (int)0x0001)
   #define C_CHECKSUM       ((int)(-11)* (int)0x0001)

#endif
















