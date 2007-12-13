/* *************************************************************************
                          7_2_SchedulerTask.cpp - tutorial for pure ISO address claim
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
 * applications.                                                        *
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
/** \example 7_2_SchedulerTask.cpp
* This tutorial shall demonstrate the use of ISO<i><sub>AgLib</sub></i> to handle
 * a periodically activity inside a iSchedulerTask_c.
 * When a task which triggers periodically activity is needed, it should be derived from the interface Class
 * <b>iSchedulerTask_c</b>.
 * The periodically activity is done inside the Task method timeEvent:
 * There is no need of mutex since the coordination is done inside the class Scheduler_c.
 * The class which derives from iSchedulerTask_c is also a <b>CanCustomer</b>.
 * It can send/receive on the CAN bus and subscribe itself for a specific FILTER with the method insertMyFilter and process
 * the CAN message received with the method processMsg or processInvalidMsg.
 * In this tutorial is demostrated how to behaves when more than one PRT_INSTANCE_CNT and CAN_INSTANCE are
 * defined.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Trigger periodic activities of ISO<i><sub>AgLib</sub></i>
 * <li> How to initialize a iSchedulerTask_c in case of PRT_INSTANCE_CNT > 1.
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
 * <li>Overview information on Scheduling at \ref SchedulerPage
 * <li>Overview on System Management at \ref SystemMgmtPage
 * <li>Overview on global communication functions at \ref CommOverPage
 * </ol>
 *
 * <H1>Howto compile this example</H1>
 * <ol>
 * <li>Call the script update_makefile.sh with the spec file
 *     conf_7_2_SchedulerTask from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_7_2_SchedulerTask</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 7_2_SchedulerTask</i> to go to the subdirectory 7_2_SchedulerTask
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 7_2_SchedulerTask in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-7_2_SchedulerTask.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_7_2_SchedulerTask</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec7_2_SchedulerTask__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_linux__rte__rte
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_linux__rte__simulating
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_linux__rte__sys
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_linux__simulating__rte
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_linux__simulating__simulating
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_win32__vector_canlib__simulating
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_win32__vector_canlib__sys
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_win32__simulating__simulating
 *    <li> \ref PrjSpec7_2_SchedulerTask__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpec7_2_SchedulerTask__esx__sys__sys
 *    <li> \ref PrjSpec7_2_SchedulerTask__imi__sys__sys
 *    <li> \ref PrjSpec7_2_SchedulerTask__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists7_2_SchedulerTask__pc_linux__simulating__simulating for needed files
 * ( filelist-7_2_SchedulerTask-doxygen_import.txt ),
 *  with \ref SrcList7_2_SchedulerTask__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList7_2_SchedulerTask__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileLists7_2_SchedulerTask__pc_linux__rte__rte
 *    <li> \ref FileLists7_2_SchedulerTask__pc_linux__rte__simulating
 *    <li> \ref FileLists7_2_SchedulerTask__pc_linux__rte__sys
 *    <li> \ref FileLists7_2_SchedulerTask__pc_linux__simulating__rte
 *    <li> \ref FileLists7_2_SchedulerTask__pc_linux__simulating__simulating
 *    <li> \ref FileLists7_2_SchedulerTask__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileLists7_2_SchedulerTask__pc_win32__vector_canlib__simulating
 *    <li> \ref FileLists7_2_SchedulerTask__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref FileLists7_2_SchedulerTask__pc_win32__vector_canlib__sys
 *    <li> \ref FileLists7_2_SchedulerTask__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref FileLists7_2_SchedulerTask__pc_win32__simulating__simulating
 *    <li> \ref FileLists7_2_SchedulerTask__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileLists7_2_SchedulerTask__esx__sys__sys
 *    <li> \ref FileLists7_2_SchedulerTask__imi__sys__sys
 *    <li> \ref FileLists7_2_SchedulerTask__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/IsoAgLib/isoaglib_config.h
 * if the #define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_7_2_SchedulerTask ( see also at \ref PrjConfig7_2_SchedulerTask ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/IsoAgLib/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_7_2_SchedulerTask.h
#endif

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/* include some needed util headers */
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
#include <supplementary_driver/driver/rs232/irs232io_c.h>

#include <IsoAgLib/driver/can/iident_c.h>
#include <IsoAgLib/comm/Part3_DataLink/icanpkgext_c.h>



/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/Scheduler/ischedulertask_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisomonitor_c.h>



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

// Start address claim of the local identity/member
iIdentItem_c c_myIdent (2,     // aui8_indGroup
                        2,     // aui8_devClass
                        0,     // aui8_devClassInst
                        25,    // ab_func
                        0x7FF, // aui16_manufCode
                        27);   // aui32_serNo
                        // further parameters use the default values as given in the constructor


/** define class which is derived from iCanCustomer_c which will handle the internal CAN data */
class MyTask_0 : public IsoAgLib::iSchedulerTask_c
{
 public:
  MyTask_0(int ai_protocolInstance = 0)
   :iSchedulerTask_c(ai_protocolInstance), c_myData( ai_protocolInstance ) {};



  /** perform init */
  void init();
  /**  Operation: processMsg
    This function is called by IsoAgLib, if a matching CAN message is received.
    This will happen as one of the part actions, if you call getISchedulerInstance().timeEvent();
    So as it's no IRQ, you can safely trigger direct response within this function.
    Just plan enough time for getISchedulerInstance().timeEvent()
    @return true -> message was processed
  */

