/***************************************************************************
                          targetExtensionCanCardXWin32.cc - source for the PSEUDO BIOS for
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

#define USE_THREAD

#include "string.h"
#include "stdio.h"
#include <windows.h>
#include "dos.h"

extern "C" {
  #include <vxlapi.h>
}

namespace __HAL {
/////////////////////////////////////////////////////////////////////////
// Globals
HANDLE       gEventHandle    = 0;


// CAN Globals
char            g_AppName[XL_MAX_LENGTH+1]  = "IsoAgLib";            //!< Application name which is displayed in VHWconf
XLportHandle    g_xlPortHandle              = XL_INVALID_PORTHANDLE;  //!< Global porthandle (we use only one!)
XLdriverConfig  g_xlDrvConfig;                                        //!< Contains the actual hardware configuration
XLaccess        g_xlChannelMask             = 0;                      //!< Global channelmask (includes all founded channels)
XLaccess        g_xlPermissionMask          = 0;                      //!< Global permissionmask (includes all founded channels)
int16_t gChannel = 0;

#ifdef USE_CAN_CARD_TYPE
uint16_t gHwType = USE_CAN_CARD_TYPE;
#else
// select the Vector CAN card type to use
//
uint16_t gHwType = HWTYPE_CANCARDX;
#endif

struct can_data {
  int32_t i32_time;
  int32_t i32_ident;
  uint8_t b_dlc;
  uint8_t b_xtd;
  uint8_t pb_data[8];
};

static can_data* rec_buf[2][16];
static int16_t rec_bufCnt[2][16];
static int16_t rec_bufSize[2][16];
static int16_t rec_bufOut[2][16];
static int16_t rec_bufIn[2][16];
static bool rec_bufXtd[2][16];
static uint32_t rec_bufFilter[2][16];
int32_t i32_lastReceiveTime;

static uint16_t ui16_globalMask[2];
static uint32_t ui32_globalMask[2];
static uint32_t ui32_lastMask[2];
static bool b_busOpened[2];

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
// tread variables
XLhandle        g_hMsgEvent;                                          //!< notification handle for the receive queue
HANDLE          g_hRXThread;                                          //!< thread handle (RX)
HANDLE          g_hTXThread;                                          //!< thread handle (TX)
int             g_RXThreadRun;                                        //!< flag to start/stop the RX thread
int             g_TXThreadRun;                                        //!< flag to start/stop the TX thread (for the transmission burst)
////////////////////////////////////////////////////////////////////////////
// functions (Threads)

DWORD     WINAPI RxThread( PVOID par )
{
	while( gThreadRunning )
	{
		if ( b_blockThread )
		{ // application is accesing buffer or another action of app
			// is running where problems can be caused by parallel access from
			// this thread
			continue;
		}
		WaitForSingleObject(g_hMsgEvent,INFINITE);
		// now check for received messages
		b_blockApp = true;
		checkMsg();
		b_blockApp = false;
	}
}

#endif


int16_t can_startDriver()
{
  // open the driver
  XLstatus xlStatus;
  canlogDat = NULL;
  xlStatus = xlOpenDriver();
  xlStatus = xlGetDriverConfig(&g_xlDrvConfig);

  b_busOpened[0] = false;
  b_busOpened[1] = false;

	#ifdef USE_THREAD
	/** flag to control running thread */
	gThreadRunning = true;
	/** the thread is only allowed to perform activities, if this var is false */
	b_blockThread = true;
	/** the app must wait until this var is false, before it can safely take a message from buffer */
	b_blockApp = false;

  // Send a event for each Msg!!!
  xlStatus = xlSetNotification (g_xlPortHandle, &g_hMsgEvent, 1);
  DWORD         ThreadId=0;
  // Create the thread
  g_hRXThread = CreateThread(0, 0x1000, RxThread, (LPVOID) 0, 0, &ThreadId);
  SetThreadPriority(g_hRXThread,THREAD_PRIORITY_NORMAL/*THREAD_PRIORITY_TIME_CRITICAL*/);
	if( xlStatus == XL_ERROR )
	{
		return HAL_CONFIG_ERR;
	}
	#endif

  for (uint8_t b_bus = 0; b_bus < 2; b_bus++)
  {
    for (uint8_t ui8_nr = 0; ui8_nr < 16; ui8_nr++)
    {
      rec_bufSize[b_bus][ui8_nr] = 0;
      rec_bufCnt[b_bus][ui8_nr] = 0;
    }
  }
  if (xlStatus) goto error;

  return HAL_NO_ERR;

  error:
    printf("ERROR: %s!\n", xlGetErrorString(xlStatus));
    return HAL_CONFIG_ERR;
}
int16_t can_stopDriver()
{
	#ifdef USE_THREAD
	// set gThreadRunning to false so that the thread stops
	gThreadRunning = false;
	b_blockThread = true;
	#endif
  xlCloseDriver();
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
  return ((bBusNumber < 2)&&(bMsgObj < 16))?rec_bufCnt[bBusNumber][bMsgObj]:0;
};


