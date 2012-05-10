/*
  can_device_pcan.cpp: interface for PEAK can card

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "wrapper_for_can_server_interface.h"
#include "can_server_common.h"
#include <string.h>
#include <iostream>
#include <assert.h>

#ifdef WIN32
  #include <windows.h>
  #include <Pcan_usb.h>
#else
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <sys/ioctl.h>
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


using namespace __HAL;

static struct canDevice_s {
  struct canBus_s {
    bool          mb_canBusIsOpen;
    canBus_s();
  };
  canBus_s &canBus(size_t n_index);
  size_t nCanBusses();

private:
  std::vector< canBus_s > mvec_canBus;
} ss_canDevice;

inline canDevice_s::canBus_s &canDevice_s::canBus(size_t n_index)
{
  if (mvec_canBus.size() <= n_index)
    mvec_canBus.resize(n_index + 1);
  return mvec_canBus[n_index];
}

inline size_t canDevice_s::nCanBusses()
{
  return mvec_canBus.size();
}

canDevice_s::canBus_s::canBus_s() :
  mb_canBusIsOpen(false)
{
}

bool isBusOpen(uint8_t ui8_bus)
{
  return ss_canDevice.canBus(ui8_bus).mb_canBusIsOpen;
}

uint32_t initCardApi ()
{
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

  if( !ss_canDevice.canBus(ui8_bus).mb_canBusIsOpen ) {
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
        ss_canDevice.canBus(ui8_bus).mb_canBusIsOpen = true;
        pc_serverData->canBus(ui8_bus).mb_deviceConnected = true;
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

    pc_serverData->canBus(ui8_bus).mi32_can_device = open(fname, O_RDWR | O_NONBLOCK);
    if (pc_serverData->canBus(ui8_bus).mi32_can_device == -1) {
      DEBUG_PRINT1("Could not open CAN bus %d\n",ui8_bus);
      return false;
    }

    TPBTR0BTR1 ratix;
    TPCANInit init;

    // init wBitrate
    DEBUG_PRINT1("Init Bitrate with PCAN_BTR0BTR1 wBitrate =%d\n",wBitrate*1000);
    ratix.dwBitRate = wBitrate * 1000;
    ratix.wBTR0BTR1 = 0;
    if ((ioctl(pc_serverData->canBus(ui8_bus).mi32_can_device, PCAN_BTR0BTR1, &ratix)) < 0)
      return false;

    // init CanMsgType (if extended Can Msg of not)
    DEBUG_PRINT1("Init CAN Driver with PCAN_INIT wBitrate =%x\n",ratix.wBTR0BTR1);
    //default value = extended
    init.wBTR0BTR1    = ratix.wBTR0BTR1;
    init.ucCANMsgType = MSGTYPE_EXTENDED;  // 11 or 29 bits
    init.ucListenOnly = 0;            // listen only mode when != 0
    if ((ioctl(pc_serverData->canBus(ui8_bus).mi32_can_device, PCAN_INIT, &init)) < 0)
      return false;

    ss_canDevice.canBus(ui8_bus).mb_canBusIsOpen = true;
    pc_serverData->canBus(ui8_bus).mb_deviceConnected = true;

    return true;
#endif
  }
  else
    return true; // already initialized and files are already open

}

void closeBusOnCard(uint8_t ui8_bus, server_c* /*pc_serverData*/)
{
  (void)ui8_bus;
  DEBUG_PRINT1("close can bus %d\n", ui8_bus);
  //ss_canDevice.canBus(ui8_bus).mb_canBusIsOpen = false;
  // do not call close or CAN_CLOSE because COMMAND_CLOSE is received during initialization!
}


void __HAL::updatePendingMsgs(server_c* pc_serverData, int8_t i8_bus)
{
#ifndef WIN32
  // get amount of waiting-to-be-sent-out can-msgs in can-controller
  static TPEXTENDEDSTATUS extstat;
  if (i8_bus < 0)
  { // update all buses!
    for (uint8_t ui8_bus=0; ui8_bus < ss_canDevice.nCanBusses(); ui8_bus++)
    {
      if (pc_serverData->canBus(ui8_bus).mi_pendingMsgs >= 5)
      { // we only need to update those who could change from >= 5 to < 5...
        if ((ioctl(pc_serverData->canBus(ui8_bus).mi32_can_device, PCAN_GET_EXT_STATUS, &extstat)) < 0) continue;
        pc_serverData->canBus(ui8_bus).mi_pendingMsgs = extstat.nPendingWrites;
        DEBUG_PRINT1 ("peak-can's number of pending msgs is %d\n", pc_serverData->canBus(ui8_bus).mi_pendingMsgs);
      }
    }
  }
  else
  { // update just the given bus!
    if ((ioctl(pc_serverData->canBus(i8_bus).mi32_can_device, PCAN_GET_EXT_STATUS, &extstat)) < 0) return;
    pc_serverData->canBus(i8_bus).mi_pendingMsgs = extstat.nPendingWrites;
    DEBUG_PRINT1 ("peak-can's number of pending msgs is %d\n", pc_serverData->canBus(i8_bus).mi_pendingMsgs);
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

    assert((ui8_bus <= HAL_CAN_MAX_BUS_NR) && ss_canDevice.canBus(ui8_bus).mb_canBusIsOpen);
    rc = CAN_Write(&msg);
    // printf("CAN_write rc: %x, ID %x, len %d, data %x %x %x %x %x %x %x %x\n", rc, msg.ID, msg.LEN, msg.DATA[0], msg.DATA[1], msg.DATA[2], msg.DATA[3], msg.DATA[4], msg.DATA[5], msg.DATA[6], msg.DATA[7]);
    if (CAN_ERR_OK == rc)
      return 1;
    else
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

  // should have been checked already by calling function isBusOpen:
  assert((ui8_bus <= HAL_CAN_MAX_BUS_NR) && ss_canDevice.canBus(ui8_bus).mb_canBusIsOpen);
  ret = ioctl(pc_serverData->canBus(ui8_bus).mi32_can_device, PCAN_WRITE_MSG, &msg);
  if (ret < 0)
  {
    perror("sendToBus ioctl");
    // CAN server depends on that to suppress call of
    // addSendTimeStampToList:
    return 0;
  }

  updatePendingMsgs(pc_serverData, ui8_bus);
  unsigned int i_pendingMsgs = pc_serverData->canBus(ui8_bus).mi_pendingMsgs;
  if ((i_pendingMsgs > 0) && (list_sendTimeStamps.size() >= (i_pendingMsgs)))
  { // something pending!
    std::list<int32_t>::iterator pc_iter = list_sendTimeStamps.begin();
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
    DEBUG_PRINT3 ("target_extension_can_server_pcan::ca_TransmitCanCard_1: SEND_DELAY WAS: %d    -- max for bus %d is: %d\n", ci_delay, ui8_bus, pc_serverData->canBus(ui8_bus).mi32_sendDelay);
    if (ci_delay > pc_serverData->canBus(ui8_bus).mi32_sendDelay)
    { // yes we do, so set it!
      pc_serverData->canBus(ui8_bus).mi32_sendDelay = ci_delay;
      DEBUG_PRINT ("target_extension_can_server_pcan::ca_TransmitCanCard_1: reporting back HAL_NEW_SEND_DELAY");
      return HAL_NEW_SEND_DELAY;
    }
  }

  return 1;
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
  int ret = ioctl(pc_serverData->canBus(ui8_bus).mi32_can_device, PCAN_READ_MSG, &msgRd);
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

  (void) memcpy( ps_canMsg->ui8_data, msg.DATA, msg.LEN );

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

