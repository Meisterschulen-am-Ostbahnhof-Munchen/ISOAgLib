/* *************************************************************************
                          2_11_TaskcontrollerClientVirtualTerminalClient.cpp - tutorial for tc/vt client
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
/** \example 2_11_TaskcontrollerClientVirtualTerminalClient.cpp
 * In section 2 tutorial examples the provision and distribution of process 
 * values is demonstrated. This communication is done over CAN-BUS. An 
 * example consists of a pair of two applications. One 
 * application is ment as local process (*_Local*), another is ment as remote 
 * process (*_Remote*). If an example provides sole measurment values it is 
 * grouped in a read example (*Read*). If the remote application sets values
 * in a local application, it is gouped in a write example (*Write*).
 *
 *
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>This example ("2_11_TaskcontrollerClientVirtualTerminalClient") is based on 2_11_TaskcontrollerClientVirtualTerminalClient
 * and has an additional virtual terminal object pool. It can act therefore as a Taskcontroller and as a Virtual Terminal client
 * See the 2_11_TaskcontrollerClientVirtualTerminalClient and the 3_0_VirtualTerminalIso example for specific information
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
 *     conf_2_11_TaskcontrollerClientVirtualTerminalClient from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_2_11_TaskcontrollerClientVirtualTerminalClient</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 2_11_TaskcontrollerClientVirtualTerminalClient</i> to go to the subdirectory 2_11_TaskcontrollerClientVirtualTerminalClient
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 2_11_TaskcontrollerClientVirtualTerminalClient in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist__2_11_TaskcontrollerClientVirtualTerminalClient.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_2_11_TaskcontrollerClientVirtualTerminalClient</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_no_card__rte
 *		<li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_rte__rte
 *		<li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_A1__rte
 *		<li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_pcan__rte
 *    <li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_no_card__simulating
 *		<li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_rte__simulating
 *		<li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_A1__simulating
 *		<li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_pcan__simulating
 *    <li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_no_card__sys
 *		<li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_rte__sys
 *		<li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_A1__sys
 *		<li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_pcan__sys
 *    <li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__simulating__rte
 *    <li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__simulating__simulating
 *    <li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_win32__can_server_sock__simulating
 *    <li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pc_win32__can_server_sock__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__esx__sys__sys
 *    <li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__imi__sys__sys
 *    <li> \ref PrjSpec2_11_TaskcontrollerClientVirtualTerminalClient__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__simulating__simulating for needed files
 * ( filelist__2_11_TaskcontrollerClientVirtualTerminalClient.txt ),
 *  with \ref SrcList2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_no_card__rte
 *		<li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_rte__rte
 *		<li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_A1__rte
 *		<li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_pcan__rte
 *    <li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_no_card__simulating
 *		<li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_rte__simulating
 *		<li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_A1__simulating
 *		<li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_pcan__simulating
 *    <li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_no_card__sys
 *		<li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_rte__sys
 *		<li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_A1__sys
 *		<li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__msq_server_pcan__sys
 *    <li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__simulating__rte
 *    <li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__simulating__simulating
 *    <li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_win32__can_server_sock__simulating
 *    <li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pc_win32__can_server_sock__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__esx__sys__sys
 *    <li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__imi__sys__sys
 *    <li> \ref FileLists2_11_TaskcontrollerClientVirtualTerminalClient__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/IsoAgLib/isoaglib_config.h
 * if the \#define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_2_11_TaskcontrollerClientVirtualTerminalClient ( see also at \ref PrjConfig2_11_TaskcontrollerClientVirtualTerminalClient ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/IsoAgLib/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#include "MaskDefinition/IsoTerminalObjectSelection.inc"

#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_2_11_TaskcontrollerClientVirtualTerminalClient.h
#endif

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/* include some needed util headers */
//#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>

/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisomonitor_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/proc_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/Local/Std/iprocdatalocal_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/iprocess_c.h>

#include <IsoAgLib/comm/Part3_DataLink/imultisend_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtclientservercommunication_c.h>
#include <supplementary_driver/driver/datastreams/streaminput_c.h>

#include "devicedescription/DeviceDescription.xml-func.h"

/* the following include direction includes
   all generated ISO Terminal Object Pool Definitions */
#include "MaskDefinition/simpleVTIsoPool_direct.h"

// if following define is active, the version with HANDLER usage is compiled
#define USE_PROC_HANDLER

#ifdef USE_PROC_HANDLER
  #include <IsoAgLib/comm/Part7_ProcessData/processdatachangehandler_c.h>
#endif

#if defined(WIN32) || defined(SYSTEM_PC)
  #define LOG_INFO STL_NAMESPACE::cout
  #include <iostream>
#elif defined(DEBUG)
  #define LOG_INFO getIrs232Instance()
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
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

static iObjectPool_simpleVTIsoPool_c Tutorial_3_0_Pool_c;
static iVtClientServerCommunication_c* spc_tut30csc;

