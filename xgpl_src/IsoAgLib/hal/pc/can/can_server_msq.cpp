/***************************************************************************
              target_extension_can_server_A1_binary.cpp -
                    can server communicates with clients through message queues
                    read/write operation for /dev/wecan<bus_no>
                    message forwarding to other clients
                    #define SIMULATE_BUS_MODE for operation without can device (operation based on message forwarding)
                    use "can_server_a1 --help" for input parameters (log, replay mode)

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


#include "can_target_extensions.h"
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
#include <sys/resource.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include <pthread.h>
#include <linux/version.h>

#include "can_server.h"

#ifdef DEBUG
  //backtrace
  #include <unistd.h>
  #include <execinfo.h>

  char*  reserve_memory_heap;
#endif


using namespace __HAL;


/////////////////////////////////////////////////////////////////////////
// Globals
/** if the following define is active, the can_server writes important logging info to the given path.
 *  comment this define out to eliminate this */
#ifdef WIN32
  #define CAN_SERVER_LOG_PATH ".\\can_server.log"
#elif defined( SYSTEM_A1 )
  #define CAN_SERVER_LOG_PATH "/sd0/settings/can_server.log"
#else
  #define CAN_SERVER_LOG_PATH "./can_server.log"
#endif

// set nice level for the threads
#define PRIORITY_CAN_READ    10
#define PRIORITY_CAN_WRITE    0
#define PRIORITY_CAN_COMMAND  0


// CAN Globals
static int apiversion;

// IO address for LPT and ISA ( PCI and PCMCIA use automatic adr )
const int32_t LptIsaIoAdr = 0x378;

#define USE_CAN_CARD_TYPE c_CANA1BINARY

server_c::server_c()
  : b_logMode(FALSE), b_inputFileMode(FALSE), i32_lastPipeId(0), i16_reducedLoadOnIsoBus(-1)
{
  memset(f_canOutput, 0, sizeof(f_canOutput));
  memset(ui16_globalMask, 0, sizeof(ui16_globalMask));
  memset(arrb_remoteDestinationAddressInUse, 0, sizeof(arrb_remoteDestinationAddressInUse));
  memset(ui16_busRefCnt, 0, sizeof(ui16_busRefCnt));

  for (uint32_t i=0; i<cui32_maxCanBusCnt; i++)
  {
    i32_sendDelay[i] = 0;
    i_pendingMsgs[i] = 0;
    can_device[i] = 0;
  }

  pthread_mutex_init(&m_protectClientList, NULL);

}

client_c::client_c() 
  : ui16_pid(0), i32_msecStartDeltaClientMinusServer(0), i32_pipeHandle(0)
{
  memset(b_busUsed, 0, sizeof(b_busUsed));
  memset(ui16_globalMask, 0, sizeof(ui16_globalMask));
  memset(ui32_globalMask, 0, sizeof(ui32_globalMask));
  memset(ui32_lastMask, 0, sizeof(ui32_lastMask));
  memset(i32_sendDelay, 0, sizeof(i32_sendDelay));
  memset(b_initReceived, 0, sizeof(b_initReceived));
}


namespace __HAL {

static int si_highPrioModeIfMin=0; // can be switch by commandline-parameter

#ifndef WIN32
/** linux-begin */
#ifndef LINUX_VERSION_CODE
#error "LINUX_VERSION_CODE is not defined"
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
/** On Linux 2.4: Define the amount of MSec per Clock_t, in case the project config didn't this before */
#ifndef msecPerClock
#define msecPerClock 10
#define clocksPer100Msec 10
#endif
#endif

clock_t getStartUpTime()
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
  if ( msecPerClock != (1000 / sysconf(_SC_CLK_TCK)) )
  { // BIG PROBLEM -> THE DEFINE DURING COMPILE TIME DOES NOT MATCH THE RUNTIME
    INTERNAL_DEBUG_DEVICE << "\n\nVERY BIG PROBLEM!!!\nThis program was compiled with\n#define msecPerClock " << msecPerClock
        << "\nwhile the runtime system has " << (1000 / sysconf(_SC_CLK_TCK))
        << "\n\nSO PLEASE add\n#define msecPerClock " << (1000 / sysconf(_SC_CLK_TCK))
        << "\nto your project configuration header or Makefile, so that a matching binary is built. This program is aborted now, as none of any time calculations will match with this problem.\n\n"
        << INTERNAL_DEBUG_DEVICE_ENDL;
    abort();
  }
  static clock_t st_startup4Times = times(NULL);
#else
  static clock_t st_startup4Times = -1;
  if (st_startup4Times < 0)
  {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    st_startup4Times = int32_t(ts.tv_sec)*1000 + int32_t(ts.tv_nsec/1000000);
  }
#endif
  return st_startup4Times;
}
/** linux-end */
#endif




#ifdef WIN32
#if defined( _MSC_VER )
  // VC++ with native Win32 API provides very accurate
  // msec timer - use that
  int32_t getTime()
  { // returns time in msec
    return timeGetTime();
  }
#else
  // MinGW has neither simple access to timeGetTime()
  // nor to gettimeofday()
  // - but beware: at least in LINUX clock() measures
  //   only the times of the EXE in CPU core
  int32_t getTime()
  { // returns time in msec
    return (clock()/(CLOCKS_PER_SEC/1000));
  }
#endif
#else
  /** linux-begin */
  int32_t getTime()
  {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
    // use gettimeofday for native LINUX 2.4 system
    // sysconf(_SC_CLK_TCK) provides clock_t ticks per second
    //  static const int64_t ci64_mesecPerClock = 1000 / sysconf(_SC_CLK_TCK);
    static struct timeval now;
    gettimeofday(&now, 0);
    // fetch RAW - non normalized - time in scaling of gettimeofday()
    int64_t i64_time4Timeofday = int64_t(now.tv_sec)*1000LL + int64_t(now.tv_usec/1000);


    // store offset between gettimeofday() and system start
    static int64_t si64_systemStart4Timeofday = i64_time4Timeofday;
    // static store delta between times() normalization and gettimeofday() norm
    static int64_t si64_deltaStartTimes = i64_time4Timeofday - int64_t(getStartUpTime()) * msecPerClock;

    // derive change of the normalization delta
    const int64_t ci64_deltaChange = i64_time4Timeofday - int64_t(times(NULL)) * msecPerClock - si64_deltaStartTimes;
    if ( ( ci64_deltaChange >= 1000 ) || ( ci64_deltaChange <= -1000 ) )
    { // user changed the system clock inbetween
      si64_deltaStartTimes += ci64_deltaChange;
      si64_systemStart4Timeofday += ci64_deltaChange;
    }

    // now calculate the real time in [msec] since startup
    i64_time4Timeofday -= si64_systemStart4Timeofday;
    // now derive the well define overflows
    while ( i64_time4Timeofday > 0x7FFFFFFFLL )
    {
      i64_time4Timeofday         -= 0xFFFFFFFF;
      si64_systemStart4Timeofday += 0xFFFFFFFF;
      si64_deltaStartTimes       += 0xFFFFFFFF;
    }

    return i64_time4Timeofday;
#else
    /** linux-2.6 */
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    const int32_t ci_now = int32_t(ts.tv_sec)*1000 + int32_t(ts.tv_nsec/1000000);
    return ci_now - getStartUpTime();
#endif
  }
  /** linux-end */
