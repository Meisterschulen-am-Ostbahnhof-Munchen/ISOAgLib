/***************************************************************************
              can_server_common.h - Code common for both can_servers,
                                    socket and msg-queue.
                             -------------------
    begin                : Apr 12 2009
    copyright            : (C) 2009 Thomas Hafner
    email                : T.Hafner@osb-ag.de
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
 * Copyright (C) 2009 Thomas Hafner                                        *
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
 * the main author Achim Spangler by a.spangler@osb-ag.de                  *
 ***************************************************************************/
#ifndef _CAN_SERVER_COMMON_H_
#define _CAN_SERVER_COMMON_H_

#include <pthread.h>
#include <../../tools/libs/misc/yasper.h>

namespace __HAL {

class server_c;

}

void *readUserInput(void *ap_arg);

void checkAndHandleOptions(int argc, char *argv[], __HAL::server_c &ar_server);
void printSettings(__HAL::server_c &ar_server);
void usage();

bool newFileLog( __HAL::server_c *p_server, size_t n_bus );
void closeFileLog(__HAL::server_c *ap_server, size_t an_bus );
void dumpCanMsg (uint8_t bBusNumber, uint8_t bMsgObj, canMsg_s* ps_canMsg, FILE *f_handle);

namespace __HAL {

class LogFile_c {
public:
  LogFile_c ( std::string const &arstr_filename )
    : mp_file( std::fopen( arstr_filename.c_str(), "a+" ) ) {}
  ~LogFile_c() {
    if (mp_file)
      std::fclose( mp_file );
  }
  std::FILE *getRaw() {
    return mp_file;
  }
  struct Null_s {
    yasper::ptr< LogFile_c > operator()() {
      return 0;
    }
  };
private:
  std::FILE *mp_file;
  // intentionally not implemented (prevent use):
  LogFile_c( LogFile_c const & );
  LogFile_c &operator= ( LogFile_c const & );
};

// server specific data
class server_c {
public:
  server_c();

#ifdef CAN_DRIVER_MESSAGE_QUEUE
  msqData_s ms_msqDataServer;
#endif

  std::list<client_c> mlist_clients;
  std::string mstr_logFileBase;
  std::string mstr_inputFile;
  uint16_t marrui16_globalMask[cui32_maxCanBusCnt];
  // logging
  bool     mb_logMode;
  std::vector< yasper::ptr< LogFile_c > > mvec_logFile;
  // monitor
  bool     mb_monitorMode;
  // replay
  bool     mb_inputFileMode;
  FILE*    mf_canInput;

  bool     marrb_remoteDestinationAddressInUse[0x100];

#ifdef CAN_DRIVER_MESSAGE_QUEUE
  int32_t  mi32_lastPipeId;
  int32_t  marri32_fileDescrWakeUpPipeForNewBusEvent[2];
#endif

  // if >0 => do not send messages with local destination address on the bus
  int16_t  mi16_reducedLoadOnIsoBus;

  int32_t  marri32_can_device[cui32_maxCanBusCnt];
  int32_t  marri32_sendDelay[cui32_maxCanBusCnt];
  int      marri_pendingMsgs[cui32_maxCanBusCnt];
  bool     marrb_deviceConnected[cui32_maxCanBusCnt];

  uint16_t marrui16_busRefCnt[cui32_maxCanBusCnt];

  pthread_mutex_t mt_protectClientList;
  bool     mb_interactive;
  int      mi_canReadNiceValue;
  int      mi_highPrioModeIfMin;
};

extern std::list<int32_t> list_sendTimeStamps;
void updatePendingMsgs(server_c* rpc_server, int8_t i8_bus);
int32_t getTime();

} //namespace __HAL

class AOption_c {
public:
  int checkAndHandle(int argc, char *argv[], int ai_pos, __HAL::server_c &ar_server) const {
    return doCheckAndHandle(argc, argv, ai_pos, ar_server);
  }
  std::string getSetting(__HAL::server_c &ar_server) const { return doGetSetting(ar_server); }
  std::string getUsage() const { return doGetUsage(); }
  virtual ~AOption_c() {}
private:
  virtual int doCheckAndHandle(int argc, char *argv[], int ai_pos, __HAL::server_c &ar_server) const = 0;
  virtual std::string doGetSetting(__HAL::server_c &ar_server) const = 0;
  virtual std::string doGetUsage() const = 0;
};

/* The following identifiers OPTION_... serve as template parameter
 * values for Option_c (which is defined below). */
enum OPTION_MONITOR {};
enum OPTION_LOG {};
enum OPTION_FILE_INPUT {};
enum OPTION_HIGH_PRIO_MINIMUM {};
enum OPTION_REDUCED_LOAD_ISO_BUS_NO {};
enum OPTION_NICE_CAN_READ {};
enum OPTION_INTERACTIVE {};
enum OPTION_PRODUCTIVE {};
enum OPTION_HELP {};

template < typename OPTION >
class Option_c : public AOption_c {
public:
  static yasper::ptr< AOption_c > create() {
    return yasper::ptr< AOption_c >( new Option_c );
  }
private:
  virtual int doCheckAndHandle(int argc, char *argv[], int ai_pos, __HAL::server_c &ar_server) const;
  virtual std::string doGetSetting(__HAL::server_c &ar_server) const;
  virtual std::string doGetUsage() const;
};

extern yasper::ptr< AOption_c > const *const gp_optionsBegin;
extern yasper::ptr< AOption_c > const *const gp_optionsEnd;

/////////////////////////////////////////////////////////////////////////
// Driver Function Declarations

uint32_t initCardApi();
bool     resetCard(void);

bool     openBusOnCard(uint8_t ui8_bus, uint32_t wBitrate, __HAL::server_c* pc_serverData);
void     closeBusOnCard(uint8_t ui8_bus, __HAL::server_c* pc_serverData);

int16_t  sendToBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, __HAL::server_c* pc_serverData);
bool     readFromBus(uint8_t ui8_bus, canMsg_s* ps_canMsg, __HAL::server_c* pc_serverData);

bool     isBusOpen(uint8_t ui8_bus);

void addSendTimeStampToList(__HAL::client_c *ps_client, int32_t i32_sendTimeStamp);

#endif //ndef _CAN_SERVER_COMMON_H_
