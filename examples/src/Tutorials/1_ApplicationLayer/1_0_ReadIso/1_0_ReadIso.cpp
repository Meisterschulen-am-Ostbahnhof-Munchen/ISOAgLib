/* *************************************************************************
                          1_0_ReadIso.cpp - tutorial for pure ISO address claim
                             -------------------
    begin                : Sun Jul 18 17:00:00 CEST 2004

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
/** \example 1_0_ReadIso.cpp
 * This tutorial shall provide a simple base program, which reads some
 * regularly sent base data ( especially tractor information ).
 * Demonstrate also the use of GPS data - which might be wrong decoded
 * at the moment, as only open accessible sources ( Google ) were used
 * to retrieve the format of these few NMEA 2000 strings.
 * Provide option to send result of lookup on RS232 - this is dependent on
 * define USE_RS232_FOR_DEBUG .
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Access on the cached base data ( important tractor information like
 *    speed, distance and hitch ) information from tractor
 * <ul>
 *  <li>Data storage class IsoAgLib::iBase_c
 *  <li>Access functions like IsoAgLib::iBase_c::distTheor() , IsoAgLib::iBase_c::distReal(),
 *    IsoAgLib::iBase_c::speedTheor() , IsoAgLib::iBase_c::speedReal() , IsoAgLib::iBase_c::hitchRear
 *    IsoAgLib::iBase_c::month()
 * </ul>
 * <li>Use RS232 for C++ iostream like text output with class IsoAgLib::iRS232IO_c
 * <li>Trigger periodic activities of ISO<i><sub>AgLib</sub></i>
 *  <ul>
 *  <li>Core class IsoAgLib::iScheduler_c for scheduling of all periodic activities
 *  <li>Method IsoAgLib::iScheduler_c::timeEvent() which can<ul>
 *    <li>Perform activities until defined al_endTime is reached, which is important
 *      for scheduling purposes of whole system - call by IsoAgLib::iScheduler_c::timeEvent( al_endTime )
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
 * <li>Overview on accessing cached main information at \ref BaseDataPage
 * <li>Overview information on Scheduling at \ref SchedulerPage
 * <li>Overview on System Management at \ref SystemMgmtPage
 * <li>Overview on global communication functions at \ref CommOverPage
 * </ol>
 *
 * <H1>Howto compile this example</H1>
 * <ol>
 * <li>Call the script update_makefile.sh with the spec file
 *     conf_1_0_ReadIso from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_1_0_ReadIso</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 1_0_ReadIso</i> to go to the subdirectory 1_0_ReadIso
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 1_0_ReadIso in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-1_0_ReadIso.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_1_0_ReadIso</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec1_0_ReadIso__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpec1_0_ReadIso__pc_linux__rte__rte
 *    <li> \ref PrjSpec1_0_ReadIso__pc_linux__rte__simulating
 *    <li> \ref PrjSpec1_0_ReadIso__pc_linux__rte__sys
 *    <li> \ref PrjSpec1_0_ReadIso__pc_linux__simulating__rte
 *    <li> \ref PrjSpec1_0_ReadIso__pc_linux__simulating__simulating
 *    <li> \ref PrjSpec1_0_ReadIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpec1_0_ReadIso__pc_win32__vector_canlib__simulating
 *    <li> \ref PrjSpec1_0_ReadIso__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref PrjSpec1_0_ReadIso__pc_win32__vector_canlib__sys
 *    <li> \ref PrjSpec1_0_ReadIso__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref PrjSpec1_0_ReadIso__pc_win32__simulating__simulating
 *    <li> \ref PrjSpec1_0_ReadIso__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpec1_0_ReadIso__esx__sys__sys
 *    <li> \ref PrjSpec1_0_ReadIso__imi__sys__sys
 *    <li> \ref PrjSpec1_0_ReadIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists1_0_ReadIso__pc_linux__simulating__simulating for needed files
 * ( filelist-1_0_ReadIso-doxygen_import.txt ),
 *  with \ref SrcList1_0_ReadIso__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList1_0_ReadIso__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileLists1_0_ReadIso__pc_linux__rte__rte
 *    <li> \ref FileLists1_0_ReadIso__pc_linux__rte__simulating
 *    <li> \ref FileLists1_0_ReadIso__pc_linux__rte__sys
 *    <li> \ref FileLists1_0_ReadIso__pc_linux__simulating__rte
 *    <li> \ref FileLists1_0_ReadIso__pc_linux__simulating__simulating
 *    <li> \ref FileLists1_0_ReadIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileLists1_0_ReadIso__pc_win32__vector_canlib__simulating
 *    <li> \ref FileLists1_0_ReadIso__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref FileLists1_0_ReadIso__pc_win32__vector_canlib__sys
 *    <li> \ref FileLists1_0_ReadIso__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref FileLists1_0_ReadIso__pc_win32__simulating__simulating
 *    <li> \ref FileLists1_0_ReadIso__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileLists1_0_ReadIso__esx__sys__sys
 *    <li> \ref FileLists1_0_ReadIso__imi__sys__sys
 *    <li> \ref FileLists1_0_ReadIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/IsoAgLib/isoaglib_config.h
 * if the #define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_1_0_ReadIso ( see also at \ref PrjConfig1_0_ReadIso ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/IsoAgLib/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_1_0_ReadIso.h
#endif

/** set the following define, if the lookup result shall be sent via RS232 */
#define USE_RS232_FOR_DEBUG

