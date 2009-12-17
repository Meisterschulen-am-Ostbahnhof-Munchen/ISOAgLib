/*
  rs232.h: definition of HAL namespace layer for AMS5

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_AMS5_RS232_H_
#define _HAL_AMS5_RS232_H_


#include "hal_rs232_interface.h"

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
   {
      return __HAL::init_rs232(wBaudrate, bMode, bStoppbits, bitSoftwarehandshake, aui8_channel);
   };

   /** close the RS232 interface. */
   inline int16_t close_rs232(uint8_t aui8_channel)
   {
      return __HAL::close_rs232(aui8_channel);
   };

   /**
      set the RS232 Baudrate
      @param wBaudrate wanted baudrate
      @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
   */
   inline int16_t setRs232Baudrate(uint16_t wBaudrate, uint8_t aui8_channel)
   {
      return __HAL::setRs232Baudrate(wBaudrate, aui8_channel);
   };

   /**
      get the amount of data [uint8_t] in receive buffer
      @return receive buffer data byte
   */
   inline int16_t getRs232RxBufCount(uint8_t aui8_channel)
   {
      return __HAL::getRs232RxBufCount(aui8_channel);
   };

   /**
      get the amount of data [uint8_t] in send buffer
      @return send buffer data byte
   */
   inline int16_t getRs232TxBufCount(uint8_t aui8_channel)
   {
      return __HAL::getRs232TxBufCount(aui8_channel);
   };

   /**
      configure a receive buffer and set optional irq function pointer for receive
      @param wBuffersize wanted buffer size
      @param pFunction pointer to irq function or NULL if not wanted
   */
   inline int16_t configRs232RxObj(uint16_t wBuffersize,void (*pFunction)(uint8_t *bByte), uint8_t aui8_channel)
   {
      return __HAL::configRs232RxObj(wBuffersize, pFunction, aui8_channel);
   };

   /**
      configure a send buffer and set optional irq function pointer for send
      @param wBuffersize wanted buffer size
      @param funktionAfterTransmit pointer to irq function or NULL if not wanted
      @param funktionBeforTransmit pointer to irq function or NULL if not wanted
   */
   inline int16_t configRs232TxObj(uint16_t wBuffersize,void (*funktionAfterTransmit)(uint8_t *bByte),
                                   void (*funktionBeforTransmit)(uint8_t *bByte),uint8_t aui8_channel)
   {
      return __HAL::configRs232TxObj(wBuffersize, funktionAfterTransmit, funktionBeforTransmit, aui8_channel);
   };

   /**
      get errr code of BIOS
      @return 0=parity, 1=stopbit framing error, 2=overflow
   */
   inline int16_t getRs232Error(uint8_t *Errorcode, uint8_t aui8_channel)
   {
      return __HAL::getRs232Error(Errorcode, aui8_channel);
   };

   /**
      read single int8_t from receive buffer
      @param pbRead pointer to target data
      @return HAL_NO_ERR -> o.k. else buffer underflow
   */
   inline int16_t getRs232Char(uint8_t *pbRead, uint8_t aui8_channel)
   {
      return __HAL::getRs232Char(pbRead, aui8_channel);
   };

   /**
      read bLastChar terminated string from receive buffer
      @param pbRead pointer to target data
      @param bLastChar terminating char
      @return HAL_NO_ERR -> o.k. else buffer underflow
   */
   inline int16_t getRs232String(uint8_t *pbRead,uint8_t bLastChar, uint8_t aui8_channel)
   {
      return __HAL::getRs232String(pbRead, bLastChar, aui8_channel);
   };

   /**
      send single uint8_t on RS232
      @param bByte data uint8_t to send
      @return HAL_NO_ERR -> o.k. else send buffer overflow
    */
   inline int16_t put_rs232Char(uint8_t bByte, uint8_t aui8_channel)
   {
      return __HAL::put_rs232Char(bByte, aui8_channel);
   };

   /**
      send string of n uint8_t on RS232
      @param bpWrite pointer to source data string
      @param wNumber number of data uint8_t to send
      @return HAL_NO_ERR -> o.k. else send buffer overflow
   */
   inline int16_t put_rs232NChar(const uint8_t *bpWrite,uint16_t wNumber, uint8_t aui8_channel)
   {
      return __HAL::put_rs232NChar((uint8_t*)bpWrite, wNumber, aui8_channel);
   };

   /**
      send '\\0' terminated string on RS232
      @param pbString pointer to '\\0' terminated (!) source data string
      @return HAL_NO_ERR -> o.k. else send buffer overflow
   */
   inline int16_t put_rs232String(const uint8_t *pbString, uint8_t aui8_channel)
   {
      return __HAL::put_rs232String((uint8_t*)pbString, aui8_channel);
   };

   /**
      clear receive buffer
   */
   inline void clearRs232RxBuffer(uint8_t aui8_channel)
   {
      __HAL::clearRs232RxBuffer(aui8_channel);
   };

   /**
      clear send buffer
   */
   inline void clearRs232TxBuffer(uint8_t aui8_channel)
   {
      __HAL::clearRs232TxBuffer(aui8_channel);
   };
   /*@}*/
}
#endif
