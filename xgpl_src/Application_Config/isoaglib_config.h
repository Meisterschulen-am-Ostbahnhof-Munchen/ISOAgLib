/***************************************************************************
                          isoaglib_config.h  -  application specific
                                               configuration settings
                             -------------------
    begin                : Sun May 21 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
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
#ifndef _ISOAGLIB_CONFIG_H_
#define _ISOAGLIB_CONFIG_H_

/** \page Hardwareadoption Howto adopt IsoAgLib to new platform
  This page describes the needed steps to adopt the IsoAgLib to an new
  platform type.<ul>
  <li> define a new SYSTEM_XX define in the header isoaglib_config.h
  <li> comment out all the other SYSTEM_YY constants in isoaglib_config.h
  <li> create new #elif part in isoaglib_config.h to define subdirectory
     name bios_xy for the new platform
  <li> create new directories IsoAgLib/hal/XY (and
       optional supplementary_driver/hal/XY) as copy from
       IsoAgLib/hal/esx (and optional supplementary_driver/hal/esx)
  <li> adopt the files within IsoAgLib/hal/XY
       (and optional IsoAgLib/hal/XY) to your platform<ol>
    <li> config.h, typedef.h and errcodes.h platform specific config settings (e.g. type name for 16bit integer)
    <li> in each of the subdirectories
			FOO elem { can, eeprom, system (,actor, rs232, sensor ) }
    <ol>
			<li>\<FOO\>.h -> adopt calls to wanted target BIOS/OS
      <li>\<FOO\>_target_extensions.h/cc -> implement functions which aren't included in standard library of target ( i.e. which can't be adopted by simple name mapping and
			parameter reordering )
      <li>hal_can_interface.h/cc -> standardized CAN implementation ( simple name mapping is not suitable for CAN )
    </ol>
  </ol>
  </ul>
*/
/** allow configuration by parameter value "YES" */
#define YES 1
/** allow configuration by parameter value "NO" */
#define NO 0

/* ******************************************************** */
/**
 * \name Decide if the project uses a auto generated configuration
 *  header.
 */
/*@{*/
/** test if the autogenerated config header must be included */
#ifdef PRJ_USE_AUTOGEN_CONFIG
//	#define _autogen_config_header_ <Application_Config/.PRJ_USE_AUTOGEN_CONFIG>
	#define _autogen_config_header_ <PRJ_USE_AUTOGEN_CONFIG>
	#include _autogen_config_header_

	//#define Stringize(x) #x
	//#define ExpandAndStringize(x) Stringize(x)
	//#define IncFile ExpandAndStringize(ProjName.h)
#endif
/*@}*/


/* ******************************************************** */
/**
 * \name CONFIG_DEFAULT_POWERDOWN_STRATEGY Default strategy which is used by
 * IsoAgLib to decide on powerdown. As an automatic poweroff due to
 * detection of CanEn loss could cause unwanted system stopp on short
 * voltage low bursts of the power supply, it is appropriate to
 * stop the automatic powerdown in favour of an explicit call of
 * power off.
 * But this explicit handling could cause problems, if the application
 * reaches a state, where the watchdog is triggered, so that no reset
 * occurs, but the application doesn't either handle a loss of CanEn.
 * This could causes a never ending run of the ECU, if the application
 * can reach such a state.
 * Possible Settings: IsoAgLib::PowerdownByExplcitCall
 *                    or  IsoAgLib::PowerdownOnCanEnLoss
 */
/*@{*/
#ifndef CONFIG_DEFAULT_POWERDOWN_STRATEGY
	/** select Power Down trigger source:
			- IsoAgLib::PowerdownByExplcitCall for staying alive during CAN_EN loss,
					and allowing controlled powerdown after detection of CAN_EN loss
			- IsoAgLib::PowerdownOnCanEnLoss for automatic stop of ECU on detection of
					CAN_EN loss
	*/
	/// select reaction on powerdown detection - can be manually overridden in project config file
  #define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownByExplcitCall
#endif

#ifndef CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC
	/** select time intervall in MSEC during which the loss of CAN_EN is ignored.
			IMPORTANT: This slection has only influence on system if IsoAgLib::PowerdownByExplcitCall
			is selected, as this settings causes iSystem_c::canEn() to return _TRUE_ in shorter power loss
			phases than the time intervall specified here.
			The automatic power down of the ECU ( OS or BIOS ) can't be manipulated this way.
			IF iSystem_c::canEn() shall immediately answer false on CAN_EN loss, set this defined as
			UNDEFINED!!! - the handling of this buffer time is then compiled away
		*/
	/// define consolidation time for CAN_EN loss detection - can be manually overridden in project config file
	#define CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC 1000
