/***************************************************************************
                          target_extension_can_simulating.cpp  - simulating PC CAN driver
                                                 which uses files for I/O

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
#include "can_target_extensions.h"
#include <IsoAgLib/hal/pc/system/system.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cctype>

namespace __HAL {

// globale Filehandle fuer simulierte CAN Input
static FILE* can_input[2][15];
static FILE* can_output[2][2];
static tReceive pRead[2][15];
static tIRQ_FUNCTION can_irq[2][15];
static uint8_t pOpen[2][15];
static uint8_t pSendOpen[2][15];
static uint8_t dateiende[2][15];
static int32_t lastCloseTimeArr[2][15];
static bool b_canBufferLock[2][15];
int32_t i32_lastSendTime;
int32_t i32_lastReceiveTime;
int32_t i32_lastCloseTime = 0;
uint16_t ui16_sendPause;

static boolean pb_bXtd[2][15];

int16_t can_startDriver()
{
  i32_lastCloseTime = 0;
  return HAL_NO_ERR;
}
int16_t can_stopDriver()
{
  return HAL_NO_ERR;
}

/** get last timestamp of CAN receive */
int32_t can_lastReceiveTime()
{
  checkMsg();
  return i32_lastReceiveTime;
}

int16_t getCanMsgBufCount(uint8_t bBusNumber,uint8_t bMsgObj)
{
  if((bBusNumber > 1)&&(bMsgObj > 14)) return HAL_RANGE_ERR;
  if (pOpen[bBusNumber][bMsgObj] != 1) return 0;
  if (dateiende[bBusNumber][bMsgObj] == 1) return 0;
  return (pRead[bBusNumber][bMsgObj].tReceiveTime.l1ms <= getTime())?1:0;
}
int16_t init_can ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg,uint16_t wBitrate )
{
  int16_t i;
  printf("init_can mit BusNr: %u,\nglobale Maske: %x, %x, lastMsg: %x, \nBitrate: %u\n",
    bBusNumber,wGlobMask,dwGlobMask,dwGlobMaskLastmsg,wBitrate);
  static bool firstCall = true;
  if ( firstCall )
  {
    for (i=0; i < 15; i++)
    {
      pOpen[bBusNumber][i] = pSendOpen[bBusNumber][i] = lastCloseTimeArr[bBusNumber][i] = 0;
    }
    i32_lastSendTime = 0;
    ui16_sendPause = 0;
  }
  firstCall = false;
  return HAL_NO_ERR;
}

int16_t closeCan ( uint8_t bBusNumber )
{
  printf("can_close fuer BUS %d\n", bBusNumber);
  i32_lastCloseTime = getTime();
  for (int i=0; i < 15; i++)
  {
    if ( pOpen[bBusNumber][i] == 1 )
    {
      fclose(can_input[bBusNumber][i]);
      pOpen[bBusNumber][i] = 0;
    }
    if ( pSendOpen[bBusNumber][i] == 1 )
    {
      fclose(can_output[bBusNumber][i]);
      pSendOpen[bBusNumber][i] = 0;
    }
  }
  return HAL_NO_ERR;
}

int16_t getCanBusStatus(uint8_t bBusNumber, tCanBusStatus* ptStatus)
{
  //prFieldstarOldintf("getCanBusStatus fuer BUS %d\n", bBusNumber);
  return ((bBusNumber < 2)&&(ptStatus != NULL))?HAL_NO_ERR:HAL_RANGE_ERR;
}
int16_t clearCanObjBuf(uint8_t bBusNumber, uint8_t bMsgObj)
{
  printf("clearCanObjBuf fuer BUS %hd und Object %hd\n", bBusNumber, bMsgObj);
  return HAL_NO_ERR;
}

