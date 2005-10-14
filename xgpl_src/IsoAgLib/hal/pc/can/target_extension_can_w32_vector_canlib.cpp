/***************************************************************************
                          target_extension_can_w32_vector_canlib.cpp - source for the PSEUDO BIOS for
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

/** @todo Thanks for any user who has this card type, and can try to compile
    and run with USE_THREAD defined, so that USE_THREAD can be changed to default
  */
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
// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );


// CAN Globals
int AllCanChannelCount = 0;

VportHandle gPortHandle[cui32_maxCanBusCnt];
Vaccess gChannelMask[cui32_maxCanBusCnt];
Vaccess gPermissionMask[cui32_maxCanBusCnt];
Vaccess gInitMask[cui32_maxCanBusCnt];

Vevent  gEvent;
Vevent*  gpEvent;
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

static can_data* rec_buf[cui32_maxCanBusCnt][15];
static int32_t rec_bufCnt[cui32_maxCanBusCnt][15];
static int32_t rec_bufSize[cui32_maxCanBusCnt][15];
static int32_t rec_bufOut[cui32_maxCanBusCnt][15];
static int32_t rec_bufIn[cui32_maxCanBusCnt][15];
static int32_t rec_bufXtd[cui32_maxCanBusCnt][15];
static uint32_t rec_bufFilter[cui32_maxCanBusCnt][15];
static bool b_canBufferLock[cui32_maxCanBusCnt][15];
int32_t i32_lastReceiveTime;

static uint16_t ui16_globalMask[cui32_maxCanBusCnt];
static uint32_t ui32_globalMask[cui32_maxCanBusCnt];
static uint32_t ui32_lastMask[cui32_maxCanBusCnt];
static bool b_busOpened[cui32_maxCanBusCnt];

// globale Filehandle fuer simulierte CAN Input
static FILE* canlogDat[cui32_maxCanBusCnt];

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
    // checkMsg() sets the b_blockApp
    // flag as soon as the buffers are written
    checkMsg();
  }
  return 0;
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
  vErr = ncdOpenDriver();
  if (vErr) return HAL_CONFIG_ERR;
  uint32_t ind;
  for ( ind = 0; ind < cui32_maxCanBusCnt; ind++ )
  {
    gPortHandle[ind] = INVALID_PORTHANDLE;
    gChannelMask[ind]    = 0;
    gPermissionMask[ind] = 0;
    gInitMask[ind]       = 0;
    b_busOpened[ind]             = false;
    canlogDat[ind]               = NULL;
    for (uint8_t ui8_nr = 0; ui8_nr < 15; ui8_nr++)
    {
      rec_bufSize[ind][ui8_nr] = 0;
      rec_bufCnt[ind][ui8_nr] = 0;
    }
  }

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
  for ( ind = 0; ind < cui32_maxCanBusCnt; ind++ )
  {
    vErr = ncdSetNotification(gPortHandle[ind], (unsigned long*)&gEventHandle, 1);
  }
  if (vErr) return HAL_CONFIG_ERR;

  // create a thread
  threadHandle = CreateThread(0,0x1000,thread,0,0,&threadId);
  SetThreadPriority(threadHandle,THREAD_PRIORITY_NORMAL/*THREAD_PRIORITY_TIME_CRITICAL*/);
  #endif

  if (vErr) goto error;

  return HAL_NO_ERR;

  error:
    printf("ERROR: %s!\n", ncdGetErrorString(vErr));
    return HAL_CONFIG_ERR;
}
int16_t can_stopDriver()
{
  #ifdef USE_THREAD
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
  }
  // set gThreadRunning to false so that the thread stops
  gThreadRunning = false;
  b_blockThread = true;
  #endif
  ncdCloseDriver();
  for ( uint32_t ind = 0; ind < cui32_maxCanBusCnt; ind++ )
  {
    if (canlogDat[ind] != NULL){
      fclose(canlogDat[ind]);
      canlogDat[ind] = NULL;
    }
  }

  return HAL_NO_ERR;
}

/** get last timestamp of CAN receive */
int32_t can_lastReceiveTime()
{
  #ifndef USE_THREAD
  checkMsg();
  #else
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
  }
  #endif
  return i32_lastReceiveTime;
}