#endif
/*@}*/


/* ******************************************************** */
/**
 * \name Set buffer sizes for CAN handling
 */
/*@{*/
#ifndef CONFIG_CAN_SEND_BUFFER_SIZE
	/// set puffer size for CAN send
	#define CONFIG_CAN_SEND_BUFFER_SIZE 20
#endif

#ifndef CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN
	/// set default puffer size for CAN receive
	#define CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN 15
#endif

#ifndef CONFIG_CAN_HIGH_LOAD_REC_BUF_SIZE_MIN
	/// high load of some CAN idents need larger buffers
	#define CONFIG_CAN_HIGH_LOAD_REC_BUF_SIZE_MIN 25
#endif

#ifndef CONFIG_CAN_BUF_INCREASE_ON_OVERLOAD
	/// set increment for automatic buffer increase after overload ( 0 == no increase )
	#define CONFIG_CAN_BUF_INCREASE_ON_OVERLOAD 5
#endif

#ifndef CONFIG_CAN_HIGH_LOAD_IDENT_LIST
	/// define list of CAN filters where large buffers are needed due to high load
	#define CONFIG_CAN_HIGH_LOAD_IDENT_LIST { 0x100, 0x200, 0x500 }
	/// define amount of list entries in CONFIG_CAN_HIGH_LOAD_IDENT_LIST
	#define CONFIG_CAN_HIGH_LOAD_IDENT_CNT 3
#endif


/** configure the amount of max filterBox instances to one msgObj instance
		static array for better quicker access to data.

		for ISO 11783 more FilterBox'es must be mapped into onte MsgObj
		for the different PGN's which are mainly managed by Base_c,
		and which are very similar)
*/
#ifndef CONFIG_CAN_ISO_FILTER_BOX_PER_MSG_OBJ
	/// Max amount of Filters which can be connected to a CAN MsgObj for ISO
  #define CONFIG_CAN_ISO_FILTER_BOX_PER_MSG_OBJ 15
#endif

/** configure the amount of max filterBox instances to one msgObj instance
		static array for better quicker access to data.

		DIN uses only some FilterBoxes which can be mapped
		by 5 entries in one MsgObj
*/
#ifndef CONFIG_CAN_DIN_FILTER_BOX_PER_MSG_OBJ
	/// Max amount of Filters which can be connected to a CAN MsgObj for DIN
  #define CONFIG_CAN_DIN_FILTER_BOX_PER_MSG_OBJ 5
#endif

#ifndef FILTER_BOX_PER_MSG_OBJ
	#ifdef USE_ISO_11783
		#define FILTER_BOX_PER_MSG_OBJ CONFIG_CAN_ISO_FILTER_BOX_PER_MSG_OBJ
	#else
		#define FILTER_BOX_PER_MSG_OBJ CONFIG_CAN_DIN_FILTER_BOX_PER_MSG_OBJ
	#endif
#endif
/*@}*/


#define WORD_LO_HI 0
#define WORD_HI_LO 1

/* ******************************************************** */
/**
 * \name Config access to RS232
 * Set the baudrate, buffer sizes and value coding.
 * These settings are used for initialisation on first access to RS232 ( pattern of singletons ).
 * All defines of this block can be overridden by project specific config file.
 */
/*@{*/
#ifndef CONFIG_RS232_DEFAULT_BAUDRATE
	/// define default RS232 baudrate - can be manually overridden in project config file
	#define CONFIG_RS232_DEFAULT_BAUDRATE 19200
#endif

#ifndef CONFIG_RS232_DEFAULT_DATA_MODE
	/// define default RS232 bit encoding - can be manually overridden in project config file
	#define CONFIG_RS232_DEFAULT_DATA_MODE RS232IO_c::_8_N_1
#endif

#ifndef CONFIG_RS232_DEFAULT_XON_XOFF
	/// define default RS232 XON-XOFF usage - can be manually overridden in project config file
	#define CONFIG_RS232_DEFAULT_XON_XOFF false
#endif

#ifndef CONFIG_RS232_DEFAULT_SND_PUF_SIZE
	/// define default size of RS232 send puffer - can be manually overridden in project config file
	#define CONFIG_RS232_DEFAULT_SND_PUF_SIZE 200
#endif

#ifndef CONFIG_RS232_DEFAULT_REC_PUF_SIZE
	/// define size of RS232 receive puffer - can be manually overridden in project config file
	#define CONFIG_RS232_DEFAULT_REC_PUF_SIZE 10
#endif
/*@}*/

