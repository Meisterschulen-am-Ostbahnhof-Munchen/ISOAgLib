#ifndef _PC_HAL_CAN_CAN_SERVER_H_
#define _PC_HAL_CAN_CAN_SERVER_H_

#include <list>
#include <string>
#include "can_msq.h"
#include "can_typedef.h"
#include <sys/time.h>
#include <time.h>

#include <vector>

using namespace __HAL;

namespace __HAL {

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
typedef struct {
  int32_t  i32_clientID;
  int32_t  i32_msecStartDeltaClientMinusServer;

  //typedef STL_NAMESPACE::vector<tMsgObj> ArrMsgObj;
  //ArrMsgObj arrMsgObj[cui32_maxCanBusCnt];
  std::vector<tMsgObj> arrMsgObj[cui32_maxCanBusCnt];
  
  uint16_t ui16_globalMask[cui32_maxCanBusCnt];
  uint32_t ui32_globalMask[cui32_maxCanBusCnt];
  uint32_t ui32_lastMask[cui32_maxCanBusCnt];
  int32_t  i32_pipeHandle;
  int32_t  i32_sendDelay[cui32_maxCanBusCnt];
} client_s;



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
  std::list<client_s> l_clients;
  std::string logFileBase;
  std::string inputFile;
  uint16_t ui16_globalMask[cui32_maxCanBusCnt];
  // logging
  bool     b_logMode;
  FILE*    f_canOutput[cui32_maxCanBusCnt];
  // replay
  bool     b_inputFileMode;
  FILE*    f_canInput;

  int32_t  i32_lastPipeId;
  int16_t  can_device[cui32_maxCanBusCnt];
  int32_t  i32_sendDelay[cui32_maxCanBusCnt];
};

extern std::list<int32_t> list_sendTimeStamps;
int32_t getTime();

void usage();
int open_semaphore_set(int sema_proj_id);
int get_semaphore(int sid, int operation);
void dumpCanMsg (uint8_t bBusNumber, uint8_t bMsgObj, tSend* ptSend, FILE* f_handle);
bool readCanDataFile(server_c* pc_serverData, can_recv_data* ps_receiveData);
// iterator reference because releaseClient erases client
void releaseClient(server_c* pc_serverData, std::list<client_s>::iterator& iter_delete);


} // end namespace

/////////////////////////////////////////////////////////////////////////
// Driver Function Declarations

int ca_InitApi_1 ();
int ca_ResetCanCard_1(void);
int ca_InitCanCard_1 (uint32_t channel, int wBitrate, server_c* pc_serverData);
int ca_TransmitCanCard_1(tSend* ptSend, uint8_t ui8_bus, server_c* pc_serverData);
int ca_ReceiveCanCard_1(can_recv_data* receiveData, uint8_t ui8_bus, server_c* pc_serverData);
bool ca_GetcanBusIsOpen_1 (int busId);

#endif
