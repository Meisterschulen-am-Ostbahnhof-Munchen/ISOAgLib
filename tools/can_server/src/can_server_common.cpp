/***************************************************************************
              can_server_common.cpp - Code common for both can_servers,
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
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

#include "can_server.h"
#include "can_server_common.h"

#include <iostream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <string.h>

#ifdef WIN32
#include <conio.h>
#include <time.h>
#else
#include <unistd.h>
#include <sys/times.h>
#endif

// The implementation of the new "send" feature is not finished,
// so the first parts are just commited #if'd out so they are right
// in place if it will be continued later...
#define ENABLE_NEW_SEND_FEATURE 0


struct PrintSetting_s : public std::unary_function< yasper::ptr< AOption_c >, void >{
  PrintSetting_s( __HAL::server_c &ar_server ) : mr_server(ar_server) {}
  void operator()(yasper::ptr< AOption_c > ap_option) {
    std::cerr << ap_option->getSetting(mr_server);
  }
  __HAL::server_c &mr_server;
};


void printSettings(__HAL::server_c &ar_server)
{
  std::for_each(gp_optionsBegin, gp_optionsEnd, PrintSetting_s(ar_server));
}


void checkAndHandleOptionsAndStartup(int argc, char *argv[], __HAL::server_c &ar_server)
{
  for (int i = 1, step = 0; i < argc; i += step) {
    for (yasper::ptr< AOption_c > const *p_opt = gp_optionsBegin; p_opt != gp_optionsEnd; ++p_opt) {
      step = (*p_opt)->checkAndHandle(argc, argv, i, ar_server);
      if (step)
        goto NextStep;
    }
    std::cerr << "Error: unknown option. ";
    usage();
    exit(1);
  NextStep:;
  }

#ifndef WIN32
  if (ar_server.mb_daemon) {
    // turn off interactive in daemon mode:
    ar_server.mb_interactive = false;

    // daemonize
    if (daemon( 0, 0 ) < 0) {
      std::cerr << "error during daemonization\n" << std::endl;
      exit(1);
    }
  }
#endif

  if (ar_server.mb_interactive) {
    std::cerr << "IsoAgLib CAN-Server" << std::endl;
    std::cerr << "(Run with '--help' to get help)" << std::endl << std::endl;
    printSettings (ar_server);
  }

#if DEBUG_CANSERVER
#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  printf("SYSTEM_WITH_ENHANCED_CAN_HAL is defined !\n");
#else
  printf("SYSTEM_WITH_ENHANCED_CAN_HAL is NOT defined !\n");
#endif
#endif

  // explicitly call getTime to initialize the time to 0.
  (void) __HAL::getTime();

  const uint32_t apiversion = initCardApi();
  if ( apiversion == 0 ) { // failure - nothing found
    DEBUG_PRINT("FAILURE - No CAN card was found with automatic search\n");
    exit(1);
  }

  // do the reset
  if (!resetCard()) {
    DEBUG_PRINT("Reset not ok\n");
    exit(1);
  }
}

static std::string readInputLine()
{
#ifdef WIN32
  static DWORD const millisecondsToWaitForNextKeypress = 100u;
  std::ostringstream ostr_accumulator;
  for(;;) {
    while (!_kbhit())
      Sleep(millisecondsToWaitForNextKeypress);  
    int i_char = _getche();
    if ('\r' == i_char)
      break;
    ostr_accumulator << char(i_char);
  }
  return ostr_accumulator.str();
#else
  char c_buf;
  std::ostringstream ostr_accumulator;
  bool b_eof = false;
  do {
    ssize_t len = read(fileno(stdin), &c_buf, sizeof(c_buf));
    if (len == -1) // error condition
      break;
    b_eof = (0 < len) && ('\n' == c_buf);
    ostr_accumulator << std::string(&c_buf, len - ssize_t(b_eof));
  } while (!b_eof);
  return ostr_accumulator.str();
#endif
}

static void enableLog( __HAL::server_c *p_server )
{
  for (size_t n_bus = 0; n_bus < p_server->nCanBusses(); ++n_bus) {
    if (0 < p_server->canBus(n_bus).mui16_busRefCnt) {
      (void)newFileLog( p_server, n_bus );
    }
  }
  p_server->mb_logMode = true;
}

static void disableLog( __HAL::server_c *p_server )
{
  p_server->mb_logMode = false;

  for (size_t n_canBusses = p_server->nCanBusses(); 0 < n_canBusses; ) {
    --n_canBusses;
    p_server->canBus(n_canBusses).m_logFile = __HAL::LogFile_c::Null_s()();
  }
}

void *readUserInput( void *ap_arg )
{
  static char const s_on[] = "on";
  static char const s_off[] = "off";
  static char const s_enable[] = "enable";
  static char const s_disable[] = "disable";
  static char const s_monitor[] = "monitor";
  static char const s_log[] = "log";
  static char const s_help[] = "help";
#if ENABLE_NEW_SEND_FEATURE
  static char const s_send[] = "send";
#endif
  __HAL::server_c *pc_serverData = static_cast< __HAL::server_c * >(ap_arg);
  for (;;) {
    std::istringstream istr_inputLine( readInputLine() );
    std::string s_rawCommand;
    istr_inputLine >> s_rawCommand;
    size_t size_ignore = ( s_rawCommand.substr(0,2) == "--" ) ? 2 : 0;
    std::string s_command = s_rawCommand.substr( size_ignore );
    bool b_needHelp = false;
    if (!s_command.compare( s_on ) || !s_command.compare( s_enable )) {
      std::string s_toEnable;
      istr_inputLine >> s_toEnable;
      if (!s_toEnable.compare( s_monitor )) {
        std::cerr << "Enabling monitoring." << std::endl;
        pc_serverData->mb_monitorMode = true;
      } else if (!s_toEnable.compare( s_log )) {
        istr_inputLine >> pc_serverData->mstr_logFileBase;
        enableLog( pc_serverData );
      } else {
        std::cerr << "Don't know how to enable " << s_toEnable << "." << std::endl;
        b_needHelp = true;
      }
    } else if (!s_command.compare( s_off ) || !s_command.compare( s_disable )) {
      std::string s_toDisable;
      istr_inputLine >> s_toDisable;
      if (!s_toDisable.compare( s_monitor )) {
        std::cerr << "Disabling monitoring." << std::endl;
        pc_serverData->mb_monitorMode = false;
      } else if (!s_toDisable.compare( s_log )) {
        disableLog( pc_serverData );
      } else {
        std::cerr << "Don't know how to disable " << s_toDisable << "." << std::endl;
        b_needHelp = true;
      }
#if ENABLE_NEW_SEND_FEATURE
    } else if (!s_command.compare( s_send )) {
      std::string s_type;
      istr_inputLine >> s_type;
      bool b_ext = false;
      if (!s_type.compare( "x" ) || !s_type.compare( "ext" ) || !s_type.compare( "extended" )) {
        b_ext = true;
      } else if (!s_type.compare( "s" ) || !s_type.compare( "std" ) || !s_type.compare( "standard" )) {
        b_ext = false; // though already set initially
      } else {
        b_needHelp = true;
      }
      if (!b_needHelp)
      { // parse and send the data
        int i_id=0;
        int i_db [8]={-1,-1,-1,-1,-1,-1,-1,-1};
        istr_inputLine >> std::hex >> i_id >> i_db[0] >> i_db[1] >> i_db[2] >> i_db[3] >> i_db[4] >> i_db[5] >> i_db[6] >> i_db[7];
        int i_len = 0;
        for (int i=0; i < 8; ++i)
        { // count databytes
          if (i_db[i] == -1)
            break; // no databyte at this position
          if ((i_db[i] < 0) || (i_db[i] > 255))
          {
            std::cout << "ERROR: Databyte out of range. Valid range is 0..FF" << std::endl;
            b_needHelp = true;
            break;
          }
          ++i_len;
        }
        if (!b_needHelp)
        { // still everything fine
          std::cout << "Sending ID " << i_id << " with " << i_len << " databytes";
          for (int i=0; i<i_len; ++i) std::cout << " " << i_db[i];
          std::cout << std::endl;
        }
      }
#endif
    } else if (!s_command.compare( s_help )) {
      b_needHelp = true; // set to wrongCommand to get help shown!
    } else {
      std::cerr << "Don't know command " << s_command << ". Use 'help' for help." << std::endl;
    }

    if (b_needHelp) {
      std::cerr << "Interactive usage:" << std::endl <<
        "  " << s_enable << " " << s_monitor << std::endl <<
        "  " << s_disable << " " << s_monitor << std::endl <<
        "  " << s_enable << " " << s_log << " FILENAMEPREFIX" << std::endl <<
        "  " << s_disable << " " << s_log << " FILENAMEPREFIX" << std::endl <<
        "  " << s_on << " ... (see " << s_enable << " ...)" << std::endl <<
        "  " << s_off << " ... (see " << s_disable << " ...)" << std::endl <<
#if ENABLE_NEW_SEND_FEATURE
        "  " << s_send << " s/std/standard/x/ext/extended ID(hex) DB1 DB2 .. DBx" << std::endl <<
#endif
        "  " << s_help << std::endl;
    }
  }
  // shouldn't reach here as thread runs in an endless loop.
  return NULL; // dummy return
}

void usage() {
  std::cout
    << "IsoAgLib CAN-Server - Virtual Host CAN-Bus." << std::endl << std::endl
    << "Usage: can_server [OPTIONS]" << std::endl << std::endl
    << "Note: When running in Interactive mode (default)" << std::endl
    << "      type 'help' for a list of interactive commands)" << std::endl << std::endl
    << "Options:" << std::endl;
  for (yasper::ptr< AOption_c > const *p_opt = gp_optionsBegin; p_opt < gp_optionsEnd; ++p_opt) {
    std::cout << (*p_opt)->getUsage();
  }
}

template <>
int Option_c< OPTION_MONITOR >::doCheckAndHandle(int /*argc*/, char *argv[], int ai_pos, __HAL::server_c &ar_server) const
{
  if (!strcmp(argv[ai_pos], "--monitor")) {
    ar_server.mb_monitorMode=true;
    return 1;
  }
  return 0;  
}