/* ******************************************************** */
/**
 * \name Basic config for sensor and actor
 * Set default PWM value for digital output and
 * calculation factors for analog input.
 * All defines of this block can be overridden by project specific config file.
 */
/*@{*/
#ifndef CONFIG_PWM_DEFAULT_FREQUENCY
	/// set default PWM frequency for digital output - can be manually overridden in project config file
	#define CONFIG_PWM_DEFAULT_FREQUENCY 100000
#endif
/*@}*/


/* ******************************************************** */
/**
 * \name Basic config of Hdd access
 * These settings are used for initialisation on first RS232 based HDD access ( pattern of singletons ).
 * All defines of this block can be overridden by project specific config file.
 */
/*@{*/
#ifndef CONFIG_HDD_READ_PUFFER_SIZE
	/// define size of buffer for Hdd read access - can be manually overridden in project config file
	#define CONFIG_HDD_READ_PUFFER_SIZE 100
#endif

#ifndef CONFIG_HDD_READ_BLOCK_SIZE
	/// define minimum size of data to read as one bock from Hdd - can be manually overridden in project config file
	#define CONFIG_HDD_READ_BLOCK_SIZE 20
#endif

#ifndef CONFIG_HDD_MAX_TOKEN_LEN
	/// define longest token which is used to translate printed numbers to number variables - can be manually overridden in project config file
	#define CONFIG_HDD_MAX_TOKEN_LEN 10
#endif

#ifndef CONFIG_HDD_DEFAULT_DATA_MODE
	/// define value coding for RS232 connected DOS Drive - can be manually overridden in project config file
	#define CONFIG_HDD_DEFAULT_DATA_MODE RS232IO_c::_8_N_1
#endif

/*@}*/

/* ******************************************************** */
/**
 * \name Different time intervalls
 * Define execution time of timeEvent calls
 * and choose time for clearing of stale monitor list entries
 * All defines of this block can be overridden by project specific config file.
 */
/*@{*/
/** default maximum time to perform the function iScheduler_c::timeEvent
		if this function isn't called with a defined termination timestamp.
		This is needed to avoid a deadlock which could happen if processing
		and reaction on received CAN messages causes a never endling loop of
		new received messages - or in other words: if the CAN message queues
		don't ever get empty during the processing.
		If this time limit is reached, then something is probably quite broken
		on CAN BUS.
	*/
#ifndef CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME
	/// default max execution time of iScheduler_c::timeEvent - can be manually overridden in project config file
	#define CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME 500
#endif

/** time to delete inactive ISO items in monitor list [msec]
    - if CONFIG_ISO_ITEM_MAX_AGE is not defined, no items are deleted
*/
#ifndef CONFIG_ISO_ITEM_MAX_AGE
	/// erase ISO 11783 items after time (>0), on missing address claim after request - can be manually overridden in project config file
	#define CONFIG_ISO_ITEM_MAX_AGE 3000
#endif

/** time to delete inactive member items in monitor list [msec]
    - if CONFIG_DIN_ITEM_MAX_AGE is not defined, no items are deleted
		- set to 0 to deactivate
*/
#ifndef CONFIG_DIN_ITEM_MAX_AGE
	/// erase DIN 9684 devices from monitor list after given dead time ( 0 == no delete ) - can be manually overridden in project config file
	#define CONFIG_DIN_ITEM_MAX_AGE 0
#endif

/** time to delete inactive service items in monitor list [msec]
    - if CONFIG_DIN_SERVICE_MAX_AGE is not defined, no items are deleted
		- set to 0 to deactivate
*/
#ifndef CONFIG_DIN_SERVICE_MAX_AGE
	/// erase DIN 9684 services from monitor list after given dead time ( 0 == no delete ) - can be manually overridden in project config file
	#define CONFIG_DIN_SERVICE_MAX_AGE 3000
#endif

/** interval in sec to store actual process data value for process datas which are
    permanent in EEPROM
*/
#ifndef CONFIG_PROC_STORE_EEPROM_INTERVAL
	/// interval [ms] for value update in EEPROM for local process data - can be manually overridden in project config file
	#define CONFIG_PROC_STORE_EEPROM_INTERVAL 5000
#endif
/*@}*/


/* ******************************************************** */
/**
 * \name Basic BUS and MsgObj settings for CAN access
 */
/*@{*/
#ifndef CONFIG_CAN_DEFAULT_BUS_NUMBER
	/// CAN BUS number for IsoAgLib (0xFF forces explicit call of init, to open the CAN BUS )
	#define CONFIG_CAN_DEFAULT_BUS_NUMBER 0xFF
#endif

