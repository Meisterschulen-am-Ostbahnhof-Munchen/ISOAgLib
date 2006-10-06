/* *************************************************************************
                          4_1_Sensor.cpp - tutorial to show sensor use
                             -------------------
    begin                : Mon Nov 29 10:00:00 CEST 2004

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
/** \example 4_1_Sensor.cpp
 * This tutorial shall demonstrate the capable extensions of sensor input use by
 * ISO<i><sub>AgLib</sub></i>. It provides automatic calculation for dimensions
 * like [mV] or [mA] and is extended by automatic range check for analog inputs.
 * As some input signals might be evaluted as counting signal with a very low rate
 * ( e.g. one impulse every 5 seconds ), the normal timer based low level functions
 * of the BIOS/OS can't be used. Here ISO<i><sub>AgLib</sub></i> can count these
 * signals in a standardized manner.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Use digital input with polling evaluation and handler function/class
 * <li>Evaluate analog input signales with their [mV] and [mA] dimension
 * <li>Use possibility to configure the ranges of analog input independent from their monitoring
 * <li>Use central access to sensor inputs bases on their input channel
 * <li>Trigger periodic activities of ISO<i><sub>AgLib</sub></i>
 * 	<ul>
 *	<li>Core class IsoAgLib::iScheduler_c for scheduling of all periodic activities
 *	<li>Method IsoAgLib::iScheduler_c::timeEvent() which can<ul>
 *		<li>Perform activities until defined rl_endTime is reached, which is important
 *			for scheduling purposes of whole system - call by IsoAgLib::iScheduler_c::timeEvent( rl_endTime )
 *		<li>Process all received CAN messages until all receive buffers are empty
 *			-> simple call, but can lead to deadlock on to high CAN load
 *		</ul>
 *	</ul>
 * <li>Create local identity, for which the ISO<i><sub>AgLib</sub></i> performs an address claim,
 *		so that the ECU can access the <b><i>ISO11783</i></b> BUS with IsoAgLib::iIdentItem_c
 *		and its constructor IsoAgLib::iIdentItem_c::iIdentItem_c
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
 *     conf_4_1_Sensor from the directory where the spec file
 *     and the script are located<br>
 *     <i>#> update_makefile.sh conf_4_1_Sensor</i>
 * <li><b>Only Linux:</b><ul>
 *     <li><i>#>cd 4_1_Sensor</i> to go to the subdirectory 4_1_Sensor
 *     <li><i>#> make</i><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 4_1_Sensor in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-4_1_Sensor.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_4_1_Sensor</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec4_1_Sensor__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *		<li> \ref PrjSpec4_1_Sensor__pc_linux__rte__rte
 *		<li> \ref PrjSpec4_1_Sensor__pc_linux__rte__simulating
 *		<li> \ref PrjSpec4_1_Sensor__pc_linux__rte__sys
 *		<li> \ref PrjSpec4_1_Sensor__pc_linux__simulating__rte
 *		<li> \ref PrjSpec4_1_Sensor__pc_linux__simulating__simulating
 *		<li> \ref PrjSpec4_1_Sensor__pc_linux__simulating__sys
 *	</ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *		<li> \ref PrjSpec4_1_Sensor__pc_win32__vector_canlib__simulating
 *		<li> \ref PrjSpec4_1_Sensor__pc_win32__vector_xl_drv_lib__simulating
 *		<li> \ref PrjSpec4_1_Sensor__pc_win32__vector_canlib__sys
 *		<li> \ref PrjSpec4_1_Sensor__pc_win32__vector_xl_drv_lib__sys
 *		<li> \ref PrjSpec4_1_Sensor__pc_win32__simulating__simulating
 *		<li> \ref PrjSpec4_1_Sensor__pc_win32__simulating__sys
 * 	</ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *		<li> \ref PrjSpec4_1_Sensor__esx__sys__sys
 *		<li> \ref PrjSpec4_1_Sensor__imi__sys__sys
 *		<li> \ref PrjSpec4_1_Sensor__pm167__sys__sys
 *	</ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists4_1_Sensor__pc_linux__simulating__simulating for needed files
 * ( filelist-4_1_Sensor-doxygen_import.txt ),
 *  with \ref SrcList4_1_Sensor__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList4_1_Sensor__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *		<li> \ref FileLists4_1_Sensor__pc_linux__rte__rte
 *		<li> \ref FileLists4_1_Sensor__pc_linux__rte__simulating
 *		<li> \ref FileLists4_1_Sensor__pc_linux__rte__sys
 *		<li> \ref FileLists4_1_Sensor__pc_linux__simulating__rte
 *		<li> \ref FileLists4_1_Sensor__pc_linux__simulating__simulating
 *		<li> \ref FileLists4_1_Sensor__pc_linux__simulating__sys
 *	</ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *		<li> \ref FileLists4_1_Sensor__pc_win32__vector_canlib__simulating
 *		<li> \ref FileLists4_1_Sensor__pc_win32__vector_xl_drv_lib__simulating
 *		<li> \ref FileLists4_1_Sensor__pc_win32__vector_canlib__sys
 *		<li> \ref FileLists4_1_Sensor__pc_win32__vector_xl_drv_lib__sys
 *		<li> \ref FileLists4_1_Sensor__pc_win32__simulating__simulating
 *		<li> \ref FileLists4_1_Sensor__pc_win32__simulating__sys
 * 	</ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *		<li> \ref FileLists4_1_Sensor__esx__sys__sys
 *		<li> \ref FileLists4_1_Sensor__imi__sys__sys
 *		<li> \ref FileLists4_1_Sensor__pm167__sys__sys
 *	</ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/Application_Config/isoaglib_config.h
 * if the #define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_4_1_Sensor ( see also at \ref PrjConfig4_1_Sensor ).
 *                                                                         */
