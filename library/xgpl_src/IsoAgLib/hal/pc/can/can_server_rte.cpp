/***************************************************************************
              can_server_rte.cpp -
                    interface for RTE can 
                             -------------------
    begin                : Tue Jul 15 2003
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

#define CVS_REV "$Revision: 2865 $"
#define RTE_CLIENT

#include "rte_global.h"

#define DEF_USE_SERVER_SPECIFIC_HEADER
#include "can_server.h"

// RTE Includes
#include <rte_client.h>   // be an RTE client
#include <iostream>
#include <iomanip.h>

#include <list.h>

using namespace __HAL;

/////////////////////////////////////////////////////////////////////////
// Globals


static bool   canBusIsOpen[cui32_maxCanBusCnt];
static can_c* rteCan_c [cui32_maxCanBusCnt];
static std::list<canMsg_s> l_canMsg;

/*! Handler for identification reply */
static int ident_hdl(rtd_handler_para_t* para, char* ident) {
  if ((para == 0) || (ident == 0))
    return rte_set_error( null_pointer );

  return 0;
}

/*! Handler for acknowledge reply */
static int ackHdl(rtd_handler_para_t*, rte_return_t ) {
  return 0;
}

/*! Handler for incoming CAN messages. */
static int can_receive(rtd_handler_para_t* para, rtd_can_type_t type, uint32_t id, uint8_t size, const void *data,
                       rte_client_t cid, rtd_can_event_flag_t flag, uint8_t /*code*/)
{
  if (0 != flag)
  { /* ERROR has been reported. */
    return -1;
  }

  if ( rteCan_c [para->rtd_msg->channel]->get_cid() == cid )
  { // echo of this rte client
    return -1;
  }

  canMsg_s s_canMsg;
  s_canMsg.ui32_id = id;
  s_canMsg.i32_len = size;
  s_canMsg.i32_msgType = (type == rtd_can_type_xtd_msg) ? 1 : 0;
  memcpy(s_canMsg.ui8_data, data, size);

  l_canMsg.push_back(s_canMsg);

  return 0;
}
bool isBusOpen(uint8_t ui8_bus)
{
  return canBusIsOpen[ui8_bus];
}


uint32_t initCardApi ()
{
  if ( ! rte_is_init() ) {
    std::string c_rteServer = HAL_PC_RTE_DEFAULT_SERVER;
    if ( getenv( "RTE_HOST" ) != NULL ) c_rteServer = getenv( "RTE_HOST" );

    if (rte_connect( c_rteServer.c_str() ) < 0) {
      cerr << "Unable to connect " << c_rteServer << " server." << endl;
      exit(1);
    } else {
      cout << "Connected to " << c_rteServer << " server." << endl;
    }
  }

  for( uint32_t i=0; i<cui32_maxCanBusCnt; i++ )
  {
    canBusIsOpen[i] = false;
    rteCan_c [i] = new can_c ();
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
    // Open a CAN messaging interface
    rteCan_c [ui8_bus]->set_channel( ui8_bus );
    rteCan_c [ui8_bus]->set_default_timeout( 60*RTE_ONE_SECOND );
    rteCan_c [ui8_bus]->init();
    // build parameter string with sprintf to handle different baudrates
    char parameterString[50];
    sprintf( parameterString, "C%d,R,B%d,E1,L0", ui8_bus, wBitrate );
    rteCan_c [ui8_bus]->set_parameters( parameterString );

    // Set handler functions.
    rteCan_c [ui8_bus]->set_acknowledge_handler( ackHdl, rteCan_c [ui8_bus]);
    rteCan_c [ui8_bus]->set_identify_handler( ident_hdl, rteCan_c [ui8_bus]);
    rteCan_c [ui8_bus]->set_send_handler( can_receive, rteCan_c [ui8_bus] );
    //  rteCan_c [ui8_bus]->set_set_bus_state_handler( setBusState, rteCan_c [ui8_bus] );

    canBusIsOpen[ui8_bus] = true;
    pc_serverData->marrb_deviceConnected[ui8_bus] = true;

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


// PURPOSE: To send a msg on the specified CAN BUS
// RETURNS: non-zero if msg was sent ok
//          0 on error
int16_t sendToBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
  rteCan_c[ui8_bus]->set_delta (0); // NOW!
  rteCan_c[ui8_bus]->set_count (1); // send only once

  if (ps_canMsg->i32_msgType) {
    rteCan_c[ui8_bus]->send (rtd_can_type_xtd_msg, ps_canMsg->ui32_id, ps_canMsg->i32_len, ps_canMsg->ui8_data);
  } else {
    rteCan_c[ui8_bus]->send (rtd_can_type_std_msg, ps_canMsg->ui32_id, ps_canMsg->i32_len, ps_canMsg->ui8_data);
  }

  rte_poll();

  return 1;
}


bool readFromBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData)
{
  rte_poll();

  if (l_canMsg.size())
  {
    *ps_canMsg = l_canMsg.front();
    l_canMsg.pop_front();
    return true;
  }

  return false;
}

void __HAL::updatePendingMsgs(server_c* pc_serverData, int8_t i8_bus)
{
}
void addSendTimeStampToList(client_c *ps_client, int32_t i32_sendTimeStamp)
{
}

