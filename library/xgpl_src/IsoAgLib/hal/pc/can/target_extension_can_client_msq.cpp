/*
  target_extension_can_client_msq.cpp:
    CAN-Server client based on message-queues

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/isoaglib_config.h>
#include "can_target_extensions.h"
#include <IsoAgLib/hal/pc/system/system.h>
#include <IsoAgLib/hal/pc/system/system_target_extensions.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <list>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>


#include "can_server.h"

namespace __HAL {


// globals
static msqData_s msqDataClient = {-1,-1,-1,-1,-1,-1};
static int32_t i32_lastReceiveTime = 0;
static bool gsb_sendPrioritized = false;


// Special function only for this module
// can be used directly in special-case
// when you know what you're doing...
void setSendPrioritized (bool prio)
{
  gsb_sendPrioritized = prio;
}


int32_t getPipeHandleForCanRcvEvent()
{
  return msqDataClient.i32_pipeHandle;
}

/////////////////////////////////////////////////////////////////////////
int16_t can_startDriver()
{

  transferBuf_s s_transferBuf;
  memset(&s_transferBuf, 0, sizeof(transferBuf_s));

  DEBUG_PRINT("can_startDriver called\n");
#if DEBUG_CAN
#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  printf("SYSTEM_WITH_ENHANCED_CAN_HAL is defined !\n");
#else
  printf("SYSTEM_WITH_ENHANCED_CAN_HAL is NOT defined !\n");
#endif
#endif

  int16_t i16_rc = createMsqs(msqDataClient);

  if (i16_rc)
    // queue connect/creation failed
    return i16_rc;

  // client registration

  // use process id for own client id
  s_transferBuf.i32_mtypePid = msqDataClient.i32_pid;
  s_transferBuf.ui16_command = COMMAND_REGISTER;
  s_transferBuf.s_startTimeClock.t_clock = getStartupTime();

  i16_rc = send_command(&s_transferBuf, &msqDataClient);

  if (i16_rc == HAL_NO_ERR) {

    if (s_transferBuf.s_acknowledge.i32_dataContent != ACKNOWLEDGE_DATA_CONTENT_PIPE_ID)
    {
      if (s_transferBuf.s_acknowledge.i32_dataContent == ACKNOWLEDGE_DATA_CONTENT_ERROR_VALUE)
        return s_transferBuf.s_acknowledge.i32_data; // this is the error-code!
      else
        return HAL_UNKNOWN_ERR;
    }
    // else: PIPE_ID
    char pipe_name[255];
    sprintf(pipe_name, "%s%d", PIPE_PATH, s_transferBuf.s_acknowledge.i32_data); // == i32_pipeId

    // pipe is already created by can server!

    // O_NONBLOCK because open in read only mode blocks!
    if ((msqDataClient.i32_pipeHandle = open(pipe_name, O_NONBLOCK | O_RDONLY)) == -1)
      return HAL_UNKNOWN_ERR;
  }

  return i16_rc;

}

int16_t can_stopDriver()
{
  transferBuf_s s_transferBuf;

  DEBUG_PRINT("can_stopDriver called\n");

  if (msqDataClient.i32_pipeHandle)
    close(msqDataClient.i32_pipeHandle);

  s_transferBuf.i32_mtypePid = msqDataClient.i32_pid;
  s_transferBuf.ui16_command = COMMAND_DEREGISTER;

  return send_command(&s_transferBuf, &msqDataClient);

}


int16_t init_can ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg,uint16_t wBitrate )
{
  transferBuf_s s_transferBuf;
  memset(&s_transferBuf, 0, sizeof(transferBuf_s));

  DEBUG_PRINT2("init_can, bus %d, bitrate %d\n", bBusNumber, wBitrate);

  if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) return HAL_RANGE_ERR;

  s_transferBuf.i32_mtypePid = msqDataClient.i32_pid;
  s_transferBuf.ui16_command = COMMAND_INIT;
  s_transferBuf.s_init.ui8_bus = bBusNumber;
  s_transferBuf.s_init.ui16_wGlobMask = wGlobMask;
  s_transferBuf.s_init.ui32_dwGlobMask = dwGlobMask;
  s_transferBuf.s_init.ui32_dwGlobMaskLastmsg = dwGlobMaskLastmsg;
  s_transferBuf.s_init.ui16_wBitrate = wBitrate;

  return send_command(&s_transferBuf, &msqDataClient);

};

int16_t changeGlobalMask( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg )
{
  transferBuf_s s_transferBuf;

  DEBUG_PRINT1("changeGlobalMask, bus %d\n", bBusNumber);

  if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) return HAL_RANGE_ERR;

  s_transferBuf.i32_mtypePid = msqDataClient.i32_pid;
  s_transferBuf.ui16_command = COMMAND_CHG_GLOBAL_MASK;
  s_transferBuf.s_init.ui8_bus = bBusNumber;
  s_transferBuf.s_init.ui16_wGlobMask = wGlobMask;
  s_transferBuf.s_init.ui32_dwGlobMask = dwGlobMask;
  s_transferBuf.s_init.ui32_dwGlobMaskLastmsg = dwGlobMaskLastmsg;

  return send_command(&s_transferBuf, &msqDataClient);

};

int16_t closeCan ( uint8_t bBusNumber )
{
  transferBuf_s s_transferBuf;
  memset(&s_transferBuf, 0, sizeof(transferBuf_s));

  DEBUG_PRINT1("closeCan, bus %d\n", bBusNumber);

  if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) return HAL_RANGE_ERR;

  s_transferBuf.i32_mtypePid = msqDataClient.i32_pid;
  s_transferBuf.ui16_command = COMMAND_CLOSE;
  s_transferBuf.s_init.ui8_bus = bBusNumber;

  // read/write queue is cleared by server!
  return send_command(&s_transferBuf, &msqDataClient);

};

int16_t chgCanObjPause ( uint8_t bBusNumber, uint8_t bMsgObj, uint16_t wPause)
{
  (void)bBusNumber;
  (void)bMsgObj;
  (void)wPause;

  DEBUG_PRINT2("chgCanObjPause, bus %d, obj %d\n", bBusNumber, bMsgObj);
/*
  fprintf(stderr,"sende Pause auf BUS %d fuer CAN Objekt %d auf %d eingestellt\n",
  (int32_t)bBusNumber, (int32_t)bMsgObj, wPause);
  */
  return HAL_NO_ERR;
}

