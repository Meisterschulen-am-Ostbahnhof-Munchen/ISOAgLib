/*
  can_typedef.h: header for special BIOS types

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
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

