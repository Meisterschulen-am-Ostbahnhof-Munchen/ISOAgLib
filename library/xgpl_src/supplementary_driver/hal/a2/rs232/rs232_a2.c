/*
  rs232_a2.c

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#ifdef DEBUG
//extern "C"
//{
 #include "cap_interface.h"
// }



   typedef unsigned int      word;
   typedef unsigned char     byte;
   typedef unsigned long     dword;
   typedef unsigned char     boolean;



   /* --- GLOBAL TYPDEFINITIONS --- */
   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */
//extern    int  init_rs232(word wBaudrate,byte bMode,byte bStoppbits,boolean bitSoftwarehandshake){return 0;}
extern   int  init_rs232(word wBaudrate,byte bMode,byte bStoppbits,boolean bitSoftwarehandshake)
{
 #if defined( APP_DEBUG ) || defined( DEBUG )

	Uart_Config_s 			RS232_config;

  //	RS232_config.Uart_Baudrate 		= 4800;
	RS232_config.Uart_Baudrate 		=  19200;
	RS232_config.Uart_ModeSelect	= UART_8BIT_DATA_ASYN;
	RS232_config.Uart_StopBits		= UART_ONE_STOP_BIT;
	RS232_config.Uart_ParitySelect	= UART_PARITY_EVEN_SEL;
	RS232_config.Uart_ParityCheck	= UART_PARITY_IGNORE;
	RS232_config.Uart_FrameCheck	= UART_FRAME_IGNORE;
	RS232_config.Uart_OverrunCheck	= UART_OVERRUN_IGNORE;

	uart_init(&RS232_config);
 #endif
return 0;
}

extern    int  get_rs232_rx_buf_count(void){return 0;}
extern   int  get_rs232_tx_buf_count(void){return 0;}		
    int  set_rs232_baudrate(word wBaudrate){return 0;}
    int  get_rs232_char(byte *pbRead){return 0;}
    int  put_rs232_char(byte bByte){return 0;}
    int  put_rs232_n_char(byte *bpWrite,word wNumber)
    {
	   
#if defined( APP_DEBUG ) || defined( DEBUG )

struct uart_GPS_s 	GPS_TX_sentence;
int i;
int cnt;

cnt= wNumber<100? wNumber:100; //A2 can only allow send 100 chars per time 
GPS_TX_sentence.ctMsg=cnt;

for (i=0;i<GPS_TX_sentence.ctMsg;i++)
	GPS_TX_sentence.MsgBuf[i] = *(bpWrite+i);

uart_tx_GPS_Sentence(&GPS_TX_sentence);
return 0;

#else
	  return 0;

#endif
    }

    int  put_rs232_string(byte *pbString){return 0;}
//    int  get_rs232_string(byte *pbRead,byte bLast_char){return 0;}

int  get_rs232_string(byte *pbRead,byte bLast_char)
{
return 0;
}

   int  config_rs232_rx_obj(word wBuffersize,void (*pFunction)(byte huge *bByte))
   {
   return 0;
   }
extern    int  config_rs232_tx_obj(word wBuffersize,void (*funktion_after_transmit)(byte huge *bByte),
                                  void (*funktion_befor_transmit)(byte huge *bByte))
                                 {
                                  return 0;
                                  }
                                  
//   #endif


