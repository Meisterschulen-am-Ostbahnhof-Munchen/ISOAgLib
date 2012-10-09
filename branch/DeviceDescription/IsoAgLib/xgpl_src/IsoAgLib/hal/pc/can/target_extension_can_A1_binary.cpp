/***************************************************************************
						  target_extension_can_A1_binary.cpp - source for the
									   PSEUDO BIOS for development and test on
									   the Opus A1 with CAN communication using
									   the Binary can drivers for /dev/wecan0
									   and /dev/wecan1

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
#define _GNU_SOURCE
#include "can_target_extensions.h"
#include <IsoAgLib/hal/pc/system/system.h>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define USE_THREAD

#ifdef USE_THREAD
	#include <pthread.h>
#endif

namespace __HAL {
#define HWTYPE_AUTO 1000
#define c_ICAN     1
#define c_PowerCAN 2
#define c_CANAS    3
#define c_CANA1ASCII    4
#define c_CANA1BINARY    5
#define c_EICAN    6
#define c_ECAN_PCI 7
#define c_CANLpt   8
#define c_PowerCANPCI       10
#define c_CANUSB_Std_Api    11


/////////////////////////////////////////////////////////////////////////
// Globals

// CAN Globals
int AllCanChannelCount = 0;
int apiversion;

// IO address for LPT and ISA ( PCI and PCMCIA use automatic adr )
const int32_t LptIsaIoAdr = 0x378;

#define USE_CAN_CARD_TYPE c_CANA1BINARY

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

struct can_recv_data {
  uint32_t b_bus;
  can_data msg;
};


////////////////////////////////////////////////////////////////////////////////////////
// Wachendorff stuff...
#include <sys/ioctl.h>

#define MSGTYPE_EXTENDED        0x02            /* extended frame */
#define MSGTYPE_STANDARD        0x00            /* standard frame */

/* ioctl request codes */
#define CAN_MAGIC_NUMBER        'z'
#define MYSEQ_START             0x80

#define CAN_WRITE_MSG   _IOW(CAN_MAGIC_NUMBER, MYSEQ_START + 1, sizeof(canmsg))
#define CAN_READ_MSG    _IOR(CAN_MAGIC_NUMBER, MYSEQ_START + 2, sizeof(canmsg))

struct CANmsg {
        unsigned        id;
        int             msg_type;
        int             len;
        unsigned char   data[8];
        unsigned long   time;           /* timestamp in msec, at read only */
};
typedef struct CANmsg canmsg;
// ...Wachendorff stuff
////////////////////////////////////////////////////////////////////////////////////////


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
int can_device = 0;

#ifdef USE_THREAD
/** flag to control running thread */
static bool gThreadRunning;
/** the thread is only allowed to perform activities, if this var is false */
static bool b_blockThread;
/** the app must wait until this var is false, before it can safely take a message from buffer */
static bool b_blockApp;


/** handle for threading */
static pthread_t thread_canReceive;
static int  i_createHandleThreadCanReceive;

static void *thread_canReceive_function( void *ptr )
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

/////////////////////////////////////////////////////////////////////////
// Function Declarations

int ca_InitApi_1 (int typ, int IoAdr);
int ca_ResetCanCard_1(void);
int ca_InitCanCard_1 (int channel, int msgnr, int accode, int accmask
	, int fullcanmask[5], int btr0, int btr1, int octrl, int typ, int extended);
int ca_TransmitCanCard_1(int channel, int extended, tSend* ptSend);
int ca_GetData_1 (can_recv_data* receivedata);

/////////////////////////////////////////////////////////////////////////
// Local Data

int	canBusIsOpen[cui32_maxCanBusCnt];
FILE*	canBusFp[cui32_maxCanBusCnt];
can_recv_data receivedata;

