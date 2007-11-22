/* *************************************************************************
                          imi.cpp  -  example main application for IMI
                             -------------------
    begin                : Mon Okt 16 08:00:00 CEST 2000

    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
 ***************************************************************************/

/* *************************************************************************
 * This example main application is published NOT as GPL'ed Open Source,   *
 * so that you can include this source file in propietary closed projects. *
 * Everybody is encouraged to use the examples in the examples    *
 * directory for a quick and easy start of development for IsoAgLib applications*
 *                                                                         *
 * So nobody is bound to publish changes of the example source files.      *
 * But every changed example, which could help learning the use of the     *
 * GPLed "IsoAgLib", is invited to be sent to the original author, so      *
 * that it can be published with the other examples.                       *
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 * This is an example of "IsoAgLib", an object oriented program library *
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
 * The "IsoAgLib" is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU Lesser General Public License as   *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * The "IsoAgLib" is distributed in the hope that it will be useful, but*
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

/* *********************************************************************** */
/** \example imi.cpp
 * This example claims an address for an IMI on Scheduler_c, requests the member
 * names of the other systems after succeded address claim and create the
 * local process data that should be recorded. All informations including
 * ISOName, name, transport-or-not and working width are read from EEPROM.
 * If hitch position is not available in Scheduler_c base data, the information is
 * requested as propietary process data from task controller (needed for
 * Scheduler_c retrofit system within research).
 * This leads to the solution: "one binary fits all"
 *
 * <h1>Needed Modules</h1>
 * This example needs the following source modules<ul>
 * <li>lbsLibModuleBase
 * <li>lbsLibModuleProcess
 * <li>lbsLibModuleEeprom
 * <li>lbsLibModuleVirttermDin (optional!!)
 * <li>lbsLibModuleIso11783 (optional!)
 * </ul>
 * <H1>Appropriate Config Settings in isoaglib_config.h</H1>
 * This example needs the following active config constants in the
 * Modulesection of isoaglib_config.h<ul>
 * <li>USE_PROCESS (undefine SIMPLE_SETPOINT)
 * <li>USE_EEPROM_IO
 * <li>USE_ISO_11783 (optional!)
 * </ul>.                                                                  */
/* *************************************************************************/

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/driver/system/isystem_c.h>
#include <supplementary_driver/driver/sensor/ianalogi_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/ISO11783/iisoname_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/proc_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/iprocess_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/Remote/Std/iprocdataremote_c.h>


#ifdef USE_EEPROM_IO
  #include <IsoAgLib/driver/eeprom/ieepromio_c.h>
  // include the configuration header with addresses of some EEPROM informations
  #include "eeprom_adr.h"
  /// specify if ident information should be read from EEPROM
  #define READ_EEPROM_IDENT_YN  YES
#endif

// include the central interface header for the communication layer part
// of the "IsoAgLib"
#include <IsoAgLib/util/iutil_funcs.h>
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracgeneral_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracmove_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracpto_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/ISO11783/iisomonitor_c.h>

// all used local process data doesn't need sophisticated setpoint management
#include <IsoAgLib/comm/Part7_ProcessData/proc_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/Local/SimpleSetpoint/iprocdatalocalsimplesetpoint_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/Remote/Std/iprocdataremote_c.h>

#include <IsoAgLib/driver/can/icanio_c.h>

// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;

