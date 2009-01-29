/***************************************************************************
                          rs232.h - definition of HAL
                                    namesapce layer for IMI
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2009 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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

#ifndef _HAL_IMI_RS232_H_
#define _HAL_IMI_RS232_H_

#include <IsoAgLib/hal/imi/config.h>
#include <IsoAgLib/hal/imi/typedef.h>
#include <IsoAgLib/hal/imi/errcodes.h>

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_imi/adis10.h>
  }
}
/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaptation to new platform
 */
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
  inline int16_t init_rs232(uint16_t wBaudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake)
    {return __HAL::init_rs232(wBaudrate,bMode,bStoppbits,bitSoftwarehandshake) ;};
	/** close the RS232 interface. */
	inline int16_t close_rs232()
		{__HAL::config_rs232_rx_obj(0,NULL); return __HAL::config_rs232_tx_obj(0,NULL,NULL);};
  /**
    set the RS232 Baudrate
    @param wBaudrate wanted baudrate
    @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
  */
  inline int16_t setRs232Baudrate(uint16_t wBaudrate)
    {return __HAL::set_rs232_baudrate(wBaudrate) ;};
  /**
    get the amount of data [uint8_t] in receive buffer
    @return receive buffer data byte
  */
  inline int16_t getRs232RxBufCount(void)
    {return __HAL::get_rs232_rx_buf_count();};
  /**
    get the amount of data [uint8_t] in send buffer
    @return send buffer data byte
  */
  inline int16_t getRs232TxBufCount(void)
    {return __HAL::get_rs232_tx_buf_count();};
  /**
    configure a receive buffer and set optional irq function pointer for receive
    @param wBuffersize wanted buffer size
    @param pFunction pointer to irq function or NULL if not wanted
  */
  inline int16_t configRs232RxObj(uint16_t wBuffersize,void (*pFunction)(uint8_t *bByte))
    {return __HAL::config_rs232_rx_obj(wBuffersize,pFunction) ;};
  /**
    configure a send buffer and set optional irq function pointer for send
    @param wBuffersize wanted buffer size
    @param funktionAfterTransmit pointer to irq function or NULL if not wanted
    @param funktionBeforTransmit pointer to irq function or NULL if not wanted
  */
  inline int16_t configRs232TxObj(uint16_t wBuffersize,void (*funktionAfterTransmit)(uint8_t *bByte),
                                  void (*funktionBeforTransmit)(uint8_t *bByte))
    {return __HAL::config_rs232_tx_obj(wBuffersize,funktionAfterTransmit,funktionBeforTransmit);};
  /**
    get errr code of BIOS
    @return 0=parity, 1=stopbit framing error, 2=overflow
  */
  inline int16_t getRs232Error(uint8_t *Errorcode)
    {return __HAL::get_rs232_error(Errorcode);};

  /**
    read single int8_t from receive buffer
    @param pbRead pointer to target data
    @return HAL_NO_ERR -> o.k. else buffer underflow
  */
  inline int16_t getRs232Char(uint8_t *pbRead)
    {return __HAL::get_rs232_char(pbRead);};
  /**
    read bLastChar terminated string from receive buffer
    @param pbRead pointer to target data
    @param bLastChar terminating char
    @return HAL_NO_ERR -> o.k. else buffer underflow
  */
  inline int16_t getRs232String(uint8_t *pbRead,uint8_t bLastChar)
    {return __HAL::get_rs232_string(pbRead,bLastChar);};

  /**
    send single uint8_t on RS232
    @param bByte data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  inline int16_t put_rs232Char(uint8_t bByte)
    {return __HAL::put_rs232_char(bByte);};
  /**
    send string of n uint8_t on RS232
    @param bpWrite pointer to source data string
    @param wNumber number of data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  inline int16_t put_rs232NChar(uint8_t *bpWrite,uint16_t wNumber)
    {return __HAL::put_rs232_n_char(bpWrite,wNumber);};
  /**
    send '\0' terminated string on RS232
    @param pbString pointer to '\0' terminated (!) source data string
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  inline int16_t put_rs232String(uint8_t *pbString)
    {return __HAL::put_rs232_string(pbString);};

  /**
    clear receive buffer
  */
  inline void clearRs232RxBuffer(void)
    {__HAL::clear_rs232_rx_buffer();};
  /**
    clear send buffer
  */
  inline void clearRs232TxBuffer(void)
    {__HAL::clear_rs232_tx_buffer();};
  /*@}*/
}
#endif