#ifdef USE_RS232_FOR_DEBUG
 #include <supplementary_driver/driver/rs232/irs232io_c.h>
#endif


/** set the following defines if to test onforceMaintainPowerforceMaintainPowerforceMaintainPowere or more of the base data*/
//the defines are set with the update_makefile skript if in the corresponding config_1_0_ReadIso
//the defines are set. All base data is defined if the USE_BASE define is active
#ifdef USE_TRACTOR_LIGHT
  #define TEST_TRACTOR_LIGHTING
#endif
#ifdef USE_TRACTOR_GENERAL
  #define TEST_TRACTOR_GENERAL
#endif
#ifdef USE_TRACTOR_MOVE
 #define TEST_TRACTOR_MOVING
 #define TEST_TRACMOVESETPOINT
#endif
#ifdef USE_TIME_GPS
  #define TEST_TIME
#endif
#ifdef USE_TRACTOR_AUX
  #define TEST_TRACAUX
#endif
#ifdef USE_TRACTOR_PTO
 #define TEST_TRACPTO
 #define TEST_TRACPTOSETPOINT
#endif
#ifdef USE_TRACTOR_CERTIFICATION
  #define TEST_TRACCERT
#endif
#ifdef USE_TRACTOR_GUIDANCE
  #define TEST_TRACGUIDANCE
#endif

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/* include some needed util headers */
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>

#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>

/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//iisomonitor_c.h>

#ifdef TEST_TIME
  #include <IsoAgLib/comm/Part7_ApplicationLayer/itimeposgps_c.h>
#endif
#ifdef TEST_TRACTOR_GENERAL
  #include <IsoAgLib/comm/Part7_ApplicationLayer/itracgeneral_c.h>
#endif
#ifdef TEST_TRACTOR_MOVING
  #include <IsoAgLib/comm/Part7_ApplicationLayer/itracmove_c.h>
#endif
#ifdef TEST_TRACTOR_LIGHTING
  #include <IsoAgLib/comm/Part7_ApplicationLayer/ext/itraclight_c.h>
#endif
#ifdef TEST_TRACPTOSETPOINT
  #include <IsoAgLib/comm/Part7_ApplicationLayer/ext/itracptosetpoint_c.h>
#endif
#ifdef TEST_TRACMOVESETPOINT
  #include <IsoAgLib/comm/Part7_ApplicationLayer/ext/itracmovesetpoint_c.h>
#endif
#ifdef TEST_TRACPTO
  #include <IsoAgLib/comm/Part7_ApplicationLayer/itracpto_c.h>
#endif
#ifdef TEST_TRACAUX
  #include <IsoAgLib/comm/Part7_ApplicationLayer/ext/itracaux_c.h>
#endif
#ifdef TEST_TRACCERT
  #include <IsoAgLib/comm/Part7_ApplicationLayer/ext/itraccert_c.h>
#endif
#ifdef TEST_TRACGUIDANCE
  #include <IsoAgLib/comm/Part7_ApplicationLayer/ext/itracguidance_c.h>
  #include <IsoAgLib/comm/Part7_ApplicationLayer/ext/itracguidancecommand_c.h>
#endif


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

#if defined TEST_TRACTOR_LIGHTING || defined TEST_TRACPTO || defined TEST_TRACGUIDANCE || defined TEST_TRACTOR_GENERAL
/**interpret the IsoActiveFlag_t values as strings when output on console */
STL_NAMESPACE::string getIsoActiveFlag(IsoAgLib::IsoActiveFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoInactive";
    case 1: return "IsoActive";
    case 2: return "IsoError";
    case 3: return "IsoNotAvailable";
    default: return "there went something wrong!!";

  }
}

