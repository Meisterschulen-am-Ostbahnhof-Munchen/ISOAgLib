/***************************************************************************
              can_device_vector_xl.cpp -
                    interface for Vector-Informatik can card (xl)

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
#define  STRICT

#include <stdlib.h>
#include <ctype.h>

#ifdef WIN32
#include <dos.h>

/* Typedef taken from windows.h to avoid complete windows inclusion: */
typedef void *HANDLE;
extern "C" {
  #include <vxlapi.h>
}

#endif

#include "can_server.h"
#include "../can_server_common.h"
#include <assert.h>

#define XL_HWTYPE_AUTO 1000

using namespace __HAL;

// CAN Globals
int AllCanChannelCount = 0;

char            g_AppName[XL_MAX_LENGTH+1]  = "IsoAgLib";   //!< Application name which is displayed in VHWconf
XLdriverConfig  g_xlDrvConfig;                              //!< Contains the actual hardware configuration
int16_t gChannel = 0;

XLportHandle    g_xlPortHandle[cui32_maxCanBusCnt]     ;    //!< Global porthandle (we use only one!)
XLaccess        g_xlChannelMask[cui32_maxCanBusCnt]    ;    //!< Global channelmask (includes all founded channels)
XLaccess        g_xlPermissionMask[cui32_maxCanBusCnt] ;    //!< Global permissionmask (includes all founded channels)
XLaccess        g_xlInitMask[cui32_maxCanBusCnt];

#ifdef USE_CAN_CARD_TYPE
uint16_t gHwType = USE_CAN_CARD_TYPE;
#else
// select the Vector CAN card type to use
//
uint16_t gHwType = XL_HWTYPE_AUTO;
#endif

static bool  canBusIsOpen[cui32_maxCanBusCnt];

static void printDriverConfig( void )
{
  unsigned int i;
  char         str[XL_MAX_LENGTH + 1]="";

  printf("----------------------------------------------------------\n");
  printf("- %02d channels       Hardware Configuration               -\n", g_xlDrvConfig.channelCount);
  printf("----------------------------------------------------------\n");

  for (i=0; i < g_xlDrvConfig.channelCount; i++) {

    printf("- Ch.: %02d, CM:0x%3I64x,",
      g_xlDrvConfig.channel[i].channelIndex, g_xlDrvConfig.channel[i].channelMask);
    printf(" %20s ", g_xlDrvConfig.channel[i].name);
    if (g_xlDrvConfig.channel[i].transceiverType != XL_TRANSCEIVER_TYPE_NONE) {
      strncpy( str, g_xlDrvConfig.channel[i].transceiverName, 13 );
      printf(" %3s -\n", str);
    }
    else printf(" no Cab!       -\n", str);

  }
  //0x%I64x

  printf("----------------------------------------------------------\n\n");

}


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

  #ifdef USE_CAN_CARD_TYPE
  gHwType = USE_CAN_CARD_TYPE;
  #else
  // select the Vector CAN card type to use
  gHwType = XL_HWTYPE_AUTO;
  #endif

  // open the driver
  XLstatus xlStatus;
  xlStatus = xlOpenDriver();
  if (xlStatus)
    goto error;

  uint32_t ind;
  for ( ind = 0; ind < cui32_maxCanBusCnt; ind++ )
  {
    g_xlPortHandle[ind]     = XL_INVALID_PORTHANDLE;  //!< Global porthandle (we use only one!)
    g_xlChannelMask[ind]    = 0;                      //!< Global channelmask (includes all founded channels)
    g_xlPermissionMask[ind] = 0;                      //!< Global permissionmask (includes all founded channels)
    g_xlInitMask[ind]       = 0;
    canBusIsOpen[ind]    = false;
  }

  printf("xlDriverConfig()\n");
  xlStatus = xlGetDriverConfig(&g_xlDrvConfig);
  if (xlStatus) return 0;

  printf(" %u channels found\n",g_xlDrvConfig.channelCount);
  printDriverConfig();

  return 1;

  error:
    printf("ERROR: %s!\n", xlGetErrorString(xlStatus));
    return 0;
}

bool resetCard(void)
{
  xlCloseDriver();
  return true;
}


