/* *************************************************************************
                          3_1_LBSTerminalDin.cc - tutorial for pure ISO address claim
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
/** \example 3_1_LBSTerminalDin.cc
 * This tutorial shall provide the base program, which uploads a DIN 9684 LBS+
 * terminal mask definition ( binary ) to the terminal.
 * This example demonstrates the terminal dependend selection of the terminal
 * definition to upload. This is important, as terminals like Varioterminal and
 * Fieldstar Terminal requires different terminal types.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Upload of binary mask pool to DIN terminal
 * <ul>
 *	<li>Register two alternative pools for Varioterminal and Fieldstar terminal
 *		with IsoAgLib::iDINMaskUpload_c::registerLbsPlusMask()
 *	<li>Use structure types IsoAgLib::t_maskDefinition and IsoAgLib::t_syncTupel
 *		to define DIN terminal resources
 *		for sync between terminal and ECU
 *	<li>Used member method IsoAgLib::iDINMonitor_c::existMemberGtp()
 *		to look for existance of possible versions of DIN terminals
 *		( based on IsoAgLib::iDINItem_c::gtp() , IsoAgLib::iDINItem_c::name()
 * </ul>
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
 * <li>Create local identity, for which the ISO<i><sub>AgLib</sub></i> performs an address claim,
 *		so that the ECU can access the <b><i>ISO11783</i></b> BUS with IsoAgLib::iIdentItem_c
 *		and its constructor IsoAgLib::iIdentItem_c::iIdentItem_c
 * </ol>
 * <H1>Where to look for further information</H1>
 * <ol>
 * <li>Overview on upload of LBS+ mask pools ( especially DIN Varioterminal of AGCO/Fendt,
 *		Fieldstar Terminal of AGCO/Dronningborg and DIN ACT of Agrocom ) at \ref DinTerminalPage
 * <li>Overview information on Scheduling at \ref SchedulerPage
 * <li>Overview on global communication functions at \ref CommOverPage
 * </ol>
 *
 * <H1>Howto compile this example</H1>
 * <ol>
 * <li>Call the script update_makefile.sh with the spec file
 *     conf_3_0_VirtualTerminalIso from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_3_0_VirtualTerminalIso</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 3_0_VirtualTerminalIso</i> to go to the subdirectory 3_0_VirtualTerminalIso
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 3_0_VirtualTerminalIso in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-3_0_VirtualTerminalIso.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_3_0_VirtualTerminalIso</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Howto compile this example</H1>
 * <ol>
 * <li>Call the script update_makefile.sh with the spec file
 *     conf_3_1_LBSTerminalDin from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_3_1_LBSTerminalDin</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 3_1_LBSTerminalDin</i> to go to the subdirectory 3_1_LBSTerminalDin
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 3_1_LBSTerminalDin in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-3_1_LBSTerminalDin.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_3_1_LBSTerminalDin</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec3__1_LBSTerminalDin__pc_linux__simulating__simulating.
 *
 * The adopted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 *	<ul>
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_linux__rte__rte
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_linux__rte__simulating
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_linux__rte__sys
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_linux__simulating__rte
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_linux__simulating__simulating
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_linux__simulating__sys
 *	</ul>
 * <li>Configuration Setups for Win32 on PC:
 * 	<ul>
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_win32__vector_canlib__simulating
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_win32__vector_xl__simulating
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_win32__vector_canlib__sys
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_win32__vector_xl__sys
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_win32__simulating__simulating
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pc_win32__simulating__sys
 * 	</ul>
 * <li>Configuration Setupts for some embedded targets:
 *	<ul>
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__esx__sys__sys
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__imi__sys__sys
 *		<li> \ref PrjSpec3_1_LBSTerminalDin__pm167__sys__sys
 *	</ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists3_1_LBSTerminalDin__pc_linux__simulating__simulating for needed files
 * ( filelist-3_1_LBSTerminalDin-doxygen_import.txt ),
 *  with \ref SrcList3_1_LBSTerminalDin__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList3_1_LBSTerminalDin__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 *	<ul>
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_linux__rte__rte
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_linux__rte__simulating
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_linux__rte__sys
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_linux__simulating__rte
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_linux__simulating__simulating
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_linux__simulating__sys
 *	</ul>
 * <li>Configuration Setups for Win32 on PC:
 *	<ul>
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_win32__vector_canlib__simulating
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_win32__vector_xl__simulating
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_win32__vector_canlib__sys
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_win32__vector_xl__sys
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_win32__simulating__simulating
 *		<li> \ref FileLists3_1_LBSTerminalDin__pc_win32__simulating__sys
 * 	</ul>
 * <li>Configuration Setupts for some embedded targets:
 *	<ul>
 *		<li> \ref FileLists3_1_LBSTerminalDin__esx__sys__sys
 *		<li> \ref FileLists3_1_LBSTerminalDin__imi__sys__sys
 *		<li> \ref FileLists3_1_LBSTerminalDin__pm167__sys__sys
 *	</ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/Application_Config/isoaglib_config.h
 * if the #define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_3_1_LBSTerminalDin ( see also at \ref PrjConfig3_1_LBSTerminalDin__esx__sys__sys ).
 *                                                                         */


