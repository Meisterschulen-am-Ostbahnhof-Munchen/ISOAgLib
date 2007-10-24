#ifndef _PC_HAL_CAN_CAN_SERVER_H_
#define _PC_HAL_CAN_CAN_SERVER_H_

#include <list>
#include <string>
#include "can_msq.h"
#include "can_typedef.h"
#ifndef WIN32
 #include <sys/time.h>
#endif

#include <time.h>

#include <cstdlib>	// Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>

#define MSGTYPE_EXTENDED        0x02            /* extended frame */
#define MSGTYPE_STANDARD        0x00            /* standard frame */

using namespace __HAL;

namespace __HAL {

struct CANmsg {
        unsigned        id;
        int             msg_type;
        int             len;
        unsigned char   data[8];
        unsigned long   time;           /* timestamp in msec, at read only */
};
typedef struct CANmsg canmsg;


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

// client specific data
struct client_c 
{
public:
  client_c();

  uint16_t ui16_pID;
  int32_t  i32_msecStartDeltaClientMinusServer;

  //typedef STL_NAMESPACE::vector<tMsgObj> ArrMsgObj;
  //ArrMsgObj arrMsgObj[cui32_maxCanBusCnt];
  STL_NAMESPACE::vector<tMsgObj> arrMsgObj[cui32_maxCanBusCnt];

  bool     b_busUsed[cui32_maxCanBusCnt];

  uint16_t ui16_globalMask[cui32_maxCanBusCnt];
  uint32_t ui32_globalMask[cui32_maxCanBusCnt];
  uint32_t ui32_lastMask[cui32_maxCanBusCnt];
  int32_t  i32_pipeHandle;
  int32_t  i32_sendDelay[cui32_maxCanBusCnt];

  bool     b_initReceived[cui32_maxCanBusCnt];
};



// server specific data
class server_c {
public:
  server_c();
//  : b_logMode(FALSE), b_inputFileMode(FALSE), i32_lastPipeId(0)
//  {
//     memset(f_canOutput, 0, sizeof(f_canOutput));
//      memset(ui16_globalMask, 0, sizeof(ui16_globalMask));
//  }
  msqData_s msqDataServer;
  STL_NAMESPACE::list<client_c> l_clients;
  STL_NAMESPACE::string logFileBase;
  STL_NAMESPACE::string inputFile;
  uint16_t ui16_globalMask[cui32_maxCanBusCnt];
  // logging
  bool     b_logMode;
  FILE*    f_canOutput[cui32_maxCanBusCnt];
  // replay
  bool     b_inputFileMode;
  FILE*    f_canInput;

  bool     arrb_remoteDestinationAddressInUse[0x100];

  int32_t  i32_lastPipeId;
  // if >0 => do not send messages with local destination address on the bus
  int16_t  i16_reducedLoadOnIsoBus;

  int16_t  can_device[cui32_maxCanBusCnt];
  int32_t  i32_sendDelay[cui32_maxCanBusCnt];
  int      i_pendingMsgs[cui32_maxCanBusCnt];

  uint16_t ui16_busRefCnt[cui32_maxCanBusCnt];

  pthread_mutex_t m_protectClientList;

};

extern STL_NAMESPACE::list<int32_t> list_sendTimeStamps;
void updatePendingMsgs(server_c* apc_server, int8_t i8_bus);
int32_t getTime();

void usage();
void dumpCanMsg (uint8_t bBusNumber, uint8_t bMsgObj, tSend* ptSend, FILE* f_handle);
bool readCanDataFile(server_c* pc_serverData, can_recv_data* ps_receiveData);
// iterator reference because releaseClient erases client
void releaseClient(server_c* pc_serverData, STL_NAMESPACE::list<client_c>::iterator& iter_delete);


} // end namespace

/////////////////////////////////////////////////////////////////////////
// Driver Function Declarations

int ca_InitApi_1 ();
int ca_ResetCanCard_1(void);
int ca_InitCanCard_1 (uint32_t channel, int wBitrate, server_c* pc_serverData);
int ca_TransmitCanCard_1(tSend* ptSend, uint8_t ui8_bus, server_c* pc_serverData);
int ca_ReceiveCanCard_1(uint8_t ui8_bus, server_c* pc_serverData, CANmsg *ps_canMsg);
bool ca_GetcanBusIsOpen_1 (int busId);

void addSendTimeStampToList(client_c *ps_client, int32_t i32_sendTimeStamp);

#endif
