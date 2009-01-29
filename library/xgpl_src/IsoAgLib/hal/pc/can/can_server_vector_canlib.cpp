/***************************************************************************
              can_server_vector_canlib.cpp -
                    interface for Vector-Informatik can card (canlib)

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

extern "C" {
  #include <VCanD.h>
}

#endif

#define DEF_USE_SERVER_SPECIFIC_HEADER
#include "can_server.h"

#define HWTYPE_AUTO 1000

using namespace __HAL;

// CAN Globals
int AllCanChannelCount = 0;

VportHandle gPortHandle[cui32_maxCanBusCnt];
Vaccess gChannelMask[cui32_maxCanBusCnt];
Vaccess gPermissionMask[cui32_maxCanBusCnt];
Vaccess gInitMask[cui32_maxCanBusCnt];

Vevent  gEvent;
Vevent*  gpEvent;
VDriverConfig *gDriverConfig = 0;

#ifdef USE_CAN_CARD_TYPE
uint32_t gHwType = USE_CAN_CARD_TYPE;
#else
// select the Vector CAN card type to use
uint32_t gHwType = HWTYPE_AUTO;
#endif


static bool  canBusIsOpen[cui32_maxCanBusCnt];

static void printDriverConfig( void ) {
  int i;
  printf("Driver Configuration:\n  ChannelCount=%u\n", gDriverConfig->channelCount);
  for (i=0; i<gDriverConfig->channelCount; i++) {
    printf("  Channel %u (%04X): %s, (%u,%u,%u), isOnBus=%u\n",
      gDriverConfig->channel[i].channelIndex,
      gDriverConfig->channel[i].channelMask,
      gDriverConfig->channel[i].name,
      gDriverConfig->channel[i].hwIndex,
      gDriverConfig->channel[i].hwType,
      gDriverConfig->channel[i].hwChannel,
      gDriverConfig->channel[i].isOnBus
    );
  }
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
  gHwType = HWTYPE_AUTO;
  #endif
  // open the driver
  Vstatus vErr;
  vErr = ncdOpenDriver();
  if (vErr) goto error;

  uint32_t ind;
  for ( ind = 0; ind < cui32_maxCanBusCnt; ind++ )
  {
    gPortHandle[ind] = INVALID_PORTHANDLE;
    gChannelMask[ind]    = 0;
    gPermissionMask[ind] = 0;
    gInitMask[ind]       = 0;
    canBusIsOpen[ind]    = false;
  }

  printf("ncdDriverConfig()\n");
  vErr = ncdGetDriverConfig(&AllCanChannelCount, NULL); // get the number of channels
  if (vErr) goto error;

  printf(" %u channels found\n",AllCanChannelCount);
  gDriverConfig = (VDriverConfig*)malloc(AllCanChannelCount*sizeof(VDriverConfig));
  if (!gDriverConfig) return 0;

  vErr = ncdGetDriverConfig(&AllCanChannelCount, gDriverConfig); // get information about all channels
  if (vErr) goto error;

  printDriverConfig();

  return 1;

  error:
    printf("ERROR: %s!\n", ncdGetErrorString(vErr));
    return 0;

}

bool resetCard(void)
{
  ncdCloseDriver();
  return true;
}


bool openBusOnCard(uint8_t ui8_bus, uint32_t wBitrate, server_c* pc_serverData)
{
  DEBUG_PRINT1("init can bus %d\n", ui8_bus);

  Vstatus vErr;

  if( !canBusIsOpen[ui8_bus] ) {
    DEBUG_PRINT1("Opening CAN BUS channel=%d\n", ui8_bus);

    int32_t i32_busInd = -1, i32_virtualBusInd = -1;
    Vaccess virtualChannelMask = 0;

    // select the wanted channels
    gChannelMask[ui8_bus] = gInitMask[ui8_bus] = 0;
    i32_busInd = -1;
    for (int32_t i=0; i<AllCanChannelCount; i++) {
      if ( ( gDriverConfig->channel[i].hwType==gHwType                                           )
        || ( ( gHwType == HWTYPE_AUTO ) && ( gDriverConfig->channel[i].hwType > HWTYPE_VIRTUAL ) ) )
      {
        i32_busInd++;
        printf( "Detect Real Channel %d\n", i32_busInd );
        if ( ui8_bus == i32_busInd )
        { // BUS found
          gChannelMask[ui8_bus] |= gDriverConfig->channel[i].channelMask;
        }
      }
      else if ( gDriverConfig->channel[i].hwType == HWTYPE_VIRTUAL )
      {
        i32_virtualBusInd++;
        printf( "Detect Virtual Channel %d\n", i32_virtualBusInd );
        if ( ui8_bus == i32_virtualBusInd )
        { // BUS found
          virtualChannelMask |= gDriverConfig->channel[i].channelMask;
        }
      }
    }
 
    // if AUTO HW detection is wanted, and only virtual channels are found
    // use virtualChannelMask
    if ( ( gHwType == HWTYPE_AUTO ) && ( i32_busInd == -1 ) )
    { // no real CAN channels found
      gChannelMask[ui8_bus] = virtualChannelMask;
    }

    gInitMask[ui8_bus] = gChannelMask[ui8_bus];

    // open a port
    printf("ncdOpenPort(channelMask=%04X, initMask=%04X)\n", gChannelMask[ui8_bus], gInitMask[ui8_bus]);
    vErr = ncdOpenPort(&gPortHandle[ui8_bus], "IsoAgLib", gChannelMask[ui8_bus], gInitMask[ui8_bus], &gPermissionMask[ui8_bus], 1024);
    if (vErr) goto error;

    printf(" portHandle=%u\n", gPortHandle[ui8_bus]);
    if (gPortHandle[ui8_bus]==INVALID_PORTHANDLE) goto error;

    printf(" permissionMask=%04X\n", gPermissionMask[ui8_bus]);

    // if permision to init
    if (gPermissionMask[ui8_bus]) {
      // set BUS timing
      vErr = ncdSetChannelBitrate(gPortHandle[ui8_bus], gPermissionMask[ui8_bus], (wBitrate * 1000));
      if (vErr) goto error;
    }
    else if (wBitrate) {
      printf("WARNING: No init access, bitrate ignored!\n");
    }

    // Disable the TX and TXRQ notifications
    vErr = ncdSetChannelMode(gPortHandle[ui8_bus],gChannelMask[ui8_bus],0,0);
    if (vErr) goto error;

    if (canBusIsOpen[ui8_bus] == false)
    { // set global mask
      VsetAcceptance acc;
      acc.mask = 0x000;
      acc.code = 0x000;
      vErr = ncdSetChannelAcceptance(gPortHandle[ui8_bus], gChannelMask[ui8_bus], &acc);
      if (vErr) goto error;
      acc.mask = 0x80000000;
      acc.code = 0x80000000;
      vErr = ncdSetChannelAcceptance(gPortHandle[ui8_bus], gChannelMask[ui8_bus], &acc);
      if (vErr) goto error;

      // reset clock
      vErr = ncdResetClock(gPortHandle[ui8_bus]);
      if (vErr) goto error;
      // Go on bus
      vErr = ncdActivateChannel(gPortHandle[ui8_bus],gChannelMask[ui8_bus]);
      if (vErr) goto error;
    }
    canBusIsOpen[ui8_bus] = true;
  }
  
  // either fresh init without error or the bus was already initialized
  return true; // already initialized and files are already open

  error:
    printf("ERROR: %s!\n", ncdGetErrorString(vErr));

    if (gPortHandle[ui8_bus] != INVALID_PORTHANDLE) {
      ncdClosePort(gPortHandle[ui8_bus]);
      gPortHandle[ui8_bus] = INVALID_PORTHANDLE;
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
  VportHandle lPortHandle = gPortHandle[ui8_bus];
  Vaccess lChannelMask = gChannelMask[ui8_bus];
  Vaccess lPermissionMask = gPermissionMask[ui8_bus];

  Vevent  lEvent = gEvent;
  Vstatus vErr;

  lEvent.tag = V_TRANSMIT_MSG;
  lEvent.tagData.msg.id = ps_canMsg->ui32_id;

  // set extended type if needed by setting MSB bit
  if (ps_canMsg->i32_msgType > 0)
    lEvent.tagData.msg.id |= 0x80000000UL;
  lEvent.tagData.msg.flags = 0;
  lEvent.tagData.msg.dlc = ps_canMsg->i32_len;
  memcpy(lEvent.tagData.msg.data, ps_canMsg->ui8_data, ps_canMsg->i32_len);

  vErr = ncdTransmit(lPortHandle, lChannelMask, &lEvent);

  if (vErr == VERR_QUEUE_IS_FULL)
    return 0;

  return 1; // success
}

uint32_t readFromBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
  Vstatus vErr;

  vErr = ncdReceive1(gPortHandle[ui8_bus], &gpEvent);
  // msg from CANcardX buffer
  // this functions retrurns not only received messages
  // ACK for SENT messages is also returned!!!
  if( vErr || ( gpEvent->tag != V_RECEIVE_MSG ) || ( gpEvent->tagData.msg.flags != 0 ) )
  { // don't further process this message as it is NO received message
    return 0;
  }

  ps_canMsg->ui32_id = (gpEvent->tagData.msg.id & 0x1FFFFFFF);
  if (ps_canMsg->ui32_id >= 0x7FFFFFFF)
  {
    printf("!!Received of malformed message with undefined CAN ident: %x\n", ps_canMsg->ui32_id);
    return 0;
  }

  ps_canMsg->i32_len = gpEvent->tagData.msg.dlc;
  ps_canMsg->i32_msgType = (gpEvent->tagData.msg.id > 0x7FFFFFFF) ? 1 : 0;

  memcpy( ps_canMsg->ui8_data, gpEvent->tagData.msg.data, ps_canMsg->i32_len );

  return ps_canMsg->i32_len;
}

void addSendTimeStampToList(client_c * /*ps_client*/, int32_t /*i32_sendTimeStamp*/)
{
}

