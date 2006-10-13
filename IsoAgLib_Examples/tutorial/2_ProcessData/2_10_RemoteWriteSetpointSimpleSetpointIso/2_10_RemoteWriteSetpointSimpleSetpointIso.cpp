/* *************************************************************************
                          2_10_RemoteWriteSetpointSimpleSetpointIso.cpp - tutorial for pure ISO address claim
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
/** \example 2_10_RemoteWriteSetpointSimpleSetpointIso.cpp
 * This tutorial shall provide a simple base program, which creates some
 * remote process data, which are used to control the remote device.
 * This example uses the simple version of setpoint control, where no explicit
 * detection of accept or deny is possible.
 * Demonstrate optional usage of handler class with a method which is automatically
 * called on each setpoint response receive.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Create some remote standard process data and request measurement data from remote device
 * <ul>
 *  <li>Standard remote process data class IsoAgLib::iProcDataRemoteSimpleSetpoint_c
 *  <li>Use constructor IsoAgLib::iProcDataRemoteSimpleSetpoint_c::iProcDataRemoteSimpleSetpoint_c to create variable with defined property
 *  <li>Use IsoAgLib::iProcDataRemoteSimpleSetpoint_c::init to define process data properties independent from instantiation ( needed especially for arrays of process data )
 *  <li>Use IsoAgLib::iProcDataRemoteSimpleSetpoint_c::setSetpointMasterVal to send setpoint
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
 *     conf_2_10_RemoteWriteSetpointSimpleSetpointIso from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_2_10_RemoteWriteSetpointSimpleSetpointIso</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 2_10_RemoteWriteSetpointSimpleSetpointIso</i> to go to the subdirectory 2_10_RemoteWriteSetpointSimpleSetpointIso
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 2_10_RemoteWriteSetpointSimpleSetpointIso in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-2_10_RemoteWriteSetpointSimpleSetpointIso.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_2_10_RemoteWriteSetpointSimpleSetpointIso</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__rte__rte
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__rte__simulating
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__rte__sys
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__simulating__rte
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__simulating__simulating
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__vector_canlib__simulating
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__vector_canlib__sys
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__simulating__simulating
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__esx__sys__sys
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__imi__sys__sys
 *    <li> \ref PrjSpec2_10_RemoteWriteSetpointSimpleSetpointIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__simulating__simulating for needed files
 * ( filelist-2_10_RemoteWriteSetpointSimpleSetpointIso-doxygen_import.txt ),
 *  with \ref SrcList2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__rte__rte
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__rte__simulating
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__rte__sys
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__simulating__rte
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__simulating__simulating
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__vector_canlib__simulating
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__vector_canlib__sys
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__simulating__simulating
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__esx__sys__sys
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__imi__sys__sys
 *    <li> \ref FileLists2_10_RemoteWriteSetpointSimpleSetpointIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/Application_Config/isoaglib_config.h
 * if the #define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_2_10_RemoteWriteSetpointSimpleSetpointIso ( see also at \ref PrjConfig2_10_RemoteWriteSetpointSimpleSetpointIso ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/Application_Config/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_2_10_RemoteWriteSetpointSimpleSetpointIso.h
#endif

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/* include some needed util headers */
//#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisoname_c.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
#include <supplementary_driver/driver/rs232/irs232io_c.h>

/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisomonitor_c.h>
#include <IsoAgLib/comm/Process/proc_c.h>
#include <IsoAgLib/comm/Process/Remote/SimpleSetpoint/iprocdataremotesimplesetpoint_c.h>

// if following define is active, the version with HANDLER usage is compiled
// the handler allows immediate reaction on each received value - no polling is needed
#define USE_PROC_HANDLER

#ifdef USE_PROC_HANDLER
  #include <IsoAgLib/comm/Process/processdatachangehandler_c.h>
#endif

#ifdef WIN32
  #define LOG_INFO std::cout
  #include <iostream>
#else
  #define LOG_INFO getIrs232Instance()
#endif

// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;

/** define channel to write:
 - access data from tutorial examples 2_0_xy to 2_6_nm which listen to requests on channel 0
    -> select here the corresponding channel in your configuration
       ( e.g. select channel 1 for a 2-channel CAN-Hardware )
  - simply write on channel 0, when no connection needed, or the connection is
    realized with another external connection
*/
static const int32_t cui32_canChannel = 1;


/** dummy function which can be called from some other module to control the remote work state */
void controlRemoteWorkState( bool rb_isWorking );
/** dummy function which can be called from some other module to control the remote application rate */
void controlRemoteApplicationRate( int32_t ri32_applicationRate );

/** last wanted setpoint for work state */
uint8_t ui8_mySetpointWorkState = 0;
/** last wanted setpoint for application rate */
int32_t i32_mySetpointApplicationRate = 0;

