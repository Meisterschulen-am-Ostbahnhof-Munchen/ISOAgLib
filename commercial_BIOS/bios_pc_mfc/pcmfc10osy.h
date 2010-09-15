/*--------------------------------------------------------------------------.DA*
  Name:         def00typ.h       
  Description:  common defines and type definitions
  Version: 04.05.99 /Se                             Copyrights (c) 1998, STW
  --------------------------------------------------------------------------.DE*/
#ifndef __DEF00TYP_H
   #define __DEF00TYP_H

   /* --- GLOBAL DEFINES AND CONSTANTS --- */
   #define FALSE             0
   #define TRUE              1

   #define HIGH              1
   #define LOW               0

   #define ON                1
   #define OFF               0

   #define CODE              const  /*04.05.99*/

   /* --- GLOBAL TYPDEFINITIONS --- */
   typedef unsigned int      word;
   typedef unsigned char     byte;
   typedef unsigned long     dword;
   typedef unsigned char     boolean;

   typedef void (*pfFunction)(void); /* typdefinitions for indirect functioncall */

   typedef struct           /* Struct Time */
   {
      long  l1ms;
      word  w1us;
   } tTime;

   typedef union            /* Union byte, word Zugriff (2 Byte) */
   {
      byte  b[2];
      word  w;
   } tBw;

   typedef union            /* Union byte, word, dword Zugriff (4 Byte) */
   {
      volatile byte  b[4];
      volatile word  w[2];
      volatile dword dw;
   } tDw;



   typedef struct           /* Struct Identification for Software */
   {
      byte abProgname[8];
      byte abVersion[5];
      byte bTyp;
      int  iOption;
   } tIdent;

   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */

#endif

/*--------------------------------------------------------------------------.DA*
  Name:         can00def.h                    
  Description:  common CAN-Definitions
  Version:      24.04.2001 /Se /AL                  Copyrights (c) 2001, STW
  --------------------------------------------------------------------------.DE*/
#ifndef __CAN00DEF_H
   #define __CAN00DEF_H

   /* GLOBAL STRUCTURES AND UNIONS */
   /* Datastructure: internal CAN-Register (16 Byte) */
   typedef struct  
   {
      tBw tMessageCtrl;            /* Message-Control-Register */
      tDw tArbit;                  /* Identifier for Arbitration */
      tBw tCfg_D0;                 /*  */
      tDw tD1_D4;                  /*  */
      tDw tD5_D7;                  /*  */
   } tCanMsgReg;

   /* Functionpointer: reaction at CAN-Error */
   typedef word (*tCAN_ERR_FUNC)(byte,word);

   /* Functionpointer: Serviceroutine for CAN in order to receive and transmit data */
   typedef tCanMsgReg *(*tIRQ_FUNCTION)(byte,byte,tCanMsgReg *);

   /* Datastructure: for Transmit (14 Byte) */
   typedef struct  
   {
      dword dwId;                  /* Identifier */
      byte bXtd;                   /* :=FALSE  11bit-Identifier, :=TRUE  29bit-Identifier */
      byte bDlc;                   /* Datalengthcode */
      byte abData[8];              /* Databuffer */
   } tSend;

   /* Datastructure: for Receive (20 Byte) */
   typedef struct  
   {
      dword dwId;                  /* Identifier */
      byte bXtd;                   /* :=FALSE  11bit-Identifier, :=TRUE  29bit-Identifier */
      byte bDlc;                   /* Datalengthcode */
      byte abData[8];              /* Databuffer */
      tTime tReceiveTime;          /* Time of received data */
   } tReceive;

   /* Datastructure: Status of CAN (30 Byte) */
   typedef struct  
   {
      tCAN_ERR_FUNC pfwBusOff;     /* reaction at BusOff */
      tCAN_ERR_FUNC pfwErrorWarn;  /* reaction at ErrorWarn */
      tTime tLastErrorTime;        /* Time of last error */
      dword dwGlobMask;            /* extended Global Mask (29 Bit) */
      word wGlobMask;              /* Global Mask (11 Bit) */
      dword dwLastMsgObjMask;      /* Mask of Last-Message-Objekt */
      word wBitrate;               /* Bitrate CAN */
      word wCtrlStatusReg;         /* Status Control Register */
      word wBittimingReg;          /* Bittiming Register */
   } tCanBusStatus;

   /* Datastructure: Configuration of Messageobject (16 Byte) */
   typedef struct  
   {
      dword dwId;                  /* Identifier */
      byte bXtd;                   /* :=FALSE  11bit-Identifier, :=TRUE  29bit-Identifier */
      byte bMsgType;               /* Type of Messageobject (RX,TX,...) */
      word wNumberMsgs;            /* reserve Buffer for Messageobject */
      byte bTimeStamped;           /* :=TRUE  activate Timestamp */
      word wPause;                 /* Delaytime for transmit data (0...32000ms) */
      tIRQ_FUNCTION pfIrqFunction; /* :=NULL  internal reaction by send/receive data on CAN */
   } tCanObjConfig;

   /* Datastructure: Status of Messageobject (30 Byte) */
   typedef struct  
   {
      tTime tLastIrqTime;          /* Timerecording by send/receive data on CAN */
      dword dwId;                  /* Identifier */
      byte bXtd;                   /* :=FALSE  11bit-Identifier, :=TRUE  29bit-Identifier */
      word wPause;                 /* Delaytime for transmit data (0...32000ms) */
      byte bType;                  /* Typ of Messageobject */
      byte bUsage;                 /* Usage of Messageobject */
      byte bOverflow;              /* :=TRUE  Bufferoverflow of Mesageobject */
      tTime tOverflowTime;         /* Time of last Bufferoverflow */
      byte bSendRequest;           /* :=TRUE  Messageobject will be sent */
      word wNumberMsgs;            /* Anzahl der Nachrichten in Nachrichtenobjekt (S/E) */
      word wMsgCtrlReg;            /* Message Control Register */
   } tCanObjStatus;

   /* Datastructure: for Transmit with "Default Identifier" (10 Byte) */
   typedef struct  
   {
      byte bDlc;                   /* Datalengthcode */
      byte abData[8];              /* Databuffer */
   } tSendDefault;

   /* Datastructur: for function req_can_msg() */
   typedef struct   
   {
      dword   dwId;                /* Identifier */
      byte    bXtd;                /* :=FALSE  11bit-Identifier, :=TRUE  29bit-Identifier */
   } tRequestRtr;

   /* GLOBAL DEFINES AND CONSTANTS */
   /* Can Messagetypes */
   #define   RX                   0
   #define   RX_RTR               1
   #define   TX                   2
   #define   TX_RTR               3
   #define   DEFAULT_TX           4
   #define   DEFAULT_TX_RTR       5

#endif


/*--------------------------------------------------------------------------.DA*
  Name:           Can01167.h            
  Description:    Header for CAN Interface V2.0	 (only for internal use)
  Version:        24.04.2001 /Fi /Se                 Copyright (c) 2001, STW
  --------------------------------------------------------------------------.DE*/