#endif


void initClientTime( client_c& r_receiveClient, clock_t at_startupClock )
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
  r_receiveClient.i32_msecStartDeltaClientMinusServer = (at_startupClock - getStartUpTime())*msecPerClock;
#else
  r_receiveClient.i32_msecStartDeltaClientMinusServer = (at_startupClock - getStartUpTime()); // we get msec directly with Linux 2.6. upwards!
#endif
  DEBUG_PRINT1 ("Initialized r_receiveClient.i32_msecStartDeltaClientMinusServer to %d\n", r_receiveClient.i32_msecStartDeltaClientMinusServer);
}

int32_t getClientTime( client_c& r_receiveClient )
{
  return getTime() - r_receiveClient.i32_msecStartDeltaClientMinusServer;
}

//int32_t getServerTimeFromClientTime( client_c& r_receiveClient, int32_t ai32_clientTime )
//{
//  return ai32_clientTime + r_receiveClient.i32_msecStartDeltaClientMinusServer;
//}

} // end namespace


void usage() {
  printf("usage: can_server_A1 [--log <log_file_name_base>] [--file-input <log_file_name>] [--high-prio-minimum <num_pending_writes>] [--reduced-load-iso-bus-no <bus_number>\n\n");
  printf("  --log                      log can traffic into <log_file_name_base>_<bus_id>\n");
  printf("  --file-input               read can data from file <log_file_name>\n");
  printf("  --high-prio-minimum        if 0: start normally without priority-handling (default - used if param not given!).\n");
  printf("                             if >0: only clients with activated high-priority-send-mode can send messages if\n");
  printf("                                    can-controller has equal or more than <num_pending_writes> in its queue.\n");
  printf("  --reduced-load-iso-bus-no  avoid unnecessary CAN bus load due to\n");
  printf("                             messages with local destination addresses\n");
  printf("  --nice-can-read            set a nice value for can read thread to reduce slowing down effect on other applications\n");
  printf("                             due to heavy OS task switches, 0 < value < 20 for reducing the priority\n\n");
}