int16_t getCanMsgBufCount(uint8_t bBusNumber,uint8_t bMsgObj)
{
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
  #ifndef USE_THREAD
  checkMsg();
  #else
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
  }
  #endif
  return ((bBusNumber < cui32_maxCanBusCnt)&&(bMsgObj < 15))?rec_bufCnt[bBusNumber][bMsgObj]:0;
};


bool waitUntilCanReceiveOrTimeout( uint16_t rui16_timeoutInterval )
{
  const int32_t ci32_endWait = getTime() + rui16_timeoutInterval;
  int32_t i32_waitSlice rui16_timeoutInterval;

  // if greater than 50msec -> divide so that about 10 slices are realized
  if ( i32_waitSlice > 50 ) i32_waitSlice /= 10;
  // if still more than 50msec slice limit to 50
  if ( i32_waitSlice > 50 ) i32_waitSlice = 50;

  while (true)
  {
    #ifndef USE_THREAD
    checkMsg();
    #else
    // wait until the receive thread allows access to buffer
    while ( b_blockApp )
    { // do something for 1msec - just to take time
      Sleep( i32_waitSlice );
    }
    #endif
    for ( unsigned int busInd = 0; busInd < cui32_maxCanBusCnt; busInd++)
    {
      if ( !b_busOpened[busInd] ) continue;
      for ( unsigned int msgInd = 0; msgInd < 15; msgInd++ )
      {
        if ( rec_bufCnt[busInd][msgInd] > 0 ) return true;
      }
    }
    #ifndef USE_THREAD
    Sleep( i32_waitSlice );
    #endif
    if ( getTime() >= ci32_endWait ) return false;
  }
}

/**
  check if MsgObj is currently locked
  @param rui8_busNr number of the BUS to check
  @param rui8_msgobjNr number of the MsgObj to check
  @return true -> MsgObj is currently locked
*/
bool getCanMsgObjLocked( uint8_t rui8_busNr, uint8_t rui8_msgobjNr )
{
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return true;
  else if ( b_canBufferLock[rui8_busNr][rui8_msgobjNr] ) return true;
  else return false;
}

