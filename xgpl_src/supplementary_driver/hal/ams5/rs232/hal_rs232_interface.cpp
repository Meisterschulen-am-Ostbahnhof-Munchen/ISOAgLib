/***************************************************************************
    hal_rs232_interface.cpp - definition of AMS5 RS232 interface

    -------------------
    date                 : 18.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Module
 ***************************************************************************/


#include "hal_rs232_interface.h"
#include <..\commercial_BIOS\bios_ams5\ams_bios.h>

#include <stddef.h>

namespace __HAL
{
   /* ****************************** */
   /** \name RS232 I/O BIOS functions */
   /*@{*/

   /**
      init the RS232 interface
      @param wBaudrate wanted Baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}
             as configured in <Application_Config/isoaglib_config.h>

      @param bMode one of (DATA_7_BITS_EVENPARITY = 1, DATA_8_BITS_EVENPARITY = 2,
             DATA_7_BITS_ODDPARITY = 3, DATA_8_BITS_ODDPARITY = 4, DATA_8_BITS_NOPARITY = 5)
      @param bStoppbits amount of stop bits (1,2)
      @param bitSoftwarehandshake true -> use xon/xoff software handshake
      @return HAL_NO_ERR -> o.k. else one of settings incorrect
   */
   int16_t init_rs232(uint16_t wBaudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake, uint8_t rui8_channel)
   {
      uartParameter_t Parameter;

      // Baudrate  (see config.h)
      // UART_38400_BAUD is not supported by HAL
      // baudrate 75     is not supported by BIOS
      switch ( wBaudrate )
      {
         case 38400: Parameter.baudrate = UART_38400_BAUD; break;
         case 19200: Parameter.baudrate = UART_19200_BAUD; break;
         case  9600: Parameter.baudrate = UART_9600_BAUD;  break;
         case  4800: Parameter.baudrate = UART_4800_BAUD;  break;
         case  2400: Parameter.baudrate = UART_2400_BAUD;  break;
         case  1200: Parameter.baudrate = UART_1200_BAUD;  break;
         case   600: Parameter.baudrate = UART_600_BAUD;   break;
         case    75: 
         default:   return HAL_CONFIG_ERR;
      }

      // Data-Mode (see config.h)
      switch ( bMode )
      {
         case 1: Parameter.data_mode = UART_DATA_7_BITS_EVENPARITY; break;
         case 2: Parameter.data_mode = UART_DATA_8_BITS_EVENPARITY; break;
         case 3: Parameter.data_mode = UART_DATA_7_BITS_ODDPARITY;  break;
         case 4: Parameter.data_mode = UART_DATA_8_BITS_ODDPARITY;  break;
         case 5: Parameter.data_mode = UART_DATA_8_BITS_NOPARITY;   break;
         default:   return HAL_CONFIG_ERR;
      }

      Parameter.stopbits = bStoppbits;   // amount of stop bits (1,2)
      Parameter.handshake= bitSoftwarehandshake;  // true -> use xon/xoff software handshake

      return AMSBIOS::uart_openChannel(rui8_channel, &Parameter);
   }

   /** close the RS232 interface. */
   int16_t close_rs232(uint8_t rui8_channel)
   {
      return AMSBIOS::uart_closeChannel(rui8_channel);
   }

   /**
      set the RS232 Baudrate
      @param wBaudrate wanted baudrate
      @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
   */
   int16_t setRs232Baudrate(uint16_t wBaudrate, uint8_t rui8_channel)
   {
      uint8_t Baudrate;
      
      // Baudrate  (see config.h)
      // UART_38400_BAUD is not supported by HAL
      // baudrate 75     is not supported by BIOS
      switch ( wBaudrate )
      {
         case 38400: Baudrate = UART_38400_BAUD; break;
         case 19200: Baudrate = UART_19200_BAUD; break;
         case  9600: Baudrate = UART_9600_BAUD;  break;
         case  4800: Baudrate = UART_4800_BAUD;  break;
         case  2400: Baudrate = UART_2400_BAUD;  break;
         case  1200: Baudrate = UART_1200_BAUD;  break;
         case   600: Baudrate = UART_600_BAUD;   break;
         case    75: 
         default:   return HAL_CONFIG_ERR;
      }

      return AMSBIOS::uart_setBaudrate(rui8_channel, Baudrate);
   }

   /**
      get the amount of data [uint8_t] in receive buffer
      @return receive buffer data byte
   */
   int16_t getRs232RxBufCount(uint8_t rui8_channel)
   {
      return AMSBIOS::uart_getBufferCount(rui8_channel, UART_RX);
   }

   /**
      get the amount of data [uint8_t] in send buffer
      @return send buffer data byte
   */
   int16_t getRs232TxBufCount(uint8_t rui8_channel)
   {
      return AMSBIOS::uart_getBufferCount(rui8_channel, UART_TX);
   }