void dumpCanMsg (uint8_t bBusNumber, uint8_t bMsgObj, canMsg_s* ps_canMsg, FILE* f_handle)
{
  clock_t t_sendTimestamp = times(NULL);

  if (f_handle) {
    fprintf(f_handle, "%05d %d %d %d %d %d %-8x  ",
            t_sendTimestamp*10, bBusNumber, bMsgObj, ps_canMsg->i32_msgType, ps_canMsg->i32_len,
            (ps_canMsg->ui32_id >> 26) & 7 /* priority */, ps_canMsg->ui32_id);
    for (uint8_t ui8_i = 0; (ui8_i < ps_canMsg->i32_len) && (ui8_i < 8); ui8_i++)
      fprintf(f_handle, " %-3hx", ps_canMsg->ui8_data[ui8_i]);
    fprintf(f_handle, "\n");
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

void releaseClient(server_c* pc_serverData, STL_NAMESPACE::list<client_c>::iterator& iter_delete) {

  for (uint8_t i=0; i<cui32_maxCanBusCnt; i++)
  {
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    for (uint8_t j=0; j<iter_delete->arrMsgObj[i].size(); j++) {
      clearReadQueue (i, j, pc_serverData->msqDataServer.i32_rdHandle, iter_delete->ui16_pid);
//      clearWriteQueue(i, j, pc_serverData->msqDataServer.i32_wrHandle, iter_delete->ui16_pid);
    }
#else
    clearReadQueue (i, COMMON_MSGOBJ_IN_QUEUE, pc_serverData->msqDataServer.i32_rdHandle,iter_delete->ui16_pid);
//  clearWriteQueue(i, COMMON_MSGOBJ_IN_QUEUE, pc_serverData->msqDataServer.i32_wrHandle,iter_delete->ui16_pid);
#endif

    if (iter_delete->b_initReceived[i] && (pc_serverData->ui16_busRefCnt[i] > 0))
      pc_serverData->ui16_busRefCnt[i]--; // decrement bus ref count when client dropped off
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


static void enqueue_msg(uint32_t DLC, uint32_t ui32_id, uint32_t b_bus, uint8_t b_xtd, uint8_t* pui8_data, uint16_t ui16_pid, server_c* pc_serverData)
{

  can_data* pc_data;
  STL_NAMESPACE::list<client_c>::iterator iter, iter_delete = pc_serverData->l_clients.end();

  // mutex to prevent client list modification already got in calling function

  // prepare msqRead_s for each potential broadcast send
  msqRead_s msqReadBuf;
  pc_data = &(msqReadBuf.s_canData);
  // preset constant fields of msqReadBuf
  pc_data->i32_ident = ui32_id;
  pc_data->b_dlc = DLC;
  pc_data->b_xtd = b_xtd;
  memcpy( pc_data->pb_data, pui8_data, DLC );

  for (iter = pc_serverData->l_clients.begin(); iter != pc_serverData->l_clients.end(); iter++) {

    if (!iter->b_busUsed[b_bus])
      continue;

    // send signal 0 (no signal is send, but error handling is done) to check is process is alive
    if (kill(iter->ui16_pid, 0) == -1) {
      // client dead!
      DEBUG_PRINT1("client with ID %d no longer alive!\n", iter->ui16_pid);
      // remove this client from list after iterator loop
      iter_delete = iter;
      continue;
    }

    // i32_clientID != 0 in forwarding mode during send, do not enqueue this message for sending client
    if (ui16_pid && (iter->ui16_pid == ui16_pid))
      continue;

    // now search for MsgObj queue on this b_bus, where new message from b_bus maps
    // start with 1 since MsgObj with id 0 is anyway planned for sending
    const int32_t i32_maxObj = iter->arrMsgObj[b_bus].size();
    for (int32_t i32_obj = 1; i32_obj < i32_maxObj; i32_obj++) {

      if ( (iter->arrMsgObj[b_bus][i32_obj].ui8_bMsgType != RX )
        || (iter->arrMsgObj[b_bus][i32_obj].ui16_size == 0     ) )
      { // this MsgObj is no candidate for message receive
        continue;
      }

      if (!iter->arrMsgObj[b_bus][i32_obj].b_canObjConfigured) {
        continue;
      }

      if ( iter->arrMsgObj[b_bus][i32_obj].b_canBufferLock ) {
        // don't even check this MsgObj as it shall not receive messages
        DEBUG_PRINT2("lock bus %d, obj %d\n", b_bus, i32_obj);
        continue;
      }

      // compare received msg with filter
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
      if
        (
         (
          ( i32_obj < cui8_maxCanObj - 1 )
          && (
               ( (iter->arrMsgObj[b_bus][i32_obj].ui8_bufXtd == 1)
                  && (b_xtd == 1)
                  && ( (ui32_id & iter->ui32_globalMask[b_bus]) == ((iter->arrMsgObj[b_bus][i32_obj].ui32_filter) & iter->ui32_globalMask[b_bus]) )
               )
               ||
               ( (iter->arrMsgObj[b_bus][i32_obj].ui8_bufXtd == 0)
                  && (b_xtd == 0)
                  && ( (ui32_id & iter->ui16_globalMask[b_bus]) == (iter->arrMsgObj[b_bus][i32_obj].ui32_filter & iter->ui16_globalMask[b_bus]) )
                 )
               )
          )
         || (
             ( i32_obj == cui8_maxCanObj - 1)
             && (
                  ( (iter->arrMsgObj[b_bus][i32_obj].ui8_bufXtd == 1)
                    && (b_xtd == 1)
                    && ( (ui32_id & iter->ui32_globalMask[b_bus] & iter->ui32_lastMask[b_bus]) ==  ((iter->arrMsgObj[b_bus][i32_obj].ui32_filter) & iter->ui32_globalMask[b_bus] & iter->ui32_lastMask[b_bus]) )
                    )
                  ||
                  ( (iter->arrMsgObj[b_bus][i32_obj].ui8_bufXtd == 0)
                     && (b_xtd == 0)
                     && ( (ui32_id & iter->ui16_globalMask[b_bus] & iter->ui32_lastMask[b_bus]) ==  (iter->arrMsgObj[b_bus][i32_obj].ui32_filter & iter->ui16_globalMask[b_bus] & iter->ui32_lastMask[b_bus]) )
                  )
                )
             )
         )
#else
         if (
             ( (iter->arrMsgObj[b_bus][i32_obj].ui8_bufXtd == 1)
                   && (b_xtd == 1)
                   && ( (ui32_id & iter->arrMsgObj[b_bus][i32_obj].ui32_mask_xtd) == (iter->arrMsgObj[b_bus][i32_obj].ui32_filter & iter->arrMsgObj[b_bus][i32_obj].ui32_mask_xtd) )
                 )
                 ||
                 ( (iter->arrMsgObj[b_bus][i32_obj].ui8_bufXtd == 0)
                   && (b_xtd == 0)
                   && ( (ui32_id & iter->arrMsgObj[b_bus][i32_obj].ui16_mask_std) == (iter->arrMsgObj[b_bus][i32_obj].ui32_filter & iter->arrMsgObj[b_bus][i32_obj].ui16_mask_std) )
                 )
            )
#endif
        { // received msg fits actual filter
          DEBUG_PRINT("queueing message\n");

          pc_data->i32_time = getClientTime(*iter);

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
          DEBUG_PRINT1("mtype: 0x%08x\n", assembleRead_mtype(iter->ui16_pid, b_bus, i32_obj));
          msqReadBuf.i32_mtypePidBusObj = assembleRead_mtype(iter->ui16_pid, b_bus, i32_obj);
#else
          msqReadBuf.i32_mtypePidBusObj = assembleRead_mtype(iter->ui16_pid, b_bus, COMMON_MSGOBJ_IN_QUEUE);
          msqReadBuf.s_canData.bMsgObj = i32_obj;
#endif

          int i_rcSnd=msgsnd(pc_serverData->msqDataServer.i32_rdHandle, &msqReadBuf, sizeof(msqRead_s) - sizeof(int32_t), IPC_NOWAIT);
          if (i_rcSnd == -1)
          {
            int error = errno;
            DEBUG_PRINT2("error in msgsnd (errno: %d) %s\n", error, strerror(error));
            #ifdef CAN_SERVER_LOG_PATH
            STL_NAMESPACE::ofstream logging( CAN_SERVER_LOG_PATH );
            logging << "error in msgsnd (errno: " << errno << ")" << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
            if (error == EAGAIN)
            { // queue is full => remove oldest msg and try again
              msqWrite_s msqWriteBuf;
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
              DEBUG_PRINT4("message queue for CAN Ident: %x with Filter: %x, global Mask: %x for MsgObj: %d is full => try to remove oldest msg and send again!!\n",
                           ui32_id, iter->arrMsgObj[b_bus][i32_obj].ui32_filter, iter->ui32_globalMask[b_bus], i32_obj );
#else
              DEBUG_PRINT4("message queue for CAN Ident: %x with Filter: %x, Mask: %x for MsgObj: %d is full => try to remove oldest msg and send again!!\n",
                           ui32_id, iter->arrMsgObj[b_bus][i32_obj].ui32_filter, iter->arrMsgObj[b_bus][i32_obj].ui32_mask_xtd, i32_obj );
#endif
              #ifdef CAN_SERVER_LOG_PATH
              logging << "message queue for CAN Ident: " << ui32_id << " with Filter: "
                << iter->arrMsgObj[b_bus][i32_obj].ui32_filter << ", global Mask: " << iter->ui32_globalMask[b_bus]
                << " for MsgObj NR: " << i32_obj
                << " is full => try to remove oldest msg and send again!!" << INTERNAL_DEBUG_DEVICE_ENDL;
              #endif
              int i_rcRcv = msgrcv(pc_serverData->msqDataServer.i32_rdHandle, &msqWriteBuf, sizeof(msqWrite_s) - sizeof(int32_t), 0,IPC_NOWAIT);
              if ( i_rcRcv > 0 )
              { // number of received bytes > 0 => msgrcv successfull => try again
                DEBUG_PRINT("oldest msg from queue removed!!\n");
                #ifdef CAN_SERVER_LOG_PATH
                logging << "oldest msg from queue removed!!" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                i_rcSnd=msgsnd(pc_serverData->msqDataServer.i32_rdHandle, &msqReadBuf, sizeof(msqRead_s) - sizeof(int32_t), IPC_NOWAIT);
                if (i_rcSnd == 0)
                {
                  DEBUG_PRINT("message sent again after queue full!!\n");
                  #ifdef CAN_SERVER_LOG_PATH
                  logging << "message sent again after queue full!!" << INTERNAL_DEBUG_DEVICE_ENDL;
                  #endif
                }
              }
            }
          }

#ifndef CAN_SERVER_SKIP_CLIENT_WAKEUP_VIA_PIPE
          if (iter->i32_pipeHandle) {
            static uint8_t ui8_buf;

#ifndef CAN_SERVER_SKIP_PIPE_CLEARING
            // clear pipe (is done also in client)
            read(iter->i32_pipeHandle, &ui8_buf, 1); // just read one byte into static variable
#endif
            // trigger wakeup
            write(iter->i32_pipeHandle, &ui8_buf, 1);
          }
#endif

          // don't check following objects if message is already enqueued for this client
          break;

        } // if fit

    } // for objNr
  }// for iter

  if (iter_delete != pc_serverData->l_clients.end())
    // clear read/write queue for this client, close pipe, remove from client list
    releaseClient(pc_serverData, iter_delete);

}


STL_NAMESPACE::list<int32_t> __HAL::list_sendTimeStamps;

// This thread ahndles the client's writes to the server (us).
/////////////////////////////////////////////////////////////////////////
static void* can_write_thread_func(void* ptr)
{
  int16_t i16_rc;
  int32_t i32_error;
  msqWrite_s msqWriteBuf;

  server_c* pc_serverData = static_cast<server_c*>(ptr);

  bool b_highPrioMode=false; // default to FALSE until some bus reports >= 5 pending msgs.
  for (;;) {

    i32_error = 0;


    if (si_highPrioModeIfMin > 0)
    { // only if user requested high-prio mode via program parameter!
      if (b_highPrioMode)
      { // we've been in a pending-busy state before, so let's just update our pendingWrites to see if we still are...
        // otherwise we wouldn't have a chance to recover from highPrio mode. (the only chance is that messages get sent and there're less than 5 pending ones!
        updatePendingMsgs(pc_serverData, -1); // will get ALL "i_pendingMsgs" variables updated!
      }

      b_highPrioMode=false; // default to FALSE until some bus reports >= 5 pending msgs.
      for (uint8_t ui8_bus = 0; ui8_bus < cui32_maxCanBusCnt; ui8_bus++)
      {
        if (pc_serverData->i_pendingMsgs[ui8_bus] >= si_highPrioModeIfMin)
        {
          b_highPrioMode = true;
          break;
        }
      }
    }

    if (b_highPrioMode)
    { // at least 5 messages are pending - so check for the prioritized clients first now!
      // receive for max 10 msec only prioritized messages (listen for prioritized MTYPE msgs only)
      if (msgrcv(pc_serverData->msqDataServer.i32_wrHandle, &msqWriteBuf, sizeof(msqWrite_s) - sizeof(int32_t), MTYPE_WRITE_PRIO_HIGH, IPC_NOWAIT) == -1)
      { // no hi-prio msgs waiting in the queue...
        usleep(3000); // will probably actually wait for 10ms anyway :-(
        continue;
      }
    }
    else
    {
      if (msgrcv(pc_serverData->msqDataServer.i32_wrHandle, &msqWriteBuf, sizeof(msqWrite_s) - sizeof(int32_t), MTYPE_ANY, 0) == -1)
      {
        perror("msgrcv-all");
        usleep(1000);
        continue;
      }
    }


    // acquire mutex (prevents concurrent read/write access to can driver and modification of client list during execution of enqueue_msg
    pthread_mutex_lock( &(pc_serverData->m_protectClientList) );

    if (pc_serverData->i16_reducedLoadOnIsoBus == msqWriteBuf.ui8_bus)
    { // We're on ISOBUS, so mark this SA as LOCAL (i.e. NOT REMOTE)
      #ifdef DEBUG
      if (pc_serverData->arrb_remoteDestinationAddressInUse[msqWriteBuf.s_canMsg.ui32_id & 0xFF])
      {
        DEBUG_PRINT1("Reduced ISO bus load: source address 0x%x is now LOCAL (has been REMOTE until now)\n", msqWriteBuf.s_canMsg.ui32_id & 0xFF);
      }
      #endif
      pc_serverData->arrb_remoteDestinationAddressInUse[msqWriteBuf.s_canMsg.ui32_id & 0xFF] = false;
    }

    client_c* ps_client = NULL;
    // search client...
    for (STL_NAMESPACE::list<client_c>::iterator iter = pc_serverData->l_clients.begin(); iter != pc_serverData->l_clients.end(); iter++)
    {
      if (iter->ui16_pid == msqWriteBuf.ui16_pid) // not used any more in write thread!!! directly having the variables now! disassemble_client_id(msqWriteBuf.i32_mtype))
      {
        ps_client = &(*iter);
        break;
      }
    }
    if (ps_client != NULL)
    {
      addSendTimeStampToList(ps_client, msqWriteBuf.i32_sendTimeStamp);

      bool b_sendOnBus = true;
      // destination address check based on already collected source addresses from CAN bus messages
      if ( (pc_serverData->i16_reducedLoadOnIsoBus == msqWriteBuf.ui8_bus) &&
           (((msqWriteBuf.s_canMsg.ui32_id >> 16) & 0xFF) < 0xF0) && // PDU1 check
           (((msqWriteBuf.s_canMsg.ui32_id >> 8) & 0xFF) < 0xFE) ) // no broadcast (0xFF) or invalid (0xFE) message
      {
        if (!pc_serverData->arrb_remoteDestinationAddressInUse[(msqWriteBuf.s_canMsg.ui32_id >> 8) & 0xFF])
        { // destination address matches LOCAL source address
          b_sendOnBus = false; // ==> So DON'T send out on real ISOBUS.
        }
      }

      if (b_sendOnBus)
      {
        DEBUG_PRINT("+");
        i16_rc = sendToBus(msqWriteBuf.ui8_bus, &(msqWriteBuf.s_canMsg), pc_serverData);
        DEBUG_PRINT1("send: %d\n", i16_rc);
      }
      else
      { // no send => no error
        i16_rc = 1; // successfull return value of ca_TransmitCanCard_1
      }

      if (pc_serverData->b_logMode)
      { /** @todo shouldn't we only dump the message to the FILE if NO ERROR? Or at elast flag it like this in the can-log!! ? */
        dumpCanMsg (msqWriteBuf.ui8_bus, msqWriteBuf.ui8_obj, &(msqWriteBuf.s_canMsg), pc_serverData->f_canOutput[msqWriteBuf.ui8_bus]);
      }

      if (!i16_rc) 
        i32_error = HAL_OVERFLOW_ERR;
      else
      { // i16_rc != 0: send was ok
        if (i16_rc == HAL_NEW_SEND_DELAY)
        { // send was okay with "new send delay detected"!
          for (STL_NAMESPACE::list<client_c>::iterator iter = pc_serverData->l_clients.begin(); iter != pc_serverData->l_clients.end(); iter++)
          {
            iter->i32_sendDelay[msqWriteBuf.ui8_bus] = pc_serverData->i32_sendDelay[msqWriteBuf.ui8_bus];
          }
          i32_error = HAL_NEW_SEND_DELAY; // so it will be sent back with the ACK!
        }
        // in ANY okay case, do:
        // message forwarding
        // get clientID from msqWriteBuf.i32_mtype
        enqueue_msg(msqWriteBuf.s_canMsg.i32_len, msqWriteBuf.s_canMsg.ui32_id, msqWriteBuf.ui8_bus, msqWriteBuf.s_canMsg.i32_msgType, &msqWriteBuf.s_canMsg.ui8_data[0], msqWriteBuf.ui16_pid, pc_serverData); // not done any more: disassemble_client_id(msqWriteBuf.i32_mtype)
      }

      if (ps_client->i32_sendDelay[msqWriteBuf.ui8_bus] > 0)
      { // report the send delay back until the client explicitly clears it (sets it to negative!)
        i32_error = HAL_NEW_SEND_DELAY;
      }

      send_command_ack (msqWriteBuf.ui16_pid, &(pc_serverData->msqDataServer), ACKNOWLEDGE_DATA_CONTENT_ERROR_VALUE, i32_error); // not used any more, we have now direct fields in: disassemble_client_id(msqWriteBuf.i32_mtype)
    }

    pthread_mutex_unlock( &(pc_serverData->m_protectClientList) );

  }

  return NULL;
}


/////////////////////////////////////////////////////////////////////////
static void can_read(server_c* pc_serverData)
{
  can_recv_data receiveData;
  uint32_t DLC;
  bool b_moreToRead = TRUE;
  bool b_processMsg = FALSE;

  static canMsg_s s_canMsg;

  fd_set rfds;
  int retval;
  uint32_t channel;

  uint32_t channel_with_change = 0;
  uint8_t ui8_cntOpenDevice = 0;

  for (;;) {

    b_processMsg = FALSE;

    FD_ZERO(&rfds);

    ui8_cntOpenDevice = 0;

    for (channel=0; channel<cui32_maxCanBusCnt; channel++ )
    {
      if (isBusOpen(channel) && (pc_serverData->can_device[channel] > 0))
      { // valid file handle => use it for select
        FD_SET(pc_serverData->can_device[channel], &rfds);
        ui8_cntOpenDevice++;
      }
    }

    // we have open devices
    if (ui8_cntOpenDevice > 0) {
      retval = select(FD_SETSIZE, &rfds, NULL, NULL, NULL);  // wait infinitely for next CAN msg

      if(retval == -1) {
        DEBUG_PRINT("Error Occured in select\n");
        break;
      } else if (retval == 0) {
        continue;
      } else {

        // get device with changes
        for (channel_with_change=0; channel_with_change<cui32_maxCanBusCnt; channel_with_change++ ) {
          if (isBusOpen(channel_with_change)) {
            if (FD_ISSET(pc_serverData->can_device[channel_with_change], &rfds) == 1) {
              DEBUG_PRINT2("change on channel %d, FD_ISSET %d\n", channel_with_change, FD_ISSET(pc_serverData->can_device[channel_with_change], &rfds));
              b_processMsg = TRUE;
              break;
            }
          }
        }
      }
    }
    else
    {
      usleep(50000); // no device => nothing to read
      // check devices which have no device handle (RTE)
      b_processMsg = TRUE;
    }

    if (b_processMsg) {
      // acquire mutex to prevent concurrent read/write to can driver
      pthread_mutex_lock( &(pc_serverData->m_protectClientList) );

      int16_t i16_rc = readFromBus(channel_with_change, &s_canMsg, pc_serverData);

      if (i16_rc < 0) {
        /* nothing to read or interrupted system call */
#ifdef DEBUG
        perror("ioctl");
#endif
        DEBUG_PRINT1("CANRead error: %i\n", i16_rc);
        b_processMsg = FALSE;

        // unlock mutex
        pthread_mutex_unlock( &(pc_serverData->m_protectClientList) );

        continue;
      }

      if (i16_rc == 0)
      { // invalid message
        pthread_mutex_unlock( &(pc_serverData->m_protectClientList) );
        continue;
      }

    }

    if (pc_serverData->b_inputFileMode && b_moreToRead) {
      b_moreToRead = readCanDataFile(pc_serverData, &receiveData);
      b_processMsg = TRUE;

      // acquire mutex to prevent modification of client list during execution of enqueue_msg
      pthread_mutex_lock( &(pc_serverData->m_protectClientList) );

    }

    if (!b_processMsg) {
      usleep(10000);
      continue;
    }

    DLC = ( s_canMsg.i32_len & 0xF );
    if ( DLC > 8 ) DLC = 8;

    const uint8_t b_xtd = s_canMsg.i32_msgType;

    DEBUG_PRINT4("DLC %d, ui32_id 0x%08x, b_bus %d, b_xtd %d\n", DLC, s_canMsg.ui32_id, channel_with_change, b_xtd);

    if (s_canMsg.ui32_id >= 0x7FFFFFFF) {
      DEBUG_PRINT1("!!Received of malformed message with undefined CAN ident: %x\n", s_canMsg.ui32_id);
    }
    else
    { // check for new source address
      if (pc_serverData->i16_reducedLoadOnIsoBus == (int16_t)channel_with_change)
      { // On ISOBUS, mark this SA as REMOTE
        #ifdef DEBUG
        if (!pc_serverData->arrb_remoteDestinationAddressInUse[s_canMsg.ui32_id & 0xFF] && ((s_canMsg.ui32_id & 0xFF) != 0xFE)) // skip 0xFE source address
        { // new, unknown source address
          DEBUG_PRINT1("Reduced ISO bus load: new source address 0x%x is now marked as REMOTE (was LOCAL before).\n", s_canMsg.ui32_id & 0xFF);
        }
        #endif
        pc_serverData->arrb_remoteDestinationAddressInUse[s_canMsg.ui32_id & 0xFF] = true;
      }

      if (pc_serverData->b_logMode)
      { /** @todo shouldn't we only dump the message to the FILE if NO ERROR? Or at elast flag it like this in the can-log!! ? */
        dumpCanMsg (channel_with_change, 0/* we don't have no msgobj when receiving .. msqWriteBuf.ui8_obj*/, &s_canMsg, pc_serverData->f_canOutput[channel_with_change]);
      }
      enqueue_msg(DLC, s_canMsg.ui32_id, channel_with_change, b_xtd, &s_canMsg.ui8_data[0], 0, pc_serverData);
    }

    // release mutex
    pthread_mutex_unlock( &(pc_serverData->m_protectClientList) );

  }
}



/////////////////////////////////////////////////////////////////////////
static void* command_thread_func(void* ptr)
{

  int16_t i16_rc;
  int32_t i32_error;
  int32_t i32_dataContent;
  int32_t i32_data;
  transferBuf_s s_transferBuf;

  server_c* pc_serverData = static_cast<server_c*>(ptr);

  for (;;) {

    // check command queue
    /* The length is essentially the size of the structure minus sizeof(mtype) */
    if((i16_rc = msgrcv(pc_serverData->msqDataServer.i32_cmdHandle, &s_transferBuf, sizeof(transferBuf_s) - sizeof(int32_t), 0, 0)) == -1) {
      perror("msgrcv");
      usleep(1000);
      continue;
    }

    DEBUG_PRINT1("cmd %d\n", s_transferBuf.ui16_command);

    i32_error = 0;

    // acquire mutex
    pthread_mutex_lock( &(pc_serverData->m_protectClientList) );

    // get client
    STL_NAMESPACE::list<client_c>::iterator iter_client = pc_serverData->l_clients.end(), tmp_iter;
    for (tmp_iter = pc_serverData->l_clients.begin(); tmp_iter != pc_serverData->l_clients.end(); tmp_iter++)
      if (tmp_iter->ui16_pid == s_transferBuf.i32_mtypePid) // here the mtype is the PID without any disassembling needed!
      {
        iter_client = tmp_iter;
        break;
      }

    // default to simple ACK which returns the error.
    // Overwrite if special answers needed like PIPE_ID or SEND_DELAY
    i32_dataContent = ACKNOWLEDGE_DATA_CONTENT_ERROR_VALUE;
    // in DATA_CONTENT_ERROR_VALUE case i32_error will be used as i32_data!!!
    i32_error = 0; // default to no error
    if (s_transferBuf.ui16_command != COMMAND_REGISTER)
    { /// iter_client check needed, iter_client WILL be used and MUST be set!
      if (iter_client == pc_serverData->l_clients.end())
      { // client not found, but was required. return HAL_CONFIG_ERR
        i32_error = HAL_CONFIG_ERR;
      }
    }

    // if still no error, evaluate command
    if (!i32_error)
    {
      switch (s_transferBuf.ui16_command)
      {

        case COMMAND_REGISTER:
        {
          client_c s_tmpClient; // constructor initialize values to zero
          DEBUG_PRINT("command start driver\n");

          // do check for dead clients before queueing any new message
          for (STL_NAMESPACE::list<client_c>::iterator iter_deadClient = pc_serverData->l_clients.begin(); iter_deadClient != pc_serverData->l_clients.end();) {

            // send signal 0 (no signal is send, but error handling is done) to check is process is alive
            if (kill(iter_deadClient->ui16_pid, 0) == -1) {
              // client dead!
              DEBUG_PRINT1("client with ID %d no longer alive!\n", iter_deadClient->ui16_pid);
              // clear read/write queue for this client, close pipe, remove from client list, iter_deadClient is incremented
              releaseClient(pc_serverData, iter_deadClient);
            } else
              // increment iter_deadClient manually (not in for statement)
              iter_deadClient++;
          }

          // no need to set to 0, as everything got set to zero before!
          // s_tmpClient.i32_sendDelay[all-buses] = 0;

          // client process id is used as clientID
          s_tmpClient.ui16_pid = s_transferBuf.i32_mtypePid;

          DEBUG_PRINT1 ("Client registering with startTimeClock_t from his REGISTER message as %d\n", s_transferBuf.s_startTimeClock.t_clock);
          initClientTime( s_tmpClient, s_transferBuf.s_startTimeClock.t_clock );

          //DEBUG_PRINT1("client start up time (absolute value in clocks): %d\n", s_tmpClient.t_startTimeClock);

          char pipe_name[255];
          sprintf(pipe_name, "%s%d", PIPE_PATH, ++(pc_serverData->i32_lastPipeId));

          umask(0);
          // if not existing already
          mknod(pipe_name, S_IFIFO|0666, 0);

          // open pipe in read/write mode to allow read access to clean the pipe and to avoid SIGPIPE when client dies and this client was the only reader to the pipe
          if ((s_tmpClient.i32_pipeHandle = open(pipe_name, O_NONBLOCK | O_RDWR, 0)) == -1)
            i32_error = HAL_UNKNOWN_ERR;

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
          // used cui8_maxCanObj MsgObj
          for (uint8_t j=0; j<cui32_maxCanBusCnt; j++)
            s_tmpClient.arrMsgObj[j].resize(cui8_maxCanObj);
  #endif

          if (!i32_error)
          { // no error
            pc_serverData->l_clients.push_back(s_tmpClient);
            // transmit current pipeId to client (for composition of pipe name)
            i32_dataContent = ACKNOWLEDGE_DATA_CONTENT_PIPE_ID;
            i32_data = pc_serverData->i32_lastPipeId;
          }
        } break;


        case COMMAND_DEREGISTER: {
          DEBUG_PRINT("command stop driver\n");

          // @todo: is queue clearing necessary?
          for (uint8_t j=0; j<cui32_maxCanBusCnt; j++)
          {
            iter_client->arrMsgObj[j].clear();
            if (iter_client->b_initReceived[j] && (pc_serverData->ui16_busRefCnt[j] > 0))
              pc_serverData->ui16_busRefCnt[j]--; // decrement ref count only when we received the INIT command before
          }

          if (iter_client->i32_pipeHandle)
            close(iter_client->i32_pipeHandle);

          pc_serverData->l_clients.erase(iter_client);
          // i32_error will stay at 0 for "no error"
        } break;


        case COMMAND_INIT:

          if (s_transferBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR)
            i32_error = HAL_RANGE_ERR;
          else if (!pc_serverData->ui16_busRefCnt[s_transferBuf.s_init.ui8_bus])
          { // first init command for current bus
            // open log file only once per bus
            if (pc_serverData->b_logMode) {
              char file_name[255];
              snprintf(file_name, 255, "%s_%hx", pc_serverData->logFileBase.c_str(), s_transferBuf.s_init.ui8_bus);
              if ((pc_serverData->f_canOutput[s_transferBuf.s_init.ui8_bus] = fopen(file_name, "a+")) == NULL ) {
                perror("fopen");
                exit(1);
              }
            }

            // just to get sure that we reset the number of pending write-messages
            pc_serverData->i_pendingMsgs[s_transferBuf.s_init.ui8_bus] = 0;

            int16_t i16_init_rc = openBusOnCard(s_transferBuf.s_init.ui8_bus,  // 0 for CANLPT/ICAN, else 1 for first BUS
                                                s_transferBuf.s_init.ui16_wBitrate,  // BTR0BTR1
                                                pc_serverData);

            if (!i16_init_rc) {
              printf("can't initialize CAN\n");
              i32_error = HAL_CONFIG_ERR;
            }
          }

          if (!i32_error) {
            pc_serverData->ui16_busRefCnt[s_transferBuf.s_init.ui8_bus]++;
            iter_client->b_initReceived[s_transferBuf.s_init.ui8_bus] = true; // when the CLOSE command is received => allow decrement of ref count
            iter_client->b_busUsed[s_transferBuf.s_init.ui8_bus] = true; // when the CLOSE command is received => allow decrement of ref count
          }
          // do rest of init handling in next case statement (no break!)

        case COMMAND_CHG_GLOBAL_MASK:
          if (!i32_error) {
            iter_client->ui16_globalMask[s_transferBuf.s_init.ui8_bus] = s_transferBuf.s_init.ui16_wGlobMask;
            iter_client->ui32_globalMask[s_transferBuf.s_init.ui8_bus] = s_transferBuf.s_init.ui32_dwGlobMask;
            iter_client->ui32_lastMask[s_transferBuf.s_init.ui8_bus] = s_transferBuf.s_init.ui32_dwGlobMaskLastmsg;
          }
          break;


        case COMMAND_CLOSE:
        {
          if (s_transferBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR )
            i32_error = HAL_RANGE_ERR;
          else
          {
            if (iter_client->b_initReceived[s_transferBuf.s_init.ui8_bus] && (pc_serverData->ui16_busRefCnt[s_transferBuf.s_init.ui8_bus] > 0))
            {
              pc_serverData->ui16_busRefCnt[s_transferBuf.s_init.ui8_bus]--; // decrement ref count only when we received the INIT command before
            }

            iter_client->b_initReceived[s_transferBuf.s_init.ui8_bus] = false; // reset flag

            if (pc_serverData->ui16_busRefCnt[s_transferBuf.s_init.ui8_bus] == 0)
            { // last connection on bus closed, so reset pending msgs...
              pc_serverData->i_pendingMsgs[s_transferBuf.s_init.ui8_bus] = 0;
            }

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
            for (uint8_t j=0; j<iter_client->arrMsgObj[s_transferBuf.s_init.ui8_bus].size(); j++) {
              clearReadQueue (s_transferBuf.s_init.ui8_bus, j, pc_serverData->msqDataServer.i32_rdHandle, iter_client->ui16_pid);
//            clearWriteQueue(s_transferBuf.s_init.ui8_bus, j, pc_serverData->msqDataServer.i32_wrHandle, iter_client->ui16_pid);
            }
  #else
            clearReadQueue (s_transferBuf.s_init.ui8_bus, COMMON_MSGOBJ_IN_QUEUE, pc_serverData->msqDataServer.i32_rdHandle, iter_client->ui16_pid);
//          clearWriteQueue(s_transferBuf.s_init.ui8_bus, COMMON_MSGOBJ_IN_QUEUE, pc_serverData->msqDataServer.i32_wrHandle, iter_client->ui16_pid);
  #endif

            if (!pc_serverData->ui16_busRefCnt[s_transferBuf.s_init.ui8_bus] && pc_serverData->b_logMode && pc_serverData->f_canOutput[s_transferBuf.s_init.ui8_bus]) {
              fclose(pc_serverData->f_canOutput[s_transferBuf.s_init.ui8_bus]);
              pc_serverData->f_canOutput[s_transferBuf.s_init.ui8_bus] = 0;
            }

  #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
            iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus].clear();
  #endif
            // i32_error will stay at 0 for "no error"
          }
        } break;


        case COMMAND_CONFIG:
  #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
          if (s_transferBuf.s_config.ui8_obj >= iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus].size()) {
            // add new elements in the vector with resize
            iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus].resize(s_transferBuf.s_config.ui8_obj+1);
          } else {
            // reconfigure element
          }
          // break left out intentionally
  #endif

        case COMMAND_CHG_CONFIG:

          if ((s_transferBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( s_transferBuf.s_config.ui8_obj > iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus].size()-1 ))
            i32_error = HAL_RANGE_ERR;
          else
          {
            iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].b_canObjConfigured = TRUE;

            iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].ui8_bufXtd = s_transferBuf.s_config.ui8_bXtd;
            iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].ui32_filter = s_transferBuf.s_config.ui32_dwId;

    #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
            if (s_transferBuf.s_config.ui8_bXtd)
                iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].ui32_mask_xtd = s_transferBuf.s_config.ui32_mask;
            else
                iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].ui16_mask_std = s_transferBuf.s_config.ui32_mask;
    #endif

            if (s_transferBuf.ui16_command == COMMAND_CONFIG) {
              clearReadQueue (s_transferBuf.s_config.ui8_bus, s_transferBuf.s_config.ui8_obj, pc_serverData->msqDataServer.i32_rdHandle, iter_client->ui16_pid);
//            clearWriteQueue(s_transferBuf.s_config.ui8_bus, s_transferBuf.s_config.ui8_obj, pc_serverData->msqDataServer.i32_wrHandle, iter_client->ui16_pid);

              iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].ui8_bMsgType = s_transferBuf.s_config.ui8_bMsgType;
              iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].ui16_size = s_transferBuf.s_config.ui16_wNumberMsgs;
              iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].b_canBufferLock = false;
            }
          }
          break;


        case COMMAND_LOCK:
        case COMMAND_UNLOCK:

          if ((s_transferBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( s_transferBuf.s_config.ui8_obj > iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus].size()-1 ))
            i32_error = HAL_RANGE_ERR;
          else {
            if (s_transferBuf.ui16_command == COMMAND_LOCK) {
              iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].b_canBufferLock = TRUE;
              DEBUG_PRINT2("locked buf %d, obj %d\n", s_transferBuf.s_config.ui8_bus, s_transferBuf.s_config.ui8_obj);
            } else {
              iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].b_canBufferLock = FALSE;
              DEBUG_PRINT2("unlocked buf %d, obj %d\n", s_transferBuf.s_config.ui8_bus, s_transferBuf.s_config.ui8_obj);
            }
          }
          break;


        case COMMAND_QUERYLOCK:

          if ((s_transferBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( s_transferBuf.s_config.ui8_obj > iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus].size()-1 ))
            i32_error = HAL_RANGE_ERR;
          else {
            i32_dataContent = ACKNOWLEDGE_DATA_CONTENT_QUERY_LOCK;
            i32_data = iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].b_canBufferLock;
          }
          break;


        case COMMAND_CLOSEOBJ:

          if ((s_transferBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( s_transferBuf.s_config.ui8_obj > iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus].size()-1 ))
            i32_error = HAL_RANGE_ERR;
          else {
            iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].b_canObjConfigured = FALSE;

            iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].b_canBufferLock = FALSE;
            iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus][s_transferBuf.s_config.ui8_obj].ui16_size = 0;
            clearReadQueue (s_transferBuf.s_config.ui8_bus, s_transferBuf.s_config.ui8_obj, pc_serverData->msqDataServer.i32_rdHandle, iter_client->ui16_pid);
