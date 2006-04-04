#ifndef _PC_HAL_CAN_CAN_SERVER_H_
#define _PC_HAL_CAN_CAN_SERVER_H_

#include <list>
#include <string>
#include "can_msq.h"
#include <sys/time.h>
#include <time.h>


namespace __HAL {

// client specific data
typedef struct {
  int32_t  i32_clientID;
  clock_t  t_start4Times;
  struct timeval t_start4Timeofday;
  bool     b_canBufferLock[cui32_maxCanBusCnt][cui8_maxCanObj];
  bool     b_canObjConfigured[cui32_maxCanBusCnt][cui8_maxCanObj];
  uint8_t  ui8_bufXtd[cui32_maxCanBusCnt][cui8_maxCanObj];
  uint8_t  ui8_bMsgType[cui32_maxCanBusCnt][cui8_maxCanObj];
  uint32_t ui32_filter[cui32_maxCanBusCnt][cui8_maxCanObj];
  uint16_t ui16_size[cui32_maxCanBusCnt][cui8_maxCanObj];
  uint16_t ui16_globalMask[cui32_maxCanBusCnt];
  uint32_t ui32_globalMask[cui32_maxCanBusCnt];
  uint32_t ui32_lastMask[cui32_maxCanBusCnt];
  int32_t  i32_pipeHandle;
} client_s;



// server specific data
class server_c {
public:
  server_c();
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
};


void usage();
int open_semaphore_set(int sema_proj_id);
int get_semaphore(int sid, int operation);
void dumpCanMsg (uint8_t bBusNumber, uint8_t bMsgObj, tSend* ptSend, FILE* f_handle);
bool readCanDataFile(server_c* pc_serverData, can_recv_data* ps_receiveData);
// iterator reference because releaseClient erases client
void releaseClient(server_c* pc_serverData, std::list<client_s>::iterator& iter_delete);

} // end namespace

#endif
