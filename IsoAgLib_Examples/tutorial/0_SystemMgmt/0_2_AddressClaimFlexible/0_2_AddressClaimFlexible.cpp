/* *************************************************************************
                          0_2_AddressClaimFlexible.cc
														- tutorial for state dependend address claim as
															DIN 9684 or ISO 11783 device
                             -------------------
    begin                : Sun Jul 18 17:00:00 CEST 2004

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
/** \example 0_2_AddressClaimFlexible.cc
 * This tutorial shall provide the simples base program, which
 * can decide on some logical check, if it shall be active on BUS
 * as ISO 11783 or DIN 9684 device. This could be used to
 * base the decision on a digital input ( switch, which is set by farmer )
 * or by some EEPROM configuration value.
 * The selection for this example is only based on a constant.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Trigger periodic activities of ISO<i><sub>AgLib</sub></i>
 * 	<ul>
 *	<li>Core class IsoAgLib::iScheduler_c for scheduling of all periodic activities
 *	<li>Method IsoAgLib::iScheduler_c::timeEvent() which can<ul>
 *		<li>Perform activities until defined rl_endTime is reached, which is important
 *			for scheduling purposes of whole system - call by IsoAgLib::iScheduler_c::timeEvent( rl_endTime )
 *		<li>Process all received CAN messages until all receive buffers are empty
 *			-> simple call, but can lead to deadlock on to high CAN load
 *		</ul>
 *	</ul>
 * <li>Create local identity, for which the ISO<i><sub>AgLib</sub></i> performs an address claim
 * <li>Select the used protocol during runtime based on some other runtime evaluated criteria
 *		and start address claim with explicit call to IsoAgLib::iIdentItem_c::start()
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
 *     conf_0_2_AddressClaimFlexible from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_0_2_AddressClaimFlexible</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 0_2_AddressClaimFlexible</i> to go to the subdirectory 0_2_AddressClaimFlexible
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 0_2_AddressClaimFlexible in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-0_2_AddressClaimFlexible.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_0_2_AddressClaimFlexible</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec0_2_AddressClaimFlexible__pc_linux__simulating__simulating .
 *
 * The adopted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_linux__rte__rte
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_linux__rte__simulating
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_linux__rte__sys
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_linux__simulating__rte
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_linux__simulating__simulating
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_linux__simulating__sys
 *	</ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_win32__vector_canlib__simulating
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_win32__vector_xl__simulating
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_win32__vector_canlib__sys
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_win32__vector_xl__sys
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_win32__simulating__simulating
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pc_win32__simulating__sys
 * 	</ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__esx__sys__sys
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__imi__sys__sys
 *		<li> \ref PrjSpec0_2_AddressClaimFlexible__pm167__sys__sys
 *	</ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists0_2_AddressClaimFlexible__pc_linux__simulating__simulating for needed files
 * ( filelist-0_2_AddressClaimFlexible-doxygen_import.txt ),
 *  with \ref SrcList0_2_AddressClaimFlexible__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList0_2_AddressClaimFlexible__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_linux__rte__rte
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_linux__rte__simulating
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_linux__rte__sys
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_linux__simulating__rte
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_linux__simulating__simulating
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_linux__simulating__sys
 *	</ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_win32__vector_canlib__simulating
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_win32__vector_xl__simulating
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_win32__vector_canlib__sys
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_win32__vector_xl__sys
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_win32__simulating__simulating
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pc_win32__simulating__sys
 * 	</ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *		<li> \ref FileLists0_2_AddressClaimFlexible__esx__sys__sys
 *		<li> \ref FileLists0_2_AddressClaimFlexible__imi__sys__sys
 *		<li> \ref FileLists0_2_AddressClaimFlexible__pm167__sys__sys
 *	</ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/Application_Config/isoaglib_config.h
 * if the #define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_0_2_AddressClaimFlexible ( see also at \ref PrjConfig0_2_AddressClaimFlexible__esx__sys__sys ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/Application_Config/isoaglib_config.h
    to include project specific configuration settings.
		Set this define in the project file or Makefile of the whole
		project, so that each source file is compiled with this setting
	*/
#ifndef PRJ_USE_AUTOGEN_CONFIG
	#define PRJ_USE_AUTOGEN_CONFIG config_0_2_AddressClaimFlexible.h
#endif

// include the central interface header for the hardware adaption layer part
// of the "IsoAgLib"

/* include some needed util headers */
//#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/igetypos_c.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>

/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>
#include <IsoAgLib/comm/SystemMgmt/isystemmgmt_c.h>


// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;


/** this function is a dummy to represent a runtime decision source for the
	protocol selection
*/
bool checkForStartAsIso11783( void )
{   // a real implementation would check some EEPROM
	// or input settings
	return true;
}

int main()
{ // simply call startImi
  getIcanInstance().init( 0, 250 );
  // variable for GETY_POS
  // default with primary cultivation mounted back
  IsoAgLib::iGetyPos_c myGtp( 2, 0 );
  // specific DIN 9684 setting
	uint8_t myName[12] = "TestIMI";

	// specific ISO 11783 settings
	bool b_selfConf = true;
  uint8_t ui8_indGroup = 2,
      b_func = 25,
      b_wantedSa = 128,
      b_funcInst = 0,
      b_ecuInst = 0;
  uint16_t ui16_manufCode = 0x7FF;
  uint32_t ui32_serNo = 27;

	// if an instance of IsoAgLib::iIdentItem_c is created without any
	// parameters, then the address claim is only started on a following
	// start() call
	IsoAgLib::iIdentItem_c c_myIdent;

	// start address claim of the local member "IMI"
  // if GETY_POS conflicts forces change of POS, the
  // IsoAgLib can change the myGtp val through the pointer to myGtp
	if ( checkForStartAsIso11783() )
	{ // start ECU address claim as ISO 11783
		c_myIdent.start( &myGtp,
				b_selfConf, ui8_indGroup, b_func, ui16_manufCode,
				ui32_serNo, b_wantedSa, 0xFFFF, b_funcInst, b_ecuInst);
	}
	else
	{ // start ECU address claim as DIN 9684
		c_myIdent.start( &myGtp, myName );
	}

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
			This function can be configured by the #define BUFFER_SHORT_CAN_EN_LOSS_MSEC
			in isoaglib_config.h to ignore short CAN_EN loss.
		- This explicit control of power state without automatic powerdown on CanEn loss
			can be controled with the central config define
			#define DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownByExplcitCall
			or
			#define DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownOnCanEnLoss
			in the header xgpl_src/Application_Config/isoaglib_config.h
		- This can be also controlled during runtime with the function call:
			getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownByExplcitCall )
			or
			getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownOnCanEnLoss )
	*/
	while ( iSystem_c::canEn() )
	{ // run main loop
		// IMPORTANT: call main timeEvent function for
		// all time controlled actions of IsoAgLib
		getISchedulerInstance().timeEvent();
  }
  return 1;
}