/**interpret the IsoDataReq_t values as strings when output on console */
STL_NAMESPACE::string getIsoDataReq(IsoAgLib::IsoDataReq_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoNoDataRequested";
    case 1: return "IsoDataRequested";
    case 2: return "IsoReservedData";
    case 3: return "IsoDontCare";
    default: return "there went something wrong!!";

  }
}
#endif

#ifdef TEST_TRACPTO
/**interpret the IsoReqFlag values as strings when output on console */
STL_NAMESPACE::string getIsoReqFlag(IsoAgLib::IsoReqFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoExternalReqAccepted";
    case 1: return "IsoControlOverride";
    case 2: return "IsoErrorReq";
    case 3: return "IsoNotAvailableReq";
    default: return "there went something wrong!!";

  }
}
#endif

#ifdef TEST_TRACAUX
/**interpret the getIsoCertRevisionFlag values as strings when output on console*/

/**interpret the IsoAuxFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoAuxFlag(IsoAgLib::IsoAuxFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoBlock";
    case 1: return "IsoFloat";
    case 2: return "IsoError";
    case 3: return "IsoNotAvailable";
    default: return "there went something wrong!!";
  }
}

/**interpret the IsoAuxFlagExtended values as strings when output on console*/
STL_NAMESPACE::string getIsoAuxFlagExtended(IsoAgLib::IsoAuxFlagExtended_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoBlocked";
    case 1: return "IsoExtend";
    case 2: return "IsoRetract";
    case 3: return "IsoFloating";
    case 14: return "IsoError";
    case 15: return "IsoNotAvailable";
    default: return "there went something wrong!!";
  }
}
#endif

#ifdef TEST_TRACCERT
/**interpret the IsoCertLabTypeFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoCertLabTypeFlag(IsoAgLib::IsoCertLabTypeFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoNonCert";
    case 1: return "IsoEUCert";
    case 2: return "IsoNaCert";
    case 7: return "IsoNotAvailableCert";
    default: return "there went something wrong!!";
  }
}

STL_NAMESPACE::string getIsoCertRevisionFlag(IsoAgLib::IsoCertRevisionFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoFirstRev";
    case 1: return "IsoSecondRev";
    case 2: return "IsoThirdRev";
    case 3: return "IsoFourthRev";
    case 7: return "IsoNotAvailableRev";
    default: return "there went something wrong!!";
  }
}

/**interpret the IsoCertTypeFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoCertTypeFlag(IsoAgLib::IsoCertTypeFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoNotCert";
    case 1: return "IsoCert";
    default: return "there went something wrong!!";
  }
}
#endif

#ifdef TEST_TRACGUIDANCE
/**interpret the IsoSteerPosFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoSteerPosFlag(IsoAgLib::IsoSteerPosFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoNotCorrectPos";
    case 1: return "IsoCorrectPos";
    case 2: return "IsoErrorSteerPos";
    case 3: return "IsoNotAvailableSteerPos";
    default: return "there went something wrong!!";
  }
}

/**interpret the IsoSteerReadinessFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoSteerReadinessFlag(IsoAgLib::IsoSteerReadinessFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoSystemNotReady";
    case 1: return "IsoSystemReady";
    case 2: return "IsoErrorSteerReadiness";
    case 3: return "IsoNotAvailableSteerReadiness";
    default: return "there went something wrong!!";
  }
}

/**interpret the IsoResetFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoResetFlag(IsoAgLib::IsoResetFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoResetNotRequired";
    case 1: return "IsoResetRequired";
    case 2: return "IsoErrorReset";
    case 3: return "IsoNotAvailableReset";
    default: return "there went something wrong!!";
  }
}
#endif

#ifdef TEST_TRACTOR_MOVING
/**interpret the IsoDirectionFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoDirectionFlag(IsoAgLib::IsoDirectionFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoReverse";
    case 1: return "IsoForward";
    case 2: return "IsoErrorReset";
    case 3: return "IsoNotAvailableDirection";
    default: return "there went something wrong!!";
  }
}

/**interpret the IsoSpeedSourceFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoSpeedSourceFlag(IsoAgLib::IsoSpeedSourceFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoWheelBasedSpeed";
    case 1: return "IsoGroundBasedSpeed";
    case 2: return "IsoNavigationBasedSpeed";
    case 3: return "IsoBlendedSpeed";
    case 4: return "IsoSimulatedSpeed";
    case 7: return "IsoNotAvailableSpeed";
    default: return "there went something wrong!!";
  }
}

/**interpret the IsoOperatorDirectionFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoOperatorDirectionFlag(IsoAgLib::IsoOperatorDirectionFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoNotReversed";
    case 1: return "IsoReversed";
    case 2: return "IsoErrorReversed";
    case 3: return "IsoNotAvailableReversed";
    default: return "there went something wrong!!";
  }
}
#endif

#if defined TEST_TRACTOR_MOVING || defined TEST_TRACAUX || defined TEST_TRACPTO || defined TEST_TRACTOR_GENERAL
/**interpret the IsoLimitFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoLimitFlag(IsoAgLib::IsoLimitFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoNotLimited";
    case 1: return "IsoOperatorLimited";
    case 2: return "IsoLimitedHigh";
    case 3: return "IsoLimitedLow";
    case 6: return "IsoNonRecoverabelFault";
    case 7: return "IsoNotAvailableLimit";
    default: return "there went something wrong!!";
  }
}
#endif

#if defined TEST_TRACPTOSETPOINT
/** dummy function to serve a hitch position for demonstration */
int8_t localGetHitchPos(int8_t t_val) { return ( t_val % 101); }
/** dummy function to serve a output shaft speed set point for demonstration */
int16_t localGetOutputShaft(int16_t t_val) { return ( t_val % 8032 ); }
#endif

