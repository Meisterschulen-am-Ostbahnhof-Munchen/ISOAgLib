/* *************************************************************************
                          1_2_WriteIso.cpp - tutorial for pure ISO address claim
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
/** \example 1_2_WriteIso.cpp
 * This tutorial shall provide a simple base program, which writes some
 * regularly sent base data, as it acts as a tractor.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Send periodically some base data, where application can set
 *    data independent from required timing -> ISO<i><sub>AgLib</sub></i>
 *    guarantees to send all data in demanded time period
 * <ul>
 *  <li>Data storage class IsoAgLib::iBase_c
 *  <li>configure the sending with IsoAgLib::iBase_c::config()
 *  <li>Update data with IsoAgLib::iBase_c::setDistTheor() , IsoAgLib::iBase_c::setDistReal() ,
 *    IsoAgLib::iBase_c::setSpeedTheor() , IsoAgLib::iBase_c::setSpeedReal()
 * </ul>
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
 *     conf_1_2_WriteIso from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_1_2_WriteIso</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 1_2_WriteIso</i> to go to the subdirectory 1_2_WriteIso
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 1_2_WriteIso in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist__1_2_WriteIso.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_1_2_WriteIso</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec1_2_WriteIso__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_no_card__rte
 *		<li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_rte__rte
 *		<li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_A1__rte
 *		<li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_pcan__rte
 *    <li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_no_card__simulating
 *		<li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_rte__simulating
 *		<li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_A1__simulating
 *		<li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_pcan__simulating
 *    <li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_no_card__sys
 *		<li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_rte__sys
 *		<li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_A1__sys
 *		<li> \ref PrjSpec1_2_WriteIso__pc_linux__msq_server_pcan__sys
 *    <li> \ref PrjSpec1_2_WriteIso__pc_linux__simulating__rte
 *    <li> \ref PrjSpec1_2_WriteIso__pc_linux__simulating__simulating
 *    <li> \ref PrjSpec1_2_WriteIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpec1_2_WriteIso__pc_win32__can_server_sock_no_card__simulating
 *		<li> \ref PrjSpec1_2_WriteIso__pc_win32__can_server_sock_sontheim__simulating
 *		<li> \ref PrjSpec1_2_WriteIso__pc_win32__can_server_sock_vector_canlib__simulating
 *    <li> \ref PrjSpec1_2_WriteIso__pc_win32__can_server_sock_vector_xl__simulating
 *    <li> \ref PrjSpec1_2_WriteIso__pc_win32__can_server_sock_no_card__sys
 *		<li> \ref PrjSpec1_2_WriteIso__pc_win32__can_server_sock_sontheim__sys
 *		<li> \ref PrjSpec1_2_WriteIso__pc_win32__can_server_sock_vector_canlib__sys
 *    <li> \ref PrjSpec1_2_WriteIso__pc_win32__can_server_sock_vector_xl__sys
 *    <li> \ref PrjSpec1_2_WriteIso__pc_win32__simulating__simulating
 *    <li> \ref PrjSpec1_2_WriteIso__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpec1_2_WriteIso__esx__sys__sys
 *    <li> \ref PrjSpec1_2_WriteIso__imi__sys__sys
 *    <li> \ref PrjSpec1_2_WriteIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists1_2_WriteIso__pc_linux__simulating__simulating for needed files
 * ( filelist__1_2_WriteIso.txt ),
 *  with \ref SrcList1_2_WriteIso__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList1_2_WriteIso__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_no_card__rte
 *		<li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_rte__rte
 *		<li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_A1__rte
 *		<li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_pcan__rte
 *    <li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_no_card__simulating
 *		<li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_rte__simulating
 *		<li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_A1__simulating
 *		<li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_pcan__simulating
 *    <li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_no_card__sys
 *		<li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_rte__sys
 *		<li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_A1__sys
 *		<li> \ref FileLists1_2_WriteIso__pc_linux__msq_server_pcan__sys
 *    <li> \ref FileLists1_2_WriteIso__pc_linux__simulating__rte
 *    <li> \ref FileLists1_2_WriteIso__pc_linux__simulating__simulating
 *    <li> \ref FileLists1_2_WriteIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileLists1_2_WriteIso__pc_win32__can_server_sock_no_card__simulating
 *		<li> \ref FileLists1_2_WriteIso__pc_win32__can_server_sock_sontheim__simulating
 *		<li> \ref FileLists1_2_WriteIso__pc_win32__can_server_sock_vector_canlib__simulating
 *    <li> \ref FileLists1_2_WriteIso__pc_win32__can_server_sock_vector_xl__simulating
 *    <li> \ref FileLists1_2_WriteIso__pc_win32__can_server_sock_no_card__sys
 *		<li> \ref FileLists1_2_WriteIso__pc_win32__can_server_sock_sontheim__sys
 *		<li> \ref FileLists1_2_WriteIso__pc_win32__can_server_sock_vector_canlib__sys
 *    <li> \ref FileLists1_2_WriteIso__pc_win32__can_server_sock_vector_xl__sys
 *    <li> \ref FileLists1_2_WriteIso__pc_win32__simulating__simulating
 *    <li> \ref FileLists1_2_WriteIso__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileLists1_2_WriteIso__esx__sys__sys
 *    <li> \ref FileLists1_2_WriteIso__imi__sys__sys
 *    <li> \ref FileLists1_2_WriteIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/IsoAgLib/isoaglib_config.h
 * if the \#define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_1_2_WriteIso ( see also at \ref PrjConfig1_2_WriteIso ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/IsoAgLib/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_1_2_WriteIso.h
#endif

/** set the following define, if the lookup result shall be sent via RS232 */
#define USE_RS232_FOR_DEBUG

