/***************************************************************************
                          hal_rs232_interface.h - declaration of Mitron167
                                                  RS232 interface
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
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
/** \file supplementary_driver/hal/mitron167/rs232/rs232.h
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

#ifndef _HAL_MITRON167_RS232_INTERFACE_H_
#define _HAL_MITRON167_RS232_INTERFACE_H_

#include <IsoAgLib/hal/mitron167/config.h>
#include <IsoAgLib/hal/mitron167/typedef.h>
#include <IsoAgLib/hal/mitron167/errcodes.h>
namespace __HAL {
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
  int16_t init_rs232(uint16_t wBaudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake);
	/** close the RS232 interface. */
	int16_t close_rs232();
  /**
    set the RS232 Baudrate
    @param wBaudrate wanted baudrate
    @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
  */
  int16_t setRs232Baudrate(uint16_t wBaudrate);
    /**
    get the amount of data [uint8_t] in receive puffer
    @return receive puffer data byte
  */
  int16_t getRs232RxBufCount(void);
    /**
    get the amount of data [uint8_t] in send puffer
    @return send puffer data byte
  */
  int16_t getRs232TxBufCount(void);
    /**
    configure a receive puffer and set optional irq function pointer for receive
    @param wBuffersize wanted puffer size
    @param pFunction pointer to irq function or NULL if not wanted
  */
  int16_t configRs232RxObj(uint16_t wBuffersize,void (*pFunction)(uint8_t _huge *bByte));
    /**
    configure a send puffer and set optional irq function pointer for send
    @param wBuffersize wanted puffer size
    @param funktionAfterTransmit pointer to irq function or NULL if not wanted
    @param funktionBeforTransmit pointer to irq function or NULL if not wanted
  */
  int16_t configRs232TxObj(uint16_t wBuffersize,void (*funktionAfterTransmit)(uint8_t _huge *bByte),
                                  void (*funktionBeforTransmit)(uint8_t _huge *bByte));

 /**
    get errr code of BIOS
    @return 0=parity, 1=stopbit framing error, 2=overflow
  */
  int16_t getRs232Error(uint8_t *Errorcode);

  /**
    read single int8_t from receive puffer
    @param pbRead pointer to target data
    @return HAL_NO_ERR -> o.k. else puffer underflow
  */
  int16_t getRs232Char(uint8_t *pbRead);
  /**
    read bLastChar terminated string from receive puffer
    @param pbRead pointer to target data
    @param bLastChar terminating char
    @return HAL_NO_ERR -> o.k. else puffer underflow
  */
  int16_t getRs232String(uint8_t *pbRead,uint8_t bLastChar);

  /**
    send single uint8_t on RS232
    @param bByte data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send puffer overflow
  */
  int16_t put_rs232Char(uint8_t bByte);
  /**
    send string of n uint8_t on RS232
    @param bpWrite pointer to source data string
    @param wNumber number of data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send puffer overflow
  */
  int16_t put_rs232NChar(const uint8_t *bpWrite,uint16_t wNumber);
  /**
    send '\0' terminated string on RS232
    @param pbString pointer to '\0' terminated (!) source data string
    @return HAL_NO_ERR -> o.k. else send puffer overflow
  */
  int16_t put_rs232String(const uint8_t *pbString);

  /**
    clear receive puffer
  */
  void clearRs232RxBuffer(void);
  /**
    clear send puffer
  */
  void clearRs232TxBuffer(void);
}
#endif
