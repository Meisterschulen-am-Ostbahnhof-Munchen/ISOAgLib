/***************************************************************************
                          target_extension_can_simulating.cpp  - simulating PC CAN driver
                                                 which uses files for I/O

                             -------------------
    begin                : Wed Aug 4 1999
    copyright            : (C) 1999 - 2009 Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                 *
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
#ifdef WIN32
       #include <windows.h>
#endif



#include <cstdlib>	// Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>

#include <IsoAgLib/hal/generic_utils/can/write_central_fifo.h>

namespace __HAL {

// globale Filehandle fuer simulierte CAN Input
static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );
//static FILE* can_input[cui32_maxCanBusCnt][15];
static FILE* can_output[cui32_maxCanBusCnt][2];
//static tReceive pRead[cui32_maxCanBusCnt][15];
//static tIRQ_FUNCTION can_irq[cui32_maxCanBusCnt][15];
//static uint8_t pOpen[cui32_maxCanBusCnt][15];
//static uint8_t pSendOpen[cui32_maxCanBusCnt][15];
//static uint8_t dateiende[cui32_maxCanBusCnt][15];
//static int32_t lastCloseTimeArr[cui32_maxCanBusCnt][15];
//static bool b_canBufferLock[cui32_maxCanBusCnt][15];
int32_t i32_lastSendTime;
int32_t i32_lastReceiveTime;
int32_t i32_lastCloseTime = 0;
uint16_t ui16_sendPause;

///////////////

typedef struct {

FILE* can_input;
tReceive pRead;
tIRQ_FUNCTION can_irq;
uint8_t pOpen;
uint8_t pSendOpen;
uint8_t dateiende;
int32_t lastCloseTimeArr;
bool b_canBufferLock;

boolean pb_bXtd;

} tMsgObj;

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  tMsgObj arrMsgObj[cui32_maxCanBusCnt][15];
#else
  //typedef STL_NAMESPACE::vector<tMsgObj> ArrMsgObj;
  //ArrMsgObj arrMsgObj[cui32_maxCanBusCnt];
  STL_NAMESPACE::vector<tMsgObj> arrMsgObj[cui32_maxCanBusCnt];
#endif

///////////////

//static boolean pb_bXtd[cui32_maxCanBusCnt][15];

int16_t can_startDriver()
{
  i32_lastCloseTime = 0;
  return HAL_NO_ERR;
}
int16_t can_stopDriver()
{
#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL

  for( uint32_t i=0; i<cui32_maxCanBusCnt; i++ )
	  arrMsgObj[i].clear();
#endif
  return HAL_NO_ERR;
}

/** get last timestamp of CAN receive */
int32_t can_lastReceiveTime()
{
  checkMsg();
  return i32_lastReceiveTime;
}

#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
/** there's no send-delay recognition in SIMULATING */
int32_t getMaxSendDelay(uint8_t aui8_busNr) { return 0; }
#endif

int16_t getCanMsgBufCount(uint8_t bBusNumber,uint8_t bMsgObj)
{
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  if((bBusNumber > 1)&&(bMsgObj > 14)) return HAL_RANGE_ERR;
#else
  if((bBusNumber > 1)&&(bMsgObj > arrMsgObj[bBusNumber].size()-1 )) return HAL_RANGE_ERR;
#endif
  if (arrMsgObj[bBusNumber][bMsgObj].pOpen != 1) return 0;
  if (arrMsgObj[bBusNumber][bMsgObj].dateiende == 1) return 0;
  return (arrMsgObj[bBusNumber][bMsgObj].pRead.tReceiveTime.l1ms <= getTime())?1:0;
}
int16_t init_can ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg,uint16_t wBitrate )
{
  int16_t i;
  printf("init_can mit BusNr: %u,\nglobale Maske: %x, %x, lastMsg: %x, \nBitrate: %u\n",
    bBusNumber,wGlobMask,dwGlobMask,dwGlobMaskLastmsg,wBitrate);
  static bool firstCall = true;
  if ( firstCall )
  {
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    for (i=0; i < 15; i++)
    {
      arrMsgObj[bBusNumber][i].pOpen = arrMsgObj[bBusNumber][i].pSendOpen = arrMsgObj[bBusNumber][i].lastCloseTimeArr = 0;
    }
#endif
    i32_lastSendTime = 0;
    ui16_sendPause = 0;
  }
  firstCall = false;
  return HAL_NO_ERR;
}