  void insertMyFilter();

  virtual bool processMsg();

  virtual bool processInvalidMsg();

  virtual bool timeEvent( void );

  virtual void close( void ){};

  virtual const char* getTaskName() const;



  /**  Operation: dataBase
    This function is needed by IsoAgLib to get handle for the iCanPkgExt_c
    derived class. This way IsoAgLib has already called string2Flags()
    before processMsg() is executed -> processMsg() can immediately work on the
    received data
  */
  virtual IsoAgLib::iCanPkgExt_c& iDataBase();
  /** perform dummy send */

  /** just make compiler happy */
  virtual ~MyTask_0();
 private:
  iCanPkgExt_c c_myData;
};

void MyTask_0::insertMyFilter()
{ // init second internal CAN at channel 1 ( counting [0..n] )
  // register FilterBoxes to receive some exact ident settings
  IsoAgLib::iCanIo_c &c_can = IsoAgLib::getIcanInstance(1);
  // if more different filter settings are wanted, copy first line, as fourth parameter >false<
  // avoids immediate reconfiguration of CAN filter settings - this is a time expensive opteration
 c_can.insertFilter(*this, static_cast<MASK_TYPE>(0xFFFFFFFF),
                            static_cast<MASK_TYPE>(PROCESS_DATA_PGN), false, IsoAgLib::iIdent_c::ExtendedIdent);


}

/** perform init */
void MyTask_0::init(){}

 /**  Operation:  Function for Debugging in Scheduler_c
  */
  const char* MyTask_0::getTaskName() const
  {
    return "MyTASK_0";
  }

/**  Operation: processMsg
  This function is called by IsoAgLib, if a matching CAN message is received.
  This will happen as one of the part actions, if you call getISchedulerInstance().timeEvent();
  So as it's no IRQ, you can safely trigger direct response within this function.
  Just plan enough time for getISchedulerInstance().timeEvent()
  @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
*/
bool MyTask_0::processMsg()
{

  INTERNAL_DEBUG_DEVICE << getTaskName() << " processMsg" << INTERNAL_DEBUG_DEVICE_ENDL;

  INTERNAL_DEBUG_DEVICE
    << "Received internal CAN message with ident: "  << c_myData.ident()
    << ", len: " << c_myData.getLen()
    << " and data string: " ;
  for ( uint16_t ind = 0; ind < c_myData.getLen(); ind++ )
  {
    if ( ind > 0 ) INTERNAL_DEBUG_DEVICE << ", ";
    INTERNAL_DEBUG_DEVICE << uint16_t( c_myData.getUint8Data( ind ) );
  }

  return true;
}

/** This function processes the not standard messages */
bool MyTask_0::processInvalidMsg()
{

 INTERNAL_DEBUG_DEVICE << getTaskName() << " processInvalidMsg " << INTERNAL_DEBUG_DEVICE_ENDL;

  INTERNAL_DEBUG_DEVICE
    << "Received internal CAN message with ident: "  << c_myData.ident()
    << ", len: " << c_myData.getLen()
    << " and data string: " ;
  for ( uint16_t ind = 0; ind < c_myData.getLen(); ind++ )
  {
    if ( ind > 0 ) INTERNAL_DEBUG_DEVICE << ", ";
    INTERNAL_DEBUG_DEVICE << uint16_t( c_myData.getUint8Data( ind ) );
  }

  return true;
}



bool MyTask_0::timeEvent( void )
{
  INTERNAL_DEBUG_DEVICE << " I`m task :" << getTaskName()
    << " in the timeEvent at protocol instance: " << getSingletonVecKey() << INTERNAL_DEBUG_DEVICE_ENDL;
  // the Postfix "4Comm" after the function name "getIsoMonitorInstance"
  // results in a #define, which calls in the end "getIsoMonitorInstance( getSingletonVecKey() )"
  __IsoAgLib::getIsoMonitorInstance4Comm().sendRequestForClaimedAddress( true );


    return true;

}


/**  Operation: dataBase
  This function is needed by IsoAgLib to get handle for the iCanPkgExt_c
  derived class. This way IsoAgLib has already called string2Flags()
  before processMsg() is executed -> processMsg() can immediately work on the
  received data
*/
IsoAgLib::iCanPkgExt_c& MyTask_0::iDataBase()
{
  return c_myData;
}


/** just make compiler happy */
MyTask_0::~MyTask_0(){}

int main()
{
  // Initialize CAN-Bus
  getIcanInstance(0).init (0); // CAN-Bus 0 (with defaulting 250 kbit)
  getIcanInstance(1).init (1);

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  // create object for handling of internal CAN data
  MyTask_0 c_myTask_0(1);
  #else
  MyTask_0 c_myTask_0;
  #endif


  c_myTask_0.insertMyFilter();

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

  int32_t i32_idleTimeSpread = 0;
  while ( iSystem_c::canEn() )
  { // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib


  for(int i=0; i < PRT_INSTANCE_CNT; i++)
  {
    i32_idleTimeSpread = IsoAgLib::getISchedulerInstance(i).timeEvent();
  }


    if ( i32_idleTimeSpread > 0 ) IsoAgLib::iCanIo_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
  }
  return 1;
}

