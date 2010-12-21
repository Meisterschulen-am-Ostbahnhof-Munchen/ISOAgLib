/* *************************************************************************
                          autodatacollector.cpp - tutorial for pure ISO address claim
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
/** \example autodatacollector.cpp
 * This example program is actively used at TUM - Weihenstephan to collect
 * automatically without any need for task preperation the relevant process
 * data of all connected implements. The decision on the recorded data is
 * derived based on device type and device identification.
 * There are some standard default rules like "record application rate
 * in kg/ha for fertilizer spreader". Other device specific rules can be
 * defined in EEPROM.
 * As soon as a new implement is detected at BUS, autodatacollector checks
 * in EEPROM and defaults for a rule to control process data recording.
 * This base version dumps the data in CSV format at RS232.
 * This can simply be extended by writing files of a (local) storage medium.
 * The original name of this example was "autodatacollector" which was
 * irritating, as it never had in mind to implement the ISO 11783 Task-Controller.
 * The target of this program is the automatic data collection, whereas the
 * standard describes rules to define for each tast the data to record.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Create some remote standard process data and request measurement data from remote device
 * <ul>
 *  <li>Standard remote process data class IsoAgLib::iProcDataRemote_c
 *  <li>Use constructor IsoAgLib::iProcDataRemote_c::iProcDataRemote_c to create variable with defined property
 *  <li>Use IsoAgLib::iProcDataRemote_c::init to define process data properties independent from instantiation ( needed especially for arrays of process data )
 *  <li>Use IsoAgLib::iProcDataRemote_c::prog() to retrive measurement program information as class IsoAgLib::iMeasureProgRemote_c
 *  <li>USe IsoAgLib::iProcDataRemote_c::masterVal to access current measurement value
 *  <li>Use IsoAgLib::iMeasureProgRemote_c::addSubprog , IsoAgLib::iMeasureProgRemote_c::start and IsoAgLib::iMeasureProgRemote_c::stop to control programs
 *  <li>Use IsoAgLib::iMeasureProgRemote_c::lastMeasurementReceive to check for last receive time
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
 * <li>Overview on process data at \ref ProcDataPage
 * <li>Overview information on Scheduling at \ref SchedulerPage
 * <li>Overview on global communication functions at \ref CommOverPage
 * </ol>
 *
 * <H1>Howto compile this example</H1>
 * <ol>
 * <li>Call the script update_makefile.sh with the spec file
 *     conf_AutoDataCollector from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_AutoDataCollector</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd AutoDataCollector</i> to go to the subdirectory AutoDataCollector
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable AutoDataCollector in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist__AutoDataCollector.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_AutoDataCollector</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpecAutoDataCollector__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpecAutoDataCollector__pc_linux__msq_server_rte__rte
 *    <li> \ref PrjSpecAutoDataCollector__pc_linux__msq_server_rte__simulating
 *    <li> \ref PrjSpecAutoDataCollector__pc_linux__msq_server_rte__sys
 *    <li> \ref PrjSpecAutoDataCollector__pc_linux__simulating__rte
 *    <li> \ref PrjSpecAutoDataCollector__pc_linux__simulating__simulating
 *    <li> \ref PrjSpecAutoDataCollector__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpecAutoDataCollector__pc_win32__can_server_sock__simulating
 *    <li> \ref PrjSpecAutoDataCollector__pc_win32__can_server_sock__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpecAutoDataCollector__esx__sys__sys
 *    <li> \ref PrjSpecAutoDataCollector__imi__sys__sys
 *    <li> \ref PrjSpecAutoDataCollector__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileListsAutoDataCollector__pc_linux__simulating__simulating for needed files
 * ( filelist__AutoDataCollector.txt ),
 *  with \ref SrcListAutoDataCollector__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrListAutoDataCollector__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileListsAutoDataCollector__pc_linux__msq_server_rte__rte
 *    <li> \ref FileListsAutoDataCollector__pc_linux__msq_server_rte__simulating
 *    <li> \ref FileListsAutoDataCollector__pc_linux__msq_server_rte__sys
 *    <li> \ref FileListsAutoDataCollector__pc_linux__simulating__rte
 *    <li> \ref FileListsAutoDataCollector__pc_linux__simulating__simulating
 *    <li> \ref FileListsAutoDataCollector__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileListsAutoDataCollector__pc_win32__can_server_sock_vector_canlib__simulating
 *    <li> \ref FileListsAutoDataCollector__pc_win32__can_server_sock_vector_xl__simulating
 *    <li> \ref FileListsAutoDataCollector__pc_win32__can_server_sock_vector_canlib__sys
 *    <li> \ref FileListsAutoDataCollector__pc_win32__can_server_sock_vector_xl__sys
 *    <li> \ref FileListsAutoDataCollector__pc_win32__simulating__simulating
 *    <li> \ref FileListsAutoDataCollector__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileListsAutoDataCollector__esx__sys__sys
 *    <li> \ref FileListsAutoDataCollector__imi__sys__sys
 *    <li> \ref FileListsAutoDataCollector__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/IsoAgLib/isoaglib_config.h
 * if the \#define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_AutoDataCollector ( see also at \ref PrjConfigAutoDataCollector ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/IsoAgLib/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_AutoDataCollector.h
#endif

#include <list>

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/* include some needed util headers */
//#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/driver/eeprom/ieepromio_c.h>
#include <supplementary_driver/driver/rs232/irs232io_c.h>

