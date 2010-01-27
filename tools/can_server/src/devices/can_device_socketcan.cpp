/***************************************************************************
    can_device_socketcan.cpp - interface for SocketCAN can
    based on kernel version 
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

#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/version.h>

#include "can_server.h"
#include "can_server_common.h"
#include <assert.h>

using namespace __HAL;

#warning "At time of writing, these constants are not defined in the headers"
#ifndef PF_CAN
  #define PF_CAN 29
#endif

#ifndef AF_CAN
  #define AF_CAN PF_CAN
#endif

extern int errno;

/* prior to 2.5.x versions of socket theres a specific ioctrl for 
 * setting the baudrate abailable. */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
typedef uint32_t can_baudrate_t;
#define SIOCSCANBAUDRATE	(SIOCDEVPRIVATE+0)
#endif

#ifdef DEBUG_CAN
std::ofstream m_clog;
#endif


static bool canBusIsOpen[cui32_maxCanBusCnt];

bool isBusOpen(uint8_t ui8_bus)
{
    return canBusIsOpen[ui8_bus];
}

uint32_t initCardApi ()
{
    for( uint32_t i=0; i<cui32_maxCanBusCnt; i++ )
    {
        canBusIsOpen[i] = false;
    }

#ifdef DEBUG_CAN
    m_clog.open("/tmp/vcan.log", std::ios::out|std::ios::app);
#endif

    return 1;
}

bool resetCard(void)
{
    return true;
}

// PURPOSE: To initialize the specified CAN BUS to begin sending/receiving msgs
bool openBusOnCard(uint8_t ui8_bus, uint32_t wBitrate, server_c* pc_serverData)
{
    (void)wBitrate; // not used for Kernel >= 2.6.

    DEBUG_PRINT1("init can bus %d\n", ui8_bus);

    if( !canBusIsOpen[ui8_bus] ) 
    {
        DEBUG_PRINT1("Opening CAN BUS channel=%d\n", ui8_bus);

        char fname[32];
        sprintf(fname, "can%u", ui8_bus);

        // Create a socket for talking to the CAN Bus
        int m_handle = socket(PF_CAN, SOCK_RAW, CAN_RAW);
#ifdef DEBUG_CAN
        m_clog << "Init(): bus: " << (int)ui8_bus << "fname: " << fname << "m_handle: " << m_handle << std::endl;
#endif
        if (m_handle < 0)
        {
            return false;
        }

        // Get the interface
        struct ifreq ifr;
        strncpy(ifr.ifr_name, fname, IFNAMSIZ);
        int ret = ioctl(m_handle, SIOCGIFINDEX, &ifr);
#ifdef DEBUG_CAN
        m_clog << "Init(): ret: " << ret << std::endl;
#endif
        if (ret < 0)
        {
            perror( "SIOCGIFINDEX" );
            pc_serverData->canBus(ui8_bus).mi32_can_device = NULL;
            return false;
        }

// Setting of Bitrate not possible in >= 2.6. Kernel version
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
        // Set the baudrate.  Must be set before first use
        struct ifreq ifr_baudrate;   // Use different structure for setting the baud rate as the ioctls
                                    // overwrite the structure
        strcpy(ifr_baudrate.ifr_name, fname);
        can_baudrate_t *baudrate = (can_baudrate_t *)&ifr_baudrate.ifr_ifru;
        *baudrate = wBitrate * 1000; 
        int baud_ret = ioctl(m_handle, SIOCSCANBAUDRATE, &ifr_baudrate);
#ifdef DEBUG_CAN
        m_clog << "Init(): baud_ret: " << baud_ret << std::endl;
#endif
        if (baud_ret < 0)
        {
            perror( "SIOCSCANBAUDRATE" );
            pc_serverData->canBus(ui8_bus).mi32_can_device = NULL;
            return false;
        }
#endif
        // Bind to the socket
        struct sockaddr_can addr;
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        int result = bind( m_handle, (struct sockaddr*)&addr, sizeof(addr) );

#ifdef DEBUG_CAN
        m_clog << "Init(): result: " << result << std::endl;
#endif
        if (result < 0)
        {
            perror( "bind" );
            pc_serverData->canBus(ui8_bus).mi32_can_device = NULL;
            return false;
        }

        // Enable loopback (ensure, because it may not be default in the future)
        int loopback = 1; /* 0 = disabled, 1 = enabled (default) */
        if( -1 == setsockopt( m_handle, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback)) ) {
            perror( "enabling loopback" );
            return false;
        }

        int recv_own_msgs = 0; /* 0 = disabled (default), 1 = enabled */
        if( -1 == setsockopt( m_handle, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof(recv_own_msgs)) ) {
            perror( "setting local echo option" );
            return false;
        }

        pc_serverData->canBus(ui8_bus).mi32_can_device = m_handle;
        canBusIsOpen[ui8_bus]  = true;

        return true;
    }
    else
        return true; // already initialized and files are already open
}


