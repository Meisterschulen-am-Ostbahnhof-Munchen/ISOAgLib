/***************************************************************************
              target_extension_can_server_A1_binary.cpp -
                    can server communicates with clients through message queues
                    read/write operation for /dev/wecan<bus_no>
                    message forwarding to other clients
                    #define SIMULATE_BUS_MODE for operation without can device (operation based on message forwarding)
                    use "can_server_a1 --help" for input parameters (log, replay mode)

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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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

// do not use can bus, operation is based on message forwarding in server
#ifndef SYSTEM_A1
#define SIMULATE_BUS_MODE 1
#endif

#include "can_target_extensions.h"
#include <cstring>
#include <cstdio>
#include <cctype>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include <pthread.h>
#include <linux/version.h>

#include "can_server.h"
#include "can_msq.h"

using namespace __HAL;


/////////////////////////////////////////////////////////////////////////
// Globals
/** if the following define is active, the can_server writes important logging info to the given path.
 *  comment this define out to eliminate this */
#ifdef WIN32
  #define CAN_SERVER_LOG_PATH ".\\can_server.log"
#elif defined( SYSTEM_A1 )
  #define CAN_SERVER_LOG_PATH "/sd0/settings/can_server.log"
#else
  #define CAN_SERVER_LOG_PATH "./can_server.log"
#endif



////////////////////////////////////////////////////////////////////////////////////////
// Wachendorff stuff...

#define MSGTYPE_EXTENDED        0x02            /* extended frame */
#define MSGTYPE_STANDARD        0x00            /* standard frame */

/* ioctl request codes */
#define CAN_MAGIC_NUMBER        'z'
#define MYSEQ_START             0x80

// kernel 2.6 needs type for third argument and not sizeof()
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define CAN_WRITE_MSG   _IOW(CAN_MAGIC_NUMBER, MYSEQ_START + 1, sizeof(canmsg))
#define CAN_READ_MSG    _IOR(CAN_MAGIC_NUMBER, MYSEQ_START + 2, sizeof(canmsg))
#else
#define CAN_WRITE_MSG   _IOW(CAN_MAGIC_NUMBER, MYSEQ_START + 1, canmsg)
#define CAN_READ_MSG    _IOR(CAN_MAGIC_NUMBER, MYSEQ_START + 2, canmsg)
#endif

struct CANmsg {
        unsigned        id;
        int             msg_type;
        int             len;
        unsigned char   data[8];
        unsigned long   time;           /* timestamp in msec, at read only */
};
typedef struct CANmsg canmsg;





/////////////////////////////////////////////////////////////////////////
// Local Data

static bool  canBusIsOpen[cui32_maxCanBusCnt];

