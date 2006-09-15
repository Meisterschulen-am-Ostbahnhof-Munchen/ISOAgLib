/* *************************************************************************
                          2_4_LocalWriteSetpointStdIso.cpp - tutorial for pure ISO address claim
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
/** \example 2_4_LocalWriteSetpointStdIso.cpp
 * This tutorial shall provide the simples base program, which creates some
 * local process data, which value is regularly updated.
 * Remote ECUs can request single values or can start measurement programs.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Create some local standard process data and handle received setpoints
 * <li>Use the posibility to check for device type of setpoints sender to decide on
 *     accept or deny of setpoint value
 * <ul>
 *  <li>Standard local process data class IsoAgLib::iProcDataLocal_c
 *  <li>Use constructor IsoAgLib::iProcDataLocal_c::iProcDataLocal_c to create variable with defined property
 *  <li>Use IsoAgLib::iProcDataLocal_c::init to define process data properties independent from instantiation ( needed especially for arrays of process data )
 *  <li>Use IsoAgLib::iProcDataLocal_c::setMasterMeasurementVal() to set current measurement data
 *  <li>Use IsoAgLib::iProcDataLocal_c::setpoint to get access to the setpoint handling part of the process data of type IsoAgLib::iSetpointLocal_c
 *  <li>Use IsoAgLib::iSetpointLocal_c::existUnhandledMaster , IsoAgLib::iSetpointLocal_c::master , IsoAgLib::iSetpointLocal_c::unhandledMaster ,
 *      IsoAgLib::iSetpointLocal_c::acceptNewMaster to check for current master setpoint sender ( master == ECU which sent first accepted setpoint, and which
 *      should be able to rule the further work of the controlled ECU --> later setpoints of other ECUs should never cause a deviation from the control of the master
 *  <li>Use IsoAgLib::iSetpointLocal_c::unhandledCnt and unhandledInd to access the not yet handled received setpoints
 *  <li>Use IsoAgLib::iSetpointLocal_c::respondAckNack to notify all not yet handled setpoint senders of acceptance or deny
 *  <li>Use IsoAgLib::iSetpointLocal_c::setAllowedDeltaPercent to control the allowed deviation between setpoint and measurement -> send problem indication to controlling
 *      ECU if current setpoint can't be realized at the moment ( IsoAgLib sends this indication automatically if call of IsoAgLib::iProcDataLocal_c::setMasterMeasurementVal
 *      indicates this problem --> application don't have to do this )
 *  <li>Use IsoAgLib::iSetpointRegister_c::devKey to get device type of corresponding setpoint sender
 *  <li>Use IsoAgLib::iSetpointRegister_c::existExact , IsoAgLib::iSetpointRegister_c::existPercent , IsoAgLib::iSetpointRegister_c::existMin and
 *      IsoAgLib::iSetpointRegister_c::existMax to check which setpoint type was used
 *  <li>Use IsoAgLib::iSetpointRegister_c::exact , IsoAgLib::iSetpointRegister_c::percent , IsoAgLib::iSetpointRegister_c::min and
 *      IsoAgLib::iSetpointRegister_c::max to access the setpoint value
 *  <li>Use IsoAgLib::iSetpointRegister_c::setMaster( bool ) to dedicate the corresponding setpoint and its sending ECU as master
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
 *     conf_2_4_LocalWriteSetpointStdIso from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_2_4_LocalWriteSetpointStdIso</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 2_4_LocalWriteSetpointStdIso</i> to go to the subdirectory 2_4_LocalWriteSetpointStdIso
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 2_4_LocalWriteSetpointStdIso in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-2_4_LocalWriteSetpointStdIso.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_2_4_LocalWriteSetpointStdIso</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_linux__rte__rte
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_linux__rte__simulating
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_linux__rte__sys
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_linux__simulating__rte
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_linux__simulating__simulating
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_win32__vector_canlib__simulating
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_win32__vector_canlib__sys
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_win32__simulating__simulating
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__esx__sys__sys
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__imi__sys__sys
 *    <li> \ref PrjSpec2_4_LocalWriteSetpointStdIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists2_4_LocalWriteSetpointStdIso__pc_linux__simulating__simulating for needed files
 * ( filelist-2_4_LocalWriteSetpointStdIso-doxygen_import.txt ),
 *  with \ref SrcList2_4_LocalWriteSetpointStdIso__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList2_4_LocalWriteSetpointStdIso__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_linux__rte__rte
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_linux__rte__simulating
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_linux__rte__sys
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_linux__simulating__rte
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_linux__simulating__simulating
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_win32__vector_canlib__simulating
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_win32__vector_canlib__sys
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_win32__simulating__simulating
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__esx__sys__sys
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__imi__sys__sys
 *    <li> \ref FileLists2_4_LocalWriteSetpointStdIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/Application_Config/isoaglib_config.h
 * if the #define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_2_4_LocalWriteSetpointStdIso ( see also at \ref PrjConfig2_4_LocalWriteSetpointStdIso ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/Application_Config/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_2_4_LocalWriteSetpointStdIso.h
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
#include <IsoAgLib/comm/Process/Local/Std/iprocdatalocal_c.h>
#include <IsoAgLib/comm/Process/iprocess_c.h>

#include "devicedescription/DeviceDescription.xml-func.h"

// if following define is active, the version with HANDLER usage is compiled
#define USE_PROC_HANDLER

#ifdef USE_PROC_HANDLER
  #include <IsoAgLib/comm/Process/processdatachangehandler_c.h>
#endif

// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;

/** dummy local variable which is used by the dummy functions for work state handling */
uint8_t ui8_localDummyWorkState;
/** dummy local variable which is used by the dummy functions for application rate handling */
uint32_t ui32_localDummyApplicationRate;