#ifndef __CAN01167_H
   #define __CAN01167_H

   /* FUNCTIONCODE */
   #define _INIT_CAN                    1
   #define _CLOSE_CAN                   2
   #define _GET_CAN_BUS_STATUS          3
   #define _INIT_CAN_ERR_IRQ            4
   #define _CLEAR_CAN_BUSOFF            5
   #define _CONFIG_CAN_OBJ              6
   #define _CLOSE_CAN_OBJ               7
   #define _GET_CAN_OBJ_STATUS          8
   #define _CHG_CAN_OBJ_ID              9
   #define _CHG_CAN_OBJ_PAUSE           10
   #define _SEND_CAN_MSG_DEF_ID         11
   #define _SEND_CAN_MSG                12
   #define _GET_CAN_MSG                 13
   #define _REQ_CAN_MSG                 14
   #define _CLEAR_CAN_OBJ_BUF           15
   #define _GET_CAN_MSG_BUF_COUNT       16
   #define _LOCK_CAN_OBJ                17
   #define _UNLOCK_CAN_OBJ              18
   #define _CHG_CAN_BUSOFF_TIME         19
   #define _SET_CAN_BITTIME_REG         20

   /* GLOBAL DEFINES AND CONSTANTS */
   /* Can Messagetypes */
   #define NO_INIT                      0x00
   #define IN_USE                       0x11
   #define LOCKED                       0xEE

   //The calling Source
   #define S_ESX                        0
   #define S_BBA                        1

   //Busnumbers not for users (only used in System functioncall check_can_timer(..))
   #define BUSNO_ESX_CAN0               0
   #define BUSNO_ESX_CAN1               1
  
   /* GLOBAL TYPDEFINITIONS */
   // typedefinition for functioncall of a function 
   typedef byte (*pfbFunction)(void);

   //Definitions for Configuration of Modul can00167.c
   typedef struct  
   {
      dword       dwBaseAddressCAN;          //BaseAdress of CAN
      dword       dwAdrBittimingReg0;        //Offset of the Bittimingregister 0 to BaseaddressCAN
      dword       dwAdrBittimingReg1;        //Offset of the Bittimingregister 1 to BaseaddressCAN
      dword       dwAdrCanInterruptReg;      //Offset of the Interruptregister to BaseaddressCAN
      pfbFunction FPGetInterruptLevel;       //Functionpointer to the Function which delivers the Interruptlevel
      pfFunction  FPSetHighspeedCAN;         //Functionpointer to the Function which is setting the Highspeed of the CAN-No
      pfFunction  FPSetLowspeedCAN;          //Functionpointer to the Function which is setting the Lowspeed of the CAN-No
      pfFunction  FPInitInterrupt;           //Functionpointer to the Function which is initializing the Interrupt of the CAN-No
      pfFunction  FPSetInterrupt;            //Functionpointer to the Function which is setting the Interrupt of the CAN-No
   }tCfg; //Structur for Configuration 

   //Configuration of the Errorsettings
   typedef struct  
   {
      byte bLowestNo;                        //Lowest Busnumber
      byte bHighestNo;                       //Highest Busnumber
   }tBusNumberLimit; 

   typedef struct 
   {
      byte   bFuncNoOffset;                  //Offset for returned Functionnummer in case of Error
      byte   bArea;                          //Area in case of Error
   }tErr;                                    //Structure for serving the Errorhandler

   typedef struct  
   {
      byte  bDetail;                         //Detail in case of Error
      byte  bModul;                          //Modul in case of Error
      byte  bSource;                         //In Busoffroutine the source is determind from Busnumber
   }tErr2;                                   //Structur for serving the Errorhandler


   /* GLOBAL VARIABLES */
   /* GLOBAL PROTOTYPES */
   extern int _init_can(byte bBusNumber,word wMaskShort,dword dwMaskLong,dword dwLastMsgObjMask,word wBitrate,byte bSource);
   extern int _close_can(byte bBusNumber,byte bSource);
   extern int _get_can_obj_status(byte bBusNumber,byte bMsgObj,tCanObjStatus *ptStatus, byte bSource);
   extern int _get_can_bus_status(byte bBusNumber,tCanBusStatus *ptStatus, byte bSource);
   extern int _init_can_err_irq(byte bBusNumber,tCAN_ERR_FUNC pfwBusOff,tCAN_ERR_FUNC pfwErrorWarn,byte bSource);
   extern int _clear_can_busoff(byte bBusNumber, byte bSource );
   extern int _config_can_obj(byte bBusNumber,byte bMsgObj,tCanObjConfig *ptConfig, byte bSource );
   extern int _close_can_obj(byte bBusNumber,byte bMsgObj, byte bSource);
   extern int _chg_can_obj_id(byte bBusNumber,byte bMsgObj,dword dwId,byte bXtd, byte bSource);
   extern int _chg_can_obj_pause(byte bBusNumber,byte bMsgObj,word wPause, byte bSource);
   extern int _clear_can_obj_buf(byte bBusNumber,byte bMsgObj, byte bSource);
   extern int _get_can_msg_buf_count(byte bBusNumber,byte bMsgObj, byte bSource);
   extern int _lock_can_obj(byte bBusNumber,byte bMsgObj, byte bSource);
   extern int _unlock_can_obj(byte bBusNumber,byte bMsgObj, byte bSource);
   extern int _send_can_msg_def_id(byte bBusNumber,byte bMsgObj,tSendDefault *ptSend,byte bSource);
   extern int _send_can_msg(byte bBusNumber,byte bMsgObj,tSend *ptSend,byte bSource);
   extern int _req_can_msg(byte bBusNumber,byte bMsgObj,tRequestRtr *ptRtr,byte bSource);
   extern int _get_can_msg(byte bBusNumber,byte bMsgObj,tReceive *ptReceive,byte bSource);
   extern int _chg_can_busoff_time(byte bBusNumber,word wT1,word wT2,word wT3,word wT4,word wT5,word wTwait,byte bSource);
   extern int _set_can_bittime_reg(byte bBusNumber,word wBittimeReg,word wBitrate,byte bSource);

#endif


/*--------------------------------------------------------------------------
  Name:    c2c10ser.h                Description: Header for RS232 Interface
  Version: 23.08.99 /JR /Se                          Copyright (c) 1998, STW
  --------------------------------------------------------------------------*/
