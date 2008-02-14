/* *************************************************************************
    TractorInternalCan.cpp - example for tractor ECU (work-in-progress)
                             -------------------
    begin                : Sun Dec 19 20:00:00 CEST 2004

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
/** \example TractorInternalCan.cpp
 * This example application of ISO<i><sub>AgLib</sub></i>is a work-in-progress
 * of a tractor ECU. It receives the tractor data from internal CAN, and
 * provides them as ISO 11783 tractor data on external BUS.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
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
 * <li>Overview information on Scheduling at \ref SchedulerPage
 * <li>Overview on System Management at \ref SystemMgmtPage
 * <li>Overview on global communication functions at \ref CommOverPage
 * </ol>
 *
 * <H1>Howto compile this example</H1>
 * <ol>
 * <li>Call the script update_makefile.sh with the spec file
 *     conf_TractorInternalCan from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_TractorInternalCan</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd TractorInternalCan</i> to go to the subdirectory TractorInternalCan
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable TractorInternalCan in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist__TractorInternalCan.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_TractorInternalCan</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpecTractorInternalCan__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpecTractorInternalCan__pc_linux__msq_server_rte__rte
 *    <li> \ref PrjSpecTractorInternalCan__pc_linux__msq_server_rte__simulating
 *    <li> \ref PrjSpecTractorInternalCan__pc_linux__msq_server_rte__sys
 *    <li> \ref PrjSpecTractorInternalCan__pc_linux__simulating__rte
 *    <li> \ref PrjSpecTractorInternalCan__pc_linux__simulating__simulating
 *    <li> \ref PrjSpecTractorInternalCan__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpecTractorInternalCan__pc_win32__can_server_sock__simulating
 *    <li> \ref PrjSpecTractorInternalCan__pc_win32__can_server_sock__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpecTractorInternalCan__esx__sys__sys
 *    <li> \ref PrjSpecTractorInternalCan__imi__sys__sys
 *    <li> \ref PrjSpecTractorInternalCan__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileListsTractorInternalCan__pc_linux__simulating__simulating for needed files
 * ( filelist__TractorInternalCan.txt ),
 *  with \ref SrcListTractorInternalCan__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrListTractorInternalCan__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileListsTractorInternalCan__pc_linux__msq_server_rte__rte
 *    <li> \ref FileListsTractorInternalCan__pc_linux__msq_server_rte__simulating
 *    <li> \ref FileListsTractorInternalCan__pc_linux__msq_server_rte__sys
 *    <li> \ref FileListsTractorInternalCan__pc_linux__simulating__rte
 *    <li> \ref FileListsTractorInternalCan__pc_linux__simulating__simulating
 *    <li> \ref FileListsTractorInternalCan__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileListsTractorInternalCan__pc_win32__can_server_sock_vector_canlib__simulating
 *    <li> \ref FileListsTractorInternalCan__pc_win32__can_server_sock_vector_xl__simulating
 *    <li> \ref FileListsTractorInternalCan__pc_win32__can_server_sock_vector_canlib__sys
 *    <li> \ref FileListsTractorInternalCan__pc_win32__can_server_sock_vector_xl__sys
 *    <li> \ref FileListsTractorInternalCan__pc_win32__simulating__simulating
 *    <li> \ref FileListsTractorInternalCan__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileListsTractorInternalCan__esx__sys__sys
 *    <li> \ref FileListsTractorInternalCan__imi__sys__sys
 *    <li> \ref FileListsTractorInternalCan__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/IsoAgLib/isoaglib_config.h
 * if the \#define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_tractor ( see also at \ref PrjConfigTractorInternalCan ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/IsoAgLib/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_TractorInternalCan.h
#endif

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/* include some needed util headers */
//#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>

#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/driver/can/iident_c.h>
#include <IsoAgLib/util/icanpkgext_c.h>
#include <IsoAgLib/util/icancustomer_c.h>

#include <supplementary_driver/driver/rs232/irs232io_c.h>


/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//iisomonitor_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itimeposgps_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracgeneral_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracmove_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracpto_c.h>


// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;


/** define identifiers for internal CAN data */
typedef enum {
  InternalSpeed      = 1,
  InternalDistance   = 2,
  InternalPtoFront   = 3,
  InternalPtoRear    = 4,
  InternalRpm        = 5,
  InternalHitchFront = 6,
  InternalHitchRear  = 7,
  InternalPower      = 8,
  InternalCalendar   = 9
} InternalIdent_t;

/**
  define class for decode/encode of propietary CAN messages
  on internal BUS
  */
class MyInternalPkg_c : public IsoAgLib::iCanPkgExt_c
{
 public:
  /**
    Operation: flags2String
    Called on each CAN send from IsoAgLib base functions.
    Must be overloaded, as base class iCanPkgExt_c defines
    only abstract function.
    Target: transfer information from flags to the max 8 Byte
    data string and the corresponding ident. Perform here some
    standard jobs like inserting timestamp, SA or whatever is easier
    to implement here than in the higher level using classes
  */
  virtual void flags2String();

