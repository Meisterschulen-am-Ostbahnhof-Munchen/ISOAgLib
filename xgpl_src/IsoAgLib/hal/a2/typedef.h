/***************************************************************************
                          typedef.h  -  system dependent basic typedef's
                             -------------------
    begin                : Thu Aug 08 2002
    copyright            : (C) 2002 - 2004 Dipl.-Inform. Achim Spangler
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
#ifndef TYPEDEF_A2_H
#define TYPEDEF_A2_H

#if defined(__TSW_CPP__)
 #include <cstddef>
 #include <cstring>
 typedef unsigned int size_t;
#endif

typedef unsigned char uint8_t;
typedef unsigned char byte;
typedef char int8_t;
typedef unsigned int uint16_t;
typedef int int16_t;
typedef unsigned long uint32_t;
typedef long int32_t;

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
















