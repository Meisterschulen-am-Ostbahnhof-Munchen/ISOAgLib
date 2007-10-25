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

#ifdef WIN32
  #include <windows.h>
  #include "Pcan_usb.h"
#else
  #include <pcan.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <sys/ioctl.h>
#endif

#include "can_server_sock.h"
using namespace __HAL;

#ifndef WIN32
  // device nodes minor base. Must be the same as defined in the driver (USB driver).
  #ifndef PCAN_MSCAN_MINOR_BASE
    #define PCAN_MSCAN_MINOR_BASE     32
  #endif
#endif

static bool  canBusIsOpen[cui32_maxCanBusCnt];

bool isBusOpen(uint8_t ui8_bus)
{
  printf("_____isBusOpen\n");
  return canBusIsOpen[ui8_bus];
}

uint32_t initCardApi ()
{
  for( uint32_t i=0; i<cui32_maxCanBusCnt; i++ )
  {
    canBusIsOpen[i] = false;
  }

  return true;
}

bool resetCard(void)
{
  return true;
}

// PURPOSE: To initialize the specified CAN BUS to begin sending/receiving msgs
bool openBusOnCard(uint8_t ui8_bus, uint32_t wBitrate, server_c* pc_serverData)
{
  DEBUG_PRINT1("init can bus %d\n", ui8_bus);

  if( !canBusIsOpen[ui8_bus] ) {
    DEBUG_PRINT1("Opening CAN BUS channel=%d\n", ui8_bus);

#if WIN32
    DWORD rc;
    rc = CAN_Init(CAN_BAUD_250K, 1);  // Baudrate
    printf("Init can driver: %x\n", rc);
    if (CAN_ERR_OK == rc)
    {
      canBusIsOpen[ui8_bus] = true;
      return true;
    }
    else
      return false;

#else

    char fname[32];
    sprintf( fname, "/dev/pcan%u", PCAN_MSCAN_MINOR_BASE + ui8_bus );

    pc_serverData->can_device[ui8_bus] = open(fname, O_RDWR | O_NONBLOCK);
    if (pc_serverData->can_device[ui8_bus] == -1) {
      DEBUG_PRINT1("Could not open CAN bus %d\n",ui8_bus);
      return false;
    }

    TPBTR0BTR1 ratix;
    TPCANInit init;

    // init wBitrate
    DEBUG_PRINT1("Init Bitrate with PCAN_BTR0BTR1 wBitrate =%d\n",wBitrate*1000);
    ratix.dwBitRate = wBitrate * 1000;
    ratix.wBTR0BTR1 = 0;
    if ((ioctl(pc_serverData->can_device[ui8_bus], PCAN_BTR0BTR1, &ratix)) < 0)
      return false;

    // init CanMsgType (if extended Can Msg of not)
    DEBUG_PRINT1("Init CAN Driver with PCAN_INIT wBitrate =%x\n",ratix.wBTR0BTR1);
    //default value = extended
    init.wBTR0BTR1    = ratix.wBTR0BTR1;
    init.ucCANMsgType = MSGTYPE_EXTENDED;  // 11 or 29 bits
    init.ucListenOnly = 0;            // listen only mode when != 0
    if ((ioctl(pc_serverData->can_device[ui8_bus], PCAN_INIT, &init)) < 0)
      return false;

    canBusIsOpen[ui8_bus] = true;

    return true;
#endif
  }
  else
    return true; // already initialized and files are already open

}

void closeBusOnCard(uint8_t ui8_bus, server_c* pc_serverData)
{
  DEBUG_PRINT1("close can bus %d\n", ui8_bus);
  //canBusIsOpen[ui8_bus] = false;
  // do not call close or CAN_CLOSE because COMMAND_CLOSE is received during initialization!
}


void __HAL::updatePendingMsgs(server_c* /* pc_serverData */, int8_t /* i8_bus */)
{
/// @todo not implemented for right now on A1!
}

