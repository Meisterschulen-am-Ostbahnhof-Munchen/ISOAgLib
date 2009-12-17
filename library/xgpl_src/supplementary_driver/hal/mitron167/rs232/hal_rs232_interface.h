/*
  hal_rs232_interface.h: declaration of Mitron167 RS232 interface

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

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

#ifndef _HAL_MITRON167_RS232_INTERFACE_H_
#define _HAL_MITRON167_RS232_INTERFACE_H_

#include <IsoAgLib/hal/mitron167/config.h>
#include <IsoAgLib/hal/mitron167/typedef.h>
#include <IsoAgLib/hal/mitron167/errcodes.h>
namespace __HAL {
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
    get the amount of data [uint8_t] in receive buffer
    @return receive buffer data byte
  */
  int16_t getRs232RxBufCount(void);
    /**
    get the amount of data [uint8_t] in send buffer
    @return send buffer data byte
  */
  int16_t getRs232TxBufCount(void);
    /**
    configure a receive buffer and set optional irq function pointer for receive
    @param wBuffersize wanted buffer size
    @param pFunction pointer to irq function or NULL if not wanted
  */
  int16_t configRs232RxObj(uint16_t wBuffersize,void (*pFunction)(uint8_t _huge *bByte));
    /**
    configure a send buffer and set optional irq function pointer for send
    @param wBuffersize wanted buffer size
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
    read single int8_t from receive buffer
    @param pbRead pointer to target data
    @return HAL_NO_ERR -> o.k. else buffer underflow
  */
  int16_t getRs232Char(uint8_t *pbRead);
  /**
    read bLastChar terminated string from receive buffer
    @param pbRead pointer to target data
    @param bLastChar terminating char
    @return HAL_NO_ERR -> o.k. else buffer underflow
  */
  int16_t getRs232String(uint8_t *pbRead,uint8_t bLastChar);

  /**
    send single uint8_t on RS232
    @param bByte data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  int16_t put_rs232Char(uint8_t bByte);
  /**
    send string of n uint8_t on RS232
    @param bpWrite pointer to source data string
    @param wNumber number of data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  int16_t put_rs232NChar(const uint8_t *bpWrite,uint16_t wNumber);
  /**
    send '\\0' terminated string on RS232
    @param pbString pointer to '\\0' terminated (!) source data string
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  int16_t put_rs232String(const uint8_t *pbString);

  /**
    clear receive buffer
  */
  void clearRs232RxBuffer(void);
  /**
    clear send buffer
  */
  void clearRs232TxBuffer(void);
}
#endif
