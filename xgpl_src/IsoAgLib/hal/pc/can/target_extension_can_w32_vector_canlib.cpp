/***************************************************************************
                          target_extension_can_w32_vector_canlib.cc - source for the PSEUDO BIOS for
                                       development and test on a Win32 PC
                                       with CAN communication through Vector-Informatik
                                       CANcardX
                             -------------------
    begin                : Tue Oct 2 2001
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
#define  STRICT


#include "can_target_extensions.h"
#include <stdlib.h>
#include <ctype.h>
#include <IsoAgLib/hal/system.h>

// #define USE_THREAD

#include "string.h"
#include "stdio.h"
#include <windows.h>
#include "dos.h"

#define HWTYPE_AUTO 1000

extern "C" {
  #include <VCanD.h>
}

namespace __HAL {
/////////////////////////////////////////////////////////////////////////
// Globals
HANDLE       gEventHandle    = 0;


// CAN Globals
VportHandle gPortHandle = INVALID_PORTHANDLE;
int AllCanChannelCount = 0;
Vaccess gChannelMask = 0;
Vaccess gPermissionMask = 0;
Vaccess gInitMask            = 0;
Vevent  gEvent;
Vevent*  gpEvent;
int32_t gChannel = 0;
VDriverConfig *gDriverConfig = 0;

#ifdef USE_CAN_CARD_TYPE
uint32_t gHwType = USE_CAN_CARD_TYPE;
#else
// select the Vector CAN card type to use
uint32_t gHwType = HWTYPE_AUTO;
#endif

struct can_data {
  int32_t i32_time;
  int32_t i32_ident;
  uint8_t b_dlc;
  uint8_t b_xtd;
  uint8_t pb_data[8];
};

static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 - HAL_CAN_MIN_BUS_NR );
static can_data* rec_buf[cui32_maxCanBusCnt][16];
static int32_t rec_bufCnt[cui32_maxCanBusCnt][16];
static int32_t rec_bufSize[cui32_maxCanBusCnt][16];
static int32_t rec_bufOut[cui32_maxCanBusCnt][16];
static int32_t rec_bufIn[cui32_maxCanBusCnt][16];
static bool rec_bufXtd[cui32_maxCanBusCnt][16];
static uint32_t rec_bufFilter[cui32_maxCanBusCnt][16];
int32_t i32_lastReceiveTime;

static uint16_t ui16_globalMask[cui32_maxCanBusCnt];
static uint32_t ui32_globalMask[cui32_maxCanBusCnt];
static uint32_t ui32_lastMask[cui32_maxCanBusCnt];
static bool b_busOpened[cui32_maxCanBusCnt];

// globale Filehandle fuer simulierte CAN Input
static FILE* canlogDat;

#ifdef USE_THREAD
/** flag to control running thread */
static bool gThreadRunning;
/** the thread is only allowed to perform activities, if this var is false */
static bool b_blockThread;
/** the app must wait until this var is false, before it can safely take a message from buffer */
static bool b_blockApp;

/** handle for threading */
HANDLE threadHandle;
DWORD threadId               = 0;

DWORD WINAPI thread( PVOID par )
{
	while( gThreadRunning )
	{
		if ( b_blockThread )
		{ // application is accesing buffer or another action of app
			// is running where problems can be caused by parallel access from
			// this thread
			continue;
		}
		WaitForSingleObject( gEventHandle, INFINITE );
		// now check for received messages
		b_blockApp = true;
		checkMsg();
		b_blockApp = false;
	}
}

#endif

static void printDriverConfig( void ) {
  int i;
  printf("Driver Configuration:\n  ChannelCount=%u\n", gDriverConfig->channelCount);
  for (i=0; i<gDriverConfig->channelCount; i++) {
    printf("  Channel %u (%04X): %s, (%u,%u,%u), isOnBus=%u\n",
      gDriverConfig->channel[i].channelIndex,
      gDriverConfig->channel[i].channelMask,
      gDriverConfig->channel[i].name,
      gDriverConfig->channel[i].hwIndex,
      gDriverConfig->channel[i].hwType,
      gDriverConfig->channel[i].hwChannel,
      gDriverConfig->channel[i].isOnBus
    );
  }
}

