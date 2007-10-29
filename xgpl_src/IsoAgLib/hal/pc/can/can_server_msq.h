#ifndef _PC_HAL_CAN_CAN_SERVER_MSQ_H_
#define _PC_HAL_CAN_CAN_SERVER_MSQ_H_

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
void dumpCanMsg (uint8_t bBusNumber, uint8_t bMsgObj, canMsg_s* ps_canMsg, FILE* f_handle);
bool readCanDataFile(server_c* pc_serverData, can_recv_data* ps_receiveData);
// iterator reference because releaseClient erases client
void releaseClient(server_c* pc_serverData, STL_NAMESPACE::list<client_c>::iterator& iter_delete);


} // end namespace

/////////////////////////////////////////////////////////////////////////
// Driver Function Declarations

uint32_t initCardApi();
bool     resetCard(void);

bool     openBusOnCard(uint8_t ui8_bus, uint32_t wBitrate, server_c* pc_serverData);
void     closeBusOnCard(uint8_t ui8_bus, server_c* pc_serverData);

int16_t  sendToBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData);
uint32_t readFromBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, server_c* pc_serverData);

bool     isBusOpen(uint8_t ui8_bus);

void addSendTimeStampToList(client_c *ps_client, int32_t i32_sendTimeStamp);

#endif