#ifdef USE_RS232_FOR_DEBUG
  #include <supplementary_driver/driver/rs232/irs232io_c.h>
#endif

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/** set the following defines if to test one or more of the base data*/
//the defines are set with the update_makefile skript if in the corresponding config_1_2_WriteIso
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

/* include some needed util headers */
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>

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
#ifdef TEST_TRACAUX
  #include <IsoAgLib/comm/Part7_ApplicationLayer/ext/itracaux_c.h>
#endif
#ifdef TEST_TRACPTO
  #include <IsoAgLib/comm/Part7_ApplicationLayer/itracpto_c.h>
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


/** define channel to write:
 - provide data for tutorial example 1_0_ReadIso which reads on channel 0
    -> select here the corresponding channel in your configuration
       ( e.g. select channel 1 for a 2-channel CAN-Hardware )
  - simply write on channel 0, when no connection needed, or the connection is
    realized with another external connection
*/
static const int32_t cui32_canChannel = 0;

#if defined TEST_TRACTOR_MOVING || defined TEST_TRACMOVESETPOINT
/*************DUMMY FUNCTIONALITY FOR MOVING *****************/
/** dummy function to serve a real speed for the demonstration */
int32_t localGetRealSpeed() { return (iSystem_c::getTime()*34/1000);; }
/** dummy function to serve a theor speed for the demonstration */
int32_t localGetTheorSpeed() { return (iSystem_c::getTime()*40/1000); }
/** dummy function to serve a real distance for the demonstration */
int32_t localGetRealDist() { return (iSystem_c::getTime()*localGetRealSpeed()/1000); }
/** dummy function to serve a teor distance for the demonstration */
int32_t localGetTheorDist() { return (iSystem_c::getTime()*localGetTheorSpeed()/1000);}

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
#endif

#ifdef TEST_TRACAUX
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
#endif

#ifdef TEST_TRACGUIDANCE
/**interpret IsoSteerFlag values as strings when output on console*/
STL_NAMESPACE::string IsoSteerFlag(IsoAgLib::IsoSteerFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoNotIntendedToSteer";
    case 1: return "IsoIntendedToSteer";
    case 2: return "IsoReserved";
    case 3: return "IsoNotAvailableSteer";
    default: return "there went something wrong!!";
  }
}
#endif