bool ca_GetcanBusIsOpen_1 (int busId)
{
  return canBusIsOpen[busId];
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_InitApi_1
// PURPOSE: To load and initialize the CAN driver and return the CAN API version
// RETURNS: API version greater than 0
//      0 on error (no CAN device available)
// NOTES: In this case, we will simply return an API version of 1.0  (0x0100)
//      Highversion is in highbyte, Lowversion is in lowbyte
//
// FUTURE:  This should be finished to actually check if /dev/wecan0 and
//      /dev/wecan1 are available and the drivers installed correctly or not
//
/////////////////////////////////////////////////////////////////////////
int ca_InitApi_1 ()
{
  for( uint32_t i=0; i<cui32_maxCanBusCnt; i++ )
    {
    canBusIsOpen[i] = false;
    }

  return 0x0100;
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_ResetCanCard_1
// PURPOSE: To reset the alreay initialized CAN driver (especially clear its
//      buffers and any error conditions)
// RETURNS: non-zero if CAN device was reset ok
//      0 on error
// NOTES: In this case, we will simply return 1
// FUTURE:  This should be finished to actually check the return values of fflush()
//
/////////////////////////////////////////////////////////////////////////
int ca_ResetCanCard_1(void)
{
  return 1;
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_InitCanCard_1
// PURPOSE: To initialize the specified CAN BUS to begin sending/receiving msgs
// PARAMS:  channel   // 0 for CANLPT/ICAN, else 1 for first BUS
//      msgnr   // msg-nr / 0 for CANLPT/ICAN
//      accode    // Acceptance Code to receive everything for ICAN
//      accmask   // Acceptance Mask to receive everything for ICAN
//      fullcanmask // filter array of int[16];
//      btr0    // BTR0
//      btr1    // BTR1
//      octrl   // reserved
//      typ     // typ 0 = 2 x 32 Bit, 1 = 4 x 16 Bit,
//            // 2 = 8 x 8 Bit, 3 = kein durchlass
//      extended  // reserved
// RETURNS: non-zero if CAN BUS was initialized ok
//      0 on error
// NOTES: In this case, we will simply return 1
//
// FUTURE:  This should be finished to actually use the parameters like baud
//      rate and extended IDs during the initialization.  Right now they
//      are hardcoded and the passed in params are ignored.
//      Should check return value of write() to make sure the baud rate
//      was set correctly before proceeding.
//
/////////////////////////////////////////////////////////////////////////
int ca_InitCanCard_1 (uint32_t channel, int wBitrate, server_c* pc_serverData)
{

  DEBUG_PRINT1("init can channel %d\n", channel);

  int btr0 = 0;
  int btr1 = 1;

  switch ( wBitrate ) {
    case 100: { btr0 = 0xc3; btr1 = 0x3e;} break;
    case 125: { btr0 = 0xc3; btr1 = 0x3a;} break;
    case 250: { btr0 = 0xc1; btr1 = 0x3a;} break;
    case 500: { btr0 = 0xc0; btr1 = 0x3a;} break;
  }

  if( !canBusIsOpen[channel] ) {
    DEBUG_PRINT1("Opening CAN BUS channel=%d\n", channel);

    char fname[32];
#ifndef SIMULATE_BUS_MODE
    sprintf( fname, "/dev/wecan%u", channel );
#else
    sprintf( fname, "/dev/null");
#endif

    DEBUG_PRINT1("open( \"%s\", O_RDRWR)\n", fname);

    pc_serverData->can_device[channel] = open(fname, O_RDWR | O_NONBLOCK);

    if (pc_serverData->can_device[channel] == -1) {
      DEBUG_PRINT1("Could not open CAN bus%d\n",channel);
      return 0;
    }

    // Set baud rate to 250 and turn on extended IDs
    // For Opus A1, it is done by sending the following string to the can_device
    {
      char buf[16];
      sprintf( buf, "i 0x%2x%2x e\n", btr0 & 0xFF, btr1 & 0xFF );     //, (extended?" e":" ") extended is not being passed in! Don't use it!

      DEBUG_PRINT3("write( device-\"%s\"\n, \"%s\", %d)\n", fname, buf, strlen(buf));
      write(pc_serverData->can_device[channel], buf, strlen(buf));
    }

    canBusIsOpen[channel] = true;
    return 1;

  } else
    return 1; // already initialized and files are already open
}


void __HAL::updatePendingMsgs(server_c* pc_serverData, int8_t i8_bus)
{
/// @todo not implemented for right now on A1!
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_TransmitCanCard_1
// PURPOSE: To send a msg on the specified CAN BUS
// PARAMS:  channel   // 0 for CANLPT/ICAN, else 1 for first BUS
//      extended  // extended Frame
//      ptSend    // can object
// RETURNS: non-zero if msg was sent ok
//      0 on error
// NOTES: In this case, we will simply return 1
//
/////////////////////////////////////////////////////////////////////////
#ifndef SIMULATE_BUS_MODE
int ca_TransmitCanCard_1(tSend* ptSend, uint8_t ui8_bus, server_c* pc_serverData)
#else
int ca_TransmitCanCard_1(tSend* ptSend, uint8_t /* ui8_bus */, server_c* /* pc_serverData */)
#endif
{
// Always Transmit to this format:
//  the letter 'm', extended/standard ID, CAN ID, Data Length, data bytes, timestamp
//  m e 0x0cf00300 8  0xff 0xfe 0x0b 0xff 0xff 0xff 0xff 0xff       176600
//
// Input is in this format:
// typedef struct
// {
//   uint32_t dwId;                          /** Identifier */
//   uint8_t bXtd;                           /** Laenge Bit Identifier */
//   uint8_t bDlc;                           /** Anzahl der empfangenen Daten */
//   uint8_t abData[8];                      /** Datenpuffer */
// } tSend;

//  fprintf(stderr,"Transmitting data to channel %d\n", channel);


  CANmsg msg;
  msg.id = ptSend->dwId;
  msg.msg_type = ( ptSend->bXtd ? MSGTYPE_EXTENDED : MSGTYPE_STANDARD );
  msg.len = ptSend->bDlc;
  msg.time = 0;

  for( int i=0; i<msg.len; i++ )
    msg.data[i] = ptSend->abData[i];

#ifndef SIMULATE_BUS_MODE

// select call should not be necessary during write
#if 0
  fd_set wfds;
  int retval;
  int maxfd = pc_serverData->can_device[ui8_bus]+1;

  FD_ZERO(&wfds);
  FD_SET(pc_serverData->can_device[ui8_bus], &wfds);

  struct timeval tv;
  tv.tv_sec = 0;    // tv_sec and tv_usec= 0: timeout immediately (only check if data is waiting when on a timer)
  tv.tv_usec = 10000;  // wait not more than 10 msec for the state: nonblocking write
  retval = select(maxfd, NULL, &wfds, NULL, &tv);
  if(retval == -1)
  {
    DEBUG_PRINT("Error Occured in select\n");
    return 0;

  } else if(retval == 0)
  {
    DEBUG_PRINT("Error can't write Occured in select\n");
    return 0;
  } else
  {
    if(FD_ISSET(pc_serverData->can_device[channel], &wfds) != 1)
    {
      DEBUG_PRINT("Not selecting right thing\n");
      return 0;
    }
  }
#endif

  int ret = 0;

  if ((ui8_bus < HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus]) {
    ret = ioctl(pc_serverData->can_device[ui8_bus], CAN_WRITE_MSG, &msg);

#ifdef DEBUG_IOCTL
    if (ret < 0) {
      perror("ca_TransmitCanCard_1 ioctl");

      // try to read
      CANmsg msg;
      ret = ioctl(pc_serverData->can_device[ui8_bus], CAN_READ_MSG, &msg);
      printf("id 0x%x msg_type 0x%x len 0x%x data 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x time 0x%x\n", msg.id, msg.msg_type, msg.len, msg.data[0], msg.data[1], msg.data[2], msg.data[3], msg.data[4], msg.data[5], msg.data[6], msg.data[7], msg.time);

      if (ret < 0) {
        perror("ioctl read after write");
      }
    }
#endif
  }

#else
  int ret = 0;
#endif

  if (ret < 0) {
    // perror("ca_TransmitCanCard_1 ioctl");
    /* nothing to read or interrupted system call */
  }

  return 1;
}

int ca_ReceiveCanCard_1(can_recv_data* receiveData, uint8_t ui8_bus, server_c* pc_serverData)
{

  CANmsg msg;
  int ret = ioctl(pc_serverData->can_device[ui8_bus], CAN_READ_MSG, &msg);

  if (ret < 0) {
    return ret;
  } else {
    receiveData->b_bus = ui8_bus;
    receiveData->msg.i32_ident = msg.id;
    receiveData->msg.b_xtd = (msg.msg_type & MSGTYPE_EXTENDED) == MSGTYPE_EXTENDED;
    receiveData->msg.b_dlc = msg.len;
    receiveData->msg.i32_time = msg.time;
    memcpy( receiveData->msg.pb_data, msg.data, msg.len );
  }

  return 1;

}


