/***************************************************************************
                          target_extensions_rs232_simulating.cpp - source for PC specific
                                                extensions for the HAL
                                                for RS232
                             -------------------
    begin                : Sat Jan 01 2003
    copyright            : (C) 2003 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
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
#include "rs232_target_extensions.h"
#include <IsoAgLib/hal/pc/system/system.h>
#include <cstdio>
#include <cstring>

#ifdef USE_REAL_RS232
  #ifdef PC_OS_Linux
    #include "Linux/linux_real_rs232.h"
  #endif
  #ifdef PC_OS_Win32
    #include "Windows/win32_real_rs232.h"
  #endif
#endif

// #define WRITE_LOG_FILE

namespace __HAL {

/* **************************************** */
/* ****** RS232 I/O BIOS functions  ******* */
/* **************************************** */
typedef FILE* FilePointer_t;
static FilePointer_t rs232_output[RS232_INSTANCE_CNT];

#ifdef WRITE_LOG_FILE
	static FilePointer_t rs232_log[RS232_INSTANCE_CNT];
#endif
static char sendName[RS232_INSTANCE_CNT][200];

/**
  init the RS232 interface
  @param wBaudrate wnated Baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}
        as configured in <Application_Config/isoaglib_config.h>
  @param bMode one of (DATA_7_BITS_EVENPARITY = 1, DATA_8_BITS_EVENPARITY = 2,
          DATA_7_BITS_ODDPARITY = 3, DATA_8_BITS_ODDPARITY = 4, DATA_8_BITS_NOPARITY = 5)
  @param bStoppbits amount of stop bits (1,2)
  @param bitSoftwarehandshake true -> use xon/xoff software handshake
  @return HAL_NO_ERR -> o.k. else one of settings incorrect
*/
int16_t init_rs232(uint16_t wBaudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
	if (rs232_output[rui8_channel]) fclose( rs232_output[rui8_channel] );
  #ifdef WRITE_LOG_FILE
	if (rs232_log[rui8_channel]) fclose( rs232_log[rui8_channel] );
	#endif
  char name[50];
	#ifdef WIN32
	sprintf(sendName[rui8_channel], "rs232_send_%hd", rui8_channel);
  rs232_output[rui8_channel] = fopen(sendName[rui8_channel], "w"); // "a+"
	#else
	sprintf(sendName[rui8_channel], "../../../simulated_io/rs232_send_%hd", rui8_channel);
  rs232_output[rui8_channel] = fopen(sendName[rui8_channel], "w"); // "a+"
  // BEGIN: Added by M.Wodok 6.12.04
  if (rs232_output[rui8_channel] == NULL) {
    // try opening in current directory...
    sprintf(sendName[rui8_channel], "rs232_send_%hd", rui8_channel);
    rs232_output[rui8_channel] = fopen(sendName[rui8_channel], "w"); // "a+"

		printf("init_rs232 aufgerufen mit %d Baud, Modus %hd, %hd Stop-Bits, XON/XOFF %d, Channel %hd, Ausgabedatei %s\n",
					wBaudrate,bMode, bStoppbits, bitSoftwarehandshake, rui8_channel, sendName[rui8_channel]);
  }
	else
	{
		printf("init_rs232 aufgerufen mit %d Baud, Modus %hd, %hd Stop-Bits, XON/XOFF %d, Channel %hd, Ausgabedatei %s\n",
					wBaudrate,bMode, bStoppbits, bitSoftwarehandshake, rui8_channel, sendName[rui8_channel]);
	}
  // END: Added by M.Wodok 6.12.04
  #endif

  #ifdef WRITE_LOG_FILE
	#ifdef WIN32
  sprintf(name, "rs232_log_%hd", rui8_channel);
	#else
	sprintf(sendName[rui8_channel], "../../../simulated_io/rs232_log_%hd", rui8_channel);
	#endif
  rs232_log[rui8_channel] = fopen(name, "w"); // "a+"
  // BEGIN: Added by M.Wodok 6.12.04
  if (rs232_log[rui8_channel] == NULL) {
    // try opening in current directory...
    strcpy(name, "rs232_log");
    rs232_log[rui8_channel] = fopen(name, "w"); // "a+"
  }
  printf("Versuch Datei mit Name %s zum schreiben zu oeffnen\n", name);
  #endif

	#ifdef USE_REAL_RS232
  SioInit(1,wBaudrate);
	#endif
  return HAL_NO_ERR;
}
/**
  set the RS232 Baudrate
  @param wBaudrate wanted baudrate
  @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
*/
int16_t setRs232Baudrate(uint16_t wBaudrate, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  printf("RS232 Baudrate auf %d gestellt\n", wBaudrate);
  return HAL_NO_ERR;
}
/**
  get the amount of data [uint8_t] in receive puffer
  @return receive puffer data byte
*/
int16_t getRs232RxBufCount(uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
#ifdef USE_REAL_RS232
  return SioRecPuffCnt();
#else
  return 2;
#endif
}
/**
  get the amount of data [uint8_t] in send puffer
  @return send puffer data byte
*/
int16_t getRs232TxBufCount(uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
	fclose( rs232_output[rui8_channel] );
	rs232_output[rui8_channel] = fopen(sendName[rui8_channel], "a+"); // "a+"
  return 0;
}
/**
  configure a receive puffer and set optional irq function pointer for receive
  @param wBuffersize wanted puffer size
  @param pFunction pointer to irq function or NULL if not wanted
*/
int16_t configRs232RxObj(uint16_t wBuffersize,void (*pFunction)(uint8_t *bByte), uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  printf("configRs232RxObj aufgerufen mit empfang puffersize %d \n", wBuffersize);
//  return (pFunction!=NULL)?HAL_NO_ERR:HAL_CONFIG_ERR;
  return HAL_NO_ERR;
}
/**
  configure a send puffer and set optional irq function pointer for send
  @param wBuffersize wanted puffer size
  @param funktionAfterTransmit pointer to irq function or NULL if not wanted
  @param funktionBeforTransmit pointer to irq function or NULL if not wanted
*/
int16_t configRs232TxObj(uint16_t wBuffersize,void (*funktionAfterTransmit)(uint8_t *bByte),
                                void (*funktionBeforTransmit)(uint8_t *bByte), uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  printf("configRs232TxObj aufgerufen mit sende puffersize %d \n", wBuffersize);
//  return ((funktionAfterTransmit!=NULL)&&(funktionBeforTransmit!=NULL))?HAL_NO_ERR:HAL_CONFIG_ERR;
  return HAL_NO_ERR;
}
/**
  get errr code of BIOS
  @return 0=parity, 1=stopbit framing error, 2=overflow
*/
int16_t getRs232Error(uint8_t *Errorcode, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  *Errorcode = 0;
  return HAL_NO_ERR;
}