//          clearWriteQueue(s_transferBuf.s_config.ui8_bus, s_transferBuf.s_config.ui8_obj, pc_serverData->msqDataServer.i32_wrHandle, iter_client->ui16_pid);

  #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
            // erase element if it is the last in the vector, otherwise it can stay there
            while (iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus].back().b_canObjConfigured == FALSE)
                iter_client->arrMsgObj[s_transferBuf.s_config.ui8_bus].pop_back();
  #endif
          }
          break;


        case COMMAND_SEND_DELAY:
          if ( s_transferBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR )
          {
            i32_error = HAL_RANGE_ERR;
          } else {
            i32_dataContent = ACKNOWLEDGE_DATA_CONTENT_SEND_DELAY;
            i32_data = iter_client->i32_sendDelay [s_transferBuf.s_config.ui8_bus];
            if (i32_data < 0) i32_data = -i32_data; // so we always return a positive send delay!
            else iter_client->i32_sendDelay [s_transferBuf.s_config.ui8_bus] = -i32_data; // stamp a positive stored delay as READ by negating it!
          }
          break;


        default:
          i32_error = HAL_UNKNOWN_ERR;
          break;
      } // end switch
    } // end error check


    // do centralized error-answering here
    if (i32_dataContent == ACKNOWLEDGE_DATA_CONTENT_ERROR_VALUE) i32_data = i32_error;
    send_command_ack(s_transferBuf.i32_mtypePid, &(pc_serverData->msqDataServer), i32_dataContent, i32_data);


    // release mutex
    pthread_mutex_unlock( &(pc_serverData->m_protectClientList) );

  } // end for

}