/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
//
// METHOD:	ca_InitApi_1
// PURPOSE:	To load and initialize the CAN driver and return the CAN API version
// RETURNS:	API version greater than 0
// 			0 on error (no CAN device available)
// NOTES:	In this case, we will simply return an API version of 1.0  (0x0100)
// 			Highversion is in highbyte, Lowversion is in lowbyte
//
// FUTURE:  This should be finished to actually check if /dev/wecan0 and
//			/dev/wecan1 are available and the drivers installed correctly or not
//
/////////////////////////////////////////////////////////////////////////
int ca_InitApi_1 (int typ, int IoAdr)
{
	for( int i=0; i<cui32_maxCanBusCnt; i++ )
		{
		canBusIsOpen[i] = false;
		canBusFp[i] = NULL;
		}

	return 0x0100;
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:	ca_ResetCanCard_1
// PURPOSE:	To reset the alreay initialized CAN driver (especially clear its
//			buffers and any error conditions)
// RETURNS:	non-zero if CAN device was reset ok
// 			0 on error
// NOTES:	In this case, we will simply return 1
// FUTURE:  This should be finished to actually check the return values of fflush()
//
/////////////////////////////////////////////////////////////////////////
int ca_ResetCanCard_1(void)
{
	for( int i=0; i<cui32_maxCanBusCnt; i++ )
		if( canBusIsOpen[i] )
			fflush( canBusFp[i] );

	return 1;
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:	ca_InitCanCard_1
// PURPOSE:	To initialize the specified CAN BUS to begin sending/receiving msgs
// PARAMS:	channel		// 0 for CANLPT/ICAN, else 1 for first BUS
//			msgnr		// msg-nr / 0 for CANLPT/ICAN
//			accode		// Acceptance Code to receive everything for ICAN
//			accmask		// Acceptance Mask to receive everything for ICAN
//			fullcanmask	// filter array of int[16];
//			btr0		// BTR0
//			btr1		// BTR1
//			octrl		// reserved
//			typ			// typ 0 = 2 x 32 Bit, 1 = 4 x 16 Bit,
//						// 2 = 8 x 8 Bit, 3 = kein durchlass
//			extended	// reserved
// RETURNS:	non-zero if CAN BUS was initialized ok
// 			0 on error
// NOTES:	In this case, we will simply return 1
//
// FUTURE:  This should be finished to actually use the parameters like baud
//			rate and extended IDs during the initialization.  Right now they
//			are hardcoded and the passed in params are ignored.
//			Should check return value of write() to make sure the baud rate
//			was set correctly before proceeding.
//
/////////////////////////////////////////////////////////////////////////
int ca_InitCanCard_1 (int channel, int msgnr, int accode, int accmask
	, int fullcanmask[5], int btr0, int btr1, int octrl, int typ, int extended)
{
#ifdef DEBUG
	fprintf(stderr,"In Init Function - can channel = %d\n", channel);
#endif

	int retval = 0;	// default to error

	if( channel >= 0 && channel < cui32_maxCanBusCnt )
		{
		if( !canBusIsOpen[channel] )
			{
#ifdef DEBUG
fprintf(stderr,"Opening CAN BUS channel=%d\n", channel);
#endif
			char fname[32];
			sprintf( fname, "/dev/wecan%u", channel );
#ifdef DEBUG
fprintf(stderr,"open( \"%s\", O_RDRWR)\n", fname);
#endif
    			can_device = open(fname, O_RDWR | O_NONBLOCK);
			if (!can_device)
				{
#ifdef DEBUG
				fprintf(stderr,"Could not open CAN bus%d\n",channel);
#endif
				return 0;
				}

			// Set baud rate to 250 and turn on extended IDs
            // For Opus A1, it is done by sending the following string to the can_device
            {
            char buf[16];
            sprintf( buf, "i 0x%2x%2x e\n", btr0 & 0xFF, btr1 & 0xFF );     //, (extended?" e":" ") extended is not being passed in! Don't use it!
#ifdef DEBUG
fprintf(stderr,"write( device-\"%s\"\n, \"%s\", %d)\n", fname, buf, strlen(buf));
#endif
			write(can_device, buf, strlen(buf));
            }

			canBusFp[channel] = fdopen( can_device, "w+" );
			if( !canBusFp[channel] )
				{
				// open succeeded, but fdopen failed!!!
				// close the open file handle created with open()
				if( can_device )
					close(can_device);

#ifdef DEBUG
				fprintf(stderr,"Could not fdopen CAN bus%d\n",channel);
#endif
				return 0;
				}

			canBusIsOpen[channel] = true;
			return 1;
			}
		else
			return 1;	// already initialized and files are already open
		}
	else
		{
#ifdef DEBUG
		fprintf(stderr,"Invalid CAN bus%d\n", channel);
#endif
		return 0;
		}
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:	ca_TransmitCanCard_1
// PURPOSE:	To send a msg on the specified CAN BUS
// PARAMS:	channel		// 0 for CANLPT/ICAN, else 1 for first BUS
//			extended	// extended Frame
//			ptSend		// can object
// RETURNS:	non-zero if msg was sent ok
// 			0 on error
// NOTES:	In this case, we will simply return 1
//
/////////////////////////////////////////////////////////////////////////
int ca_TransmitCanCard_1(int channel, tSend* ptSend)
{
// Always Transmit to this format:
//  the letter 'm', extended/standard ID, CAN ID, Data Length, data bytes, timestamp
//	m e 0x0cf00300 8  0xff 0xfe 0x0b 0xff 0xff 0xff 0xff 0xff       176600
//
// Input is in this format:
// typedef struct
// {
//   uint32_t dwId;                          /** Identifier */
//   uint8_t bXtd;                           /** Laenge Bit Identifier */
//   uint8_t bDlc;                           /** Anzahl der empfangenen Daten */
//   uint8_t abData[8];                      /** Datenpuffer */
// } tSend;

//	fprintf(stderr,"Transmitting data to channel %d\n", channel);

	char sendbuf[80+1];
	char* buf = sendbuf;


//    fprintf(stderr,"CAN-RX: %9ld: 0x%08x %c [%d]",
//            msg.time, msg.id,
//            (msg.msg_type & MSGTYPE_EXTENDED) ? 'E' : 'S',
//            msg.len);
//    for (k = 0; k < msg.len; k++) {
//            fprintf(stderr," 0x%02x", msg.data[k]);
//    }
//    fprintf(stderr,"\n");

    CANmsg msg;
    msg.id = ptSend->dwId;
    msg.msg_type = ( ptSend->bXtd ? MSGTYPE_EXTENDED : MSGTYPE_STANDARD );
    msg.len = ptSend->bDlc;
    msg.time = 0;
	for( int i=0; i<msg.len; i++ )
        msg.data[i] = ptSend->abData[i];

  fd_set wfds;
  int retval;
  int maxfd = can_device+1;

  FD_ZERO(&wfds);
  FD_SET(can_device, &wfds);

  struct timeval tv;
  tv.tv_sec = 0;  	// timeout immediately (only check if data is waiting when on a timer)
  tv.tv_usec = 0;
  retval = select(maxfd, NULL, &wfds, NULL, &tv);
  if(retval == -1)
  {
    fprintf(stderr, "Error Occured in select\n");
    return 0;

  } else if(retval == 0)
  {
    fprintf(stderr, "Error can't write Occured in select\n");
    return 0;
  } else
  {
    if(FD_ISSET(can_device, &wfds) != 1)
    {
      fprintf(stderr, "Not selecting right thing\n");
      return 0;
    }
  }

    int ret = ioctl(can_device, CAN_WRITE_MSG, &msg);
    if (ret < 0)
        {
        /* nothing to read or interrupted system call */
#ifdef DEBUG
        fprintf(stderr, "CAN error: \n");
#endif
//        fprintf(stderr, "CAN error: %s\n", strerror(errno));
        }
//    else
//    {
//	buf += sprintf( sendbuf, "m %c 0x%08lx %u  ", (ptSend->bXtd ? 'e' : 's'), ptSend->dwId, (uint16_t) ptSend->bDlc );
//	for( int i=0; i<8; i++ )
//		{
//		if( i < ptSend->bDlc )
//			buf += sprintf( buf, "0x%02x ", (uint16_t) ptSend->abData[i] );
//		else
//			buf += sprintf( buf, "     " );
//		}
//
//	buf += sprintf( buf, "       192990\n" );   // Not supposed to be necessary!
//
////	fprintf( canBusFp[channel], "%s", sendbuf );
////	fflush( canBusFp[channel]);
//	fprintf(stderr, "%s", sendbuf );
//    }

	return 1;
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:	ca_GetData_1
// PURPOSE:	To receive a msg from a CAN BUS
// PARAMS:	receivedata		// can object
// RETURNS:	non-zero if a msg was received
// 			0 if no msg received
// NOTES:	In this case, we will simply return 0
//
// FUTURE:  This should be finished to actually receive a CAN msg
//
/////////////////////////////////////////////////////////////////////////
int ca_GetData_1 (can_recv_data* receivedata)
{
	char* line = NULL;
	char* next = NULL;
	size_t len = 0;
	ssize_t read;


	for( int channel=0; channel<cui32_maxCanBusCnt; channel++ )
	{
		if( canBusIsOpen[channel])
		{

		fd_set rfds;
	    int retval;
		int maxfd = can_device+1;

		FD_ZERO(&rfds);
       	FD_SET(can_device, &rfds);

#ifdef USE_THREAD
		retval = select(maxfd, &rfds, NULL, NULL, NULL);	// wait infinitely for next CAN msg (when multi-threaded)
#else
	    struct timeval tv;
		tv.tv_sec = 0;  	// timeout immediately (only check if data is waiting when on a timer)
		tv.tv_usec = 0;
		retval = select(maxfd, &rfds, NULL, NULL, &tv);
#endif

		if(retval == -1)
		{
#ifdef DEBUG
			fprintf(stderr,"Error Occured in select\n");
#endif
			return 0;

		} else if(retval == 0)
		{
			return 0;
		} else
		{
			if(FD_ISSET(can_device, &rfds) != 1)
			{
#ifdef DEBUG
				fprintf(stderr,"Not selecting right thing\n");
#endif
				return 0;
			}
		}
        CANmsg msg;
        int ret = ioctl(can_device, CAN_READ_MSG, &msg);
        if (ret < 0)
            {
            /* nothing to read or interrupted system call */
#ifdef DEBUG
            fprintf(stderr, "CANRead error: %i\n", ret);
#endif
    //        fprintf(stderr, "CAN error: %s\n", strerror(errno));
            return 0;
            }
        else
            {
//            fprintf(stderr, "CANRead OK: %i\n", ret);
		    receivedata->b_bus = channel;
            receivedata->msg.i32_ident = msg.id;
			receivedata->msg.b_xtd = (msg.msg_type & MSGTYPE_EXTENDED) == MSGTYPE_EXTENDED;
            receivedata->msg.b_dlc = msg.len;
            receivedata->msg.i32_time = msg.time;
	        for( int i=0; i<msg.len; i++ )
                receivedata->msg.pb_data[i] = msg.data[i];

//	{
//	fprintf(stderr,"Received data from channel %d\n", channel);
//
//	char sendbuf[80+1];
//	char* buf = sendbuf;
//
//	buf += sprintf( sendbuf, "m %c 0x%08lx %u  ", (receivedata->msg.b_xtd ? 'e' : 's'), receivedata->msg.i32_ident, (uint16_t) receivedata->msg.b_dlc );
//	for( int i=0; i<8; i++ )
//		{
//		if( i < receivedata->msg.b_dlc )
//			buf += sprintf( buf, "0x%02x ", (uint16_t) receivedata->msg.pb_data[i] );
//		else
//			buf += sprintf( buf, "     " );
//		}
//
//	buf += sprintf( buf, "       %lu\n", msg.time );
//
//	fprintf(stderr, "%s", sendbuf );
//	}

            return 1;
            }

//			FD_ZERO(&rfds);
//        		FD_SET(can_device, &rfds);
//        		tv.tv_sec = 0;  // timeout immediately (only check if data is waiting
//        		tv.tv_usec = 0;
//			retval = select(maxfd, &rfds, NULL, NULL, &tv);
//
//			if(retval == -1)
//			{
//				fprintf(stderr,"Error Occured in select\n");
//				return 0;
//
//			} else if(retval == 0)
//			{
//				return 0;
//			} else
//			{
//				if(FD_ISSET(can_device, &rfds) != 1)
//				{
//					fprintf(stderr,"Not selecting right thing\n");
//					return 0;
//				}
//			}
//			//fprintf(stderr,"before getline\n");
//			read = getline(&line, &len, canBusFp[channel]);
//			//fprintf(stderr,"Found an open can - channel = %d, len = %d, read = %d, data=\n%s\n", channel, (int)len, (int)read, line);
//			if( read > 70 )
//			{
//				receivedata->b_bus = channel;
//				if( line[0] == 'm' )
//				{
//					receivedata->msg.b_xtd = ( line[2] == 'e' );
//					line[14] = 0;
//					receivedata->msg.i32_ident = strtoul(&line[4], &next, 16);
//					line[16] = 0;
//					receivedata->msg.b_dlc = strtoul(&line[15], &next, 10);
////			fprintf(stderr,"b_xtd = %d, i32_ident = %d, b_dlc = %d\n", (int)receivedata->msg.b_xtd, (int)receivedata->msg.i32_ident, (int)receivedata->msg.b_dlc );
//
//					if( receivedata->msg.b_dlc <= 8 )
//					{
//						for( int i=0; i<receivedata->msg.b_dlc; i++ )
//						{
//							line[22+i*5] = 0;
//							receivedata->msg.pb_data[i] = strtoul(&line[18+i*5], &next, 16);
//						}
//
//						line[70] = 0;
//						receivedata->msg.i32_time = strtoul(&line[58], &next, 10);
//
//						return 1;
//					}
//				}
//			}  //else
//			   //	fprintf(stderr,"line shorter than 70\n");
		} //else
			//fprintf(stderr,"can channel %d not open\n", channel);
	}
//	fprintf(stderr,"end of get data from can\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////

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

//  	if ( gHwType = HWTYPE_AUTO )
//  	{
//  		const int32_t ci_tryCardTypes[] = { c_CANLpt, c_PowerCANPCI, c_CANUSB_Std_Api, c_PowerCAN, c_CANAS, c_ICAN, c_EICAN, c_ECAN_PCI };
//  		const int32_t ci_cardTypeCnt = 7;
//  		for ( int32_t ind = 0; ind < ci_cardTypeCnt; ind++ )
//  		{
//  			// usleep( 100 );
//  			if ( ( ci_tryCardTypes[ind] != c_CANAS ) && ( ci_tryCardTypes[ind] != c_ECAN_PCI ) )
//  			{
//  				apiversion = ca_InitApi_1(ci_tryCardTypes[ind], LptIsaIoAdr);
//  			}
//  			else
//			{
//  				apiversion = ca_InitApi_1(ci_tryCardTypes[ind], 0);
//  			}
//			// break search loop if card found
//  			if ( apiversion != 0 ) {
//  				gHwType = ci_tryCardTypes[ind]; // store used card type
//  				break;
//  			}
//  			else
//  			{
//  				//FreeLibrary(hCAPIDLL);
//  				//usleep( 10 );
//  				//loadDllFunctionAddresses();
//  			}
//  		}
//  		if ( apiversion == 0 )
//  		{ // failure - nothing found
//  			fprintf(stderr, "FAILURE - No CAN card was found with automatic search with IO address %04X for manually configured cards\r\n", LptIsaIoAdr );
//  			return HAL_CONFIG_ERR;
//  		}
//  	}
//  	else
//  	{
//  		if ( ( gHwType != c_CANAS ) && ( gHwType != c_ECAN_PCI ) )
  		{
  			apiversion = ca_InitApi_1(gHwType, LptIsaIoAdr);
//  		}
//  		else
//  		{
//  			apiversion = ca_InitApi_1(gHwType, 0);
//  		}
  		if ( apiversion == 0 )
  		{ // failure - nothing found
#ifdef DEBUG
  			fprintf(stderr, "FAILURE - No CAN card was found with automatic search with IO address %04X for manually configured cards\r\n", LptIsaIoAdr );
#endif
  			return HAL_CONFIG_ERR;
  		}
  	}
//  	fprintf(stderr,"InitApi done\n");
//  	fprintf(stderr,"API-Version......: %4x\n",(apiversion & 0xFF00) >> 8);
//	fprintf(stderr,"API-Ausfuehrung..: %4x\n",(apiversion & 0xFF));
	// ----------------------------------------------------------------------------
	// do the reset
	int i = ca_ResetCanCard_1();
	if (i) { //fprintf(stderr,"Reset CANLPT ok\n");
	}
	else   {
#ifdef DEBUG
    fprintf(stderr,"Reset CANLPT not ok\n");
#endif
					// exit(0);
	}
	// wait to be shure that CAN card is clean reset
//	usleep(100);

	#ifdef USE_THREAD
	/** flag to control running thread */
	gThreadRunning = true;
	/** the thread is only allowed to perform activities, if this var is false */
	b_blockThread = true;
	/** the app must wait until this var is false, before it can safely take a message from buffer */
	b_blockApp = false;

	// create a thread
	i_createHandleThreadCanReceive = pthread_create( &thread_canReceive, NULL, thread_canReceive_function, (void*) NULL);
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
		usleep( 1000 );
	}
	// set gThreadRunning to false so that the thread stops
	gThreadRunning = false;
	b_blockThread = true;

	// wait until thread finishes
	pthread_join( thread_canReceive, NULL);
	#endif
	for ( uint32_t ind = 0; ind < cui32_maxCanBusCnt; ind++ )
	{
		if (canlogDat[ind] != NULL)
		{

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
		usleep( 100 );
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
		usleep( 100 );
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

//	if ( ( gHwType == c_ICAN ) || ( gHwType == c_CANLpt ) )
//	{
//		if ( bBusNumber > 0 )
//		{
//			fprintf(stderr, "ERROR - the selected CAN card has only ONE channel, so that Bus-Nr: %hd is not defined\r\n", bBusNumber );
//			return HAL_CONFIG_ERR;
//		}
        channel = bBusNumber;
//	}
//	else
//	{ // channel for sontheim multi BUS cards starts counting with 1
//		channel = bBusNumber + 1;
//	}

	#ifdef USE_THREAD
	// wait until the receive thread allows access to buffer
	while ( b_blockApp )
	{ // do something for 1msec - just to take time
		usleep( 100 );
	}
	// tell thread to wait until this function is finished
    b_blockThread = true;
	#endif

  // Added by M.Wodok 6.12.04
//  canlogDat[bBusNumber] = fopen("..\\..\\..\\..\\simulated_io\\can_send.txt", "w+");
  canlogDat[bBusNumber] = NULL;
//  if(canlogDat[bBusNumber])
//  {
//    //fprintf(stderr,"canlogDat file opened: '..\\..\\..\\..\\simulated_io\\can_send.txt'\n");
//  }
//  else
//  {
//    // try in current directory...
////    canlogDat[bBusNumber] = fopen("can_send.txt", "w+");
//    if(canlogDat[bBusNumber])
//    {
//      //fprintf(stderr,"canlogDat file opened: 'can_send.txt'\n");
//    }
//    else
//    {
//      //fprintf(stderr,"canlogDat file FAILED to open! Error Code = %d\n", canlogDat[bBusNumber]);
//    }
//  }

	for ( int fdata_ind = 0; fdata_ind < 15; fdata_ind++ ) fdata[fdata_ind] = 0;

//	// set the data array for control of filters
//	// -> let here everything IN
//	switch ( gHwType )
//	{
//		case c_ICAN: // not used -> let 0
//			break;
//		case c_CANLpt:
//			// receive everything for CAN-LPT style
//			fdata[0]  = 0x00;
//			fdata[1]  = 0x00;
//			fdata[2]  = 0x00;
//			fdata[3]  = 0x00;
//			fdata[4]  = 0xFF;
//			fdata[5]  = 0xFF;
//			fdata[6]  = 0xFF;
//			fdata[7]  = 0xFF;
//
//			fdata[8]  = 0x00;
//			fdata[9]  = 0x00;
//			fdata[10] = 0x00;
//			fdata[11] = 0x00;
//			fdata[12] = 0xFF;
//			fdata[13] = 0xFF;
//			fdata[14] = 0xFF;
//			fdata[15] = 0xFF;
//			break;
//		case c_PowerCAN:
//		case c_CANAS:
//		case c_EICAN:
//			// receive everything the other style
//			fdata[0]  = 0x00;
//			fdata[1]  = 0x00;
//			fdata[2]  = 0xFFE0;
//			fdata[3]  = 0x00;
//			fdata[4]  = 0xFF;
//			break;
//	}

	// derive appropriate the bitrate setting
//	switch ( gHwType )
//	{
//		case c_CANA1BINARY:
//		case c_CANA1ASCII:
			switch ( wBitrate )
			{
//				case 10: { btr0 = 0x3F; btr1 = 0x7F;} break;
//				case 20: { btr0 = 0x31; btr1 = 0x1c;} break;
//				case 50: { btr0 = 0x13; btr1 = 0x1C;} break;
				case 100: { btr0 = 0xc3; btr1 = 0x3e;} break;
				case 125: { btr0 = 0xc3; btr1 = 0x3a;} break;
				case 250: { btr0 = 0xc1; btr1 = 0x3a;} break;
				case 500: { btr0 = 0xc0; btr1 = 0x3a;} break;
//				case 800: { btr0 = 0x01; btr1 = 0x16;} break;
//				case 1000: { btr0 = 0x00; btr1 = 0x1C;} break;
			}
//			break;
//		case c_CANLpt:
//			switch ( wBitrate )
//			{
//				case 10: { btr0 = 0x3F; btr1 = 0x7F;} break;
//				case 20: { btr0 = 0x31; btr1 = 0x1c;} break;
//				case 50: { btr0 = 0x13; btr1 = 0x1C;} break;
//				case 100: { btr0 = 0x09; btr1 = 0x1C;} break;
//				case 125: { btr0 = 0x07; btr1 = 0x1C;} break;
//				case 250: { btr0 = 0x03; btr1 = 0x1C;} break;
//				case 500: { btr0 = 0x01; btr1 = 0x1C;} break;
//				case 800: { btr0 = 0x01; btr1 = 0x16;} break;
//				case 1000: { btr0 = 0x00; btr1 = 0x1C;} break;
//			}
//			break;
//		default:
//			switch ( wBitrate )
//			{
//				case 10: { btr0 = 0x31; btr1 = 0x1c;} break;
//				case 20: { btr0 = 0x18; btr1 = 0x1c;} break;
//				case 50: { btr0 = 0x09; btr1 = 0x1c;} break;
//				case 100: { btr0 = 0x04; btr1 = 0x1c;} break;
//				case 125: { btr0 = 0x03; btr1 = 0x1c;} break;
//				case 250: { btr0 = 0x01; btr1 = 0x1C;} break;
//				case 500: { btr0 = 0x00; btr1 = 0x1C;} break;
//				case 800: { btr0 = 0x00; btr1 = 0x16;} break;
//				case 1000: { btr0 = 0x00; btr1 = 0x14;} break;
//			}
//			break;
//	}
   errcode = ca_InitCanCard_1(
						channel,  // 0 for CANLPT/ICAN, else 1 for first BUS
						0x00,  // msg-nr / 0 for CANLPT/ICAN
						0x00,  // Acceptance Code to receive everything for ICAN
						0xFF,  // Acceptance Mask to receive everything for ICAN
						fdata, // filter array of int[16];
						btr0,  // BTR0
						btr1,  // BTR1
						0x00,  // reserved
						0x00,  // typ 0 = 2 x 32 Bit, 1 = 4 x 16 Bit,
							   // 2 = 8 x 8 Bit, 3 = kein durchlass
						0x00); // reserved

//	if ( errcode && ( gHwType != c_ICAN ) )
//	{ // as the examples are not very clarifying in relation to the DOC
//		// we configure also msg obj 0xF to get a safe receive of Std AND Ext
//		if (errcode)
//		{
//			//printf ("CAN-LPT initialized for first MsgObj - Now configure for MsgObj 0xF to get safe receice of Std AND Ext\n");
//		}
//		else
//		{
//			//printf ("can't initialize CANLPT\n");
//			#ifdef USE_THREAD
//			b_blockThread = false;
//			#endif
//			return HAL_CONFIG_ERR;
//		}
//		errcode = ca_InitCanCard_1(
//							channel,  // 0 for CANLPT/ICAN, else 1 for first BUS
//							0x0F,  // msg-nr
//							0x00,  // Acceptance Code to receive everything for ICAN
//							0xFF,  // Acceptance Mask to receive everything for ICAN
//							fdata, // filter array of int[16];
//							btr0,  // BTR0
//							btr1,  // BTR1
//							0x00,  // reserved
//							0x00,  // typ 0 = 2 x 32 Bit, 1 = 4 x 16 Bit,
//										// 2 = 8 x 8 Bit, 3 = kein durchlass
//							0x00); // reserved
//	}
   if (errcode)
	 {
		 //printf ("CAN-LPT initialized\n");
	 }
   else
	 {
			printf ("can't initialize CAN\n");
			//printf ("can't initialize CANLPT\n");
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
		usleep( 100 );
	}
	// tell thread to wait until this function is finished
	b_blockThread = true;
	#endif
    if (canlogDat[bBusNumber] != NULL)
    {
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
  fprintf(stderr,"sende Pause auf BUS %d fuer CAN Objekt %d auf %d eingestellt\n",
  (int32_t)bBusNumber, (int32_t)bMsgObj, wPause);
  */
return HAL_NO_ERR;
}

int16_t getCanBusStatus(uint8_t bBusNumber, tCanBusStatus* ptStatus)
{
//  fprintf(stderr,"getCanBusStatus fuer BUS %d\n", bBusNumber);
  return HAL_NO_ERR;
}

int16_t clearCanObjBuf(uint8_t bBusNumber, uint8_t bMsgObj)
{
	if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
	#ifdef USE_THREAD
	// wait until the receive thread allows access to buffer
	while ( b_blockApp )
	{ // do something for 1msec - just to take time
		usleep( 100 );
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
		usleep( 100 );
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
		usleep( 100 );
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
		usleep( 100 );
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
//	if ( ( gHwType == c_ICAN ) || ( gHwType == c_CANLpt ) )
//	{
//		if ( bBusNumber > 0 )
//		{
//			//fprintf(stderr, "ERROR - the selected CAN card has only ONE channel, so that Bus-Nr: %hd is not defined\r\n", bBusNumber );
//			return HAL_CONFIG_ERR;
//		}
		channel = bBusNumber;
//	}
//	else
//	{ // channel for sontheim multi BUS cards starts counting with 1
//		channel = bBusNumber + 1;
//	}
//
//	if ( ((ptSend->dwId & 0x700) == 0x700)
//			||((ptSend->dwId & 0x7FF) == 0x520)
//			||((ptSend->dwId & 0x7FF) == 0x502)
//			||((ptSend->dwId & 0x700) == 0x200)
//			)
	{
#ifdef DEBUG
		fprintf(stderr,"Send: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptSend->dwId,
			ptSend->abData[0], ptSend->abData[1], ptSend->abData[2],
			ptSend->abData[3], ptSend->abData[4], ptSend->abData[5],
			ptSend->abData[6], ptSend->abData[7]);
#endif

#ifdef DEBUG
		if( canlogDat[bBusNumber] )
			fprintf(canlogDat[bBusNumber], "Sende: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptSend->dwId,
			ptSend->abData[0], ptSend->abData[1], ptSend->abData[2],
			ptSend->abData[3], ptSend->abData[4], ptSend->abData[5],
			ptSend->abData[6], ptSend->abData[7]);
#endif

	}
	int result = ca_TransmitCanCard_1(
			channel,						   // channel
			ptSend);			   // can object
	if ( result ) return HAL_NO_ERR;
	else return HAL_OVERFLOW_ERR;
};

// MDS NOTE: This is code is highly inefficient and needs to be optimized!
//  The receive structure should be changed so that it does not need to be copied from
//  one structure to another 3 or 4 times before sending it to the higher levels.
//  This should be done at most once!
//  There are several different CAN msg receive structures defined, and that should be
//  changed to only one through this file, and the one defined externally.
int16_t getCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tReceive * ptReceive )
{
	if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
	#ifdef USE_THREAD
	// wait until the receive thread allows access to buffer
	while ( b_blockApp )
	{ // do something for 1msec - just to take time
		usleep( 100 );
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

//  if ( ((ptReceive->dwId & 0x700) == 0x700)
//     ||((ptReceive->dwId & 0x7FF) == 0x520)
//     ||((ptReceive->dwId & 0x700) == 0x200)
//     ||((ptReceive->dwId & 0x7FF) == 0x502)
//      )
	{
#ifdef DEBUG
	  fprintf(stderr,"Recv: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptReceive->dwId,
      ptReceive->abData[0], ptReceive->abData[1], ptReceive->abData[2],
      ptReceive->abData[3], ptReceive->abData[4], ptReceive->abData[5],
      ptReceive->abData[6], ptReceive->abData[7]);

	if( canlogDat[bBusNumber] )
      fprintf(canlogDat[bBusNumber], "Empfang: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptReceive->dwId,
	  ptReceive->abData[0], ptReceive->abData[1], ptReceive->abData[2],
	  ptReceive->abData[3], ptReceive->abData[4], ptReceive->abData[5],
	  ptReceive->abData[6], ptReceive->abData[7]);
#endif
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
//		if ( gHwType == c_ECAN_PCI )
//		{ // check both channels - first 1
//#if 0
//			if ( ( ! b_busOpened[0] ) || ( ! ca_GetDataChannel_1( 1, &receivedata) ) )
//		{ // if 1 provides no data - then 2
//				if ( ( ! b_busOpened[1] ) || ( ! ca_GetDataChannel_1( 2, &receivedata) ) )
//				{ // if also 2 provides no data - then break
//					break;
//				}
//			}
//#endif
//		}
//		else
		{ // specify no channel on data request
			if ( ! ca_GetData_1( &receivedata) )
			{ // no data received
				break;
			}
		}
		result += 1;
		DLC = ( receivedata.msg.b_dlc & 0xF );
		if ( DLC > 8 ) DLC = 8;
		uint32_t ui32_id = receivedata.msg.i32_ident;
		b_bus = receivedata.b_bus;
		b_xtd = receivedata.msg.b_xtd;

		if (ui32_id >= 0x7FFFFFFF)
		{
#ifdef DEBUG
			fprintf(stderr,"!!Received of malformed message with undefined CAN ident: %x\n", ui32_id);
			if( canlogDat[b_bus] )
				fprintf(canlogDat[b_bus], "!!Received of malformed message with undefined CAN ident: %x\n", ui32_id);
#endif
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
				for ( uint32_t ind = 0; ind < DLC; ind++ ) pc_data->pb_data[ind] = receivedata.msg.pb_data[ind];
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

}; // end namespace __HAL