bool openBusOnCard(uint8_t ui8_bus, uint32_t wBitrate, server_c* pc_serverData)
{
  DEBUG_PRINT1("init can bus %d\n", ui8_bus);
  XLstatus xlStatus;

  if( !canBusIsOpen[ui8_bus] ) {
    DEBUG_PRINT1("Opening CAN BUS channel=%d\n", ui8_bus);

    int32_t i32_busInd = -1, i32_virtualBusInd = -1;
    XLaccess virtualChannelMask = 0;

    // select the wanted channels
    g_xlChannelMask[ui8_bus] = g_xlInitMask[ui8_bus] = 0;
    i32_busInd = -1;
    for (uint32_t i=0; i<g_xlDrvConfig.channelCount; i++)
    {
      if ( ( g_xlDrvConfig.channel[i].hwType==gHwType                                           )
        || ( ( gHwType == XL_HWTYPE_AUTO ) && ( g_xlDrvConfig.channel[i].hwType > XL_HWTYPE_VIRTUAL ) ) )
      {
        i32_busInd++;
        printf( "Detect Real Channel %d\n", i32_busInd );
        if ( ui8_bus == i32_busInd )
        { // BUS found
          g_xlChannelMask[ui8_bus] |= g_xlDrvConfig.channel[i].channelMask;
        }
      }
      else if ( g_xlDrvConfig.channel[i].hwType == XL_HWTYPE_VIRTUAL )
      {
        i32_virtualBusInd++;
        printf( "Detect Virtual Channel %d\n", i32_virtualBusInd );
        if ( ui8_bus == i32_virtualBusInd )
        { // BUS found
          virtualChannelMask |= g_xlDrvConfig.channel[i].channelMask;
        }
      }
    }

    // if AUTO HW detection is wanted, and only virtual channels are found
    // use virtualChannelMask
    if ( ( gHwType == XL_HWTYPE_AUTO ) && ( i32_busInd == -1 ) )
    { // no real CAN channels found
      g_xlChannelMask[ui8_bus] = virtualChannelMask;
    }

    g_xlInitMask[ui8_bus] = g_xlPermissionMask[ui8_bus] = g_xlChannelMask[ui8_bus];

    // ------------------------------------
    // open ONE port including all channels
    // ------------------------------------
    xlStatus = xlOpenPort(&g_xlPortHandle[ui8_bus], g_AppName, g_xlChannelMask[ui8_bus], &g_xlPermissionMask[ui8_bus], 256, XL_INTERFACE_VERSION, XL_BUS_TYPE_CAN);
    printf("- OpenPort         : CM=0x%I64x, PH=0x%02X, PM=0x%I64x, %s\n", g_xlChannelMask[ui8_bus], g_xlPortHandle[ui8_bus], g_xlPermissionMask[ui8_bus], xlGetErrorString(xlStatus) );

    if ( (xlStatus) || (g_xlPortHandle[ui8_bus] == XL_INVALID_PORTHANDLE) )
        goto error;

    // ------------------------------------
    // if we have permission we set the
    // bus parameters (baudrate)
    // ------------------------------------
    if (g_xlPermissionMask[ui8_bus]) {
      xlStatus = xlCanSetChannelBitrate(g_xlPortHandle[ui8_bus], g_xlChannelMask[ui8_bus], wBitrate * 1000);
      printf("- SetChannelBitrate: baudr.=%u, %s\n",wBitrate, xlGetErrorString(xlStatus));
      if (xlStatus) goto error;
    }
    else if (wBitrate) {
      printf("WARNING: No init access, bitrate ignored!\n");
    }

    // Disable the TX and TXRQ notifications
    xlStatus = xlCanSetChannelMode(g_xlPortHandle[ui8_bus],g_xlChannelMask[ui8_bus],0,0);
    if (xlStatus) goto error;

    if (canBusIsOpen[ui8_bus] == false)
    { // set global mask
      xlStatus = xlCanSetChannelAcceptance(g_xlPortHandle[ui8_bus], g_xlChannelMask[ui8_bus], 0x000, 0x000, XL_CAN_STD );
      if (xlStatus) goto error;
      xlStatus = xlCanSetChannelAcceptance(g_xlPortHandle[ui8_bus], g_xlChannelMask[ui8_bus], 0x000, 0x000, XL_CAN_EXT );
      if (xlStatus) goto error;

      // Go on bus and reset clock
      xlStatus = xlActivateChannel(g_xlPortHandle[ui8_bus], g_xlChannelMask[ui8_bus], XL_BUS_TYPE_CAN, XL_ACTIVATE_RESET_CLOCK);
      if (xlStatus) goto error;

    }
    canBusIsOpen[ui8_bus] = true;
    pc_serverData->canBus(ui8_bus).mb_deviceConnected = true;
  }
  
  // either fresh init without error or the bus was already initialized
  return true; // already initialized and files are already open

  error:
      printf("ERROR: %s!\n", xlGetErrorString(xlStatus));

      if (g_xlPortHandle[ui8_bus] != XL_INVALID_PORTHANDLE) {
        xlClosePort(g_xlPortHandle[ui8_bus]);
        g_xlPortHandle[ui8_bus] = XL_INVALID_PORTHANDLE;
      }

      canBusIsOpen[ui8_bus] = false;
      return false;
}

