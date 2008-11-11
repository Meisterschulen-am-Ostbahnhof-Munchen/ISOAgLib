/* *************************************************************************
                          4_2_Pwm.cpp - tutorial to show sensor use
                             -------------------
    begin                : Mon Nov 29 10:00:00 CEST 2004

    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
 ***************************************************************************/

/* *************************************************************************
 * This example main application is published NOT as GPL`ed Open Source,   *
 * so that you can include this source file in propietary closed projects. *
 * Everybody is encouraged to use the examples in the examples    *
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
/** \example 4_2_Pwm.cpp
 * This tutorial shall demonstrate the capable extensions of PWM output use by
 * ISO<i><sub>AgLib</sub></i>. It provides seperated setting of allowed current
 * limits and simple check for valid state. In combination with HAL, this
 * handling of PWM is flexible to automatically check either the measured current,
 * or some other simple boolean diagnose functions from BIOS/OS
 * ( e.g. ESX of STW provides for several PWM channels only check for short circuit
 * between PWM output and ground/UBAT --> with ISO<i><sub>AgLib</sub></i> the application
 * can simply check for state, without distinction between these channel types ).
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Use PWM output with standardized diagnose check
 * <li>Set correct current limits independent from later diagnose check ( set limit during channel init )
 * <li>Trigger periodic activities of ISO<i><sub>AgLib</sub></i>
 * 	<ul>
 *	<li>Core class IsoAgLib::iScheduler_c for scheduling of all periodic activities
 *	<li>Method IsoAgLib::iScheduler_c::timeEvent() which can<ul>
 *		<li>Perform activities until defined al_endTime is reached, which is important
 *			for scheduling purposes of whole system - call by IsoAgLib::iScheduler_c::timeEvent( al_endTime )
 *		<li>Process all received CAN messages until all receive buffers are empty
 *			-> simple call, but can lead to deadlock on to high CAN load
 *		</ul>
 *	</ul>
 * <li>Create local identity, for which the ISO<i><sub>AgLib</sub></i> performs an address claim,
 *		so that the ECU can access the <b><i>ISO11783</i></b> BUS with IsoAgLib::iIdentItem_c
 *		and its constructor IsoAgLib::iIdentItem_c::iIdentItem_c
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
 *     conf_4_2_Pwm from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_4_2_Pwm</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 4_2_Pwm</i> to go to the subdirectory 4_2_Pwm
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 4_2_Pwm in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist__4_2_Pwm.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_4_2_Pwm</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec4_2_Pwm__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_no_card__rte
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_rte__rte
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_A1__rte
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_pcan__rte
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_no_card__simulating
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_rte__simulating
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_A1__simulating
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_pcan__simulating
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_no_card__sys
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_rte__sys
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_A1__sys
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__msq_server_pcan__sys
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__simulating__rte
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__simulating__simulating
 *		<li> \ref PrjSpec4_2_Pwm__pc_linux__simulating__sys
 *	</ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *		<li> \ref PrjSpec4_2_Pwm__pc_win32__can_server_sock__simulating
 *		<li> \ref PrjSpec4_2_Pwm__pc_win32__can_server_sock__sys
 * 	</ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *		<li> \ref PrjSpec4_2_Pwm__esx__sys__sys
 *		<li> \ref PrjSpec4_2_Pwm__imi__sys__sys
 *		<li> \ref PrjSpec4_2_Pwm__pm167__sys__sys
 *	</ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists4_2_Pwm__pc_linux__simulating__simulating for needed files
 * ( filelist__4_2_Pwm.txt ),
 *  with \ref SrcList4_2_Pwm__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList4_2_Pwm__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_no_card__rte
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_rte__rte
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_A1__rte
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_pcan__rte
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_no_card__simulating
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_rte__simulating
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_A1__simulating
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_pcan__simulating
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_no_card__sys
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_rte__sys
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_A1__sys
 *		<li> \ref FileLists4_2_Pwm__pc_linux__msq_server_pcan__sys
 *		<li> \ref FileLists4_2_Pwm__pc_linux__simulating__rte
 *		<li> \ref FileLists4_2_Pwm__pc_linux__simulating__simulating
 *		<li> \ref FileLists4_2_Pwm__pc_linux__simulating__sys
 *	</ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *		<li> \ref FileLists4_2_Pwm__pc_win32__can_server_sock__simulating
 *		<li> \ref FileLists4_2_Pwm__pc_win32__can_server_sock__sys
 * 	</ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *		<li> \ref FileLists4_2_Pwm__esx__sys__sys
 *		<li> \ref FileLists4_2_Pwm__imi__sys__sys
 *		<li> \ref FileLists4_2_Pwm__pm167__sys__sys
 *	</ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/IsoAgLib/isoaglib_config.h
 * if the \#define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_4_2_Pwm ( see also at \ref PrjConfig4_2_Pwm ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/IsoAgLib/isoaglib_config.h
    to include project specific configuration settings.
		Set this define in the project file or Makefile of the whole
		project, so that each source file is compiled with this setting
	*/