/**
  read single int8_t from receive puffer
  @param pbRead pointer to target data
  @return HAL_NO_ERR -> o.k. else puffer underflow
*/
int16_t getRs232Char(uint8_t *pbRead, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
#ifdef USE_REAL_RS232
  SioGetByte(pbRead);
#endif
  int32_t i32_time = getTime();
  *pbRead = ((uint8_t*)&i32_time)[3];
	#ifdef WRITE_LOG_FILE
  fprintf(rs232_log[rui8_channel], "%d read %c\n", getTime(), *pbRead);
	#endif
  return HAL_NO_ERR;
}
/**
  read bLastChar terminated string from receive puffer
  @param pbRead pointer to target data
  @param bLastChar terminating char
  @return HAL_NO_ERR -> o.k. else puffer underflow
*/
int16_t getRs232String(uint8_t *pbRead,uint8_t bLastChar, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
#ifdef USE_REAL_RS232
  SioGetString(pbRead, bLastChar);
#endif
  int32_t i32_time = getTime();
  memcpy(pbRead, ((uint8_t*)&i32_time), 4);
  pbRead[4] = '\0';
  return HAL_NO_ERR;
}

/**
  send single uint8_t on RS232
  @param bByte data uint8_t to send
  @return HAL_NO_ERR -> o.k. else send puffer overflow
*/
int16_t put_rs232Char(uint8_t bByte, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  // printf("RS232:\n %c\n", bByte);
  fprintf(rs232_output[rui8_channel], "%c", bByte);
	fflush( rs232_output[rui8_channel] );
#ifdef USE_REAL_RS232
  uint8_t b_data = bByte;
  SioPutBuffer(&b_data, 1);
#endif
	#ifdef WRITE_LOG_FILE
  fprintf(rs232_log[rui8_channel], "%d write %c\n", getTime(), bByte);
	#endif
  return HAL_NO_ERR;
}
/**
  send string of n uint8_t on RS232
  @param bpWrite pointer to source data string
  @param wNumber number of data uint8_t to send
  @return HAL_NO_ERR -> o.k. else send puffer overflow
*/
int16_t put_rs232NChar(const uint8_t *bpWrite,uint16_t wNumber, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  uint8_t ui8_ind = 0;
  // printf("RS232: ");
//  for (; ui8_ind < wNumber; ui8_ind++)
//  {
//    putchar(bpWrite[ui8_ind]);
//    putc(bpWrite[ui8_ind], rs232_output[rui8_channel]);
//  }
//  printf("\n");
#ifdef USE_REAL_RS232
  SioPutBuffer(bpWrite, wNumber);
#endif
	#ifdef WRITE_LOG_FILE
  fprintf(rs232_log[rui8_channel], "%d write ", getTime());
	#endif
  for (ui8_ind = 0; ui8_ind < wNumber; ui8_ind++)
  {
    fprintf(rs232_output[rui8_channel], "%c", bpWrite[ui8_ind]);
		fflush( rs232_output[rui8_channel] );
		#ifdef WRITE_LOG_FILE
    fprintf(rs232_log[rui8_channel], "%c", bpWrite[ui8_ind]);
		#endif
  }
	#ifdef WRITE_LOG_FILE
  fprintf(rs232_log[rui8_channel], "\n");
	#endif
  return HAL_NO_ERR;
}
/**
  send '\0' terminated string on RS232
  @param pbString pointer to '\0' terminated (!) source data string
  @return HAL_NO_ERR -> o.k. else send puffer overflow
*/
int16_t put_rs232String(const uint8_t *pbString, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  // printf("RS232:: %s", pbString);
  fprintf(rs232_output[rui8_channel], "%s", pbString);
	fflush( rs232_output[rui8_channel] );
#ifdef USE_REAL_RS232
  SioPutBuffer(pbString, strlen(pbString));
#endif
  return HAL_NO_ERR;
}

/**
  clear receive puffer
*/
void clearRs232RxBuffer(uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return;
  printf("empfangspuffer gecleared\n");
};

/**
  clear send puffer
*/
void clearRs232TxBuffer(uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return;
  printf("sendepuffer gecleared\n");
}

} // End of name space __HAL
