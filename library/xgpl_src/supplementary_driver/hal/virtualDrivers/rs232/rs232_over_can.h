/***************************************************************************
                          rs232.h - definition of HAL namesapce layer for
                          			Hardware Abstraction Layer
                             -------------------
    begin                : Fri Aug 26 2005
    copyright            : (C) 2000 - 2009 Dipl.-Inform. Achim Spangler
						 : This file was based on the corresponding file in
						 : the C2C HAL and modified for the RS232 over CAN.
						 : These changes (C) 2004 - 2005 Michael D. Schmidt
    email                : a.spangler@osb-ag:de
						 : mike.schmidt@agcocorp:com
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                  *
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

/* ************************************************************ */
/** \file
 * 
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the PC version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */

#ifndef _HAL_RS232_OVER_CAN_H_
#define _HAL_RS232_OVER_CAN_H_

// Multiple RS232 ports, so must use the channel parameter in the function calls!
#if !defined( USE_RS232_CHANNEL )
#	define USE_RS232_CHANNEL
#endif

#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/typedef.h>

#ifndef RS232_1
#  define RS232_1		0
#endif
#ifndef RS232_2
#  define RS232_2		1
#  endif
#ifndef RS232_3
#  define RS232_3		2
#endif
#ifndef RS232_4
#  define RS232_4		3
#endif

namespace HAL
{
  /* ****************************** */
  /** \name RS232 I/O BIOS functions */
/*@{*/

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

extern uint8_t RS232_over_can_busnum;
extern uint8_t RS232_over_can_initialized;

  int16_t init_rs232(uint16_t wBaudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake, uint8_t aui8_channel);
	/** close the RS232 interface. */
  inline int16_t close_rs232(uint8_t /*aui8_channel*/)
  	{RS232_over_can_initialized = false; return HAL_NO_ERR;}
  /**
    set the RS232 Baudrate
    @param wBaudrate wanted baudrate
    @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
  */
  inline int16_t setRs232Baudrate(uint16_t /*wBaudrate*/, uint8_t /*aui8_channel*/)
  { return RS232_over_can_initialized ? HAL_NO_ERR : HAL_RANGE_ERR; };
  /**
    get the amount of data [uint8_t] in receive buffer
    @return receive buffer data byte
  */
  inline int16_t getRs232RxBufCount(uint8_t /*aui8_channel*/)
    {return 0;};
  /**
    get the amount of data [uint8_t] in send buffer
    @return send buffer data byte
  */
  inline int16_t getRs232TxBufCount(uint8_t /*aui8_channel*/)
    {return 0;};
  /**
    configure a receive buffer and set optional irq function pointer for receive
    @param wBuffersize wanted buffer size
    @param pFunction pointer to irq function or NULL if not wanted
  */
  inline int16_t configRs232RxObj(uint16_t /*wBuffersize*/, void (* /*pFunction*/)(uint8_t *bByte),
                                  uint8_t /*aui8_channel*/)
    {return RS232_over_can_initialized ? HAL_NO_ERR : HAL_RANGE_ERR;};
  /**
    configure a send buffer and set optional irq function pointer for send
    <!--@param wBuffersize wanted buffer size
    @param funktionAfterTransmit pointer to irq function or NULL if not wanted
    @param funktionBeforTransmit pointer to irq function or NULL if not wanted-->
  */
  inline int16_t configRs232TxObj(uint16_t /*wBuffersize*/,void (* /*funktionAfterTransmit*/)(uint8_t * /*bByte*/),
                                  void (* /*funktionBeforTransmit*/)(uint8_t * /*bByte*/), uint8_t /*aui8_channel*/)
    {return RS232_over_can_initialized ? HAL_NO_ERR : HAL_RANGE_ERR;};
  /**
    get errr code of BIOS
    @return 0=parity, 1=stopbit framing error, 2=overflow
  */
  inline int16_t getRs232Error(uint8_t * /*Errorcode*/, uint8_t /*aui8_channel*/)
    {return RS232_over_can_initialized ? HAL_NO_ERR : HAL_RANGE_ERR;};

  /**
    read single int8_t from receive buffer
    @param pbRead pointer to target data
    @return HAL_NO_ERR -> o.k. else buffer underflow
  */
  inline int16_t getRs232Char(uint8_t * /*pbRead*/, uint8_t /*aui8_channel*/)
    {return RS232_over_can_initialized ? HAL_NO_ERR : HAL_RANGE_ERR;};
  /**
    read bLastChar terminated string from receive buffer
    @param pbRead pointer to target data
    @param bLastChar terminating char
    @return HAL_NO_ERR -> o.k. else buffer underflow
  */
  inline int16_t getRs232String(uint8_t * /*pbRead*/,uint8_t /*bLastChar*/, uint8_t /*aui8_channel*/)
    {return RS232_over_can_initialized ? HAL_NO_ERR : HAL_RANGE_ERR;};

  /**
    send single uint8_t on RS232
    @param bByte data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  int16_t put_rs232Char(uint8_t bByte, uint8_t aui8_channel);
  /**
    send string of n uint8_t on RS232
    @param bpWrite pointer to source data string
    @param wNumber number of data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  int16_t put_rs232NChar(const uint8_t *bpWrite,uint16_t wNumber, uint8_t aui8_channel);
  /**
    send '\0' terminated string on RS232
    @param pbString pointer to '\0' terminated (!) source data string
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  int16_t put_rs232String(const uint8_t *pbString, uint8_t aui8_channel);
  /**
    clear receive buffer
  */
  inline void clearRs232RxBuffer(uint8_t /*aui8_channel*/)
    {};
  /**
    clear send buffer
  */
  inline void clearRs232TxBuffer(uint8_t /*aui8_channel*/)
    {};
  /*@}*/
}
#endif