int16_t getCanBusStatus(uint8_t /*bBusNumber*/, tCanBusStatus* /*ptStatus*/)
{
  //DEBUG_PRINT1("getCanBusStatus, bus %d\n", bBusNumber);

  return HAL_NO_ERR;
}


int16_t configCanObj ( uint8_t bBusNumber, uint8_t bMsgObj, tCanObjConfig* ptConfig )
{
  DEBUG_PRINT2("configCanObj, bus %d, obj %d\n", bBusNumber, bMsgObj);

  transferBuf_s s_transferBuf;
  memset(&s_transferBuf, 0, sizeof(transferBuf_s));

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > cui8_maxCanObj-1 ) ) return HAL_RANGE_ERR;
#else
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) ) return HAL_RANGE_ERR;
#endif

  s_transferBuf.i32_mtypePid = msqDataClient.i32_pid;
  s_transferBuf.ui16_command = COMMAND_CONFIG;
  s_transferBuf.s_config.ui8_bus = bBusNumber;
  s_transferBuf.s_config.ui8_obj = bMsgObj;
  s_transferBuf.s_config.ui8_bXtd = ptConfig->bXtd;
  s_transferBuf.s_config.ui8_bMsgType = ptConfig->bMsgType;
  s_transferBuf.s_config.ui32_dwId = ptConfig->dwId;

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  s_transferBuf.s_config.ui32_mask = ptConfig->mask;
#endif

  s_transferBuf.s_config.ui16_wNumberMsgs = ptConfig->wNumberMsgs;

  // read/write queue is cleared by server!
  return send_command(&s_transferBuf, &msqDataClient);

};