int main()
{ // simply call startImi
  getIcanInstance().init( 0, 250 );
  // variable for ISOName
  // default with primary cultivation mounted back
  iIsoName_c myISOName( 2, 0 );
  // uint8_t string for name of this IMI (7 characters + '\0')
  uint8_t myName[12] = "IMI Tes";
  // address claim state of the IMI
  bool myClaimedAddress = false;
  /** control that after 5,7,9 sec. the name is sent */
  uint8_t b_sendNameTime = 5;
  // variable for working width defaultedwith 3000mm
  int32_t myWidth = 15000; //3000;
  // DEVKEY of task controller which delivers EHR of tractor
  iIsoName_c c_autodatacollectorISOName( 1, 3 );
  // lower limit for PTO in working state (0 == work state independent from PTO)
  uint16_t ui16_workMinPtoRear = 0;
  uint16_t ui16_workMinPtoFront = 0;
  // upper limit for EHR in working state (0xFF == work state independent from EHR)
  uint8_t b_workMaxEhrRear = 0xFF;
  uint8_t b_workMaxEhrFront = 0xFF;
  uint8_t b_workMinEhrRear = 0;
  uint8_t b_workMinEhrFront = 0;
  bool b_forceWorkingSpeed = true; // if IMI is only working during movement


#ifdef USE_EEPROM_IO
  iEepromIo_c& c_eeprom = getIeepromInstance();
#endif

#if READ_EEPROM_IDENT_YN ==  YES
  // read preconfigured data from EEPROM
  // read ISOName
  // set read position in EEPROM
  c_eeprom.setg(ADR_IDENT_ISOName);
  // read EEPROM value in variable
  // (read size equivalent to sizeof(myISOName) )
//  c_eeprom >> myISOName;
  // read name
  c_eeprom.setg(ADR_IDENT_NAME_SHORT);
  c_eeprom.readString(myName, 11);
  myName[11] = '\0'; // close string
#endif
  // check for special Fieldstar bin name
  if ((strstr((const char*)myName, "0x") != NULL)
    || (strstr((const char*)myName, "0X") != NULL))
  { // hex fieldstar code
    int32_t i32_nameNr;
    sscanf((const char*)myName, "%li", &i32_nameNr);
    memset(myName, 0, 3); // set first 3 bytes to 0
    for (uint8_t ui8_ind = 0; ui8_ind < 4; ui8_ind++)
      myName[ui8_ind+3]= ((i32_nameNr >> ((3 - ui8_ind)*8)) & 0xFF);
  }

  // read working width
  c_eeprom.setg(ADR_STATIC_WORKING_WIDTH);
  c_eeprom >> myWidth;
  // read lower pto limit for working state
  c_eeprom.setg(ADR_WORK_MIN_PTO_REAR);
  c_eeprom >> ui16_workMinPtoRear;
  c_eeprom.setg(ADR_WORK_MIN_PTO_FRONT);
  c_eeprom >> ui16_workMinPtoFront;
  // read upper ehr limit for working state
  c_eeprom.setg(ADR_WORK_MAX_EHR_REAR);
  c_eeprom >> b_workMaxEhrRear;
  c_eeprom.setg(ADR_WORK_MAX_EHR_FRONT);
  c_eeprom >> b_workMaxEhrFront;
  // read lower ehr limit for working state
  c_eeprom.setg(ADR_WORK_MIN_EHR_REAR);
  c_eeprom >> b_workMinEhrRear;
  c_eeprom.setg(ADR_WORK_MIN_EHR_FRONT);
  c_eeprom >> b_workMinEhrFront;
  // read config whether IMI must move during work
  c_eeprom.setg(ADR_FORCE_WORKING_SPEED);
  c_eeprom >> b_forceWorkingSpeed;

  // start address claim of the local member "IMI"
  // if ISOName conflicts forces change of device class instance, the
  // IsoAgLib can cahnge the myISOName val through the pointer to myISOName
#ifdef USE_ISO_11783
  bool b_selfConf = true;
  uint8_t ui8_indGroup = 2,
      b_func = 25,
      b_wantedSa = 128,
      b_funcInst = 0,
      b_ecuInst = 0;
  uint16_t ui16_manufCode = 0x7FF;
  uint32_t ui32_serNo = 27;
#endif

#ifdef USE_EEPROM_IO
  // read preconfigured data from EEPROM
  // read ISOName
  // set read position in EEPROM
  c_eeprom.setg(ADR_IDENT_ISOName);
  // read EEPROM value in variable
  // (read size equivalent to sizeof(myISOName) )
//  c_eeprom >> myISOName;
  // read name
  c_eeprom.setg(ADR_IDENT_NAME_SHORT);
  c_eeprom.readString(myName, 7);
  myName[7] = '\0'; // close string
  #ifdef USE_ISO_11783
  /*
    c_eeprom.setg(ADRISOIdentNumber);
    c_eeprom >> ui32_serNo;

    c_eeprom.setg(ADRISOManufacturerCode);
    c_eeprom >> ui16_manufCode;

    c_eeprom.setg(ADRISOECUInstance);
    c_eeprom >> b_ecuInst;

    c_eeprom.setg(ADRISOFunctionInstance);
    c_eeprom >> b_funcInst;

    c_eeprom.setg(ADRISOFunction);
    c_eeprom >> b_func;

    c_eeprom.setg(ADRISOIndustryGroup);
    c_eeprom >> ui8_indGroup;


    c_eeprom.setg(ADRISOSelfConfAdr);
    c_eeprom >> b_selfConf;

    c_eeprom.setg(ADR_ISO_SA);
    c_eeprom >> b_wantedSa;
    */
  #endif
#endif
  // start address claim of the local member "IMI"
  // if ISOName conflicts forces change of device class instance, the
  // IsoAgLib can change the myISOName val through the pointer to myISOName
  iIdentItem_c c_myIdent( ui8_indGroup, myISOName.devClass(), myISOName.devClassInst(),
                          b_func, ui16_manufCode, ui32_serNo, b_wantedSa, 0xffff,
                          b_funcInst, b_ecuInst, b_selfConf);
  // Information for calculating informations from remote Scheduler_c base data
  int32_t i32_workDist = 0, // complete working distance
       i32_lastDist = 0, // base data dist from last check
       i32_lastTime = 0, // time of the last check
       i32_newDist,
       i32_workArea = 0,
       i32_workTime = 0,
       i32_distanceOffset = 0; // base data distance on address claim of this IMI
  bool b_workingState = false, // check if hitch is in working position
       b_newWorkingState;
  // the hitch val has bit0-bit6 the position in [%], bit7 tells "eingerueckt"
  uint8_t b_hitchVal;
  int16_t i16_ptoRearVal, i16_ptoFrontVal;

  // local process data for "on/off mechanical" [0/0x64] of primaer Bodenbearbeitung (LIS=0, DEVCLASS=2, WERT=1, INST=0)
  // with full working width (ZAEHLNUM 0xFF), POS, ISOName of local data (can vary from previously given device class & instance),
  // the pointer to myISOName helps automatic update of ISOName, mark this value as NOT cumulated (default)
//   iProcDataLocalSimpleSetpoint_c(const IsoAgLib::ElementDdi_s* ps_elementDDI = NULL, uint16_t aui16_element = 0xFFFF,
//                                  const iIsoName_c& ac_isoName = iIsoName_c::iIsoNameInitialProcessData(),
//                                      const iIsoName_c& ac_ownerISOName = iIsoName_c::iIsoNameUnspecified(),
//                                          const iIsoName_c *apc_isoName = NULL,
//                                          bool ab_cumulativeValue = false,


//  iProcDataLocalSimpleSetpoint_c c_myOnoff(0, myISOName, 0x1, 0x0, 0xFF, 2, myISOName, &myISOName, false);
  iProcDataLocalSimpleSetpoint_c c_myOnoff(NULL, 0, myISOName, myISOName, &myISOName, false);

  // local process data for "whole distance" [m] of primaer Bodenbearbeitung (LIS=0, DEVCLASS=2, WERT=8, INST=1)
//  iProcDataLocalSimpleSetpoint_c c_myWholeDist(0, myISOName, 0x8, 0x1, 0xFF, 2, myISOName, &myISOName, true, ADR_TRIP_DIST);
  iProcDataLocalSimpleSetpoint_c c_myWholeDist(NULL, 0, myISOName, myISOName, &myISOName, true, ADR_TRIP_DIST);
  // local process data for "working distance" [m] of primaer Bodenbearbeitung (LIS=0, DEVCLASS=2, WERT=8, INST=4)
//  iProcDataLocalSimpleSetpoint_c c_myWorkDist(0, myISOName, 0x8, 0x4, 0xFF, 2, myISOName, &myISOName, true, ADR_WHOLE_DIST);
  iProcDataLocalSimpleSetpoint_c c_myWorkDist(NULL, 0, myISOName, myISOName, &myISOName, true, ADR_WHOLE_DIST);

  // local process data for "whole time" [sec] of primaer Bodenbearbeitung (LIS=0, DEVCLASS=2, WERT=0xA, INST=0)
//   iProcDataLocalSimpleSetpoint_c c_myWholeTime(0, myISOName, 0xA, 0x0, 0xFF, 2, myISOName, &myISOName, true, ADR_TRIP_TIME);
  iProcDataLocalSimpleSetpoint_c c_myWholeTime(NULL, 0, myISOName, myISOName, &myISOName, true, ADR_TRIP_TIME);
  // local process data for "work time" [sec] of primaer Bodenbearbeitung (LIS=0, DEVCLASS=2, WERT=0xA, INST=7)
//   iProcDataLocalSimpleSetpoint_c c_myWorkTime(0, myISOName, 0xA, 0x7, 0xFF, 2, myISOName, &myISOName, true, ADR_WHOLE_TIME);
  iProcDataLocalSimpleSetpoint_c c_myWorkTime(NULL, 0, myISOName, myISOName, &myISOName, true, ADR_WHOLE_TIME);

  // create pure NULL pointer for working width and working area
  // and init them only, if working width and area are ddefined
  iProcDataLocalSimpleSetpoint_c *pMyWidth = NULL;
  iProcDataLocalSimpleSetpoint_c *pMyWorkArea = NULL;
  if (myWidth > 0)
  { // only create and handle working width and working area if working width is defined == transport has no working width
    // local process data for "working width" [mm] of primaer Bodenbearbeitung (LIS=0, DEVCLASS=2, WERT=3, INST=1)
//     pMyWidth = new iProcDataLocalSimpleSetpoint_c( 0, myISOName, 0x3, 0x1, 0xFF, 2, myISOName, &myISOName, false);
    pMyWidth = new iProcDataLocalSimpleSetpoint_c( NULL, 0, myISOName, myISOName, &myISOName, false);
    // set the constant width
    pMyWidth->setMasterMeasurementVal(myWidth);
    // local process data for "working area" [m2] of primaer Bodenbearbeitung (LIS=0, DEVCLASS=2, WERT=8, INST=0)
    // with full working width (ZAEHLNUM 0xFF), POS, ISOName of local data (can vary from previously given device class & instance),
    // the pointer to myISOName helps automatic update of ISOName, mark this value as CUMULATED (area grows -> update by de/increment)
//     pMyWorkArea = new iProcDataLocalSimpleSetpoint_c( 0, myISOName, 0x8, 0x0, 0xFF, 2, myISOName, &myISOName, true);
    pMyWorkArea = new iProcDataLocalSimpleSetpoint_c( NULL, 0, myISOName, myISOName, &myISOName, true);
  }

   // self defined remote process data for "EHR position" [%] of tracctor (LIS=0, DEVCLASS=1, WERT=0x4, INST=0)
  // with full working width (ZAEHLNUM 0xFF), POS, ISOName of remote data (can vary from previously given device class & instance),
  // ui8_remoteISOName tells the remote owner of the Process Data (where it is located as local),
  // the pointer to myISOName tells the local commanding member, used for target messages as SEND
//   iProcDataRemote_c c_remoteEhr(0, c_autodatacollectorISOName, 0x4, 0x0, 0xFF, 2, c_autodatacollectorISOName, &myISOName);
  const ElementDdi_s s_WorkStateElementDDI[2] =
  {
    {141, true, GeneralCommand_c::exactValue},
    {0xffff, false, GeneralCommand_c::exactValue}
  };

  IsoAgLib::iProcDataRemote_c c_remoteEhr(s_WorkStateElementDDI, 0, c_autodatacollectorISOName, c_autodatacollectorISOName, &myISOName);
  int8_t c_ehrStartProgCnt = 10;


  // timestamps: first begin of this main loop
  int32_t i32_loopTime = iSystem_c::getTime(),
        // timestamp of last loop
       i32_lastLoopTime = i32_loopTime,
       // last timestamp with active D+ / CAN_EN
       i32_lastPowerTime = i32_loopTime;

  // run loop as int32_t as D+ isn't inactive longer than 1000msec.
  for (i32_loopTime = iSystem_c::getTime(); ((i32_loopTime - i32_lastPowerTime) < 1000); i32_loopTime = iSystem_c::getTime())
  {
    // update i32_lastPowerTime if CAN_EN is still on
    if (getIsystemInstance().canEn()) i32_lastPowerTime = i32_loopTime;

    // run main loop every 10msec.
    if ((i32_loopTime - i32_lastLoopTime) > 50)
    { // store actual timestamp as last loop time
      i32_lastLoopTime = i32_loopTime;

      // IMPORTANT: call main timeEvent function for
      // all time controlled actions of IsoAgLib
      getISchedulerInstance().timeEvent();

      if (getIisoMonitorInstance().existIsoMemberISOName(myISOName, true))
      { // local IMI member has claimed address (as DIN or ISO)
        if (!myClaimedAddress)
        { // first time with claimed address -> do some initial actions
          // send request for member names of other systems
          // (and send own name as demanded by standard)
          // get the i32_distanceOffset to calculate distance from address claim on
          i32_distanceOffset = getITracMoveInstance().distTheor();
          // set the i32_lastDist for getting the working dist to the actual base dist
          i32_lastDist = 0;
          // set i32_lastTime to this time
          i32_lastTime = i32_loopTime;
          myClaimedAddress = true;
        }

        // test if name should be sent (send name after 5, 7 and 9 sec
        if ( (getIisoMonitorInstance().existIsoMemberISOName(myISOName, true))
          && ((i32_loopTime / 1000) == b_sendNameTime) )
        { // inkrement b_sendNameTime to send every 2 sec
          if (b_sendNameTime < 9) b_sendNameTime += 2;
          else b_sendNameTime = 0;
        }

        if (getIisoMonitorInstance().existIsoMemberISOName(c_autodatacollectorISOName, true))
        { // check for all progs, if they should be started or stopped
          switch (c_ehrStartProgCnt)
          {
            case 0:
              // now start EHR prog
              // add time proportional subprog with every 1000msec. (there can be more
              // settings of different type like Proc_c::DistProp)
              c_remoteEhr.prog().addSubprog(Proc_c::TimeProp, 1000);
              // start program with target message time proportional and tell remote
              // system to send actual measured value
              c_remoteEhr.prog().start(/*Proc_c::Target, */Proc_c::TimeProp, Proc_c::DoVal);
              // now set the counter to -1 to avoid further starts
              c_ehrStartProgCnt = -1;
              break;
            case -1:
              // EHR prog is already started
              // -> check if values are arriving
              if (iSystem_c::getTime() - c_remoteEhr.prog().lastMeasurementReceive() > 3000)
              { // no EHR val received since >3sec -> restart
                c_remoteEhr.prog().stop();
                c_ehrStartProgCnt = 2;
              }
              break;
            default:
              // count down
              // control that the EHR prog is started 10 loop runs after
              // detection of active autodatacollector
              c_ehrStartProgCnt -= 1;
              break;
          }
        }
        else
        {  //deactivate pc_gpsData;
          if (c_ehrStartProgCnt == -1)
          {
            c_remoteEhr.prog().stop();
            c_ehrStartProgCnt = 10;
          }
        }

        // calculate some informations from Scheduler_c Data
        b_hitchVal = getITracGeneralInstance().hitchRear() & 0x7F;
        // check if Scheduler_c Base data have correct EHR val
        if (getITracGeneralInstance().hitchFront() == 0xFF)
        { // this is sign, that tractor back EHR data may also not be integrated in Base_c
          // use EHR value communicated by proc data
          b_hitchVal = (c_remoteEhr.prog().val()  & 0x7F);
        }
        // test IMI working state dependent on settings in example.h
        // test first for standard speed != 0 because this is forced for all
        // OR if it is not forced to drive during work set independent from speed working dist
        if ((getITracMoveInstance().speedTheor() != 0 ) || (b_forceWorkingSpeed == false))
        { // IMI and tractor are moving
          b_newWorkingState = true;

          // check if PTO limit is exceeded (if PTO irrelevant the limit is set to 0)
          i16_ptoRearVal = getITracPtoInstance().ptoRear();
          if (i16_ptoRearVal >= 0)
          { // check working state dependent on rear PTO as rear PTO is defined in lbs base data
            if (i16_ptoRearVal < ui16_workMinPtoRear) b_newWorkingState = false;
          }
          i16_ptoFrontVal = getITracPtoInstance().ptoFront();
          if (i16_ptoFrontVal >= 0)
          { // check working state dependent on front PTO as front PTO is defined in lbs base data
            if (i16_ptoFrontVal < ui16_workMinPtoFront) b_newWorkingState = false;
          }

          // check rear EHR: value must be within intervall [b_workMinEhrRear..b_workMaxEhrRear]
          if ((b_hitchVal > b_workMaxEhrRear) || (b_hitchVal < b_workMinEhrRear))
            b_newWorkingState = false;
          // check front EHR: value must be within intervall [b_workMinEhrFront..b_workMaxEhrFront]
          if (((getITracGeneralInstance().hitchFront()  & 0x7F) > b_workMaxEhrFront) || ((getITracGeneralInstance().hitchFront()  & 0x7F) < b_workMinEhrFront))
            b_newWorkingState = false;
        }
        else
        { // tractor stands still (speed == 0)
          b_newWorkingState = false;
        }
        // set the values, which are independent from working state (set in [m] -> from base data in [mm])
        i32_newDist = (getITracMoveInstance().distTheor() - i32_distanceOffset);
        c_myWholeDist.setMasterMeasurementVal(i32_newDist/1000);
        c_myWholeTime.setMasterMeasurementVal(i32_loopTime/1000);

        // check if this and last check was working
        if (b_newWorkingState && b_workingState)
        { // the last intervall was in working state
          // calculate the actual whole working dist
          i32_workDist += i32_newDist - i32_lastDist;
          if (myWidth > 0)
          { // only create and handle working width and working area if working width is defined == transport has no working width
            // now calculate the working area ([mm2]/1000000 -> [m2])
            i32_workArea = mul1Div1Mul2Div2(i32_workDist, 1000, myWidth, 1000);
            // set work area
            pMyWorkArea->setMasterMeasurementVal(i32_workArea);
          }
          // calculate work time in msec
          i32_workTime += i32_loopTime - i32_lastTime;
          // set work time
          c_myWorkTime.setMasterMeasurementVal(i32_workTime/1000);
          // set work dist (set in [m] -> from base data in [mm])
          c_myWorkDist.setMasterMeasurementVal(i32_workDist/1000);
        }
        // set the new vals as old vals for next intervall
        i32_lastDist = i32_newDist;
        b_workingState = b_newWorkingState;
        i32_lastTime = i32_loopTime;

        // set working state to 100% or 0%
        if (b_workingState) c_myOnoff.setMasterMeasurementVal(0x64);
        else c_myOnoff.setMasterMeasurementVal(0);

       }
      else
      {
        myClaimedAddress = false;
      }
    }
  }
  return 1;
}

