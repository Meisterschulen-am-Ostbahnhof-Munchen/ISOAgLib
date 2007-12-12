/***************************************************************************
    config.h  -  system dependent configs, defines and includes
    -------------------
    date                 : 18.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
 ***************************************************************************/

#ifndef _HAL_AMS5_CONFIG_H_
   #define _HAL_AMS5_CONFIG_H_

#define HAL_CAN_MAX_BUS_NR 1

#define SIZEOF_INT 2

#define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN 1
#define USE_LITTLE_ENDIAN_CPU 1

   /** UART-Mode */
   #define UART_DATA_7_BITS_EVENPARITY  0
   #define UART_DATA_8_BITS_EVENPARITY  1
   #define UART_DATA_7_BITS_ODDPARITY   2
   #define UART_DATA_8_BITS_ODDPARITY   3
   #define UART_DATA_8_BITS_NOPARITY    4

   /** UART-Baudrate */
   #define  UART_600_BAUD   0
   #define  UART_1200_BAUD  1
   #define  UART_2400_BAUD  2
   #define  UART_4800_BAUD  3
   #define  UART_9600_BAUD  4
   #define  UART_19200_BAUD 5
   #define  UART_38400_BAUD 6

   /** RS485-Transceiver */
   #define REC485    0
   #define SEND485   1

   /** UART-Buffer */
   #define UART_RX   0
   #define UART_TX   1
   #define UART_XON  0x11
   #define UART_XOFF 0x13

   /** I2C-Bus peripheral devices (slave address) */
   #define DS1307W   0xD0 // RTC/NVRAM
   #define DS1307R   0xD1
   #define AT24C64W  0xA0 // EEPROM
   #define AT24C64R  0xA1

   #define EEPROM_SIZE 32
   #define MAX_EEPROM_SEGMENT_SIZE 32
   #define MAX_EEPROM_WRITE_TRY_CYCLE_CNT 5
   #define EE_READY 0
   #define EE_NOT_READY 1

   /** main power voltage 12V or 24V in [mV] */
   #define  AMS5_MAIN_POWER_VOLTAGE 12000
   // #define  AMS5_MAIN_POWER_VOLTAGE 24000

   /** voltage of the external reference: 2.5V in [mV] */
   #define AMS5_REFERENCE_VOLTAGE 2500

   /** symbolic digital input channels */
   enum sysDigitalInputChannels
   {
      diX7_1,  diX7_2,  diX7_3,  diX7_4,  diX7_5,  diX7_6,   diX7_7,   diX7_8,
      diX8_1,  diX8_2,  diX8_3,  diX8_4,  diX8_5,  diX8_6,   diX8_7,   diX8_8,
      diX5_2,  diX5_4,  diX5_6,  diX5_8,  diX6_2,  diX6_4,   diX6_6,   diX6_8,
      diX19_2, diX19_3, diX19_4, diX20_3, diX20_4, diX19_5,  diX19_6,  diX21_5,
      diSW1_1, diSW1_2, diSW1_3, diSW1_4, diSW1_5, diSW1_6,  diSW1_7,  diSW1_8
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
   #define cntPOLE_WHEEL

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
      aoX16_1,   aoX16_3,   aoX16_5,   doMOTOR1,  doMOTOR2
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

#define HUGE_MEM

/** define suitable  keyword for near memory type */
#define NEAR_MEM      __near
#define USE_NEAR_MEM

#endif