int16_t can_startDriver()
{
	#ifdef USE_CAN_CARD_TYPE
	gHwType = USE_CAN_CARD_TYPE;
	#else
	// select the Vector CAN card type to use
	gHwType = HWTYPE_AUTO;
	#endif
  // open the driver
  Vstatus vErr;
  canlogDat = NULL;
	vErr = ncdOpenDriver();
  if (vErr) return HAL_CONFIG_ERR;
  b_busOpened[0] = false;
  b_busOpened[1] = false;

  printf("ncdDriverConfig()\n");
  vErr = ncdGetDriverConfig(&AllCanChannelCount, NULL); // get the number of channels
  if (vErr) return HAL_CONFIG_ERR;
  printf(" %u channels found\n",AllCanChannelCount);
  gDriverConfig = (VDriverConfig*)malloc(AllCanChannelCount*sizeof(VDriverConfig));
  if (!gDriverConfig) return HAL_CONFIG_ERR;
  vErr = ncdGetDriverConfig(&AllCanChannelCount, gDriverConfig); // get information about all channels
  if (vErr) return HAL_CONFIG_ERR;
  printDriverConfig();

	#ifdef USE_THREAD
	/** flag to control running thread */
	gThreadRunning = true;
	/** the thread is only allowed to perform activities, if this var is false */
	b_blockThread = true;
	/** the app must wait until this var is false, before it can safely take a message from buffer */
	b_blockApp = false;

  // create a synchronisation object
  gEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
  vErr = ncdSetNotification(gPortHandle, (unsigned long*)&gEventHandle, 1);
  if (vErr) return HAL_CONFIG_ERR;

	// create a thread
  threadHandle = CreateThread(0,0x1000,thread,0,0,&threadId);
  SetThreadPriority(threadHandle,THREAD_PRIORITY_NORMAL/*THREAD_PRIORITY_TIME_CRITICAL*/);
	#endif

  for (uint8_t b_bus = 0; b_bus < cui32_maxCanBusCnt; b_bus++)
  {
    for (uint8_t ui8_nr = 0; ui8_nr < 16; ui8_nr++)
    {
      rec_bufSize[b_bus][ui8_nr] = 0;
      rec_bufCnt[b_bus][ui8_nr] = 0;
    }
  }
  if (vErr) goto error;

  return HAL_NO_ERR;

  error:
    printf("ERROR: %s!\n", ncdGetErrorString(vErr));
    return HAL_CONFIG_ERR;
}
int16_t can_stopDriver()
{
	#ifdef USE_THREAD
	// set gThreadRunning to false so that the thread stops
	gThreadRunning = false;
	b_blockThread = true;
	#endif
  ncdCloseDriver();
    if (canlogDat != NULL){
    fclose(canlogDat);
    canlogDat = NULL;
  }
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
  checkMsg();
  return ((bBusNumber < cui32_maxCanBusCnt)&&(bMsgObj < 16))?rec_bufCnt[bBusNumber][bMsgObj]:0;
};


