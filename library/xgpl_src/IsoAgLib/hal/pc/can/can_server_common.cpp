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

// following define is used for "can_server.h" to enable datastructs/functions
// only used for the Server-part of can_server.
#define DEF_USE_SERVER_SPECIFIC_HEADER
#include "can_server.h"

#include "can_server_common.h"
#include <iostream>
#include <sstream>
#ifdef WIN32
#include <conio.h>
#else
#include <unistd.h>
#endif
#include <functional>
#include <algorithm>

void checkOptions(int argc, char *argv[], __HAL::server_c &ar_server)
{
  for (int i = 1, step = 0; i < argc; i += step) {
    for (Option_s const *p_opt = gp_optionsBegin; p_opt != gp_optionsEnd; ++p_opt) {
      step = p_opt->mpf_check(argc, argv, i, ar_server);
      if (step)
        goto NextStep;
    }
    std::cerr << "Error: unknown option. ";
    usage();
    exit(1);
  NextStep:;
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
  char buf[16]; // size of userinput part
  std::ostringstream ostr_accumulator;
  bool b_eof = false;
  do {
    ssize_t len = read(fileno(stdin), buf, sizeof(buf));
    if (len == -1) // error condition
      break;
    b_eof = (0 < len) && ('\n' == buf[len-1]);
    ostr_accumulator << std::string(buf, len - ssize_t(b_eof));
  } while (!b_eof);
  return ostr_accumulator.str();
#endif
}

static void enableLog( __HAL::server_c *p_server )
{
  for (size_t n_bus = 0; n_bus < __HAL::cui32_maxCanBusCnt; ++n_bus) {
    if (0 < p_server->marrui16_busRefCnt[n_bus]) {
      (void)newFileLog( p_server, n_bus );
    }
  }
  p_server->mb_logMode = true;
}

struct ForgetLog_s : public std::unary_function< yasper::ptr< __HAL::LogFile_c >, yasper::ptr< __HAL::LogFile_c > > {
  yasper::ptr< __HAL::LogFile_c > operator()( yasper::ptr< __HAL::LogFile_c > ) {
    return 0;
  }
};

static void disableLog( __HAL::server_c *p_server )
{
  p_server->mb_logMode = false;
  std::transform(
      p_server->mvec_logFile.begin(),
      p_server->mvec_logFile.end(),
      p_server->mvec_logFile.begin(),
      ForgetLog_s() );
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
  __HAL::server_c *pc_serverData = static_cast< __HAL::server_c * >(ap_arg);
  for (;;) {
    std::istringstream istr_inputLine( readInputLine() );
    std::string s_command;
    istr_inputLine >> s_command;
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
    } else if (!s_command.compare( s_help )) {
      b_needHelp = true; // set to wrongCommand to get help shown!
    } else {
      std::cerr << "Don't know command " << s_command << "." << std::endl;
      b_needHelp = true;
    }

    if (b_needHelp) {
      std::cerr << "Interactive usage:" << std::endl <<
        "  " << s_enable << " " << s_monitor << std::endl <<
        "  " << s_disable << " " << s_monitor << std::endl <<
        "  " << s_enable << " " << s_log << " FILENAMEPREFIX" << std::endl <<
        "  " << s_disable << " " << s_log << " FILENAMEPREFIX" << std::endl <<
        "  " << s_on << " ... (see " << s_enable << " ...)" << std::endl <<
        "  " << s_off << " ... (see " << s_disable << " ...)" << std::endl <<
        "  " << s_help << std::endl;
    }
  }
  // shouldn't reach here as thread runs in an endless loop.
  return NULL; // dummy return
}

void usage() {
  std::cout << "Usage: can_server [OPTION...]" << std::endl << std::endl;
  std::cout << "Options:" << std::endl;
  for (Option_s const *p_opt = gp_optionsBegin; p_opt < gp_optionsEnd; ++p_opt) {
    std::cout << p_opt->ms_usage;
  }
}

int checkMonitor(int /*argc*/, char *argv[], int ai_pos, __HAL::server_c &ar_server)
{
  if (!strcmp(argv[ai_pos], "--monitor")) {
    ar_server.mb_monitorMode=true;
    return 1;
  }
  return 0;
}

int checkLog(int argc, char *argv[], int ai_pos, __HAL::server_c &ar_server)
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

int checkHighPrioMinimum(int argc, char *argv[], int ai_pos, __HAL::server_c &ar_server)
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

int checkReducedLoadIsoBusNo(int argc, char *argv[], int ai_pos, __HAL::server_c &ar_server)
{
  if (!strcmp(argv[ai_pos], "--reduced-load-iso-bus-no")) {
    ar_server.mi16_reducedLoadOnIsoBus = atoi(argv[ai_pos+1]);
    if (ai_pos+1>=argc) {
      std::cerr << "error: option needs second parameter" << std::endl;
      exit(1);
    }
    std::cout << "reduced bus load on ISO bus no. " << ar_server.mi16_reducedLoadOnIsoBus << std::endl;
    return 2;
  }
  return 0;
}

int checkInteractive(int /*argc*/, char *argv[], int ai_pos, __HAL::server_c &ar_server)
{
  if (!strcmp(argv[ai_pos], "--interactive")) {
    ar_server.mb_interactive = true;
    return 1;
  }
  return 0;
}

int checkProductive(int /*argc*/, char *argv[], int ai_pos, __HAL::server_c &ar_server)
{
  if (!strcmp(argv[ai_pos], "--productive")) {
    ar_server.mb_interactive = false;
    return 1;
  }
  return 0;
}

int checkHelp(int /*argc*/, char *argv[], int ai_pos, __HAL::server_c &/*ar_server*/)
{
  if (!strcmp(argv[ai_pos], "--help")) {
    usage();
    exit(0);
  }
  return 0;
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
    ap_server->mvec_logFile[an_bus] = p_logFile;
  }
  return b_error;
}
