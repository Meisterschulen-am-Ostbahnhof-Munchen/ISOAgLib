/***************************************************************************
                          target_extension_can_w32_sontheim_canlpt.cpp - source for the PSEUDO BIOS for
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

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>

#define HWTYPE_AUTO 1000


#define c_ICAN     1
#define c_PowerCAN 2
#define c_CANAS    3
#define c_EICAN    6
#define c_ECAN_PCI 7
#define c_CANLpt   8
#define c_PowerCANPCI       10
#define c_CANUSB_Std_Api    11


/////////////////////////////////////////////////////////////////////////////
// definitions of DLL functions
/////////////////////////////////////////////////////////////////////////////
int (far __stdcall *ca_InitApi_1) (int typ, int IoAdr);

int (far __stdcall *ca_ResetCanCard_1) (void);

int (far __stdcall *ca_InitCanCard_1)
		(int channel, int msgnr, int accode, int accmask, int fullcanmask[5],
		 int btr0, int btr1, int octrl, int typ, int extended);
int  (far __stdcall *ca_TransmitCanCard_1)
		(int channel, int extended, int *data);

int  (far __stdcall *ca_GetData_1) (int *data);

int  (far __stdcall *ca_GetDataChannel_1) (int channel, int *data);

// alle Identifier sperren
int  (far __stdcall *ca_ResetSoftwareFilterMask_1) (int channel);

// alle Identifier zulassen
int  (far __stdcall *ca_SetAllId_1 ) (int channel);


int  (far __stdcall *ca_SetFilterId_1) (int channel, int identifier);

int  (far __stdcall *ca_Instruction_1) (int *data);


namespace __HAL {
/////////////////////////////////////////////////////////////////////////
// Globals
HANDLE       gEventHandle    = 0;
HINSTANCE hCAPIDLL;


// CAN Globals
int AllCanChannelCount = 0;
int apiversion;
int transmitdata[15];
int receivedata[15];

// IO address for LPT and ISA ( PCI and PCMCIA use automatic adr )
const int32_t LptIsaIoAdr = 0x378;


#ifdef USE_CAN_CARD_TYPE
int32_t gHwType = USE_CAN_CARD_TYPE;
#else
// select the Vector CAN card type to use
int32_t gHwType = HWTYPE_AUTO;
#endif

struct can_data {
  int32_t i32_time;
  int32_t i32_ident;
  uint8_t b_dlc;
  uint8_t b_xtd;
  uint8_t pb_data[8];
};

// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );
static can_data* rec_buf[cui32_maxCanBusCnt][15];
static int32_t rec_bufCnt[cui32_maxCanBusCnt][15];
static int32_t rec_bufSize[cui32_maxCanBusCnt][15];
static int32_t rec_bufOut[cui32_maxCanBusCnt][15];
static int32_t rec_bufIn[cui32_maxCanBusCnt][15];
static bool rec_bufXtd[cui32_maxCanBusCnt][15];
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

int16_t loadDllFunctionAddresses(void)
{
  // if ((int) (hCAPIDLL = LoadLibrary ("C:\\Hör\\AKTUELL\\Canapi_CanLPT\\Software\\PC\\api\\Release\\CANAPI.DLL")) <=  HINSTANCE_ERROR) {
  if ((int) (hCAPIDLL = LoadLibrary ("CANAPI.DLL")) <=  HINSTANCE_ERROR) {
       printf ("can't open Library \n");
		return HAL_CONFIG_ERR;
	}
  ca_InitApi_1 =
		(int (far __stdcall*) (int typ, int IoAdr)) GetProcAddress (hCAPIDLL,"ca_InitApi_1");

  ca_ResetCanCard_1 =
		(int (far __stdcall*) (void))               GetProcAddress (hCAPIDLL,"ca_ResetCanCard_1");

  ca_InitCanCard_1 =
		(int (far __stdcall*) (int channel, int msgnr, int accode, int accmask, int fullcanmask[5], int btr0, int btr1, int octrl,
			int typ, int extended))                   GetProcAddress (hCAPIDLL,"ca_InitCanCard_1");

  ca_TransmitCanCard_1 =
		(int (far __stdcall*) (int channel, int extended, int *data)) GetProcAddress (hCAPIDLL,"ca_TransmitCanCard_1");

  ca_GetData_1 =
		(int (far __stdcall*) (int *data))          GetProcAddress (hCAPIDLL,"ca_GetData_1");

  ca_ResetSoftwareFilterMask_1 =
		(int (far __stdcall*) (int channel))        GetProcAddress (hCAPIDLL,"ca_ResetSoftwareFilterMask_1");

  ca_SetAllId_1 =
		(int (far __stdcall*) (int channel))        GetProcAddress (hCAPIDLL,"ca_SetAllId_1");


  ca_SetFilterId_1 =
		(int (far __stdcall*) (int channel,int identifier)) GetProcAddress (hCAPIDLL,"ca_SetFilterId_1");

  ca_Instruction_1 =
		(int (far __stdcall*) (int *data))          GetProcAddress (hCAPIDLL,"ca_Instruction_1");
	return HAL_NO_ERR;
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
	for ( uint32_t ind = 0; ind < cui32_maxCanBusCnt; ind++ )
	{
		b_busOpened[ind]             = false;
		canlogDat[ind]               = NULL;
	}

  printf("load driver Dll\n");
	if ( loadDllFunctionAddresses() != HAL_NO_ERR) return HAL_CONFIG_ERR;
  printf("load driver Dll\n");

	if ( gHwType = HWTYPE_AUTO )
	{
		const int32_t ci_tryCardTypes[] = { c_CANLpt, c_PowerCANPCI, c_CANUSB_Std_Api, c_PowerCAN, c_CANAS, c_ICAN, c_EICAN, c_ECAN_PCI };
		const int32_t ci_cardTypeCnt = 7;
		for ( int32_t ind = 0; ind < ci_cardTypeCnt; ind++ )
		{
			Sleep( 100 );

			if ( ( ci_tryCardTypes[ind] != c_CANAS ) && ( ci_tryCardTypes[ind] != c_ECAN_PCI ) )
			{
				if (ci_tryCardTypes[ind]== c_CANUSB_Std_Api)
                    apiversion = ca_InitApi_1(ci_tryCardTypes[ind], 0x300);  //I/O Address is 0x300 for CANUSB!
                else
                    apiversion = ca_InitApi_1(ci_tryCardTypes[ind], LptIsaIoAdr);
			}
			else
			{
				apiversion = ca_InitApi_1(ci_tryCardTypes[ind], 0);
			}
			// break search loop if card found
			if ( apiversion != 0 ) {
				gHwType = ci_tryCardTypes[ind]; // store used card type
				break;
			}
			else
			{
				FreeLibrary(hCAPIDLL);
				Sleep( 10 );
				loadDllFunctionAddresses();
			}
		}
		if ( apiversion == 0 )
		{ // failure - nothing found
			printf( "FAILURE - No CAN card was found with automatic search with IO address %04X for manually configured cards\r\n", LptIsaIoAdr );
			return HAL_CONFIG_ERR;
		}
	}
	else
	{
		if ( ( gHwType != c_CANAS ) && ( gHwType != c_ECAN_PCI ) )
		{
			apiversion = ca_InitApi_1(gHwType, LptIsaIoAdr);
		}
		else
		{
			apiversion = ca_InitApi_1(gHwType, 0);
		}
		if ( apiversion == 0 )
		{ // failure - nothing found
			printf( "FAILURE - No CAN card was found with automatic search with IO address %04X for manually configured cards\r\n", LptIsaIoAdr );
			return HAL_CONFIG_ERR;
		}
	}
	printf("InitApi done\n");
	printf("API-Version......: %4x\n",(apiversion & 0xFF00) >> 8);
	printf("API-Ausfuehrung..: %4x\n",(apiversion & 0xFF));
	// ----------------------------------------------------------------------------
	// do the reset
	int i = ca_ResetCanCard_1();
	if (i) { printf("Reset CANLPT ok\n"); }
	else   { printf("Reset CANLPT not ok\n");
					// exit(0);
	}

    // Added this section of code to try to get the version from the CANUSB adapter.
    if (gHwType == c_CANUSB_Std_Api)
    {
        int sdata[16];
        int k=0;
        int erg = 0;
        char buf[256];
        for (k=0 ; k<16 ; k++) sdata[k]=0;
        sdata[0]=0x25;
        erg = ca_Instruction_1(sdata);
        if(erg)
        {
            buf[0]=0;
            for(k=3;k<16;k++)
            {
                buf[k-3] = (char)sdata[k];
            }
            buf[12]=0;
            printf("\nVersion of CANUSB: %s", buf);
        }
        else
        {
            printf("\nReading of Version failed !!!");
           // getch();
        }
    }
	// wait to be shure that CAN card is clean reset
	Sleep(100);

	#ifdef USE_THREAD
	/** flag to control running thread */
	gThreadRunning = true;
	/** the thread is only allowed to perform activities, if this var is false */
	b_blockThread = true;
	/** the app must wait until this var is false, before it can safely take a message from buffer */
	b_blockApp = false;

  // create a synchronisation object
  gEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

	// create a thread
  threadHandle = CreateThread(0,0x1000,thread,0,0,&threadId);
  SetThreadPriority(threadHandle,THREAD_PRIORITY_NORMAL/*THREAD_PRIORITY_TIME_CRITICAL*/);
	#endif

  for (uint8_t b_bus = 0; b_bus < cui32_maxCanBusCnt; b_bus++)
  {
    for (uint8_t ui8_nr = 0; ui8_nr < 15; ui8_nr++)
    {
      rec_bufSize[b_bus][ui8_nr] = 0;
      rec_bufCnt[b_bus][ui8_nr] = 0;
    }
  }
  return HAL_NO_ERR;
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