/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/isystemmgmt_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/DIN9684/idinmonitor_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracgeneral_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itimeposgps_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracmove_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracpto_c.h>

#include <IsoAgLib/comm/Part7_ProcessData/proc_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/Remote/Std/iprocdataremote_c.h>


// include the configuration header with addresses of some EEPROM informations
#include <IsoAgLib/eeprom_adr.h>
// include object headers for flexible management of remote data sources
#include "AutoDataCollector/getyflexmanager_c.h"
// handle GPS informations of Fieldstar (tm) terminal
#include "AutoDataCollector/gpsmanager_c.h"
// handling of configuration data
#include "AutoDataCollector/defaultrecordconfig_c.h"
/// specify if ident information should be read from EEPROM
#define READ_EEPROM_IDENT_YN  YES

// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;

/**
  write data column header to RS232
*/
void writeHeader();
/**
  check for members where data should be recorded
  andd start / stop measuring ass appropriate
  @return true -> a new imi was found -> print header
*/
bool check_for_imi();
void writeData();

// DEVKEY of other devices
IsoAgLib::iIsoName_c pc_otherGdevKey[10];
uint8_t ui8_cntOtherISOName = 0;
uint8_t ui8_rearEhrVal;

typedef STL_NAMESPACE::list<DevClassFlexManager_c> list_device_data;
typedef STL_NAMESPACE::list<DevClassFlexManager_c>::iterator iterator_device_data;
list_device_data c_listDeviceData;


// instantiate handler for Fieldstar GPS
GpsManager_c c_gpsData;

// object to handle default config settings for control of recording
DefaultRecordConfig_c c_defaultRecordConfig;

#ifdef USE_HDD_IO
  char fahrer_name[10] = "nobody";
  typedef enum  { initialised = 1, opened_auftrag = 2, read_auftrag_called = 3,
    can_close_auftrag = 4, closed_auftrag = 5, opened_task = 6, close_task_for_summary = 7,
    closed_task = 8, opening_task = 9, opened_summary = 10, written_summary = 11, closed_summary = 12} t_hdd_working_state;

  t_hdd_working_state en_hddWorkingState = initialised;
#endif

// variable for DEV_KEY
IsoAgLib::iIsoName_c c_myISOName( 1, 5 );