int16_t init_can ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg,uint16_t wBitrate )
{
  if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) return HAL_RANGE_ERR;
  int i,n;
  int32_t i32_busInd = -1, i32_virtualBusInd = -1;
  Vstatus vErr;
  Vaccess virtualChannelMask = 0;
  #ifdef USE_THREAD
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
  }
  // tell thread to wait until this function is finished
  b_blockThread = true;
  #endif
  // Added by M.Wodok 6.12.04
  canlogDat[bBusNumber] = fopen("..\\..\\..\\..\\simulated_io\\can_send.txt", "w+");
  if(canlogDat[bBusNumber])
  {
    printf("canlogDat file opened: '..\\..\\..\\..\\simulated_io\\can_send.txt'\n");
  }
  else
  {
    // try in current directory...
    canlogDat[bBusNumber] = fopen("can_send.txt", "w+");
    if(canlogDat[bBusNumber])
    {
      printf("canlogDat file opened: 'can_send.txt'\n");
    }
    else
    {
      printf("canlogDat file FAILED to open! Error Code = %d\n", canlogDat[bBusNumber]);
    }
  }

  // select the wanted channels
  gChannelMask[bBusNumber] = gInitMask[bBusNumber] = 0;
  i32_busInd = -1;
  for (i=0; i<AllCanChannelCount; i++) {
    if ( ( gDriverConfig->channel[i].hwType==gHwType                                           )
      || ( ( gHwType == HWTYPE_AUTO ) && ( gDriverConfig->channel[i].hwType > HWTYPE_VIRTUAL ) ) )
    {
      i32_busInd++;
      printf( "Detect Real Channel %d\n", i32_busInd );
      if ( bBusNumber == i32_busInd )
      { // BUS found
        gChannelMask[bBusNumber] |= gDriverConfig->channel[i].channelMask;
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
    gChannelMask[bBusNumber] = virtualChannelMask;
  }

  gInitMask[bBusNumber] = gChannelMask[bBusNumber];

  // open a port
  printf("ncdOpenPort(channelMask=%04X, initMask=%04X)\n", gChannelMask[bBusNumber], gInitMask[bBusNumber]);
  vErr = ncdOpenPort(&gPortHandle[bBusNumber], "IsoAgLib", gChannelMask[bBusNumber], gInitMask[bBusNumber], &gPermissionMask[bBusNumber], 1024);
  if (vErr) goto error;
  printf(" portHandle=%u\n", gPortHandle[bBusNumber]);
  if (gPortHandle[bBusNumber]==INVALID_PORTHANDLE) goto error;
  printf(" permissionMask=%04X\n", gPermissionMask[bBusNumber]);

  // if permision to init
  if (gPermissionMask[bBusNumber]) {
    // set BUS timing
    vErr = ncdSetChannelBitrate(gPortHandle[bBusNumber], gPermissionMask[bBusNumber], (wBitrate * 1000));
    if (vErr) goto error;
  }
  else if (wBitrate) {
    printf("WARNING: No init access, bitrate ignored!\n");
  }
  ui16_globalMask[bBusNumber] = wGlobMask;
  ui32_globalMask[bBusNumber] = dwGlobMask;
  ui32_lastMask[bBusNumber] = dwGlobMaskLastmsg;

  // Disable the TX and TXRQ notifications
  vErr = ncdSetChannelMode(gPortHandle[bBusNumber],gChannelMask[bBusNumber],0,0);
  if (vErr) goto error;


  if (b_busOpened[bBusNumber] == false)
  { // set global mask
    VsetAcceptance acc;
    acc.mask = 0x000;
    acc.code = 0x000;
    vErr = ncdSetChannelAcceptance(gPortHandle[bBusNumber], gChannelMask[bBusNumber], &acc);
    if (vErr) goto error;
    acc.mask = 0x80000000;
    acc.code = 0x80000000;
    vErr = ncdSetChannelAcceptance(gPortHandle[bBusNumber], gChannelMask[bBusNumber], &acc);
    if (vErr) goto error;

    // reset clock
    vErr = ncdResetClock(gPortHandle[bBusNumber]);
    if (vErr) goto error;
    // Go on bus
    vErr = ncdActivateChannel(gPortHandle[bBusNumber],gChannelMask[bBusNumber]);
    if (vErr) goto error;

  }
  b_busOpened[bBusNumber] = true;
  #ifdef USE_THREAD
  b_blockThread = false;
  #endif

  return HAL_NO_ERR;

  error:
    printf("ERROR: %s!\n", ncdGetErrorString(vErr));

    if (gPortHandle[bBusNumber] != INVALID_PORTHANDLE) {
      ncdClosePort(gPortHandle[bBusNumber]);
      gPortHandle[bBusNumber] = INVALID_PORTHANDLE;
    }

    b_busOpened[bBusNumber] = false;
    #ifdef USE_THREAD
    b_blockThread = false;
    #endif
    return HAL_RANGE_ERR;
};

int16_t changeGlobalMask ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg)
{
  if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) return HAL_RANGE_ERR;

  #ifdef USE_THREAD
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
  }
  // tell thread to wait until this function is finished
  b_blockThread = true;
  #endif

  ui16_globalMask[bBusNumber] = wGlobMask;
  ui32_globalMask[bBusNumber] = dwGlobMask;
  ui32_lastMask[bBusNumber] = dwGlobMaskLastmsg;

  #ifdef USE_THREAD
  b_blockThread = false;
  #endif

  return HAL_NO_ERR;
};


int16_t closeCan ( uint8_t bBusNumber )
{
  if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) return HAL_RANGE_ERR;
  #ifdef USE_THREAD
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
  }
  // tell thread to wait until this function is finished
  b_blockThread = true;
  #endif
  if (canlogDat[bBusNumber] != NULL){
    fclose(canlogDat[bBusNumber]);
    canlogDat[bBusNumber] = NULL;
  }
  ncdDeactivateChannel(gPortHandle[bBusNumber], gChannelMask[bBusNumber]);
  ncdClosePort(gPortHandle[bBusNumber]);
  gPortHandle[bBusNumber] = INVALID_PORTHANDLE;
  b_busOpened[bBusNumber] = false;
  #ifdef USE_THREAD
  b_blockThread = false;
  #endif
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
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
  #ifdef USE_THREAD
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
  }
  // tell thread to wait until this function is finished
  b_blockThread = true;
  #endif
  if (rec_bufCnt[bBusNumber][bMsgObj] == -1)
  { // it's a send object -> call native clear transmit
    ncdFlushTransmitQueue(gPortHandle[bBusNumber], gChannelMask[bBusNumber]);
  }
  else
  { // set receive buffer to 0
    rec_bufCnt[bBusNumber][bMsgObj] = 0;
    rec_bufOut[bBusNumber][bMsgObj] = 0;
    rec_bufIn[bBusNumber][bMsgObj] = 0;
  }
  #ifdef USE_THREAD
  b_blockThread = false;
  #endif
  return HAL_NO_ERR;
}