#ifndef __C2C10SER_H
   #define __C2C10SER_H

   /* --- GLOBAL DEFINES AND CONSTANTS --- */
   // Mode of RS232
   #define DATA_7_BITS_EVENPARITY      1
   #define DATA_8_BITS_EVENPARITY      2
   #define DATA_7_BITS_ODDPARITY       3
   #define DATA_8_BITS_ODDPARITY       4
   #define DATA_8_BITS_NOPARITY        5

   // Errorcode of function get_rs232_error()
   #define OVERRUN_ERROR               0x04
   #define FRAME_ERROR                 0x02
   #define PARITY_ERROR                0x01
   #define HARDWARE                    2

   #define RS232_1                     0
   #define RS232_2                     1
   #define RS232_3                     2
   #define RS232_4                     3
   #define RS232_5                     4


   /* --- GLOBAL TYPDEFINITIONS --- */
   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */
   extern int init_rs232(byte bSINumber, dword dwBaudrate,byte bMode,byte bStoppbits,byte bSoftwarehandshake);
   extern int get_rs232_rx_buf_count(byte bSINumber);
   extern int get_rs232_tx_buf_count(byte bSINumber);
   extern int set_rs232_baudrate(byte bSINumber, dword dwBaudrate);
   extern int get_rs232_char(byte bSINumber, byte *pbRead);
   extern int get_rs232_char_echo(byte bSINumber, byte *pbRead);
   extern int put_rs232_char(byte bSINumber,byte bByte);
   extern int put_rs232_n_char(byte bSINumber,byte *bpWrite,word wNumber);
   extern int put_rs232_string(byte bSINumber,byte *pbString);
   extern int get_rs232_string(byte bSINumber,byte *pbRead,byte bLast_char);
   extern int get_rs232_string_echo(byte bSINumber,byte *pbRead,byte bLast_char);
   extern int clear_rs232_rx_buffer(byte bSINumber);
   extern int clear_rs232_tx_buffer(byte bSINumber);
   extern int config_rs232_rx_obj(byte bSINumber, word wBuffersize,void (*pFunction)(byte *bByte));
   extern int config_rs232_tx_obj(byte bSINumber, word wBuffersize,void (*funktion_after_transmit)(byte *bByte),
                                  void (*funktion_befor_transmit)(byte *bByte));
   extern int get_rs232_error(byte bSINumber,byte *Errorcode);

   /* --- FUNCTIONCODE --- */
   #define GET_RS232_STRING            1
   #define INIT_RS232                  2
   #define SET_RS232_BAUDRATE          3
   #define GET_RS232_CHAR              4
   #define GET_RS232_CHAR_ECHO         5
   #define PUT_RS232_CHAR              6
   #define PUT_RS232_N_CHAR            7
   #define PUT_RS232_STRING            8
   #define CONFIG_RS232_RX_OBJ         9
   #define CONFIG_RS232_TX_OBJ         10
   #define GET_RS232_ERROR             11
   #define GET_RS232_STRING_ECHO       12
   #define GET_RS232_RX_BUF_COUNT      13
   #define GET_RS232_TX_BUF_COUNT      14
   #define CLEAR_RS232_RX_BUFFER       15
   #define CLEAR_RS232_TX_BUFFER       16
#endif
/*------------------------------------------------------------------------------------
  Name:    c2c10btr.h      Description: Utilities for task handling
  Version:   17.11.98 /BA             Copyrights (c) 1998, STW
  -----------------------------------------------------------------------------------*/
#ifndef __C2C10BTR_H  
   #define __C2C10BTR_H

    /*--- GLOBAL DEFINES CONSTANTS ---*/
   #define TASKLEVEL_4D                0x43
   #define TASKLEVEL_4C                0x42
   #define TASKLEVEL_4B                0x41
   #define TASKLEVEL_4A                0x40
   #define TASKLEVEL_3D                0x33
   #define TASKLEVEL_3C                0x32
   #define TASKLEVEL_3B                0x31
   #define TASKLEVEL_3A                0x30
   #define TASKLEVEL_2D                0x23
   #define TASKLEVEL_2C                0x22
   #define TASKLEVEL_2B                0x21
   #define TASKLEVEL_2A                0x20
   #define TASKLEVEL_1D                0x13
   #define TASKLEVEL_1C                0x12
   #define TASKLEVEL_1B                0x11
   #define TASKLEVEL_1A                0x10
   #define HIGHER_THAN_USERTASK_LEVEL  13

   #define EVENTS                      word  //for 15 Events
   #define NO_EVENTS                   0x0


    /*--- GLOBAL TYPDEFINITIONS ---*/
   
   typedef struct {
                  unsigned TIMER       : 1;
                  unsigned EVENT1      : 1;
                  unsigned EVENT2      : 1;
                  unsigned EVENT3      : 1;
                  unsigned EVENT4      : 1;
                  unsigned EVENT5      : 1;
                  unsigned EVENT6      : 1;
                  unsigned EVENT7      : 1;
                  unsigned EVENT8      : 1;
                  unsigned EVENT9      : 1;
                  unsigned EVENT10     : 1;
                  unsigned EVENT11     : 1;
                  unsigned EVENT12     : 1;
                  unsigned EVENT13     : 1;
                  unsigned EVENT14     : 1;
                  unsigned EVENT15     : 1;
                  } T_EventBit;

   typedef union  {
                  T_EventBit  tEventBit;
                  EVENTS      tEvents;
                  } TU_Events;

   /*--- GLOBAL VARIABLES ---*/

   /*--- PROTOTYPES ---*/
   extern int  init_task_call(word wPriority, word wIntervall, word wOffset, void(*pfFunktion)(void));
   extern void start_task_timer(word wT5Reload);
   extern int  get_task_overload(void);
   extern int  set_event_mask(TU_Events tNewEventMask);
   extern int  set_events(TU_Events tNewEvents);
   extern int  get_events(TU_Events * ptActualEvents);
   extern int  get_task_running(word * pwActualTask);
   extern int  retrigger_me(word wIntervall, word wOffset);

   /*--- FUNCTION CODE ---*/
   #define INIT_TASK_CALL        1
   #define GET_TASK_OVERLOAD     2
   #define SET_EVENT_MASK        3
   #define SET_EVENTS            4
   #define GET_EVENTS            5
   #define GET_TASK_RUNNING      6
   #define RETRIGGER_ME          7

#endif

/*--------------------------------------------------------------------------
  Name:    c2c10eep.h        Description: Functions for eeprom communication
  Version: 07.02.02 /RF                             Copyrights (c) 1998, STW
  --------------------------------------------------------------------------*/
#ifndef __C2C10EEP_H  
   #define __C2C10EEP_H

   /*--- GLOBAL DEFINES CONSTANTS ---*/
   #define EE_READY                    0
   #define EE_NOT_READY                1

   /*--- GLOBAL TYPDEFINITIONS ---*/
   /*--- GLOBAL VARIABLES ---*/
   /*--- PROTOTYPES ---*/
   extern int get_eeprom_size(void);                                 /* get the size of the eeprom */
   extern int get_eeprom_segment_size(void);                         /* get the segment size of the eeprom for page write access*/
   extern int eeprom_write(word wAddress,word wNumber,byte *pbData); /* write one or more bytes into the eeprom*/
   extern int eeprom_write_byte(word wAddress,byte bByte);           /* write one byte into the eeprom */
   extern int eeprom_read(word wAddress,word wNumber,byte *pbByte);  /* read one or more byte from the eeprom*/

   /*--- FUNCTION CODE ---*/
   #define EEPROM_READ                 1
   #define EEPROM_WRITE                2
   #define EEPROM_WRITE_BYTE           3

#endif
/*--------------------------------------------------------------------------.DA*
  Name:           c2c10inp.h      
  Description:    Functions to evaluate the C2C input signals
  Version: 08.02.02 /RF                             Copyrights (c) 2002, STW
  --------------------------------------------------------------------------.DE*/