template <>
std::string Option_c< OPTION_MONITOR >::doGetUsage() const
{
  return "  --monitor                  Display can traffic in console\n";
}

template <>
std::string Option_c< OPTION_MONITOR >::doGetSetting(__HAL::server_c &ar_server) const
{
  return ar_server.mb_monitorMode ? "Monitoring.\n" : "";
}

template <>
int Option_c< OPTION_LOG >::doCheckAndHandle(int argc, char *argv[], int ai_pos, __HAL::server_c &ar_server) const
{
  if (!strcmp(argv[ai_pos], "--log")) {
    if (ai_pos+1>=argc) {
      std::cerr << "error: option needs second parameter" << std::endl;
      exit(1);
    }
    ar_server.mstr_logFileBase = argv[ai_pos+1];
    ar_server.mb_logMode=true;
    return 2;
  }
  return 0;
}

template <>
std::string Option_c< OPTION_LOG >::doGetSetting(__HAL::server_c &ar_server) const
{
  std::ostringstream ostr_setting;
  if (ar_server.mb_logMode) {
    ostr_setting << "Logging to file(s), filename prefix set to " <<
      ar_server.mstr_logFileBase << std::endl;
  }
  return ostr_setting.str();
}

template <>
std::string Option_c< OPTION_LOG >::doGetUsage() const
{
  return "  --log LOG_FILE_NAME_BASE   Log can traffic into <LOG_FILE_NAME_BASE>_<bus_id>\n";
}