int16_t closeCanObj ( uint8_t bBusNumber,uint8_t bMsgObj )
{
  transferBuf_s s_transferBuf;

  DEBUG_PRINT2("closeCanObj, bus %d, obj %d\n", bBusNumber, bMsgObj);

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > cui8_maxCanObj-1 ) ) return HAL_RANGE_ERR;
#else
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) ) return HAL_RANGE_ERR;
#endif

  s_transferBuf.i32_mtypePid = msqDataClient.i32_pid;
  s_transferBuf.ui16_command = COMMAND_CLOSEOBJ;
  s_transferBuf.s_config.ui8_bus = bBusNumber;
  s_transferBuf.s_config.ui8_obj = bMsgObj;

  // read / write queue is cleared by server!
  return send_command(&s_transferBuf, &msqDataClient);

};

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint8_t bXtd )
#else
int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint32_t mask, uint8_t bXtd )
#endif
{

  DEBUG_PRINT2("chgCanObjId, bus %d, obj %d\n", bBusNumber, bMsgObj);
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > cui8_maxCanObj-1 ) ) return HAL_RANGE_ERR;
#else
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) ) return HAL_RANGE_ERR;
#endif

  transferBuf_s s_transferBuf;

  s_transferBuf.i32_mtypePid = msqDataClient.i32_pid;
  s_transferBuf.ui16_command = COMMAND_CHG_CONFIG;
  s_transferBuf.s_config.ui8_bus = bBusNumber;
  s_transferBuf.s_config.ui8_obj = bMsgObj;
  s_transferBuf.s_config.ui8_bXtd = bXtd;
  s_transferBuf.s_config.ui32_dwId = dwId;

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  s_transferBuf.s_config.ui32_mask = mask;
#endif

  return send_command(&s_transferBuf, &msqDataClient);

}
/**
       lock a MsgObj to avoid further placement of messages into buffer.
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
       @param ab_doLock true==lock(default); false==unlock
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
       */
int16_t lockCanObj( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, bool ab_doLock )
{

  transferBuf_s s_transferBuf;

  DEBUG_PRINT3("lockCanObj, bus %d, obj %d, lock %d\n", aui8_busNr, aui8_msgobjNr, ab_doLock);

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( ( aui8_busNr > HAL_CAN_MAX_BUS_NR ) || ( aui8_msgobjNr > cui8_maxCanObj-1 ) ) return HAL_RANGE_ERR;
#else
  if ( ( aui8_busNr > HAL_CAN_MAX_BUS_NR ) ) return HAL_RANGE_ERR;
#endif

  s_transferBuf.i32_mtypePid = msqDataClient.i32_pid;

  if (ab_doLock)
    s_transferBuf.ui16_command = COMMAND_LOCK;
  else
    s_transferBuf.ui16_command = COMMAND_UNLOCK;

  s_transferBuf.s_config.ui8_bus = aui8_busNr;
  s_transferBuf.s_config.ui8_obj = aui8_msgobjNr;

  return send_command(&s_transferBuf, &msqDataClient);

}

/**
       check if MsgObj is currently locked
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check
       @return true -> MsgObj is currently locked
*/
bool getCanMsgObjLocked( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
{

  int16_t i16_rc;
  transferBuf_s s_transferBuf;

  DEBUG_PRINT2("getCanMsgObjLocked, bus %d, obj %d\n", aui8_busNr, aui8_msgobjNr);

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( ( aui8_busNr > HAL_CAN_MAX_BUS_NR ) || ( aui8_msgobjNr > cui8_maxCanObj-1 ) ) return true;
#else
  if ( ( aui8_busNr > HAL_CAN_MAX_BUS_NR ) ) return true;
#endif
  else {

    s_transferBuf.i32_mtypePid = msqDataClient.i32_pid;
    s_transferBuf.ui16_command = COMMAND_QUERYLOCK;
    s_transferBuf.s_config.ui8_bus = aui8_busNr;
    s_transferBuf.s_config.ui8_obj = aui8_msgobjNr;

    i16_rc = send_command(&s_transferBuf, &msqDataClient);

    if (!i16_rc)
    {
      if (s_transferBuf.s_acknowledge.i32_dataContent != ACKNOWLEDGE_DATA_CONTENT_QUERY_LOCK)
        return false;
      else
        return s_transferBuf.s_acknowledge.i32_data;
    }
    else
      return false;
  }

}


int16_t clearCanObjBuf(uint8_t bBusNumber, uint8_t bMsgObj)
{

  DEBUG_PRINT2("clearCanObjBuf, bus %d, obj %d\n", bBusNumber, bMsgObj);

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > cui8_maxCanObj-1 ) ) return HAL_RANGE_ERR;
#else
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) ) return HAL_RANGE_ERR;
#endif

  clearReadQueue(bBusNumber, bMsgObj, msqDataClient.i32_rdHandle, msqDataClient.i32_pid);
//clearWriteQueue(bBusNumber, bMsgObj, msqDataClient.i32_wrHandle, msqDataClient.i32_pid);

  return HAL_NO_ERR;
}


/** get last timestamp of CAN receive */
int32_t can_lastReceiveTime()
{
  DEBUG_PRINT("can_lastReceiveTime\n");

  return i32_lastReceiveTime;
}


