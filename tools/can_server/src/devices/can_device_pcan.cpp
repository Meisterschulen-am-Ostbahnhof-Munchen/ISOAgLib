/***************************************************************************
              can_device_pcan.cpp -
                    interface for PEAK can card

                             -------------------
    begin                : Tue Oct 2 2001
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                  *
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

#include "can_server.h"
#include "../can_server_common.h"

#ifdef WIN32
  #include <windows.h>
  #include <Pcan_usb.h>
#else
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <sys/ioctl.h>
#endif

using namespace __HAL;

#ifndef WIN32
  #include <pcan.h>

  // device nodes minor base. Must be the same as defined in the driver
  #ifndef PCAN_MSCAN_MINOR_BASE
    // (USB driver)
    #ifdef SYSTEM_A5
      #define PCAN_MSCAN_MINOR_BASE     0
    #else
      #define PCAN_MSCAN_MINOR_BASE     32
      // (file pcan_mpc5200.c)
      //#define PCAN_MSCAN_MINOR_BASE     40
    #endif
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
  /* 
   * ATTENTION!
   *
   * the PCAN multi card abilities are currently unused. Working with several (can_server)busses
   * and less devices plugged causes random distribution of incomming messages.
   *
   * This is a temporary workarround!
   *
   */
    if( 0 == ui8_bus ) {
      DWORD rc;
      rc = CAN_Init(CAN_BAUD_250K, 1 );  // Baudrate
      if (CAN_ERR_OK == rc)
      {
        canBusIsOpen[ui8_bus] = true;
        pc_serverData->marrb_deviceConnected[ui8_bus] = true;
        return true;
      }
      else {
        std::cerr << "Open CAN Fault with return-code: " << rc << std::endl;
        return false;
      }
    }
    else 
    {
      /* wrong bus - but that's ok - see above */
      return true;
    }
#else

    char fname[32];
    sprintf( fname, "/dev/pcan%u", PCAN_MSCAN_MINOR_BASE + ui8_bus );

    pc_serverData->marri32_can_device[ui8_bus] = open(fname, O_RDWR | O_NONBLOCK);
    if (pc_serverData->marri32_can_device[ui8_bus] == -1) {
      DEBUG_PRINT1("Could not open CAN bus %d\n",ui8_bus);
      return false;
    }

    TPBTR0BTR1 ratix;
    TPCANInit init;

    // init wBitrate
    DEBUG_PRINT1("Init Bitrate with PCAN_BTR0BTR1 wBitrate =%d\n",wBitrate*1000);
    ratix.dwBitRate = wBitrate * 1000;
    ratix.wBTR0BTR1 = 0;
    if ((ioctl(pc_serverData->marri32_can_device[ui8_bus], PCAN_BTR0BTR1, &ratix)) < 0)
      return false;

    // init CanMsgType (if extended Can Msg of not)
    DEBUG_PRINT1("Init CAN Driver with PCAN_INIT wBitrate =%x\n",ratix.wBTR0BTR1);
    //default value = extended
    init.wBTR0BTR1    = ratix.wBTR0BTR1;
    init.ucCANMsgType = MSGTYPE_EXTENDED;  // 11 or 29 bits
    init.ucListenOnly = 0;            // listen only mode when != 0
    if ((ioctl(pc_serverData->marri32_can_device[ui8_bus], PCAN_INIT, &init)) < 0)
      return false;

    canBusIsOpen[ui8_bus] = true;
    pc_serverData->marrb_deviceConnected[ui8_bus] = true;

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
#ifndef WIN32
  // get amount of waiting-to-be-sent-out can-msgs in can-controller
  static TPEXTENDEDSTATUS extstat;
  if (i8_bus < 0)
  { // update all buses!
    for (uint8_t ui8_bus=0; ui8_bus < cui32_maxCanBusCnt; ui8_bus++)
    {
      if (pc_serverData->marri_pendingMsgs[ui8_bus] >= 5)
      { // we only need to update those who could change from >= 5 to < 5...
        if ((ioctl(pc_serverData->marri32_can_device[ui8_bus], PCAN_GET_EXT_STATUS, &extstat)) < 0) continue;
        pc_serverData->marri_pendingMsgs[ui8_bus] = extstat.nPendingWrites;
        DEBUG_PRINT1 ("peak-can's number of pending msgs is %d\n", pc_serverData->marri_pendingMsgs[ui8_bus]);
      }
    }
  }
  else
  { // update just the given bus!
    if ((ioctl(pc_serverData->marri32_can_device[i8_bus], PCAN_GET_EXT_STATUS, &extstat)) < 0) return;
    pc_serverData->marri_pendingMsgs[i8_bus] = extstat.nPendingWrites;
    DEBUG_PRINT1 ("peak-can's number of pending msgs is %d\n", pc_serverData->marri_pendingMsgs[i8_bus]);
  }