/* *************************************************************************/

/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/Application_Config/isoaglib_config.h
    to include project specific configuration settings.
		Set this define in the project file or Makefile of the whole
		project, so that each source file is compiled with this setting
	*/
#ifndef PRJ_USE_AUTOGEN_CONFIG
	#define PRJ_USE_AUTOGEN_CONFIG config_4_1_Sensor.h
#endif

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"

/* include some needed util headers */
//#include <IsoAgLib/util/config.h>

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
#include <supplementary_driver/driver/sensor/isensori_c.h>
#include <supplementary_driver/driver/sensor/ianalogirangecheck_c.h>
#include <supplementary_driver/driver/rs232/irs232io_c.h>

/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisomonitor_c.h>


// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;

/** handler class for digital input interrupt handling */
class MyDiginIrqHandle : public IsoAgLib::iSensorEventHandler
{
	/** function to handle a DigitalI_c event from HAL
		* @param rui8_channel channel of the input object, which received the IRQ
		*        from HAL
		*/
	void handleDigitalEvent( uint8_t rui8_channel );
	/** simple counter - just to do more than simply logging */
	int16_t i16_dummyCounter;
};

/** function to handle a DigitalI_c event from HAL
	* @param rui8_channel channel of the input object, which received the IRQ
	*        from HAL
	*/
void MyDiginIrqHandle::handleDigitalEvent( uint8_t rui8_channel )
{
	IsoAgLib::getIrs232Instance()
		<< "Detected DiginIRQ Event at channel: " << uint16_t(rui8_channel) << "\r\n";
	i16_dummyCounter++;
}


