/***************************************************************************
              can_server_pcan.cpp -
                    interface for PEAK can card

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
  #include <Pcan_usb.h>
#else
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <sys/ioctl.h>
#endif

#include "can_server.h"

using namespace __HAL;

#ifndef WIN32
  #ifdef USE_PCAN_LIB
    #include <libpcan.h>
    HANDLE driverHandle[cui32_maxCanBusCnt];
  #else
    #include <pcan.h>
  #endif
#endif

#ifndef WIN32
  // device nodes minor base. Must be the same as defined in the driver
  #ifndef PCAN_MSCAN_MINOR_BASE
    // (USB driver)
    #define PCAN_MSCAN_MINOR_BASE     32
    // (file pcan_mpc5200.c)
    //#define PCAN_MSCAN_MINOR_BASE     40
  #endif
#endif

static bool  canBusIsOpen[cui32_maxCanBusCnt];

bool isBusOpen(uint8_t ui8_bus)
{
  return canBusIsOpen[ui8_bus];
}

uint32_t initCardApi ()
{
  for( uint32_t i=0; i<cui32_maxCanBusCnt; i++ )
  {
    canBusIsOpen[i] = false;
    driverHandle[i] = NULL;
  }

  return 1;
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

#ifdef USE_PCAN_LIB
    driverHandle[ui8_bus] = LINUX_CAN_Open(fname, O_RDWR | O_NONBLOCK);
    pc_serverData->can_device[ui8_bus] = LINUX_CAN_FileHandle(driverHandle[ui8_bus]);
    if ( driverHandle[ui8_bus] == NULL ) {
      std::cerr << "Open CAN Fault" << std::endl;
      return false;
    }
#else
    pc_serverData->marri16_can_device[ui8_bus] = open(fname, O_RDWR | O_NONBLOCK);
    if (pc_serverData->marri16_can_device[ui8_bus] == -1) {
      DEBUG_PRINT1("Could not open CAN bus %d\n",ui8_bus);
      return false;
    }
#endif

#ifdef USE_PCAN_LIB
    WORD useBtr = LINUX_CAN_BTR0BTR1(driverHandle[ui8_bus], wBitrate*1000);
    if (CAN_Init(driverHandle[ui8_bus], useBtr, 2) < 0) {
      std::cerr << "Init Problem" << std::endl;
      return false;
    }
#else
    TPBTR0BTR1 ratix;
    TPCANInit init;

    // init wBitrate
    DEBUG_PRINT1("Init Bitrate with PCAN_BTR0BTR1 wBitrate =%d\n",wBitrate*1000);
    ratix.dwBitRate = wBitrate * 1000;
    ratix.wBTR0BTR1 = 0;
    if ((ioctl(pc_serverData->marri16_can_device[ui8_bus], PCAN_BTR0BTR1, &ratix)) < 0)
      return false;

    // init CanMsgType (if extended Can Msg of not)
    DEBUG_PRINT1("Init CAN Driver with PCAN_INIT wBitrate =%x\n",ratix.wBTR0BTR1);
    //default value = extended
    init.wBTR0BTR1    = ratix.wBTR0BTR1;
    init.ucCANMsgType = MSGTYPE_EXTENDED;  // 11 or 29 bits
    init.ucListenOnly = 0;            // listen only mode when != 0
    if ((ioctl(pc_serverData->marri16_can_device[ui8_bus], PCAN_INIT, &init)) < 0)
      return false;
#endif
    canBusIsOpen[ui8_bus] = true;

    return true;
#endif
  }
  else
    return true; // already initialized and files are already open

}

void closeBusOnCard(uint8_t ui8_bus, server_c* /*pc_serverData*/)
{
  DEBUG_PRINT1("close can bus %d\n", ui8_bus);
  //canBusIsOpen[ui8_bus] = false;
  // do not call close or CAN_CLOSE because COMMAND_CLOSE is received during initialization!
}


void __HAL::updatePendingMsgs(server_c* pc_serverData, int8_t i8_bus)
{
  // get amount of waiting-to-be-sent-out can-msgs in can-controller
  static TPEXTENDEDSTATUS extstat;
  if (i8_bus < 0)
  { // update all buses!
    for (uint8_t ui8_bus=0; ui8_bus < cui32_maxCanBusCnt; ui8_bus++)
    {
      if (pc_serverData->marri_pendingMsgs[ui8_bus] >= 5)
      { // we only need to update those who could change from >= 5 to < 5...
#if 0 //def USE_PCAN_LIB
        if (LINUX_CAN_Extended_Status(driverHandle[ui8_bus], &(extstat.nPendingReads), &(extstat.nPendingWrites))) continue;
#else
        if ((ioctl(pc_serverData->marri16_can_device[ui8_bus], PCAN_GET_EXT_STATUS, &extstat)) < 0) continue;
#endif
        pc_serverData->marri_pendingMsgs[ui8_bus] = extstat.nPendingWrites;
        DEBUG_PRINT1 ("peak-can's number of pending msgs is %d\n", pc_serverData->marri_pendingMsgs[ui8_bus]);
      }
    }
  }
  else
  { // update just the given bus!
#if 0 //def USE_PCAN_LIB
    if (LINUX_CAN_Extended_Status(driverHandle[ui8_bus], &(extstat.nPendingReads), &(extstat.nPendingWrites))) return;
#else
    if ((ioctl(pc_serverData->marri16_can_device[i8_bus], PCAN_GET_EXT_STATUS, &extstat)) < 0) return;
#endif
    pc_serverData->marri_pendingMsgs[i8_bus] = extstat.nPendingWrites;
    DEBUG_PRINT1 ("peak-can's number of pending msgs is %d\n", pc_serverData->marri_pendingMsgs[i8_bus]);
  }
}

