/***************************************************************************
              can_device_sontheim_mt_api.cpp -
                    interface for SONTHEIM can card (multi thread API)

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
#if !defined(WIN32)
typedef int HINSTANCE;
#endif

#define  STRICT

#include "can_target_extensions.h"
#include <stdlib.h>
#include <ctype.h>
#include <IsoAgLib/hal/system.h>

#include <stdio.h>
#include <conio.h>
#include <time.h>

#include <SIECA132.h>


#include "can_server.h"
#include "../can_server_common.h"
#include <assert.h>

using namespace __HAL;

/////////////////////////////////////////////////////////////////////////
// Globals

// CAN Globals
HANDLE htEventR[cui32_maxCanBusCnt];
HANDLE htEventE[cui32_maxCanBusCnt];
HANDLE handle[cui32_maxCanBusCnt];
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
    htEventR[i] = CreateEvent (NULL,TRUE,FALSE,"R1");
    htEventE[i] = CreateEvent (NULL,TRUE,FALSE,"E1");
    handle[i]   = 0;
  }

  long l_netnumber_max = 26;                  // highest net number
  long l_txtimeout = 50;                      // msec
  long l_rxtimeout = 1;                       // msec
  long l_retval;

  // create events
 
  printf ( "HW detection ...\n");
  
  uint32_t busnr=0;
  long l_netnumber = 0;
  for (; (l_netnumber < l_netnumber_max) && (busnr<cui32_maxCanBusCnt); l_netnumber++)
  {
    // open new handle
    l_retval = canOpen (l_netnumber,
                        0,
                        0,
                        l_txtimeout,
                        l_rxtimeout,
                        "c_MyFirstApplication",
                        "R1",
                        "E1",
                        &handle[busnr]);
    // successful?
    if ( l_retval == NTCAN_SUCCESS ) {
      // success => HW found
      printf ( "canOpen: CAN%d using netnumber %d\n", (unsigned int) busnr, l_netnumber);
      busnr++;
    }
  }
  
  if (busnr == 0) 
  {
     printf("HW detection failed.\n");
     exit(1);
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

    long                    l_retval;
    long                    l_baud;

    switch (wBitrate) {
      case 1000: l_baud = 0; break;
      case 800:  l_baud = 1; break;
      case 500:  l_baud = 2; break;
      case 250:  l_baud = 3; break;
      case 125:  l_baud = 4; break;
      case 100:  l_baud = 5; break;
      case  50:  l_baud = 6; break;
      case  20:  l_baud = 7; break;
      default: 
        printf ( "invalid baud rate %dk\n(user defined baud rate not yet supported)\n", wBitrate);
        return false;
    }

    l_retval = canSetBaudrate ( handle[ui8_bus], l_baud );
    // successful ?
    if ( l_retval != NTCAN_SUCCESS ) {
      // error
      printf ( "could not initialize CAN!\n");
      return false;
    }

    // enable all CAN identifiers
    l_retval = canEnableAllIds(handle[ui8_bus], true);
    if (l_retval != NTCAN_SUCCESS) {
      // Error handling...
      printf ( "error in  canEnableAllIds!\n");
      return false;       
    }

    l_retval = canSetFilterMode(handle[ui8_bus], filterMode_nofilter);
    if (l_retval != NTCAN_SUCCESS) {
      // Error handling...
      printf ( "error in  canEnableAllIds!\n");
      return false;       
    }

    CAN_IF_STATUS t_CANStatus;
    l_retval = canStatus ( handle[ui8_bus], &t_CANStatus );
    if ( l_retval == NTCAN_SUCCESS ) {
      printf ( "status from card:\n");
    //  printf ( "HW Rev. : %04X\n", t_CANStatus.w_hw_recv );
      printf ( "FW Rev. : %04X\n", t_CANStatus.w_fw_rev );
      printf ( "DLL Rev. : %04X\n", t_CANStatus.w_dll_rev);
      printf ( "Drv Rev. : %04X\n", t_CANStatus.w_drv_rev);
      printf ( "exist. Nets: %02x\n", t_CANStatus.w_netctr);
    }

    canBusIsOpen[ui8_bus] = true;
    pc_serverData->canBus(ui8_bus).b_deviceConnected = true;
    return true;
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


bool readFromBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
  long            l_retval;
  CMSG            t_CANMsg[1];
  long      l_len;

  l_len = 1;
  l_retval = canRead (handle[ui8_bus],
                      &t_CANMsg[0],
                      &l_len);
  // successful ?
  if ( l_retval == NTCAN_SUCCESS ) {
    ps_canMsg->ui32_id = t_CANMsg[0].l_id;
    ps_canMsg->i32_len = t_CANMsg[0].by_len & 0x0F;
    ps_canMsg->i32_msgType = (t_CANMsg[0].by_extended ? 1 : 0);

    for (uint8_t ui8_cnt = 0; ui8_cnt < ps_canMsg->i32_len; ui8_cnt++)
      ps_canMsg->ui8_data[ui8_cnt] = t_CANMsg[0].aby_data[ui8_cnt];

    return true;
  }
  return false;
}


// PURPOSE: To send a msg on the specified CAN BUS
// RETURNS: non-zero if msg was sent ok
//          0 on error
int16_t sendToBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
  CMSG       t_CANMsg[1];
  long l_len;
  long l_retval;

  l_len = 1; // one frame
  // fill with CAN data
  t_CANMsg[0].l_id = ps_canMsg->ui32_id;
  t_CANMsg[0].by_len = ps_canMsg->i32_len;
  t_CANMsg[0].by_extended = ( ps_canMsg->i32_msgType ? 1 : 0 );
  t_CANMsg[0].by_remote = 0;
  for ( int32_t ind = 0; ind < ps_canMsg->i32_len; ind++ )
    t_CANMsg[0].aby_data[ind] = ps_canMsg->ui8_data[ind];

  // should have been checked already by calling function isBusOpen:
  assert((ui8_bus <= HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus]);
  // blocked transmit
  l_retval = canSend ( handle[ui8_bus],
                       &t_CANMsg[0],
                       &l_len );
  if ( l_retval == NTCAN_SUCCESS ) {
    return 1;
  }
  else
  {
    printf("error in canSend: %d!\n", l_retval);
    return 0;
  }
}

