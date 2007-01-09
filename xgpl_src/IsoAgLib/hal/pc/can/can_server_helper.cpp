/***************************************************************************
						  can_server_helper.cpp -
                    helper routines for can server
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

#include <cstring>
#include <cstdio>
#include <cctype>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>

#include "can_target_extensions.h"
#include "can_server.h"
#include "can_msq.h"

namespace __HAL {

void usage() {
  printf("usage: can_server_A1 [--log <log_file_name_base>] [--file-input <log_file_name>]\n\n");
  printf("  --log         log can traffic into <log_file_name_base>_<bus_id>\n");
  printf("  --file-input  read can data from file <log_fil_name>\n\n");
}


int open_semaphore_set(int sema_proj_id)
{
  int sid;
  key_t  semkey;

  union semun {
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
  } semopts;

  /* Generate our IPC key value */
  semkey = ftok(MSQ_KEY_PATH, sema_proj_id);

  if((sid = semget( semkey, 1, IPC_CREAT | 0660 )) == -1) {
    return(-1);
  }

  semopts.val = 1;  /* initial value: resource free */
  if (semctl( sid, 0, SETVAL, semopts) == -1) {
    return(-1);
  }

  return(sid);
}


int get_semaphore(int sid, int operation ) {

  struct sembuf sem_command = { 0, operation, 0 };

  if (semop(sid, &sem_command, 1) == -1)
    return(-1);

  return 1;

}

void dumpCanMsg (uint8_t bBusNumber, uint8_t bMsgObj, tSend* ptSend, FILE* f_handle)
{
  uint8_t data[8] = {0,0,0,0,0,0,0,0};
  clock_t t_sendTimestamp = times(NULL);
  memcpy(data, ptSend->abData, ptSend->bDlc);

  if (f_handle) {
    fprintf(f_handle,
            "%05d %d %d %d %d %d %-8x   %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx %-3hx\n",
            t_sendTimestamp*10, bBusNumber, bMsgObj, ptSend->bXtd, ptSend->bDlc, (ptSend->dwId >> 26) & 7 /* priority */, ptSend->dwId,
            data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    fflush(f_handle);
  }
};


bool readCanDataFile(server_c* pc_serverData, can_recv_data* ps_receiveData)
{
  char zeile[100];
  static struct timeval tv_start;
  static bool first_call = TRUE;
  struct timeval tv_current;
  struct timezone tz;
  int32_t mydiff;

  tz.tz_minuteswest=0;
  tz.tz_dsttime=0;

  if (first_call) {
    first_call = FALSE;

    if ((pc_serverData->f_canInput = fopen(pc_serverData->inputFile.c_str(), "r")) == NULL ) {
      perror("fopen");
      exit(1);
    }

    if (gettimeofday(&tv_start, &tz)) {
      perror("error in gettimeofday()");
      exit(1);
    }
  }

  for (;;) {

    if (fgets(zeile, 99, pc_serverData->f_canInput) == NULL) {
      if ( feof(pc_serverData->f_canInput) )
        // no more data available
        return FALSE;
    }

    if ( strlen(zeile) == 0 )
      continue;

    int obj, xtd, dlc, prio;
    int rc = sscanf(zeile, "%u %d %d %d %d %d %x   %hx %hx %hx %hx %hx %hx %hx %hx \n",
                    &(ps_receiveData->msg.i32_time),
                    &(ps_receiveData->b_bus), &obj,
                    &xtd, &dlc, &prio,
                    &(ps_receiveData->msg.i32_ident),
                    &(ps_receiveData->msg.pb_data[0]), &(ps_receiveData->msg.pb_data[1]),
                    &(ps_receiveData->msg.pb_data[2]), &(ps_receiveData->msg.pb_data[3]),
                    &(ps_receiveData->msg.pb_data[4]), &(ps_receiveData->msg.pb_data[5]),
                    &(ps_receiveData->msg.pb_data[6]), &(ps_receiveData->msg.pb_data[7]));

    if (!rc)
      return FALSE;

    // sscanf uses int by default (overwriting of two adjacent uint8_t with format "%d"!)
    ps_receiveData->msg.b_xtd = xtd;
    ps_receiveData->msg.b_dlc = dlc;

    // is data ready for submission?
    if (gettimeofday(&tv_current, &tz)) {
      perror("error in gettimeofday()");
      exit(1);
    }

    mydiff = ps_receiveData->msg.i32_time - (tv_current.tv_sec-tv_start.tv_sec)*1000 + (tv_current.tv_usec-tv_start.tv_usec)/1000;

    if (mydiff < 0)
      mydiff = 0;

    usleep(mydiff*1000);

    break;
  }

  // more data available
  return TRUE;
}

void releaseClient(server_c* pc_serverData, std::list<client_s>::iterator& iter_delete) {

  for (uint8_t i=0; i<cui32_maxCanBusCnt; i++)
  {
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    for (uint8_t j=0; j<iter_delete->arrMsgObj[i].size(); j++) {
      clearReadQueue(i, j, pc_serverData->msqDataServer.i32_rdHandle, iter_delete->i32_clientID);
      clearWriteQueue(i, j, pc_serverData->msqDataServer.i32_wrHandle, iter_delete->i32_clientID);
    }
#else
    clearReadQueue(i, COMMON_MSGOBJ_IN_QUEUE, pc_serverData->msqDataServer.i32_rdHandle,iter_delete->i32_clientID);
    clearWriteQueue(i, COMMON_MSGOBJ_IN_QUEUE, pc_serverData->msqDataServer.i32_wrHandle,iter_delete->i32_clientID);
#endif
  }

  if (iter_delete->i32_pipeHandle)
    close(iter_delete->i32_pipeHandle);

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  for (uint8_t k=0; k<cui32_maxCanBusCnt; k++)
    iter_delete->arrMsgObj[k].clear();
#endif

  // erase sets iterator to next client
  iter_delete = pc_serverData->l_clients.erase(iter_delete);

}

} // end namespace __HAL
