/* *************************************************************************
                          5_0_InternalCan.cpp - tutorial for pure ISO address claim
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
/** \example 5_0_InternalCan.cpp
 * This tutorial shall demonstrate the use of ISO<i><sub>AgLib</sub></i> to handle
 * additional CAN channels for propietary internal CAN communication.
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
 *     conf_5_0_InternalCan from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_5_0_InternalCan</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 5_0_InternalCan</i> to go to the subdirectory 5_0_InternalCan
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 5_0_InternalCan in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-5_0_InternalCan.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_5_0_InternalCan</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec5_0_InternalCan__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpec5_0_InternalCan__pc_linux__rte__rte
 *    <li> \ref PrjSpec5_0_InternalCan__pc_linux__rte__simulating
 *    <li> \ref PrjSpec5_0_InternalCan__pc_linux__rte__sys
 *    <li> \ref PrjSpec5_0_InternalCan__pc_linux__simulating__rte
 *    <li> \ref PrjSpec5_0_InternalCan__pc_linux__simulating__simulating
 *    <li> \ref PrjSpec5_0_InternalCan__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpec5_0_InternalCan__pc_win32__vector_canlib__simulating
 *    <li> \ref PrjSpec5_0_InternalCan__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref PrjSpec5_0_InternalCan__pc_win32__vector_canlib__sys
 *    <li> \ref PrjSpec5_0_InternalCan__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref PrjSpec5_0_InternalCan__pc_win32__simulating__simulating
 *    <li> \ref PrjSpec5_0_InternalCan__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpec5_0_InternalCan__esx__sys__sys
 *    <li> \ref PrjSpec5_0_InternalCan__imi__sys__sys
 *    <li> \ref PrjSpec5_0_InternalCan__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists5_0_InternalCan__pc_linux__simulating__simulating for needed files
 * ( filelist-5_0_InternalCan-doxygen_import.txt ),
 *  with \ref SrcList5_0_InternalCan__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList5_0_InternalCan__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileLists5_0_InternalCan__pc_linux__rte__rte
 *    <li> \ref FileLists5_0_InternalCan__pc_linux__rte__simulating
 *    <li> \ref FileLists5_0_InternalCan__pc_linux__rte__sys
 *    <li> \ref FileLists5_0_InternalCan__pc_linux__simulating__rte
 *    <li> \ref FileLists5_0_InternalCan__pc_linux__simulating__simulating
 *    <li> \ref FileLists5_0_InternalCan__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileLists5_0_InternalCan__pc_win32__vector_canlib__simulating
 *    <li> \ref FileLists5_0_InternalCan__pc_win32__vector_xl_drv_lib__simulating
 *    <li> \ref FileLists5_0_InternalCan__pc_win32__vector_canlib__sys
 *    <li> \ref FileLists5_0_InternalCan__pc_win32__vector_xl_drv_lib__sys
 *    <li> \ref FileLists5_0_InternalCan__pc_win32__simulating__simulating
 *    <li> \ref FileLists5_0_InternalCan__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileLists5_0_InternalCan__esx__sys__sys
 *    <li> \ref FileLists5_0_InternalCan__imi__sys__sys
 *    <li> \ref FileLists5_0_InternalCan__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/IsoAgLib/isoaglib_config.h
 * if the #define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_5_0_InternalCan ( see also at \ref PrjConfig5_0_InternalCan ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/IsoAgLib/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_5_0_InternalCan.h
#endif

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/* include some needed util headers */
//#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
#include <supplementary_driver/driver/rs232/irs232io_c.h>

#include <IsoAgLib/driver/can/iident_c.h>
#include <IsoAgLib/util/icanpkgext_c.h>
#include <IsoAgLib/util/icancustomer_c.h>


/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//iisomonitor_c.h>

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

  /** individual dummy function to set flag data */
  void setMyDummyData( uint16_t aui16_flag1, uint16_t aui16_flag2, uint16_t aui16_flag3 );

  /** individual dummy function to get flag data */
  uint16_t getMyDummyData1( void ) const { return ui16_flag1;};

  /** individual dummy function to get flag data */
  uint16_t getMyDummyData2( void ) const { return ui16_flag2;};

  /** individual dummy function to get flag data */
  uint16_t getMyDummyData3( void ) const { return ui16_flag3;};
 private:
  uint16_t ui16_flag1;
  uint16_t ui16_flag2;
  uint16_t ui16_flag3;
};


