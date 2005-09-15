/* *************************************************************************
                          2_0_LocalReadDataStdIso.cpp - tutorial for pure ISO address claim
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
/** \example 2_0_LocalReadDataStdIso.cpp
 * This tutorial shall provide the simples base program, which creates some
 * local process data, which value is regularly updated.
 * Remote ECUs can request single values or can start measurement programs.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Create some local standard process data and update their data so that
 *    a remote device can request current data
 * <ul>
 *  <li>Standard local process data class IsoAgLib::iProcDataLocal_c
 *  <li>Use constructor IsoAgLib::iProcDataLocal_c::iProcDataLocal_c to create variable with defined property
 *  <li>Use IsoAgLib::iProcDataLocal_c::setMasterVal() to set current measurement data
 * </ul>
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
 * <li>Overview on process data at \ref ProcDataPage
 * <li>Overview information on Scheduling at \ref SchedulerPage
 * <li>Overview on global communication functions at \ref CommOverPage
 * </ol>
 *
 * <H1>Howto compile this example</H1>
 * <ol>
 * <li>Call the script update_makefile.sh with the spec file
 *     conf_2_0_LocalReadDataStdIso from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_2_0_LocalReadDataStdIso</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 2_0_LocalReadDataStdIso</i> to go to the subdirectory 2_0_LocalReadDataStdIso
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 2_0_LocalReadDataStdIso in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-2_0_LocalReadDataStdIso.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_2_0_LocalReadDataStdIso</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec2_0_LocalReadDataStdIso__pc_linux__simulating__simulating .
 *
 * The adopted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_linux__rte__rte
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_linux__rte__simulating
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_linux__rte__sys
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_linux__simulating__rte
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_linux__simulating__simulating
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_win32__vector_canlib__simulating
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_win32__vector_canlib__sys
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_win32__simulating__simulating
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__esx__sys__sys
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__imi__sys__sys
 *    <li> \ref PrjSpec2_0_LocalReadDataStdIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists2_0_LocalReadDataStdIso__pc_linux__simulating__simulating for needed files
 * ( filelist-2_0_LocalReadDataStdIso-doxygen_import.txt ),
 *  with \ref SrcList2_0_LocalReadDataStdIso__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList2_0_LocalReadDataStdIso__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_linux__rte__rte
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_linux__rte__simulating
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_linux__rte__sys
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_linux__simulating__rte
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_linux__simulating__simulating
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_win32__vector_canlib__simulating
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_win32__vector_canlib__sys
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_win32__simulating__simulating
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__esx__sys__sys
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__imi__sys__sys
 *    <li> \ref FileLists2_0_LocalReadDataStdIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/Application_Config/isoaglib_config.h
 * if the #define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_2_0_LocalReadDataStdIso ( see also at \ref PrjConfig2_0_LocalReadDataStdIso ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/Application_Config/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_2_0_LocalReadDataStdIso.h
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
#include <IsoAgLib/comm/Process/proc_c.h>
#include <IsoAgLib/comm/Process/Local/Std/iprocdatalocal_c.h>


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
  getIcanInstance().init( 0, 250 );
  // variable for GETY_POS
  // default with fertilizer spreader mounted back
  IsoAgLib::iGetyPos_c myGtp( 5, 0 );

  // start address claim of the local member "IMI"
  // if GETY_POS conflicts forces change of POS, the
  // IsoAgLib can cahnge the myGtp val through the pointer to myGtp
  bool b_selfConf = true;
  uint8_t ui8_indGroup = 2,
      b_func = 25,
      b_wantedSa = 128,
      b_funcInst = 0,
      b_ecuInst = 0;
  uint16_t ui16_manufCode = 0x7FF;
  uint32_t ui32_serNo = 27;

  // start address claim of the local member "IMI"
  // if GETY_POS conflicts forces change of POS, the
  // IsoAgLib can change the myGtp val through the pointer to myGtp
  IsoAgLib::iIdentItem_c c_myIdent( &myGtp,
      b_selfConf, ui8_indGroup, b_func, ui16_manufCode,
      ui32_serNo, b_wantedSa, 0xFFFF, b_funcInst, b_ecuInst);


  // local process data for "on/off mechanical" [0/0x64] of primaer Bodenbearbeitung (LIS=0, GETY=2, WERT=1, INST=0)
  // with full working width (ZAEHLNUM 0xFF), POS, GETY_POS of local data (can vary from previously given GETY and POS),
  // the pointer to myGtp helps automatic update of GETY_POS, mark this value as NOT cumulated (default)
  const ElementDDI_s s_myOnoff[2] =
  { 
    // DDI 141, element 0
    {141, 0, true, GeneralCommand_c::exactValue},
    // termination entry
    {0xFFFF, 0xFFFF, false, GeneralCommand_c::noValue}
  };
  IsoAgLib::iProcDataLocal_c c_myOnoff(
    #ifdef USE_ISO_11783
    s_myOnoff,
    #endif
    #ifdef USE_DIN_9687
    0, 0x1, 0x0, 0xFF,
    #endif
    myGtp, 2, myGtp, &myGtp, false);
  // local process data for "working width" [mm] of primaer Bodenbearbeitung (LIS=0, GETY=2, WERT=3, INST=1)
  const ElementDDI_s s_myWorkWidth[2] =
  { 
    // DDI 67, element 0
    {67, 0, true, GeneralCommand_c::exactValue},
    // termination entry
    {0xFFFF, 0xFFFF, false, GeneralCommand_c::noValue}
  };
  IsoAgLib::iProcDataLocal_c c_myWorkWidth(
    #ifdef USE_ISO_11783
    s_myWorkWidth,
    #endif
    #ifdef USE_DIN_9687
    0, 0x3, 0x1, 0xFF,
    #endif
    myGtp, 2, myGtp, &myGtp, false);
  // local process data for "application rate" [kg/ha] of primaer Bodenbearbeitung (LIS=0, GETY=2, WERT=5, INST=0)
  const ElementDDI_s s_myApplicationRate[2] =
  { 
    // DDI 7, element 0
    {7, 0, true, GeneralCommand_c::exactValue},
    // termination entry
    {0xFFFF, 0xFFFF, false, GeneralCommand_c::noValue}
  };
  IsoAgLib::iProcDataLocal_c c_myApplicationRate(
    #ifdef USE_ISO_11783
    s_myApplicationRate,
    #endif
    #ifdef USE_DIN_9687
    0, 0x5, 0x0, 0xFF,
    #endif
    myGtp, 2, myGtp, &myGtp, false);

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
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib
    IsoAgLib::getISchedulerInstance().timeEvent();

    // update local value for local process data so that remote ECUs can get the current
    // value from IsoAgLib
    c_myOnoff.setMasterVal( localIsWorking() );
    c_myWorkWidth.setMasterVal( localGetWorkingWidth() );
    c_myApplicationRate.setMasterVal( localGetApplicationRate() );
  }
  return 1;
}

