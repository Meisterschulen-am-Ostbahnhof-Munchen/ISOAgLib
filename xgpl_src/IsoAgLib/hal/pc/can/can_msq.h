#ifndef _PC_HAL_TARGET_EXTENSIONS_CAN_SERVER_H_
#define _PC_HAL_TARGET_EXTENSIONS_CAN_SERVER_H_

namespace __HAL {

#ifdef DEBUG
#define DEBUG_PRINT(str) printf(str); fflush(0)
#define DEBUG_PRINT1(str,a) printf(str,a); fflush(0)
#define DEBUG_PRINT2(str,a,b) printf(str,a,b); fflush(0)
#define DEBUG_PRINT3(str,a,b,c) printf(str,a,b,c); fflush(0)
#define DEBUG_PRINT4(str,a,b,c,d) printf(str,a,b,c,d); fflush(0)
#else
#define DEBUG_PRINT
#define DEBUG_PRINT1
#define DEBUG_PRINT2
#define DEBUG_PRINT3
#define DEBUG_PRINT4
#endif

#define MSQ_COMMAND      'm'
#define MSQ_COMMAND_ACK  'n'
#define MSQ_CLIENT_READ  'o'
#define MSQ_CLIENT_WRITE 'p'

#define SERVER_SEMAPHORE 'u'

#define MSQ_KEY_PATH     "/root"

#define COMMAND_ACK        1
#define COMMAND_NACK       2
#define COMMAND_REGISTER   10
#define COMMAND_DEREGISTER 11
#define COMMAND_INIT       20
#define COMMAND_CLOSE      21
#define COMMAND_CONFIG     30
#define COMMAND_CHG_CONFIG 31
#define COMMAND_LOCK       40
#define COMMAND_UNLOCK     41
#define COMMAND_QUERYLOCK  42
#define COMMAND_CLOSEOBJ   50


// message queues / process id 
typedef struct {
  int32_t i32_cmdHandle;
  int32_t i32_cmdAckHandle;
  // client read queue 
  int32_t i32_rdHandle;
  // client write queue
  int32_t i32_wrHandle;
  int32_t i32_pid;
} msqData_s;


typedef struct {
  int32_t i32_mtype;
  int16_t i16_command;
  union {
    struct {
      int32_t i32_runTime_msec;
      int32_t i32_fill1;
      int32_t i32_fill2;
      int32_t i32_fill3;
    } s_runtime;
    struct {
      int32_t i32_error;
      int32_t i32_fill1;
      int32_t i32_fill2;
      int32_t i32_fill3;
    } s_error;
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
      int32_t  i32_fill2;
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

struct can_data {
  int32_t i32_time;
  int32_t i32_ident;
  uint8_t b_dlc;
  uint8_t b_xtd;
  uint8_t pb_data[8];
};

struct can_recv_data {
  uint32_t b_bus;
  can_data msg;
};


typedef struct {
  int32_t i32_mtype;
  struct can_data s_canData;
} msqRead_s;

typedef struct {
  int32_t  i32_mtype;
  uint8_t  ui8_bus;
  uint8_t  ui8_obj;
  uint16_t ui16_fill;
  tSend    s_sendData;
} msqWrite_s;


// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );
static const uint8_t cui8_maxCanObj = 15;

void send_command_ack(int32_t i32_mtype, int32_t i32_error, msqData_s* p_msqDataServer);

int32_t send_command(msqCommand_s* p_msqCommandBuf, msqData_s* p_msqDataClient);

int16_t ca_createMsqs(msqData_s& msqData);

uint32_t assemble_mtype(int32_t i32_pid, uint8_t bus, uint8_t obj);

void clearReadQueue(uint8_t bBusNumber, uint8_t bMsgObj, int32_t i32_msqHandle, int32_t i32_pid);

void clearWriteQueue(uint8_t bBusNumber, uint8_t bMsgObj, int32_t i32_msqHandle, int32_t i32_pid);

} // end namespace

#endif