#ifndef __C2C10INP_H
   #define __C2C10INP_H

   /* --- GLOBAL DEFINES AND CONSTANTS --- */
   /* Defines fuer die Digitaleingaenge */
   #define DIN1                   0
   #define DIN2                   1

   #define MB_L1                  0
   #define MB_L2                  1
   #define MB_H1                  2
   #define MB_H2                  3

   #define DIGIN                  0   /* = 0; digital inputg*/
   #define RISING_EDGE            1   /* = 1; interrupt on rising edge*/
   #define FALLING_EDGE           2   /* = 2; interrupt on falling edge*/
   #define BOTH_EDGE              3   /* = 3; interrupt on both edges*/


   #define VIRTUAL_VALUE          1   /* = 1; virtual value */
   #define REST_PULSE             2   /* = 2; */
   #define REST_PULSE_VIRTUAL     3   /* = 3; */

   #define RPM_PRESCALER_8        0   /* = 0; CPU/8   */
   #define RPM_PRESCALER_16       1   /* = 1; CPU/16  */
   #define RPM_PRESCALER_32       2   /* = 2; CPU/32  */
   #define RPM_PRESCALER_64       3   /* = 3; CPU/64  */
   #define RPM_PRESCALER_128      4   /* = 4; CPU/128 */
   #define RPM_PRESCALER_256      5   /* = 5; CPU/256 */
   #define RPM_PRESCALER_512      6   /* = 6; CPU/512 (default) */
   #define RPM_PRESCALER_1024     7   /* = 7; CPU/1024*/

   /* --- GLOBAL TYPDEFINITIONS --- */
   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */
   extern int  init_digin(byte bMode,void (*pfFunction_name)());
   extern int  get_digin_hilo(void);
   extern int  set_digin_prescaler(byte bMode);
   extern int  get_digin_hilo_static(void);
   extern int  get_digin_period(word *pwPeriod, word *pwImpulse);
   extern int  get_digin_pwm_time(word *pwHigh_time,word *pwLow_time,word *pwDiff_time);
   extern int  get_digin_freq(word *pwFrequency);
   extern int  config_digin_freq(word wWindow,byte bCount);

   /* --- FUNCTION CODE --- */
   #define INIT_DIGIN                  1
   #define GET_DIGIN_HILO              2
   #define SET_DIGIN_PRESCALER         3
   #define GET_DIGIN_HILO_STATIC       4
   #define GET_DIGIN_PERIOD            5
   #define GET_DIGIN_PWM_TIME          6
   #define GET_DIGIN_FREQ              7
   #define CONFIG_DIGIN_FREQ           8

#endif
/*--------------------------------------------------------------------------
  Name:    c2c10out.h                   Description: Outputfunctions for C2C-C2C
  Version: 11.02.02 /RF                         Copyrights (c) 2002, STW
  --------------------------------------------------------------------------*/
#ifndef __C2C10OUT_H
   #define __C2C10OUT_H
                            
   /* --- GLOBAL TYPDEFINITIONS --- */
   #define OUT1                        0
   #define OUT2                        1
   #define OUT3                        2
   #define OUT4                        3
   #define LED1                        4
   #define LED2                        5
   #define LED3                        6
   #define LED4                        7
   #define LED5                        8
   #define LED6                        9

   /* PWM Output groups */
   #define PWM_OUT1                    0       /* output 0 */
   #define PWM_OUT2                    1       /* output 1 */

   typedef struct
   {
      word wOutputValue;                       /* aktual output value */
      word wMaxOutput;                         /* highest pwm puls width value */
      dword dwOutputFreq;                      /* pwm frequence */
      tTime TSetTime;                          /* time of output setting */
   }  tOutput;

   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPEN --- */
   extern int  set_digout(byte bOutputNo, word wPWM_Value);             /* set pwm value */
   extern int  get_digout_status(byte bOutputNo, tOutput *ptStatus);    /* get the status of the chosen output */
   extern int  set_pwm_freq(byte bOutputGroup, dword dwFrequency_mHz);  /* define the frequency of the pwm signal */
   extern int  set_led(byte bNumber, boolean bitState);                 /* set the LED indication on or off */
   extern int  get_status_BTS(void);                                    /* get the BTS 0 and 1 status */
   extern void set_gsm_supply(boolean bitState);                        /* set the supplyvoltage for the GSM on or off */
   extern void set_gsm_ignition(boolean bitState);                      /* set the ignition pin of the GSM on or off */   
   extern int  set_digout_1000 (byte bOutputNo,word wPWM_Value);        /* set the PWM value from 0..100,0 % */

   /* --- FUNCTION CODE --- */
   #define SET_DIGOUT                  1
   #define GET_DIGOUT_STATUS           2
   #define SET_PWM_FREQ                3
   #define SET_LED                     4
   #define SET_DIGOUT_1000             5
#endif


/*--------------------------------------------------------------------------.DA*
  Name:           c2c10tim.h              
  Description:    Functions for software timer
  Version: 13.08.99 /BA /Se                         Copyrights (c) 1998, STW
  --------------------------------------------------------------------------.DE*/
#ifndef __C2C10TIM_H
    #define __C2C10TIM_H

   /* --- GLOBAL DEFINES CONSTANTS --- */
   /* --- GLOBAL TYPDEFINITIONS --- */
   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */
   extern int  get_timer(void);                             /* get a software timer from the system*/
   extern int  free_timer(int iTimerHandle);                /* give the software timer back to the system*/
   extern int  start_timer(int iTimerHandle, long lTime);   /* start software timer */
   extern int  stop_timer(int iTimerHandle);                /* stop software timer*/
   extern int  timeout(int iTimerHandle);                   /* service the overflow situation of the software timer*/
   extern long rest_time(int iTimerHandle, long *lPastTime);/* the time to the overflow*/

   /* --- FUNCTION CODE --- */
   #define GET_TIMER                       1
   #define FREE_TIMER                      2
   #define START_TIMER                     3
   #define STOP_TIMER                      4
   #define TIMEOUT                         5
   #define REST_TIME                       6

#endif


/*--------------------------------------------------------------------------
  Name:    c2c10sys.h                   Description: Systemfunctions for C2C
  Version: 19.01.99 /BA /Se                               Copyrights (c) STW
  --------------------------------------------------------------------------*/
#ifndef __C2C10SYS_H
   #define __C2C10SYS_H

   /* --- GLOBAL DEFINES CONSTANTS --- */
   #define WD_OFF                     0
   #define WD_TIME_100_MS             1
   #define WD_TIME_450_MS             2
   #define WD_TIME_1000_MS            3
        
   /* --- GLOBAL TYPDEFINITIONS --- */

   typedef struct
   {
      byte bHardware;               // Hardware
      byte bCPUType;
      byte bCPU_freq;
      word wRAMSize;
      word wROMSize;
      word wDataFlashSize;          // Size of datenflash
      byte bRTCType;                // RTC typ
      byte bRTCSize;                // Size of RAM from the RTC
      byte bUARTType;
      byte bRS232Iso;               // RS232 optocouppled
      byte baGSMType[8];            // Name of the modem
      byte bGSMAccess;              // Access permission
      byte baSIMNumber[20];         // Simcardnumber
      byte baBabyBoard[8];          // For babyboard
      byte bEESize;
      byte bEEWriteSegmentSize;
   }  tSystem;

   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */
   extern void trigger_wd(void);                                     /* watchdog triggering */
   extern int  reset_wd(void);                                       /* to force a reset */
   extern int  open_c2c(tSystem *tSystem);                           /* start C2C background system */
   extern void get_clock(word *pwTimer500_us, word *pwTimer200_ns);  /* evaluation of a hardware timer */
   extern void get_time_ext(tTime *pTTime);                          /*    "  */
   extern long get_time(void);                                       /*    "  */
   extern void delay_us(word wTimer);                                /* delay micro secounds */
   extern int  get_cpu_freq(void);                                   /* get the cpu frequency */
   extern void get_system_data(tSystem *tSystem);                    /* get system configuration */
   extern int  config_wd(byte bTime);                                /* config the extern WD */
   extern void staying_alive(void);                                  /* to activate the power selfholding */
   extern int  power_down(void);                                     /* to deactivate the power selfholding */
   extern int  get_on_off_switch(void);                              /* the evaluation of the on/off-switch (D+)*/
   extern void init_off_reaction(void (*pfFunction_name)());         /* install a function to be invoked by on-off-interrupt*/
   extern int  get_boardtemperature(void);                           /* return the boardtemperature */
   extern void init_gsm_alive_reaction(void (*pfFunction_name)());   /* install a function to be invoked by an WD impulse from the GSM module */
