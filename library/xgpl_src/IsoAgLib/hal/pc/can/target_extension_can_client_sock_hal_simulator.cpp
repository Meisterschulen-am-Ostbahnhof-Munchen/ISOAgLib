/*
  target_extension_can_client_sock_hal_simulator.cpp:
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
// Note: bMsgObj may be changed by the HALSimulator_c - this happens in the case bMsgObj = COMMON_MSGOBJ_IN_QUEUE (0xFF)
// The HALSimulator_c is responsible for setting bMsgObj correctly for the returned message
// Otherwise, it can cause an infinite loop if the bMsgObj doesn't match the message.
// ( bMsgObj is basically which CAN hardware filter the message should fall into )
#define ENTRY_POINT_FOR_INSERT_RECEIVE_CAN_MSG \
  if( halSimulator().InsertReceiveCanMsg( bBusNumber, &bMsgObj, &ptReceive->bXtd, &ptReceive->dwId, &ptReceive->bDlc, ptReceive->abData ) == true ) \
  { \
    i32_lastReceiveTime = getTime(); \
    ptReceive->bMsgObj = bMsgObj; \
    ptReceive->tReceiveTime.w1us = 0; \
    ptReceive->tReceiveTime.l1ms = i32_lastReceiveTime; \
    \
    return HAL_NO_ERR; \
  }

#define ENTRY_POINT_FOR_RECEIVE_CAN_MSG \
  halSimulator().ReceiveCanMsg( bBusNumber, ptReceive->tReceiveTime.l1ms, ptReceive->bXtd, ptReceive->dwId, ptReceive->bDlc, ptReceive->abData );

#define ENTRY_POINT_FOR_SEND_CAN_MSG \
  halSimulator().SendCanMsg( bBusNumber, s_transferBuf.s_data.i32_sendTimeStamp, ptSend->bXtd, ptSend->dwId, ptSend->bDlc, ptSend->abData );

  
#include "target_extension_can_client_sock.cpp"