template <>
int Option_c< OPTION_HIGH_PRIO_MINIMUM >::doCheckAndHandle(int argc, char *argv[], int ai_pos, __HAL::server_c &ar_server) const
{
 if (!strcmp(argv[ai_pos], "--high-prio-minimum")) {
   if (ai_pos+1>=argc) {
     std::cerr << "error: option needs second parameter" << std::endl;
     exit(1);
   }
   ar_server.mi_highPrioModeIfMin = atoi(argv[ai_pos+1]); // 0 for no prio mode!
   return 2;
 }
 return 0;
}

template <>
std::string Option_c< OPTION_HIGH_PRIO_MINIMUM >::doGetSetting(__HAL::server_c &ar_server) const
{
  std::ostringstream ostr_setting;
  if (ar_server.mi_highPrioModeIfMin > 0) {
    ostr_setting << "High priority handling on, pending writes threshold set to " <<
      ar_server.mi_highPrioModeIfMin << std::endl;
  }
  return ostr_setting.str();
}

template <>
std::string Option_c< OPTION_HIGH_PRIO_MINIMUM >::doGetUsage() const
{
  return
    "  --high-prio-minimum NUM_PENDING_WRITES\n"
    "                             If =0: start normally without priority-handling\n"
    "                                    (default - used if parameter not given!)\n"
    "                             If >0: Only clients with activated high-priority-\n"
    "                                    send-mode can send messages if\n"
    "                                    can-controller has equal or more than\n"
    "                                    <NUM_PENDING_WRITES> in its queue.\n";
}