/**
  Operation: flags2String
  Called on each CAN send from IsoAgLib base functions.
  Must be overloaded, as base class iCanPkgExt_c defines
  only abstract function.
*/
void MyInternalPkg_c::flags2String()
{
  setUint16Data( 0, ui16_flag1 );
  setUint16Data( 2, ui16_flag2 );
  setUint16Data( 4, ui16_flag3 );

  switch ( ui16_flag1 )
  {
    case 0:
      if (c_myIdent.isClaimedAddress())
      { // local ident has already claimed address
        // just place some data which is retrieved in standardized manner
        setUint8Data (6, c_myIdent.getIsoItem()->nr());
        setUint8Data (7, ( IsoAgLib::iSystem_c::getTime() / 1000 ) % 0xFF );
        // set len and ident
        setLen( 8 );
        setIdent( MASK_TYPE( 0xAFFE ), IsoAgLib::iIdent_c::ExtendedIdent );
      }
      else
      { // local ident has not yet claimed address
        // just place some data which is retrieved in standardized manner
        setUint8Data (6, 0xFF);
        // set len and ident
        setLen( 7 );
        setIdent( MASK_TYPE( 0xABBA ), IsoAgLib::iIdent_c::ExtendedIdent );
      }
      break;
    default:
      // just do some default decoding
      setUint16Data( 6, ( IsoAgLib::iSystem_c::getTime() / 1000 ) );
      setLen( 8 );
      setIdent( MASK_TYPE( 0xBAD ), IsoAgLib::iIdent_c::ExtendedIdent );
      break;
  }
}

/**
  Operation: string2Flags
  transfer received data string to the corresponding data flags.
*/
void MyInternalPkg_c::string2Flags()
{
  if ( identType() == IsoAgLib::iIdent_c::StandardIdent )
  {
    ui16_flag1 = ( ident() & 0xFFFF );
    ui16_flag2 = ( ( getUint8Data(0) * getUint8Data(1) ) & 0xFFFF );
  }
  else
  {
    if ( getUint8Data(5) != 0 ) ui16_flag1 = ( ( ident() & 0xFFFF ) / getUint8Data(5) );
    else ui16_flag1 = ( ident() & 0xFFFF );
    // use helper function to read uint16_t value starting from Byte2 ( start count with [0..n] )
    ui16_flag2 = getUint16Data( 2 );
  }
  ui16_flag3 = getUint8Data(4);
}

/** individual dummy function to set flag data */
void MyInternalPkg_c::setMyDummyData( uint16_t aui16_flag1, uint16_t aui16_flag2, uint16_t aui16_flag3 )
{
  ui16_flag1 = aui16_flag1;
  ui16_flag2 = aui16_flag2;
  ui16_flag3 = aui16_flag3;
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
  // if more different filter settings are wanted, copy first line, as fourth parameter >false<
  // avoids immediate reconfiguration of CAN filter settings - this is a time expensive opteration
  c_can.insertFilter(*this, static_cast<MASK_TYPE>(0xFF00),
                            static_cast<MASK_TYPE>( 0x1200 ), false, IsoAgLib::iIdent_c::ExtendedIdent);
  // trigger immediate update of CAN filtering after set of last FilterBox
  // Important: With FilterBox abstraction, your can combine freely mask/filter pairs
  c_can.insertFilter(*this, static_cast<MASK_TYPE>(0xFFFF),
                            static_cast<MASK_TYPE>( 0x1234 ), true, IsoAgLib::iIdent_c::ExtendedIdent);
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
  IsoAgLib::getIrs232Instance()
    << "Received internal CAN message at time: " << c_myData.time()
    << " with ident: " << c_myData.ident()
    << ", len: " << c_myData.getLen()
    << " and data string: " ;
  for ( uint16_t ind = 0; ind < c_myData.getLen(); ind++ )
  {
    if ( ind > 0 ) IsoAgLib::getIrs232Instance() << ", ";
    IsoAgLib::getIrs232Instance() << uint16_t( c_myData.getUint8Data( ind ) );
  }
  IsoAgLib::getIrs232Instance()
    << "\r\nThis was encoded into flag1: " << c_myData.getMyDummyData1()
    << ", flag2: " << c_myData.getMyDummyData2()
    << ", flag3: " << c_myData.getMyDummyData3()
    << "\r\n";
  return true;
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
/** perform dummy send */
void MyInternalCanHandler_c::doSendTest( uint16_t aui16_flag1, uint16_t aui16_flag2, uint16_t aui16_flag3 )
{ // place data in MyInternalPkg_c
  c_myData.setMyDummyData( aui16_flag1, aui16_flag2, aui16_flag3 );
  // call CanIo_c send function - it does automatically call flags2string to get the data string
  IsoAgLib::getIcanInstance( 1 ) << c_myData;
}

/** just make compiler happy */
MyInternalCanHandler_c::~MyInternalCanHandler_c(){}


int main()
{
  // Initialize CAN-Bus
  getIcanInstance().init (0); // CAN-Bus 0 (with defaulting 250 kbit)

  // create object for handling of internal CAN data
  MyInternalCanHandler_c c_myDataHandler;

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

    if ( IsoAgLib::iSystem_c::getTime() >= i32_nextDebugSend )
    { // if we are getting here - send test message
      i32_nextDebugSend = IsoAgLib::iSystem_c::getTime() + 1000;
      c_myDataHandler.doSendTest( (IsoAgLib::iSystem_c::getTime()&0xFFFF), (IsoAgLib::iSystem_c::getTime()>>8),
        (IsoAgLib::iSystem_c::getTime()%100) );
    }
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