/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/Application_Config/isoaglib_config.h
    to include project specific configuration settings.
		Set this define in the project file or Makefile of the whole
		project, so that each source file is compiled with this setting
	*/
#ifndef PRJ_USE_AUTOGEN_CONFIG
	#define PRJ_USE_AUTOGEN_CONFIG config_3_1_LBSTerminalDin.h
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
#include <IsoAgLib/comm/SystemMgmt/DIN9684/idinmonitor_c.h>
#include <IsoAgLib/comm/DIN_Terminal/idinmaskupload_c.h>

#ifdef SYSTEM_PC
  // include File I/O header
  #include <fstream>
  #include <iostream>
#endif


// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;

int main()
{ // variable for GETY_POS
  // default with primary cultivation mounted back
  IsoAgLib::iGetyPos_c myGtp( 2, 0 );
  // uint8_t string for name of this IMI (7 characters + '\0')
  uint8_t myName[8];
  uint8_t pui8_nameForFieldstar[7] = {0,0,0,0x32,0,0x10,1};
  uint8_t pb_fieldstarName[7] = {0, 0, 0, 0x31, 0, 0x10, 2};
  // address claim state of the IMI
  bool myClaimedAddress = false;
  enum t_maskNameSetting { fieldstarName = 1, varioName = 2 };
  t_maskNameSetting en_maskNameSetting = fieldstarName;

  #ifdef SYSTEM_PC
   char c_data;
   std::ifstream fileMaske;

   // mask source pointer and mask source size for Fieldstar
   int32_t i32_menueByteSize = 0;
   HUGE_MEM uint8_t pMenueStartVektor[46000];

   fileMaske.open("../../../simulatedIo/menufiles/implmenuFieldstar.bin", std::ios_base::in|std::ios_base::binary);
   while (fileMaske && !fileMaske.eof())
   {
     fileMaske.get(c_data);
     pMenueStartVektor[i32_menueByteSize] = c_data;
     i32_menueByteSize++;
   }
   // size index counted EOF
   i32_menueByteSize--;
   fileMaske.close();
   fileMaske.clear();

   // mask source pointer and mask source size for Fendt Vario
   int32_t i32_VariomenueByteSize = 0;
   HUGE_MEM uint8_t pVarioConfigStartVektor[0x81];
   memset(pVarioConfigStartVektor, 0xFF, 0x80);
   HUGE_MEM uint8_t pVarioMenueStartVektor[46000];


   fileMaske.open("../../../simulatedIo/menufiles/implmenuVario.bin", std::ios_base::in|std::ios_base::binary);
   while (fileMaske && !fileMaske.eof())
   {
     fileMaske.get(c_data);
     pVarioMenueStartVektor[i32_VariomenueByteSize] = c_data;
     i32_VariomenueByteSize++;
   }
   // size index counted EOF
   i32_VariomenueByteSize--;
   fileMaske.close();

  #else
   // mask source pointer and mask source size for Fieldstar
   int32_t i32_menueByteSize = 0x9D86;
   HUGE_MEM uint8_t* pMenueStartVektor = (HUGE_MEM uint8_t*)0x50000;

   // mask source pointer and mask source size for Fendt Vario
   int32_t i32_VariomenueByteSize = 0x15DBC;
   HUGE_MEM uint8_t* pVarioConfigStartVektor = (HUGE_MEM uint8_t*)0x60000;
   HUGE_MEM uint8_t* pVarioMenueStartVektor = (HUGE_MEM uint8_t*)0x30000;
  #endif

  // register Fieldstar Old Style
  IsoAgLib::iGetyPos_c c_FieldstarGtp(1,2); // 0xA
  IsoAgLib::t_syncTupel psFieldstarOldSyncNo[] = {
   {100, FS_OLD_SW_VERSION},
   {430, FS_OLD_REQ_TERM_VERSION},
   {100, FS_OLD_PROJECT_VERSION}
  };

  IsoAgLib::t_maskDefinition sFieldstarOldMask = {
   &myGtp, // pointer to local gtp
   iGetyPos_c(1,2), // GetyPos of Terminal (0xA) -> Gety:1, Pos:2
   {0, 0, 0, 0x31, 0, 0x10, 2}, // name of Fieldstar Term
   FieldstarOld, // is LBS+ terminal
   NULL, // no config data
   0, // no config data for mask
   pMenueStartVektor, // pointer to mask data
   i32_menueByteSize, // size of the mask
   3, // use 13
   psFieldstarOldSyncNo
  };
  sFieldstarOldMask.pt_syncArray = psFieldstarOldSyncNo;
  sFieldstarOldMask.phb_maskData = pMenueStartVektor;


  // register Fendt Vario
  IsoAgLib::iGetyPos_c c_VarioGtp(1, 5); // 0xD
  IsoAgLib::t_syncTupel psVarioSyncNo[] = {
   {1, LBS_PLUS_SYSTEM_STATUS},
   {100, LBS_PLUS_GENERIC_DRIVER_VERSION},
   {100, LBS_PLUS_PROJECT_VERSION},
   {0x353, LBS_PLUS_PROJECT_DATE},
   {0x04000082, LBS_PLUS_PROJECT_STATUS},
   {200, LBS_PLUS_PROJECT_IDENT_NO},
   {0x0000406B, LBS_PLUS_PROJECT_SIGNATURE},
   {0x00000196, LBS_PLUS_PROJECTOR_VERSION},
   {0x000FA266, LBS_PLUS_PROJECTOR_DATE},
   {0x60000, LBS_PLUS_PROJECT_START},
   {0x70000, LBS_PLUS_PROJECT_VISISTART},
   {0x00000080, LBS_PLUS_PROJECT_VISISIZE}
  };
  IsoAgLib::t_maskDefinition sVarioMask = {
   &myGtp, // pointer to local gtp
   iGetyPos_c(1, 5), // GetyPos of Terminal
   "FENVARC", // name of Fendt Vario Term
   FendtVario, // is LBS+ terminal
   pVarioConfigStartVektor, // string with config settings
   0x80, // size of config string
   pVarioMenueStartVektor, // pointer to mask data
   i32_VariomenueByteSize, // size of the mask
   12, // use 13
   psVarioSyncNo
  };
  sVarioMask.pt_syncArray = psVarioSyncNo;
  sVarioMask.phb_maskData = pVarioMenueStartVektor;

  // configure the base data for receive
  getIdinMaskuploadInstance().registerLbsPlusMask(&sFieldstarOldMask);
  getIdinMaskuploadInstance().registerLbsPlusMask(&sVarioMask);

	// start address claim of the local member "IMI"
  // if GETY_POS conflicts forces change of POS, the
  // IsoAgLib can change the myGtp val through the pointer to myGtp
  IsoAgLib::iIdentItem_c c_myIdent( &myGtp, myName );

  /** IMPORTANT:
	  - The following loop could be replaced of any repeating call of
			IsoAgLib::getISchedulerInstance().timeEvent();
			which is needed to perform all internal activities of the IsoAgLib.
		- Define the time intervall for IsoAgLib::getISchedulerInstance().timeEvent()
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
		IsoAgLib::getISchedulerInstance().timeEvent();
		if ( ( !getIdinMonitorInstance().existDinMemberGtp(myGtp, true) )
			&& ( getIdinMonitorInstance().existDinMemberGtp(myGtp)        ) )
		{ // local member exist but has not yet claimed address -> check name
			if ( (en_maskNameSetting != varioName)
				&& (getIdinMonitorInstance().existDinMemberGtp(c_VarioGtp, true))
				&& (memcmp(getIdinMonitorInstance().dinMemberGtp(c_VarioGtp).name(), "FENVARC", 7) == 0)
					)
			{ // Vario Terminal has claimed address
				getIdinMonitorInstance().dinMemberGtp(myGtp).setName((const uint8_t*)"LEMK_1P");
				en_maskNameSetting = varioName;
			}
			else if ( (en_maskNameSetting != fieldstarName)
				&& (getIdinMonitorInstance().existDinMemberGtp(c_FieldstarGtp, true))
				&& (memcmp(getIdinMonitorInstance().dinMemberGtp(c_VarioGtp).name(), pb_fieldstarName, 7) == 0)
					)
			{ // Fieldstar Terminal has claimed address
				getIdinMonitorInstance().dinMemberGtp(myGtp).setName(pui8_nameForFieldstar);
				en_maskNameSetting = fieldstarName;
			}
		}
  }
  return 1;
}