template <>
int Option_c< OPTION_REDUCED_LOAD_ISO_BUS_NO >::doCheckAndHandle(int argc, char *argv[], int ai_pos, __HAL::server_c &ar_server) const
{
  if (!strcmp(argv[ai_pos], "--reduced-load-iso-bus-no")) {
    ar_server.mi16_reducedLoadOnIsoBus = atoi(argv[ai_pos+1]);
    if (ai_pos+1>=argc) {
      std::cerr << "error: option needs second parameter" << std::endl;
      exit(1);
    }
    return 2;
  }
  return 0;
}

template <>
std::string Option_c< OPTION_REDUCED_LOAD_ISO_BUS_NO >::doGetSetting(__HAL::server_c &ar_server) const
{
  std::ostringstream ostr_setting;
  if (ar_server.mi16_reducedLoadOnIsoBus >= 0) {
    ostr_setting << "Reducing load on Bus no. " <<
      ar_server.mi16_reducedLoadOnIsoBus << std::endl;
  }
  return ostr_setting.str();
}

template <>
std::string Option_c< OPTION_REDUCED_LOAD_ISO_BUS_NO >::doGetUsage() const
{
  return
    "  --reduced-load-iso-bus-no BUS_NUMBER\n"
    "                             Don't send internal traffic to the physical\n"
    "                             CAN-Bus and thus reduce the load on the specified\n"
    "                             (ISOBUS) bus number. This is detected by source\n"
    "                             and destination addresses in the identifier\n";
}

template <>
int Option_c< OPTION_INTERACTIVE >::doCheckAndHandle(int /*argc*/, char *argv[], int ai_pos, __HAL::server_c &ar_server) const
{
  if (!strcmp(argv[ai_pos], "--interactive")) {
    ar_server.mb_interactive = true;
    return 1;
  }
  return 0;
}

template <>
std::string Option_c< OPTION_INTERACTIVE >::doGetSetting(__HAL::server_c &ar_server) const
{
  return ar_server.mb_interactive ? "Interactive mode set\n" : "";
}

template <>
std::string Option_c< OPTION_INTERACTIVE >::doGetUsage() const
{
  return
    "  --interactive              Set interactive mode (contrarily to --productive)\n"
    "                             explicitly. (This is the default!)\n";
}

template <>
int Option_c< OPTION_PRODUCTIVE >::doCheckAndHandle(int /*argc*/, char *argv[], int ai_pos, __HAL::server_c &ar_server) const
{
  if (!strcmp(argv[ai_pos], "--productive")) {
    ar_server.mb_interactive = false;
    return 1;
  }
  return 0;
}