//   #define SCAN_CAN_ENABLE
   #ifdef SCAN_CAN_ENABLE
      extern int  scan_can_bitrate (byte bBusNumber, word wScanningTime, byte bitTriggerWd);   /* scan the bitrate of the CAN bus. There must be signals */
   #endif
   // wegen Kompatibilität
   #define wd_trigger trigger_wd
   
   /*--- FUNCTION CODE ---*/
   #define CONFIG_WD                   1
   #define RESET_WD                    2
   #define OPEN_C2C                    3
   #define POWER_DOWN                  4
   #ifdef SCAN_CAN_ENABLE
      #define SCAN_CAN_BITRATE         5
   #endif

#endif


/*------------------------------------------------------------------------------------
  Name:    c2c10scu.h      Description: Utilities for the security controll unit
  Version:   17.11.98 /BA             Copyrights (c) 1998, STW
  -----------------------------------------------------------------------------------*/
#ifndef __C2C10SCU_H
    #define __C2C10SCU_H
   /*--- GLOBAL DEFINES AND CONSTANTS ---*/
   /*--- GLOBAL TYPDEFINITIONS ---*/
   /*--- GLOBAL VARIABLES ---*/

   /*--- PROTOTYPES ---*/
   extern int eeprom_read_crc(word wAddress, word wNumber, byte *pbData);
   extern int eeprom_write_crc(word wAddress, word wNumber, byte *pbData);
   extern int calc_crc(word wNumber, byte *pbData, word *pbCRC);
   extern int rtcram_write_crc(word wAddress, word wNumber, byte *pbData);
   extern int rtcram_read_crc(word wAddress, word wNumber, byte *pbData);

   /*--- FUNCTION CODE ---*/
   #define EEPROM_READ_CRC    1  
   #define EEPROM_WRITE_CRC   2
   #define CALC_CRC           3
   #define RTCRAM_WRITE_CRC   4
   #define RTCRAM_READ_CRC    5

#endif
/*--------------------------------------------------------------------------.DA*
  Name:           c2c10fls.h 
  Description:    Utilities for flashprogramming parameters
  Version: 19.01.99 /BA /Se                               Copyrights (c) STW
  --------------------------------------------------------------------------.DE*/
#ifndef __C2C10FLS_H
   #define __C2C10FLS_H

   /* --- GLOBAL DEFINES CONSTANTS --- */
   /* --- GLOBAL TYPDEFINITIONS --- */
   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */
   extern int get_lokal_id(byte * bLokalId);                /* get C2C ID to start flash programming */
   extern int get_snr(byte *snr_dat);                       /* serial number of C2C */
   extern int get_cantype(byte *bCantype);                  /* get the defined can type standard or extended ID */
   extern int get_comp_id(byte *pbComp);                    /* get the defined company id to start flash programming */
   extern int get_bitrate_can(word *pwBitrate);             /* get the defined can bitrate */
   extern int get_can_id(dword *pdwCANId);                  /* get can id to start flash programming */
   extern int set_lokal_id(byte bLocal_id);                 /* define a new C2C ID */
   extern int set_bitrate_can(word wCan_bitrate);           /* define a new can bitrate */
   extern int set_cantype(byte bCantype);                   /* define can type (standard/extended) */
   extern int set_can_id(dword dwId_dat);                   /* define a new can id for flash programming */
   extern int get_sec_compare_mode(dword *dwCompareMode);
   extern int set_sec_compare_mode(dword dwCompareMode);    /* set the startup check of the flash */

   /* --- FUNCTION CODE --- */
   #define GET_LOKAL_ID               1
   #define GET_SNR                    2
   #define SET_LOKAL_ID               3
   #define SET_BITRATE_CAN            4
   #define SET_CANTYPE                5
   #define SET_CAN_ID                 6
   #define GET_CANTYPE                7
   #define GET_COMP_ID                8
   #define GET_BITRATE_CAN            9
   #define GET_CAN_ID                 10
   #define GET_SEC_COMPARE_MODE       11
   #define SET_SEC_COMPARE_MODE       12
#endif
   /*--------------------------------------------------------------------------
  Name:    c2c10can.h         Description:  Can-Header for Babyboard Typ "A"
  Version: 19.07.2000 /Fi/Se/AL                      Copyright (c) 1998, STW
  --------------------------------------------------------------------------*/
#ifndef __C2C10CAN_H
   #define __C2C10CAN_H

   /* --- GLOBAL TYPDEFINITIONS --- */
   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPS --- */
   extern int init_can(byte bBusNumber,word wMaskShort,dword dwMaskLong,dword dwLastMsgObjMask, word wBitrate);
   extern int close_can(byte bBusNumber);
   extern int get_can_obj_status(byte bBusNumber,byte bMsgObj,tCanObjStatus *ptStatus);
   extern int get_can_bus_status(byte bBusNumber,tCanBusStatus *ptStatus);
   extern int init_can_err_irq(byte bBusNumber,tCAN_ERR_FUNC pfwBusOff,tCAN_ERR_FUNC pfwErrorWarn);
   extern int clear_can_busoff(byte bBusNumber);
   extern int config_can_obj(byte bBusNumber,byte bMsgObj,tCanObjConfig *ptConfig);
   extern int close_can_obj(byte bBusNumber,byte bMsgObj);
   extern int chg_can_obj_id(byte bBusNumber,byte bMsgObj,dword dwId,byte bXtd);
   extern int chg_can_obj_pause(byte bBusNumber,byte bMsgObj,word wPause);
   extern int clear_can_obj_buf(byte bBusNumber,byte bMsgObj);
   extern int get_can_msg_buf_count(byte bBusNumber,byte bMsgObj);
   extern int lock_can_obj(byte bBusNumber,byte bMsgObj);
   extern int unlock_can_obj(byte bBusNumber,byte bMsgObj);
   extern int send_can_msg_def_id(byte bBusNumber,byte bMsgObj,tSendDefault *ptSend);
   extern int send_can_msg(byte bBusNumber,byte bMsgObj,tSend *ptSend);
   extern int req_can_msg(byte bBusNumber,byte bMsgObj,tRequestRtr *ptRtr);
   extern int get_can_msg(byte bBusNumber,byte bMsgObj,tReceive *ptReceive);
   extern int chg_can_busoff_time(byte bBusNumber,word wT1,word wT2,word wT3,word wT4,word wT5,word wTwait);
   extern int set_can_bittime_reg(byte bBusNumber,word wBittimeReg,word wBitrate);

   /* --- FUNCTIONCODE --- */

   #define INIT_CAN                       _INIT_CAN
   #define CLOSE_CAN                      _CLOSE_CAN
   #define GET_CAN_BUS_STATUS             _GET_CAN_BUS_STATUS
   #define INIT_CAN_ERR_IRQ               _INIT_CAN_ERR_IRQ
   #define CLEAR_CAN_BUSOFF               _CLEAR_CAN_BUSOFF
   #define CONFIG_CAN_OBJ                 _CONFIG_CAN_OBJ
   #define CLOSE_CAN_OBJ                  _CLOSE_CAN_OBJ
   #define GET_CAN_OBJ_STATUS             _GET_CAN_OBJ_STATUS
   #define CHG_CAN_OBJ_ID                 _CHG_CAN_OBJ_ID
   #define CHG_CAN_OBJ_PAUSE              _CHG_CAN_OBJ_PAUSE
   #define SEND_CAN_MSG_DEF_ID            _SEND_CAN_MSG_DEF_ID
   #define SEND_CAN_MSG                   _SEND_CAN_MSG
   #define GET_CAN_MSG                    _GET_CAN_MSG
   #define REQ_CAN_MSG                    _REQ_CAN_MSG
   #define CLEAR_CAN_OBJ_BUF              _CLEAR_CAN_OBJ_BUF
   #define GET_CAN_MSG_BUF_COUNT          _GET_CAN_MSG_BUF_COUNT
   #define LOCK_CAN_OBJ                   _LOCK_CAN_OBJ
   #define UNLOCK_CAN_OBJ                 _UNLOCK_CAN_OBJ
   #define CHG_CAN_BUSOFF_TIME            _CHG_CAN_BUSOFF_TIME
   #define SET_CAN_BITTIME_REG            _SET_CAN_BITTIME_REG