#ifdef TEST_TRACTOR_GENERAL
/**interpret IsoImplWorkFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoImplWorkFlag(IsoAgLib::IsoImplWorkFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoNotReadyForFieldWork";
    case 1: return "IsoReadyForFieldWork";
    case 2: return "IsoErrorWork";
    case 3: return "IsoNotAvailableWork";
    default: return "there went something wrong!!";
  }
}

/**interpret IsoImplTransportFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoImplTransportFlag(IsoAgLib::IsoImplTransportFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoNotTransported";
    case 1: return "IsoTransported";
    case 2: return "IsoErrorTransport";
    case 3: return "IsoNotAvailableTransport";
    default: return "there went something wrong!!";
  }
}

/**interpret IsoImplParkFlag values as strings when output on console*/
STL_NAMESPACE::string getIsoImplParkFlag(IsoAgLib::IsoImplParkFlag_t t_val)
{
  switch ( t_val )
  {
    case 0: return "IsoNotDisconnected";
    case 1: return "IsoDisconnect";
    case 2: return "IsoErrorPark";
    case 3: return "IsoNotAvailablePark";
    default: return "there went something wrong!!";
  }
}
#endif

#if defined TEST_TRACPTO
/** dummy function to serve a pto speed for demonstration */
uint16_t localGetPtoOutputShaft(uint16_t t_val) { return ( t_val % 8032); }
#endif
#if defined TEST_TRACTOR_MOVING
/** dummy function to serve a pto speed for demonstration */
int32_t localGetSpeedReal(int32_t t_val) { return ( t_val ); }
/** dummy function to serve a pto speed for demonstration */
int32_t localGetSpeedTheor(int32_t t_val) { return ( t_val ); }
#endif

