/*
  can_target_extensions.h: header for PC specific extensions for the
    HAL for CAN

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _PC_HAL_CAN_EXTENSIONS_H_
#define _PC_HAL_CAN_EXTENSIONS_H_

#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"
#include "can_typedef.h"

namespace __HAL {

#define COMMON_MSGOBJ_IN_QUEUE  0xFF

/* ******************************************************** */
/**
 * \name CAN Specific Functions
 */
/* ******************************************************** */
/*@{*/
int16_t can_startDriver();
int16_t can_stopDriver();
int32_t can_lastReceiveTime();
int16_t init_can ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg,uint16_t wBitrate );
int16_t changeGlobalMask( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg );
int16_t closeCan ( uint8_t bBusNumber );
int16_t getCanBusStatus(uint8_t bBusNumber, tCanBusStatus* ptStatus);
int16_t clearCanObjBuf(uint8_t bBusNumber, uint8_t bMsgObj);
int16_t configCanObj ( uint8_t bBusNumber, uint8_t bMsgObj, tCanObjConfig * ptConfig );
int16_t closeCanObj ( uint8_t bBusNumber,uint8_t bMsgObj );
bool waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval );
int16_t sendCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tSend * ptSend );
int16_t checkMsg();
int16_t getCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tReceive * ptReceive );
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint8_t bXtd );
#else
int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint32_t mask, uint8_t bXtd );
#endif
int16_t lockCanObj( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, bool ab_doLock );

int16_t chgCanObjPause ( uint8_t bBusNumber, uint8_t bMsgObj, uint16_t wPause);
int16_t getCanMsgBufCount(uint8_t bBusNumber,uint8_t bMsgObj);
bool getCanMsgObjLocked(uint8_t bBusNumber,uint8_t bMsgObj);

int32_t getMaxSendDelay(uint8_t aui8_busNr);

int32_t getPipeHandleForCanRcvEvent();

/* Typ CAN-Nachrichtenobjekt */
#define  RX                             0
#define  TX                             1

/* Datastructure: Status of Messageobject (30 Byte) */
typedef struct  {
  tTime tLastIrqTime;        /* Timerecording by send/receive data on CAN */
  uint32_t dwId;                /* Identifier */
  uint8_t bXtd;                  /* == 0 => 11bit-Identifier    == 1 => 29bit-Identifier */
  uint16_t wPause;               /* Delaytime for transmit data (0...32000ms) */
  uint8_t bType;                /* Typ of Messageobject */
  uint8_t bUsage;               /* Usage of Messageobject */
  uint8_t bOverflow;            /* :=TRUE  Bufferoverflow of Mesageobject */
  tTime tOverflowTime;       /* Time of last Bufferoverflow */
  uint8_t bSendRequest;         /* :=TRUE  Messageobject will be sent */
  uint16_t wNumberMsgs;          /* Anzahl der Nachrichten in Nachrichtenobjekt (S/E) */
  uint16_t wMsgCtrlReg;          /* Message Control Register */
} tCanObjStatus;
/*@}*/

} // End of __HAL
#endif
