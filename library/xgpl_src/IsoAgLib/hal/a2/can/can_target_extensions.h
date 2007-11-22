/***************************************************************************
                          can_target_extensions.h - header for PC specific
                                                extensions for the HAL
                                                for CAN
                             -------------------
    begin                : Sat Jan 01 2003
    copyright            : (C) 2003 by Dipl.-Inform. Achim Spangler
             : This file was based on the corresponding file in
             : the ESX HAL and modified for A2 HAL.
             : These changes (C) 2006 - 2010 Brian Wei
    email                : a.spangler@osb-ag:de
             : brian.wei@agcocorp:com
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
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
#ifndef _PC_HAL_CAN_EXTENSIONS_H_
#define _PC_HAL_CAN_EXTENSIONS_H_

#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"
#include "can_typedef.h"

namespace __HAL {
/* ******************************************************** */
/**
 * \name CAN Specific Functions
 */
/* ******************************************************** */
/*@{*/

/* Typ CAN-Nachrichtenobjekt */
#define  RX                             0
#define  TX                             1

/* Datastructure: Status of Messageobject (30 Byte) */
typedef struct  {
  tTime tLastIrqTime;        /* Timerecording by send/receive data on CAN */
  uint32_t dwId;                /* Identifier */
  uint8_t bXtd;                  /* :=FALSE  11bit-Identifier, :=TRUE  29bit-Identifier */
  uint16_t wPause;               /* Delaytime for transmit data (0...32000ms) */
  uint8_t bType;                /* Typ of Messageobject */
  uint8_t bUsage;               /* Usage of Messageobject */
  uint8_t bOverflow;            /* :=TRUE  Bufferoverflow of Mesageobject */
  tTime tOverflowTime;       /* Time of last Bufferoverflow */
  uint8_t bSendRequest;         /* :=TRUE  Messageobject will be sent */
  uint16_t wNumberMsgs;          /* Anzahl der Nachrichten in Nachrichtenobjekt (S/E) */
  uint16_t wMsgCtrlReg;          /* Message Control Register */
} tCanObjStatus;

int16_t initCan ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg,uint16_t wBitrate );

#ifndef A2_SQUEEZE_CODE
int16_t changeGlobalMask( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg );
int16_t lockCanObj( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, bool ab_doLock );
#endif
int16_t closeCan ( uint8_t bBusNumber );
int16_t getCanBusStatus(uint8_t bBusNumber, tCanBusStatus* ptStatus);
int16_t getCanObjStatus(byte bBusNumber,byte bMsgObj,tCanObjStatus *ptStatus);
int16_t clearCanObjBuf(uint8_t bBusNumber, uint8_t bMsgObj);
int16_t configCanObj ( uint8_t bBusNumber, uint8_t bMsgObj, tCanObjConfig * ptConfig );
int16_t closeCanObj ( uint8_t bBusNumber,uint8_t bMsgObj );
bool waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval );
int16_t sendCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tSend * ptSend );
int16_t checkMsg();
int16_t getCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tReceive * ptReceive );
int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint8_t bXtd );

int16_t chgCanObjPause ( uint8_t bBusNumber, uint8_t bMsgObj, uint16_t wPause);
int16_t getCanMsgBufCount(uint8_t bBusNumber,uint8_t bMsgObj);
bool getCanMsgObjLocked(uint8_t bBusNumber,uint8_t bMsgObj);


/*@}*/

} // End of __HAL
#endif