void iObjectPool_simpleVTIsoPool_c::eventNumericValue ( uint16_t objId, uint8_t ui8_value, uint32_t ui32_value )
{
}

void iObjectPool_simpleVTIsoPool_c::eventKeyCode ( uint8_t keyActivationCode, uint16_t /*objId*/, uint16_t /*objIdMask*/, uint8_t keyCode, bool /*wasButton*/ )
{
}

void iObjectPool_simpleVTIsoPool_c::eventObjectPoolUploadedSuccessfully (bool ab_wasLanguageUpdate, int8_t ai8_languageIndex, uint16_t aui16_languageCode)
{
}

void iObjectPool_simpleVTIsoPool_c::eventEnterSafeState ()
{
}

void iObjectPool_simpleVTIsoPool_c::eventStringValue (uint16_t aui16_objId, uint8_t aui8_length, StreamInput_c &rc_streaminput, uint8_t /*aui8_unparsedBytes*/, bool /*b_isFirst*/, bool b_isLast)
{
}



/** dummy local variable which is used by the dummy functions for work state handling */
uint8_t ui8_localDummyWorkState;
/** dummy local variable which is used by the dummy functions for application rate handling */
uint32_t ui32_localDummyApplicationRate;

/** dummy function to decide on acceptance of received setpoint */
bool localIsAcceptableWorkState( const IsoAgLib::iIsoName_c& ac_deviceType, uint32_t aui32_setpointValue )
{ // just for demo - accept from other than device type 1 or 2 only values smaller than 255

  if ( ac_deviceType.devClass() < 3 )
  {
    ui8_localDummyWorkState = aui32_setpointValue;
    return true;
  }
  else
  {
    return false;
  }
}