int16_t init_can ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg,uint16_t wBitrate )
{
  int i,n;
  int32_t i32_busInd = -1, i32_virtualBusInd = -1;
  Vstatus vErr;
  VportHandle* pgPortHandle = &gPortHandle;
  Vaccess* pgChannelMask = &gChannelMask;
  Vaccess* pgPermissionMask = &gPermissionMask;


	Vaccess virtualChannelMask = 0;

  canlogDat = fopen("..\\..\\..\\..\\simulated_io\\can_send.txt", "w+");
  if(canlogDat)
  {
	printf("canlogDat file opened\n");
  }
  else
  {
	printf("canlogDat file FAILED to open! Error Code = %d\n", canlogDat);
  }

  // select the wanted channels
  gChannelMask = gInitMask = 0;
  i32_busInd = -1;
  for (i=0; i<AllCanChannelCount; i++) {
    if ( ( gDriverConfig->channel[i].hwType==gHwType                                           )
			|| ( ( gHwType == HWTYPE_AUTO ) && ( gDriverConfig->channel[i].hwType > HWTYPE_VIRTUAL ) ) )
		{
			i32_busInd++;
			printf( "Detect Real Channel %d\n", i32_busInd );
			if ( bBusNumber == i32_busInd )
			{ // BUS found
				gChannelMask |= gDriverConfig->channel[i].channelMask;
			}
		}
		else if ( gDriverConfig->channel[i].hwType == HWTYPE_VIRTUAL )
		{
			i32_virtualBusInd++;
			printf( "Detect Virtual Channel %d\n", i32_virtualBusInd );
			if ( bBusNumber == i32_virtualBusInd )
			{ // BUS found
				virtualChannelMask |= gDriverConfig->channel[i].channelMask;
			}
		}
  }

	// if AUTO HW detection is wanted, and only virtual channels are found
	// use virtualChannelMask
	if ( ( gHwType == HWTYPE_AUTO ) && ( i32_busInd == -1 ) )
	{ // no real CAN channels found
		gChannelMask = virtualChannelMask;
	}

  gInitMask = gChannelMask;

  // open a port
  printf("ncdOpenPort(channelMask=%04X, initMask=%04X)\n", gChannelMask, gInitMask);
  vErr = ncdOpenPort(&gPortHandle, "IsoAgLib", gChannelMask, gInitMask, &gPermissionMask, 1024);
  if (vErr) goto error;
  printf(" portHandle=%u\n", gPortHandle);
  if (gPortHandle==INVALID_PORTHANDLE) goto error;
  printf(" permissionMask=%04X\n", gPermissionMask);
  
  // if permision to init
  if (gPermissionMask) {
    // set BUS timing
    vErr = ncdSetChannelBitrate(gPortHandle, gPermissionMask, (wBitrate * 1000));
    if (vErr) goto error;
  }
  else if (wBitrate) {
    printf("WARNING: No init access, bitrate ignored!\n");
  }
  ui32_globalMask[bBusNumber] = wGlobMask;
  ui32_globalMask[bBusNumber] = dwGlobMask;
	ui32_lastMask[bBusNumber] = dwGlobMaskLastmsg;

  // Disable the TX and TXRQ notifications
  vErr = ncdSetChannelMode(gPortHandle,gChannelMask,0,0);
  if (vErr) goto error;


  if (b_busOpened[bBusNumber] == false)
  { // set global mask
    VsetAcceptance acc;
    acc.mask = 0x000;
    acc.code = 0x000;
    vErr = ncdSetChannelAcceptance(gPortHandle, gChannelMask, &acc);
    if (vErr) goto error;
    acc.mask = 0x80000000;
    acc.code = 0x80000000;
    vErr = ncdSetChannelAcceptance(gPortHandle, gChannelMask, &acc);
    if (vErr) goto error;

    // reset clock
    vErr = ncdResetClock(gPortHandle);
    if (vErr) goto error;
    // Go on bus
    vErr = ncdActivateChannel(gPortHandle,gChannelMask);
    if (vErr) goto error;

  }
  b_busOpened[bBusNumber] = true;
	#ifdef USE_THREAD
	b_blockThread = false;
	#endif

  return HAL_NO_ERR;

  error:
    printf("ERROR: %s!\n", ncdGetErrorString(vErr));

    if (gPortHandle != INVALID_PORTHANDLE) {
      ncdClosePort(gPortHandle);
      gPortHandle = INVALID_PORTHANDLE;
    }

    b_busOpened[bBusNumber] = false;
    return HAL_RANGE_ERR;
};

int16_t closeCan ( uint8_t bBusNumber )
{
  if (canlogDat != NULL){
    fclose(canlogDat);
    canlogDat = NULL;
  }
  ncdDeactivateChannel(gPortHandle, gChannelMask);
  ncdClosePort(gPortHandle);
  gPortHandle = INVALID_PORTHANDLE;
  b_busOpened[bBusNumber] = false;
  return HAL_NO_ERR;
};