int16_t configCanObj ( uint8_t bBusNumber, uint8_t bMsgObj, tCanObjConfig * ptConfig )
{
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
  #ifdef USE_THREAD
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
  }
  // tell thread to wait until this function is finished
  b_blockThread = true;
  #endif
  b_canBufferLock[bBusNumber][bMsgObj] = false;
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
  #ifdef USE_THREAD
  b_blockThread = false;
  #endif
  return HAL_NO_ERR;
};

int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint8_t bXtd )
{
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
  #ifdef USE_THREAD
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
  }
  // tell thread to wait until this function is finished
  b_blockThread = true;
  #endif
  b_canBufferLock[bBusNumber][bMsgObj] = false;
  if (rec_bufSize[bBusNumber][bMsgObj] > -1)
  { // active receive object
    rec_bufFilter[bBusNumber][bMsgObj] = dwId;
    rec_bufXtd[bBusNumber][bMsgObj] = bXtd;
  }
  #ifdef USE_THREAD
  b_blockThread = false;
  #endif
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
  if ( ( rui8_busNr > HAL_CAN_MAX_BUS_NR ) || ( rui8_msgobjNr > 14 ) ) return HAL_RANGE_ERR;
  #ifdef USE_THREAD
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
  }
  // tell thread to wait until this function is finished
  b_blockThread = true;
  #else
  checkMsg();
  #endif
  b_canBufferLock[rui8_busNr][rui8_msgobjNr] = rb_doLock;
  #ifdef USE_THREAD
  b_blockThread = false;
  #endif
  return HAL_NO_ERR;
}

int16_t closeCanObj ( uint8_t bBusNumber,uint8_t bMsgObj )
{
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
  #ifdef USE_THREAD
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
  }
  // tell thread to wait until this function is finished
  b_blockThread = true;
  #endif
  b_canBufferLock[bBusNumber][bMsgObj] = false;
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
  #ifdef USE_THREAD
  b_blockThread = false;
  #endif
  return HAL_NO_ERR;
};

int16_t sendCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tSend * ptSend )
{
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
  VportHandle lPortHandle = gPortHandle[bBusNumber];
  Vaccess lChannelMask = gChannelMask[bBusNumber];
  Vaccess lPermissionMask = gPermissionMask[bBusNumber];

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
      fprintf(canlogDat[bBusNumber], "Sende: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptSend->dwId,
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
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
  #ifdef USE_THREAD
  // wait until the receive thread allows access to buffer
  while ( b_blockApp )
  { // do something for 1msec - just to take time
    Sleep( 100 );
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

      fprintf(canlogDat[bBusNumber], "Empfang: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptReceive->dwId,
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

  for (uint32_t b_bus = 0; b_bus < cui32_maxCanBusCnt; b_bus++)
  { // if b_bus is not open --> immediately try next bus
    if ( !b_busOpened[b_bus] ) continue;
    // try to receive a message
    for(vErr = ncdReceive1(gPortHandle[b_bus], &gpEvent); vErr == VSUCCESS; vErr = ncdReceive1(gPortHandle[b_bus], &gpEvent))
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
        fprintf(canlogDat[b_bus], "!!Received of malformed message with undefined CAN ident: %x\n", ui32_id);
        continue;
      }
      #ifdef USE_THREAD
      // block access from application on the buffers, as long as
      // the current CAN message is placed into one of the buffers
      b_blockApp = true;
      #endif
      // now search for MsgObj queue on this b_bus, where new message from b_bus maps
      for (int32_t i32_obj = 1; i32_obj < 15; i32_obj++)
      { // compare received msg with filter
        int32_t i32_in;
        can_data* pc_data;
        if ( b_canBufferLock[b_bus][i32_obj] )
        { // don't even check this MsgObj as it shall not receive messages
          continue;
        }
        if
          (
            (
              ( i32_obj < 14 )
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
              ( i32_obj == 14 )
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
      #ifdef USE_THREAD
      // un-block access from application on the buffers, as
      // the current buffers are again free for access
      b_blockApp = false;
      #endif
    } // for bus
  } // for receive msg from CANcardX
  return result;
}

} // end namespace __HAL