bool localIsAcceptableApplicationRate( const IsoAgLib::iIsoName_c& ac_deviceType, uint32_t aui32_setpointValue )
{ // just for demo - accept from other than device type 1 or 2 only values smaller than 255
  if ( ( ac_deviceType.devClass() < 3 ) || ( aui32_setpointValue < 255 ) )
  {
    ui32_localDummyApplicationRate = aui32_setpointValue;
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
const uint8_t cui8_indexDefaultDataLogging = 2;
IsoAgLib::iProcDataLocal_c arr_procData[3];

class MyProcDataHandler_c : public IsoAgLib::ProcessDataChangeHandler_c
{
  public:
    //! This handler function is called if one of the process data
    //! entries received a setpoint.
    //! Derive with the help of pointer to the triggered process data
    //! instance the corresponding variable and reacte then specific for this type.
    //! Parameter:
    //! @param ac_src encapsulated pointer to triggered process data variable
    //! @param ai32_val new received setpoint value ( raw from CAN msg; independent interpretation on Exact/Min/Max needed )
    //! @param ac_setpointSender device type of setpoint sender
    //! @param ab_change display if value change or if just new msg arrived, which could be important for handling
    virtual bool processSetpointSet(IsoAgLib::EventSource_c ac_src,
                                    int32_t ai32_val,
                                    const IsoAgLib::iIsoName_c& ac_setpointSender,
                                    bool ab_change);


    //! This handler function is called if a remote process data instances sends a request value command for DDI 0xDFFF (default data logging DDI)
    //! can be used to start measurement programms in local process data instances
    //! Parameter:
    //! @param ac_src: encapsulated pointer to triggered process data variable
    //! @param ai32_val: data value in the request value command
    //! @param ac_callerIsoName: iso name of the sender
    virtual bool processDefaultLoggingStart(IsoAgLib::EventSource_c ac_src,
                                            int32_t ai32_val,
                                            const IsoAgLib::iIsoName_c& ac_callerIsoName);
};


bool MyProcDataHandler_c::processSetpointSet(IsoAgLib::EventSource_c ac_src, int32_t ai32_val, const IsoAgLib::iIsoName_c& ac_setpointSender, bool ab_change)
{
  if ( ! ab_change )
  { // don't handle succeeding setpoints which don't contain new value - maybe still relevant for other applications
    return false; // indicate that this information is not again handled - just ignored
  }

  #if defined(DEBUG) || defined(SYSTEM_PC)
  LOG_INFO << "processSetpointSet called for DDI " << ac_src.makeIProcDataLocal()->getDDIfromCANPkg() << EXTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  // use helper function to get automatically casted pointer to used process data type
  uint16_t ui16_index = ac_src.makeIProcDataLocal() - arr_procData;
  switch ( ui16_index )
  {
    case cui8_indexWorkState:
      if ( ! arr_procData[cui8_indexWorkState].setpoint().existMaster() ) {
        if ( localIsAcceptableWorkState( ac_setpointSender, ai32_val ) )
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
        if ( localIsAcceptableApplicationRate( ac_setpointSender, ai32_val ) )
        { // accept this setpoint and regard from now on the sender as master setpoint sender
          arr_procData[cui8_indexApplicationRate].setpoint().unhandledFirst().setMaster( true );
        }
      }
      else if ( ( arr_procData[cui8_indexApplicationRate].setpoint().existUnhandledMaster() )
             && ( localIsAcceptableApplicationRate( ac_setpointSender, ai32_val )           ) )
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


bool MyProcDataHandler_c::processDefaultLoggingStart(IsoAgLib::EventSource_c /* ac_src */, int32_t /* ai32_val */, const IsoAgLib::iIsoName_c& /* ac_callerIsoName */)
{
  #if defined(DEBUG) || defined(SYSTEM_PC)
  if (arr_procData[cui8_indexApplicationRate].startDataLogging(IsoAgLib::Proc_c::TimeProp, 1000))
    LOG_INFO << "starting measurement application rate success!" <<  EXTERNAL_DEBUG_DEVICE_ENDL;
  else
    LOG_INFO << "starting measurement application rate failure!" <<  EXTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  return true;
}


// create one class instance for the handler
MyProcDataHandler_c c_mySetpointHandler;
#endif

int main()
{ // init CAN channel with 250kBaud at channel 0 ( count starts with 0 )
  IsoAgLib::getIcanInstance().init( 0, 250 );

  spc_tut30csc = getIisoTerminalInstance().initAndRegisterIsoObjectPool (c_myIdent, Tutorial_3_0_Pool_c, "T30v2"); // PoolName: Tutorial 3.0 Version 2
  // only use 5 chars as the pool supports Multi-Language (the last 2 chars are used for the language-code then!
  if (spc_tut30csc == NULL)
  { // shouldln't happen normally!
    #if defined(DEBUG) && defined(SYSTEM_PC)
    EXTERNAL_DEBUG_DEVICE << "Could not registerIsoObjectPool()." << EXTERNAL_DEBUG_DEVICE_ENDL;
    abort();
    #else
    return 1; // Return from main() with error indicated
    #endif
  }

  ui32_localDummyApplicationRate = 10;

  /// Identity Number definition -> change Device Description xml file to used one of the following method.
  // Identity Number is represented by the last 21 bits of the workingset master name. It could be set :
  // - directly in the Device Description xml file. It could be the last 21 bit of attribut workingset_mastername <deviceelement>
  //    or attribut ws_identity_number with decimal or hexadecimal value f.e. ws_identity_number="0x1b"
  //    example :
  //    <device designator="Device1" software_version="0.1" workingset_mastername="Ma00a1900ffe0001b"
  //        serialnumber="WXYZ999" structure_label="STRUCT4" localization_label="de50015A56FF" device_program_name="myDevice">
  // - with bios information through a call to function IsoAgLib::iSystem_c::serialNo()
  //    in this case every single part of the workingset master name must be specified in DeviceDescription.xml,
  //    and ws_identity_number gives a function call f.e. ws_identity_number="IsoAgLib::iSystem_c::serialNo()"
  //    example :
  //    <device designator="Device1" software_version="0.1" self_configurable_address="true" industry_group="2"
  //        device_class_instance="0" device_class="5" function="25" function_instance="0"
  //        ecu_instance="0" manufacturer_code="0x7ff" ws_identity_number="IsoAgLib::iSystem_c::serialNo()"

  /// register pool of the device description
  bool b_registerSuccess = IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().registerDevicePool(&c_myIdent, deviceDescription_de, ui32_arrayLength_de, true);

#ifdef USE_PROC_HANDLER
  arr_procData[cui8_indexWorkState].init(
                                         s_workStateElementDDI,
                                         scui16_workStateElementNumber,
                                         c_myIdent.isoName(), &(c_myIdent.isoName()), false /*ab_cumulativeValue */,
  #ifdef USE_EEPROM_IO
                                         0xFFFF,
  #endif
                                         &c_mySetpointHandler);

  arr_procData[cui8_indexApplicationRate].init(
                                               s_applicationRateElementDDI,
                                               scui16_applicationRateElementNumber,
                                               c_myIdent.isoName(), &(c_myIdent.isoName()), false /*ab_cumulativeValue */,
  #ifdef USE_EEPROM_IO
                                               0xFFFF,
  #endif
                                               &c_mySetpointHandler);
  arr_procData[cui8_indexDefaultDataLogging].init(
                                                  s_defaultLoggingElementDDI,
                                                  0,
                                                  c_myIdent.isoName(), &(c_myIdent.isoName()), false /*ab_cumulativeValue */,
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
  int32_t i32_idleTimeSpread = 0;
  while ( iSystem_c::canEn() )
  { // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib
    i32_idleTimeSpread = IsoAgLib::getISchedulerInstance().timeEvent();

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

    if ( i32_idleTimeSpread > 0 ) IsoAgLib::iCanIo_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
  }
  return 1;
}