/////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void print_trace (void)
{
    void * array[100];
    int size;
    char ** strings;
    int i;

    int pid = getpid();

    size = backtrace (array, 100);
    strings = backtrace_symbols (array, size);

    fprintf (stderr,"Obtained %i stack frames.\n", size);

    for (i = 0; i < size; i++)
    {
        fprintf (stderr,"(%d) %s\n", pid, strings[i]);
    }
}


void segfaulthand(int /* i */ )
{
    delete[] reserve_memory_heap;

    print_trace();
    killpg( 0, 9 ); // pgid 0 means process group of current process.
    exit(-1);
}
#endif

int main(int argc, char *argv[])
{
  pthread_t thread_registerClient, thread_canWrite;
  int i_registerClientThreadHandle, i_canWriteThreadHandle;
  server_c c_serverData;
  int i_canReadNiceValue = 0;

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  printf("SYSTEM_WITH_ENHANCED_CAN_HAL is defined !\n");
#else
  printf("SYSTEM_WITH_ENHANCED_CAN_HAL is NOT defined !\n");
#endif

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    usage();
    exit(0);
  }

  for (int i=1; i<argc; i=i+2) {
    if (i==argc-1) {
      printf("error: option needs second parameter\n");
      exit(1);
    }
    if (strcmp(argv[i], "--log") == 0) {
      c_serverData.logFileBase = argv[i+1];
      c_serverData.b_logMode=true;
    }
    if (strcmp(argv[i], "--file-input") == 0) {
      c_serverData.inputFile = argv[i+1];
      c_serverData.b_inputFileMode=true;
    }
    if (strcmp(argv[i], "--high-prio-minimum") == 0) {
      si_highPrioModeIfMin = atoi (argv[i+1]); // 0 for no prio mode!
    }
    if (strcmp(argv[i], "--reduced-load-iso-bus-no") == 0) {
      c_serverData.i16_reducedLoadOnIsoBus = atoi (argv[i+1]);
      printf("reduced bus load on ISO bus no. %d\n", c_serverData.i16_reducedLoadOnIsoBus); fflush(0);
    }
    if (strcmp(argv[i], "--nice-can-read") == 0) {
      i_canReadNiceValue = atoi (argv[i+1]); // 0 for no prio mode!
      printf("nice value for can read thread: %d\n", i_canReadNiceValue); fflush(0);
    }
  }

  getTime();

  apiversion = initCardApi();
  if ( apiversion == 0 ) { // failure - nothing found
    DEBUG_PRINT1("FAILURE - No CAN card was found with automatic search with IO address %04X for manually configured cards\r\n", LptIsaIoAdr );
    exit(1);
  }


  // do the reset
  int i = resetCard();
  if (i) {
    DEBUG_PRINT("Reset CANLPT ok\n");
  } else   {
    DEBUG_PRINT("Reset CANLPT not ok\n");
    exit(1);
  }

  // wait to be shure that CAN card is clean reset
  //  usleep(100);

  int16_t i16_rc = ca_createMsqs(c_serverData.msqDataServer);

  if (i16_rc) {
    printf("error creating queues\n");
    exit(1);
  }
#ifdef DEBUG
  reserve_memory_heap = new char[4096];
  signal(11,segfaulthand);
#endif

  printf("creating threads\n");

  i_registerClientThreadHandle = pthread_create( &thread_registerClient, NULL, &command_thread_func, &c_serverData);
  i_canWriteThreadHandle = pthread_create( &thread_canWrite, NULL, &can_write_thread_func, &c_serverData);

  printf("operating\n");

#ifndef WIN32
  if (i_canReadNiceValue)
    setpriority(PRIO_PROCESS, 0, i_canReadNiceValue);
#endif

  can_read(&c_serverData);

}