void scanCanMsgLine( uint8_t bBusNumber,uint8_t bMsgObj )
{
  char zeile[100];
	printf("lese naechste Zeile aus BUS %d und Objekt %d\n", bBusNumber, bMsgObj);
	while ( true )
	{ // read lines till timestamp is >= last close time
		fgets(zeile, 99, can_input[bBusNumber][bMsgObj]);
		if ( strlen( zeile ) == 0 )
		{
			if ( feof(can_input[bBusNumber][bMsgObj]) )
			{
				dateiende[bBusNumber][bMsgObj] = 1;
				break;
			}
			else continue;
		}
		int iResult = sscanf(zeile, "%u %x  %hx %hx %hx %hx %hx %hx %hx %hx \n",
			&(pRead[bBusNumber][bMsgObj].tReceiveTime.l1ms), &(pRead[bBusNumber][bMsgObj].dwId),
			&(pRead[bBusNumber][bMsgObj].abData[0]), &(pRead[bBusNumber][bMsgObj].abData[1]),
			&(pRead[bBusNumber][bMsgObj].abData[2]), &(pRead[bBusNumber][bMsgObj].abData[3]),
			&(pRead[bBusNumber][bMsgObj].abData[4]), &(pRead[bBusNumber][bMsgObj].abData[5]),
			&(pRead[bBusNumber][bMsgObj].abData[6]), &(pRead[bBusNumber][bMsgObj].abData[7]));
		// check for close time
		if ( ( pRead[bBusNumber][bMsgObj].tReceiveTime.l1ms >= lastCloseTimeArr[bBusNumber][bMsgObj] )
			&& ( iResult != EOF ) )
		{
			pRead[bBusNumber][bMsgObj].bDlc = iResult;
			printf("vorabgelesene Zeile in BIOS Funktion:\n%s\n", zeile);
			break;
		}
		else printf( "ignore old message from file with timestamp %u with last close time %i\n",
									pRead[bBusNumber][bMsgObj].tReceiveTime.l1ms, lastCloseTimeArr[bBusNumber][bMsgObj]);
		if ( feof(can_input[bBusNumber][bMsgObj]) != 0 )
		{ // if actual time after last message in this file, set the "dateiende" flag
			printf("in getCanMsg die letzte Zeile geliefert, keine neue Zeile in Buffer gelesen, da Dateiende erreicht\n");
			dateiende[bBusNumber][bMsgObj] = 1;
			break;
		}
	}
}

int16_t configCanObj ( uint8_t bBusNumber, uint8_t bMsgObj, tCanObjConfig * ptConfig )
{
  char name[50];
  char zeile[100];


  pb_bXtd[bBusNumber][bMsgObj] = ptConfig->bXtd;
	b_canBufferLock[bBusNumber][bMsgObj] = false;

  /* test ob sendeobjekt configuriert wird */
  if (ptConfig->bMsgType == TX)
  { /* Sendeobjekt */
#ifdef WIN32
    sprintf(name, "..\\..\\..\\simulated_io\\can_send_%hx_%hx_", bBusNumber, bMsgObj);
#else
    sprintf(name, "../../../simulated_io/can_send_%hx_%hx_", bBusNumber, bMsgObj);
#endif
    if (ptConfig->bXtd) strcat(name, "ext");
    else strcat(name, "std");

    if ( pSendOpen[bBusNumber][bMsgObj] != 1 )
    {
      pSendOpen[bBusNumber][bMsgObj] = 1;
      can_output[bBusNumber][bMsgObj] = fopen(name, "a+");
      // BEGIN: Added by M.Wodok 6.12.04
      if (can_output[bBusNumber][bMsgObj] == NULL) {
        sprintf(name, "can_send_%hx_%hx_", bBusNumber, bMsgObj);
        if (ptConfig->bXtd) strcat(name, "ext");
        else strcat(name, "std");
        
        can_output[bBusNumber][bMsgObj] = fopen(name, "a+");
      }
      // END: Added by M.Wodok 6.12.04
      printf("Versuch Datei mit Name %s erstmalig zum schreiben zu oeffnen\n", name);
    }
    else
    {
      printf("Versuch Datei mit Name %s zum WEITEREN schreiben zu oeffnen\n", name);
    }
  }
  else
  { /* empfangsobjekt */
#ifdef WIN32
    sprintf(name, "..\\..\\..\\simulated_io\\can_%hx_%x_", bBusNumber, ptConfig->dwId);
#else
    sprintf(name, "../../../simulated_io/can_%hx_%x_", bBusNumber, ptConfig->dwId);
#endif
    if (ptConfig->bXtd) strcat(name, "ext");
    else strcat(name, "std");
    
    can_input[bBusNumber][bMsgObj] = fopen(name, "r");
    // BEGIN: Added by M.Wodok 6.12.04
    if (can_input[bBusNumber][bMsgObj] == NULL) {
      sprintf(name, "can_%hx_%x_", bBusNumber, ptConfig->dwId);
      if (ptConfig->bXtd) strcat(name, "ext");
      else strcat(name, "std");

      can_input[bBusNumber][bMsgObj] = fopen(name, "r");
    }
    // END: Added by M.Wodok 6.12.04
    
    printf("Versuch Datei mit Name %s zum lesen zu oeffnen\n", name);

    dateiende[bBusNumber][bMsgObj] = 0;
    pRead[bBusNumber][bMsgObj].bXtd = ptConfig->bXtd;
    can_irq[bBusNumber][bMsgObj] = ptConfig->pfIrqFunction;
    pOpen[bBusNumber][bMsgObj] = 1;

    /* erste Zeile einlesen */
		scanCanMsgLine( bBusNumber, bMsgObj );
  }
  return HAL_NO_ERR;
}

