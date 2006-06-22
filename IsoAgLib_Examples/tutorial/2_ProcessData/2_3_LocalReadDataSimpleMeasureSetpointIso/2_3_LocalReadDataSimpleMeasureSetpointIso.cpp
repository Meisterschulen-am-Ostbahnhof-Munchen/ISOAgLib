/* *************************************************************************
                          2_3_LocalReadDataSimpleMeasureSetpointIso.cpp
													- tutorial for pure ISO address claim
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
/** \example 2_3_LocalReadDataSimpleMeasureSetpointIso.cpp
 * This tutorial shall provide the simples base program, which creates some
 * local process data, which value is regularly updated.
 * Remote ECUs can only request single values, as this variant uses the
 * simple process data type iProcDataLocalSimpleMeasure_c.

 * Remote ECUs can request single values or start measurement programs.
 * This variant reduces ressources need by exclusion of the sophisticted
 * setpoint management. Received setpoints are simply stored and an ACK
 * is automatically sent on receive. So this variant shouldn't be used
 * for ECUs which must decide on acceptable setpoints.
 * In case an ECU uses several process data with different requirements,
 * it is useful to use the suitable feature set for each process data type.
 * A sophisticated setpoint or measurement type needs some heap memory for the
 * data management.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Create some local process data which can't distinguish setpoints from
 *		different senders ad which can only accept automatically received
 *		setpoints ( look for IsoAgLib::iProcDataLocal_c to get possibility for explicit
 *		decision on implementation of received setpoints based on value,
 *		time and sender )
 *	<li>These process data type additionally don't support measurement programs
 *		so that remote device can only perform one shot
 *		data requests ( start of a periodical send of data isn't supported -
 *		look at IsoAgLib::iProcDataLocal_c for this )
 * <ul>
 *	<li>Local process data class IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c with simple setpoint handling
 *	<li>Use constructor IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c::iProcDataLocalSimpleSetpointSimpleMeasure_c
 *		to create variable with defined property
 *	<li>Use IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c::setMasterMeasurementVal() to set current measurement data
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
 * <li>Overview on process data at \ref ProcDataPage
 * <li>Overview information on Scheduling at \ref SchedulerPage
 * <li>Overview on global communication functions at \ref CommOverPage
 * </ol>
 *
 * <H1>Howto compile this example</H1>
 * <ol>
 * <li>Call the script update_makefile.sh with the spec file
 *     conf_2_3_LocalReadDataSimpleMeasureSetpointIso from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_2_3_LocalReadDataSimpleMeasureSetpointIso</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 2_3_LocalReadDataSimpleMeasureSetpointIso</i> to go to the subdirectory 2_3_LocalReadDataSimpleMeasureSetpointIso
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 2_3_LocalReadDataSimpleMeasureSetpointIso in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-2_3_LocalReadDataSimpleMeasureSetpointIso.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_2_3_LocalReadDataSimpleMeasureSetpointIso</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__rte__rte
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__rte__simulating
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__rte__sys
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__simulating__rte
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__simulating__simulating
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__simulating__sys
 *	</ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__vector_canlib__simulating
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__vector_xl_drv_lib__simulating
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__vector_canlib__sys
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__vector_xl_drv_lib__sys
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__simulating__simulating
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__simulating__sys
 * 	</ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__esx__sys__sys
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__imi__sys__sys
 *		<li> \ref PrjSpec2_3_LocalReadDataSimpleMeasureSetpointIso__pm167__sys__sys
 *	</ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__simulating__simulating for needed files
 * ( filelist-2_3_LocalReadDataSimpleMeasureSetpointIso-doxygen_import.txt ),
 *  with \ref SrcList2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__rte__rte
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__rte__simulating
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__rte__sys
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__simulating__rte
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__simulating__simulating
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_linux__simulating__sys
 *	</ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__vector_canlib__simulating
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__vector_xl_drv_lib__simulating
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__vector_canlib__sys
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__vector_xl_drv_lib__sys
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__simulating__simulating
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pc_win32__simulating__sys
 * 	</ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__esx__sys__sys
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__imi__sys__sys
 *		<li> \ref FileLists2_3_LocalReadDataSimpleMeasureSetpointIso__pm167__sys__sys
 *	</ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/Application_Config/isoaglib_config.h
 * if the #define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_2_3_LocalReadDataSimpleMeasureSetpointIso ( see also at \ref PrjConfig2_3_LocalReadDataSimpleMeasureSetpointIso ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/Application_Config/isoaglib_config.h
    to include project specific configuration settings.
		Set this define in the project file or Makefile of the whole
		project, so that each source file is compiled with this setting
	*/
#ifndef PRJ_USE_AUTOGEN_CONFIG
	#define PRJ_USE_AUTOGEN_CONFIG config_2_3_LocalReadDataSimpleMeasureSetpointIso.h
#endif

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/* include some needed util headers */
//#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/idevkey_c.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>

/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>
#include <IsoAgLib/comm/SystemMgmt/isystemmgmt_c.h>
#include <IsoAgLib/comm/Process/proc_c.h>
#include <IsoAgLib/comm/Process/Local/SimpleMeasureSetpoint/iprocdatalocalsimplesetpointsimplemeasure_c.h>
#include <IsoAgLib/comm/Process/iprocess_c.h>

#include "devicedescription/DeviceDescription.xml-func.h"

// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;

/** dummy function to deliver the current working state */
bool localIsWorking()       { return ( ( ( IsoAgLib::iSystem_c::getTime() / 3000 ) % 2 ) == 0 )?true:false; }
bool localGetWorkingWidth() { return 3000; }
int32_t localGetApplicationRate() { return IsoAgLib::iSystem_c::getTime(); }

int main()
{ // init CAN channel with 250kBaud at channel 0 ( count starts with 0 )
  IsoAgLib::getIcanInstance().init( 0, 250 );

  /// register pool of the device description
  bool b_registerSuccess = IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().registerDevicePool(&c_myIdent, deviceDescription_de, ui32_arrayLength_de, true);

	// start address claim of the local member "IMI"
  // if DEV_KEY conflicts forces change of device class instance, the
  // IsoAgLib can change the myDevKey val through the pointer to myDevKey
  //  DIN:
#if defined(USE_DIN_9684) && !defined(USE_ISO_11783)
  uint8_t c_myName[] = "Hi-Me";
  IsoAgLib::iIdentItem_c c_myIdent( &myDeviceDevKey, c_myName, IsoAgLib::IState_c::DinOnly);
#endif

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
			This function can be configured by the #define CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC
			in isoaglib_config.h to ignore short CAN_EN loss.
		- This explicit control of power state without automatic powerdown on CanEn loss
			can be controled with the central config define
			#define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownByExplcitCall
			or
			#define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownOnCanEnLoss
			in the header xgpl_src/Application_Config/isoaglib_config.h
		- This can be also controlled during runtime with the function call:
			getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownByExplcitCall )
			or
			getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownOnCanEnLoss )
	*/
	while ( iSystem_c::canEn() )
	{ // run main loop
    IsoAgLib::iCANIO_c::waitUntilCanReceiveOrTimeout( 50 );

		// IMPORTANT: call main timeEvent function for
		// all time controlled actions of IsoAgLib
		IsoAgLib::getISchedulerInstance().timeEvent();

		// update local value for local process data so that remote ECUs can get the current
		// value from IsoAgLib
		c_myOnoff.setMasterMeasurementVal( localIsWorking() );
		c_myWorkWidth.setMasterMeasurementVal( localGetWorkingWidth() );
		c_myApplicationRate.setMasterMeasurementVal( localGetApplicationRate() );
	}
  return 1;
}