#ifndef CONFIG_CAN_DEFAULT_SEND_OBJ
	/// define default MsgObj_c number to use for CAN send
	#define CONFIG_CAN_DEFAULT_SEND_OBJ 0
#endif

#ifndef CONFIG_CAN_DEFAULT_MIN_OBJ_NR
	/// define default min number for CAN Msg-Obj used for IsoAgLib
	#define CONFIG_CAN_DEFAULT_MIN_OBJ_NR 0
#endif

#ifndef CONFIG_CAN_DEFAULT_MAX_OBJ_NR
	/// define default max number for CAN Msg-Obj used for IsoAgLib
	#define CONFIG_CAN_DEFAULT_MAX_OBJ_NR 13
#endif
/*@}*/




/* ******************************************************** */
/**
 * \name Config detection of CAN BUS off
 * For autosense of ISO 11783 and DIN 9684 an runtime error
 * can occur, if a system starts with 250kbaud
 * and later a simple DIN 9684 device is connected with 125kbaud
 * -> IsoAgLib must detect BUS OFF and interprete it as result
 * of differing CAN baudrates ==> IsoAgLib can switch baudrate
 * to fallback speed.
 */
/*@{*/
#ifndef CONFIG_CAN_USE_FALLBACK_ON_CAN_DEADLOCK
	/// select if CAN should change baudrate to fallback, if BUS deadlock is detected
	#define CONFIG_CAN_USE_FALLBACK_ON_CAN_DEADLOCK 0
#endif

#ifndef CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING
	/// max time intervall of CAN err before switching back from 250 to 125 kbit/s in [msec.]
	#define CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING 3000
#endif

#ifndef CONFIG_CAN_FALLBACK_BITRATE
	/// define bitrate to use after fallbcack /ref CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING
	#define CONFIG_CAN_FALLBACK_BITRATE 125
#endif

/** define max wait time between call of sendCan to
    successfull send of the msg,
    longer wait time is regarded of evidence for BUS OFF
*/
#ifndef CONFIG_CAN_MAX_SEND_WAIT_TIME
	/// BUS OFF if CAN send lasts longer then defined
	#define CONFIG_CAN_MAX_SEND_WAIT_TIME 200
#endif

/** define max normal CAN idle time (e.g. no msg received)
    -> used to detect general CAN problem
*/
#ifndef CONFIG_CAN_MAX_CAN_IDLE
	/// BUS problem, if no message received in this time
	#define CONFIG_CAN_MAX_CAN_IDLE 3000
#endif
/*@}*/

/* ******************************************************** */
/**
 * \name Configure the use of an EEPROM Editor
 * Enable EEPROM data can be read and
 * write during runtime via RS232 or CAN
 */
/*@{*/
/**
  * @def CONFIG_EEPROM_USE_CAN_EDITOR_YN
  * use CAN connected EEPROM editor
  */
  #ifndef CONFIG_EEPROM_USE_CAN_EDITOR_YN
		/// YES | NO for EEPROM editor access via CAN
    #define CONFIG_EEPROM_USE_CAN_EDITOR_YN NO
  #endif
  #if CONFIG_EEPROM_USE_CAN_EDITOR_YN == YES && !defined(USE_CAN_EEPROM_EDITOR)
    #define USE_CAN_EEPROM_EDITOR
  #endif

/**
  * @def CONFIG_EEPROM_USE_RS232_EDITOR_YN
  * use RS232 connected EEPROM editor
  */
  #ifndef CONFIG_EEPROM_USE_RS232_EDITOR_YN
		/// YES | NO for EEPROM editor access via RS232
    #define CONFIG_EEPROM_USE_RS232_EDITOR_YN NO
  #endif
  #if CONFIG_EEPROM_USE_RS232_EDITOR_YN == YES && !defined(USE_RS232_EEPROM_EDITOR)
    #define USE_RS232_EEPROM_EDITOR
  #endif


#ifndef CONFIG_EEPROM_USE_CAN_REC_IDENT
	/// set CAN Ident_c for filtering out EEEditor msgs
	#define CONFIG_EEPROM_USE_CAN_REC_IDENT 0x700
#endif

#ifndef CONFIG_EEPROM_USE_CAN_REC_OBJ
	/** set CAN Msg obejct for receiving EEEditor msg */
	#define CONFIG_EEPROM_USE_CAN_REC_OBJ   13
#endif

#ifndef CONFIG_EEPROM_USE_CAN_SEND_OBJ
	/** set CAN Msg obejct for sending EEEditor msg */
	#define CONFIG_EEPROM_USE_CAN_SEND_OBJ  14
#endif