#ifndef PRJ_USE_AUTOGEN_CONFIG
	#define PRJ_USE_AUTOGEN_CONFIG config_4_2_Pwm.h
#endif

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/* include some needed util headers */
//#include <IsoAgLib/util/config.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
#include <supplementary_driver/driver/actor/idigitalo_c.h>
#include <supplementary_driver/driver/rs232/irs232io_c.h>

/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//iisomonitor_c.h>

#ifdef SYSTEM_PC
  #ifdef WIN32
    #include <windows.h>
  #endif
#endif


// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;


int main()
{
  // Initialize CAN-Bus
  getIcanInstance().init (0); // CAN-Bus 0 (with defaulting 250 kbit)

  // start address claim of the local identity/member
  IsoAgLib::iIdentItem_c c_myIdent (2,    // aui8_indGroup
                                    2,    // aui8_devClass
                                    0,    // aui8_devClassInst
                                    25,   // ab_func
                                    0x7FF,// aui16_manufCode
                                    27);  // aui32_serNo
                                    // further parameters use the default

  // create some PWM instances
  // STW BIOS of ESX provides current measurement for channel 0-4
  IsoAgLib::iDigitalO_c c_pwmCurrentDiagnose( 0 );
  // set the current limits - they are only used if output is active
  // allowed [50..500] [mA] ( too low would mean that consuming device is lost )
  c_pwmCurrentDiagnose.setActiveCurrentLimits( 50, 500 );
  // set individual PWM frequency if default CONFIG_PWM_DEFAULT_FREQUENCY (100000) is not as wanted
  c_pwmCurrentDiagnose.setFreq( 200000 ); // 200 Hz

  // STW BIOS of ESX can only check for short circuit for other PWM
  IsoAgLib::iDigitalO_c c_pwmCurrentSimple( 5 );
  // set individual PWM frequency if default CONFIG_PWM_DEFAULT_FREQUENCY (100000) is not as wanted
  c_pwmCurrentSimple.setFreq( 400000 ); // 400 Hz

  /** IMPORTANT:
    - The following loop could be replaced of any repeating call of
      getISchedulerInstance().timeEvent();
      which is needed to perform all internal activities of the IsoAgLib.
    - Define the time intervall for getISchedulerInstance().timeEvent()
      in a way, that allows IsoAgLib to trigger all reactions on BUS
      in the ISO 11783 defined time resolution - especially the address
      claim process has some tight time restrictions, that suggest
      a trigger rate of at least 100msec ( you could call the function
      only during address claim, mask updload and other special
      circumstances in a high repetition rate )
    - The main loop is running until iSystem_c::canEn() is returning false.
      This function can be configured by the \#define CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC
      in isoaglib_config.h to ignore short CAN_EN loss.
    - This explicit control of power state without automatic powerdown on CanEn loss
      can be controled with the central config define
      #define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownByExplcitCall
      or
      #define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownOnCanEnLoss
      in the header xgpl_src/IsoAgLib/isoaglib_config.h
    - This can be also controlled during runtime with the function call:
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownByExplcitCall )
      or
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownOnCanEnLoss )
  */
  int32_t i32_nextDebug = 0;
  int32_t i32_idleTimeSpread = 0;
  while ( iSystem_c::canEn() )
  { // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib
    i32_idleTimeSpread = IsoAgLib::getISchedulerInstance().timeEvent();

    // immediately re-loop if it's not yet time for debug messages
    if ( IsoAgLib::iSystem_c::getTime() >= i32_nextDebug )
    { // now it's time for debug
      i32_nextDebug = IsoAgLib::iSystem_c::getTime() + 1000;

      // use simple loop to switch several states
      switch ( ( IsoAgLib::iSystem_c::getTime() / 1000 ) % 4 )
      {
        case 0: // first second - both on
          // either set max possible value for total opening
          c_pwmCurrentDiagnose.set( true );
          // or set PWM value for detailed control
          c_pwmCurrentSimple.set( uint16_t(1024) );
          break;
        case 1:
          // either set max possible value for total opening
          c_pwmCurrentDiagnose.set( false );
          // or set PWM value for detailed control
          c_pwmCurrentSimple.set( uint16_t(2048) );
          break;
        case 2:
          // either set max possible value for total opening
          c_pwmCurrentDiagnose.set( false );
          // or set PWM value for detailed control
          c_pwmCurrentSimple.set( uint16_t(0) );
          break;
        case 3:
          // either set max possible value for total opening
          c_pwmCurrentDiagnose.set( true );
          // or set PWM value for detailed control
          c_pwmCurrentSimple.set( uint16_t(0) );
          break;
      }

      // now check for states
      // use operator!() - which is comparable to if ( !c_pwmCurrentDiagnose.good() )
      if ( ! c_pwmCurrentDiagnose  )
      { // ERROR
        IsoAgLib::getIrs232Instance()
          << "ERROR: c_pwmCurrentDiagnose is faulted with current: " << c_pwmCurrentDiagnose.getDigoutCurrent();
      }
      else
      {
        IsoAgLib::getIrs232Instance()
          << "FINE: c_pwmCurrentDiagnose is in good state with current: " << c_pwmCurrentDiagnose.getDigoutCurrent();
      }
      IsoAgLib::getIrs232Instance()
        << ", current state: " << c_pwmCurrentDiagnose.get()
        << " and PWM output voltage: " << c_pwmCurrentDiagnose.getDigoutAdc()
        << "[mV]\r\n";

      // do some more evaluation
      IsoAgLib::getIrs232Instance()
        << "c_pwmCurrentSimple is currently set to: " << c_pwmCurrentSimple.get()
        << ", has PWM output voltage: " << c_pwmCurrentSimple.getDigoutAdc()
        << "[mV]\r\n";
      switch ( c_pwmCurrentSimple.getState() )
      {
        case IsoAgLib::iDigitalO_c::noDoutErr :
          IsoAgLib::getIrs232Instance()
            << "FINE - c_pwmCurrentSimple is in good state\r\n";
          break;
        case IsoAgLib::iDigitalO_c::dout_openErr :
          IsoAgLib::getIrs232Instance()
            << "PROBLEM - c_pwmCurrentSimple has no contact to consuming device\r\n";
          break;
        case IsoAgLib::iDigitalO_c::dout_shortcutErr :
          IsoAgLib::getIrs232Instance()
            << "ERROR - c_pwmCurrentSimple has shortcut\r\n";
          break;
      }
    }

    if ( i32_idleTimeSpread > 0 ) IsoAgLib::iCanIo_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
  }
  return 1;
}