   /**
      configure a receive buffer and set optional irq function pointer for receive
      @param wBuffersize wanted buffer size
      @param pFunction pointer to irq function or NULL if not wanted
   */
   int16_t configRs232RxObj(uint16_t wBuffersize,void (*pFunction)(uint8_t *bByte), uint8_t rui8_channel)
   {
      uartConfigObj_t ConfigObj;

      ConfigObj.bufferType       = UART_RX;
      ConfigObj.bufferSize       = wBuffersize;
      ConfigObj.UART_FunctionA_p = pFunction;
      ConfigObj.UART_FunctionB_p = NULL;

      return AMSBIOS::uart_configChannel(rui8_channel, &ConfigObj);
   }

   /**
      configure a send buffer and set optional irq function pointer for send
      @param wBuffersize wanted buffer size
      @param funktionAfterTransmit pointer to irq function or NULL if not wanted
      @param funktionBeforTransmit pointer to irq function or NULL if not wanted
   */
   int16_t configRs232TxObj(uint16_t wBuffersize,void (*funktionAfterTransmit)(uint8_t *bByte),
                                   void (*funktionBeforTransmit)(uint8_t *bByte),uint8_t rui8_channel)
   {
      uartConfigObj_t ConfigObj;

      ConfigObj.bufferType       = UART_TX;
      ConfigObj.bufferSize       = wBuffersize;
      ConfigObj.UART_FunctionA_p = funktionBeforTransmit;
      ConfigObj.UART_FunctionB_p = funktionAfterTransmit;

      return AMSBIOS::uart_configChannel(rui8_channel, &ConfigObj);
   }

   /**
      get errr code of BIOS
      @return 0=parity, 1=stopbit framing error, 2=overflow
   */
   int16_t getRs232Error(uint8_t *Errorcode, uint8_t rui8_channel)
   {
      return AMSBIOS::uart_getError(rui8_channel, Errorcode);
   }

   /**
      read single int8_t from receive buffer
      @param pbRead pointer to target data
      @return HAL_NO_ERR -> o.k. else buffer underflow
   */
   int16_t getRs232Char(uint8_t *pbRead, uint8_t rui8_channel)
   {
      return AMSBIOS::uart_getChar(rui8_channel, pbRead);
   }

   /**
      read bLastChar terminated string from receive buffer
      @param pbRead pointer to target data
      @param bLastChar terminating char
      @return HAL_NO_ERR -> o.k. else buffer underflow
   */
   int16_t getRs232String(uint8_t *pbRead, uint8_t bLastChar, uint8_t rui8_channel)
   {
      int16_t RetVal = HAL_OVERFLOW_ERR;

      while ( AMSBIOS::uart_getChar(rui8_channel, pbRead) == HAL_NO_ERR )
      {
         if (*pbRead == bLastChar)
         {
            RetVal = HAL_NO_ERR;
            break;
         }
         else 
         {
            pbRead++;
         }
      }

 	   *pbRead = '\0';
      return RetVal;
   }

   /**
      send single uint8_t on RS232
      @param bByte data uint8_t to send
      @return HAL_NO_ERR -> o.k. else send buffer overflow
    */
   int16_t put_rs232Char(uint8_t bByte, uint8_t rui8_channel)
   {
      return AMSBIOS::uart_putChar(rui8_channel, bByte);
   }

   /**
      send string of n uint8_t on RS232
      @param bpWrite pointer to source data string
      @param wNumber number of data uint8_t to send
      @return HAL_NO_ERR -> o.k. else send buffer overflow
   */
   int16_t put_rs232NChar(const uint8_t *bpWrite, uint16_t wNumber, uint8_t rui8_channel)
   {
      int16_t RetVal;
      
      while (wNumber--)
      {
         RetVal = AMSBIOS::uart_putChar(rui8_channel, *bpWrite++);
         if (RetVal != HAL_NO_ERR) break; 
      }
      return RetVal;
   }

   /**
      send '\0' terminated string on RS232
      @param pbString pointer to '\0' terminated (!) source data string
      @return HAL_NO_ERR -> o.k. else send buffer overflow
   */
   int16_t put_rs232String(const uint8_t *pbString, uint8_t rui8_channel)
   {
      int16_t RetVal;
      
      while (*pbString)
      {
         RetVal = AMSBIOS::uart_putChar(rui8_channel, *pbString++);
         if (RetVal != HAL_NO_ERR) break; 
      }
      return RetVal;
   }

   /**
      clear receive buffer
   */
   void clearRs232RxBuffer(uint8_t rui8_channel)
   {
      AMSBIOS::uart_resetBuffer(rui8_channel, UART_RX);
   }

   /**
      clear send buffer
   */
   void clearRs232TxBuffer(uint8_t rui8_channel)
   {
      AMSBIOS::uart_resetBuffer(rui8_channel, UART_TX);
   }
   /*@}*/
}