// PURPOSE: To send a msg on the specified CAN BUS
// RETURNS: non-zero if msg was sent ok
//          0 on error
int16_t sendToBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
#if WIN32
  DWORD rc;
  TPCANMsg msg;

  msg.ID = ps_canMsg->ui32_id;
  msg.MSGTYPE = (ps_canMsg->i32_msgType ? MSGTYPE_EXTENDED : MSGTYPE_STANDARD );
  msg.LEN = ps_canMsg->i32_len;

  for( int i=0; i<msg.LEN; i++ )
    msg.DATA[i] = ps_canMsg->ui8_data[i];

  if ((ui8_bus < HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus])
  {
    rc = CAN_Write(&msg);
    // printf("CAN_write rc: %x, ID %x, len %d, data %x %x %x %x %x %x %x %x\n", rc, msg.ID, msg.LEN, msg.DATA[0], msg.DATA[1], msg.DATA[2], msg.DATA[3], msg.DATA[4], msg.DATA[5], msg.DATA[6], msg.DATA[7]);
    if (CAN_ERR_OK == rc)
      return 1;
  }
  return 0;

#else

  TPCANMsg msg;
  msg.ID = ps_canMsg->ui32_id;
  msg.MSGTYPE = ( ps_canMsg->i32_msgType ? MSGTYPE_EXTENDED : MSGTYPE_STANDARD );
  msg.LEN = ps_canMsg->i32_len;

  for( int i=0; i<msg.LEN; i++ )
    msg.DATA[i] = ps_canMsg->ui8_data[i];

  int ret = 0;

  if ((ui8_bus < HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus]) {
#if 0 //def USE_PCAN_LIB
    ret = CAN_Write(driverHandle[ui8_bus], &msg);
#else
    ret = ioctl(pc_serverData->marri16_can_device[ui8_bus], PCAN_WRITE_MSG, &msg);
#endif

    if (ret < 0)
    {
      perror("sendToBus ioctl");
      return 0;
    }

    updatePendingMsgs(pc_serverData, ui8_bus);
    int i_pendingMsgs = pc_serverData->i_pendingMsgs[ui8_bus];
    if ((i_pendingMsgs > 0) && (list_sendTimeStamps.size() >= (i_pendingMsgs)))
    { // something pending!
      STL_NAMESPACE::list<int32_t>::iterator pc_iter = list_sendTimeStamps.begin();
      i_pendingMsgs--; // we're >0 at the beginning!
      while (i_pendingMsgs)
      {
        pc_iter++;
        i_pendingMsgs--;
      }
      const int ci_delay = getTime() - (*pc_iter);
      pc_iter++; // go to the one that's been already sent and remove from list!
      while (pc_iter != list_sendTimeStamps.end())
      { // remove all the timestamps of the already sent messages!
        pc_iter = list_sendTimeStamps.erase (pc_iter);
      }
      // do we have a new max for this bus?
      DEBUG_PRINT3 ("target_extension_can_server_pcan::ca_TransmitCanCard_1: SEND_DELAY WAS: %d    -- max for bus %d is: %d\n", ci_delay, ui8_bus, pc_serverData->marri32_sendDelay[ui8_bus]);
      if (ci_delay > pc_serverData->marri32_sendDelay[ui8_bus])
      { // yes we do, so set it!
        pc_serverData->marri32_sendDelay[ui8_bus] = ci_delay;
        DEBUG_PRINT ("target_extension_can_server_pcan::ca_TransmitCanCard_1: reporting back HAL_NEW_SEND_DELAY");
        return HAL_NEW_SEND_DELAY;
      }
    }

    return 1;
  }
  return 0;

#endif
}

uint32_t readFromBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
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
      ps_canMsg->ui32_id = msg.ID;

      if (msg.MSGTYPE > 0)
        ps_canMsg->i32_msgType = 1;
      else
        ps_canMsg->i32_msgType = 0;

      ps_canMsg->i32_len = msg.LEN;

      memcpy(ps_canMsg->ui8_data, msg.DATA, msg.LEN );

      return msg.LEN;
    }
  }

#else

  TPCANRdMsg msg;
#ifdef USE_PCAN_LIB
  int ret = LINUX_CAN_Read(driverHandle[ui8_bus], &msg);
#else
  int ret = ioctl(pc_serverData->marri16_can_device[ui8_bus], PCAN_READ_MSG, &msg);
#endif

  if (ret < 0)
    return 0;

  if (msg.Msg.ID == 0)
    // invalid message
    return 0;

  ps_canMsg->ui32_id = msg.Msg.ID;
  ps_canMsg->i32_msgType = (msg.Msg.MSGTYPE ? 1 : 0);
  ps_canMsg->i32_len = msg.Msg.LEN;

  memcpy( ps_canMsg->ui8_data, msg.Msg.DATA, msg.Msg.LEN );

  return msg.Msg.LEN;

#endif

  return 0;

}

int32_t getServerTimeFromClientTime( client_c& r_receiveClient, int32_t ai32_clientTime )
{
  return ai32_clientTime + r_receiveClient.i32_msecStartDeltaClientMinusServer;
}


void addSendTimeStampToList(client_c *ps_client, int32_t i32_sendTimeStamp)
{
  list_sendTimeStamps.push_front (getServerTimeFromClientTime (*ps_client, i32_sendTimeStamp));
}