#ifndef CONFIG_EEPROM_USE_CAN_BUS
	/** set CAN BUS number for CAN EEPROM Editor */
	#define CONFIG_EEPROM_USE_CAN_BUS       0
#endif

#ifndef CONFIG_EEPROM_USE_CAN_BUFFER_SIZE
	/** set CAN puffer size for receive and send */
	#define CONFIG_EEPROM_USE_CAN_BUFFER_SIZE    10
#endif

#ifndef CONFIG_EEPROM_USE_CAN_EXT_IDENT
	/** to use standard ident (11bit) ->0, 29bit -> 1 */
	#define CONFIG_EEPROM_USE_CAN_EXT_IDENT 0
#endif
/*@}*/

/* ******************************************************** */
/**
 * \name Config basic properties of process data handling
 * Choose basic properties for process data handling
 */
/*@{*/
/// define special command value for release of setpoint
#define SETPOINT_RELEASE_COMMAND  NO_VAL_32S
/// define special command value for signaling setpoint error
#define SETPOINT_ERROR_COMMAND  ERROR_VAL_32S

/**
  * @def RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT_YN
  * some systems send exact setpoint cmd with value 0 to reset a measurement value
  */
  #ifndef RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT_YN

    #define RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT_YN NO
  #endif
  #if RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT_YN == YES && !defined(RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT)
    #define RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT
  #endif


/**
  * @def LAV_COMMENT_MEASURE_PROG_CMD_YN
  * LAV_COMMENT_MEASURE_PROG_CMD allows usage of specific start/stop/reset cmd values
  */
  #ifndef LAV_COMMENT_MEASURE_PROG_CMD_YN
    #define LAV_COMMENT_MEASURE_PROG_CMD_YN NO
  #endif
  #if LAV_COMMENT_MEASURE_PROG_CMD_YN == YES && !defined(LAV_COMMENT_MEASURE_PROG_CMD)
    #define LAV_COMMENT_MEASURE_PROG_CMD
  #endif

/**
  * @def SIMPLE_SETPOINT_WITH_PERCENT_YN
  * choose if simple setpoint should support percent setpoints
  */

  #ifndef SIMPLE_SETPOINT_WITH_PERCENT_YN
    #define SIMPLE_SETPOINT_WITH_PERCENT_YN YES
  #endif
  #if SIMPLE_SETPOINT_WITH_PERCENT_YN == YES && !defined(SIMPLE_SETPOINT_WITH_PERCENT)
    #define SIMPLE_SETPOINT_WITH_PERCENT
  #endif

/**
  * @def SIMPLE_RESPOND_ON_SET_YN
  * choose if local process data should simply respond received setpoint
  * value after storing it (like Fieldstar (tm) )
  */
  #ifndef SIMPLE_RESPOND_ON_SET_YN
    #define SIMPLE_RESPOND_ON_SET_YN YES
  #endif
  #if SIMPLE_RESPOND_ON_SET_YN == YES && !defined(SIMPLE_RESPOND_ON_SET)
    #define SIMPLE_RESPOND_ON_SET
  #endif

/**
  * @def HANDLE_SETPOINT_MEASURE_EQUIVALENT_YN
  * choose if simple setpoint shouldn't distinguish between measurement and setpoint value
  */
  #ifndef HANDLE_SETPOINT_MEASURE_EQUIVALENT_YN
    #define HANDLE_SETPOINT_MEASURE_EQUIVALENT_YN NO
  #endif
  #if HANDLE_SETPOINT_MEASURE_EQUIVALENT_YN == YES && !defined(HANDLE_SETPOINT_MEASURE_EQUIVALENT)
    #define HANDLE_SETPOINT_MEASURE_EQUIVALENT
  #endif

/*@}*/

/* ******************************************************** */
/**
 * \name Define used optional modules
 * Define which of the configurable
 * services should be compiled and used.
 * Set the single ZZ_YN defines to YES / NO or
 * define the equivalent ZZ constant.
 */
/*@{*/
/**
  * @def USE_ISO_11783_YN
  * choose the use of ISO 11783
  */
  #ifndef USE_ISO_11783_YN
    #define USE_ISO_11783_YN NO
  #endif
  #if USE_ISO_11783_YN == YES && !defined(USE_ISO_11783)
    #define USE_ISO_11783
  #endif

/**
  * @def USE_ISO_TERMINAL_YN
  * choose the use of ISO TERMINAL
  */
  #ifndef USE_ISO_TERMINAL_YN
    #define USE_ISO_TERMINAL_YN NO
  #endif
  #if USE_ISO_TERMINAL_YN == YES && !defined(USE_ISO_TERMINAL) && defined( USE_ISO_11783 )
    #define USE_ISO_TERMINAL
  #endif

