#ifndef _PC_HAL_CAN_CAN_MSG_H_
#define _PC_HAL_CAN_CAN_MSG_H_

#ifdef DEBUG_CAN
#define DEBUG_PRINT(str) printf(str); fflush(0)
#define DEBUG_PRINT1(str,a) printf(str,a); fflush(0)
#define DEBUG_PRINT2(str,a,b) printf(str,a,b); fflush(0)
#define DEBUG_PRINT3(str,a,b,c) printf(str,a,b,c); fflush(0)
#define DEBUG_PRINT4(str,a,b,c,d) printf(str,a,b,c,d); fflush(0)
#else
#define DEBUG_PRINT(str)
#define DEBUG_PRINT1(str,a)
#define DEBUG_PRINT2(str,a,b)
#define DEBUG_PRINT3(str,a,b,c)
#define DEBUG_PRINT4(str,a,b,c,d)
#endif

namespace __HAL {

#define MSQ_COMMAND      'm'
#define MSQ_COMMAND_ACK  'n'
#define MSQ_CLIENT_READ  'o'
#define MSQ_CLIENT_WRITE 'p'

#define MSQ_KEY_PATH     "/root"
#define PIPE_PATH        "/tmp/can_server_"

#define COMMAND_ACKNOWLEDGE     1
#define COMMAND_REGISTER        10
#define COMMAND_DEREGISTER      11
#define COMMAND_INIT            20
#define COMMAND_CLOSE           21
#define COMMAND_CHG_GLOBAL_MASK 22
#define COMMAND_CONFIG          30
#define COMMAND_CHG_CONFIG      31
#define COMMAND_LOCK            40
#define COMMAND_UNLOCK          41
#define COMMAND_QUERYLOCK       42
#define COMMAND_CLOSEOBJ        50
#define COMMAND_SEND_DELAY      60


struct can_data {
  int32_t i32_time;
  int32_t i32_ident;
  uint8_t b_dlc;
  uint8_t b_xtd;
  uint8_t pb_data[8];

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  uint8_t bMsgObj;
#endif

};

struct can_recv_data {
  uint32_t b_bus;
  can_data msg;
};


// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );
static const uint8_t cui8_maxCanObj = 15;


// message queues / process id
typedef struct {
  int32_t i32_cmdHandle;
  int32_t i32_cmdAckHandle;
  // client read queue
  int32_t i32_rdHandle;
  // client write queue
  int32_t i32_wrHandle;
  int32_t i32_pid;
  int32_t i32_pipeHandle;
} msqData_s;


#define ACKNOWLEDGE_DATA_CONTENT_ERROR_VALUE 0
#define ACKNOWLEDGE_DATA_CONTENT_PIPE_ID     1
#define ACKNOWLEDGE_DATA_CONTENT_SEND_DELAY  2
#define ACKNOWLEDGE_DATA_CONTENT_QUERY_LOCK  3

#define MTYPE_ANY               0x0
#define MTYPE_WRITE_PRIO_NORMAL 0x1
#define MTYPE_WRITE_PRIO_HIGH   0x3

typedef struct {
  long    i32_mtypePid;
  int16_t i16_command;
  union {
    struct {
      clock_t t_clock;
      int32_t i32_fill1;
      int32_t i32_fill2;
      int32_t i32_fill3;
    } s_startTimeClock;
    struct {
      int32_t i32_dataContent; // set to DATA_CONTENT_xxx
      int32_t i32_data; // depends on dataContent
      int32_t i32_fill2;
      int32_t i32_fill3;
    } s_acknowledge;
    struct {
      // byte 0-3
      uint8_t  ui8_bus;
      uint8_t  ui8_obj;
      uint8_t  ui8_bXtd;
      uint8_t  ui8_bMsgType;
      // byte 4-7
      uint32_t ui32_dwId;
      // byte 8-11
      uint16_t ui16_wNumberMsgs;
      uint16_t ui16_queryLockResult;
      // byte 12-15
      // always ui32_mask even if it is only used with SYSTEM_WITH_ENHANCED_CAN_HAL -> avoid preprocessor
      uint32_t ui32_mask;
    } s_config;
    struct {
      // byte 0-3
      uint8_t  ui8_bus;
      uint8_t  ui8_fill1;
      uint16_t ui16_wGlobMask;
      // byte 4-7
      uint32_t ui32_dwGlobMask;
      // byte 8-11
      uint32_t ui32_dwGlobMaskLastmsg;
      // byte 12-15
      uint16_t ui16_wBitrate;
      uint16_t ui16_fill2;
    } s_init;
  };
} msqCommand_s;


typedef struct {
  int32_t i32_mtypePidBusObj;
  struct can_data s_canData;
} msqRead_s;

typedef struct {
  int32_t  i32_mtypePrioAnd1; // has now priority and Pid (PID is needed for clearing the queue :-()
  int16_t  ui16_pid;
  uint8_t  ui8_bus;
  uint8_t  ui8_obj;
  tSend    s_sendData;
  int32_t  i32_sendTimeStamp;
} msqWrite_s;


void send_command_ack(int32_t ai32_mtype, msqData_s* p_msqDataServer, int32_t ai32_dataContent, int32_t ai32_data);

int32_t send_command(msqCommand_s* p_msqCommandBuf, msqData_s* p_msqDataClient);

int16_t ca_createMsqs(msqData_s& msqData);

int32_t assembleRead_mtype (uint16_t ui16_pid, uint8_t bus, uint8_t obj);
int32_t assembleWrite_mtype(bool ab_prio);

uint16_t disassembleRead_client_id (int32_t i32_mtype);
uint16_t disassembleWrite_client_id(int32_t i32_mtype);

void clearReadQueue (uint8_t bBusNumber, uint8_t bMsgObj, int32_t i32_msqHandle, uint16_t ui16_pID);
void clearWriteQueue(bool ab_prio, int32_t i32_msqHandle, uint16_t ui16_pID);

} // end namespace

#endif