int main()
{ // simply call startImi
  IsoAgLib::getIcanInstance().init( 0, 125 );
  uint8_t myName[12] = "Taskcon";
  bool my_announced = false;

  for ( uint8_t ind = 0; ind < 10; ind++ )
  {
    pc_otherGdevKey[ind].setUnspecified();
  }

  // init RS232
  IsoAgLib::iRS232IO_c& c_rs232 = IsoAgLib::getIrs232Instance();
  c_rs232.init( 19200, IsoAgLib::iRS232IO_c::_8_N_1, false, 80, 10 );

  #if READ_EEPROM_IDENT_YN == YES
  iEepromIo_c& c_eeprom = getIeepromInstance();

  // read preconfigured data from EEPROM
  // read DEV_KEY
  // set read position in EEPROM
  c_eeprom.setg(ADR_IDENT_DEVKEY);
  // read EEPROM value in variable
  // (read size equivalent to sizeof(c_myISOName) )
  uint8_t ui8_tempDinISOName;
  c_eeprom >> ui8_tempDinISOName;
  c_myISOName.set( &ui8_tempDinISOName );
  // read name
  c_eeprom.setg(ADR_IDENT_NAME_SHORT);
  c_eeprom.readString(myName, 11);
  myName[7] = '\0'; // close string
  #endif

  // start address claim of the local member "Taskcon"
  // if DEV_KEY conflicts forces change of device class instance, the
  // IsoAgLib can change the myISOName val through the pointer to myISOName
  IsoAgLib::iIdentItem_c c_myIdent( &c_myISOName, myName );

  // decide if column header should be written on new detected system
  bool b_writeHeader = false;
  // object to handle default config settings for control of recording
  c_defaultRecordConfig.init(ADR_TASK_CONTROLLER_DEFAULT_CONFIG);

#ifdef USE_HDD_IO
  char pc_auftragLine[50],
      tag[10], wert[10];
  int32_t i32_hddWarnLedOnTime = 0;
  bool b_hdd_warn_led = false;
  IsoAgLib::iDigitalO_c c_alarm_beeper(0);

  // Control if data can be logged to HDD
  bool w_can_log_data = false;
#endif

  int32_t i32_lastDataWriteTime = 0,
          i32_announceTime = 0,
          i32_loopTime;

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
      in the header xgpl_src/IsoAgLib/isoaglib_config.h
    - This can be also controlled during runtime with the function call:
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownByExplcitCall )
      or
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownOnCanEnLoss )
  */
  while ( iSystem_c::canEn() )
  { // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib
    i32_loopTime = IsoAgLib::iSystem_c::getTime();
    IsoAgLib::getISchedulerInstance().timeEvent( i32_loopTime + 1000 );

    IsoAgLib::iSystem_c::triggerWd();

#ifdef USE_HDD_IO
      w_can_log_data = false;
      if ((!c_rs232.error()) && (c_rs232.isFreeSpace()))
      {
        switch (en_hddWorkingState)
        {
          case initialised:
            if (c_rs232.awaitNewCmd(true))
            {
              c_rs232.open("auftrag.txt");
              en_hddWorkingState = opened_auftrag;
            }
            break;
          case opened_auftrag:
            if (c_rs232.awaitNewCmd(true))
            {
              if (c_rs232.isOpen())
              {
                c_rs232.getline(pc_auftragLine, 49);
                en_hddWorkingState = read_auftrag_called;
              }
              if (c_rs232.isFileNotFound())
              {
                strcpy(fahrer_name, "jeder");
                en_hddWorkingState = can_close_auftrag;
              }
            }
            break;
        case read_auftrag_called:
          if (c_rs232.awaitNewCmd(true))
          {
            for (uint8_t ui8_ind = 0; ui8_ind < 50; ui8_ind++)pc_auftragLine[ui8_ind] = tolower(pc_auftragLine[ui8_ind]);
            sscanf(pc_auftragLine, "%s %s", tag, wert);
            if (strcmp(tag, "fahrer:") == 0)
            { // Fahrer Tag
              strncpy(fahrer_name, wert, 10);
            }
            else
            {
              strcpy(fahrer_name, "jeder");
            }
            en_hddWorkingState = can_close_auftrag;
          }
          break;
        case can_close_auftrag:
          c_rs232.close();
          en_hddWorkingState = closed_auftrag;
          break;
        case closed_auftrag:
          if (c_rs232.awaitNewCmd(true))
          {
            c_rs232.open("taskcon.txt");
            en_hddWorkingState = opening_task;
          }
          break;
        case opening_task:
          if ( (c_rs232.isOpen())
          && (c_rs232.awaitNewCmd(true)) )
          { // now first write can begin
            en_hddWorkingState = opened_task;
          }
          break;
        case opened_task:
          // check if O.K. state for write
          if ( (c_rs232.isOpen())
            && (c_rs232.isFreeSpace())
            && (c_rs232.awaitNewCmd(true))
             ) w_can_log_data = true;
          break;
        case close_task_for_summary:
          if (c_rs232.awaitNewCmd(true))
          {
            c_rs232.close();
            en_hddWorkingState = closed_task;
          }
          break;
        case closed_task:
          if (c_rs232.awaitNewCmd(true))
          {
            // open the summary
            //c_rs232.open("summary.txt");
            en_hddWorkingState = opened_summary;
          }
          break;
        case opened_summary:
          if ( (c_rs232.isOpen())
          && (c_rs232.awaitNewCmd(true)) )
          {
           // write_summary();
            en_hddWorkingState = written_summary;
            ;
          }
          break;
        case written_summary:
          if ( (c_rs232.isOpen())
          && (c_rs232.awaitNewCmd(true))
          && (!c_rs232.waitingTextForFlush())
              )
          {
            c_rs232.close();
            en_hddWorkingState = closed_summary;
          }
          break;
        case closed_summary:
          if (c_rs232.awaitNewCmd(true))
          {
            c_rs232.open("taskcon.txt");
            en_hddWorkingState = opening_task;
          }
          break;
        }
        pc_alarm_beeper->set(0);
        b_hdd_warn_led = false;
        i32_hddWarnLedOnTime = 0;
      }
      else
      { // during error beep rythm 1sec.
        // only change state to closed_auftrag if not during init (initial TAN sync)
        if (en_hddWorkingState != initialised) en_hddWorkingState = closed_auftrag;
        if ((i32_loopTime - i32_hddWarnLedOnTime) > 1000)
        { // switch LED state
          if (b_hdd_warn_led)
          {
            b_hdd_warn_led = false;
            pc_alarm_beeper->set(0);
          }
          else
          {
            b_hdd_warn_led = true;
            pc_alarm_beeper->set(0xFFFF);
          }
          i32_hddWarnLedOnTime = i32_loopTime;
        }
      }
#endif
    ui8_rearEhrVal = getITracGeneralInstance().hitchRear() & 0x7F;
    if (getIdinMonitorInstance().existDinMemberISOName(c_myISOName, true))
    {
      if (!my_announced)
      {
        getIdinMonitorInstance().requestDinMemberNames();
        // hardware.rs232() << "System angemeldet\r\n";
        b_writeHeader = true;
        i32_announceTime = i32_loopTime;
      }
      my_announced = true;
    }
    else
    {
      my_announced = false;
    }
    // check if imi is announced, so that measuring prog can be started
#ifdef USE_HDD_IO
    if ((my_announced) && (w_can_log_data) && (i32_loopTime - i32_announceTime > 1000) )
    {
#else
    if ((my_announced) && (i32_loopTime - i32_announceTime > 1000))
    {
#endif
          if (check_for_imi() || b_writeHeader)
      {
        writeHeader();
        b_writeHeader = false;
        // NEW ACHIM 2004-05-04
        // delay next write to two seconds after header
        i32_lastDataWriteTime = i32_loopTime + (2 * c_defaultRecordConfig.recordingRate());
      }
      else if ((i32_loopTime - i32_lastDataWriteTime) > c_defaultRecordConfig.recordingRate())
      { // write data to RS232
        i32_lastDataWriteTime = i32_loopTime;
        // write standard traktor data
        writeData();
      }

      // this process might last longer -> trigger watchdog
      IsoAgLib::iSystem_c::triggerWd();
    }
  }
  return 1;
}