bool waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval )
{
  const int32_t ci32_endWait = getTime() + aui16_timeoutInterval;
  int32_t i32_waitSlice = aui16_timeoutInterval;

  // if greater than 50msec -> divide so that about 10 slices are realized
  if ( i32_waitSlice > 50 ) i32_waitSlice /= 10;
  // if still more than 50msec slice limit to 50
  if ( i32_waitSlice > 50 ) i32_waitSlice = 50;

  while (true)
  {
    for ( unsigned int busInd = 0; busInd < cui32_maxCanBusCnt; busInd++)
    {
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
      for ( unsigned int msgInd = 0; msgInd < 15; msgInd++ )
#else
      for ( unsigned int msgInd = 0; msgInd < arrMsgObj[busInd].size(); msgInd++ )
#endif
      {
        if ( getCanMsgBufCount( busInd, msgInd ) > 0 ) return true;
      }
    }
    #if defined(WIN32)
    Sleep( i32_waitSlice );
    #else
    usleep( i32_waitSlice * 1000 );
    #endif
    if ( getTime() >= ci32_endWait ) return false;
  }
}


int16_t changeGlobalMask ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg)
{
  return HAL_NO_ERR;
}


int16_t closeCan ( uint8_t bBusNumber )
{
  printf("can_close fuer BUS %d\n", bBusNumber);
  i32_lastCloseTime = getTime();
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  for (int i=0; i < 15; i++)
#else
  for (int i=0; i < arrMsgObj[bBusNumber].size(); i++)
#endif
  {
    if ( arrMsgObj[bBusNumber][i].pOpen == 1 )
    {
      fclose(arrMsgObj[bBusNumber][i].can_input);
      arrMsgObj[bBusNumber][i].pOpen = 0;
    }
    if ( arrMsgObj[bBusNumber][i].pSendOpen == 1 )
    {
      fclose(can_output[bBusNumber][i]);
      arrMsgObj[bBusNumber][i].pSendOpen = 0;
    }
  }

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  arrMsgObj[bBusNumber].clear();
#endif
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
  //printf("lese naechste Zeile aus BUS %d und Objekt %d\n", bBusNumber, bMsgObj);
  while ( true )
  { // read lines till timestamp is >= last close time
    fgets(zeile, 99, arrMsgObj[bBusNumber][bMsgObj].can_input);
    if ( strlen( zeile ) == 0 )
    {
      if ( feof(arrMsgObj[bBusNumber][bMsgObj].can_input) )
      {
        arrMsgObj[bBusNumber][bMsgObj].dateiende = 1;
        break;
      }
      else continue;
    }
    int iResult = sscanf(zeile, "%u %x  %hx %hx %hx %hx %hx %hx %hx %hx \n",
      &(arrMsgObj[bBusNumber][bMsgObj].pRead.tReceiveTime.l1ms), &(arrMsgObj[bBusNumber][bMsgObj].pRead.dwId),
      &(arrMsgObj[bBusNumber][bMsgObj].pRead.abData[0]), &(arrMsgObj[bBusNumber][bMsgObj].pRead.abData[1]),
      &(arrMsgObj[bBusNumber][bMsgObj].pRead.abData[2]), &(arrMsgObj[bBusNumber][bMsgObj].pRead.abData[3]),
      &(arrMsgObj[bBusNumber][bMsgObj].pRead.abData[4]), &(arrMsgObj[bBusNumber][bMsgObj].pRead.abData[5]),
      &(arrMsgObj[bBusNumber][bMsgObj].pRead.abData[6]), &(arrMsgObj[bBusNumber][bMsgObj].pRead.abData[7]));

    // check for close time
    if ( ( arrMsgObj[bBusNumber][bMsgObj].pRead.tReceiveTime.l1ms >= arrMsgObj[bBusNumber][bMsgObj].lastCloseTimeArr )
      && ( iResult != EOF ) )
    {
      arrMsgObj[bBusNumber][bMsgObj].pRead.bDlc = iResult;


      //printf("vorabgelesene Zeile in BIOS Funktion:\n%s\n", zeile);
      break;
    }
    else printf( "ignore old message from file with timestamp %u with last close time %i\n",
                  arrMsgObj[bBusNumber][bMsgObj].pRead.tReceiveTime.l1ms, arrMsgObj[bBusNumber][bMsgObj].lastCloseTimeArr);
    if ( feof(arrMsgObj[bBusNumber][bMsgObj].can_input) != 0 )
    { // if actual time after last message in this file, set the "dateiende" flag
      printf("in getCanMsg die letzte Zeile geliefert, keine neue Zeile in Buffer gelesen, da Dateiende erreicht\n");
      arrMsgObj[bBusNumber][bMsgObj].dateiende = 1;
      break;
    }
  }
}

