/***************************************************************************
                          pc_eeditor_can.cpp  - source for the PSEUDO BIOS
                                        CAN EEPROM Editor module for
                                       development and test on a PC
                             -------------------
    begin                : Wed Aug 4 1999
    copyright            : (C) 1999 - 2004 Dipl.-Inform. Achim Spangler
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
#include "pc_eeditor_can.h"
#include "eeprom.h"
#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"
#include <string.h>
#include <cstdio>
//typedef char int8_t;

namespace __HAL {

int16_t iInitEEEditor(  uint8_t bBus,
                       int16_t iObjNrReceiveCan, int16_t iObjNrTransmitCan,
                       uint32_t dwReceiveCanId, uint8_t bCanType,
                       int16_t iNumberMsgsReceive, int16_t iNumberMsgsTransmit)
{
 FILE *eeditor;
 int16_t wAddress, wLen;
 int32_t lVal;
 short iVal;
 int8_t cVal;
 float fVal;
 int8_t pcType;
 char pcZeile[50], pcVal[50];

  printf("iInitEEEditor mit CAN-BUS %hu, Empfangs Objekt %hd, Sende Objekt %hd, \
    Empfangs CAN Ident_c %x, Empfangspuffer %d, Sendepuffer %d und CanType %hd aufgerufen.\n",
    bBus, iObjNrReceiveCan, iObjNrTransmitCan, dwReceiveCanId, iNumberMsgsReceive,
    iNumberMsgsTransmit, bCanType);

 printf("Funktion zum Start des EEPROM Editors aufgerufen - lese Daten in EEPROM Datei\n");
#ifdef WIN32
 eeditor = fopen("..\\..\\..\\simulated_io\\eeditor.dat", "r+");
#else
 eeditor = fopen("../../../simulated_io/eeditor.dat", "r+");
#endif
 // BEGIN: Added by M.Wodok 6.12.04
 if (eeditor == NULL) {
   // try again with current path...
   fopen("eeditor.dat", "r+");
 }
 // END: Added by M.Wodok 6.12.04

 while (fgets(pcZeile, 49, eeditor) != NULL)
 {
  /* erase '\n' at end of line */
  if (strchr(pcZeile, '\n') != 0)
    *(strchr(pcZeile, '\n')) = '\0';

  sscanf(pcZeile, "%hd %hd %c %s", &wAddress, &wLen, &pcType, pcVal);
  switch (pcType)
  {
    case 'l':
      sscanf(pcVal, "%d", &lVal);
      printf("EEprom Editor Long %d and Addresse %d\n", lVal, wAddress);
      eepromWrite(wAddress, 4, (const uint8_t*)&lVal);
      break;
    case 'i':
      sscanf(pcVal, "%hd", &iVal);
      printf("EEprom Editor Int16 %hd and Addresse %d\n", iVal, wAddress);
      eepromWrite(wAddress, 2, (const uint8_t*)&iVal);
      break;
    case 'c':
      sscanf(pcVal, "%hd", &iVal);
      cVal = iVal;
      printf("EEprom Editor int8_t %c and Addresse %d\n", cVal, wAddress);
      eepromWrite(wAddress, 1, (const uint8_t*)&cVal);
      break;
    case 'f':
      sscanf(pcVal, "%f", &fVal);
      printf("EEprom Editor Float %f and Addresse %d\n", fVal, wAddress);
      eepromWrite(wAddress, 4, (const uint8_t*)&fVal);
      break;
    case 's':
      printf("EEprom Editor Text %s and Addresse %d\n", pcVal, wAddress);
      eepromWrite(wAddress, wLen, (const uint8_t*)pcVal);
      break;
  }
 }
  return HAL_NO_ERR;
}

int16_t iCallCanEEMonitor(void)
{
  //printf("iCallCanEEMonitor())\n");
  return HAL_NO_ERR;
}

} //end namespace __HAL