void closeBusOnCard(uint8_t ui8_bus, server_c* pc_serverData )
{
  canBusIsOpen[ui8_bus] = false;
  if( 0 > close( pc_serverData->canBus(ui8_bus).mi32_can_device ) ) {
      perror( "close" );
  }
}


void __HAL::updatePendingMsgs(server_c* /*pc_serverData*/, int8_t /*i8_bus*/)
{
}

// PURPOSE: To send a msg on the specified CAN BUS
// RETURNS: non-zero if msg was sent ok
//          0 on error
int16_t sendToBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
#ifdef DEBUG_CAN
  m_clog << "sendToBus(): bus: " << (int)ui8_bus << " isOpen: " << canBusIsOpen[ui8_bus] << std::endl;
#endif
  assert((ui8_bus <= HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus]);
  // Create the frame
  struct can_frame frame;

  frame.can_id = ps_canMsg->ui32_id;
  if (ps_canMsg->i32_msgType > 0)
    frame.can_id |= CAN_EFF_FLAG;   // set extended frame
  frame.can_dlc = ps_canMsg->i32_len;

  for( int i=0; i<frame.can_dlc; i++ )
    frame.data[i] = ps_canMsg->ui8_data[i];

  // ...and send the packet
  int bytes_sent = send(pc_serverData->canBus(ui8_bus).mi32_can_device, &frame, sizeof(frame), 0 );
  if (bytes_sent < 0)
  {
    perror( "send" );
#ifdef DEBUG_CAN
    m_clog << "sendToBus(): error, bytes_sent: " << bytes_sent << std::endl;
#endif
    return 0;
  }

#ifdef DEBUG_CAN
  char buf[200];
  m_clog << "sendToBus(): bytes_sent: " << bytes_sent << std::endl;
  sprintf(buf,"sendToBus(): %X,%X,%d,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X",
          frame.can_id, ps_canMsg->i32_msgType, frame.can_dlc, frame.data[0], frame.data[1], frame.data[2], frame.data[3], frame.data[4], frame.data[5], frame.data[6], frame.data[7]);
  m_clog << buf << std::endl;
#endif
  return 1;
}


bool readFromBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
#ifdef DEBUG_CAN
        m_clog << "readFromBus(): bus: " << (int)ui8_bus << " driverHandle: " << pc_serverData->canBus(ui8_bus).mi32_can_device << std::endl;
#endif
    if ((ui8_bus <= HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus])
    {
        struct can_frame frame;
        fd_set rfds;       // read file descriptor
        struct timeval tv;
        int retval;

        FD_ZERO(&rfds);
        FD_SET(pc_serverData->canBus(ui8_bus).mi32_can_device, &rfds);

        tv.tv_sec  = 0;
        tv.tv_usec = 0;

        retval = select(pc_serverData->canBus(ui8_bus).mi32_can_device+1, &rfds, NULL, NULL, &tv);
#ifdef DEBUG_CAN
        m_clog << "readFromBus(timeout): retval: " << retval << std::endl;
#endif
        if (retval <= 0)     // -1 on error, 0 on no data within timeout
        {
            if ( -1 == retval ) {
                perror( "select" );
            }
            return false;
        }

        int status = recv(pc_serverData->canBus(ui8_bus).mi32_can_device, &frame, sizeof(frame), 0);
#ifdef DEBUG_CAN
        m_clog << "readFromBus(): status: " << status << std::endl;
#endif
        if (status <= 0) {
            if ( -1 == status ) {
                perror( "read" );
            }
            return false;
        }

        ps_canMsg->ui32_id = frame.can_id & CAN_ERR_MASK;
        for(int i = 0; i < 8; ++i)
            ps_canMsg->ui8_data[i] = frame.data[i];
        ps_canMsg->i32_len = frame.can_dlc;
        ps_canMsg->i32_msgType = 0;
        if ((frame.can_id & CAN_EFF_FLAG) == CAN_EFF_FLAG)
            ps_canMsg->i32_msgType = 1;

#ifdef DEBUG_CAN
        char buf[200];
        m_clog << "readFromBus(): bytes recv: " << status << std::endl;
        sprintf(buf,"readFromBus(): %X,%X,%d,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X",
                ps_canMsg->ui32_id, ps_canMsg->i32_msgType, ps_canMsg->i32_len, frame.data[0], frame.data[1], frame.data[2], frame.data[3], frame.data[4], frame.data[5], frame.data[6], frame.data[7]);
        m_clog << buf << std::endl;
#endif

        return true;
    }
    return false;
}

void addSendTimeStampToList(client_c* /*ps_client*/, int32_t /*i32_sendTimeStamp*/)
{
}

