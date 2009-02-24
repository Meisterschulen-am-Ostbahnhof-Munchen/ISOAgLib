/***************************************************************************
                          target_extension_rs232_hal_simulator.cpp
                             - source for HAL simulator for RS-232
                          This file was based on target_extension_rs232_w32_sys.cpp
                             -------------------
    begin                : Mon Feb 16 2009
    author               : 2009 - Michael D. Schmidt
    email                : Mike.Schmidt@AgcoCorp.com
    type                 : Header
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                 *
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

#include <IsoAgLib/hal/pc/errcodes.h>

#include <stdio.h>      /* for fgets()  */
#include <stdlib.h>     /* for atexit() */
#include <conio.h>      /* for KeyXXX() */
#include <windows.h>    /* for SioXXX() */
#include <deque>

#include <IsoAgLib/hal/pc/HALSimulator/HALSimulator_c.h>

namespace __HAL {
HANDLE hCom[RS232_INSTANCE_CNT];
DCB oldConfig[RS232_INSTANCE_CNT];
STL_NAMESPACE::deque<int8_t> deq_readBuff[RS232_INSTANCE_CNT];

int8_t c_read;

static int arr_usedPort[RS232_INSTANCE_CNT] = {
#if RS232_INSTANCE_CNT > 0
false
#endif
#if RS232_INSTANCE_CNT > 1
, false
#endif
#if RS232_INSTANCE_CNT > 2
, false
#endif
#if RS232_INSTANCE_CNT > 3
, false
#endif
#if RS232_INSTANCE_CNT > 4
, false
#endif
};


/** close the RS232 interface. */
int16_t close_rs232(uint8_t comport)
{
	if ( comport >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
	
	return HAL_NO_ERR;
}

void close_rs232()
{
  for ( int comport = 0; comport < RS232_INSTANCE_CNT; comport++)
  {
    if ( arr_usedPort[comport] )
    {
      arr_usedPort[comport] = false;
    }
  }
}

/**
  init the RS232 interface
  @param wBaudrate wnated Baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}
        as configured in <IsoAgLib/isoaglib_config.h>
  @param bMode one of (DATA_7_BITS_EVENPARITY = 1, DATA_8_BITS_EVENPARITY = 2,
  DATA_7_BITS_ODDPARITY = 3, DATA_8_BITS_ODDPARITY = 4, DATA_8_BITS_NOPARITY = 5)
  @param bStoppbits amount of stop bits (1,2)
  @param bitSoftwarehandshake true -> use xon/xoff software handshake
  @return HAL_NO_ERR -> o.k. else one of settings incorrect
 */
int16_t init_rs232(uint16_t baudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake, uint8_t comport)
{
	if ( comport >= RS232_INSTANCE_CNT )
		return HAL_RANGE_ERR;

	HALSimulator_c::GetHALSimulator_c().init_rs232( baudrate, bMode, bStoppbits, bitSoftwarehandshake, comport );

	return HAL_NO_ERR;
}

/**
  set the RS232 Baudrate
  @param wBaudrate wanted baudrate
  @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
 */
int16_t setRs232Baudrate(uint16_t wBaudrate, uint8_t comport)
{
	if ( comport >= RS232_INSTANCE_CNT )
		return HAL_RANGE_ERR;

	return HAL_NO_ERR;
}

BOOL WriteABuffer( uint8_t aui8_channel, const uint8_t * lpBuf, uint16_t dwToWrite)
{
	HALSimulator_c::GetHALSimulator_c().write_rs232( aui8_channel, lpBuf, dwToWrite );
	
	return TRUE;
}

/**
  send single uint8_t on RS232
  @param bByte data uint8_t to send
  @return HAL_NO_ERR -> o.k. else send buffer overflow
 */
int16_t put_rs232Char(uint8_t bByte, uint8_t aui8_channel)
{
	if ( aui8_channel >= RS232_INSTANCE_CNT )
		return HAL_RANGE_ERR;

	return WriteABuffer( aui8_channel, &bByte, 1) ? HAL_NO_ERR : HAL_NOACT_ERR;
}
/**
  send string of n uint8_t on RS232
  @param bpWrite pointer to source data string
  @param wNumber number of data uint8_t to send
  @return HAL_NO_ERR -> o.k. else send buffer overflow
 */
int16_t put_rs232NChar(const uint8_t *bpWrite,uint16_t wNumber, uint8_t aui8_channel)
{
	if ( aui8_channel >= RS232_INSTANCE_CNT )
		return HAL_RANGE_ERR;

	return WriteABuffer( aui8_channel, bpWrite, wNumber) ? HAL_NO_ERR : HAL_NOACT_ERR;
}
/**
  send '\0' terminated string on RS232
  @param pbString pointer to '\0' terminated (!) source data string
  @return HAL_NO_ERR -> o.k. else send buffer overflow
 */
int16_t put_rs232String(const uint8_t *pbString, uint8_t aui8_channel)
{
	if ( aui8_channel >= RS232_INSTANCE_CNT )
		return HAL_RANGE_ERR;

	return WriteABuffer( aui8_channel, pbString, strlen((const char*)pbString)) ? HAL_NO_ERR : HAL_NOACT_ERR;
}

/**
  get the amount of data [uint8_t] in receive buffer
  @return receive buffer data byte
 */
int16_t getRs232RxBufCount(uint8_t comport)
{
	return HALSimulator_c::GetHALSimulator_c().getRs232RxBufCount( comport );
}

/**
  read bLastChar terminated string from receive buffer
  @param pbRead pointer to target data
  @param bLastChar terminating char
  @return HAL_NO_ERR -> o.k. else buffer underflow
 */
int16_t getRs232String(uint8_t *pbRead, uint8_t ui8_terminateChar, uint8_t comport)
{
	int16_t ret;

	do
	{
		ret = getRs232Char( pbRead, comport );
	} while( ret == HAL_NO_ERR && (*(pbRead++) != ui8_terminateChar ) );

	return ret;
}


/**
  read single int8_t from receive buffer
  @param pbRead pointer to target data
  @return HAL_NO_ERR -> o.k. else buffer underflow
 */
int16_t getRs232Char(uint8_t *pbRead, uint8_t aui8_channel)
{
	return HALSimulator_c::GetHALSimulator_c().getRs232Char( aui8_channel, pbRead );
}


/**
  get the amount of data [uint8_t] in send buffer
  @return send buffer data byte
 */
int16_t getRs232TxBufCount(uint8_t aui8_channel)
{
  if ( aui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  /** @todo ON REQUEST: decide if RS232 TX buffer from OS should be asked */
  return 0;
}
/**
  configure a receive buffer and set optional irq function pointer for receive
  @param wBuffersize wanted buffer size
  @param pFunction pointer to irq function or NULL if not wanted
 */
int16_t configRs232RxObj(uint16_t wBuffersize,void (*pFunction)(uint8_t *bByte), uint8_t aui8_channel)
{
  if ( aui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  /** @todo ON REQUEST: should this be implemented? */
  return HAL_NO_ERR;
}
/**
  configure a send buffer and set optional irq function pointer for send
  @param wBuffersize wanted buffer size
  @param funktionAfterTransmit pointer to irq function or NULL if not wanted
  @param funktionBeforTransmit pointer to irq function or NULL if not wanted
 */
int16_t configRs232TxObj(uint16_t wBuffersize,void (*funktionAfterTransmit)(uint8_t *bByte),
                         void (*funktionBeforTransmit)(uint8_t *bByte), uint8_t aui8_channel)
{
  if ( aui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  /** @todo ON REQUEST: should this be implemented? */
  return HAL_NO_ERR;
}
/**
  get errr code of BIOS
  @return 0=parity, 1=stopbit framing error, 2=overflow
 */
int16_t getRs232Error(uint8_t *Errorcode, uint8_t aui8_channel)
{
  if ( aui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  /** @todo ON REQUEST: should this be implemented? */
  return HAL_NO_ERR;
}


/**
  clear receive buffer
 */
void clearRs232RxBuffer(uint8_t aui8_channel)
{
  if ( aui8_channel >= RS232_INSTANCE_CNT ) return;
  /** @todo ON REQUEST: should this be implemented? */
};

/**
  clear send buffer
 */
void clearRs232TxBuffer(uint8_t aui8_channel)
{
  if ( aui8_channel >= RS232_INSTANCE_CNT ) return;
  /** @todo ON REQUEST: should this be implemented? */
}

} // end of namespace __HAL
