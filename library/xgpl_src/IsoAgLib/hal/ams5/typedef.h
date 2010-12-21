// ---------------------------------------------------------------------------------------------
/** \file      typedef.h
    \brief     header file for AMS5 (M32C/87)
               system dependent basic type definitions
    \version   1.02
    \date      09.09.2008
    \author    Stefan Klueh
    \author    copyright (c) 2007/2008 GESAS GmbH
*/
// ---------------------------------------------------------------------------------------------
/*   History: 
              10.07.2007 V1.00  - first release

              28.07.2008 V1.01  - structure "sysIrqPriority_t" changed -> ams_can.cpp V2.00

              09.09.2008 V1.02  - structure "ioExtNodeImage_t" added -> see also ams_periphery.cpp
                                - structure "sysRTC_t" changed -> "day of week" added
                                - structure "canMsg_t" and "canConfigObj_t" changed -> element renamed: "id" to "dwId"
                                - Global definitions: directive "#ifndef" added
*/

#ifndef _TYPEDEF_AMS5_H
   #define _TYPEDEF_AMS5_H

   #include <stdint.h>

   /* --- GLOBAL DEFINES CONSTANTS --- */
#ifndef USER_WATCHDOG
   #define USER_WATCHDOG           0      /* Define for User-Watchdog */
#endif
#ifndef DATA_CHANGED
   #define DATA_CHANGED            1      /* Returnvalue from config_wd */
#endif
#ifndef FALSE
   #define FALSE             0
#endif
#ifndef TRUE
   #define TRUE              1
#endif
#ifndef HIGH
   #define HIGH              1
#endif
#ifndef LOW
   #define LOW               0
#endif
#ifndef ON
   #define ON                1
#endif
#ifndef OFF
   #define OFF               0