int16_t chgCanObjPause ( uint8_t bBusNumber, uint8_t bMsgObj, uint16_t wPause)
{
/*
  printf("sende Pause auf BUS %d fuer CAN Objekt %d auf %d eingestellt\n",
  (int32_t)bBusNumber, (int32_t)bMsgObj, wPause);
  */
return HAL_NO_ERR;
}

int16_t getCanBusStatus(uint8_t bBusNumber, tCanBusStatus* ptStatus)
{
//  printf("getCanBusStatus fuer BUS %d\n", bBusNumber);
  return HAL_NO_ERR;
}

int16_t clearCanObjBuf(uint8_t bBusNumber, uint8_t bMsgObj)
{
  if (rec_bufCnt[bBusNumber][bMsgObj] == -1)
  { // it's a send object -> call native clear transmit
    ncdFlushTransmitQueue(gPortHandle, gChannelMask);
  }
  else
  { // set receive buffer to 0
    rec_bufCnt[bBusNumber][bMsgObj] = 0;
    rec_bufOut[bBusNumber][bMsgObj] = 0;
    rec_bufIn[bBusNumber][bMsgObj] = 0;
  }
  return HAL_NO_ERR;
}

int16_t configCanObj ( uint8_t bBusNumber, uint8_t bMsgObj, tCanObjConfig * ptConfig )
{
  if (ptConfig->bMsgType == TX)
  { /* Sendeobjekt */
    rec_bufSize[bBusNumber][bMsgObj] = -1;
  }
  else
  { // empfangsobjekt
    rec_bufXtd[bBusNumber][bMsgObj] = ptConfig->bXtd;
    rec_buf[bBusNumber][bMsgObj] = new can_data[ptConfig->wNumberMsgs];
    rec_bufSize[bBusNumber][bMsgObj] = ptConfig->wNumberMsgs;
    rec_bufCnt[bBusNumber][bMsgObj] = 0;
    rec_bufOut[bBusNumber][bMsgObj] = 0;
    rec_bufIn[bBusNumber][bMsgObj] = 0;
    rec_bufFilter[bBusNumber][bMsgObj] = ptConfig->dwId;
  }
  return HAL_NO_ERR;
};

int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint8_t bXtd )
{
  if (rec_bufSize[bBusNumber][bMsgObj] > -1)
  { // active receive object
    rec_bufFilter[bBusNumber][bMsgObj] = dwId;
    rec_bufXtd[bBusNumber][bMsgObj] = bXtd;
  }
  return HAL_NO_ERR;
}

int16_t closeCanObj ( uint8_t bBusNumber,uint8_t bMsgObj )
{
  if (rec_bufSize[bBusNumber][bMsgObj] == -1)
  { /* Sendeobjekt */
    rec_bufSize[bBusNumber][bMsgObj] = -1;
  }
  else
  { // empfangsobjekt
    delete [] (rec_buf[bBusNumber][bMsgObj]);
    rec_bufSize[bBusNumber][bMsgObj] = -1;
    rec_bufCnt[bBusNumber][bMsgObj] = 0;
    rec_bufOut[bBusNumber][bMsgObj] = 0;
    rec_bufIn[bBusNumber][bMsgObj] = 0;
  }
  return HAL_NO_ERR;
};