int main()
{
  // Initialize CAN-Bus
  getIcanInstance().init (0); // CAN-Bus 0 (with defaulting 250 kbit)

  // Start address claim of the local identity/member
  IsoAgLib::iIdentItem_c c_myIdent (2,     // aui8_indGroup
                                    2,     // aui8_devClass
                                    0,     // aui8_devClassInst
                                    25,    // ab_func
                                    0x7FF, // aui16_manufCode
                                    27);   // aui32_serNo
                                    // further parameters use the default values as given in the constructor

  #ifdef TEST_TRACTOR_LIGHTING
  // configure to send information for lighting on BUS
  getITracLightInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeImplement);
  #endif

  #ifdef TEST_TRACPTOSETPOINT
  //pto set point commands are send from implement!!
  getITracPtoSetPointInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeImplement);
  #endif

  #ifdef TEST_TRACAUX
  getITracAuxInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeImplement);
  #endif

  #ifdef TEST_TRACGUIDANCE
  getITracGuidanceCommandInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeImplement);
  #endif

  #ifdef TEST_TRACMOVESETPOINT
  getITracMoveSetPointInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeImplement);
  #endif

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
      This function can be configured by the #define CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC
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
  #ifdef USE_RS232_FOR_DEBUG
    EXTERNAL_DEBUG_DEVICE << "\n";
  #endif
  #ifdef TEST_TRACTOR_GENERAL
  IsoAgLib::IsoActiveFlag_t t_keySwitch = IsoAgLib::IsoNotAvailable;
  #endif
  #if defined TEST_TRACPTOSETPOINT
  uint8_t hitchPos = 0;
  uint16_t outputShaft = 0;
  #endif
  #ifdef TEST_TRACGUIDANCE
  int curvatureCmd = -100;
  #endif

  int32_t i32_idleTimeSpread = 0;
  while ( iSystem_c::canEn() )
  { // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib
    //use idle_time of Scheduler_c for relax to next call
    i32_idleTimeSpread = getISchedulerInstance().timeEvent();


    #ifdef USE_RS232_FOR_DEBUG
    static int32_t si32_lastDebug = 0;

    if ( ( IsoAgLib::iSystem_c::getTime() / 1000 ) > si32_lastDebug )
    { // it's time to print debug msg
      si32_lastDebug = ( IsoAgLib::iSystem_c::getTime() / 1000 );


      #ifdef TEST_TRACTOR_LIGHTING
      //LIGHTING CLASS TEST FUNCTIONALITY
      EXTERNAL_DEBUG_DEVICE << "\t+++++++++LIGHTING+++++++++\n";
      IsoAgLib::IsoActiveFlag_t temp;

      temp = getITracLightInstance().getCommand(IsoAgLib::daytimeRunning);
      EXTERNAL_DEBUG_DEVICE << "daytime running light:              " <<  getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::alternateHead);
      EXTERNAL_DEBUG_DEVICE << "alternate head light:               " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::lowBeamHead);
      EXTERNAL_DEBUG_DEVICE << "low beam head light:                " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::highBeamHead);
      EXTERNAL_DEBUG_DEVICE << "high beam head light:               " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::frontFog);
      EXTERNAL_DEBUG_DEVICE << "front fog light:                    " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::beacon);
      EXTERNAL_DEBUG_DEVICE << "beacon light:                       " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::rightTurn);
      EXTERNAL_DEBUG_DEVICE << "right turn light:                   " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::leftTurn);
      EXTERNAL_DEBUG_DEVICE << "left turn light:                    " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::backUpLightAlarmHorn);
      EXTERNAL_DEBUG_DEVICE << "back up light and alarm horn:       " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::centerStop);
      EXTERNAL_DEBUG_DEVICE << "center stop light:                  " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::rightStop);
      EXTERNAL_DEBUG_DEVICE << "right stop light:                   " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::leftStop);
      EXTERNAL_DEBUG_DEVICE << "left stop light:                    " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::implClearance);
      EXTERNAL_DEBUG_DEVICE << "implement clearance light:          " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::tracClearance);
      EXTERNAL_DEBUG_DEVICE << "tractor clearance light:            " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::implMarker);
      EXTERNAL_DEBUG_DEVICE << "implement marker light:             " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::tracMarker);
      EXTERNAL_DEBUG_DEVICE << "tractor marker light:               " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::rearFog);
      EXTERNAL_DEBUG_DEVICE << "rear fog light:                     " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::undersideWork);
      EXTERNAL_DEBUG_DEVICE << "underside work light:               " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::rearLowWork);
      EXTERNAL_DEBUG_DEVICE << "rear low work light:                " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::rearHighWork);
      EXTERNAL_DEBUG_DEVICE << "rear high work light:               " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::sideLowWork);
      EXTERNAL_DEBUG_DEVICE << "side low work light:                " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::sideHighWork);
      EXTERNAL_DEBUG_DEVICE << "side high work light:               " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::frontLowWork);
      EXTERNAL_DEBUG_DEVICE << "front low work light:               " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::frontHighWork);
      EXTERNAL_DEBUG_DEVICE << "front high work light:              " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::implOEMOpt2);
      EXTERNAL_DEBUG_DEVICE << "implement OEM option 2 light:       " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::implOEMOpt1);
      EXTERNAL_DEBUG_DEVICE << "implement OEM option 1 light:       " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::implRightForwardWork);
      EXTERNAL_DEBUG_DEVICE << "implement right forward work light: " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::implLeftForwardWork);
      EXTERNAL_DEBUG_DEVICE << "implement left forward work light:  " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::dataMsgReq);
      EXTERNAL_DEBUG_DEVICE << "lighting data message request:      " << getIsoDataReq( static_cast<IsoAgLib::IsoDataReq_t>(temp) ) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::implRightFacingWork);
      EXTERNAL_DEBUG_DEVICE << "implement right facing work light:  " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::implLeftFacingWork);
      EXTERNAL_DEBUG_DEVICE << "implement left facing work light:   " << getIsoActiveFlag(temp) << "\n";

      temp = getITracLightInstance().getCommand(IsoAgLib::implRearWork);
      EXTERNAL_DEBUG_DEVICE << "implement rear work light:          " << getIsoActiveFlag(temp) << "\n";
      #endif

      #ifdef TEST_TRACTOR_MOVING
      //MOVING CLASS TEST FUNCTIONALITY
      EXTERNAL_DEBUG_DEVICE << "\t+++++++++MOVING KLASSE+++++++++\n";
      EXTERNAL_DEBUG_DEVICE << "Distance theoretisch:        " << getITracMoveInstance().distTheor() << "\n";
      EXTERNAL_DEBUG_DEVICE << "Distance real:               " << getITracMoveInstance().distReal() << "\n";
      EXTERNAL_DEBUG_DEVICE << "Geschwindigkeit theoretisch: " << getITracMoveInstance().speedTheor() << "\n";
      EXTERNAL_DEBUG_DEVICE << "Geschwindigkeit real:        " << getITracMoveInstance().speedReal() << "\n";
      EXTERNAL_DEBUG_DEVICE << "selected speed:              " << getITracMoveInstance().selectedSpeed() << "\n";
      EXTERNAL_DEBUG_DEVICE << "selected speed source:       " << getIsoSpeedSourceFlag(getITracMoveInstance().selectedSpeedSource() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "Richtung theoretisch:        " << getIsoDirectionFlag(getITracMoveInstance().directionTheor() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "Richtung real:               " << getIsoDirectionFlag(getITracMoveInstance().directionReal() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "operator Direction:          " << getIsoOperatorDirectionFlag(getITracMoveInstance().operatorDirectionReversed() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "selected distance:           " << getITracMoveInstance().selectedDistance() << "\n";
      EXTERNAL_DEBUG_DEVICE << "selected speed limit status: " << getIsoLimitFlag(getITracMoveInstance().selectedSpeedLimitStatus() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "selected direction:          " << getIsoDirectionFlag(getITracMoveInstance().selectedDirection() ) << "\n";
      #endif

      #ifdef TEST_TRACMOVESETPOINT
      //MOVING SET POINT CLASS TEST FUNCTIONALITY
      getITracMoveSetPointInstance().setSelectedDirectionCmd(IsoAgLib::IsoForward);
      getITracMoveSetPointInstance().setSelectedSpeedSetPointCmd(30547);
      getITracMoveSetPointInstance().setSelectedSpeedSetPointLimit(253);
      #endif

      #ifdef TEST_TRACTOR_GENERAL
      //GENERAL CLASS TEST FUNCTIONALITY

      EXTERNAL_DEBUG_DEVICE << "\t+++++++++GENERAL KLASSE+++++++++\n";
      EXTERNAL_DEBUG_DEVICE << "Hitch front:                  " << static_cast<int16_t>(getITracGeneralInstance().hitchFront()) << "\n";
      EXTERNAL_DEBUG_DEVICE << "Hitch rear:                   " << static_cast<int16_t>(getITracGeneralInstance().hitchRear() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "Hitch front draft:            " << getITracGeneralInstance().hitchFrontDraft() << "\n";
      EXTERNAL_DEBUG_DEVICE << "Hitch rear draft:             " << getITracGeneralInstance().hitchRearDraft() << "\n";
      EXTERNAL_DEBUG_DEVICE << "Hitch front lower link force: " << getITracGeneralInstance().hitchFrontLowerLinkForce() << "\n";
      EXTERNAL_DEBUG_DEVICE << "Hitch rear lower link force:  " << getITracGeneralInstance().hitchRearLowerLinkForce() << "\n";
      EXTERNAL_DEBUG_DEVICE << "Hitch front lower link force: " << getIsoLimitFlag( getITracGeneralInstance().frontHitchPosLimitStatus() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "Hitch rear lower link force:  " << getIsoLimitFlag( getITracGeneralInstance().rearHitchPosLimitStatus() ) << "\n";

      getITracGeneralInstance().setMaintainPowerForImplInWork(IsoAgLib::IsoReadyForFieldWork);
      getITracGeneralInstance().setMaintainPowerForImplInTransport(IsoAgLib::IsoTransported);
      getITracGeneralInstance().setMaintainPowerForImplInPark(IsoAgLib::IsoNotAvailablePark);

      if (t_keySwitch == IsoAgLib::IsoActive && getITracGeneralInstance().keySwitch() == IsoAgLib::IsoInactive)
      { //should be retrieved if key switch changes from ON to OFF
        getITracGeneralInstance().forceMaintainPower(true, true, IsoAgLib::implInTransport);
      }
      t_keySwitch = getITracGeneralInstance().keySwitch();
      EXTERNAL_DEBUG_DEVICE << "Key switch:                  " << getIsoActiveFlag( t_keySwitch ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "Maximum power time:          " << static_cast<int>(getITracGeneralInstance().maxPowerTime()) << "\n";

      #endif

      #ifdef TEST_TIME
        //Time CLASS TEST FUNCTIONALITY
      EXTERNAL_DEBUG_DEVICE << "\t+++++++++ TIME KLASSE +++++++++\n";
      EXTERNAL_DEBUG_DEVICE << "GPS Kalender Local:  " << getITimePosGpsInstance().isCalendarReceived() << "\n";
      EXTERNAL_DEBUG_DEVICE << "GPS Monat UTC:       " << static_cast<int>(getITimePosGpsInstance().monthUtc()) << "\n";
      EXTERNAL_DEBUG_DEVICE << "GPS Tag UTC:         " << static_cast<int>(getITimePosGpsInstance().dayUtc()) << "\n";
      EXTERNAL_DEBUG_DEVICE << "GPS Stunde UTC:      " << static_cast<int>(getITimePosGpsInstance().hourUtc()) << "\n";
      EXTERNAL_DEBUG_DEVICE << "GPS Minute UTC:      " << static_cast<int>(getITimePosGpsInstance().minuteUtc()) << "\n";
      EXTERNAL_DEBUG_DEVICE << "GPS getGpsLongitudeMinute:      " << static_cast<float>(getITimePosGpsInstance().getGpsLongitudeMinute()) << "\n";

      #endif

      #ifdef TEST_TRACPTOSETPOINT
      //TRACTOR PTO SET POINT CLASS TEST FUNCTIONALITY
      getITracPtoSetPointInstance().setFrontHitchPosCmd(localGetHitchPos(hitchPos) );
      getITracPtoSetPointInstance().setFrontPtoSetPointCmd(localGetOutputShaft(outputShaft) );
      getITracPtoSetPointInstance().setFrontPtoEngagement(IsoAgLib::IsoActive);
      getITracPtoSetPointInstance().setFrontPtoMode(IsoAgLib::IsoInactive);
      getITracPtoSetPointInstance().setFrontPtoEconomyMode(IsoAgLib::IsoActive);
      hitchPos += 10;
      outputShaft += 50;
      #endif

      #ifdef TEST_TRACPTO
      //TRACTOR PTO CLASS TEST FUNCTIONALITY
      EXTERNAL_DEBUG_DEVICE << "\t+++++++++ PTO KLASSE +++++++++\n";
      EXTERNAL_DEBUG_DEVICE << "pto front engaged:    " << getIsoActiveFlag( getITracPtoInstance().ptoFrontEngaged() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "pto rear engaged:     " << getIsoActiveFlag( getITracPtoInstance().ptoRearEngaged() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "pto rear:             " << getITracPtoInstance().ptoRear() << "\n";
      EXTERNAL_DEBUG_DEVICE << "pto front:            " << getITracPtoInstance().ptoFront() << "\n";
      EXTERNAL_DEBUG_DEVICE << "pto front mode:       " << getIsoActiveFlag( getITracPtoInstance().ptoFront1000() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "pto rear economy:     " << getIsoActiveFlag( getITracPtoInstance().ptoRearEconomy() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "front pto set point:  " << getITracPtoInstance().frontPtoSetPoint() << "\n";
      EXTERNAL_DEBUG_DEVICE << "rear pto set point:   " << getITracPtoInstance().rearPtoSetPoint() << "\n";

      EXTERNAL_DEBUG_DEVICE << "pto front engagement request status: " << getIsoReqFlag( getITracPtoInstance().frontPtoEngagementReqStatus() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "pto rear engagement request status:  " << getIsoReqFlag( getITracPtoInstance().rearPtoEngagementReqStatus() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "pto front mode request status:       " << getIsoReqFlag( getITracPtoInstance().frontPtoModeReqStatus() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "pto rear mode request status:        " << getIsoReqFlag( getITracPtoInstance().rearPtoModeReqStatus())  << "\n";
      EXTERNAL_DEBUG_DEVICE << "pto front economy mode requ status:  " << getIsoReqFlag( getITracPtoInstance().frontPtoEconomyModeReqStatus() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "pto rear economy mode requ status:   " << getIsoReqFlag( getITracPtoInstance().rearPtoEconomyModeReqStatus() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "front pto shaft speed limit status:  " << getIsoLimitFlag( getITracPtoInstance().frontPtoShaftSpeedLimitStatus() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "rear pto shaft speed limit status:   " << getIsoLimitFlag( getITracPtoInstance().rearPtoShaftSpeedLimitStatus() ) << "\n";
      #endif

      #ifdef TEST_TRACAUX
      //TRACTOR AUXILIARY VALVE CLASS TEST FUNCTIONALITY
      EXTERNAL_DEBUG_DEVICE << "\t+++++++++ AUXILIARY VALVE KLASSE +++++++++\n";
      EXTERNAL_DEBUG_DEVICE << "extended port estimated flow: " << static_cast<int>( getITracAuxInstance().extendPortEstFlow(2) ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "retract port estimated flow:  " << static_cast<int>(getITracAuxInstance().retractPortEstFlow(2) ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "estimated fail save mode:     " << getIsoAuxFlag( getITracAuxInstance().estFailSaveMode(2) )<< "\n";
      EXTERNAL_DEBUG_DEVICE << "estimated valve state:        " << getIsoAuxFlagExtended( getITracAuxInstance().estValveState(2) ) << "\n";

      EXTERNAL_DEBUG_DEVICE << "extended port measured flow:  " << static_cast<int>( getITracAuxInstance().extendPortMeasuredFlow(2) ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "retract port measured flow:   " << static_cast<int>( getITracAuxInstance().retractPortMeasuredFlow(2) ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "extended port pressure:       " << getITracAuxInstance().extendPortPressure(2) << "\n";
      EXTERNAL_DEBUG_DEVICE << "retract port pressure:        " << getITracAuxInstance().retractPortPressure(2) << "\n";
      EXTERNAL_DEBUG_DEVICE << "return port pressure:         " << getITracAuxInstance().returnPortPressure(2) << "\n";
      EXTERNAL_DEBUG_DEVICE << "estimated valve limit status: " << getIsoLimitFlag(getITracAuxInstance().estValveLimitStatus(2) ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "measured valve limit status:  " << getIsoLimitFlag(getITracAuxInstance().measuredValveLimitStatus(2) ) << "\n";

      if ( !getITracAuxInstance().setCmdPortFlow(0, 100) )
        EXTERNAL_DEBUG_DEVICE << "Either command port flow value is not between 0 - 100 or valve number is not betwenn 0 - 15.\n";
      if ( !getITracAuxInstance().setCmdFailSaveMode(2, IsoAgLib::IsoBlock) )
        EXTERNAL_DEBUG_DEVICE << "(cmdFailSaveMode) Valve number is not between 0 - 15.\n";
      if ( !getITracAuxInstance().setCmdValveState(2, IsoAgLib::IsoNotAvailableAuxExt) )
        EXTERNAL_DEBUG_DEVICE << "(cmdValveState) Valve number is not between 0 - 15.\n";
      #endif

      #ifdef TEST_TRACCERT
      EXTERNAL_DEBUG_DEVICE << "\t+++++++++ Zertifikation KLASSE +++++++++\n";
      EXTERNAL_DEBUG_DEVICE << "Certification reference number:        " << getITracCertInstance().certReferenceNumber() << "\n";
      EXTERNAL_DEBUG_DEVICE << "ISOBUS certification year:             " << static_cast<int>(getITracCertInstance().isobusCertYear()) << "\n";
      EXTERNAL_DEBUG_DEVICE << "certification laboratory ID:           " << getITracCertInstance().certLabID() << "\n";
      EXTERNAL_DEBUG_DEVICE << "ISOBUS certification revision:         " << getIsoCertRevisionFlag(getITracCertInstance().isobusCertRevision() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "ISOBUS certification laboratory type:  " << getIsoCertLabTypeFlag(getITracCertInstance().isobusCertLabType() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "Minimum ECU:                           " << getIsoCertTypeFlag(getITracCertInstance().minimumECU() ) << "\n";

      //request for new Certification info
      IsoAgLib::iCanPkgExt_c c_myCanExt;
      c_myCanExt.setIsoPgn(REQUEST_PGN_MSG_PGN);
      c_myCanExt.setMonitorItemForDA(NULL);
      c_myCanExt.setISONameForSA(c_myIdent.isoName());
      c_myCanExt.setIsoPri(3);
      c_myCanExt.setLen(3);
      //set 3 Byte long PGN
      c_myCanExt.setUint16Data(0,ISOBUS_CERTIFICATION);
      c_myCanExt.setUint8Data(2,ISOBUS_CERTIFICATION >> 16);

      getIcanInstance() <<    c_myCanExt ;


      #endif

      #ifdef TEST_TRACGUIDANCE
      //TRACTOR GUIDANCE CLASS TEST FUNCTIONALITY
      EXTERNAL_DEBUG_DEVICE << "\t+++++++++ Guidance KLASSE +++++++++\n";
      EXTERNAL_DEBUG_DEVICE << "estimated curvature command:    " << getITracGuidanceInstance().estCurvature() << "\n";
      EXTERNAL_DEBUG_DEVICE << "request reset command status:   " << getIsoResetFlag(getITracGuidanceInstance().requestResetCmdStatus() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "steering input position status: " << getIsoSteerPosFlag(getITracGuidanceInstance().steeringInputPosStatus() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "steering system readiness:      " << getIsoSteerReadinessFlag(getITracGuidanceInstance().steeringSystemReadiness() ) << "\n";
      EXTERNAL_DEBUG_DEVICE << "mechanical system logout:       " << getIsoActiveFlag(getITracGuidanceInstance().mechanicalSystemLogout() ) << "\n";

      curvatureCmd += 3;
      getITracGuidanceCommandInstance().setCurvatureCmd(curvatureCmd);
      getITracGuidanceCommandInstance().setCurvatureCmdStatus(IsoAgLib::IsoIntendedToSteer);
      #endif
    }
    #endif


    // The following sleep mechanism uses idle_time of  scheduler reported back
    // no need to sleep on single-task systems
    #ifdef SYSTEM_PC
      #ifdef WIN32
        if ( i32_idleTimeSpread > 0 ) Sleep(i32_idleTimeSpread);
      #else
        if ( i32_idleTimeSpread > 0 ) IsoAgLib::iCanIo_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
      #endif
    #endif



  }
  return 1;
}
