#ifndef _PC_HAL_CAN_CAN_SERVER_H_
#define _PC_HAL_CAN_CAN_SERVER_H_

// USE_UNIX_SOCKET to use /tmp/can_server.sock.<command_port> and /tmp/can_server.sock.<data_port> instead of real sockets 
// => no loopback network configuration necessary
//#define USE_UNIX_SOCKET

#define DEBUG_CAN

#include <list>
#include <string>
#include <cstdlib>  // Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>

#include <time.h>
#include <pthread.h> 
#include <sys/types.h>

#ifndef WIN32
  #include <sys/time.h>
  #include <sys/socket.h>
  #include <unistd.h>
#else 
  #include <Windows.h>
#endif

#ifdef USE_UNIX_SOCKET
  #define SOCKET_TYPE_INET_OR_UNIX AF_UNIX
  #define SOCKET_PATH "/tmp/can_server.sock"
#else
  #define SOCKET_TYPE_INET_OR_UNIX AF_INET
#endif

#include "can_target_extensions.h"

#define MSGTYPE_EXTENDED        0x02            /* extended frame */
#define MSGTYPE_STANDARD        0x00            /* standard frame */


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
#define COMMAND_DATA            70

#define COMMAND_TRANSFER_PORT 36798
#define DATA_TRANSFER_PORT    36799

#ifdef WIN32
  typedef SOCKET SOCKET_TYPE;
#else
  typedef int    SOCKET_TYPE;
#endif


using namespace __HAL;

namespace __HAL {

// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );
static const uint8_t cui8_maxCanObj = 15;


typedef struct {
  bool     b_canBufferLock;
  bool     b_canObjConfigured;
  uint8_t  ui8_bufXtd;
  uint8_t  ui8_bMsgType;
  uint32_t ui32_filter;
  uint32_t ui32_mask_xtd;
  uint16_t ui16_mask_std;
  uint16_t ui16_size;

} tMsgObj;



#define ACKNOWLEDGE_DATA_CONTENT_ERROR_VALUE 0
#define ACKNOWLEDGE_DATA_CONTENT_PIPE_ID     1
#define ACKNOWLEDGE_DATA_CONTENT_SEND_DELAY  2
#define ACKNOWLEDGE_DATA_CONTENT_QUERY_LOCK  3

#define MTYPE_ANY               0x0
#define MTYPE_WRITE_PRIO_NORMAL 0x1
#define MTYPE_WRITE_PRIO_HIGH   0x3

typedef struct {
  uint16_t ui16_command;
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
    struct {
      uint32_t dwId;                          /** Identifier */
      int32_t  i32_sendTimeStamp;
      uint8_t  ui8_bus;
      uint8_t  ui8_obj;
      uint8_t  bXtd;                           /** Laenge Bit Identifier */
      uint8_t  bDlc;                           /** Anzahl der empfangenen Daten */
      uint8_t  abData[8];                      /** Datenpuffer */
    } s_data;
  };
} socketBuf_s;

// client specific data
struct client_c 
{
public:
  client_c();

  SOCKET_TYPE  i32_commandSocket;
  SOCKET_TYPE  i32_dataSocket;

  uint16_t ui16_pid;
  int32_t  i32_msecStartDeltaClientMinusServer;

  //typedef STL_NAMESPACE::vector<tMsgObj> ArrMsgObj;
  //ArrMsgObj arrMsgObj[cui32_maxCanBusCnt];
  std::vector<tMsgObj> arrMsgObj[cui32_maxCanBusCnt];

  bool     b_busUsed[cui32_maxCanBusCnt];

  uint16_t ui16_globalMask[cui32_maxCanBusCnt];
  uint32_t ui32_globalMask[cui32_maxCanBusCnt];
  uint32_t ui32_lastMask[cui32_maxCanBusCnt];
  int32_t  i32_sendDelay[cui32_maxCanBusCnt];

  bool     b_initReceived[cui32_maxCanBusCnt];
};



// server specific data
class server_c {
public:
  server_c();
  std::list<client_c> l_clients;
  std::string logFileBase;
  std::string inputFile;
  uint16_t ui16_globalMask[cui32_maxCanBusCnt];
  // logging
  bool     b_logMode;
  FILE*    f_canOutput[cui32_maxCanBusCnt];
  // replay
  bool     b_inputFileMode;
  FILE*    f_canInput;

  bool     arrb_remoteDestinationAddressInUse[0x100];

  // if >0 => do not send messages with local destination address on the bus
  int16_t  i16_reducedLoadOnIsoBus;

  int16_t  can_device[cui32_maxCanBusCnt];
  int32_t  i32_sendDelay[cui32_maxCanBusCnt];
  int      i_pendingMsgs[cui32_maxCanBusCnt];

  uint16_t ui16_busRefCnt[cui32_maxCanBusCnt];

  pthread_mutex_t m_protectClientList;

};


extern std::list<int32_t> list_sendTimeStamps;
void updatePendingMsgs(server_c* rpc_server, int8_t i8_bus);
int32_t getTime();

} // end namespace

/////////////////////////////////////////////////////////////////////////
// Driver Function Declarations

uint32_t initCardApi();
bool     resetCard(void);

bool     openBusOnCard(uint8_t ui8_bus, uint32_t wBitrate, server_c* pc_serverData);
void     closeBusOnCard(uint8_t ui8_bus, server_c* pc_serverData);

bool     sendToBus(uint8_t ui8_bus, socketBuf_s* p_sockBuf, server_c* pc_serverData);
uint32_t readFromBus(uint8_t ui8_bus, socketBuf_s* p_sockBuf, server_c* pc_serverData);

bool     isBusOpen(uint8_t ui8_bus);

void addSendTimeStampToList(client_c *ps_client, int32_t i32_sendTimeStamp);

#endif
