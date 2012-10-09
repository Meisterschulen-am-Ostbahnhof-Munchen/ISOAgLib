/***************************************************************************
                          can_typedef.h  - header for special BIOS types
                             -------------------
    begin                : Wed Aug 4 1999
    copyright            : THIS HEADER IS DERIVED FROM ORIGNINAL STW HEADER
    email                : a.spangler@osb-ag:de
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

#ifndef __CAN_TYPEDEF_
#define __CAN_TYPEDEF_

#include "../typedef.h"

namespace __HAL {

/** Funktionszeiger Fehlerreaktion auf Can Bus Fehler*/
typedef uint16_t (*tCAN_ERR_FUNC)(uint8_t,uint16_t);

/** Datenstruktur im CAN Register (16 Byte) */
typedef struct
{
  tBw   tMessageCtrl;
  tDw   tArbit;
  tBw   tCfg_D0;
  tDw   tD1_D4;
  tDw   tD5_D7;
} tCanMsgReg;

/** Funktionszeiger =Funktion nach Sende- Empfangsinterrupt
 * uint8_t bBusNumber, uint8_t bMsgObj [0..13],
 * tCanMsgReg * ptCanMsgObj = Adresse Nachrichtenobjekt Can Register */
 typedef tCanMsgReg * (*tIRQ_FUNCTION)(uint8_t,uint8_t,tCanMsgReg *);



/** Datenstruktur fuer CAN status */
typedef struct
{
  tIRQ_FUNCTION pfwBusOff;
  tIRQ_FUNCTION pfwBusWarn;
  tTime tLastErrorTime;
  uint32_t dwGlobMask;
  uint16_t wGlobMask;
  uint32_t dwLastMsgObjMask;
  uint16_t wBitrate;
  uint16_t wCtrlStatusReg;
  uint16_t wBittimingReg;
} tCanBusStatus;

/** Message Objekt Datenstruktur fuer CAN Botschaft *
 * Struktur Sende/Empfang Daten allgemein fuer Anwender (14 Byte) */
typedef struct
{
  uint32_t dwId;                          /** Identifier */
  uint8_t bXtd;                           /** Laenge Bit Identifier */
  uint8_t bDlc;                           /** Anzahl der empfangenen Daten */
  uint8_t abData[8];                      /** Datenpuffer */
} tSend;


/** Struktur empfange Daten allgemein (20 Byte) */
typedef struct
{
  uint32_t dwId;                          /** Identifier */
  uint8_t bXtd;                           /** Laenge Bit Identifier */
  uint8_t bDlc;                           /** Anzahl der empfangenen Daten */
  uint8_t abData[8];                      /** Datenpuffer */
  tTime tReceiveTime;             /** Zeitpunkt Empfang */
} tReceive;


typedef struct                  /** Struktur Konfigurierung von Nachrichtenobjekten allgemein (16 Byte) */
{
  uint32_t dwId;                          /** Identifier */
  uint8_t bXtd;                           /** Laenge Bit Identifier */
  uint8_t bMsgType;                       /** Typ Nachrichtenobjekt */
  uint16_t wNumberMsgs;                    /** Groesse Ringpuffer in Nachrichtenobjekten */
  uint8_t bTimeStamped;                   /** Zeitstempel Empfang aktualisieren */
  uint16_t wPause;                         /** Pausenzeit fuer verzoegertes Senden */
  tIRQ_FUNCTION pfIrqFunction;         /** Funktionszeiger fuer "Anwenderdefinierte" Fkt nach
                                       * Can HW Interrupt*/
} tCanObjConfig;

} // end namespace __HAL
#endif

