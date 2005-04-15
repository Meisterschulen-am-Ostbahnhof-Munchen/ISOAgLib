/***************************************************************************
						  target_extension_can_A1_binary.cpp - source for the
									   PSEUDO BIOS for development and test on
									   the Opus A1 with CAN communication using
									   the Binary can drivers for /dev/wecan0
									   and /dev/wecan1

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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
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
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstdio>

#include "can_target_extensions.h"

#include "can_msq.h"

namespace __HAL {

void send_command_ack(int32_t i32_mtype, int32_t i32_error, msqData_s* p_msqDataServer)
{
  msqCommand_s msqCommandBuf;

  msqCommandBuf.i32_mtype = i32_mtype;

  // send ACK/NACK
  if (!i32_error)
    msqCommandBuf.i16_command = COMMAND_ACK;
  else {
    // inform client about troubles with can bus init
    msqCommandBuf.i16_command = COMMAND_NACK;
    msqCommandBuf.s_error.i32_error = i32_error;
  }
  msgsnd(p_msqDataServer->i32_cmdAckHandle, &msqCommandBuf, sizeof(msqCommand_s) - sizeof(int32_t), IPC_NOWAIT);
}


int32_t send_command(msqCommand_s* p_msqCommandBuf, msqData_s* p_msqDataClient)
{
  int16_t i16_rc;
  
  if ((i16_rc = msgsnd(p_msqDataClient->i32_cmdHandle, p_msqCommandBuf, sizeof(msqCommand_s) - sizeof(int32_t), 0)) == -1)
    return HAL_UNKNOWN_ERR;

  // wait for ACK
  if((i16_rc = msgrcv(p_msqDataClient->i32_cmdAckHandle, p_msqCommandBuf, sizeof(msqCommand_s) - sizeof(int32_t), p_msqDataClient->i32_pid, 0)) == -1)
    return HAL_UNKNOWN_ERR;

  if (p_msqCommandBuf->i16_command == COMMAND_ACK ) {
    return HAL_NO_ERR;
  }

  if (p_msqCommandBuf->i16_command == COMMAND_NACK ) {
    printf("nack received\n");
    return p_msqCommandBuf->s_error.i32_error;
  }
  
  return HAL_UNKNOWN_ERR;
}


/////////////////////////////////////////////////////////////////////////
int16_t ca_createMsqs(msqData_s& msqData)
{
   key_t  msgkey;

   DEBUG_PRINT("ca_createMsqs called\n");
   msqData.i32_pid = getpid();

   /* Generate our IPC key value */
   msgkey = ftok(MSQ_KEY_PATH , MSQ_COMMAND);
   if ((msqData.i32_cmdHandle = msgget(msgkey, IPC_CREAT | 0660 )) == -1)
     return HAL_UNKNOWN_ERR;

   msgkey = ftok(MSQ_KEY_PATH , MSQ_COMMAND_ACK);
   if ((msqData.i32_cmdAckHandle = msgget(msgkey, IPC_CREAT | 0660 )) == -1)
     return HAL_UNKNOWN_ERR;
  
   msgkey = ftok(MSQ_KEY_PATH , MSQ_CLIENT_READ);
   if ((msqData.i32_rdHandle = msgget(msgkey, IPC_CREAT | 0660 )) == -1)
     return HAL_UNKNOWN_ERR;

   msgkey = ftok(MSQ_KEY_PATH , MSQ_CLIENT_WRITE);
   if ((msqData.i32_wrHandle = msgget(msgkey, IPC_CREAT | 0660 )) == -1)
     return HAL_UNKNOWN_ERR;

   DEBUG_PRINT4("queues: cmd %d, ack %d, rd %d, wr %d\n", msqData.i32_cmdHandle, msqData.i32_cmdAckHandle, msqData.i32_rdHandle, msqData.i32_wrHandle);

   return 0;
}


uint32_t assemble_mtype(int32_t i32_pid, uint8_t bus, uint8_t obj)
{
  return (i32_pid << 16 | bus << 8 | obj);
}


void clearReadQueue(uint8_t bBusNumber, uint8_t bMsgObj, int32_t i32_msqHandle, int32_t i32_pid)
{
  msqRead_s msqReadBuf;

  //DEBUG_PRINT3("clear read queue: bus %d, obj %d, pid %d\n", bBusNumber, bMsgObj, i32_pid);
  while (msgrcv(i32_msqHandle, &msqReadBuf, sizeof(msqRead_s) - sizeof(int32_t), assemble_mtype(i32_pid, bBusNumber, bMsgObj), IPC_NOWAIT) > 0)
    ;
}

void clearWriteQueue(uint8_t bBusNumber, uint8_t bMsgObj, int32_t i32_msqHandle, int32_t i32_pid)
{
  msqWrite_s msqWriteBuf;

  //DEBUG_PRINT3("clear write queue: bus %d, obj %d, pid %d\n", bBusNumber, bMsgObj, i32_pid);
  while (msgrcv(i32_msqHandle, &msqWriteBuf, sizeof(msqWrite_s) - sizeof(int32_t), assemble_mtype(i32_pid, bBusNumber, bMsgObj), IPC_NOWAIT) > 0)
    ;
}


}; // end namespace __HAL