int16_t configCanObj ( uint8_t bBusNumber, uint8_t bMsgObj, tCanObjConfig * ptConfig )
{
  char name[50];
  char zeile[100];

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  // add a new element in the vector (simply resize with old size + 1)
  arrMsgObj[bBusNumber].resize(arrMsgObj[bBusNumber].size() + 1);
  bMsgObj = arrMsgObj[bBusNumber].size()-1;

  arrMsgObj[bBusNumber][bMsgObj].pOpen = arrMsgObj[bBusNumber][bMsgObj].pSendOpen = arrMsgObj[bBusNumber][bMsgObj].lastCloseTimeArr = 0;
#endif

  arrMsgObj[bBusNumber][bMsgObj].pb_bXtd = ptConfig->bXtd;


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

    if ( arrMsgObj[bBusNumber][bMsgObj].pSendOpen != 1 )
    {
      arrMsgObj[bBusNumber][bMsgObj].pSendOpen = 1;
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

    arrMsgObj[bBusNumber][bMsgObj].can_input = fopen(name, "r");
    // BEGIN: Added by M.Wodok 6.12.04
    if (arrMsgObj[bBusNumber][bMsgObj].can_input == NULL) {
      sprintf(name, "can_%hx_%x_", bBusNumber, ptConfig->dwId);
      if (ptConfig->bXtd) strcat(name, "ext");
      else strcat(name, "std");
      // when the needed simulated input file does not exist at standard place, we try locally
      // at the execution directory - here we open with creation mode, so that the program doesn't abort
      arrMsgObj[bBusNumber][bMsgObj].can_input = fopen(name, "w+");
    }
    // END: Added by M.Wodok 6.12.04

    printf("Versuch Datei mit Name %s zum lesen zu oeffnen\n", name);

    arrMsgObj[bBusNumber][bMsgObj].dateiende = 0;
    arrMsgObj[bBusNumber][bMsgObj].pRead.bXtd = ptConfig->bXtd;
    arrMsgObj[bBusNumber][bMsgObj].can_irq = ptConfig->pfIrqFunction;
    arrMsgObj[bBusNumber][bMsgObj].pOpen = 1;

    /* erste Zeile einlesen */
    scanCanMsgLine( bBusNumber, bMsgObj );
  }
  return HAL_NO_ERR;
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
  arrMsgObj[bBusNumber][bMsgObj].b_canBufferLock = false;
  if ((bMsgObj < 2) && ( arrMsgObj[bBusNumber][bMsgObj].pSendOpen == 1))
  {
    fclose(can_output[bBusNumber][bMsgObj]);
    arrMsgObj[bBusNumber][bMsgObj].pSendOpen = 0;
  }
  else
  {
    if (arrMsgObj[bBusNumber][bMsgObj].pOpen == 1)
    {
      fclose(arrMsgObj[bBusNumber][bMsgObj].can_input);
      arrMsgObj[bBusNumber][bMsgObj].pOpen = 0;
    }
  }

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
	arrMsgObj[bBusNumber].erase(arrMsgObj[bBusNumber].begin()+bMsgObj);
#endif

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

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
  /** call the IRQ function */
  __HAL::tCanMsgReg tCanregister;
  __HAL::tCanMsgReg* retValue;

  retValue = __HAL::IRQ_TriggerSend(bBusNumber,bMsgObj,&tCanregister);
#endif
#endif

  if (ptSend->bXtd)
  { // extended Ident_c mit Ident_c Laenge 5
    fprintf(can_output[bBusNumber][bMsgObj],
      "%05d %-8x  %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx",
      i32_sendTimestamp, ptSend->dwId,
      data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    printf(
      "test 1 - %05d %-8x  %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx",
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
      "test 2 - %05d %-8x  %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx",
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

  if (getTime() < arrMsgObj[bBusNumber][bMsgObj].pRead.tReceiveTime.l1ms)
  {
    /* zeit noch nicht erreicht */
    return 0;
  }
  else
  {
    i32_lastReceiveTime = getTime();
    ptReceive->dwId = arrMsgObj[bBusNumber][bMsgObj].pRead.dwId;
    memcpy(ptReceive->abData, arrMsgObj[bBusNumber][bMsgObj].pRead.abData, 8);
    ptReceive->bDlc = arrMsgObj[bBusNumber][bMsgObj].pRead.bDlc = 8;
    ptReceive->tReceiveTime.l1ms = arrMsgObj[bBusNumber][bMsgObj].pRead.tReceiveTime.l1ms ;
    ptReceive->bXtd = arrMsgObj[bBusNumber][bMsgObj].pb_bXtd;
    arrMsgObj[bBusNumber][bMsgObj].lastCloseTimeArr = arrMsgObj[bBusNumber][bMsgObj].pRead.tReceiveTime.l1ms;

    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
       __HAL::tCanMsgReg* retValue;
      __HAL::tCanMsgReg irqInValue;

        irqInValue.tArbit.dw = ptReceive->dwId; /** extended or standard **/
        irqInValue.tMessageCtrl.w = arrMsgObj[bBusNumber][bMsgObj].pb_bXtd;
        irqInValue.tCfg_D0.b[0] = ptReceive->bDlc;
        irqInValue.tCfg_D0.b[1] = ptReceive->abData[0];
        irqInValue.tD1_D4.b[0] = ptReceive->abData[1];
        irqInValue.tD1_D4.b[1] = ptReceive->abData[2];
        irqInValue.tD1_D4.b[2] = ptReceive->abData[3];
        irqInValue.tD5_D7.b[0] = ptReceive->abData[4];
        irqInValue.tD5_D7.b[1] = ptReceive->abData[5];
        irqInValue.tD5_D7.b[2] = ptReceive->abData[6];
        irqInValue.tD5_D7.b[3] = ptReceive->abData[7];

/** the input of the IwriteCentralCanfifo is the CAN bus number which start from 1, -> we have to +1 make **/
      retValue =  IwriteCentralCanfifo(bBusNumber, bMsgObj+1, &irqInValue);
      #endif

    //printf("in getCanMsg len rec %d und dat %d\n",ptReceive->bDlc, arrMsgObj[bBusNumber][bMsgObj].pRead.bDlc);

    /* naechste Zeile einlesen */
    scanCanMsgLine( bBusNumber, bMsgObj );
  }
  return HAL_NO_ERR;
};


int16_t checkMsg()
{

printf("checkMsg\n");

  int16_t i = 0, j = 0;
  int16_t result = 0;
  int16_t empfangen = 0;
  int16_t flag = 1;
  bool b_received[cui32_maxCanBusCnt][15];
  for ( unsigned int indBus = 0; indBus < cui32_maxCanBusCnt; indBus++ )
  {
    for ( unsigned int indObj = 0; indObj < 15; indObj++ )
    {
      b_received[indBus][indObj] = false;
    }
  }
  while (flag == 1)
  {
    result = i = j = 0;
    flag = 0;
    for (j = 0; j < 2; j++)
    {
        for (i=0; i < 15; i++)
        {
          if ((arrMsgObj[j][i].pOpen == 1) && (feof(arrMsgObj[j][i].can_input) == 0))
          {
            /* dateiende noch nicht erreicht */
            result = 1;
          }
          else if ((arrMsgObj[j][i].pOpen == 1) && (feof(arrMsgObj[j][i].can_input) != 0))
          {
            if (arrMsgObj[j][i].dateiende == 0)
            {
              printf("Dateiende von Bus %d und Objekt %d erreicht\n", j,i);
              arrMsgObj[j][i].dateiende = 1;
            }
          }
          if ((result == 1)&&(arrMsgObj[j][i].pOpen == 1) && (arrMsgObj[j][i].pRead.tReceiveTime.l1ms < getTime()) && (arrMsgObj[j][i].dateiende == 0))
          {
            /* zeitstempel ist erreicht -> verarbeiten */
            if ( arrMsgObj[j][i].can_irq != NULL )
            {
              if ( arrMsgObj[j][i].b_canBufferLock )
              { // don't even check this MsgObj as it shall not receive messages
                // -> read next line as a message with current timestamp shall be ignored from file
                printf( "ignore message at BUS %d and Object %d with timestamp %d, as this message object is currently locked\n",
                  j, i, arrMsgObj[j][i].pRead.tReceiveTime.l1ms);
                scanCanMsgLine( j, i );
                continue;
              }
              printf("rufe irq fun fuer BUS %d und Objekt %d auf bei zeit %d und CAN-Zeit %d\n", j, i, getTime(), arrMsgObj[j][i].pRead.tReceiveTime.l1ms);
              *(arrMsgObj[j][i].can_irq)(j,i,NULL);
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