/**
  * @def USE_DIN_9684_YN
  * choose the use of DIN 9684
  */
  #ifndef USE_DIN_9684_YN
    #define USE_DIN_9684_YN NO
  #endif
  #if USE_DIN_9684_YN == YES && !defined(USE_DIN_9684)
    #define USE_DIN_9684
  #endif

/**
  * @def USE_DIN_TERMINAL_YN
  * choose the use of DIN TERMINAL
  */
  #ifndef USE_DIN_TERMINAL_YN
    #define USE_DIN_TERMINAL_YN NO
  #endif
  #if USE_DIN_TERMINAL_YN == YES && !defined(USE_DIN_TERMINAL) && defined( USE_DIN_9684 )
    #define USE_DIN_TERMINAL
  #endif

#define EXCLUDE_RARE_DIN_SYSTEM_CMD

/**
  * @def USE_BASE_YN
  * activate module for base data ( periodic sent main tractor data like speed, PTO, Hitch )
  * ( even if IsoAgLib is mainly independent from this setting, the IsoItem_c::processMsg()
	*   requires this setting, to detect if a call to Base_c:processMsg() is defined -
	*   this call is performed if a REQUEST_PGN_MSG_PGN is processed by IsoItem_c and
	*   the requested PGN is TIME_DATE_PGN )
  */
  #ifndef USE_PROCESS_YN
    #define USE_PROCESS_YN YES
  #endif
  #if USE_PROCESS_YN == YES && !defined(USE_PROCESS)
    #define USE_PROCESS
  #endif

/**
  * @def USE_EEPROM_IO_YN
  * activate module for hardware EEPROM access via IsoAgLib
 * this is important as local process data can then store
 * actual values in confiugrable rates into EEPROM
  */
  #ifndef USE_EEPROM_IO_YN
     #define USE_EEPROM_IO_YN YES
  #endif
  #if USE_EEPROM_IO_YN == YES && !defined(USE_EEPROM_IO)
    #define USE_EEPROM_IO
  #endif


/*@}*/

/* ******************************************************** */
/**
 * \name Define usage of float data type for project.
 *	Relevant for process data, RS232 and some other places
 */
/*@{*/
/**
  * @def USE_FLOAT_DATA_TYPE_YN
  * define whether Process_c process data should provide
  * floating point
  */
  #ifndef USE_FLOAT_DATA_TYPE_YN
    #define USE_FLOAT_DATA_TYPE_YN NO
  #endif
  #if USE_FLOAT_DATA_TYPE_YN == YES && !defined(USE_FLOAT_DATA_TYPE)
    #define USE_FLOAT_DATA_TYPE 1

  #endif
/*@}*/



/* ******************************************************** */
/**
 * \name Definition of IsoAgLib managed CAN ports and dependent access macros
 * The several modules of the IsoAgLib have only one instance, as long
 * as only one CAN port is managed with IsoAgLib (DIN and/or ISO) protocol.
 * Even in case of two or more CAN ports, the so called "singleton" pattern
 * can be used to define a secure way to handle these objects as standalone
 * instances, which access the other object types by global accessible
 * static functions. This eliminates the need of pointers between the
 * objects.
 */
/*@{*/
/** configs according to CAN hardware
    define amount of available CAN buses
		- the bus number for all configuration function calls is
		  verified against this setting
*/
#ifndef CAN_BUS_CNT
	#define CAN_BUS_CNT 1
#endif

/** set the amount of CANIO_c instanes managed by System_c
  * normaly one, but if more CAN BUS'es should be managed
  * System_c can manage them seperate f.e. one for IsoAgLib and the other
  * for system/machine internal CAN communication
*/
#ifndef CAN_INSTANCE_CNT
  #define CAN_INSTANCE_CNT 1
#endif

/** count of CAN ports which are managed with IsoAgLib (DIN and/or ISO)
		- in most cases just 1
		- but if the ECU shall be used as gateway between two ISO 11783 or DIN 9684 subnets
		- or if it should translate between a DIN 9684 devices and ISO 11783 network
		- then select 2 ( or more )
		( all communication classes are designed to allow independent control of
		  individual CAN instances per protocol instance set )
	*/
#ifndef PRT_INSTANCE_CNT
  #define PRT_INSTANCE_CNT 1
#endif

#ifndef DEFAULT_BITRATE
	#ifdef USE_DIN_9684
		/// define DEFAULT BITRATE
		#define DEFAULT_BITRATE 125
	#else
		/// define DEFAULT BITRATE
		#define DEFAULT_BITRATE 250
	#endif
#endif