template <>
std::string Option_c< OPTION_PRODUCTIVE >::doGetSetting(__HAL::server_c &ar_server) const
{
  return ar_server.mb_interactive ? "" : "Productive mode set\n";
}

template <>
std::string Option_c< OPTION_PRODUCTIVE >::doGetUsage() const
{
  return "  --productive               Set productive mode (contrarily to --interactive)\n";
}

#ifndef WIN32
/*  WIN32 Platforms can't handle the daemonize syscall. The service aequivalent is not supported by can_server */
template <>
int Option_c< OPTION_DAEMON >::doCheckAndHandle(int /*argc*/, char *argv[], int ai_pos, __HAL::server_c &ar_server) const
{
  if (!strcmp(argv[ai_pos], "--daemon")) {
    ar_server.mb_daemon = true;
    return 1;
  }
  return 0;
}

template <>
std::string Option_c< OPTION_DAEMON>::doGetUsage() const
{
  return "  --daemon                   Daemonize after startup.\n";
}

template <>
std::string Option_c< OPTION_DAEMON>::doGetSetting(__HAL::server_c &ar_server) const
{
  return ar_server.mb_daemon? "Daemonized.\n" : "";
}
#endif

template <>
int Option_c< OPTION_HELP >::doCheckAndHandle(int /*argc*/, char *argv[], int ai_pos, __HAL::server_c &/*ar_server*/) const
{
  if (!strcmp(argv[ai_pos], "--help")) {
    usage();
    exit(0);
  }
  return 0;
}

template <>
std::string Option_c< OPTION_HELP >::doGetSetting(__HAL::server_c &/*ar_server*/) const
{
  return "";
}

template <>
std::string Option_c< OPTION_HELP >::doGetUsage() const
{
  return "  --help                     Print this help.\n";
}

/** Start a new file log.
 *  \return error.
 *  \retval true if an error occured.
 *  \retval false if successfull.
 */
bool newFileLog(
    __HAL::server_c *ap_server, /// server data
    size_t an_bus ) /// bus number
{
  std::ostringstream ostr_filename;
  ostr_filename << ap_server->mstr_logFileBase << "_" << std::hex << an_bus;
  yasper::ptr< __HAL::LogFile_c > p_logFile = new __HAL::LogFile_c( ostr_filename.str() );
  bool b_error = !p_logFile->getRaw();
  if (b_error) {
    if (ap_server->mb_interactive) {
      std::cerr << "Error: can not open log file " << ostr_filename.str() << "." << std::endl;
    }
  } else {
    if (ap_server->mb_interactive) {
      std::cerr << "New log file " << ostr_filename.str() << "." << std::endl;
    }
    ap_server->canBus(an_bus).m_logFile = p_logFile;
  }
  return b_error;
}

/** Close a file log.
 */
void closeFileLog(
    __HAL::server_c *ap_server, /// server data
    size_t an_bus ) /// bus number
{
  ap_server->canBus(an_bus).m_logFile = __HAL::LogFile_c::Null_s()();
}


void dumpCanMsg (uint8_t bBusNumber, uint8_t bMsgObj, canMsg_s* ps_canMsg, FILE *f_handle)
{
    fprintf(f_handle, "%10d %-2d %-2d %-2d %-2d %-2d %-8x  ",
            __HAL::getTime(), bBusNumber, bMsgObj, ps_canMsg->i32_msgType, ps_canMsg->i32_len,
            (ps_canMsg->ui32_id >> 26) & 7 /* priority */, ps_canMsg->ui32_id);
    for (uint8_t ui8_i = 0; (ui8_i < ps_canMsg->i32_len) && (ui8_i < 8); ui8_i++)
      fprintf(f_handle, " %-3hx", ps_canMsg->ui8_data[ui8_i]);
    fprintf(f_handle, "\n");
    fflush(f_handle);
}