/** dummy function to decide on acceptance of received setpoint */
bool localIsAcceptableWorkState( const IsoAgLib::iDevKey_c& rc_deviceType, uint32_t rui32_setpointValue )
{ // just for demo - accept from other than device type 1 or 2 only values smaller than 255

  if ( rc_deviceType.getDevClass() < 3 )
  {
    ui8_localDummyWorkState = rui32_setpointValue;
    return true;
  }
  else
  {
    return false;
  }
}

bool localIsAcceptableApplicationRate( const IsoAgLib::iDevKey_c& rc_deviceType, uint32_t rui32_setpointValue )
{ // just for demo - accept from other than device type 1 or 2 only values smaller than 255
  if ( ( rc_deviceType.getDevClass() < 3 ) || ( rui32_setpointValue < 255 ) )
  {
    ui32_localDummyApplicationRate = rui32_setpointValue;
    return true;
  }
  else
  {
    return false;
  }
}

/** dummy function to retrieve if the implement is at the moment in working state */
uint8_t getCurrentWorkState( void ) { return ui8_localDummyWorkState;};
/** dummy function to retrieve currently realized application rate ( is probably in reality slightly different from setpoint ) */
uint8_t getCurrentApplicationRate( void )
{
  static int8_t i8_deviation = 1;
  if (i8_deviation > 0)
     i8_deviation = -1;
  else
    i8_deviation = 1;
 return ui32_localDummyApplicationRate + i8_deviation;
};

#ifdef USE_PROC_HANDLER

const uint8_t cui8_indexWorkState = 0;
const uint8_t cui8_indexApplicationRate = 1;
IsoAgLib::iProcDataLocal_c arr_procData[2];

class MyProcDataHandler_c : public IsoAgLib::ProcessDataChangeHandler_c
{
  public:
    //! This handler function is called if one of the process data
    //! entries received a setpoint.
    //! Derive with the help of pointer to the triggered process data
    //! instance the corresponding variable and reacte then specific for this type.
    //! Parameter:
    //! @param rc_src encapsulated pointer to triggered process data variable
    //! @param ri32_val new received setpoint value ( raw from CAN msg; independent interpretation on Exact/Min/Max needed )
    //! @param rc_setpointSender device type of setpoint sender
    //! @param rb_change display if value change or if just new msg arrived, which could be important for handling
    virtual bool processSetpointSet(IsoAgLib::EventSource_c rc_src,
                                    int32_t ri32_val,
                                    const IsoAgLib::iDevKey_c& rc_setpointSender,
                                    bool rb_change);
};

bool MyProcDataHandler_c::processSetpointSet(IsoAgLib::EventSource_c rc_src, int32_t ri32_val, const IsoAgLib::iDevKey_c& rc_setpointSender, bool rb_change)
{
  if ( ! rb_change )
  { // don't handle succeeding setpoints which don't contain new value - maybe still relevant for other applications
    return false; // indicate that this information is not again handled - just ignored
  }

  std::cout << "processSetpointSet called for DDI " << rc_src.makeIProcDataLocal()->getDDIfromCANPkg() << std::endl;

  // use helper function to get automatically casted pointer to used process data type
  uint16_t ui16_index = rc_src.makeIProcDataLocal() - arr_procData;
  switch ( ui16_index )
  {
    case cui8_indexWorkState:
      if ( ! arr_procData[cui8_indexWorkState].setpoint().existMaster() ) {
        if ( localIsAcceptableWorkState( rc_setpointSender, ri32_val ) )
        { // accept this setpoint and regard from now on the sender as master setpoint sender
          arr_procData[cui8_indexWorkState].setpoint().unhandledFirst().setMaster( true );
        }
      }
      else if ( arr_procData[cui8_indexWorkState].setpoint().existUnhandledMaster() )
      { // an already existing master sent this new value
        arr_procData[cui8_indexWorkState].setpoint().acceptNewMaster();
      }
      // send answer for all received setpoints
      arr_procData[cui8_indexWorkState].setpoint().respondAckNack();
      break;
    case cui8_indexApplicationRate:
      if ( ! arr_procData[cui8_indexApplicationRate].setpoint().existMaster() ) {
        if ( localIsAcceptableApplicationRate( rc_setpointSender, ri32_val ) )
        { // accept this setpoint and regard from now on the sender as master setpoint sender
          arr_procData[cui8_indexApplicationRate].setpoint().unhandledFirst().setMaster( true );
        }
      }
      else if ( ( arr_procData[cui8_indexApplicationRate].setpoint().existUnhandledMaster() )
             && ( localIsAcceptableApplicationRate( rc_setpointSender, ri32_val )           ) )
      { // an already existing master sent this new value
        arr_procData[cui8_indexApplicationRate].setpoint().acceptNewMaster();
      }
      // send answer for all received setpoints
      arr_procData[cui8_indexApplicationRate].setpoint().respondAckNack();
      break;
  }
  // answer to IsoAgLib that this new setpoint is handled
  return true;
}