/*@}*/

/* ******************************************************** */
/**
 * \name Basic identifier settings for CAN access
 */
/*@{*/
/** set the type of mask (uint32_t, uint16_t)
  * -# for extended 29bit header CAN 2.0B
  *    (this can handle both extended AND standard
  *    but it reserves enough space for 29bit ident)

  * -# for standard 11bit header CAN 2.0A
*/
#ifndef MASK_TYPE
	#ifdef USE_ISO_11783
		#define MASK_TYPE uint32_t
	#else
		#define MASK_TYPE uint16_t
	#endif
#endif


/// define default ident type
#define DEFAULT_IDENT_TYPE __IsoAgLib::Ident_c::StandardIdent

#ifdef USE_ISO_11783
  /// config CANIO_c to handle both 11bit and 29bit ident types if ISO 11783 is active
  #define DEFAULT_CONFIG_IDENT_TYPE __IsoAgLib::Ident_c::BothIdent
#else
  /// config CANIO_c to only 11bit ident type if ISO 11783 is NOT active
  #define DEFAULT_CONFIG_IDENT_TYPE __IsoAgLib::Ident_c::StandardIdent
#endif


/** default values for global masks
    only use first 3 bits (PRI)
    for CAN 2.0A 11bit
*/
#define DEFAULT_CAN_GLOB_MASK 0x0700
/** default values for global masks
    only use first 3 bits (PRI)
    for CAN 2.0B 29bit
*/
#define DEFAULT_CAN_MASK_LASTMSG 0x00000000
/*@}*/

/* ******************************************************** */
/**
 * \name Define PGN codes of ISO 11783 messages
 * As the ISO 11783 is not yet published during implementation
 * the PGN codes can change -> centralised definition
 */
/*@{*/


// <DESTINATION> PGNs

#define ETP_DATA_TRANSFER_PGN  0x00C700LU
#define ETP_CONN_MANAGE_PGN    0x00C800LU
#define PROCESS_DATA_PGN       0x00CB00LU
#define VT_TO_GLOBAL_PGN       0x00E6FFLU
#define VT_TO_ECU_PGN          0x00E600LU
#define ECU_TO_VT_PGN          0x00E700LU
#define REQUEST_PGN_MSG_PGN    0x00EA00LU
#define TP_DATA_TRANSFER_PGN   0x00EB00LU
#define TP_CONN_MANAGE_PGN     0x00EC00LU
#define ADRESS_CLAIM_PGN       0x00EE00LU

// <NO DESTINATION> PGNs

#define WORKING_SET_MEMBER_PGN      0x00FE0CLU
#define WORKING_SET_MASTER_PGN      0x00FE0DLU
#define LANGUAGE_PGN                0x00FE0FLU
#define BACK_PTO_STATE_PGN          0x00FE43LU
#define FRONT_PTO_STATE_PGN         0x00FE44LU
#define BACK_HITCH_STATE_PGN        0x00FE45LU
#define FRONT_HITCH_STATE_PGN       0x00FE46LU
#define MAINTAIN_POWER_REQUEST_PGN  0x00FE47LU
#define WHEEL_BASED_SPEED_DIST_PGN  0x00FE48LU
#define GROUND_BASED_SPEED_DIST_PGN 0x00FE49LU
#define TIME_DATE_PGN               0x00FEE6LU

/** PGN for GPS state information - according to open accessible sources
	-> might be wrong
	-> please test and indicate correction need or successfull usage back to autthor
	*/
#define GPS_STATE_PGN 0x00FFF3LU
/** PGN for GPS latitude and longitude - according to open accessible sources
	-> might be wrong
	-> please test and indicate correction need or successfull usage back to autthor
	*/
#define GPS_LATITUDE_LONGITUDE_PGN 0x00FEF3LU
/** PGN for GPS speed, heading and altitude - according to open accessible sources
	-> might be wrong
	-> please test and indicate correction need or successfull usage back to autthor
	*/
#define GPS_SPEED_HEADING_ALTITUDE_PGN 0x00FEE8LU

/*@}*/


/* ******************************************************** */
/**
 * \name Definition of Host Target System Type
 * Set one of the following SYSTEM_XX_YN defines to YES
 * or define one of the SYSTEM_XX in your project definition.
 */
/*@{*/

/** this platform specifying constants can be defined here or in project def file */
/** @def SYSTEM_PC_YN
  * set this define to YES if IsoAgLib is used for a PC
  */
  #ifndef SYSTEM_PC_YN
    #define SYSTEM_PC_YN NO
  #endif
  #if SYSTEM_PC_YN == YES && !defined(SYSTEM_PC)
    #define SYSTEM_PC
  #endif

