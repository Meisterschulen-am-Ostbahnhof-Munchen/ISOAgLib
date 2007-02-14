/* *************************************************************************
        tractorconsole.cpp - example for tractor ECU (work-in-progress)
                             -------------------
    begin                : Sun Dec 19 20:00:00 CEST 2004

    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
 ***************************************************************************/

/* *************************************************************************
 * This example main application is published NOT as GPL`ed Open Source,   *
 * so that you can include this source file in propietary closed projects. *
 * Everybody is encouraged to use the examples in the IsoAgLib_Examples    *
 * directory for a quick and easy start of development for IsoAgLib        *
 * applications.                                                           *
 *                                                                         *
 * So nobody is bound to publish changes of the example source files.      *
 * But every changed example, which could help learning the use of the     *
 * GPLed "IsoAgLib", is invited to be sent to the original author, so      *
 * that it can be published with the other examples.                       *
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 * This is an example of "IsoAgLib", an object oriented program library    *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The "IsoAgLib" is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU Lesser General Public License as   *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * The "IsoAgLib" is distributed in the hope that it will be useful, but   *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111_1307  USA           *
 ***************************************************************************/

/* *********************************************************************** */
/** \example tractorconsole.cpp
 * This example application of ISO<i><sub>AgLib</sub></i>is a work-in-progress
 * of a tractor ECU. It receives the tractor data from internal CAN, and
 * provides them as ISO 11783 tractor data on external BUS.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Trigger periodic activities of ISO<i><sub>AgLib</sub></i>
 *  <ul>
 *  <li>Core class IsoAgLib::iScheduler_c for scheduling of all periodic activities
 *  <li>Method IsoAgLib::iScheduler_c::timeEvent() which can<ul>
 *    <li>Perform activities until defined rl_endTime is reached, which is important
 *      for scheduling purposes of whole system - call by IsoAgLib::iScheduler_c::timeEvent( rl_endTime )
 *    <li>Process all received CAN messages until all receive buffers are empty
 *      -> simple call, but can lead to deadlock on to high CAN load
 *    </ul>
 *  </ul>
 * <li>Create local identity, for which the ISO<i><sub>AgLib</sub></i> performs an address claim,
 *    so that the ECU can access the <b><i>ISO11783</i></b> BUS with IsoAgLib::iIdentItem_c
 *    and its constructor IsoAgLib::iIdentItem_c::iIdentItem_c
 * </ol>
 * <H1>Where to look for further information</H1>
 * <ol>
 * <li>Overview information on Scheduling at \ref SchedulerPage
 * <li>Overview on System Management at \ref SystemMgmtPage
 * <li>Overview on global communication functions at \ref CommOverPage
 * </ol>
 *
 * <H1>Howto compile this example</H1>
 * <ol>
 * <li>Call the script update_makefile.sh with the spec file
 *     conf_tractor from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_tractor</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd tractor</i> to go to the subdirectory tractor
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable tractor in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-tractor.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_tractor</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpectractor__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpectractor__pc_linux__rte__rte
 *    <li> \ref PrjSpectractor__pc_linux__rte__simulating
 *    <li> \ref PrjSpectractor__pc_linux__rte__sys
 *    <li> \ref PrjSpectractor__pc_linux__simulating__rte
 *    <li> \ref PrjSpectractor__pc_linux__simulating__simulating
 *    <li> \ref PrjSpectractor__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpectractor__pc_win32__vector_canlib__simulating
 *    <li> \ref PrjSpectractor__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref PrjSpectractor__pc_win32__vector_canlib__sys
 *    <li> \ref PrjSpectractor__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref PrjSpectractor__pc_win32__simulating__simulating
 *    <li> \ref PrjSpectractor__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpectractor__esx__sys__sys
 *    <li> \ref PrjSpectractor__imi__sys__sys
 *    <li> \ref PrjSpectractor__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileListstractor__pc_linux__simulating__simulating for needed files
 * ( filelist-tractor-doxygen_import.txt ),
 *  with \ref SrcListtractor__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrListtractor__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileListstractor__pc_linux__rte__rte
 *    <li> \ref FileListstractor__pc_linux__rte__simulating
 *    <li> \ref FileListstractor__pc_linux__rte__sys
 *    <li> \ref FileListstractor__pc_linux__simulating__rte
 *    <li> \ref FileListstractor__pc_linux__simulating__simulating
 *    <li> \ref FileListstractor__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileListstractor__pc_win32__vector_canlib__simulating
 *    <li> \ref FileListstractor__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref FileListstractor__pc_win32__vector_canlib__sys
 *    <li> \ref FileListstractor__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref FileListstractor__pc_win32__simulating__simulating
 *    <li> \ref FileListstractor__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileListstractor__esx__sys__sys
 *    <li> \ref FileListstractor__imi__sys__sys
 *    <li> \ref FileListstractor__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/Application_Config/isoaglib_config.h
 * if the #define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_tractorconsole ( see also at \ref PrjConfigtractor ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/Application_Config/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_tractor_can_server.h
#endif

// include the central interface header for the hardware adaption layer part
// of the "IsoAgLib"
#ifdef SYSTEM_PC
  #include <stddef.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <termios.h>
#endif

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/driver/can/iident_c.h>
#include <IsoAgLib/util/icanpkgext_c.h>
#include <IsoAgLib/util/icancustomer_c.h>
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>

/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisomonitor_c.h>
#include <IsoAgLib/comm/Base/itracmove_c.h>
#include <IsoAgLib/comm/Base/itracpto_c.h>
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>

/** the following #define should be activated when a ECU based tractor shall take sensor signals as
 *  source of simulated speed and PTO
 */
// #define USE_SENSOR_FOR_SPEED_PTO

