/*
  can_driver_canserver_socket_hal_simulator.cpp:
    source-additions for HAL simulator for CAN communication.

  NOTE: The defines below will be injected into the original
  can_client_sock-module at the end of the file to modify it
  slightly (i.e. instrument it).

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

// See if the HALSimulator_c wants to insert a can msg for this bBusNumber and bMsgObj
#define ENTRY_POINT_FOR_INSERT_RECEIVE_CAN_MSG \
  if( halSimulator().InsertReceiveCanMsg( channel ) == true )  return true;

#define ENTRY_POINT_FOR_RECEIVE_CAN_MSG \
  halSimulator().ReceiveCanMsg( channel, msg );

#define ENTRY_POINT_FOR_SEND_CAN_MSG \
  halSimulator().SendCanMsg( channel, msg, s_transferBuf.s_data.i32_sendTimeStamp );

  
#include "can_driver_canserver_socket.cpp"