  /**
    Operation: string2Flags
    transfer received data string to the corresponding data flags.
    Don't rely on data consistence of the underlying data string, as this
    is modelled as static, so that all class instances which are derived from
    iCanPkgExt_c will share that buffer. Even if no parallel processes are used,
    you can shoot in your foot, if your application reacts on a message with another
    message, before it tries so send an ACK, where simply some bytes are changed. Here
    the intermediate send of the other messages will invalidate the static data buffer,
    so that the ACK must be set up from scratch.
    This static placement allows reduced memory usage and avoids some copy operations.
  */
  virtual void string2Flags();
};


// variable for ISOName ( device type, device type instance )
// default with tractor device type
IsoAgLib::iIsoName_c myISOName( 1, 0 );

/**
  Operation: flags2String
  Called on each CAN send from IsoAgLib base functions.
  Must be overloaded, as base class iCanPkgExt_c defines
  only abstract function.
*/
void MyInternalPkg_c::flags2String()
{
  /** \todo implement send of max power request, which was received from external ECU
    so that internal node can answer this request
  */
}

/**
  Operation: string2Flags
  transfer received data string to the corresponding data flags.
*/
void MyInternalPkg_c::string2Flags()
{ // do complete interpretation in processMsg
}

/** define class which is derived from iCanCustomer_c which will handle the internal CAN data */
class MyInternalCanHandler_c : public IsoAgLib::iCanCustomer_c
{
 public:
  MyInternalCanHandler_c(){init();};
  /** perform init */
  void init();
  /**  Operation: processMsg
    This function is called by IsoAgLib, if a matching CAN message is received.
    This will happen as one of the part actions, if you call getISchedulerInstance().timeEvent();
    So as it's no IRQ, you can safely trigger direct response within this function.
    Just plan enough time for getISchedulerInstance().timeEvent()
    @return true -> message was processed
  */
  virtual bool processMsg();

  /**  Operation: dataBase
    This function is needed by IsoAgLib to get handle for the iCanPkgExt_c
    derived class. This way IsoAgLib has already called string2Flags()
    before processMsg() is executed -> processMsg() can immediately work on the
    received data
  */
  virtual IsoAgLib::iCanPkgExt_c& iDataBase();
  /** perform dummy send */
  void doSendTest( uint16_t aui16_flag1, uint16_t aui16_flag2, uint16_t aui16_flag3 );
  /** just make compiler happy */
  virtual ~MyInternalCanHandler_c();
 private:
  MyInternalPkg_c c_myData;
};

/** perform init */
void MyInternalCanHandler_c::init()
{ // init second internal CAN at channel 1 ( counting [0..n] )
  getIcanInstance( 1 ).init( 1, 250 );
  // register FilterBoxes to receive some exact ident settings
  IsoAgLib::iCanIo_c &c_can = IsoAgLib::getIcanInstance( 1 );
  // register receive of all internal data - compare all but the last 4 bits of ident
  // -> mask := 0xFFF0, receive everything with matches 0x000?
  c_can.insertFilter(*this, static_cast<MASK_TYPE>( 0xFFF0 ),
                            static_cast<MASK_TYPE>( 0x0000 ), true, IsoAgLib::iIdent_c::StandardIdent);
}

