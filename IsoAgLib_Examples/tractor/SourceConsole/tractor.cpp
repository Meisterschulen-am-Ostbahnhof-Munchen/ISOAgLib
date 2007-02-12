/* *************************************************************************
                          tractor.cpp - example for tractor ECU (work-in-progress)
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
 * modify it under the terms of the GNU Lesser General Public License as   *std::cout<< getITracMoveInstance().speedTheor()/36 *100UL << "\n" << std::endl;
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

/* include some needed util headers */
//#include <IsoAgLib/util/config.h>

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
#include <supplementary_driver/driver/sensor/isensori_c.h>
#include <supplementary_driver/driver/actor/idigitalo_c.h>
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <supplementary_driver/driver/sensor/idigitali_c.h>
#include <supplementary_driver/driver/actor/impl/digitalo_c.h>


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

#include <supplementary_driver/driver/sensor/isensori_c.h>
#include <supplementary_driver/driver/sensor/ianalogi_c.h>

  // Eing�ge zu Jobrechner

struct termios saved_attrib;
void reset_input_mode(void)
{
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_attrib);
}
 void set_input_mode(void)
{
  struct termios attrib;

  /* check stdin is a terminal */

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

// variable for DEV_KEY ( device type, device type instance )
// default with tractor device type
//IsoAgLib::iISOName_c myIsoName(1,0);

int main()
{ // Initialize the CAN BUS at channel 0 to 250 kbaud
  IsoAgLib::getIcanInstance().init( 0,250 );

  set_input_mode();
  std::cout
      << "v -> langsamer Fahren; V -> schneller Fahren\n"
      << "d -> Strecke kuerzer;  D -> Strecke laenger\n"
      << "e -> Motor langsamer;  E -> Motor schneller\n"
      << "z -> PTO langsamer;    Z -> PTO schneller\n"
      << "q -> EXIT"
      << std::endl;
  int speed = 0, distance = 0, engine = 0, pto = 0;

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
  c_tracPto.config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);

  int32_t i32_nextDebugSend = 0;
  int32_t i32_distance = 0;
  int32_t i32_lastLoop = 0;
  int32_t i32_now = 0;
  unsigned char ch;
  ssize_t size;
  int32_t i32_idleTimeSpread = 0;

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
          std::cout << "Geschwindigkeit: " << speed << "[mm/s]" << std::endl;
          c_tracMove.setSpeedReal(speed);
          c_tracMove.setSpeedTheor(speed);
          break;
        case 'V':
          speed += 100;
          std::cout << "Geschwindigkeit: " << speed << "[mm/s] "<< std::endl;
          c_tracMove.setSpeedReal(speed);
          c_tracMove.setSpeedTheor(speed);
          break;
        case 'd':
          distance -= 500;
          std::cout << "Distanz " << distance << "[mm]" << std::endl;
          c_tracMove.setDistReal( distance  );
          c_tracMove.setDistTheor( distance  );
          break;
        case 'D':
          distance += 500;
          std::cout << "Distanz " << distance << "[mm]" << std::endl;
          c_tracMove.setDistReal( distance  );
          c_tracMove.setDistTheor( distance  );
          break;
        case 'z':
          pto -= 50;
          if ( pto < 50 )
          {
            pto = 0;
          }
          std::cout << "PTO auf " << pto << "[1/s]" << std::endl;
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
          std::cout << "PTO auf " << pto << "[1/s]" << std::endl;
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
          std::cout << "Exit" << std::endl;
          exit(1);
          break;
        case 'n':
          //c_tracPto.setPtoFront(        pto );
          c_tracPto.setPtoRearEngaged( IsoAgLib::IsoInactive );
          c_tracPto.setPtoFront1000(    IsoAgLib::IsoInactive );
          c_tracPto.setPtoFrontEconomy( IsoAgLib::IsoInactive );

          //c_tracPto.setPtoRear(        pto );
          c_tracPto.setPtoFrontEngaged( IsoAgLib::IsoInactive );
          c_tracPto.setPtoFront1000(    IsoAgLib::IsoInactive );
          c_tracPto.setPtoFrontEconomy( IsoAgLib::IsoInactive );

          break;
      }
    }
     //if ( i32_idleTimeSpread > 0 ) Sleep( i32_idleTimeSpread );
     IsoAgLib::iCANIO_c::waitUntilCanReceiveOrTimeout( 50 );
  }
  return 1;
}