int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint8_t bXtd )
{
  char name[50];
  char zeile[100];

	b_canBufferLock[bBusNumber][bMsgObj] = false;
  if (pOpen[bBusNumber][bMsgObj] != 1) return 0;

  pb_bXtd[bBusNumber][bMsgObj] = bXtd;
#ifdef WIN32
  sprintf(name, "..\\..\\..\\simulated_io\\can_%hx_%x_", bBusNumber, dwId);
#else
  sprintf(name, "../../../simulated_io/can_%hx_%x_", bBusNumber, dwId);
#endif
  if (bXtd) strcat(name, "ext");
  else strcat(name, "std");

  fclose(can_input[bBusNumber][bMsgObj]);
  can_input[bBusNumber][bMsgObj] = fopen(name, "r");
  
  // BEGIN: Added by M.Wodok 6.12.04
  if (can_input[bBusNumber][bMsgObj] == NULL) {
    sprintf(name, "can_%hx_%x_", bBusNumber, dwId);
    if (bXtd) strcat(name, "ext");
    else strcat(name, "std");
  
    can_input[bBusNumber][bMsgObj] = fopen(name, "r");
  }
  // END: Added by M.Wodok 6.12.04

  /* erste Zeile einlese */
	scanCanMsgLine( bBusNumber, bMsgObj );
  return HAL_NO_ERR;
}
/**
	lock a MsgObj to avoid further placement of messages into buffer.
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
	@param rb_doLock true==lock(default); false==unlock
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
	*/
int16_t lockCanObj( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, bool rb_doLock )
{ // first get waiting messages
	checkMsg();
	b_canBufferLock[rui8_busNr][rui8_msgobjNr] = rb_doLock;
}

int16_t chgCanObjPause ( uint8_t bBusNumber, uint8_t bMsgObj, uint16_t wPause)
{
  printf("sende Pause auf BUS %d fuer CAN Objekt %d auf %d eingestellt\n",
  (int16_t)bBusNumber, (int16_t)bMsgObj, wPause);
  ui16_sendPause = wPause;
  return HAL_NO_ERR;
}


int16_t closeCanObj ( uint8_t bBusNumber,uint8_t bMsgObj )
{
  printf("closeCanObj fuer bus %d und objekt %d\n", bBusNumber, bMsgObj);
	b_canBufferLock[bBusNumber][bMsgObj] = false;
  if ((bMsgObj < 2) && ( pSendOpen[bBusNumber][bMsgObj] == 1))
  {
    fclose(can_output[bBusNumber][bMsgObj]);
    pSendOpen[bBusNumber][bMsgObj] = 0;
  }
  else
  {
    if (pOpen[bBusNumber][bMsgObj] == 1)
    {
      fclose(can_input[bBusNumber][bMsgObj]);
      pOpen[bBusNumber][bMsgObj] = 0;
    }
  }
  return HAL_NO_ERR;
};