// PURPOSE: To send a msg on the specified CAN BUS
// RETURNS: non-zero if msg was sent ok
//      0 on error
bool sendToBus(uint8_t ui8_bus, socketBuf_s* p_sockBuf, server_c* pc_serverData)
{
#if WIN32
  DWORD rc;
  TPCANMsg msg;

  msg.ID = p_sockBuf->s_data.dwId;
  msg.MSGTYPE = (p_sockBuf->s_data.bXtd ? MSGTYPE_EXTENDED : MSGTYPE_STANDARD );
  msg.LEN = p_sockBuf->s_data.bDlc;

  for( int i=0; i<msg.LEN; i++ )
    msg.DATA[i] = p_sockBuf->s_data.abData[i];

  if ((ui8_bus < HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus])
  {
    rc = CAN_Write(&msg);
    // printf("CAN_write rc: %x, ID %x, len %d, data %x %x %x %x %x %x %x %x\n", rc, msg.ID, msg.LEN, msg.DATA[0], msg.DATA[1], msg.DATA[2], msg.DATA[3], msg.DATA[4], msg.DATA[5], msg.DATA[6], msg.DATA[7]);
    if (CAN_ERR_OK == rc)
      return true;
  }
  return false;

#else

  TPCANMsg msg;
  msg.ID = p_sockBuf->s_data.dwId;
  msg.MSGTYPE = ( p_sockBuf->s_data.bXtd ? MSGTYPE_EXTENDED : MSGTYPE_STANDARD );
  msg.LEN = p_sockBuf->s_data.bDlc;

  for( int i=0; i<msg.LEN; i++ )
    msg.DATA[i] = p_sockBuf->s_data.abData[i];

  int ret = 0;

  if ((ui8_bus < HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus]) {
    ret = ioctl(pc_serverData->can_device[ui8_bus], PCAN_WRITE_MSG, &msg);
    return true;
  }
  return false;

#endif
}

uint32_t readFromBus(uint8_t ui8_bus, socketBuf_s* p_sockBuf, server_c* pc_serverData)
{
#if WIN32
  DWORD rc;
  TPCANMsg msg;

  rc = CAN_Read(&msg);
  if (CAN_ERR_OK == rc)
  {
    if (msg.MSGTYPE == MSGTYPE_EXTENDED) // MSGTYPE_EXTENDED == 2 !
    {
      //printf("CAN_receive ID %x, len %d, data %x %x %x %x %x %x %x %x\n", msg.ID, msg.LEN, msg.DATA[0], msg.DATA[1], msg.DATA[2], msg.DATA[3], msg.DATA[4], msg.DATA[5], msg.DATA[6], msg.DATA[7]);
      p_sockBuf->s_data.dwId = msg.ID;

      if (msg.MSGTYPE > 0)
        p_sockBuf->s_data.bXtd = 1;
      else
        p_sockBuf->s_data.bXtd = 0;

      p_sockBuf->s_data.bDlc = msg.LEN;

      memcpy(p_sockBuf->s_data.abData, msg.DATA, msg.LEN );

      return msg.LEN;
    }
  }

#else

  TPCANRdMsg msg;
  int ret = ioctl(pc_serverData->can_device[ui8_bus], PCAN_READ_MSG, &msg);

  if (ret < 0)
    return 0;

  p_sockBuf->s_data.dwId = msg.Msg.ID;
  p_sockBuf->s_data.bXtd = (msg.Msg.MSGTYPE ? 1 : 0);
  p_sockBuf->s_data.bDlc = msg.Msg.LEN;

  memcpy( p_sockBuf->s_data.abData, msg.Msg.DATA, msg.Msg.LEN );

  return msg.Msg.LEN;

#endif

  return 0;

}

void addSendTimeStampToList(client_c * /*ps_client*/, int32_t /*i32_sendTimeStamp*/)
{
}