int16_t init_can ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg,uint16_t wBitrate )
{
  XLstatus xlStatus;

  canlogDat = fopen("../../../simulated_io/can_send.txt", "a+");

  if (b_busOpened[bBusNumber] == false)
  { // Select channel
    gChannel = bBusNumber + 1;
    g_xlChannelMask = xlGetChannelMask(gHwType, 0, gChannel);

    if (!g_xlChannelMask) {
      printf("ERROR: no available channels found! (e.g. no CANcabs...)\n\n");
      xlStatus = XL_ERROR;
      return HAL_RANGE_ERR;
    }
    g_xlPermissionMask = g_xlChannelMask;

    // ------------------------------------
    // open ONE port including all channels
    // ------------------------------------
    xlStatus = xlOpenPort(&g_xlPortHandle, g_AppName, g_xlChannelMask, &g_xlPermissionMask, 256, XL_INTERFACE_VERSION, XL_BUS_TYPE_CAN);
    printf("- OpenPort         : CM=0x%I64x, PH=0x%02X, PM=0x%I64x, %s\n",
      g_xlChannelMask, g_xlPortHandle, g_xlPermissionMask, xlGetErrorString(xlStatus));

    if ( (xlStatus) || (g_xlPortHandle == XL_INVALID_PORTHANDLE) )
    	goto error;
  }
  // ------------------------------------
  // if we have permission we set the
  // bus parameters (baudrate)
  // ------------------------------------
  if (g_xlPermissionMask) {
    xlStatus = xlCanSetChannelBitrate(g_xlPortHandle, g_xlChannelMask, wBitrate);
    printf("- SetChannelBitrate: baudr.=%u, %s\n",wBitrate, xlGetErrorString(xlStatus));
    if (xlStatus) goto error;
  }
  else if (wBitrate) {
    printf("WARNING: No init access, bitrate ignored!\n");
  }
  ui16_globalMask[bBusNumber] = wGlobMask;
  ui32_globalMask[bBusNumber] = dwGlobMask;
	ui32_lastMask[bBusNumber] = dwGlobMaskLastmsg;

  // Disable the TX and TXRQ notifications
  xlStatus = xlCanSetChannelMode(g_xlPortHandle,g_xlChannelMask,0,0);
  if (xlStatus) goto error;


  if (b_busOpened[bBusNumber] == false)
  { // set global mask
    xlStatus = xlCanSetChannelAcceptance(g_xlPortHandle, g_xlChannelMask, 0x000, 0x000, XL_CAN_STD );
    if (xlStatus) goto error;
    xlStatus = xlCanSetChannelAcceptance(g_xlPortHandle, g_xlChannelMask, 0x000, 0x000, XL_CAN_EXT );
    if (xlStatus) goto error;

    // Go on bus and reset clock
    xlStatus = xlActivateChannel(g_xlPortHandle, g_xlChannelMask, XL_BUS_TYPE_CAN, XL_ACTIVATE_RESET_CLOCK);
    if (xlStatus) goto error;

  }
  b_busOpened[bBusNumber] = true;
	#ifdef USE_THREAD
	b_blockThread = false;
	#endif

  return HAL_NO_ERR;

  error:
    printf("ERROR: %s!\n", xlGetErrorString(xlStatus));

    if (g_xlPortHandle != XL_INVALID_PORTHANDLE) {
      xlClosePort(g_xlPortHandle);
      g_xlPortHandle = XL_INVALID_PORTHANDLE;
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
  xlDeactivateChannel(g_xlPortHandle, g_xlChannelMask);
  xlClosePort(g_xlPortHandle);
  g_xlPortHandle = XL_INVALID_PORTHANDLE;
  b_busOpened[bBusNumber] = false;
  return HAL_NO_ERR;
};

int16_t chgCanObjPause ( uint8_t bBusNumber, uint8_t bMsgObj, uint16_t wPause)
{
/*
  printf("sende Pause auf BUS %d fuer CAN Objekt %d auf %d eingestellt\n",
  (int16_t)bBusNumber, (int16_t)bMsgObj, wPause);
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
    xlCanFlushTransmitQueue(g_xlPortHandle, g_xlChannelMask);
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
  XLevent       xlEvent;
  XLstatus      xlStatus;
  unsigned int  messageCount = 1;

  xlEvent.tag                 = XL_TRANSMIT_MSG;
  xlEvent.tagData.msg.id      = ptSend->dwId;
  if (ptSend->bXtd == 1) xlEvent.tagData.msg.id |= 0x80000000UL;
  xlEvent.tagData.msg.dlc     = ptSend->bDlc;
  xlEvent.tagData.msg.flags   = 0;
  memmove(xlEvent.tagData.msg.data, ptSend->abData, ptSend->bDlc);

  xlStatus = xlCanTransmit(g_xlPortHandle, g_xlChannelMask, &messageCount, &xlEvent);

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

  if (xlStatus == XL_ERR_QUEUE_IS_FULL)
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
		WaitForSingleObject(g_hMsgEvent,100);
	}
	// tell thread to wait until this function is finished
	b_blockThread = true;
	#endif

	if (rec_bufCnt[bBusNumber][bMsgObj] > 0)
  { // data received
    i32_lastReceiveTime = getTime();
    // get access to data
    int16_t i16_out = rec_bufOut[bBusNumber][bMsgObj];
    can_data* pc_data = &(rec_buf[bBusNumber][bMsgObj][i16_out]);
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
    rec_bufOut[bBusNumber][bMsgObj] = ((i16_out + 1) % rec_bufSize[bBusNumber][bMsgObj]);
    rec_bufCnt[bBusNumber][bMsgObj] -= 1;
  }
	#ifdef USE_THREAD
	b_blockThread = false;
	#endif
  return HAL_NO_ERR;
};


int16_t checkMsg()
{ // first check if CANcardX buffer has received msg
  XLstatus xlStatus;
  XLevent*  gpEvent;

  int16_t result = 0;
  unsigned int msgsrx = 1;
  for(xlStatus = xlReceive(g_xlPortHandle, &msgsrx, gpEvent); xlStatus == XL_SUCCESS; xlStatus = xlReceive(g_xlPortHandle, &msgsrx, gpEvent))
  { // msg from CANcardX buffer
		// this functions retrurns not only received messages
		// ACK for SENT messages is also returned!!!
		if( ( gpEvent->tag != XL_RECEIVE_MSG ) || ( gpEvent->tagData.msg.flags != 0 ) )
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
    uint16_t ui16_id = (gpEvent->tagData.msg.id & 0xFFFF);
    for (uint8_t b_bus = 0; b_bus < 2; b_bus++)
    {
      if (b_busOpened[b_bus])
      {
        for (int16_t i16_obj = 0; i16_obj < 16; i16_obj++)
        { // compare received msg with filter
          int16_t i16_in;
          can_data* pc_data;
          if
						(
							(
								( i16_obj < 15 )
						&&  (
									( (rec_bufXtd[b_bus][i16_obj] == 1)
									&& (b_xtd == 1)
									&& (rec_bufSize[b_bus][i16_obj] > 0)
									&& ( (ui32_id & ui32_globalMask[b_bus]) ==  ((rec_bufFilter[b_bus][i16_obj]) & ui32_globalMask[b_bus]) )
									)
								|| ( (rec_bufXtd[b_bus][i16_obj] == 0)
									&& (b_xtd == 0)
									&& (rec_bufSize[b_bus][i16_obj] > 0)
									&& ( (ui16_id & ui16_globalMask[b_bus]) ==  (rec_bufFilter[b_bus][i16_obj] & ui16_globalMask[b_bus]) )
									)
								)
							)
					|| (
								( i16_obj == 15 )
						&&  (
									( (rec_bufXtd[b_bus][i16_obj] == 1)
									&& (b_xtd == 1)
									&& (rec_bufSize[b_bus][i16_obj] > 0)
									&& ( (ui32_id & ui32_globalMask[b_bus] & ui32_lastMask[b_bus]) ==  ((rec_bufFilter[b_bus][i16_obj]) & ui32_globalMask[b_bus] & ui32_lastMask[b_bus]) )
									)
								|| ( (rec_bufXtd[b_bus][i16_obj] == 0)
									&& (b_xtd == 0)
									&& (rec_bufSize[b_bus][i16_obj] > 0)
									&& ( (ui16_id & ui16_globalMask[b_bus] & ui32_lastMask[b_bus]) ==  (rec_bufFilter[b_bus][i16_obj] & ui16_globalMask[b_bus] & ui32_lastMask[b_bus]) )
									)
								)
							)
						)
          { // received msg fits actual filter
            i16_in = rec_bufIn[b_bus][i16_obj];
            rec_bufIn[b_bus][i16_obj] = ((i16_in + 1) % rec_bufSize[b_bus][i16_obj]);
            if (rec_bufCnt[b_bus][i16_obj] >= rec_bufSize[b_bus][i16_obj])
            { // overflow -> insert new, and overwrite oldest msg in buffer
              rec_bufOut[b_bus][i16_obj] = rec_bufIn[b_bus][i16_obj];
            }
            else
            {
              rec_bufCnt[b_bus][i16_obj] += 1;
            }
            pc_data = &(rec_buf[b_bus][i16_obj][i16_in]);
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