int16_t getCanMsgBufCount(uint8_t bBusNumber,uint8_t bMsgObj)
{
  msqRead_s s_msqReadBuf;
  // in this direction (server->client), the sendPrio-flag is NEVER set, so we need only to check for FALSE assembled mtypes!
  if (msgrcv(msqDataClient.i32_rdHandle, &s_msqReadBuf, 0, assembleRead_mtype(msqDataClient.i32_pid, bBusNumber, bMsgObj), IPC_NOWAIT) == -1)
    if (errno == E2BIG) {
      //DEBUG_PRINT("messages 1\n");
      return 1;
    }

  return 0;

};


bool waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval )
{
  // for debug only
  //int32_t i32_endWait = getTime() + aui16_timeoutInterval;

  int16_t i16_rc;
  fd_set rfds;
  struct timeval s_timeout;
  static uint8_t ui8_buf[16];

  FD_ZERO(&rfds);
  FD_SET(msqDataClient.i32_pipeHandle, &rfds);

  s_timeout.tv_sec = 0;
  s_timeout.tv_usec = aui16_timeoutInterval * 1000;

  i16_rc = select(msqDataClient.i32_pipeHandle+1, &rfds, NULL, NULL, &s_timeout);

  // return true, when the timeout was NOT the trigger for coming back from select
  const bool cb_result = ( i16_rc > 0 );

  if(i16_rc > 0 && FD_ISSET(msqDataClient.i32_pipeHandle, &rfds) > 0)
  {  // clear pipe (is done also in can server before next write)
    for (i16_rc = read(msqDataClient.i32_pipeHandle, &ui8_buf, 16); i16_rc == 16 ;
         i16_rc = read(msqDataClient.i32_pipeHandle, &ui8_buf, 16) ) {}
  }

  return cb_result;
  /*
  if ( getTime() < i32_endWait ) INTERNAL_DEBUG_DEVICE << "#";
  else INTERNAL_DEBUG_DEVICE << ".";
  static int dbgCnt = 0;
  static int32_t i32_lastDebugLinefeed = 0;
  dbgCnt++;
  if ( ( dbgCnt % 50 ) == 0 )
  {
    INTERNAL_DEBUG_DEVICE << (getTime() - i32_lastDebugLinefeed)/50 << INTERNAL_DEBUG_DEVICE_ENDL;
    i32_lastDebugLinefeed = getTime();
  }
  */
};

// MDS NOTE: This is code is highly inefficient and needs to be optimized!
//  The receive structure should be changed so that it does not need to be copied from
//  one structure to another 3 or 4 times before sending it to the higher levels.
//  This should be done at most once!
//  There are several different CAN msg receive structures defined, and that should be
//  changed to only one through this file, and the one defined externally.
int16_t getCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tReceive * ptReceive )
{
  msqRead_s msqReadBuf;

  //DEBUG_PRINT2("getCanMsg, bus %d, obj %d\n", bBusNumber, bMsgObj);

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > cui8_maxCanObj-1 ) ) return HAL_RANGE_ERR;
#else
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) ) return HAL_RANGE_ERR;
#endif

  const uint8_t cui8_useMsgObj = bMsgObj;

  // we assemble the mtype with FALSE here, as we do NOT have sendPriorities in this direction!
  if (msgrcv(msqDataClient.i32_rdHandle, &msqReadBuf, sizeof(msqRead_s) - sizeof(long), assembleRead_mtype(msqDataClient.i32_pid, bBusNumber, cui8_useMsgObj), IPC_NOWAIT) == -1)
    return HAL_UNKNOWN_ERR;
  i32_lastReceiveTime = getTime();

  // data received
  // get access to data
  can_data* pc_data = &(msqReadBuf.s_canData);
  // copy data
  ptReceive->dwId = pc_data->i32_ident;
#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  ptReceive->bMsgObj = pc_data->bMsgObj;
#endif
  ptReceive->bDlc = pc_data->b_dlc;
  // prevent timestamp which is in the future! (because of 10ms clock jitter)
  ptReceive->tReceiveTime.l1ms = (i32_lastReceiveTime > pc_data->i32_time) ? pc_data->i32_time : i32_lastReceiveTime;
  ptReceive->bXtd = pc_data->b_xtd;
  memcpy(ptReceive->abData, pc_data->pb_data, pc_data->b_dlc);