/** @def SYSTEM_ESX_YN
  * set this define to YES if IsoAgLib is used for the ECU "ESX" of STW
  */
  #ifndef SYSTEM_ESX_YN
    #define SYSTEM_ESX_YN NO
  #endif
  #if SYSTEM_ESX_YN == YES && !defined(SYSTEM_ESX)
    #define SYSTEM_ESX
  #endif

/** @def SYSTEM_IMI_YN
  * set this define to YES if IsoAgLib is used for the computing unit "IMI" of STW
  */
  #ifndef SYSTEM_IMI_YN
    #define SYSTEM_IMI_YN NO
  #endif
  #if SYSTEM_IMI_YN == YES && !defined(SYSTEM_IMI)
    #define SYSTEM_IMI
  #endif

/** @def SYSTEM_IMI_2CAN_YN

  * set this define to YES if IsoAgLib is used for the computing unit "IMI" with 2 CAN BUS of STW
  */
  #ifndef SYSTEM_IMI_2CAN_YN
    #define SYSTEM_IMI_2CAN_YN NO
  #endif
  #if SYSTEM_IMI_2CAN_YN == YES && !defined(SYSTEM_IMI_2CAN)
    #define SYSTEM_IMI_2CAN
  #endif
  #if defined(SYSTEM_IMI_2CAN) && !defined(SYSTEM_IMI)
    #define SYSTEM_IMI
  #endif

/** @def SYSTEM_PM167_YN
  * set this define to YES if IsoAgLib is used for the processor module "PM167" of STW
  */
  #ifndef SYSTEM_PM167_YN
    #define SYSTEM_PM167_YN NO
  #endif
  #if SYSTEM_PM167_YN == YES && !defined(SYSTEM_PM167)
    #define SYSTEM_PM167
  #endif

/** @def SYSTEM_MITRON167_YN
  * set this define to YES if IsoAgLib is used for the processor module "PM167" of STW
  */
  #ifndef SYSTEM_MITRON167_YN
    #define SYSTEM_MITRON167_YN NO
  #endif
  #if SYSTEM_MITRON167_YN == YES && !defined(SYSTEM_MITRON167)
    #define SYSTEM_MITRON167
  #endif

#ifdef SYSTEM_PC_VC
#define SYSTEM_PC
#endif
/*@}*/

/* ******************************************************** */
/**
 * \name Define subdirectory name dependent on platform type
 * for adoption to new SYSTEM_XX add new #elif part to set
 * according subdirectory name in IsoAgLib/hal/
 * Additionally define the offset value of the lowest possible
 * CAN MsgObj ( STW BIOS starts counting with 1, other start with 0 )
 */
/*@{*/
#if defined(SYSTEM_PC)
  /// set subdirectory name to "pc" for PC platform
  #define HAL_SUBDIR pc
#elif defined(SYSTEM_ESX)
  /// set subdirectory name to "esx" for ESX platform
  #define HAL_SUBDIR esx
#elif defined(SYSTEM_IMI)
  /// set subdirectory name to "imi" for IMI platform
  #define HAL_SUBDIR imi
#elif defined(SYSTEM_PM167)
  /// set subdirectory name to "pm167" for Procesor Module 167 platform
  #define HAL_SUBDIR pm167
#elif defined(SYSTEM_MITRON167)
  /// set subdirectory name to "mitron167" for Mitron CAN Proxy based on C167 platform
  #define HAL_SUBDIR mitron167
  // HAL has no EEPROM control -> overwrite any setting of EEPROM use
  #ifdef USE_EEPROM_IO_YN
    #undef USE_EEPROM_IO_YN
  #endif
  #define USE_EEPROM_IO_YN NO
  #ifdef USE_EEPROM_IO
    #undef USE_EEPROM_IO
  #endif
#endif
#ifndef gluetoks
  #define gluetoks(a, b)  a ## b
  #define gluetoks3(a, b, c)  a ## b ## c
  #define gluetoks4(a, b, c, d)  a ## b ## c ## d
  #define gluetoks5(a, b, c, d, e)  a ## b ## c ## d ## e
#endif
#ifndef gluemacs
  #define gluemacs(a, b)  gluetoks(a, b)
  #define gluemacs3(a, b, c)  gluetoks3(a, b, c)
  #define gluemacs4(a, b, c, d)  gluetoks4(a, b, c, d)
  #define gluemacs5(a, b, c, d, e)  gluetoks5(a, b, c, d, e)
#endif

#define HAL_PATH gluemacs( <IsoAgLib/hal/, HAL_SUBDIR )
/*@}*/


#endif