#endif

    typedef struct
   {
      uint8_t ipTA3;
      uint8_t ipTBX;
      uint8_t ipTB5;
      uint8_t ipADC;
      uint8_t ipINT6;
      uint8_t ipINT7;
      uint8_t ipCAN0INT8;
      uint8_t ipCAN1;
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
      uint8_t wday;
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

   /** ZMDL/LW Node structure */
   typedef struct
   {
      uint8_t DigIn[3];
      uint8_t DigOut[3];
      uint8_t State;
   }
   ioExtNodeImage_t;
   

   typedef struct
   {
      uint32_t GlobalMask;   /**< Global mask register */
      uint8_t GM_IdType;
      uint32_t LocalMaskA;   /**< Local mask A register */
      uint8_t LMA_IdType;
      uint32_t LocalMaskB;   /**< Local mask B register */
      uint8_t LMB_IdType;
   }
   canMaskStruct_t;
   
   /**
      CAN bus error state and timestamps

      - uint16_t .statusReg  :  copy of CANi Status Register
         

      - uint16_t .lastFlags  : error flags at last error interrupt
                               (see also .lastError timestamp) 
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

      - Timestamps [ms] uint32_t:

       .lastError    :  last error interrupt                         
       .lastSuccRx   :  last succsessful reception                 
       .lastSuccTx   :  last succsessful transmission              
       .lastErrPas   :  last time placed in an error-passive state 
       .lastBusOff   :  last time placed in a bus-off state        
       .lastBit1Err  :  last time Bit1-Error detected                     
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
      uint32_t dwId;        /**< Identifier */
      uint8_t  idType;    /**< ID-Type: SID (11 Bit) = FALSE, 
                                        EID (29 Bit) = TRUE  */
      uint8_t  dlc;       /**< data length code (0..8) */
      uint8_t  data[8];   /**< data field */
      uint32_t timestamp; /**< [ms] */
   }
   canMsg_t;
   
   /** CAN message box structure at byte access */
   struct canSlotByte_s
   {
      uint8_t sidh;              /**< Standard-ID register [10:6] */
      uint8_t sidl;              /**< Standard-ID register [5:0] */
      uint8_t eidh;              /**< Extended-ID register [17:14] */
      uint8_t eidm;              /**< Extended-ID register [13:6] */
      uint8_t eidl;              /**< Extended-ID register [5:0] */
      uint8_t dlc;               /**< Data length code */
      uint8_t data[8];           /**< Data(8byte) */
      uint8_t tsh;               /**< Time stamp register(High) */
      uint8_t tsl;               /**< Time stamp register(Low) */
   };

   /** CAN message box structure at word access */
   struct canSlotWord_s
   {
      uint8_t sidl;              /**< Standard-ID register [5:0] */
      uint8_t sidh;              /**< Standard-ID register [10:6] */
      uint8_t eidm;              /**< Extended-ID register [13:6] */
      uint8_t eidh;              /**< Extended-ID register [17:14] */
      uint8_t dlc;               /**< Data length code */
      uint8_t eidl;              /**< Extended-ID register [5:0] */
      uint8_t data[8];           /**< Data(8byte) */
      uint8_t tsl;               /**< Time stamp register(Low) */
      uint8_t tsh;               /**< Time stamp register(High) */
   };

   /** CAN message box */
   typedef union
   {
      struct canSlotByte_s ba;
      struct canSlotWord_s wa;
      uint8_t              byte[16];
      uint16_t             word[8];
   }
   canSlotMBox_t;

   /** pointer type to CAN message with timestamp (ringbuffer element) */
   typedef canMsg_t*  canBuffer_pt;
  
   typedef void (*tIRQ_FUNCTION)(uint8_t,uint8_t,canSlotMBox_t volatile*,uint8_t);
   /** CAN Config-Objekt */
   typedef struct
   {
      uint8_t  objNr;       /**< number of CAN objekt */
      uint32_t dwId;          /**< Identifier */
      uint8_t  idType;      /**< ID-Type: SID (11 Bit) = FALSE,
                                          EID (29 Bit) = TRUE  */
      uint8_t  direction;   /**< receive (0) or transmit (1) */
      uint16_t bufferSize;  /**< size of buffer */
      bool     bufferLock;  /**< state after init.: lock (true) / unlock (false) */
      tIRQ_FUNCTION pfIrqFunction;
   }
   canConfigObj_t;

   /** pointer-type to CAN Config-Objekt */
   typedef canConfigObj_t* canConfigObj_pt;

   /** CAN message control register */
   typedef union
   {
      struct
      {
         uint8_t newdata:1;      /**< Successful reception flag */
         uint8_t invaldata:1;    /**< Under reception flag */
         uint8_t msglost:1;      /**< Overwrite flag */
         uint8_t remactive:1;    /**< Remote frame transmission/reception status flag */
         uint8_t rsplock:1;      /**< Transmission reception auto response lock mode select bit */
         uint8_t remote:1;       /**< Remote frame corresponding slot select bit */
         uint8_t recreq:1;       /**< Reception slot requiest bit */
         uint8_t dummy:1;        /**< Reserved bit */
      }
      receive;
      struct
      {
         uint8_t sentdata:1;     /**< Successful transmission flag */
         uint8_t trmactive:1;    /**< Under transmission flag */
         uint8_t dummy1:1;       /**< Reserved bit */
         uint8_t remactive:1;    /**< Remote frame transmission/reception status flag */
         uint8_t dummy2:1;       /**< Reserved bit */
         uint8_t remote:1;       /**< Remote frame corresponding slot select bit */
         uint8_t dummy3:1;       /**< Reserved bit */
         uint8_t trmreq:1;       /**< Transmission slot request bit */
      }
      transmit;
      uint8_t byte;
   }
   canMCTL_t;

   /** UART parameter list */
   typedef struct
   {
      uint8_t  baudrate;   /**< Baudrate  (see config.h) */
      uint8_t  data_mode;  /**< Data-Mode (see config.h) */
      uint8_t  stopbits;   /**< amount of stop bits (1,2) */
      bool     handshake;  /**< true -> use xon/xoff software handshake */
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

   /*@}*/

#endif