void closeBusOnCard(uint8_t ui8_bus, server_c* pc_serverData)
{
  DEBUG_PRINT1("close can bus %d\n", ui8_bus);
  //ui8_bus[ui8_bus] = false;
  // do not call close or CAN_CLOSE because COMMAND_CLOSE is received during initialization!
}


void __HAL::updatePendingMsgs(server_c* /* pc_serverData */, int8_t /* i8_bus */)
{
}

// PURPOSE: To send a msg on the specified CAN BUS
int16_t sendToBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
  XLevent       xlEvent;
  XLstatus      xlStatus;
  unsigned int  messageCount = 1;

  xlEvent.tag                 = XL_TRANSMIT_MSG;
  xlEvent.tagData.msg.id      = ps_canMsg->ui32_id;

  // set extended type if needed by setting MSB bit
  if (ps_canMsg->i32_msgType > 0)
    xlEvent.tagData.msg.id |= 0x80000000UL;

  xlEvent.tagData.msg.dlc     = ps_canMsg->i32_len;
  xlEvent.tagData.msg.flags   = 0;
  memcpy(xlEvent.tagData.msg.data, ps_canMsg->ui8_data, ps_canMsg->i32_len);

  // should have been checked already by calling function isBusOpen:
  assert((ui8_bus <= HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus]);
  xlStatus = xlCanTransmit(g_xlPortHandle[ui8_bus], g_xlChannelMask[ui8_bus], &messageCount, &xlEvent);

  if (xlStatus == XL_ERR_QUEUE_IS_FULL)
    return 0;

  return 1; // success
}

bool readFromBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
  XLstatus xlStatus;
  XLevent  gpEvent;

  unsigned int    msgsrx = 1;

  // try to receive a message
  msgsrx = 1; // we want to receive always only one message
  xlStatus = xlReceive(g_xlPortHandle[ui8_bus], &msgsrx, &gpEvent);
  // msg from CANcardX buffer
  // this functions retrurns not only received messages
  // ACK for SENT messages is also returned!!!
  if( ( gpEvent.tag != XL_RECEIVE_MSG ) || ( gpEvent.tagData.msg.flags != 0 ) )
  { // don't further process this message as it is NO received message
    return false;
  }

  ps_canMsg->ui32_id = (gpEvent.tagData.msg.id & 0x1FFFFFFF);
  if (ps_canMsg->ui32_id >= 0x7FFFFFFF)
  {
    printf("!!Received of malformed message with undefined CAN ident: %x\n", ps_canMsg->ui32_id);
    return false;
  }

  ps_canMsg->i32_len = gpEvent.tagData.msg.dlc;
  ps_canMsg->i32_msgType = (gpEvent.tagData.msg.id > 0x7FFFFFFF) ? 1 : 0;

  memcpy( ps_canMsg->ui8_data, gpEvent.tagData.msg.data, ps_canMsg->i32_len );

  return true;
}

void addSendTimeStampToList(client_c * /*ps_client*/, int32_t /*i32_sendTimeStamp*/)
{
}