int16_t init_can ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg,uint16_t wBitrate )
{
	int fdata[16];
	int channel = 0;
	int btr0, btr1;
	int errcode;
    int accMask = 0x0;

	if ( ( gHwType == c_ICAN ) || ( gHwType == c_CANLpt ) )
	{
		if ( bBusNumber > 0 )
		{
			printf( "ERROR - the selected CAN card has only ONE channel, so that Bus-Nr: %hd is not defined\r\n", bBusNumber );
			return HAL_CONFIG_ERR;
		}
	}
	else
	{ // channel for sontheim multi BUS cards starts counting with 1
		channel = bBusNumber + 1;
	}

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

	for ( int fdata_ind = 0; fdata_ind < 15; fdata_ind++ ) fdata[fdata_ind] = 0;

	// set the data array for control of filters
	// -> let here everything IN
	switch ( gHwType )
	{
		case c_ICAN: // not used -> let 0
			break;
		case c_CANLpt:
        case c_CANUSB_Std_Api:
			// receive everything for CAN-LPT style
			fdata[0]  = 0x00;
			fdata[1]  = 0x00;
			fdata[2]  = 0x00;
			fdata[3]  = 0x00;
			fdata[4]  = 0xFF;
			fdata[5]  = 0xFF;
			fdata[6]  = 0xFF;
			fdata[7]  = 0xFF;

			fdata[8]  = 0x00;
			fdata[9]  = 0x00;
			fdata[10] = 0x00;
			fdata[11] = 0x00;
			fdata[12] = 0xFF;
			fdata[13] = 0xFF;
			fdata[14] = 0xFF;
			fdata[15] = 0xFF;
			break;
		case c_PowerCAN:
		case c_CANAS:
		case c_EICAN:
			// receive everything the other style
			fdata[0]  = 0x00;
			fdata[1]  = 0x00;
			fdata[2]  = 0xFFE0;
			fdata[3]  = 0x00;
			fdata[4]  = 0xFF;
			break;
	}

	// derive appropriate the bitrate setting
	switch ( gHwType )
	{
		case c_CANLpt:
            accMask = 0xFF;
			switch ( wBitrate )
			{
				case 10: { btr0 = 0x3F; btr1 = 0x7F;} break;
				case 20: { btr0 = 0x31; btr1 = 0x1c;} break;
				case 50: { btr0 = 0x13; btr1 = 0x1C;} break;
				case 100: { btr0 = 0x09; btr1 = 0x1C;} break;
				case 125: { btr0 = 0x07; btr1 = 0x1C;} break;
				case 250: { btr0 = 0x03; btr1 = 0x1C;} break;
				case 500: { btr0 = 0x01; btr1 = 0x1C;} break;
				case 800: { btr0 = 0x01; btr1 = 0x16;} break;
				case 1000: { btr0 = 0x00; btr1 = 0x1C;} break;
			}
			break;
        case c_CANUSB_Std_Api:
			switch ( wBitrate )
			{
				case 10: { btr0 = 0xF1; btr1 = 0x3E;} break;
				case 20: { btr0 = 0xF1; btr1 = 0x3E;} break;
				case 50: { btr0 = 0xF1; btr1 = 0x3E;} break;
				case 100: { btr0 = 0xD8; btr1 = 0x3E;} break;
				case 125: { btr0 = 0xD3; btr1 = 0x3E;} break;
				case 250: { btr0 = 0xC9; btr1 = 0x3E;} break;
				case 500: { btr0 = 0xC3; btr1 = 0x7F;} break;
				case 800: { btr0 = 0xC1; btr1 = 0x1C;} break;
				case 1000: { btr0 = 0xC1; btr1 = 0x1C;} break;
			}
			break;
		default:
			switch ( wBitrate )
			{
				case 10: { btr0 = 0x31; btr1 = 0x1c;} break;
				case 20: { btr0 = 0x18; btr1 = 0x1c;} break;
				case 50: { btr0 = 0x09; btr1 = 0x1c;} break;
				case 100: { btr0 = 0x04; btr1 = 0x1c;} break;
				case 125: { btr0 = 0x03; btr1 = 0x1c;} break;
				case 250: { btr0 = 0x01; btr1 = 0x1C;} break;
				case 500: { btr0 = 0x00; btr1 = 0x1C;} break;
				case 800: { btr0 = 0x00; btr1 = 0x16;} break;
				case 1000: { btr0 = 0x00; btr1 = 0x14;} break;
			}
			break;
	}
   errcode = ca_InitCanCard_1(
						channel,  // 0 for CANLPT/ICAN, else 1 for first BUS
						0x00,  // msg-nr / 0 for CANLPT/ICAN
						0x00,  // Acceptance Code to receive everything for ICAN
						accMask,  // Acceptance Mask to receive everything for ICAN
						fdata, // filter array of int[16];
						btr0,  // BTR0
						btr1,  // BTR1
						0x00,  // reserved
						0x00,  // typ 0 = 2 x 32 Bit, 1 = 4 x 16 Bit,
						       // 2 = 8 x 8 Bit, 3 = kein durchlass
						0x00); // reserved

	if ( errcode && ( gHwType != c_ICAN ) )
	{ // as the examples are not very clarifying in relation to the DOC
		// we configure also msg obj 0xF to get a safe receive of Std AND Ext
		if (errcode)
		{
			printf ("CAN-LPT initialized for first MsgObj - Now configure for MsgObj 0xF to get safe receive of Std AND Ext\n");
		}
		else
		{
			printf ("can't initialize CANLPT");
			#ifdef USE_THREAD
			b_blockThread = false;
			#endif
			return HAL_CONFIG_ERR;
		}
		errcode = ca_InitCanCard_1(
							channel,  // 0 for CANLPT/ICAN, else 1 for first BUS
							0x0F,  // msg-nr
							0x00,  // Acceptance Code to receive everything for ICAN
							accMask,  // Acceptance Mask to receive everything for ICAN
							fdata, // filter array of int[16];
							btr0,  // BTR0
							btr1,  // BTR1
							0x00,  // reserved
							0x00,  // typ 0 = 2 x 32 Bit, 1 = 4 x 16 Bit,
										// 2 = 8 x 8 Bit, 3 = kein durchlass
							0x00); // reserved
	}
   if (errcode)
	 {
		 printf ("CAN-LPT initialized\n");
	 }
   else
	 {
			printf ("can't initialize CANLPT");
			#ifdef USE_THREAD
			b_blockThread = false;
			#endif
			return HAL_CONFIG_ERR;
	 }

  ui16_globalMask[bBusNumber] = wGlobMask;
  ui32_globalMask[bBusNumber] = dwGlobMask;
	ui32_lastMask[bBusNumber] = dwGlobMaskLastmsg;

  b_busOpened[bBusNumber] = true;
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
	int channel = 0;
	if ( ( gHwType == c_ICAN ) || ( gHwType == c_CANLpt ) )
	{
		if ( bBusNumber > 0 )
		{
			printf( "ERROR - the selected CAN card has only ONE channel, so that Bus-Nr: %hd is not defined\r\n", bBusNumber );
			return HAL_CONFIG_ERR;
		}
	}
	else
	{ // channel for sontheim multi BUS cards starts counting with 1
		channel = bBusNumber + 1;
	}
	transmitdata[0] = ptSend->dwId;
	transmitdata[1] = ptSend->bDlc;
	for ( int32_t ind = 0; ind < ptSend->bDlc; ind++ ) transmitdata[2+ind] = ptSend->abData[ind];
	transmitdata[11] = 0; // no remote

	if ( ((ptSend->dwId & 0x700) == 0x700)
			||((ptSend->dwId & 0x7FF) == 0x520)
			||((ptSend->dwId & 0x7FF) == 0x502)
			||((ptSend->dwId & 0x700) == 0x200)
			)
	{
		printf("Tx: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptSend->dwId,
			ptSend->abData[0], ptSend->abData[1], ptSend->abData[2],
			ptSend->abData[3], ptSend->abData[4], ptSend->abData[5],
			ptSend->abData[6], ptSend->abData[7]);

			fprintf(canlogDat[bBusNumber], "Sende: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptSend->dwId,
			ptSend->abData[0], ptSend->abData[1], ptSend->abData[2],
			ptSend->abData[3], ptSend->abData[4], ptSend->abData[5],
			ptSend->abData[6], ptSend->abData[7]);

	}
	int result = ca_TransmitCanCard_1(
			channel,						   // channel
			ptSend->bXtd,						   // extended Frame
			transmitdata);			   // can object
	if ( result ) return HAL_NO_ERR;
	else
    {
        printf("HAL_OVERFLOW_ERROR!\n");
        return HAL_OVERFLOW_ERR;
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

  //if ( ((ptReceive->dwId & 0x700) == 0x700)
   //  ||((ptReceive->dwId & 0x7FF) == 0x520)
   //  ||((ptReceive->dwId & 0x700) == 0x200)
   //  ||((ptReceive->dwId & 0x7FF) == 0x502)
   //   )
    if( (ptReceive->dwId & 0xCB0000) == 0x00CB0000)
    {
      printf("Rx: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptReceive->dwId,
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
  int32_t result = 0;
	uint32_t DLC;
	uint8_t b_xtd;
	uint32_t ui32_id;

	uint32_t b_bus;
	// try to receive a message
	while ( true )
	{
		if ( gHwType == c_ECAN_PCI )
		{ // check both channels - first 1
			if ( ( ! b_busOpened[0] ) || ( ! ca_GetDataChannel_1( 1, receivedata) ) )
			{ // if 1 provides no data - then 2
				if ( ( ! b_busOpened[1] ) || ( ! ca_GetDataChannel_1( 2, receivedata) ) )
				{ // if also 2 provides no data - then break
					break;
				}
			}
		}
		else
		{ // specify no channel on data request
			if ( ! ca_GetData_1( receivedata) )
			{ // no data received
				break;
			}
		}
		result += 1;
		DLC = ( receivedata[2] & 0xF );
		if ( DLC > 8 ) DLC = 8;
		uint32_t ui32_id = receivedata[1];
		// detect channel
		switch ( receivedata[0] )
		{
			case 0x7001: // $7001 = CAN 1 standard data received
				b_bus = 0;
				b_xtd = 0;
				break;
			case 0x7002: // $7002 = CAN 2 standard data received
				b_bus = 1;
				b_xtd = 0;
				break;
			case 0x7003: // ICAN $7003 = CAN 1 data received
				b_bus = 0;
				b_xtd = 0;
				break;
			case 0x7103: // ICAN $7103 = CAN 2 data received
				b_bus = 1;
				b_xtd = 0;
				break;
			case 0xC001: // $C001 = CAN 1 extended data received
				b_bus = 0;
				b_xtd = 1;
				break;
			case 0xC002: // $C002 = CAN 2 extended data received
				b_bus = 1;
				b_xtd = 1;
				break;
            default:
                b_bus = 0;
                b_xtd = 0;
                break;
		}

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
				pc_data->b_dlc = DLC;
				pc_data->b_xtd = b_xtd;
				for ( uint32_t ind = 0; ind < DLC; ind++ ) pc_data->pb_data[ind] = receivedata[ind+3];
			} // if fit
		} // for objNr
		#ifdef USE_THREAD
		// un-block access from application on the buffers, as
		// the current buffers are again free for access
		b_blockApp = false;
		#endif
	} // try while loop
  return result;
}

} // end namespace __HAL
