/***************************************************************************
    typedef.h  -  system dependent basic type definitions
    -------------------
    date                 : 26.06.2007
    copyright            : (C) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
 ***************************************************************************/

#ifndef _TYPEDEF_AMS5_H
   #define _TYPEDEF_AMS5_H

   #include <stdint.h>

   /* --- GLOBAL DEFINES CONSTANTS --- */
   #define USER_WATCHDOG           0      /* Define for User-Watchdog */
   #define DATA_CHANGED            1      /* Returnvalue from config_wd */

   #define FALSE             0
   #define TRUE              1
   #define HIGH              1
   #define LOW               0

   #define ON                1
   #define OFF               0


    typedef struct
   {
      uint8_t ipTA3;
      uint8_t ipTBX;
      uint8_t ipTB5;
      uint8_t ipADC;
      uint8_t ipCAN1rx;
      uint8_t ipCAN1tx;
      uint8_t ipCAN1err;
      uint8_t ipC0rxINT6;
      uint8_t ipC0txINT7;
      uint8_t ipC0errINT8;
      uint8_t ipUART0rx;
      uint8_t ipUART0tx;
      uint8_t ipUART1rx;
      uint8_t ipUART1tx;
   }
   sysIrqPriority_t;

   /** RTC - clock and calendar */
   typedef struct
   {
      uint8_t seconds;
      uint8_t minutes;
      uint8_t hours;
      uint8_t day;
      uint8_t month;
      uint8_t year;
      uint8_t control;
   }
   sysRTC_t;

   /** 32 digital inputs */
   typedef struct
   {
      uint16_t DigInL;
      uint16_t DigInH;
   }
   ioDigIn_t;

   typedef struct
   {
      uint32_t Counts;
      uint32_t MFrequency;
   }
   ioCounterIn_t;

   typedef struct
   {
      uint16_t Period;
      uint16_t Frequency;
      uint32_t Counts;
      uint32_t Timestamp;
   }
   ioCounterValues_t;


   /** interrupt input hook function */
   typedef void (*irqFunction_pt) (void);

   typedef struct
   {
      uint32_t GlobalMask;   // Global mask register
      uint8_t GM_IdType;
      uint32_t LocalMaskA;   // Local mask A register
      uint8_t LMA_IdType;
      uint32_t LocalMaskB;   // Local mask B register
      uint8_t LMB_IdType;
   }
   canMaskStruct_t;

   /**
      CAN bus error state and timestamps

      copy of CANi Status Register: uint16_t .statusReg

      error flags at last error interrupt (.lastError timestamp): uint16_t .lastFlags:
      bit number   set to "1" when...
        0:           ... an ACK error is detected
        1:           ... a CRC error is detected
        2:           ... a form error is detected
        3:           ... a stuff error is detected
        4:           ... a bit error is detected while transmitting recessive "H"
        5:           ... a bit error is detected while transmitting dominant "L"
        6:           ... an error is detected while receiving data
        7:           ... an error is detected while transmitting data
        8:           ... an CAN communication error is detected (see errors above)
       10:           ... the CAN module is placed in an error-passive state
       11:           ... the CAN module is placed in a bus-off state
       12 to 15:     nothing is assigned

      Timestamps [ms] uint32_t:
      last error interrupt:                       .lastError
      last succsessful reception:                 .lastSuccRx
      last succsessful transmission:              .lastSuccTx
      last time placed in an error-passive state: .lastErrPas
      last time placed in a bus-off state:        .lastBusOff
      last time Bit1-Error detected:              .lastBit1Err
   */
   typedef struct
   {
      uint16_t statusReg;
      uint16_t lastFlags;
      uint32_t lastError;
      uint32_t lastSuccRx;
      uint32_t lastSuccTx;
      uint32_t lastErrPas;
      uint32_t lastBusOff;
      uint32_t lastBit1Err;
   }
   canErrorStateStruct_t;

   /** CAN message with timestamp */
   typedef struct
   {
      uint32_t dwId;        // Identifier
      uint8_t  idType;    // ID-Type: SID (11 Bit) = FALSE
                          //          EID (29 Bit) = TRUE
      uint8_t  dlc;       // data length code (0..8)
      uint8_t  data[8];   // data field
      uint32_t timestamp; // [ms]
   }
   canMsg_t;

   /** pointer type to CAN message with timestamp (ringbuffer element) */
   typedef canMsg_t*  canBuffer_pt;

   /** CAN Config-Objekt */
   typedef struct
   {
      uint8_t  objNr;       // number of CAN objekt
      uint32_t dwId;          // Identifier
      uint8_t  idType;      // ID-Type: SID (11 Bit) = FALSE
                            //          EID (29 Bit) = TRUE
      uint8_t  direction;   // receive (0) or transmit (1)
      uint16_t bufferSize;  // size of buffer
      bool     bufferLock;  // state after init.: lock (true) / unlock (false)
   }
   canConfigObj_t;

   /** pointer-type to CAN Config-Objekt */
   typedef canConfigObj_t* canConfigObj_pt;

   /**
      CAN message box structure at byte access
   */
   struct canSlotByte_s
   {
      uint8_t sidh;              /** Standard-ID register [10:6] */
      uint8_t sidl;              /** Standard-ID register [5:0] */
      uint8_t eidh;              /** Extended-ID register [17:14] */
      uint8_t eidm;              /** Extended-ID register [13:6] */
      uint8_t eidl;              /** Extended-ID register [5:0] */
      uint8_t dlc;               /** Data length code */
      uint8_t data[8];           /** Data(8byte) */
      uint8_t tsh;               /** Time stamp register(High) */
      uint8_t tsl;               /** Time stamp register(Low) */
   };

   /**
      CAN message box structure at word access
   */
   struct canSlotWord_s
   {
      uint8_t sidl;              /** Standard-ID register [5:0] */
      uint8_t sidh;              /** Standard-ID register [10:6] */
      uint8_t eidm;              /** Extended-ID register [13:6] */
      uint8_t eidh;              /** Extended-ID register [17:14] */
      uint8_t dlc;               /** Data length code */
      uint8_t eidl;              /** Extended-ID register [5:0] */
      uint8_t data[8];           /** Data(8byte) */
      uint8_t tsl;               /** Time stamp register(Low) */
      uint8_t tsh;               /** Time stamp register(High) */
   };

   typedef union
   {
      struct canSlotByte_s ba;
      struct canSlotWord_s wa;
      uint8_t              byte[16];
      uint16_t             word[8];
   }
   canSlotMBox_t;


   /**
      CAN message control register
   */
   typedef union
   {
      struct
      {
         uint8_t newdata:1;      /** Successful reception flag */
         uint8_t invaldata:1;    /** Under reception flag */
         uint8_t msglost:1;      /** Overwrite flag */
         uint8_t remactive:1;    /** Remote frame transmission/reception status flag */
         uint8_t rsplock:1;      /** Transmission reception auto response lock mode select bit */
         uint8_t remote:1;       /** Remote frame corresponding slot select bit */
         uint8_t recreq:1;       /** Reception slot requiest bit */
         uint8_t dummy:1;        /** Reserved bit */
      }
      receive;
      struct
      {
         uint8_t sentdata:1;     /** Successful transmission flag */
         uint8_t trmactive:1;    /** Under transmission flag */
         uint8_t dummy1:1;       /** Reserved bit */
         uint8_t remactive:1;    /** Remote frame transmission/reception status flag */
         uint8_t dummy2:1;       /** Reserved bit */
         uint8_t remote:1;       /** Remote frame corresponding slot select bit */
         uint8_t dummy3:1;       /** Reserved bit */
         uint8_t trmreq:1;       /** Transmission slot request bit */
      }
      transmit;
      uint8_t byte;
   }
   canMCTL_t;


   /** UART parameter list */
   typedef struct
   {
      uint8_t  baudrate;   // Baudrate  (see config.h)
      uint8_t  data_mode;  // Data-Mode (see config.h)
      uint8_t  stopbits;   // amount of stop bits (1,2)
      bool     handshake;  // true -> use xon/xoff software handshake
   }
   uartParameter_t;

   /** pointer-type to UART parameter list */
   typedef uartParameter_t* uartParameter_pt;

   /** UART hook function */
   typedef void (*uartFunction_pt) (uint8_t *Byte_p);

   /** UART Config-Objekt */
   typedef struct
   {
      uint8_t         bufferType;
      uint16_t        bufferSize;
      uartFunction_pt UART_FunctionA_p;
      uartFunction_pt UART_FunctionB_p;
   }
   uartConfigObj_t;

   /** pointer-type to UART Config-Objekt */
   typedef uartConfigObj_t* uartConfigObj_pt;

#endif
