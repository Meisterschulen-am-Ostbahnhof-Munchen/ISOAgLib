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

#ifdef DEF_USE_SERVER_SPECIFIC_HEADER

namespace __HAL {

class server_c;

}

void *readUserInput(void *ap_arg);

typedef int OptionChecker_t(int argc, char *argv[], int ai_pos, __HAL::server_c &ar_server);

struct Option_s {
  Option_s(OptionChecker_t *apf_check, char const *as_usage) :
    mpf_check(apf_check), ms_usage(as_usage) {}
  OptionChecker_t *mpf_check;
  char const *ms_usage;
};

extern Option_s const *const gp_optionsBegin;
extern Option_s const *const gp_optionsEnd;
void checkOptions(int argc, char *argv[], __HAL::server_c &ar_server);
void usage();

enum Option_e {
  OPTION_MONITOR,
  OPTION_LOG,
  OPTION_FILE_INPUT,
  OPTION_HIGH_PRIO_MINIMUM,
  OPTION_REDUCED_LOAD_ISO_BUS_NO,
  OPTION_NICE_CAN_READ,
  OPTION_INTERACTIVE,
  OPTION_PRODUCTIVE,
  OPTION_HELP
};

OptionChecker_t checkHelp;
OptionChecker_t checkMonitor;
OptionChecker_t checkLog;
OptionChecker_t checkFileInput;
OptionChecker_t checkHighPrioMinimum;
OptionChecker_t checkReducedLoadIsoBusNo;
OptionChecker_t checkNiceCanRead;
OptionChecker_t checkInteractive;
OptionChecker_t checkProductive;

template < Option_e >
inline Option_s makeOption(){
  return Option_s();
}
template <> inline Option_s makeOption< OPTION_MONITOR >(){
  return Option_s(
      &checkMonitor,
      "  --monitor                  display can traffic in console\n" );
}
template <> inline Option_s makeOption< OPTION_LOG >(){
  return Option_s(
      &checkLog,
      "  --log LOG_FILE_NAME_BASE   log can traffic into <LOG_FILE_NAME_BASE>_<bus_id>\n" );
}
template <> inline Option_s makeOption< OPTION_FILE_INPUT >(){
  return Option_s(
      &checkFileInput,
      "  --file-input LOG_FILE_NAME   read can data from file <LOG_FILE_NAME>\n" );
}
template <> inline Option_s makeOption< OPTION_HIGH_PRIO_MINIMUM >(){
  return Option_s(
      &checkHighPrioMinimum,
      "  --high-prio-minimum NUM_PENDING_WRITES   if 0: start normally without priority-handling (default - used if param not given!).\n"
      "                             if >0: only clients with activated high-priority-send-mode can send messages if\n"
      "                                    can-controller has equal or more than <NUM_PENDING_WRITES> in its queue.\n" );
}
template <> inline Option_s makeOption< OPTION_REDUCED_LOAD_ISO_BUS_NO >(){
  return Option_s(
      &checkReducedLoadIsoBusNo,
      "  --reduced-load-iso-bus-no BUS_NUMBER   avoid unnecessary CAN bus load due to\n"
      "                             messages with local destination addresses\n" );
}
template <> inline Option_s makeOption< OPTION_NICE_CAN_READ >(){
  return Option_s(
      &checkNiceCanRead,
      "  --nice-can-read VALUE      set a nice value for can read thread to reduce slowing down effect on other applications\n"
      "                             due to heavy OS task switches, 0 < VALUE < 20 for reducing the priority\n\n" );
}
template <> inline Option_s makeOption< OPTION_INTERACTIVE >(){
  return Option_s(
      &checkInteractive,
      "  --interactive              set interactive mode (contrarily to --productive)\n" );
}
template <> inline Option_s makeOption< OPTION_PRODUCTIVE >(){
  return Option_s(
      &checkProductive,
      "  --productive               set productive mode (contrarily to --interactive)\n" );
}
template <> inline Option_s makeOption< OPTION_HELP >(){
  return Option_s(
      &checkHelp,
      "  --help                     print this help.\n" );
}

bool newFileLog( __HAL::server_c *p_server, size_t n_bus );

#endif //def DEF_USE_SERVER_SPECIFIC_HEADER
#endif //ndef _CAN_SERVER_COMMON_H_