#endif
/*--------------------------------------------------------------------------.DA*
  Name:         c2c10ana.h                 
  Description:  Utilities for analog unit
  Version: 15.02.99 /BA /Se /HH                    Copyrights (c) 1998, STW
  --------------------------------------------------------------------------.DE*/
#ifndef __C2C10ANA_H
   #define __C2C10ANA_H

   /*--- GLOBAL DEFINES CONSTANTS ---*/
   #define GET_TEMP        0  /* Temperatur */
   #define GET_OUT_1       1  /* Outputvoltage 1 */
   #define GET_OUT_2       2  /* Outputvoltage 2 */
   #define GET_U_E         3  /* Inputvoltage after filter */
   #define GET_U_GSM       4  /* GSM supply voltage, 5V */
   #define GET_U_CPU       5  /* Electronic supply voltage */
   #define GET_U_10_V      6  /* Internal 10V */
   #define GET_A_IN_1      7  /* Inputvoltge of port 1 */
   #define GET_A_IN_2      8  /* Inputvoltge of port 1 */
   #define GET_A_IN_3      9  /* Inputvoltge of port 1 */
   #define GET_A_IN_4      10 /* Inputvoltge of port 1 */
   #define GET_U_SIM       11 /* SIM - card voltage -> 2,7V -> close*/

   #define AIN1            0  /* Pulldown for input 1 */ 
   #define AIN2            1  /* Pulldown for input 2 */ 
   #define AIN3            2  /* Pulldown for input 3 */ 
   #define AIN4            3  /* Pulldown for input 4 */ 


   /* --- GLOBAL TYPDEFINITIONS --- */
   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */
   extern int init_analogin(byte bNumber, boolean qPullDownResistor); /* set the pulldown resistor of the analog input port */
   extern int get_adc(byte bChannelNumber);                           /* evaluation of analog channels */
   extern int get_adc_mean(byte bChannelNumber);                      /* evaluation of the mean value of an analog input */

   /* --- FUNCTION CODE --- */
   #define INIT_ANALOGIN   1
   #define GET_ADC         2
   #define GET_ADC_MEAN    3

#endif


/*--------------------------------------------------------------------------
  Name:    c2c10rtc.h        Description: Functions for rtc communication
  Version: 01.02.00 /GM                             Copyrights (c) 2000, STW
  --------------------------------------------------------------------------*/
#ifndef __C2C10RTC_H  
   #define __C2C10RTC_H
/* --- GLOBAL TYPDEFINITIONS --- */
   typedef struct
   {
      byte bSeconds;                   // 0..59
      byte bMinutes;                   // 0..59
      byte bHours;                     // 0..23  24-hour format
      byte bDayofWeek;                 // 1..7   1=Sunday
      byte bDayofMonth;                // 1..31
      byte bMonth;                     // 1..12
      byte bYear;                      // 0..99
      byte bTimezone;                  // for future use
   } tRTCDate;
   
   typedef struct
   {
      byte bMinutes;                   // 0..59
      byte bHours;                     // 0..23
      byte bDays;                      // 0..28
   } tRTCDifTime;

   /*--- GLOBAL DEFINES CONSTANTS ---*/
   #define RTC_READY                   0
   #define RTC_NOT_READY               1
   
   /*--- GLOBAL TYPDEFINITIONS ---*/
   /*--- GLOBAL VARIABLES ---*/
   /*--- PROTOTYPES ---*/
   extern int init_rtc (word *pwStatus, boolean *bitWakeupStatus);         /* init the RTC and check it */
   extern void get_rtcram_size(byte *);                                    /* get the size of the rtcrom */
   extern boolean rtc_ready(void);                                         /* get the status of rtcrom */
   extern int rtcram_write(word wAddress,word wNumber,byte *pbData);       /* write one or more bytes into the rtcrom */
   extern int rtcram_write_byte(word wAddress,byte bByte);                 /* write one byte into the rtcrom */
   extern int rtcram_read(word wAddress,word wNumber,byte *pbByte);        /* read one or more byte from the rtcram */
   extern int get_rtc_time(tRTCDate *ptDate);                              /* get the time */
   extern int set_rtc_time(tRTCDate tDate);                                /* set the time */
   extern int get_rtc_serial_number (byte *pbByte);                        /* read the serial number, lenght 6 bytes */
   extern int get_c2c_wake_up_time (tRTCDate *ptDate);                     /* get the switch on time */
   extern int set_c2c_wake_up_time (tRTCDate tDate, boolean bitState);     /* set the switch on time */
   extern int set_c2c_sleep_time(tRTCDifTime *tDifTime);                   /* set the timedifference for wake up */
   extern int set_c2c_wake_up(boolean bitState);                           /* enable or disable the wake up funktion*/
   extern int get_c2c_wake_up(boolean *bitState);                          /* return the state of the wake up funktion */
   extern int set_rtc_mez(boolean bitState);                               /* enable or disable the automatic summer/winter time*/
   extern int get_rtc_mez(boolean *bitState);                              /* check the automatic summer/winter time*/
   extern int get_rtc_supply_on(boolean *bitState);                        /* check if the RTC turn the supply voltage on */


   
   /*--- FUNCTION CODE ---*/
   #define INIT_RTC                    1
   #define RTCRAM_WRITE                2
   #define RTCRAM_WRITE_BYTE           3
   #define RTCRAM_READ                 4
   #define GET_RTC_TIME                5
   #define SET_RTC_TIME                6
   #define GET_RTC_SERIAL_NUMBER       7
   #define GET_C2C_WAKE_UP_TIME        8
   #define SET_C2C_WAKE_UP_TIME        9
   #define SET_C2C_SLEEP_TIME          10
   #define SET_C2C_WAKE_UP             11
   #define GET_C2C_WAKE_UP             12
   #define SET_RTC_MEZ                 13
   #define GET_RTC_MEZ                 14
   #define GET_RTC_SUPPLY_ON           15


   // Status der Echtzeituhr
   #define RTC_STATUS_BATTERY          0x01
   #define RTC_STATUS_CONFIG           0x02
   #define RTC_STATUS_QUARZ            0x04
         