int16_t sendCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tSend * ptSend )
{
  VportHandle lPortHandle = gPortHandle;
  Vaccess lChannelMask = gChannelMask;
  Vaccess lPermissionMask = gPermissionMask;
  Vevent  lEvent = gEvent;


  Vstatus vErr;
  lEvent.tag = V_TRANSMIT_MSG;
  lEvent.tagData.msg.id = ptSend->dwId;

  // set extended type if needed by setting MSB bit
  if (ptSend->bXtd == 1) lEvent.tagData.msg.id |= 0x80000000UL;
  lEvent.tagData.msg.flags = 0;
  lEvent.tagData.msg.dlc = ptSend->bDlc;
  memmove(lEvent.tagData.msg.data, ptSend->abData, ptSend->bDlc);

  if ( ((ptSend->dwId & 0x700) == 0x700)
     ||((ptSend->dwId & 0x7FF) == 0x520)
     ||((ptSend->dwId & 0x7FF) == 0x502)
     ||((ptSend->dwId & 0x700) == 0x200)
      )
  {
    printf("Sende: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptSend->dwId,
      ptSend->abData[0], ptSend->abData[1], ptSend->abData[2],
      ptSend->abData[3], ptSend->abData[4], ptSend->abData[5],
      ptSend->abData[6], ptSend->abData[7]);

      fprintf(canlogDat, "Sende: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptSend->dwId,
      ptSend->abData[0], ptSend->abData[1], ptSend->abData[2],
      ptSend->abData[3], ptSend->abData[4], ptSend->abData[5],
      ptSend->abData[6], ptSend->abData[7]);

    }

  vErr = ncdTransmit(lPortHandle, lChannelMask, &lEvent);

  if (vErr == VERR_QUEUE_IS_FULL)
  {
    return HAL_OVERFLOW_ERR;
  }
  else
  {
    return HAL_NO_ERR;
  }
};

int16_t getCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tReceive * ptReceive )
{
	#ifdef USE_THREAD
	// wait until the receive thread allows access to buffer
	while ( b_blockApp )
	{ // do something for 1msec - just to take time
		WaitForSingleObject( gEventHandle, 100 );
	}
	// tell thread to wait until this function is finished
	b_blockThread = true;
	#endif

	if (rec_bufCnt[bBusNumber][bMsgObj] > 0)
  { // data received
    i32_lastReceiveTime = getTime();
    // get access to data
    int32_t i32_out = rec_bufOut[bBusNumber][bMsgObj];
    can_data* pc_data = &(rec_buf[bBusNumber][bMsgObj][i32_out]);
    // copy data
    ptReceive->dwId = pc_data->i32_ident;
    ptReceive->bDlc = pc_data->b_dlc;
    ptReceive->tReceiveTime.l1ms = pc_data->i32_time;
    ptReceive->bXtd = pc_data->b_xtd;
    memcpy(ptReceive->abData, pc_data->pb_data, pc_data->b_dlc);

  if ( ((ptReceive->dwId & 0x700) == 0x700)
     ||((ptReceive->dwId & 0x7FF) == 0x520)
     ||((ptReceive->dwId & 0x700) == 0x200)
     ||((ptReceive->dwId & 0x7FF) == 0x502)
      )
    {
      printf("Empfang: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptReceive->dwId,
      ptReceive->abData[0], ptReceive->abData[1], ptReceive->abData[2],
      ptReceive->abData[3], ptReceive->abData[4], ptReceive->abData[5],
      ptReceive->abData[6], ptReceive->abData[7]);

      fprintf(canlogDat, "Empfang: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptReceive->dwId,
      ptReceive->abData[0], ptReceive->abData[1], ptReceive->abData[2],
      ptReceive->abData[3], ptReceive->abData[4], ptReceive->abData[5],
      ptReceive->abData[6], ptReceive->abData[7]);
    }


    // update ringpuffer
    rec_bufOut[bBusNumber][bMsgObj] = ((i32_out + 1) % rec_bufSize[bBusNumber][bMsgObj]);
    rec_bufCnt[bBusNumber][bMsgObj] -= 1;
  }
	#ifdef USE_THREAD
	b_blockThread = false;
	#endif
  return HAL_NO_ERR;
};


