/*
  config.h: header file for AMS5 (M32C/87) system dependent configs,
    defines and includes

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file      config.h
    \brief     header file for AMS5 (M32C/87)
               system dependent configs, defines and includes
*/

#ifndef _HAL_AMS5_CONFIG_H_
#define _HAL_AMS5_CONFIG_H_


#define SYSTEM_AMS5


#ifndef SIZEOF_INT
  #define SIZEOF_INT 2
#endif

#ifndef DEF_Stream_c_IMPL
  #define DEF_Stream_c_IMPL StreamChunk_c
#endif

#ifndef DEF_Stream_IMPL
  #define DEF_Stream_IMPL StreamChunk
#endif

#ifndef HAL_CAN_MAX_BUS_NR
  #define HAL_CAN_MAX_BUS_NR 1
#endif

#ifndef HAL_CAN_LAST_MSG_OBJ_NR
  #define HAL_CAN_LAST_MSG_OBJ_NR 15
#endif

#ifndef CONFIG_CAN_DEFAULT_MAX_OBJ_NR
  #define CONFIG_CAN_DEFAULT_MAX_OBJ_NR 14
#endif

#ifndef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  #define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN 1
#endif

#ifndef USE_LITTLE_ENDIAN_CPU
  #define USE_LITTLE_ENDIAN_CPU 1
#endif

   /** UART-Mode */
#ifndef UART_DATA_7_BITS_EVENPARITY
   #define UART_DATA_7_BITS_EVENPARITY  0
#endif
#ifndef UART_DATA_8_BITS_EVENPARITY
   #define UART_DATA_8_BITS_EVENPARITY  1
#endif
#ifndef UART_DATA_7_BITS_ODDPARITY
   #define UART_DATA_7_BITS_ODDPARITY   2
#endif
#ifndef UART_DATA_8_BITS_ODDPARITY
   #define UART_DATA_8_BITS_ODDPARITY   3
#endif
#ifndef UART_DATA_8_BITS_NOPARITY
   #define UART_DATA_8_BITS_NOPARITY    4
#endif
   /** UART-Baudrate */
#ifndef UART_600_BAUD
   #define  UART_600_BAUD   0
#endif
#ifndef UART_1200_BAUD
   #define  UART_1200_BAUD  1
#endif
#ifndef UART_2400_BAUD
   #define  UART_2400_BAUD  2
#endif
#ifndef UART_4800_BAUD
   #define  UART_4800_BAUD  3
#endif
#ifndef UART_9600_BAUD
   #define  UART_9600_BAUD  4
#endif
#ifndef UART_19200_BAUD
   #define  UART_19200_BAUD 5
#endif
#ifndef UART_38400_BAUD
   #define  UART_38400_BAUD 6
#endif
   /** RS485-Transceiver */
#ifndef REC485
   #define REC485    0
#endif
#ifndef SEND485
   #define SEND485   1
#endif
   /** UART-Buffer */
#ifndef UART_RX
   #define UART_RX   0
#endif
#ifndef UART_TX
   #define UART_TX   1
#endif
#ifndef UART_XON
   #define UART_XON  0x11
#endif
#ifndef UART_XOFF
   #define UART_XOFF 0x13
#endif
   /** I2C-Bus peripheral devices (slave address) */
#ifndef DS1307W
   #define DS1307W   0xD0 // RTC/NVRAM
#endif
#ifndef DS1307R
   #define DS1307R   0xD1
#endif
#ifndef AT24C64W
   #define AT24C64W  0xA0 // EEPROM
#endif
#ifndef AT24C64R
   #define AT24C64R  0xA1
#endif
#ifndef EEPROM_SIZE
   #define EEPROM_SIZE 8192
#endif
#ifndef MAX_EEPROM_SEGMENT_SIZE
   #define MAX_EEPROM_SEGMENT_SIZE 32
