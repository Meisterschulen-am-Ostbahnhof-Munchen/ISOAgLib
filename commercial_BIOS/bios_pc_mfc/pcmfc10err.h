/*--------------------------------------------------------------------------.DA*
  Name:         pcmfc10err.h
  Description:  Defines for errorhandler
  Version: 15.02.99 /BA /Se /Fi                           Copyrights (c) STW
  --------------------------------------------------------------------------.DE*/
#ifndef __PCMFC10ERR_H__
   #define __PCMFC10ERR_H__

   /*--- PROTOTYPES ---*/
   extern void init_errorhandler(byte bSwitch); /* 0:= errorhandler inactiv; 1:= activ*/

   /* --- GLOBAL DEFINES CONSTANTS --- */
   /* Module */
   #define M_C2C            ((byte)0 * (byte)0x40)
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
   #define A_RTCRAM         ((byte)14* (byte)0x01)
   #define A_LOG            ((byte)15* (byte)0x01)

   /* Detail */
   #define D_CAN0           ((byte)1 * (byte)0x40)
   #define D_CAN1           ((byte)2 * (byte)0x40)
   #define D_RS232_1        ((byte)1 * (byte)0x01)
   #define D_RS232_2        ((byte)2 * (byte)0x01)
   #define D_RS232_3        ((byte)3 * (byte)0x01)

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