#endif
}

// PURPOSE: To send a msg on the specified CAN BUS
// RETURNS: non-zero if msg was sent ok
//          0 on error
int16_t sendToBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
#if WIN32
  /* 
   * ATTENTION!
   *
   * the PCAN multi card abilities are currently unused. Working with several (can_server)busses
   * and less devices plugged causes random distribution of incomming messages.
   *
   * This is a temporary workarround!
   *
   */
  if( 0 == ui8_bus ) {
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
  } else {
    return 1;
  }

#else

  TPCANMsg msg;
  msg.ID = ps_canMsg->ui32_id;
  msg.MSGTYPE = ( ps_canMsg->i32_msgType ? MSGTYPE_EXTENDED : MSGTYPE_STANDARD );
  msg.LEN = ps_canMsg->i32_len;

  for( int i=0; i<msg.LEN; i++ )
    msg.DATA[i] = ps_canMsg->ui8_data[i];

  int ret = 0;

  if ((ui8_bus < HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus]) {
    ret = ioctl(pc_serverData->marri32_can_device[ui8_bus], PCAN_WRITE_MSG, &msg);
    if (ret < 0)
    {
      perror("sendToBus ioctl");
      // return success to IsoAgLib, because can_server_msq returns otherwise HAL_OVERFLOW_ERR to upper layer
      // but can_server should continue to work even when the CAN device does not work correctly
      return 1;
    }

    updatePendingMsgs(pc_serverData, ui8_bus);
    unsigned int i_pendingMsgs = pc_serverData->marri_pendingMsgs[ui8_bus];
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

bool readFromBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
#if WIN32
  TPCANMsg msg;
  /* 
   * ATTENTION!
   *
   * the PCAN multi card abilities are currently unused. Working with several (can_server)busses
   * and less devices plugged causes random distribution of incomming messages.
   *
   * This is a temporary workarround!
   *
   */
  if( 0 == ui8_bus ) {
    DWORD rc;

    rc = CAN_Read(&msg);
    if (CAN_ERR_OK != rc) 
    {
      return false;
    }
  } else {
    /* bus number not 0 - we can't read anything here */
    return false;
  }
#else
  TPCANRdMsg msgRd;
  int ret = ioctl(pc_serverData->marri32_can_device[ui8_bus], PCAN_READ_MSG, &msgRd);
  if (ret < 0)
    return false;

  TPCANMsg& msg = msgRd.Msg;
#endif

  switch (msg.MSGTYPE)
  {
    case MSGTYPE_STANDARD:
    case MSGTYPE_EXTENDED:
      break; // process
    default: // don't process status, RTR or other messages
      return false;
  }

  ps_canMsg->ui32_id = msg.ID;
  ps_canMsg->i32_msgType = (msg.MSGTYPE ? 1 : 0);
  ps_canMsg->i32_len = msg.LEN;

  memcpy( ps_canMsg->ui8_data, msg.DATA, msg.LEN );

  return true;
}

int32_t getServerTimeFromClientTime( client_c& r_receiveClient, int32_t ai32_clientTime )
{
  return ai32_clientTime + r_receiveClient.i32_msecStartDeltaClientMinusServer;
}


void addSendTimeStampToList(client_c *ps_client, int32_t i32_sendTimeStamp)
{
  list_sendTimeStamps.push_front (getServerTimeFromClientTime (*ps_client, i32_sendTimeStamp));
}