#endif
/*--------------------------------------------------------------------------
  Name:    c2c10mem.h    Description: Header for flash programming interface
  Version: 18.02.99 /BA                           Copyright (c) 1998, STW
  --------------------------------------------------------------------------*/
#ifndef __C2C10MEM_H
   #define __C2C10MEM_H

   /* --- GLOBAL DEFINES AND CONSTANTS --- */
   /* Manufactorcode ,Devicecode, Sectornumber and size of the different devices */
   #define MAN_CODE_ST                       0x20     // SGS 29F400 / 29F800
   #define MAN_CODE_AD                       0x01     // AD 29F400 / 29F800
   #define DEV_CODE_ST_BOT_400               0xD6     // SGS 29F400BB
   #define DEV_CODE_ST_BOT_800               0x58     // SGS 29F800AB
   #define DEV_SEC_ST_400                    11       // Sectornumber for 29F400
   #define DEV_SEC_ST_800                    19       // Sectornumber for 29F800
   #define SGS_LAST_ADRESS_ST_400            0x7FFFFUL
   #define SGS_LAST_ADRESS_ST_800            0xFFFFFUL
   
   #define FLASH_READY                       0
   #define FLASH_NOT_READY                   1

   /* Startadressen der einzelnen Flashbloecke */
   #define SGS_BOOT_BLK_START_ADR            0x0
   #define SGS_PARAMETER_BLK1_START_ADR      0x04000
   #define SGS_PARAMETER_BLK2_START_ADR      0x06000
   #define SGS_MAIN_BLK1_START_ADR           0x08000
   #define SGS_MAIN_BLK2_START_ADR           0x10000
   #define SGS_MAIN_BLK3_START_ADR           0x20000
   #define SGS_MAIN_BLK4_START_ADR           0x30000
   #define SGS_MAIN_BLK5_START_ADR           0x40000
   #define SGS_MAIN_BLK6_START_ADR           0x50000
   #define SGS_MAIN_BLK7_START_ADR           0x60000
   #define SGS_MAIN_BLK8_START_ADR           0x70000
   #define SGS_MAIN_BLK9_START_ADR           0x80000
   #define SGS_MAIN_BLK10_START_ADR          0x90000
   #define SGS_MAIN_BLK11_START_ADR          0xA0000
   #define SGS_MAIN_BLK12_START_ADR          0xB0000
   #define SGS_MAIN_BLK13_START_ADR          0xC0000
   #define SGS_MAIN_BLK14_START_ADR          0xD0000
   #define SGS_MAIN_BLK15_START_ADR          0xE0000
   #define SGS_MAIN_BLK16_START_ADR          0xF0000
   #define SGS_LAST_BLK_END_ADR_P_1          0x100000

   /* --- GLOBAL TYPDEFINITIONS --- */
   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */
   extern int flash_erase(byte bSector);
   extern int flash_write_word(dword dwAddress, word wData);
   extern int flash_write(dword dwAddress, dword dwNumber, word *pwData);
   extern int detect_flash_type(word * wFlashTyp); // Return 0xAABB, AA-> DEV_CODE..
                                                   //                BB-> MAN_CODE_ST
   extern int flash_read_word(dword dwAddress,word *pwData);
   extern int flash_read(dword dwAddress, dword dwNumber,word *pwData);

   /* --- FUNCTIONCODE --- */
   #define FLASH_ERASE           1
   #define FLASH_WRITE_WORD      2
   #define FLASH_WRITE           3
   #define DETECT_FLASH_TYPE     4
   #define FLASH_READ_WORD       5
   #define FLASH_READ            6
#endif

/*--------------------------------------------------------------------------
  Name:    c2c10log.h     Description: Functions for Datalogge communication
  Version: 18.04.02 /GM                             Copyrights (c) 2002, STW
  --------------------------------------------------------------------------*/
#ifndef __C2C10LOG_H  
   #define __C2C10LOG_H

   /*--- GLOBAL DEFINES CONSTANTS ---*/
   #define MAX_DATA_BUFFER                0x1FFF            /* Buffersize for automatic storage */
   #define LOGGER_HANDLE_DO_EXIST         1
   #define LOGGER_HANDLE_DO_NOT_EXIST     0
   /*--- GLOBAL TYPDEFINITIONS ---*/
   /* Structur der Datensätze */
   /*--- GLOBAL VARIABLES ---*/
   /*--- PROTOTYPES ---*/
   extern int init_logger_system(boolean bitUseBuffer);
   extern int get_logger_free_space(word wHandle, dword * pdwFreeSpace);
   extern int release_logger(word wHandle);
   extern int release_logger_all(void);
   extern int write_logger(word wHandle, word * pwData, dword dwNumWords);
   extern int read_logger(word wHandle, word wNumber, word * pwData, boolean bitTiggerEnable);
   extern int get_logger_handle_data_length(word wHandle, word wNumber, dword * pdwDataLength);
   extern int allocate_new_logger_space(dword * dwSize, word * wHandle);
   extern int get_handle_exist(word wHandle);
   extern int get_handle_max(word * wHandle);
   extern int get_handle_allocate_max(word * wHandle);
   extern int set_spaceoverflow_function(word wHandle, void (*pfFunctionName)());
   extern int reset_spaceoverflow_function(word wHandle);
   extern int set_logger_down(void);
   extern int change_logger_int_save_speed(byte bLoggerSpeed);
   extern int lock_logger_handle(word wHandle, dword * pdwSize, dword * pdwAddress);
   extern int clear_data_locked_logger_handle(word wHandle);
   extern int flash_write_word_with_buffer(dword dwAddress, word wData);
   extern int flash_write_with_buffer(dword dwAddress, dword dwNumber, word *pwData);


   /*--- FUNCTION CODE ---*/
   #define INIT_LOGGER_SYSTEM                1
   #define GET_LOGGER_FREE_SPACE             2
   #define RELEASE_LOGGER                    3
   #define RELEASE_LOGGER_ALL                4
   #define WRITE_LOGGER                      5
   #define READ_LOGGER                       6
   #define GET_LOGGER_HANDLE_DATA_LENGTH     7
   #define ALLOCATE_NEW_LOGGER_SPACE         8
   #define GET_HANDLE_EXIST                  9
   #define GET_HANDLE_MAX                    10
   #define SET_SPACEOVERFLOW_FUNCTION        11
   #define RESET_SPACEOVERFLOW_FUNCTION      12
   #define SET_LOGGER_DOWN                   13
   #define CHANGE_LOGGER_INT_SAVE_SPEED      14
   #define GET_HANDLE_ALLOCATE_MAX           15
   #define LOCK_LOGGER_HANDLE                16
   #define CLEAR_DATA_LOCKED_LOGGER_HANDLE   17
   #define FLASH_WRITE_WORD_WITH_BUFFER      18
   #define FLASH_WRITE_WITH_BUFFER           19
       
