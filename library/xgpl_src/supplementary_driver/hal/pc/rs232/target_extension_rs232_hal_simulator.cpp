/*
  target_extension_rs232_hal_simulator.cpp: source for HAL simulator
    for RS-232. This file was based on target_extension_rs232_w32_sys.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "rs232_target_extensions.h"

#include <IsoAgLib/hal/pc/errcodes.h>

#include <stdio.h>      /* for fgets()  */
#include <stdlib.h>     /* for atexit() */
#include <conio.h>      /* for KeyXXX() */
#include <windows.h>    /* for SioXXX() */
#include <deque>

#include <IsoAgLib/hal/pc/hal_simulator/hal_simulator_c.h>

namespace __HAL {
HANDLE hCom[RS232_CHANNEL_CNT];
DCB oldConfig[RS232_CHANNEL_CNT];
STL_NAMESPACE::deque<int8_t> deq_readBuff[RS232_CHANNEL_CNT];

int8_t c_read;

static int arr_usedPort[RS232_CHANNEL_CNT] = {
#if RS232_CHANNEL_CNT > 0
false
#endif
#if RS232_CHANNEL_CNT > 1
, false
#endif
#if RS232_CHANNEL_CNT > 2
, false
#endif
#if RS232_CHANNEL_CNT > 3
, false
#endif
#if RS232_CHANNEL_CNT > 4
, false
#endif
#if RS232_CHANNEL_CNT > 5
, false
#endif
#if RS232_CHANNEL_CNT > 6
, false
#endif
#if RS232_CHANNEL_CNT > 7
, false
#endif
#if RS232_CHANNEL_CNT > 8
, false
#endif
};


/** close the RS232 interface. */
int16_t close_rs232(uint8_t comport)
{
	if ( comport >= RS232_CHANNEL_CNT ) return HAL_RANGE_ERR;
	
	return HAL_NO_ERR;
}

void close_rs232()
{
  for ( int comport = 0; comport < RS232_CHANNEL_CNT; comport++)
  {
    if ( arr_usedPort[comport] )
    {
      arr_usedPort[comport] = false;
    }
  }
}

/**
  init the RS232 interface
  @param baudrate wnated Baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}
        as configured in <IsoAgLib/isoaglib_config.h>
  @param bMode one of (DATA_7_BITS_EVENPARITY = 1, DATA_8_BITS_EVENPARITY = 2,
  DATA_7_BITS_ODDPARITY = 3, DATA_8_BITS_ODDPARITY = 4, DATA_8_BITS_NOPARITY = 5)
  @param bStoppbits amount of stop bits (1,2)
  @param bitSoftwarehandshake true -> use xon/xoff software handshake
  @return HAL_NO_ERR -> o.k. else one of settings incorrect
 */
int16_t init_rs232(uint32_t baudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake, uint8_t comport)
{
	if ( comport >= RS232_CHANNEL_CNT )
		return HAL_RANGE_ERR;

	HALSimulator_c::GetHALSimulator_c().init_rs232( baudrate, bMode, bStoppbits, bitSoftwarehandshake, comport );

	return HAL_NO_ERR;
}

/**
  set the RS232 Baudrate
  @param baudrate wanted baudrate
  @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
 */
int16_t setRs232Baudrate(uint32_t baudrate, uint8_t comport)
{
	if ( comport >= RS232_CHANNEL_CNT )
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
	if ( aui8_channel >= RS232_CHANNEL_CNT )
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
	if ( aui8_channel >= RS232_CHANNEL_CNT )
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
	if ( aui8_channel >= RS232_CHANNEL_CNT )
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
  if ( aui8_channel >= RS232_CHANNEL_CNT ) return HAL_RANGE_ERR;
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
  if ( aui8_channel >= RS232_CHANNEL_CNT ) return HAL_RANGE_ERR;
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
  if ( aui8_channel >= RS232_CHANNEL_CNT ) return HAL_RANGE_ERR;
  /** @todo ON REQUEST: should this be implemented? */
  return HAL_NO_ERR;
}
/**
  get errr code of BIOS
  @return 0=parity, 1=stopbit framing error, 2=overflow
 */
int16_t getRs232Error(uint8_t *Errorcode, uint8_t aui8_channel)
{
  if ( aui8_channel >= RS232_CHANNEL_CNT ) return HAL_RANGE_ERR;
  /** @todo ON REQUEST: should this be implemented? */
  return HAL_NO_ERR;
}


/**
  clear receive buffer
 */
void clearRs232RxBuffer(uint8_t aui8_channel)
{
  if ( aui8_channel >= RS232_CHANNEL_CNT ) return;
  /** @todo ON REQUEST: should this be implemented? */
};

/**
  clear send buffer
 */
void clearRs232TxBuffer(uint8_t aui8_channel)
{
  if ( aui8_channel >= RS232_CHANNEL_CNT ) return;
  /** @todo ON REQUEST: should this be implemented? */
}

} // end of namespace __HAL