int16_t checkMsg()
{ // first check if CANcardX buffer has received msg
  Vstatus vErr;

  int32_t result = 0;
  for(vErr = ncdReceive1(gPortHandle, &gpEvent); vErr == VSUCCESS; vErr = ncdReceive1(gPortHandle, &gpEvent))
  { // msg from CANcardX buffer
		// this functions retrurns not only received messages
		// ACK for SENT messages is also returned!!!
		if( ( gpEvent->tag != V_RECEIVE_MSG ) || ( gpEvent->tagData.msg.flags != 0 ) )
		{ // don't further process this message as it is NO received message
			continue;
		}

    result += 1;
    uint8_t b_xtd = (gpEvent->tagData.msg.id > 0x7FFFFFFF)?1:0;
    uint32_t ui32_id = (gpEvent->tagData.msg.id & 0x1FFFFFFF);
    if (ui32_id >= 0x7FFFFFFF)
    {
      printf("!!Received of malformed message with undefined CAN ident: %x\n", ui32_id);
      fprintf(canlogDat, "!!Received of malformed message with undefined CAN ident: %x\n", ui32_id);
      continue;
    }
    ui32_id = (gpEvent->tagData.msg.id & 0xFFFF);
    for (uint8_t b_bus = 0; b_bus < 2; b_bus++)
    {
      if (b_busOpened[b_bus])
      {
        for (int32_t i32_obj = 1; i32_obj < 16; i32_obj++)
        { // compare received msg with filter
          int32_t i32_in;
          can_data* pc_data;
          if
						(
							(
								( i32_obj < 15 )
						&&  (
									( (rec_bufXtd[b_bus][i32_obj] == 1)
									&& (b_xtd == 1)
									&& (rec_bufSize[b_bus][i32_obj] > 0)
									&& ( (ui32_id & ui32_globalMask[b_bus]) ==  ((rec_bufFilter[b_bus][i32_obj]) & ui32_globalMask[b_bus]) )
									)
								|| ( (rec_bufXtd[b_bus][i32_obj] == 0)
									&& (b_xtd == 0)
									&& (rec_bufSize[b_bus][i32_obj] > 0)
									&& ( (ui32_id & ui16_globalMask[b_bus]) ==  (rec_bufFilter[b_bus][i32_obj] & ui16_globalMask[b_bus]) )
									)
								)
							)
					|| (
								( i32_obj == 15 )
						&&  (
									( (rec_bufXtd[b_bus][i32_obj] == 1)
									&& (b_xtd == 1)
									&& (rec_bufSize[b_bus][i32_obj] > 0)
									&& ( (ui32_id & ui32_globalMask[b_bus] & ui32_lastMask[b_bus]) ==  ((rec_bufFilter[b_bus][i32_obj]) & ui32_globalMask[b_bus] & ui32_lastMask[b_bus]) )
									)
								|| ( (rec_bufXtd[b_bus][i32_obj] == 0)
									&& (b_xtd == 0)
									&& (rec_bufSize[b_bus][i32_obj] > 0)
									&& ( (ui32_id & ui16_globalMask[b_bus] & ui32_lastMask[b_bus]) ==  (rec_bufFilter[b_bus][i32_obj] & ui16_globalMask[b_bus] & ui32_lastMask[b_bus]) )
									)
								)
							)
						)
          { // received msg fits actual filter
            i32_in = rec_bufIn[b_bus][i32_obj];
            rec_bufIn[b_bus][i32_obj] = ((i32_in + 1) % rec_bufSize[b_bus][i32_obj]);
            if (rec_bufCnt[b_bus][i32_obj] >= rec_bufSize[b_bus][i32_obj])
            { // overflow -> insert new, and overwrite oldest msg in buffer
              rec_bufOut[b_bus][i32_obj] = rec_bufIn[b_bus][i32_obj];
            }
            else
            {
              rec_bufCnt[b_bus][i32_obj] += 1;
            }
            pc_data = &(rec_buf[b_bus][i32_obj][i32_in]);
            pc_data->i32_time = getTime();
            pc_data->i32_ident = ui32_id;
            pc_data->b_dlc = gpEvent->tagData.msg.dlc;
            pc_data->b_xtd = b_xtd;
            memcpy(pc_data->pb_data, gpEvent->tagData.msg.data,pc_data->b_dlc);
          } // if fit
        } // for objNr
      } // if opened
    } // for bus
  } // for receive msg from CANcardX
  return result;
}

} // end namespace __HAL