int16_t sendCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tSend * ptSend )
{
  uint8_t data[8] = {0,0,0,0,0,0,0,0};
  int16_t i = 0;
  int32_t i32_sendTimestamp = getTime();
  memcpy(data, ptSend->abData, ptSend->bDlc);
  if (i32_lastSendTime + ui16_sendPause > i32_sendTimestamp)
    i32_sendTimestamp = i32_lastSendTime + ui16_sendPause;
  i32_lastSendTime = i32_sendTimestamp;
  if (ptSend->bXtd)
  { // extended Ident_c mit Ident_c Laenge 5
    fprintf(can_output[bBusNumber][bMsgObj],
      "%05d %-8x  %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx",
      i32_sendTimestamp, ptSend->dwId,
      data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    printf(
      "%05d %-8x  %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx",
      i32_sendTimestamp, ptSend->dwId,
      data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    if ( ptSend->dwId == 0x10 )
      printf( "Sent DIN message through ISO MsgObj\n" );
  }
  else
  {
    fprintf(can_output[bBusNumber][bMsgObj],
      "%05d %-8x  %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx",
      i32_sendTimestamp, ptSend->dwId,
      data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    printf(
      "%05d %-8x  %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx",
      i32_sendTimestamp, ptSend->dwId,
      data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
  }
  for (i= 0; i < 8; i++)
  { if (!isalnum(data[i]) && !(data[i] == ' ')) data[i] = '.';}
  fprintf(can_output[bBusNumber][bMsgObj], "\t\t%c %c %c %c %c %c %c %c\n",
    data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
  printf("\t\t%c %c %c %c %c %c %c %c\n",
    data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
  return HAL_NO_ERR;
};

int16_t getCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tReceive * ptReceive )
{
  char zeile[100];
  int32_t empfangszeit;

  if (getTime() < pRead[bBusNumber][bMsgObj].tReceiveTime.l1ms)
  {
    /* zeit noch nicht erreicht */
    return 0;
  }
  else
  {
    i32_lastReceiveTime = getTime();
    ptReceive->dwId = pRead[bBusNumber][bMsgObj].dwId;
    memcpy(ptReceive->abData, pRead[bBusNumber][bMsgObj].abData, 8);
    ptReceive->bDlc = pRead[bBusNumber][bMsgObj].bDlc = 8;
    ptReceive->tReceiveTime.l1ms = pRead[bBusNumber][bMsgObj].tReceiveTime.l1ms ;
    ptReceive->bXtd = pb_bXtd[bBusNumber][bMsgObj];
    lastCloseTimeArr[bBusNumber][bMsgObj] = pRead[bBusNumber][bMsgObj].tReceiveTime.l1ms;

    printf("in getCanMsg len rec %d und dat %d\n",ptReceive->bDlc, pRead[bBusNumber][bMsgObj].bDlc);

    /* naechste Zeile einlesen */
		scanCanMsgLine( bBusNumber, bMsgObj );
  }
  return HAL_NO_ERR;
};


int16_t checkMsg()
{
  int16_t i = 0, j = 0;
  int16_t result = 0;
  int16_t empfangen = 0;
  int16_t flag = 1;
  bool b_received[2][15] = { {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
                             {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false}};
  while (flag == 1)
  {
    result = i = j = 0;
    flag = 0;
    for (j = 0; j < 2; j++)
    {
        for (i=0; i < 15; i++)
        {
          if ((pOpen[j][i] == 1) && (feof(can_input[j][i]) == 0))
          {
            /* dateiende noch nicht erreicht */
            result = 1;
          }
          else if ((pOpen[j][i] == 1) && (feof(can_input[j][i]) != 0))
          {
            if (dateiende[j][i] == 0)
            {
              printf("Dateiende von Bus %d und Objekt %d erreicht\n", j,i);
              dateiende[j][i] = 1;
            }
          }
          if ((result == 1)&&(pOpen[j][i] == 1) && (pRead[j][i].tReceiveTime.l1ms < getTime()) && (dateiende[j][i] == 0))
          {
            /* zeitstempel ist erreicht -> verarbeiten */
            if ( can_irq[j][i] != NULL )
            {
							if ( b_canBufferLock[j][i] )
							{ // don't even check this MsgObj as it shall not receive messages
								// -> read next line as a message with current timestamp shall be ignored from file
								printf( "ignore message at BUS %d and Object %d with timestamp %d, as this message object is currently locked\n",
									j, i, pRead[j][i].tReceiveTime.l1ms);
								scanCanMsgLine( j, i );
								continue;
							}
							printf("rufe irq fun fuer BUS %d und Objekt %d auf bei zeit %d und CAN-Zeit %d\n", j, i, getTime(), pRead[j][i].tReceiveTime.l1ms);
              *(can_irq[j][i])(j,i,NULL);
            }
            empfangen++;
            // printf("bei check %d telegramme empfangen\n", empfangen);
            if (!b_received[j][i]) flag = 1;
            b_received[j][i] = true;
          }
        }
    }
  }
  return result;
}


} // end namespace __HAL