/**
  check for members where data should be recorded
  andd start / stop measuring ass appropriate
  @return true -> a new imi was found -> print header
*/
bool check_for_imi()
{
  bool b_writeHeader = false;
  uint8_t ui8_cntAnnounced = getIdinMonitorInstance().dinMemberCnt(true);
  IsoAgLib::iIsoName_c c_testISOName;
  IsoAgLib::iDINItem_c *p_member;
  iterator_device_data pc_iter;

  for (uint8_t ui8_ind = 0; ui8_ind < ui8_cntAnnounced; ui8_ind++)
  {
    p_member = &(getIdinMonitorInstance().dinMemberInd(ui8_ind, true));
    c_testISOName = p_member->devKey();
    // continue immediate with next member if DEVCLASS < 2 (devKey == 0x10 (base or tractor)
    if (p_member->devKey().getDevClass() < 2) continue;
    // DEVCLASS is now >= 0x10
    // search in device data list
    pc_iter = find(c_listDeviceData.begin(), c_listDeviceData.end(), c_testISOName);
    // if not found -> create new item
    if (pc_iter == c_listDeviceData.end())
    { // not found
      // check if member has already name
      if (p_member->isEmptyName())
      { // empty name
        continue;
      }
      //  cehck if this device was active during last task -> continue
      uint8_t ui8_lastTaskMonth, ui8_lastTaskDay;
      uint16_t ui16_eepromOffsetAdr = 0xFFFF;
      iEepromIo_c& c_eeprom = getIeepromInstance();

      c_eeprom.setg(ADR_TASK_CONTROLLER_DATE_DAY);
      c_eeprom >> ui8_lastTaskDay;
      c_eeprom.setg(ADR_TASK_CONTROLLER_DATE_MONTH);
      c_eeprom >> ui8_lastTaskMonth;

      if ( (getITimePosGpsInstance().dayLocal() == ui8_lastTaskDay)
        && (getITimePosGpsInstance().monthLocal() == ui8_lastTaskMonth) )
      { // try to continue recording
        uint8_t ui8_lastTaskDeviceISOName_1, pui8_lastTaskDeviceName_1[8],
              ui8_lastTaskDeviceISOName_2, pui8_lastTaskDeviceName_2[8];
        c_eeprom.setg(ADR_TASK_CONTROLLER_IMPLEMENT_1_DEVKEY);
        c_eeprom >> ui8_lastTaskDeviceISOName_1;
        c_eeprom.setg(ADR_TASK_CONTROLLER_IMPLEMENT_1_NAME);
        c_eeprom.readString(pui8_lastTaskDeviceName_1, 7);
        c_eeprom.setg(ADR_TASK_CONTROLLER_IMPLEMENT_2_DEVKEY);
        c_eeprom >> ui8_lastTaskDeviceISOName_2;
        c_eeprom.setg(ADR_TASK_CONTROLLER_IMPLEMENT_2_NAME);
        c_eeprom.readString(pui8_lastTaskDeviceName_2, 7);
        if ( (c_testISOName.getCombinedDin() ==  ui8_lastTaskDeviceISOName_1)
           &&(memcmp(p_member->name(),pui8_lastTaskDeviceName_1,7) == 0)
           )
        { // actual new device is same as first device of last task -> continue
          ui16_eepromOffsetAdr =  ADR_TASK_CONTROLLER_IMPLEMENT_1_OFFSET;
        }
        else
        {
          if ( (c_testISOName.getCombinedDin() ==  ui8_lastTaskDeviceISOName_2)
             &&(memcmp(p_member->name(),pui8_lastTaskDeviceName_2,7) == 0)
             )
          { // actual new device is same as second device of last task -> continue
            ui16_eepromOffsetAdr =  ADR_TASK_CONTROLLER_IMPLEMENT_2_OFFSET;
          }
        }
      }
      if (ui16_eepromOffsetAdr == 0xFFFF)
      { // no record task to be continued -> reset offset values in EEPROM
        // as new job was detected
        c_eeprom.setp(ADR_TASK_CONTROLLER_IMPLEMENT_1_DEVKEY);
        c_eeprom << c_testISOName.getCombinedDin();
        c_eeprom.setp(ADR_TASK_CONTROLLER_IMPLEMENT_1_NAME);
        c_eeprom.writeString(p_member->name(), 7);
        // set actual task record date
        c_eeprom.setp(ADR_TASK_CONTROLLER_DATE_DAY);
        c_eeprom << getITimePosGpsInstance().dayLocal();
        c_eeprom.setp(ADR_TASK_CONTROLLER_DATE_MONTH);
        c_eeprom << getITimePosGpsInstance().monthLocal();
        // reset setting for second device
        c_eeprom.setp(ADR_TASK_CONTROLLER_IMPLEMENT_2_DEVKEY);
        c_eeprom << (uint8_t)0xFF;


        ui16_eepromOffsetAdr =  ADR_TASK_CONTROLLER_IMPLEMENT_1_OFFSET;
        // reset offset values of device 1
        c_eeprom.setp(ui16_eepromOffsetAdr);
        for (uint8_t ui8_ind = 0; ui8_ind < 9; ui8_ind++) c_eeprom << (int32_t)0;
      }
      // insert new record data set
      c_listDeviceData.push_back(DevClassFlexManager_c(&c_defaultRecordConfig, ui16_eepromOffsetAdr));
      b_writeHeader = true;
      c_listDeviceData.back().activate(p_member, &c_myISOName);
    }
  }

  bool b_repeat = true;
  while (b_repeat && (!c_listDeviceData.empty())) {
    b_repeat = false; // if none found/erased, don't repeat...
    for (pc_iter = c_listDeviceData.begin(); pc_iter != c_listDeviceData.end(); pc_iter++)
    { // test all device data  sets if according member is still active
      if (!(getIdinMonitorInstance().existDinMemberISOName(pc_iter->devKey(), true)))
      { // according member no more announced in monitor list -> delete
        b_writeHeader = true;
/*      iterator_device_data pc_iter_del = pc_iter;
        pc_iter--;
        pc_iter_del->deactivate();
        c_listDeviceData.erase(pc_iter_del);
*/      pc_iter->deactivate();
        c_listDeviceData.erase(pc_iter);

        b_repeat = true;
        break; // skip here and iterate all again!
      }
    }
  }

  if (!c_gpsData.activated())
  {
    b_writeHeader = true;
    c_gpsData.activate();
  }

  // check for other devices
  uint8_t ui8_memberCnt = getIdinMonitorInstance().dinMemberCnt(true);
  IsoAgLib::iIsoName_c c_checkedISOName;
  ui8_cntOtherISOName = 0;
  for (uint8_t ui8_ind = 0; ui8_ind < ui8_memberCnt ; ui8_ind++)
  {
    c_checkedISOName =getIdinMonitorInstance().dinMemberInd(ui8_ind, true).devKey();
    if ((c_checkedISOName.getDevClass() < 2) && (c_checkedISOName != c_myISOName))
    { // this is another not yet recorded device
      if (c_checkedISOName != pc_otherGdevKey[ui8_cntOtherISOName])
      {
        b_writeHeader = true;
        pc_otherGdevKey[ui8_cntOtherISOName] = c_checkedISOName;
      }
      // convert name to pure ascii
      ui8_cntOtherISOName++;
    }
  }
  return b_writeHeader;
}