/** the other variant is a time based ramp for constantly changing speed and PTO */
//#define USE_TIME_RAMP_FOR_SPEED_PTO

#ifdef SYSTEM_PC
/** the last variant for value control is only useable for PC compilation, where keyboard input is accessible */
 #define USE_KEYBOARD_FOR_SPEED_PTO 1
#endif

struct termios saved_attrib;
void reset_input_mode(void)
{
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_attrib);
}
 void set_input_mode(void)
{
  struct termios attrib;

  if (!isatty(STDIN_FILENO)) {
    fprintf(stderr, "Not a terminal!\n");
    exit(EXIT_FAILURE);
  }

  /* save original attributes */
  tcgetattr(STDIN_FILENO, &saved_attrib);
  atexit(reset_input_mode);

  /* make the changes */
  tcgetattr(STDIN_FILENO, &attrib);
  attrib.c_lflag &= ~(ICANON|ECHO); /* disables canonical mode, echo */
  attrib.c_cc[VMIN] = 0; /* non-blocking input */
  attrib.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &attrib);
}

int main()
{ // Initialize the CAN BUS at channel 0 to 250 kbaud
  IsoAgLib::getIcanInstance().init( 0,250 );

  set_input_mode();
  std::cout
      << "v -> decrease velocity;       V -> increase velocity\n"
      << "d -> decrease distance;       D -> increase distance\n"
      << "z -> decrease PTO;            Z -> increase PTO\n"
      << "q -> quit tractor simulation"
      << std::endl;

  ssize_t size;
  // initialize some neede variables
  int16_t speed              = 0;
  int16_t distance           = 0;
  int16_t pto                = 0;
  int32_t i32_now            = 0;
  unsigned char ch;

  // Start address claim of the local identity/member
  IsoAgLib::iIdentItem_c c_myIdent (2,     // rui8_indGroup
                                    2,     // rui8_devClass
                                    0,     // rui8_devClassInst
                                    25,    // rb_func
                                    0x7FF, // rui16_manufCode
                                    27);   // rui32_serNo
                                           // further parameters use the default values as given in the constructor

  IsoAgLib::iTracMove_c& c_tracMove = IsoAgLib::getITracMoveInstance();
  IsoAgLib::iTracPTO_c& c_tracPto = IsoAgLib::getITracPtoInstance();

  /** configure BaseData_c to send:
    - BaseDataGroup1: real and gear based speed and distance; for ISO: also key_switch_state and max power time
    - BaseDataGroup2: front and rear PTO, engine RPM, front and rear hitch information
    - BaseDataCalendar: calendar data
  */
  c_tracMove.config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
  c_tracPto.config (&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);

  while ( IsoAgLib::iSystem_c::canEn() )
  { // run main loop
    i32_now = HAL::getTime();
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib - \ref IsoAgLib::iScheduler_c::timeEvent()
    IsoAgLib::getISchedulerInstance().timeEvent();

    size = read (STDIN_FILENO, &ch, 1);

    if (size > 0)
    {
      switch (ch)
      {
        case 'v':
          speed -= 100;
          std::cout << "velocity decreased to: " << speed << "[mm/s]" << std::endl;
          c_tracMove.setSpeedReal(speed);
          c_tracMove.setSpeedTheor(speed);
          break;
        case 'V':
          speed += 100;
          std::cout << "velocity increased to: " << speed << "[mm/s] "<< std::endl;
          c_tracMove.setSpeedReal(speed);
          c_tracMove.setSpeedTheor(speed);
          break;
        case 'd':
          distance -= 500;
          std::cout << "distance decreased to: " << distance << "[mm]" << std::endl;
          c_tracMove.setDistReal( distance  );
          c_tracMove.setDistTheor( distance  );
          break;
        case 'D':
          distance += 500;
          std::cout << "distance increased to: " << distance << "[mm]" << std::endl;
          c_tracMove.setDistReal( distance  );
          c_tracMove.setDistTheor( distance  );
          break;
        case 'z':
          pto -= 50;
          if ( pto < 50 )
          {
            pto = 0;
          }
          std::cout << "PTO decreased to: " << pto << "[1/s]" << std::endl;
          c_tracPto.setPtoFront(        pto );
          c_tracPto.setPtoFrontEngaged( IsoAgLib::IsoActive );
          c_tracPto.setPtoFront1000(    IsoAgLib::IsoActive );
          c_tracPto.setPtoFrontEconomy( IsoAgLib::IsoActive );

          c_tracPto.setPtoRear(        pto );
          c_tracPto.setPtoRearEngaged( IsoAgLib::IsoActive );
          c_tracPto.setPtoRear1000(    IsoAgLib::IsoActive );
          c_tracPto.setPtoRearEconomy( IsoAgLib::IsoActive );
          break;
        case 'Z':
          pto += 50;
          std::cout << "PTO increased to: " << pto << "[1/s]" << std::endl;
          c_tracPto.setPtoFront(        pto );
          c_tracPto.setPtoFrontEngaged( IsoAgLib::IsoActive );
          c_tracPto.setPtoFront1000(    IsoAgLib::IsoActive );
          c_tracPto.setPtoFrontEconomy( IsoAgLib::IsoActive );

          c_tracPto.setPtoRear(        pto );
          c_tracPto.setPtoRearEngaged( IsoAgLib::IsoActive );
          c_tracPto.setPtoRear1000(    IsoAgLib::IsoActive );
          c_tracPto.setPtoRearEconomy( IsoAgLib::IsoActive );
          break;
        case 'q':
          reset_input_mode();
          std::cout << "quit tractor simulation" << std::endl;
          exit(1);
          break;
      }
    }
     IsoAgLib::iCANIO_c::waitUntilCanReceiveOrTimeout( 50 );
  }
  return 1;
}