#endif
/*--------------------------------------------------------------------------
  Name:    c2c10-b1.h    Description:  Header for flash programming interface
                                       for the 4MB module
  Version: 24.06.03  /GM                              Copyright (c) 2003, STW
  --------------------------------------------------------------------------*/
#ifndef __C2C10_B1_H
   #define __C2C10_B1_H

   /* --- GLOBAL DEFINES AND CONSTANTS --- */
   /* Manufactorcode ,Devicecode, Sectornumber and size of the different devices */
   #define B1_MAN_CODE_ST                       0x20     // SGS 29F400 / 29F800
   #define B1_MAN_CODE_AD                       0x01     // AD 29F400 / 29F800
   #define B1_DEV_CODE_ST_BOT_400               0xD6     // SGS 29F400BB
   #define B1_DEV_CODE_ST_BOT_800               0x58     // SGS 29F800AB
   #define B1_DEV_SEC_ST_400                    11 * 4   // Sectornumber for 29F400
   #define B1_DEV_SEC_ST_800                    19 * 4   // Sectornumber for 29F800
   #define B1_SGS_LAST_ADRESS_ST_400            (0x7FFFFUL * 4) + 3
   #define B1_SGS_LAST_ADRESS_ST_800            (0xFFFFFUL * 4) + 3
   
   #define B1_FLASH_READY                       0
   #define B1_FLASH_NOT_READY                   1

   /* Startadressen der einzelnen Flashbloecke */
   #define B1_SGS_1MB_BLK_0_START_ADR               0x0
   #define B1_SGS_1MB_BLK_1_START_ADR               0x04000
   #define B1_SGS_1MB_BLK_2_START_ADR               0x06000
   #define B1_SGS_1MB_BLK_3_START_ADR               0x08000
   #define B1_SGS_1MB_BLK_4_START_ADR               0x10000
   #define B1_SGS_1MB_BLK_5_START_ADR               0x20000
   #define B1_SGS_1MB_BLK_6_START_ADR               0x30000
   #define B1_SGS_1MB_BLK_7_START_ADR               0x40000
   #define B1_SGS_1MB_BLK_8_START_ADR               0x50000
   #define B1_SGS_1MB_BLK_9_START_ADR               0x60000
   #define B1_SGS_1MB_BLK_10_START_ADR              0x70000
   #define B1_SGS_1MB_BLK_11_START_ADR              0x80000
   #define B1_SGS_1MB_BLK_12_START_ADR              0x90000
   #define B1_SGS_1MB_BLK_13_START_ADR              0xA0000
   #define B1_SGS_1MB_BLK_14_START_ADR              0xB0000
   #define B1_SGS_1MB_BLK_15_START_ADR              0xC0000
   #define B1_SGS_1MB_BLK_16_START_ADR              0xD0000
   #define B1_SGS_1MB_BLK_17_START_ADR              0xE0000
   #define B1_SGS_1MB_BLK_18_START_ADR              0xF0000
   #define B1_SGS_1MB_LAST_BLK_END_ADR_P_1          0x100000
   #define B1_SGS_1MB_BLK_19_START_ADR              0x0 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_20_START_ADR              0x04000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_21_START_ADR              0x06000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_22_START_ADR              0x08000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_23_START_ADR              0x10000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_24_START_ADR              0x20000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_25_START_ADR              0x30000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_26_START_ADR              0x40000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_27_START_ADR              0x50000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_28_START_ADR              0x60000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_29_START_ADR              0x70000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_30_START_ADR              0x80000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_31_START_ADR              0x90000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_32_START_ADR              0xA0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_33_START_ADR              0xB0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_34_START_ADR              0xC0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_35_START_ADR              0xD0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_36_START_ADR              0xE0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_37_START_ADR              0xF0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_LAST_BLK_END_ADR_P_2          0x100000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_1
   #define B1_SGS_1MB_BLK_38_START_ADR              0x0 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_39_START_ADR              0x04000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_40_START_ADR              0x06000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_41_START_ADR              0x08000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_42_START_ADR              0x10000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_43_START_ADR              0x20000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_44_START_ADR              0x30000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_45_START_ADR              0x40000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_46_START_ADR              0x50000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_47_START_ADR              0x60000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_48_START_ADR              0x70000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_49_START_ADR              0x80000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_50_START_ADR              0x90000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_51_START_ADR              0xA0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_52_START_ADR              0xB0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_53_START_ADR              0xC0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_54_START_ADR              0xD0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_55_START_ADR              0xE0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_56_START_ADR              0xF0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_LAST_BLK_END_ADR_P_3          0x100000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_2
   #define B1_SGS_1MB_BLK_57_START_ADR              0x0 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_58_START_ADR              0x04000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_59_START_ADR              0x06000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_60_START_ADR              0x08000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_61_START_ADR              0x10000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_62_START_ADR              0x20000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_63_START_ADR              0x30000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_64_START_ADR              0x40000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_65_START_ADR              0x50000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_66_START_ADR              0x60000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_67_START_ADR              0x70000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_68_START_ADR              0x80000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_69_START_ADR              0x90000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_70_START_ADR              0xA0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_71_START_ADR              0xB0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_72_START_ADR              0xC0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_73_START_ADR              0xD0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_74_START_ADR              0xE0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_BLK_75_START_ADR              0xF0000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3
   #define B1_SGS_1MB_LAST_BLK_END_ADR_P_4          0x100000 + B1_SGS_1MB_LAST_BLK_END_ADR_P_3



   /* --- GLOBAL TYPDEFINITIONS --- */
   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */
   extern int B1_detect_flash_type(word * wFlashTyp); // Return 0xAABB, AA-> DEV_CODE..
                                                      //                BB-> MAN_CODE_ST
   extern int B1_flash_erase(byte bSector);
   extern int B1_flash_write_word(dword dwAddress, word wData);
   extern int B1_flash_write(dword dwAddress, dword dwNumber, word *pwData);
   extern int B1_flash_read_word(dword dwAddress,word *pwData);
   extern int B1_flash_read(dword dwAddress, dword dwNumber,word *pwData);

   /* --- FUNCTIONCODE --- */
   #define B1_FLASH_ERASE        1
   #define B1_FLASH_WRITE_WORD   2
   #define B1_FLASH_WRITE        3
   #define B1_DETECT_FLASH_TYPE  4
   #define B1_FLASH_READ_WORD    5
   #define B1_FLASH_READ         6
#endif

/*--------------------------------------------------------------------------
  Name:    c2c10-b2.h    Description:  Header for Bluetoothmodul
  Version: 10.08.04  /GM                              Copyright (c) 2004, STW
  --------------------------------------------------------------------------*/
#ifndef __C2C10_B2_H
   #define __C2C10_B2_H

   /* --- GLOBAL DEFINES AND CONSTANTS --- */

   /* --- GLOBAL TYPDEFINITIONS --- */
   /* --- GLOBAL VARIABLES --- */
   /* --- PROTOTYPES --- */
   extern void b2_set_gps_ant_power(byte bState);
   extern void b2_set_gps_enable(byte bState);
   extern void b2_set_gps_lock(byte bState);
   extern void b2_set_gps_reset(byte bState);
   extern void b2_set_bluetooth_reset(byte bState);
   extern void b2_set_rs232_reset(byte bState);
   extern void b2_set_bluetooth_rts(byte bState);

   /* --- FUNCTIONCODE --- */
#endif