/**
  write data column header to RS232
*/
void writeHeader()
{ // start every header with two empty lines
  IsoAgLib::iRS232IO_c& c_rs232 = IsoAgLib::getIrs232Instance();
  c_rs232 << "\r\n\r\n";

  // "date;time;lon;N/S;lat;E/W;alt;gpsstate;" see gps_manager.cc
  c_gpsData.writeHeader();

  // this process might last longer -> trigger watchdog
  IsoAgLib::iSystem_c::triggerWd();

#ifdef USE_HDD_IO
 c_rs232
    << "person;";
#endif

 c_rs232
    << "tname;tspeedr;tspeedth;tfuel;tfueltemp;"
    << "tengrpm;tfptorpm;tfhpos;tbptorpm;tbhpos;"
    << "tbhfol;tbhfor";

  for (iterator_device_data pc_iter = c_listDeviceData.begin(); pc_iter != c_listDeviceData.end(); pc_iter++)
  {
    c_rs232 << ";";
    pc_iter->writeHeader();
  }
  c_rs232 << "\r\n";
}


void writeData()
{
  IsoAgLib::iRS232IO_c& c_rs232 = IsoAgLib::getIrs232Instance();
  // timestamps for control of data poll every second
//  static long l_last_time_record = 0, l_last_dist_record = 0;
  static uint8_t b_update_eeprom_offset_countdown = 0;
  // control to read tractor offsets one time before first
  // write access
  static bool b_tractor_offset_eeprom_updated = false;
  iEepromIo_c& c_eeprom = getIeepromInstance();

  // this process might last longer -> trigger watchdog
  IsoAgLib::iSystem_c::triggerWd();

  if (b_tractor_offset_eeprom_updated == false)
  { // check for EEPROM offsets ADR_TASK_CONTROLLER_TRAC_OFFSET
    b_tractor_offset_eeprom_updated = true; // only once eeprom read
    uint8_t b_last_task_month, b_last_task_day;
    c_eeprom.setg(ADR_TASK_CONTROLLER_DATE_DAY);
    c_eeprom >> b_last_task_day;
    c_eeprom.setg(ADR_TASK_CONTROLLER_DATE_MONTH);
    c_eeprom >> b_last_task_month;
  }

  // test if new area is reached
//  check_area_type();
#ifdef USE_HDD_IO
  // check if area change was detected -> don't write to Hdd
  if (en_hdd_working_state != opened_task) return;
#endif
    c_gpsData.writeData();
#ifdef USE_HDD_IO
 c_rs232
    << fahrer_name << ";";
#endif
  int8_t pc_tempName[15];
  getIdinMonitorInstance().dinMemberISOName( c_myISOName, true ).getPureAsciiName(pc_tempName, 14);
 c_rs232
    << (char*)pc_tempName << ";";
 c_rs232
    << getITracMoveInstance().speedReal() << ";"
    << getITracMoveInstance().speedTheor() << ";";


  c_rs232
    << getITracGeneralInstance().fuelRate() << ";"
    << getITracGeneralInstance().fuelTemperature() << ";";


  c_rs232
    << getITracGeneralInstance().engine() << ";"
    << getITracPtoInstance().ptoFront() << ";"
    << (getITracGeneralInstance().hitchFront() & 0x7F) << ";"
    << getITracPtoInstance().ptoRear() << ";"
    << (int)ui8_rearEhrVal << ";";


  // NEW!! write data from NEW Base Msg 3 + Fuel Cons
 c_rs232
    << getITracGeneralInstance().rearLeftDraft() << ";"
    << getITracGeneralInstance().rearRightDraft();
  for (iterator_device_data pc_iter = c_listDeviceData.begin(); pc_iter != c_listDeviceData.end(); pc_iter++)
  {
    c_rs232 << ";";
    pc_iter->writeData();
    #if 0
    if (b_update_eeprom_offset_countdown == 0)
      pc_iter->write_offset_values_to_eeprom();
    #endif
  }
  c_rs232 << "\r\n";
}