/**  Operation: processMsg
  This function is called by IsoAgLib, if a matching CAN message is received.
  This will happen as one of the part actions, if you call getISchedulerInstance().timeEvent();
  So as it's no IRQ, you can safely trigger direct response within this function.
  Just plan enough time for getISchedulerInstance().timeEvent()
  @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
*/
bool MyInternalCanHandler_c::processMsg()
{
  IsoAgLib::iTimePosGps_c& c_timePosGps = IsoAgLib::getITimePosGpsInstance();
  IsoAgLib::iTracGeneral_c& c_tracGeneral = IsoAgLib::getITracGeneralInstance();
  IsoAgLib::iTracMove_c& c_tracMove = IsoAgLib::getITracMoveInstance();
  IsoAgLib::iTracPTO_c& c_tracPto = IsoAgLib::getITracPtoInstance();
  // simply set the fresh received value update at IsoAgLib
  // - the timeEvent of internal Base_c will then trigger send
  //   of corresponding CAN messages at the right time period
  switch ( c_myData.ident() )
  {
    case InternalSpeed:
      // await real and theor speed in one message with each two byte
      // signed integer
      c_tracMove.setSpeedReal(  c_myData.getInt16Data( 0 ) );
      c_tracMove.setSpeedTheor( c_myData.getInt16Data( 2 ) );
      return true;
    case InternalDistance:
      // await real and theor distance in one message with each four byte
      // signed integer
      c_tracMove.setDistReal(  c_myData.getInt32Data( 0 ) );
      c_tracMove.setDistTheor( c_myData.getInt32Data( 4 ) );
      return true;
    case InternalPtoFront:
      // await front PTO: RPM in first two byte as signed int,
      // third byte as IsoActiveFlag_t the engage state
      // fourth byte as IsoActiveFlag_t the 1000-mode
      // fivth byte as IsoActiveFlag_t the economy mode
      // signed integer
      c_tracPto.setPtoFront(                        c_myData.getInt16Data( 0 ) );
      c_tracPto.setPtoFrontEngaged( IsoActiveFlag_t(c_myData.getUint8Data( 2 ) ) );
      c_tracPto.setPtoFront1000(    IsoActiveFlag_t(c_myData.getUint8Data( 3 ) ) );
      c_tracPto.setPtoFrontEconomy( IsoActiveFlag_t(c_myData.getUint8Data( 4 ) ) );
      return true;
    case InternalPtoRear:
      // await rear PTO: RPM in first two byte as signed int,
      // third byte as IsoActiveFlag_t the engage state
      // fourth byte as IsoActiveFlag_t the 1000-mode
      // fivth byte as IsoActiveFlag_t the economy mode
      // signed integer
      c_tracPto.setPtoRear(                        c_myData.getInt16Data( 0 ) );
      c_tracPto.setPtoRearEngaged( IsoActiveFlag_t(c_myData.getUint8Data( 2 ) ) );
      c_tracPto.setPtoRear1000(    IsoActiveFlag_t(c_myData.getUint8Data( 3 ) ) );
      c_tracPto.setPtoRearEconomy( IsoActiveFlag_t(c_myData.getUint8Data( 4 ) ) );
      return true;
    case InternalRpm:
      // await engine RPM as 2 byte signed integer
 //     c_tracGeneral.setEngine( c_myData.getInt16Data( 0 ) );
      return true;
    case InternalPower:
      // await key switch state as one byte IsoActiveFlag_t
      // await max possible power time as one byte unsigned integer [minute]
      c_tracGeneral.setKeySwitch( IsoActiveFlag_t( c_myData.getUint8Data( 0 ) ) );
      c_tracGeneral.setMaxPowerTime(               c_myData.getUint16Data( 1 ) );
      return true;
    case InternalCalendar:
      // await first two byte unsigned int year,
      // then each unsigned int one byte:
      // month, day, hour, minute, second
      c_timePosGps.setCalendarUtc(
        c_myData.getUint16Data( 0 ), // year
        c_myData.getUint8Data( 2 ),  // month
        c_myData.getUint8Data( 3 ),  // day
        c_myData.getUint8Data( 4 ),  // hour
        c_myData.getUint8Data( 5 ),  // minute
        c_myData.getUint8Data( 6 )   // second
      );
      return true;
    default:
      return false;
  }
}

/**  Operation: dataBase
  This function is needed by IsoAgLib to get handle for the iCanPkgExt_c
  derived class. This way IsoAgLib has already called string2Flags()
  before processMsg() is executed -> processMsg() can immediately work on the
  received data
*/
IsoAgLib::iCanPkgExt_c& MyInternalCanHandler_c::iDataBase()
{
  return c_myData;
}

/** just make compiler happy */
MyInternalCanHandler_c::~MyInternalCanHandler_c(){}


int main()
{ // Initialize the CAN BUS at channel 0 to 250 kbaud
  getIcanInstance().init( 0, 250 ); // getIcanInstance() <==> getIcanInstance( 0 )

  // create object for handling of internal CAN data
  MyInternalCanHandler_c c_myDataHandler;

  // Start address claim of the local identity/member
  iIdentItem_c c_myIdent (2,     // aui8_indGroup
                          2,     // aui8_devClass
                          0,     // aui8_devClassInst
                          25,    // ab_func
                          0x7FF, // aui16_manufCode
                          27);   // aui32_serNo
                          // further parameters use the default values as given in the constructor

  /** configure BaseData_c to send:
    - BaseDataGroup1: real and gear based speed and distance; for ISO: also key_switch_state and max power time
    - BaseDataGroup2: front and rear PTO, engine RPM, front and rear hitch information
    - BaseDataCalendar: calendar data
  */
  getITimePosGpsInstance().config  (&myISOName, IsoAgLib::IdentModeTractor);
  getITracGeneralInstance().config (&myISOName, IsoAgLib::IdentModeTractor);
  getITracMoveInstance().config    (&myISOName, IsoAgLib::IdentModeTractor);
  getITracPtoInstance().config     (&myISOName, IsoAgLib::IdentModeTractor);

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
  int32_t i32_nextDebugSend = 0;
  int32_t i32_idleTimeSpread = 0;
  while ( iSystem_c::canEn() )
  { // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib
    i32_idleTimeSpread = IsoAgLib::getISchedulerInstance().timeEvent();
    // the internal data processing is also triggered by the IsoAgLib scheduler
    // - and each received value update is then automatically transfered by
    //   MyInternalCanHandler_c::processMsg() into the suitable flags of
    //   IsoAgLib internal part for base data sending ( Base_c )

    if ( i32_idleTimeSpread > 0 ) IsoAgLib::iCanIo_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
  }
  return 1;
}