#endif
#ifndef MAX_EEPROM_WRITE_TRY_CYCLE_CNT
   #define MAX_EEPROM_WRITE_TRY_CYCLE_CNT 5
#endif
#ifndef EE_READY
   #define EE_READY 0
#endif
#ifndef EE_NOT_READY
   #define EE_NOT_READY 1
#endif
   /** main power voltage 12V or 24V in [mV] */
#ifndef AMS5_MAIN_POWER_VOLTAGE
   #define  AMS5_MAIN_POWER_VOLTAGE 12000
#endif
#ifndef SYSTEM_AMS5
   #define SYSTEM_AMS5
#endif
   // #define  AMS5_MAIN_POWER_VOLTAGE 24000

   /** voltage of the external reference: 2.5V in [mV] */
#ifndef AMS5_REFERENCE_VOLTAGE
   #define AMS5_REFERENCE_VOLTAGE 2500
#endif
   /** symbolic digital input channels */
   enum sysDigitalInputChannels
   {
      diX7_1,  diX7_2,  diX7_3,  diX7_4,  diX7_5,  diX7_6,   diX7_7,   diX7_8,
      diX8_1,  diX8_2,  diX8_3,  diX8_4,  diX8_5,  diX8_6,   diX8_7,   diX8_8,
      diX5_2,  diX5_4,  diX5_6,  diX5_8,  diX6_2,  diX6_4,   diX6_6,   diX6_8,
      diX19_2, diX19_3, diX19_4, diX20_3, diX20_4, diX19_5,  diX19_6,  diX21_5,
      diSW1_1, diSW1_2, diSW1_3, diSW1_4, diSW1_5, diSW1_6,  diSW1_7,  diSW1_8,
      diZ1_X1_7,   diZ1_X1_8,   diZ1_X1_9,   diZ1_X1_10,  diZ1_X1_11,  diZ1_X1_12,  diZ1_X1_13,  diZ1_X1_14,
      diZ1_X2_1,   diZ1_X2_2,   diZ1_X2_3,   diZ1_X2_4,   diZ1_X2_5,   diZ2_X1_7,   diZ2_X1_8,   diZ2_X1_9,
      diZ2_X1_10,  diZ2_X1_11,  diZ2_X1_12,  diZ2_X1_13,  diZ2_X1_14,  diZ2_X2_1,   diZ2_X2_2,   diZ2_X2_3,
      diZ2_X2_4,   diZ2_X2_5,   diZ3_X1_7,   diZ3_X1_8,   diZ3_X1_9,   diZ3_X1_10,  diZ3_X1_11,  diZ3_X1_12,
      diZ3_X1_13,  diZ3_X1_14,  diZ3_X2_1,   diZ3_X2_2,   diZ3_X2_3,   diZ3_X2_4,   diZ3_X2_5,   diZ4_X1_7,
      diZ4_X1_8,   diZ4_X1_9,   diZ4_X1_10,  diZ4_X1_11,  diZ4_X1_12,  diZ4_X1_13,  diZ4_X1_14,  diZ4_X2_1,
      diZ4_X2_2,   diZ4_X2_3,   diZ4_X2_4,   diZ4_X2_5,   diZ5_X1_7,   diZ5_X1_8,   diZ5_X1_9,   diZ5_X1_10,
      diZ5_X1_11,  diZ5_X1_12,  diZ5_X1_13,  diZ5_X1_14,  diZ5_X2_1,   diZ5_X2_2,   diZ5_X2_3,   diZ5_X2_4,
      diZ5_X2_5,   diZ6_X1_7,   diZ6_X1_8,   diZ6_X1_9,   diZ6_X1_10,  diZ6_X1_11,  diZ6_X1_12,  diZ6_X1_13,
      diZ6_X1_14,  diZ6_X2_1,   diZ6_X2_2,   diZ6_X2_3,   diZ6_X2_4,   diZ6_X2_5,   diZ7_X1_7,   diZ7_X1_8,
      diZ7_X1_9,   diZ7_X1_10,  diZ7_X1_11,  diZ7_X1_12,  diZ7_X1_13,  diZ7_X1_14,  diZ7_X2_1,   diZ7_X2_2,
      diZ7_X2_3,   diZ7_X2_4,   diZ7_X2_5,   diZ8_X1_7,   diZ8_X1_8,   diZ8_X1_9,   diZ8_X1_10,  diZ8_X1_11,
      diZ8_X1_12,  diZ8_X1_13,  diZ8_X1_14,  diZ8_X2_1,   diZ8_X2_2,   diZ8_X2_3,   diZ8_X2_4,   diZ8_X2_5
   };

   /** symbolic analog input channels */
   enum sysAnalogInputChannels
   {
      anaX8_1, anaX8_2, anaX8_3, anaX8_4, anaX8_5, anaX8_6,  anaX8_7,  anaX8_8
   };

   /** symbolic counter input channels */

   enum sysCounterInputChannels
   {
      cntX19_2, cntX19_3, cntX19_4, cntX20_3, cntX20_4, cntX19_5,  cntX19_6,  cntX21_5
   };

   /** symbol for counter 0 alternative use of pole wheel amplifier circuit (X20_5, X20_6) */
   #ifndef cntPOLE_WHEEL
     #define cntPOLE_WHEEL
   #endif
   /** symbolic interrupt input channels */
   enum sysIrqInputChannels
   {
      irqX19_5,  irqX19_6,  irqX21_5
   };

   /** symbolic output channels */
   enum sysOutputChannels
   {
      doREL_K6,  doREL_K5,  doREL_K4,  doREL_K3,  doREL_K2,  doREL_K1,  doLED_1,  doLED_2,
      doX11_5,   doX11_6,   doX12_1,   doX12_2,   doX12_3,   doX12_4,   doX12_5,  doX12_6,
      doX10_3,   doX10_4,   doX10_5,   doX10_6,   doX11_1,   doX11_2,   doX11_3,  doX11_4,
      doX9_1,    doX9_2,    doX9_3,    doX9_4,    doX9_5,    doX9_6,    doX10_1,  doX10_2,
      aoX16_1,     aoX16_3,     aoX16_5,     doMOTOR1,    doMOTOR2,    doZ1_REL_K1, doZ1_REL_K2, doZ1_REL_K3,
      doZ1_X1_15,  doZ1_X1_16,  doZ1_X1_17,  doZ1_X1_18,  doZ1_nc,     doZ1_X2_13,  doZ1_X2_14,  doZ1_X2_16,
      doZ1_X2_17,  doZ1_X2_18,  doZ1_X2_19,  doZ1_X2_20,  doZ1_X2_21,  doZ1_LED_3,  doZ1_LED_4,  doZ2_REL_K1,
      doZ2_REL_K2, doZ2_REL_K3, doZ2_X1_15,  doZ2_X1_16,  doZ2_X1_17,  doZ2_X1_18,  doZ2_nc,     doZ2_X2_13,
      doZ2_X2_14,  doZ2_X2_16,  doZ2_X2_17,  doZ2_X2_18,  doZ2_X2_19,  doZ2_X2_20,  doZ2_X2_21,  doZ2_LED_3,
      doZ2_LED_4,  doZ3_REL_K1, doZ3_REL_K2, doZ3_REL_K3, doZ3_X1_15,  doZ3_X1_16,  doZ3_X1_17,  doZ3_X1_18,
      doZ3_nc,     doZ3_X2_13,  doZ3_X2_14,  doZ3_X2_16,  doZ3_X2_17,  doZ3_X2_18,  doZ3_X2_19,  doZ3_X2_20,
      doZ3_X2_21,  doZ3_LED_3,  doZ3_LED_4,  doZ4_REL_K1, doZ4_REL_K2, doZ4_REL_K3, doZ4_X1_15,  doZ4_X1_16,
      doZ4_X1_17,  doZ4_X1_18,  doZ4_nc,     doZ4_X2_13,  doZ4_X2_14,  doZ4_X2_16,  doZ4_X2_17,  doZ4_X2_18,
      doZ4_X2_19,  doZ4_X2_20,  doZ4_X2_21,  doZ4_LED_3,  doZ4_LED_4,  doZ5_REL_K1, doZ5_REL_K2, doZ5_REL_K3,
      doZ5_X1_15,  doZ5_X1_16,  doZ5_X1_17,  doZ5_X1_18,  doZ5_nc,     doZ5_X2_13,  doZ5_X2_14,  doZ5_X2_16,
      doZ5_X2_17,  doZ5_X2_18,  doZ5_X2_19,  doZ5_X2_20,  doZ5_X2_21,  doZ5_LED_3,  doZ5_LED_4,  doZ6_REL_K1,
      doZ6_REL_K2, doZ6_REL_K3, doZ6_X1_15,  doZ6_X1_16,  doZ6_X1_17,  doZ6_X1_18,  doZ6_nc,     doZ6_X2_13,
      doZ6_X2_14,  doZ6_X2_16,  doZ6_X2_17,  doZ6_X2_18,  doZ6_X2_19,  doZ6_X2_20,  doZ6_X2_21,  doZ6_LED_3,
      doZ6_LED_4,  doZ7_REL_K1, doZ7_REL_K2, doZ7_REL_K3, doZ7_X1_15,  doZ7_X1_16,  doZ7_X1_17,  doZ7_X1_18,
      doZ7_nc,     doZ7_X2_13,  doZ7_X2_14,  doZ7_X2_16,  doZ7_X2_17,  doZ7_X2_18,  doZ7_X2_19,  doZ7_X2_20,
      doZ7_X2_21,  doZ7_LED_3,  doZ7_LED_4,  doZ8_REL_K1, doZ8_REL_K2, doZ8_REL_K3, doZ8_X1_15,  doZ8_X1_16,
      doZ8_X1_17,  doZ8_X1_18,  doZ8_nc,     doZ8_X2_13,  doZ8_X2_14,  doZ8_X2_16,  doZ8_X2_17,  doZ8_X2_18,
      doZ8_X2_19,  doZ8_X2_20,  doZ8_X2_21,  doZ8_LED_3,  doZ8_LED_4  
   };

   /** old style AMS5 symbolic */
   enum sysOldStyleInputChannels
   {
      DE_1,   DE_2,   DE_3,   DE_4,   DE_5,   DE_6,    DE_7,    DE_8,
      DE_9,   DE_10,  DE_11,  DE_12,  DE_13,  DE_14,   DE_15,   DE_16,
      DE_A1,  DE_A2,  DE_A3,  DE_A4,  DE_A5,  DE_A6,   DE_A7,   DE_A8,
      DE_Z1,  DE_Z2,  DE_Z3,  DE_Z4,  DE_Z5,  DE_INT1, DE_INT2, DE_AUX,
      S_1,    S_2,    S_3,    S_4,    S_5,    S_6,     S_7,     S_8
   };

   /** old style AMS5 symbolic */
   enum sysOldStyleOutputChannels
   {
      REL_6,  REL_5,  REL_4,  REL_3,  REL_2,  REL_1,  STAT_1, STAT_2,
      DA_17,  DA_18,  DA_19,  DA_20,  DA_21,  DA_22,  DA_23,  DA_24,
      DA_9,   DA_10,  DA_11,  DA_12,  DA_13,  DA_14,  DA_15,  DA_16,
      DA_1,   DA_2,   DA_3,   DA_4,   DA_5,   DA_6,   DA_7,   DA_8,
      AA_1,   AA_2,   AA_3,   MOT_1,  MOT_2
   };

#ifndef HUGE_MEM
  #define HUGE_MEM
#endif

/** define suitable  keyword for near memory type */
#define NEAR_MEM      __near
#define USE_NEAR_MEM

#endif