/** dummy function which is called upon reaction from to be controlled remote device
    - for work state
  */
void indicateRemoteWorkStateResponse( bool rb_work )
{ // perform some reaction based on result
  if ( rb_work ) LOG_INFO << "Remote ECU switched to WORK-STATE\r\n";
  else LOG_INFO << "Remote ECU switched to NO-WORK\r\n";
}
/** dummy function which is called upon reaction from to be controlled remote device
    - for Application Rate
  */
void indicateRemoteApplicationRateResponse( bool rb_accepted )
{ // perform some reaction based on result
  if ( rb_accepted ) LOG_INFO << "FINE - Remote ECU accepted our last setpoint\r\n";
  else LOG_INFO << "SAD - Remote ECU didn't accept our last setpoint\r\n";
}

#ifdef USE_PROC_HANDLER

const uint8_t cui8_indexWorkState = 0;
const uint8_t cui8_indexApplicationRate = 1;
IsoAgLib::iProcDataRemoteSimpleSetpoint_c arr_procData[2];

class MyProcDataHandler_c : public IsoAgLib::ProcessDataChangeHandler_c
{
  public:
    /** react on received setpoint ACK or NACK upon previous setpoint set for remote process data
      * (remote system which manages the process data, local or other system sent previously a
      *  new setpoint; commanded manager of process data sent the response with ACK/NACK)
      * @param rc_src general event source class, which provides conversion functions to get needed event source class
      * @param ri32_val new value, which caused the event (for immediate access)
      * @param rc_callerISOName ISOName of calling device - i.e. which sent new setpoint
      * @return true -> handler class reacted on change event
      */
    virtual bool processSetpointResponse( EventSource_c rc_src, int32_t ri32_val, const iISOName_c& rc_callerISOName );
};

bool MyProcDataHandler_c::processSetpointResponse( EventSource_c rc_src, int32_t ri32_val,  const iISOName_c& /* rc_callerISOName */ )
{
  // use helper function to get automatically casted pointer to used process data type
  uint16_t ui16_index = rc_src.makeIProcDataRemoteSimpleSetpoint() - arr_procData;
  switch ( ui16_index )
  {
    case cui8_indexWorkState:
      if ( ui8_mySetpointWorkState == ri32_val )
      { // FINE!! - maybe trigger positive reaction here
        indicateRemoteWorkStateResponse( true );
      }
      else
      { // SAD
        indicateRemoteWorkStateResponse( false );
      }
      break;
    case cui8_indexApplicationRate:
      if ( i32_mySetpointApplicationRate == ri32_val )
      { // FINE!! - maybe trigger positive reaction here
        indicateRemoteApplicationRateResponse( true );
      }
      else
      {
        indicateRemoteApplicationRateResponse( false );
      }
      break;
  }
  // answer to IsoAgLib that this new setpoint is handled
  return true;
}

// create one class instance for the handler
MyProcDataHandler_c c_myMeasurementHandler;
#endif

// default with primary cultivation mounted back
IsoAgLib::iISOName_c c_myISOName( 2, 0 );
// device type of remote ECU
IsoAgLib::iISOName_c c_remoteDeviceType( 0x5, 0 );