int main()
{
  // Initialize CAN-Bus
  getIcanInstance().init (0); // CAN-Bus 0 (with defaulting 250 kbit)

  // start address claim of the local identity/member
  IsoAgLib::iIdentItem_c c_myIdent (2,    // rui8_indGroup
                                    2,    // rui8_devClass
                                    0,    // rui8_devClassInst
                                    25,   // rb_func
                                    0x7FF,// rui16_manufCode
                                    27);  // rui32_serNo
                                    // further parameters use the default

  // create some sensor instances
  // create digin which delivers true for high input ( configure digin HW for high-active )
  IsoAgLib::iDigitalI_c c_diginOnHigh( 0, IsoAgLib::iSensor_c::OnHigh );
  // create digin which deliers true for low input ( configure digin HW for low-active )
  IsoAgLib::iDigitalI_c c_diginOnLow( 1, IsoAgLib::iSensor_c::OnLow );

  // create Digin which uses hardware interrupt function
  // ( keep in mind, that most BIOS/OS will have some consolidation method to
  //   avoid reaction on erroneous signals - so use this very immediate reaction on your own risc )
  MyDiginIrqHandle myHandler;
  bool b_useStaticRead = false;
  IsoAgLib::iDigitalI_c c_diginIrqTest( 4, IsoAgLib::iSensor_c::OnHigh, b_useStaticRead, &myHandler );


  // create two simple analog inputs
  const bool cb_doMean = true;
  const bool cb_doFastAdc = false;
  IsoAgLib::iAnalogI_c c_analogVoltage(0, IsoAgLib::iSensor_c::voltage, cb_doMean, cb_doFastAdc );
  IsoAgLib::iAnalogI_c c_analogCurrent(1, IsoAgLib::iSensor_c::current, cb_doMean, cb_doFastAdc );


  // create analog inputs which provide range check
  // you can extend this idea if you derive your own classes from iAnalogIRangeCheck_c
  // so that they load their limits from given address at EEPROM
  // and can check if some more narrowed _warning_ limits are reached ( also read these limits from
  // eeprom -> read five values from EEPROM: channel, err_min, err_max, warn_min, warn_max )
  // ==>> after system init, you can simply check for GOOD, WARN, ERROR without knowing the exact settings
  //      at the position of check
  // ==>> if you place all of these variables in an array, you can simply loop through this array
  //      without the need to co-ordinate the channels and limits during time of check
  // range check voltage
  const uint16_t cui16_minVolt =  500; // MIN:  500 mV
  const uint16_t cui16_maxVolt = 4500; // MAX: 4500 mV
  IsoAgLib::iAnalogIRangeCheck_c c_analogRangeCheckVoltage( 2, IsoAgLib::iSensor_c::voltage, cb_doMean, cb_doFastAdc,
    cui16_minVolt, cui16_maxVolt);

  // range check current
  const uint16_t cui16_minCurrent =  500; // MIN:  500 mA
  const uint16_t cui16_maxCurrent = 4500; // MAX: 4500 mA
  IsoAgLib::iAnalogIRangeCheck_c c_analogRangeCheckCurrent( 3, IsoAgLib::iSensor_c::current, cb_doMean, cb_doFastAdc,
    cui16_minCurrent, cui16_maxCurrent);

  // create counter inputs
  // first counter which shall count input with high rate,
  // so that BIOS/OS standard functions can be used
  // ( BIOS can use hardware timers to evaluate with the help of interrupts the
  //   counter input )
  // use timer which overflows below 250msec ( to capture really quick rates )
  const uint16_t cui16_timerMaxOverflow = 250;
  const bool cb_doActiveHigh = true;  // evaluate high impulse
  const bool cb_doRisingEdge = false; // count falling edge
  IsoAgLib::iCounterI_c c_counterHighRate( 2, cui16_timerMaxOverflow, cb_doActiveHigh, cb_doRisingEdge );

  // capture counter signal where events occure with verly low rate
  // ( real world example: measure fuel flow )
  const uint16_t cui16_timerFuelMeasure = 5000; // count even rates with impulse distances of 5 sec
  IsoAgLib::iCounterI_c c_counterLowRate( 3, cui16_timerFuelMeasure, cb_doActiveHigh, cb_doRisingEdge );

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
      in the header xgpl_src/Application_Config/isoaglib_config.h
    - This can be also controlled during runtime with the function call:
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownByExplcitCall )
      or
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownOnCanEnLoss )
  */
  int32_t i32_nextDebug = 0;
  while ( IsoAgLib::iSystem_c::canEn() )
  { // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib - \ref IsoAgLib::iScheduler_c::timeEvent()
    getISchedulerInstance().timeEvent();

    // immediately re-loop if it's not yet time for debug messages
    if ( i32_nextDebug > IsoAgLib::iSystem_c::getTime() ) continue;
    // now it's time for debug
    i32_nextDebug = IsoAgLib::iSystem_c::getTime() + 1000;

    // example for access to sensors, if no direct access to the
    // corresponding variables ( e.g. c_diginOnHigh shall be read, but the access shall
    //  be performed from part of code which doesn't know where this variables is placed,
    //  but we know the channel )
    if ( IsoAgLib::getIsensorInstance().existDigital(0) )
    { // fine - c_diginOnHigh is already registered in IsoAgLib
      IsoAgLib::getIrs232Instance()
        << "c_diginOnHigh has value: "
        << uint16_t( IsoAgLib::getIsensorInstance().digital(0).active() )
        << "\r\n";
    }
    else
    { // something is wrong
      IsoAgLib::getIrs232Instance() << "ERROR: c_diginOnHigh is obviously not registered in IsoAgLib\r\n";
    }

    // well - in this case we can simply access the inputs with the variable
    IsoAgLib::getIrs232Instance() << "c_diginOnLow delivers: " << uint16_t(c_diginOnLow.active() ) << "\r\n";

    // read the analog values
    IsoAgLib::getIrs232Instance()
      << "Voltage at channel 0: " << c_analogVoltage.val() << " [mV]\r\n";
    IsoAgLib::getIrs232Instance()
      << "Current at channel 1: " << c_analogCurrent.val() << " [mA]\r\n";

    IsoAgLib::getIrs232Instance()
      << "Voltage at channel 2: " << c_analogRangeCheckVoltage.val() << " [mV]";
    // the following range check evaluations are possible without
    // knowledge of the limits itself as they are stored by iAnalogIRangeCheck_c
    // and can be accessed by the interface methods
    if ( c_analogRangeCheckVoltage.good() )
    {
      IsoAgLib::getIrs232Instance()
        << " which is GOOD as inside the limits ["
        << c_analogRangeCheckVoltage.getMinValid()
        << "..."
        << c_analogRangeCheckVoltage.getMaxValid()
        << "]\r\n";
    }
    else if ( c_analogRangeCheckVoltage.checkTooLow() )
    {
      IsoAgLib::getIrs232Instance()
        << " which is BAD as lower then allowed "
        << c_analogRangeCheckVoltage.getMinValid()
        << " [mV]\r\n";
    }
    else if ( c_analogRangeCheckVoltage.checkTooHigh() )
    {
      IsoAgLib::getIrs232Instance()
        << " which is BAD as higher then allowed "
        << c_analogRangeCheckVoltage.getMaxValid()
        << " [mV]\r\n";
    }
    // do less interpretation of current - simply check for error
    IsoAgLib::getIrs232Instance()
      << "Current at channel 1: " << c_analogRangeCheckVoltage.val() << " [mA]\r\n";
    if ( c_analogRangeCheckVoltage.error() )
    { // Bad: iAnalogIRangeCheck_c detected wrong value
      IsoAgLib::getIrs232Instance() << "ERROR: The value of c_analogRangeCheckVoltage is outside the limits\r\n";
    }

    // read quick counter
    IsoAgLib::getIrs232Instance()
      << "Quick counter at channel 2 has frequency: " << c_counterHighRate.frequency()
      << ", with period: " << c_counterHighRate.period()
      << "[msec] and counter value: " << c_counterHighRate.val()
      << "\r\n";

    // read slow counter
    IsoAgLib::getIrs232Instance()
      << "Slow counter at channel 3 has frequency: " << c_counterLowRate.frequency()
      << ", with period: " << c_counterLowRate.period()
      << "[msec], counter value: " << c_counterLowRate.val()
      << " and last event detected " << c_counterLowRate.lastSignalAge()
      << "[msec] ago\r\n";
  }
  return 1;
}