#if DEBUG_CAN
  printf("Empfang: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptReceive->dwId,
         ptReceive->abData[0], ptReceive->abData[1], ptReceive->abData[2],
         ptReceive->abData[3], ptReceive->abData[4], ptReceive->abData[5],
         ptReceive->abData[6], ptReceive->abData[7]);
#endif

  return HAL_NO_ERR;

};



int16_t sendCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tSend* ptSend )
{
  //DEBUG_PRINT2("sendCanMsg, bus %d, obj %d\n", bBusNumber, bMsgObj);
  msqWrite_s msqWriteBuf;
  transferBuf_s s_transferBuf;

#if DEBUG_CAN
  printf("Senden: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", ptSend->dwId,
         ptSend->abData[0], ptSend->abData[1], ptSend->abData[2],
         ptSend->abData[3], ptSend->abData[4], ptSend->abData[5],
         ptSend->abData[6], ptSend->abData[7]);
#endif

  memset(&msqWriteBuf, 0, sizeof(msqWrite_s));

  msqWriteBuf.i32_mtypePrioAnd1 = assembleWrite_mtype (gsb_sendPrioritized);
  msqWriteBuf.ui16_pid = msqDataClient.i32_pid;
  msqWriteBuf.ui8_bus = bBusNumber;
  msqWriteBuf.ui8_obj = bMsgObj;
  msqWriteBuf.s_canMsg.ui32_id = ptSend->dwId;
  msqWriteBuf.s_canMsg.i32_len = ptSend->bDlc;
  msqWriteBuf.s_canMsg.i32_msgType = ptSend->bXtd;
  memcpy(&(msqWriteBuf.s_canMsg.ui8_data), ptSend->abData, ptSend->bDlc);
  msqWriteBuf.i32_sendTimeStamp = getTime();

  if (msgsnd(msqDataClient.i32_wrHandle, &msqWriteBuf, sizeof(msqWrite_s) - sizeof(long), 0) == -1) {
    perror("msgsnd");
    return HAL_UNKNOWN_ERR;
  }

  // wait for ACK
  if(msgrcv(msqDataClient.i32_cmdAckHandle, &s_transferBuf, sizeof(transferBuf_s) - sizeof(long), msqDataClient.i32_pid, 0) == -1)
    return HAL_UNKNOWN_ERR;

  if (s_transferBuf.ui16_command == COMMAND_ACKNOWLEDGE )
  {
    if (s_transferBuf.s_acknowledge.i32_dataContent == ACKNOWLEDGE_DATA_CONTENT_ERROR_VALUE)
    {
#if DEBUG_CAN
      if ((s_transferBuf.s_acknowledge.i32_data != HAL_NO_ERR) && (s_transferBuf.s_acknowledge.i32_data != HAL_NEW_SEND_DELAY)) // == i32_error
      {
        printf("acknowledge-error received in sendCanMsg\n");
      }
#endif
      return s_transferBuf.s_acknowledge.i32_data;
    }
  }
  // other unexpected answer - shouldn't occur!
  return HAL_UNKNOWN_ERR;
};

/** @todo ON REQUEST MaxSendDelay: Change all callers of this function so that they can handle the case of returnVal<0 to interprete
 *        this as error code. THEN change this function to use negative values as error codes
 */
int32_t getMaxSendDelay(uint8_t aui8_busNr)
{
  transferBuf_s s_transferBuf;

  DEBUG_PRINT1("getMaxSendDelay called, bus %d\n", aui8_busNr);

/*  if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) return HAL_RANGE_ERR; */

  s_transferBuf.i32_mtypePid = msqDataClient.i32_pid;
  s_transferBuf.ui16_command = COMMAND_SEND_DELAY;
  s_transferBuf.s_config.ui8_bus = aui8_busNr;
  // the other fields of the s_config struct are NOT of interest here!

  int i16_rc = send_command(&s_transferBuf, &msqDataClient);
  if (i16_rc == HAL_NO_ERR)
  { // we got an answer - see if it was ACKNOWLEDGE_DATA_CONTENT_SEND_DELAY
    if (s_transferBuf.s_acknowledge.i32_dataContent == ACKNOWLEDGE_DATA_CONTENT_SEND_DELAY)
    { // yihaa, we got what we wanted!
      return s_transferBuf.s_acknowledge.i32_data;
    }
  }

  perror("msgsnd");
  return 0; // we don't have no error code, we can just return some error-maxDelay instead
}

}; // end namespace __HAL