int main()
{ // init CAN channel with 250kBaud at needed channel ( count starts with 0 )
  getIcanInstance().init( cui32_canChannel, 250 );
  // variable for DEV_KEY

  // start address claim of the local member "IMI"
  // if DEV_KEY conflicts forces change of device class instance, the
  // IsoAgLib can cahnge the c_myISOName val through the pointer to c_myISOName
  bool b_selfConf = true;
  uint8_t ui8_indGroup = 2,
      b_func = 25,
      b_wantedSa = 128,
      b_funcInst = 0,
      b_ecuInst = 0;
  uint16_t ui16_manufCode = 0x7FF;
  uint32_t ui32_serNo = 27;

  // start address claim of the local member "IMI"
  // if DEV_KEY conflicts forces change of device class instance, the
  // IsoAgLib can change the c_myISOName val through the pointer to c_myISOName
  // ISO
  IsoAgLib::iIdentItem_c c_myIdent( &c_myISOName,
    b_selfConf, ui8_indGroup, b_func, ui16_manufCode,
    ui32_serNo, b_wantedSa, 0xFFFF, b_funcInst, b_ecuInst);


  const ElementDDI_s s_WorkStateElementDDI[2] =
  {
    // DDI 141
    {141, true, GeneralCommand_c::exactValue},
    // termination entry
    {0xFFFF, false, GeneralCommand_c::noValue}
  };
  const ElementDDI_s s_ApplicationRateElementDDI[3] =
  {
    //  DDI 1
    {1, true, GeneralCommand_c::exactValue},
    //  DDI 2
    {2, false, GeneralCommand_c::exactValue},
    // termination entry
    {0xFFFF, false, GeneralCommand_c::noValue}
  };

#ifdef USE_PROC_HANDLER
  // workstate of MiniVegN (LIS=0, DEVCLASS=2, WERT=1, INST=0)
  arr_procData[cui8_indexWorkState].init(
                                         s_WorkStateElementDDI,
                                         0,
                                         c_remoteDeviceType, c_remoteDeviceType, &c_myISOName,
                                         &c_myMeasurementHandler);

  // WERT == 5 -> device specific material flow information (mostly 5/0 -> distributed/harvested amount per area )
  arr_procData[cui8_indexApplicationRate].init(
                                               s_ApplicationRateElementDDI,
                                               0,
                                               c_remoteDeviceType, c_remoteDeviceType, &c_myISOName,
                                               &c_myMeasurementHandler);

#else
  // workstate of MiniVegN (LIS=0, DEVCLASS=2, WERT=1, INST=0)
  IsoAgLib::iProcDataRemoteSimpleSetpoint_c c_workState(
                                         s_WorkStateElementDDI,
                                         0,
                                         c_remoteDeviceType, c_remoteDeviceType, &c_myISOName
                                         );

  // WERT == 5 -> device specific material flow information (mostly 5/0 -> distributed/harvested amount per area )
  IsoAgLib::iProcDataRemoteSimpleSetpoint_c c_applicationRate(
                                                s_ApplicationRateElementDDI,
                                                0,
                                                c_remoteDeviceType, c_remoteDeviceType, &c_myISOName
                                                );
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
  int32_t i32_idleTimeSpread = 0;
  while ( iSystem_c::canEn() )
  { // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib
    i32_idleTimeSpread = IsoAgLib::getISchedulerInstance().timeEvent();

    if ( ! getIisoMonitorInstance().existIsoMemberISOName(c_myISOName, true) ) continue;
    if ( ! getIisoMonitorInstance().existIsoMemberISOName(c_remoteDeviceType, true) ) continue;
    #ifndef USE_PROC_HANDLER
    if ( ( b_waitingRespWorkState ) && ( c_workState.setpointMasterVal( false ) == ui8_mySetpointWorkState ) )
    { // FINE - the current reported setpoint is the same we commanded lastly
      b_waitingRespWorkState = false;
      indicateRemoteWorkStateResponse( true );
      // an explicit detection of DENY is not possible - or at least not easy
    }
    if ( ( b_waitingRespApplicationRate ) && ( c_applicationRate.setpointMasterVal( false ) == i32_mySetpointApplicationRate ) )
    { // FINE - the current reported setpoint is the same we commanded lastly
      b_waitingRespApplicationRate = false;
      indicateRemoteApplicationRateResponse( true );
      // an explicit detection of DENY is not possible - or at least not easy
    }
    #endif

    #ifdef SYSTEM_PC
      #ifdef WIN32
        if ( i32_idleTimeSpread > 0 ) Sleep(i32_idleTimeSpread);
      #else
        if ( i32_idleTimeSpread > 0 ) IsoAgLib::iCANIO_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
      #endif
    #endif
  }
  return 1;
}

/** dummy function which can be called from some other module to control the remote work state */
void controlRemoteWorkState( bool rb_isWorking )
{
  if ( ! getIisoMonitorInstance().existIsoMemberISOName(c_myISOName, true) ) return;
  if ( ! getIisoMonitorInstance().existIsoMemberISOName(c_remoteDeviceType, true) ) return;

  if ( rb_isWorking ) ui8_mySetpointWorkState = 100;
  else ui8_mySetpointWorkState = 0;

  #ifndef USE_PROC_HANDLER
  b_waitingRespWorkState = true;
  c_workState.setSetpointMasterVal( ui8_mySetpointWorkState );
  #else
  arr_procData[cui8_indexWorkState].setSetpointMasterVal( ui8_mySetpointWorkState );
  #endif
}
/** dummy function which can be called from some other module to control the remote application rate */
void controlRemoteApplicationRate( int32_t ri32_applicationRate )
{
  if ( ! getIisoMonitorInstance().existIsoMemberISOName(c_myISOName, true) ) return;
  if ( ! getIisoMonitorInstance().existIsoMemberISOName(c_remoteDeviceType, true) ) return;

  i32_mySetpointApplicationRate = ri32_applicationRate;
  #ifndef USE_PROC_HANDLER
  b_waitingRespApplicationRate = true;
  c_applicationRate.setSetpointMasterVal( ri32_applicationRate );
  #else
  arr_procData[cui8_indexApplicationRate].setSetpointMasterVal( ri32_applicationRate );
  #endif
}
