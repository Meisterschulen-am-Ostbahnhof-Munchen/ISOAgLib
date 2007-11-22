/***************************************************************************
                          rs232.h - definition of HAL
                                    namesapce layer for C2C
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
						 : This file was based on the corresponding file in
						 : the ESX HAL and modified for the C2C HAL.
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

/* ************************************************************ */
/** \file supplementary_driver/hal/c2c/rs232/rs232.h
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

#ifndef _HAL_C2C_RS232_H_
#define _HAL_C2C_RS232_H_

// Multiple RS232 ports, so must use the channel parameter in the function calls!
#define USE_RS232_CHANNEL

#include <IsoAgLib/hal/c2c/config.h>
#include <IsoAgLib/hal/c2c/typedef.h>
#include <IsoAgLib/hal/c2c/errcodes.h>

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_c2c/c2c10osy.h>
  }
}
/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaptation to new platform
 */
#define CONSOLE 	RS232_1
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
  inline int16_t init_rs232(uint16_t wBaudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake, uint8_t aui8_channel)
    {return __HAL::init_rs232(aui8_channel,wBaudrate,bMode,bStoppbits,bitSoftwarehandshake) ;};
	/** close the RS232 interface. */
	inline int16_t close_rs232(uint8_t aui8_channel)
		{__HAL::config_rs232_rx_obj(aui8_channel,0,NULL); return __HAL::config_rs232_tx_obj(aui8_channel,0,NULL,NULL);};
  /**
    set the RS232 Baudrate
    @param wBaudrate wanted baudrate
    @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
  */
  inline int16_t setRs232Baudrate(uint16_t wBaudrate, uint8_t aui8_channel)
    {return __HAL::set_rs232_baudrate(aui8_channel,wBaudrate) ;};
  /**
    get the amount of data [uint8_t] in receive puffer
    @return receive puffer data byte
  */
  inline int16_t getRs232RxBufCount(uint8_t aui8_channel)
    {return __HAL::get_rs232_rx_buf_count(aui8_channel);};
  /**
    get the amount of data [uint8_t] in send puffer
    @return send puffer data byte
  */
  inline int16_t getRs232TxBufCount(uint8_t aui8_channel)
    {return __HAL::get_rs232_tx_buf_count(aui8_channel);};
  /**
    configure a receive puffer and set optional irq function pointer for receive
    @param wBuffersize wanted puffer size
    @param pFunction pointer to irq function or NULL if not wanted
  */
  #if 0
  inline int16_t configRs232RxObj(uint16_t wBuffersize,void (*pFunction)(byte _huge *bByte))
    {return __HAL::config_rs232_rx_obj(CONSOLE,wBuffersize,pFunction) ;};
  #else
  inline int16_t configRs232RxObj(uint16_t wBuffersize,void (*pFunction)(byte *bByte), uint8_t aui8_channel)
    {return __HAL::config_rs232_rx_obj(aui8_channel,wBuffersize,pFunction) ;};
  #endif
  /**
    configure a send puffer and set optional irq function pointer for send
    @param wBuffersize wanted puffer size
    @param funktionAfterTransmit pointer to irq function or NULL if not wanted
    @param funktionBeforTransmit pointer to irq function or NULL if not wanted
  */
  #if 0
  inline int16_t configRs232TxObj(uint16_t wBuffersize,void (*funktionAfterTransmit)(byte _huge *bByte),
                                  void (*funktionBeforTransmit)(byte _huge *bByte))
    {return __HAL::config_rs232_tx_obj(CONSOLE,wBuffersize,funktionAfterTransmit,funktionBeforTransmit);};
  #else
  inline int16_t configRs232TxObj(uint16_t wBuffersize,void (*funktionAfterTransmit)(byte *bByte),
                                  void (*funktionBeforTransmit)(byte *bByte), uint8_t aui8_channel)
    {return __HAL::config_rs232_tx_obj(aui8_channel,wBuffersize,funktionAfterTransmit,funktionBeforTransmit);};
  #endif
  /**
    get errr code of BIOS
    @return 0=parity, 1=stopbit framing error, 2=overflow
  */
  inline int16_t getRs232Error(uint8_t *Errorcode, uint8_t aui8_channel)
    {return __HAL::get_rs232_error(aui8_channel,Errorcode);};

  /**
    read single int8_t from receive puffer
    @param pbRead pointer to target data
    @return HAL_NO_ERR -> o.k. else puffer underflow
  */
  inline int16_t getRs232Char(uint8_t *pbRead, uint8_t aui8_channel)
    {return __HAL::get_rs232_char(aui8_channel,pbRead);};
  /**
    read bLastChar terminated string from receive puffer
    @param pbRead pointer to target data
    @param bLastChar terminating char
    @return HAL_NO_ERR -> o.k. else puffer underflow
  */
  inline int16_t getRs232String(uint8_t *pbRead,uint8_t bLastChar, uint8_t aui8_channel)
    {return __HAL::get_rs232_string(aui8_channel,pbRead,bLastChar);};

  /**
    send single uint8_t on RS232
    @param bByte data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send puffer overflow
  */
  inline int16_t put_rs232Char(uint8_t bByte, uint8_t aui8_channel)
    {return __HAL::put_rs232_char(aui8_channel,bByte);};
  /**
    send string of n uint8_t on RS232
    @param bpWrite pointer to source data string
    @param wNumber number of data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send puffer overflow
  */
  inline int16_t put_rs232NChar(const uint8_t *bpWrite,uint16_t wNumber, uint8_t aui8_channel)
    {return __HAL::put_rs232_n_char(aui8_channel,(uint8_t*)bpWrite,wNumber);};
  /**
    send '\0' terminated string on RS232
    @param pbString pointer to '\0' terminated (!) source data string
    @return HAL_NO_ERR -> o.k. else send puffer overflow
  */
  inline int16_t put_rs232String(const uint8_t *pbString, uint8_t aui8_channel)
    {return __HAL::put_rs232_string(aui8_channel,(uint8_t*)pbString);};

  /**
    clear receive puffer
  */
  inline void clearRs232RxBuffer(uint8_t aui8_channel)
    {__HAL::clear_rs232_rx_buffer(aui8_channel);};
  /**
    clear send puffer
  */
  inline void clearRs232TxBuffer(uint8_t aui8_channel)
    {__HAL::clear_rs232_tx_buffer(aui8_channel);};
  /*@}*/
}
#endif