int main()
{
  // Initialize CAN-Bus
  getIcanInstance().init (cui32_canChannel); // CAN-Bus "cui32_canChannel" (with defaulting 250 kbit)

  // Start address claim of the local identity/member
  iIdentItem_c c_myIdent (2,     // aui8_indGroup
                          1,     // aui8_devClass
                          0,     // aui8_devClassInst
                          25,    // ab_func
                          0x7FF, // aui16_manufCode
                          27);   // aui32_serNo
                          // further parameters use the default values as given in the constructor

  #ifdef TEST_TRACTOR_LIGHTING
  // configure send information for lighting on BUS
  getITracLightInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor );  //tractor mode
  #endif

  #ifdef TEST_TRACTOR_MOVING
  getITracMoveInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
  #endif

  #ifdef TEST_TRACTOR_GENERAL
  getITracGeneralInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
  #endif

  #ifdef TEST_TIME
  getITimePosGpsInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
  getITimePosGpsInstance().configGps(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
  #endif

  #ifdef TEST_TRACAUX
  getITracAuxInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
  #endif

  #ifdef TEST_TRACPTO
  getITracPtoInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
  #endif

  #ifdef TEST_TRACPTOSETPOINT
  getITracPtoSetPointInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
  #endif

  #ifdef TEST_TRACCERT
  getITracCertInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
  #endif

  #ifdef TEST_TRACGUIDANCE
  getITracGuidanceInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
  getITracGuidanceCommandInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
  #endif

  #ifdef TEST_TRACMOVESETPOINT
  getITracMoveSetPointInstance().config(&c_myIdent.isoName(), IsoAgLib::IdentModeTractor);
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
      This function can be configured by the \#define CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC
      in isoaglib_config.h to ignore short CAN_EN loss.
    - This explicit control of power state without automatic powerdown on CanEn loss
      can be controled with the central config define
      \#define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownByExplcitCall
      or
      \#define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownOnCanEnLoss
      in the header xgpl_src/IsoAgLib/isoaglib_config.h
    - This can be also controlled during runtime with the function call:
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownByExplcitCall )
      or
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownOnCanEnLoss )
  */
  #if defined TEST_TRACPTO
  uint16_t ui16_outputShaft = 0;
  uint16_t ui16_outputShaftSetPoint = 0;
  #endif
  #if defined TEST_TRACTOR_MOVING
  int32_t i32_speedReal = -5000;
  int32_t i32_speedTheor = -2000;
  uint32_t ui32_tempTheorDist = 0;
  uint32_t ui32_tempRealDist = 0;
  #endif
  #if defined TEST_TRACTOR_GENERAL || defined TEST_TRACTOR_MOVING
  uint32_t count = 0;
  #endif
  #ifdef TEST_TRACAUX
  int extendPortFlow = -125;
  #endif
  #ifdef TEST_TRACGUIDANCE
  int estCurvature = 0;
  #endif
  #ifdef TEST_TRACTOR_LIGHTING
  int lightCount = 0;
  #endif

  int32_t i32_idleTimeSpread = 0;
  while ( iSystem_c::canEn() )
  { // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib
    i32_idleTimeSpread = IsoAgLib::getISchedulerInstance().timeEvent();


    #ifdef USE_RS232_FOR_DEBUG
      static int32_t si32_lastDebug = 0;
      if ( ( IsoAgLib::iSystem_c::getTime() / 1000 ) > si32_lastDebug )
      { // it's time to print debug msg
        si32_lastDebug = ( IsoAgLib::iSystem_c::getTime() / 1000 );

        #ifdef TEST_TRACTOR_LIGHTING
        //LIGHTING CLASS TEST FUNCTIONALITY
        EXTERNAL_DEBUG_DEVICE << "\n";
        EXTERNAL_DEBUG_DEVICE << "Number of requesting implements. " << getITracLightInstance().getCount() << "\n";

        for (int i = 0; i < getITracLightInstance().getCount(); i++)
        {
          EXTERNAL_DEBUG_DEVICE << "State of high beam head: " << getITracLightInstance().getByIndex(i, IsoAgLib::highBeamHead) << "\n";
        }
        //LIGHTING CLASS TEST FUNCTIONALITY
        getITracLightInstance().setCommand(IsoAgLib::daytimeRunning, IsoAgLib::IsoActive);
        getITracLightInstance().setCommand(IsoAgLib::alternateHead, IsoAgLib::IsoNotAvailable);
        getITracLightInstance().setCommand(IsoAgLib::lowBeamHead, IsoAgLib::IsoError);
        getITracLightInstance().setCommand(IsoAgLib::highBeamHead, IsoAgLib::IsoActive);

        getITracLightInstance().setCommand(IsoAgLib::frontFog, IsoAgLib::IsoNotAvailable);
        getITracLightInstance().setCommand(IsoAgLib::beacon, IsoAgLib::IsoNotAvailable);
        getITracLightInstance().setCommand(IsoAgLib::rightTurn, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::leftTurn, IsoAgLib::IsoInactive);

        getITracLightInstance().setCommand(IsoAgLib::backUpLightAlarmHorn, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::centerStop, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::rightStop, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::leftStop, IsoAgLib::IsoInactive);

        getITracLightInstance().setCommand(IsoAgLib::implClearance, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::tracClearance, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::implMarker, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::tracMarker, IsoAgLib::IsoInactive);

        getITracLightInstance().setCommand(IsoAgLib::rearFog, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::undersideWork, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::rearLowWork, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::rearHighWork, IsoAgLib::IsoInactive);

        getITracLightInstance().setCommand(IsoAgLib::sideLowWork, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::sideHighWork, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::frontLowWork, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::frontHighWork, IsoAgLib::IsoInactive);

        getITracLightInstance().setCommand(IsoAgLib::implOEMOpt2, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::implOEMOpt1, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::implRightForwardWork, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::implLeftForwardWork, IsoAgLib::IsoInactive);

        if ( (lightCount % 4) == 0 )
          getITracLightInstance().setCommand(IsoAgLib::dataMsgReq, IsoAgLib::IsoActive);
        else
          getITracLightInstance().setCommand(IsoAgLib::dataMsgReq, IsoAgLib::IsoInactive);

        lightCount++;

        getITracLightInstance().setCommand(IsoAgLib::implRightFacingWork, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::implLeftFacingWork, IsoAgLib::IsoInactive);
        getITracLightInstance().setCommand(IsoAgLib::implRearWork, IsoAgLib::IsoInactive);

     //   getITracLightInstance().sendMessage();
        #endif

        #ifdef TEST_TRACTOR_MOVING
        //MOVING CLASS TEST FUNCTIONALITY
        i32_speedReal = i32_speedReal +50;
        i32_speedTheor = i32_speedTheor + 100;
        ui32_tempTheorDist += abs(i32_speedReal);
        ui32_tempRealDist += abs(i32_speedTheor);

       // if(count > 15)
        if (count > 10 && count < 20)
          getITracMoveInstance().setSpeedReal(localGetSpeedReal(i32_speedReal));
        else {
          getITracMoveInstance().setSpeedReal(localGetSpeedReal(0xFF00));
          getITracMoveInstance().setDistReal(0xFFFFFFFF);
        }

        getITracMoveInstance().setSpeedTheor(localGetSpeedTheor(i32_speedTheor));

        if (i32_speedReal < 0)
          getITracMoveInstance().setDirectionReal(IsoAgLib::IsoReverse);
        else
          getITracMoveInstance().setDirectionReal(IsoAgLib::IsoForward);

        if (i32_speedTheor < 0)
          getITracMoveInstance().setDirectionTheor(IsoAgLib::IsoReverse);
        else
          getITracMoveInstance().setDirectionTheor(IsoAgLib::IsoForward);

        getITracMoveInstance().setDistTheor(ui32_tempTheorDist);
        getITracMoveInstance().setOperatorDirectionReversed(IsoAgLib::IsoReversed);

//         if (count < 5 )
//           getITracMoveInstance().setSelectedSpeed(count);
//         else
//           getITracMoveInstance().setSelectedSpeed(256);
        //getITracMoveInstance().setSelectedSpeedSource(IsoAgLib::IsoNavigationBasedSpeed);
        count++;
        #endif

        #ifdef TEST_TRACMOVESETPOINT
        //MOVING SET POINT CLASS TEST FUNCTIONALITY
        EXTERNAL_DEBUG_DEVICE << "\t+++++++++ Moving SET POINT KLASSE +++++++++\n";
        EXTERNAL_DEBUG_DEVICE << "selected direction command:       " << getIsoDirectionFlag(getITracMoveSetPointInstance().selectedDirectionCmd() ) << "\n";
        EXTERNAL_DEBUG_DEVICE << "selected speed set point command: " << getITracMoveSetPointInstance().selectedSpeedSetPointCmd() << "\n";
        EXTERNAL_DEBUG_DEVICE << "selected speed set point limit:   " << getITracMoveSetPointInstance().selectedSpeedSetPointLimit() << "\n";
        #endif

        #ifdef TEST_TRACTOR_GENERAL
        //GENERAL CLASS TEST FUNCTIONALITY
        uint8_t hitchRear = 100;
        getITracGeneralInstance().setHitchFront(10);
        getITracGeneralInstance().setHitchRear(hitchRear);
        getITracGeneralInstance().setHitchFrontDraft(250);
        getITracGeneralInstance().setHitchRearDraft(200);
        getITracGeneralInstance().setHitchFrontLowerLinkForce(50);
        getITracGeneralInstance().setHitchRearLowerLinkForce(60);
        getITracGeneralInstance().setFrontHitchPosLimitStatus(IsoAgLib::IsoOperatorLimited);
        getITracGeneralInstance().setRearHitchPosLimitStatus(IsoAgLib::IsoLimitedLow);

        getITracGeneralInstance().setMaxPowerTime(5);
        if (count % 3 == 1)
          getITracGeneralInstance().setKeySwitch(IsoAgLib::IsoActive);
        if ( (count % 5) == 1)
        {
          getITracGeneralInstance().setKeySwitch(IsoAgLib::IsoInactive);

          EXTERNAL_DEBUG_DEVICE << "Key switch state changed from ON to OFF.\n";
          EXTERNAL_DEBUG_DEVICE << "Maintain ecu power for 2 seconds:      " << getITracGeneralInstance().maintainEcuPower() << "\n";
          EXTERNAL_DEBUG_DEVICE << "Maintain actuatur power for 2 seconds: " << getITracGeneralInstance().maintainActuatorPower() << "\n";
          EXTERNAL_DEBUG_DEVICE << "Implement in work:                     " << getIsoImplWorkFlag(getITracGeneralInstance().maintainPowerForImplInWork() ) << "\n";
          EXTERNAL_DEBUG_DEVICE << "Implement in transport:                " << getIsoImplTransportFlag(getITracGeneralInstance().maintainPowerForImplInTransport()) << "\n";
          EXTERNAL_DEBUG_DEVICE << "Implement in park:                     " << getIsoImplParkFlag(getITracGeneralInstance().maintainPowerForImplInPark()) << "\n";
        }
        count++;
        #endif

        #ifdef TEST_TIME
        int16_t year = 1990;
        uint8_t month = 4, day = 15, hour = 12, minute = 8, second = 22;
        getITimePosGpsInstance().setCalendarLocal(year, month, day, hour, minute, second);
        //getITimePosGpsInstance().setDateUtc(year, month, day);
        //getITimePosGpsInstance().setTimeUtc(hour, minute, second, msec);
        #endif

        #ifdef TEST_TRACPTOSETPOINT
        //TRACTOR PTO SET POINT CLASS TEST FUNCTIONALITY
        EXTERNAL_DEBUG_DEVICE << "\t+++++++++ Pto Set Point KLASSE +++++++++\n";
        EXTERNAL_DEBUG_DEVICE << "Front hitch pos cmd:  "     << static_cast<int>(getITracPtoSetPointInstance().frontHitchPosCmd() ) << "\n";
        EXTERNAL_DEBUG_DEVICE << "Front pto set point cmd:  " << getITracPtoSetPointInstance().frontPtoSetPointCmd() << "\n";
        EXTERNAL_DEBUG_DEVICE << "Front pto engagement:  "    << getITracPtoSetPointInstance().frontPtoEngagement() << "\n";
        EXTERNAL_DEBUG_DEVICE << "Front pto mode:  "          << getITracPtoSetPointInstance().frontPtoMode() << "\n";
        EXTERNAL_DEBUG_DEVICE << "Front pto economy mode:  "  << getITracPtoSetPointInstance().frontPtoEconomyMode() << "\n";
        #endif

        #ifdef TEST_TRACPTO
        ui16_outputShaft = ui16_outputShaft +10;
        ui16_outputShaftSetPoint = ui16_outputShaftSetPoint + 50;
        getITracPtoInstance().setPtoFront(localGetPtoOutputShaft(ui16_outputShaft));
        getITracPtoInstance().setPtoRear(localGetPtoOutputShaft(ui16_outputShaftSetPoint));
        getITracPtoInstance().setPtoFrontEngaged(IsoAgLib::IsoActive);
        getITracPtoInstance().setPtoRearEngaged(IsoAgLib::IsoActive);
        getITracPtoInstance().setPtoFront1000(IsoAgLib::IsoInactive);
        getITracPtoInstance().setFrontPtoSetPoint(1000);
        getITracPtoInstance().setPtoRearEconomy(IsoAgLib::IsoError);
        getITracPtoInstance().setFrontPtoEngagementReqStatus(IsoAgLib::IsoExternalReqAccepted);
        getITracPtoInstance().setRearPtoEngagementReqStatus(IsoAgLib::IsoControlOverride);
        getITracPtoInstance().setFrontPtoModeReqStatus(IsoAgLib::IsoNotAvailableReq);
        getITracPtoInstance().setRearPtoModeReqStatus(IsoAgLib::IsoErrorReq);
        getITracPtoInstance().setFrontPtoEconomyModeReqStatus(IsoAgLib::IsoExternalReqAccepted);
        getITracPtoInstance().setRearPtoEconomyModeReqStatus(IsoAgLib::IsoErrorReq);
        getITracPtoInstance().setFrontPtoShaftSpeedLimitStatus(IsoAgLib::IsoLimitedHigh);
        getITracPtoInstance().setRearPtoShaftSpeedLimitStatus(IsoAgLib::IsoNotLimited);
        #endif

        #ifdef TEST_TRACAUX
        //TRACTOR AUXILIARY VALVE CLASS TEST FUNCTIONALITY
        extendPortFlow += 2;
        EXTERNAL_DEBUG_DEVICE << "\t+++++++++ AUXILIARY VALVE KLASSE +++++++++\n";
        if ( !getITracAuxInstance().setExtendPortEstFlow(2, extendPortFlow) )
          EXTERNAL_DEBUG_DEVICE << "Either extend port flow estimated value is not between -125 to 125 or valve number is not betwenn 0 - 15.\n";
        if ( !getITracAuxInstance().setRetractPortEstFlow(2, 125) )
          EXTERNAL_DEBUG_DEVICE << "Either retract port flow estimated value is not between -125 to 125 or valve number is not betwenn 0 - 15.\n";
        if ( !getITracAuxInstance().setEstFailSaveMode(2, IsoAgLib::IsoFloat) )
          EXTERNAL_DEBUG_DEVICE << "(estFailSaveMode) Valve number is not between 0 - 15.\n";
        if ( !getITracAuxInstance().setEstValveState(2, IsoAgLib::IsoErrorAuxExt) )
          EXTERNAL_DEBUG_DEVICE << "(estValveState) Valve number is not between 0 - 15.\n";
        if ( !getITracAuxInstance().setExtendPortMeasuredFlow(2, 125) )
          EXTERNAL_DEBUG_DEVICE << "Either extend port flow measured value is not between -125 to 125 or valve number is not betwenn 0 - 15.\n";
        if ( !getITracAuxInstance().setRetractPortMeasuredFlow(2, 100) )
          EXTERNAL_DEBUG_DEVICE << "Either retract port flow measured value is not between -125 to 125 or valve number is not betwenn 0 - 15.\n";
        if ( !getITracAuxInstance().setExtendPortPressure(2, 321000) )
          EXTERNAL_DEBUG_DEVICE << "Either extend port pressure value is not between 0 - 321275 or valve number is not betwenn 0 - 15.\n";
        if ( !getITracAuxInstance().setRetractPortPressure(2, 13) )
          EXTERNAL_DEBUG_DEVICE << "Either retract port pressure value is not between 0 - 321275 or valve number is not betwenn 0 - 15.\n";
        if ( !getITracAuxInstance().setReturnPortPressure(2, 0) )
          EXTERNAL_DEBUG_DEVICE << "Either return port pressure value is not between 0 - 4000 or valve number is not betwenn 0 - 15.\n";
        if ( !getITracAuxInstance().setEstValveLimitStatus(2, IsoAgLib::IsoLimitedLow) )
          EXTERNAL_DEBUG_DEVICE << "(estValveLimitStatus) Valve number is not between 0 - 15.\n";
        if ( !getITracAuxInstance().setMeasuredValveLimitStatus(2, IsoAgLib::IsoNotLimited) )
          EXTERNAL_DEBUG_DEVICE << "(measuredValveLimitStatus) Valve number is not between 0 - 15.\n";

        EXTERNAL_DEBUG_DEVICE << "command port flow:  "      << static_cast<int>( getITracAuxInstance().cmdPortFlow(0) ) << "\n";
        EXTERNAL_DEBUG_DEVICE << "command fail save mode:  " << getIsoAuxFlag( getITracAuxInstance().cmdFailSaveMode(2) ) << "\n";
        EXTERNAL_DEBUG_DEVICE << "command valve state:  "    << getIsoAuxFlagExtended( getITracAuxInstance().cmdValveState(2) ) << "\n";
        #endif

        #ifdef TEST_TRACCERT
        //TRACTOR CERTIFICATION CLASS TEST FUNCTIONALITY
        getITracCertInstance().setCertReferenceNumber(1530);
        getITracCertInstance().setCertLabID(650);
        getITracCertInstance().setIsobusCertYear(2006) ;
        getITracCertInstance().setIsobusCertRevision(IsoAgLib::IsoFourthRev);
        getITracCertInstance().setIsobusCertLabType(IsoAgLib::IsoEUCert);
        getITracCertInstance().setMinimumECU(IsoAgLib::IsoCert);
        #endif

        #ifdef TEST_TRACGUIDANCE
        //TRACTOR GUIDANCE CLASS TEST FUNCTIONALITY
        EXTERNAL_DEBUG_DEVICE << "\t+++++++++ Guidance KLASSE +++++++++\n";
        EXTERNAL_DEBUG_DEVICE << "curvature command:             " << getITracGuidanceCommandInstance().curvatureCmd() << "\n";
        EXTERNAL_DEBUG_DEVICE << "request reset command status:  " << IsoSteerFlag( getITracGuidanceCommandInstance().curvatureCmdStatus() ) << "\n";
        getITracGuidanceCommandInstance().setCommander(129);

        estCurvature += 2;
        getITracGuidanceInstance().setEstCurvature(estCurvature);
        getITracGuidanceInstance().setRequestResetCmdStatus(IsoAgLib::IsoResetRequired);
        getITracGuidanceInstance().setSteeringInputPosStatus(IsoAgLib::IsoCorrectPos);
        getITracGuidanceInstance().setSteeringSystemReadiness(IsoAgLib::IsoSystemNotReady);
        getITracGuidanceInstance().setMechanicalSystemLogout(IsoAgLib::IsoActive);
        #endif
     }
    #endif

    if ( i32_idleTimeSpread > 0 ) IsoAgLib::iCanIo_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );

  }
  return 1;
}