// create one class instance for the handler
MyProcDataHandler_c c_mySetpointHandler;
#endif

int main()
{ // init CAN channel with 250kBaud at channel 0 ( count starts with 0 )
  IsoAgLib::getIcanInstance().init( 0, 250 );

  /// register pool of the device description
  bool b_registerSuccess = IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().registerDevicePool(&c_myIdent, deviceDescription_de, ui32_arrayLength_de, true);

#ifdef USE_PROC_HANDLER
  // workstate of MiniVegN (LIS=0, DEVCLASS=2, WERT=1, INST=0)
  arr_procData[cui8_indexWorkState].init(
                                         s_workStateElementDDI,
                                         scui16_workStateElementNumber,
                                         myDeviceDevKey, 2, myDeviceDevKey, &myDeviceDevKey, true,
  #ifdef USE_EEPROM_IO
                                         0xFFFF,
  #endif
                                         &c_mySetpointHandler);

  // WERT == 5 -> device specific material flow information (mostly 5/0 -> distributed/harvested amount per area )
  arr_procData[cui8_indexApplicationRate].init(
                                               s_applicationRateElementDDI,
                                               scui16_applicationRateElementNumber,
                                               myDeviceDevKey, 2, myDeviceDevKey, &myDeviceDevKey, true,
  #ifdef USE_EEPROM_IO
                                               0xFFFF,
  #endif
                                               &c_mySetpointHandler);
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

    #ifndef USE_PROC_HANDLER
    // if no handler is used to react on received setpoints, each process data must be polled for new
    // received setpoints
    if ( c_workState.setpoint().unhandledCnt() > 0 ) {
      if ( ! c_workState.setpoint().existMaster() ) {
        for ( uint8_t ind = 0; ind < c_workState.setpoint().unhandledCnt(); ind++ )
        { // now decide on all received setpoints
          if ( localIsAcceptableWorkState( c_workState.setpoint().unhandledInd( ind ).exact() ) )
          { // this setpoint is regarded as acceptable
            c_workState.setpoint().unhandledInd( ind ).setMaster( true );
            break; // ignore all others
          }
        }
      }
      else if ( localIsAcceptableWorkState( c_workState.setpoint().unhandledMaster().exact() ) )
      { // simply accept new master if 0 or 100
        c_workState.setpoint().acceptNewMaster();
      }
      // send answer for all received setpoints
      c_workState.setpoint().respondAckNack();
    }

    if ( c_applicationRate.setpoint().unhandledCnt() > 0 ) {
      if ( ! c_applicationRate.setpoint().existMaster() ) {
        for ( uint8_t ind = 0; ind < c_applicationRate.setpoint().unhandledCnt(); ind++ )
        { // now decide on all received setpoints
          if ( localIsAcceptableApplicationRate( c_applicationRate.setpoint().unhandledInd( ind ).exact() ) )
          { // this setpoint is regarded as acceptable
            c_applicationRate.setpoint().unhandledInd( ind ).setMaster( true );
            break; // ignore all others
          }
        }
      }
      else if ( localIsAcceptableApplicationRate( c_applicationRate.setpoint().unhandledMaster().exact() ) )
      { // simply accept new master if 0 or 100
        c_applicationRate.setpoint().acceptNewMaster();
      }
      // send answer for all received setpoints
      c_applicationRate.setpoint().respondAckNack();
    }
    #endif


    // update local value for local process data so that remote ECUs can get the current
    // value from IsoAgLib
    // IsoAgLib can additionally inform setpoint senders if current value is indicating that
    // setpoints can be realized ( i.e. send NACK or out-of-service information )
    #ifdef USE_PROC_HANDLER

    arr_procData[cui8_indexWorkState].setMasterMeasurementVal( getCurrentWorkState() );
    arr_procData[cui8_indexApplicationRate].setMasterMeasurementVal( getCurrentApplicationRate() );
    #else
    c_workState.setMasterMeasurementVal( getCurrentWorkState() );
    c_applicationRate.setMasterMeasurementVal( getCurrentApplicationRate() );
    #endif
  }
  return 1;
}

