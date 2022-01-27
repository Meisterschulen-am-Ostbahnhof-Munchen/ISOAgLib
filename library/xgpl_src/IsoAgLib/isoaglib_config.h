/*
  isoaglib_config.h: application specific configuration settings

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _ISOAGLIB_CONFIG_H_
#define _ISOAGLIB_CONFIG_H_

#define INCLUDED_FROM_ISOAGLIB_CONFIG
#include <isoaglib_project_config.h>
#undef INCLUDED_FROM_ISOAGLIB_CONFIG

#include <IsoAgLib/util/compiler_adaptation.h>
#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/hal/hal_config.h>

#define ISOAGLIB_VERSION_MAJOR 2
#define ISOAGLIB_VERSION_MINOR 9
#define ISOAGLIB_VERSION_PATCH 2
#define ISOAGLIB_VERSION_FIX 0
#define ISOAGLIB_VERSION_RC -1
// RC < 0 => Changes in Trunk, preparing Next Release.
// RC = 0 => Final Release.
// RC = 1 => Release Candidate 1, etc.


/***************************************/
/* ***** Check mandatory defines ***** */
/***************************************/

#if (!defined( OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN ) && !defined( OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN ) )
#  error "Please specify little or big endian!"
#endif

#if (defined( OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN ) && defined( OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN ) )
#  error "Please specify EITHER little or big endian, not both!"
#endif

#if FLOAT_WORD_ORDER == WORD_HI_LO
#elif FLOAT_WORD_ORDER == BYTE_HI_LO
#elif FLOAT_WORD_ORDER == WORD_LO_HI
#else
#  error "PLEASE set FLOAT_WORD_ORDER to either WORD_HI_LO, BYTE_HI_LO or WORD_LO_HI in the config.h from your target"
#endif


#if !defined(CAN_INSTANCE_CNT)
#  error "CAN_INSTANCE_CNT not set."
#endif


#if defined(USE_ISO_11783)
#  if !defined(DEF_Stream_h_IMPL)
#    error "DEF_Stream_h_IMPL is not set. Re-run of a current conf2build.sh seems to be necessary."
#  endif
#  if !defined(DEF_Stream_c_IMPL)
#    error "DEF_Stream_c_IMPL is not set. Re-run of a current conf2build.sh seems to be necessary."
#  endif
#  if !defined(PRT_INSTANCE_CNT)
#    error "USE_ISO_11783 set but PRT_INSTANCE_CNT not set."
#  endif
#  define PROP_INSTANCE_CNT (CAN_INSTANCE_CNT - PRT_INSTANCE_CNT)
#else
#  define PROP_INSTANCE_CNT (CAN_INSTANCE_CNT)
#endif


#if defined(USE_RS232) 
#  if !defined(RS232_CHANNEL_CNT)
#    error "USE_RS232 set but RS232_CHANNEL_CNT not set."
#  endif
#  if !defined(RS232_INSTANCE_CNT)
#    error "USE_RS232 set but RS232_INSTANCE_CNT not set."
#  endif
#endif

/* ******************************************************** */
/**
 * \name Set buffer sizes for CAN handling
 */

#ifndef CONFIG_CAN_NO_SEND_BUFFER_INFO_FALLBACK_MULTISEND
#  define CONFIG_CAN_NO_SEND_BUFFER_INFO_FALLBACK_MULTISEND 20
#endif

/** configure the MULTI-PACKET parameters (TP/ETP/FP/Chunks)
    for RECEPTION, i.e. MultiReceive-related classes
*/
#ifndef CONFIG_MULTI_RECEIVE_CHUNK_SIZE_IN_PACKETS
  /// Multiply this SizeInPackets by 7 to get the size of ONE CHUNK (ONLY OF INTEREST IF STREAM IS CHUNKED)
#  define CONFIG_MULTI_RECEIVE_CHUNK_SIZE_IN_PACKETS 16
#endif


/** uppper limit for multisend to calculate the amount of frames 
 * to send
 */
#ifndef CONFIG_MULTI_SEND_MAX_PKG_PER_TIMEEVENT 
#  define CONFIG_MULTI_SEND_MAX_PKG_PER_TIMEEVENT 20
#endif

/** minimum number of free messages left in the send buffer for other messages in multisend
 */
#ifndef CONFIG_MULTI_SEND_BUFFER_MIN_FREE_COUNT 
#  define CONFIG_MULTI_SEND_BUFFER_MIN_FREE_COUNT 5
#endif

/** configures the maximum amount of packets to be allowed by all "CTS" messages.
    So you need to have at least a CAN-buffer for this many packets,
    because the clients will burst them and you cannot guarantee to handle
    them all immediately. Note that you of course need to have an even
    larger CAN-buffer, because other packets are also being received,
    like GPS data, Tractor data, network management, etc.etc.
    Be aware: This is only the part for MULTI_RECEIVE!
*/
#ifndef CONFIG_MULTI_RECEIVE_MAX_OVERALL_PACKETS_ADDED_FROM_ALL_BURSTS
  /// Maximum amount of packets to be allowed to be CTS'd in parallel (this amount will be distributed among all running streams)
#  define CONFIG_MULTI_RECEIVE_MAX_OVERALL_PACKETS_ADDED_FROM_ALL_BURSTS 16
#endif

/** configures the maximum amount of packets to be allowed by a "CTS" message.
    This is a maximum-cutoff value per client. MultiReceive will normally
    CTS for "CONFIG_MULTI_RECEIVE_MAX_BURST_IN_PACKETS_OVERALL / Number of clients" packets
    Use this value to limit if e.g. just one stream is running.
    Example: CONFIG_MULTI_RECEIVE_MAX_BURST_IN_PACKETS_OVERALL is set to 80 because you want 5 clients
    to be able to each send 16 packets, BUT you don't want if one single client
    is connected to allow 80 packets to be sent at once!
*/
#ifndef CONFIG_MULTI_RECEIVE_MAX_PER_CLIENT_BURST_IN_PACKETS
  /// Maximum amount of packets to be allowed by a "CTS" message.
#  define CONFIG_MULTI_RECEIVE_MAX_PER_CLIENT_BURST_IN_PACKETS CONFIG_MULTI_RECEIVE_MAX_OVERALL_PACKETS_ADDED_FROM_ALL_BURSTS
#endif

/** This delay is being used if EXACTLY ONE stream (1) is being received! */
#ifndef CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_SINGLE_STREAM
  /// defines the amount of msec. to be wait until sending out the next CTS to the sender IF ONE STREAM ONLY BEING RECEIVED
#  define CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_SINGLE_STREAM 0
#endif

/** This delay is being used if MORE THAN ONE stream (2, 3, 4, ...) is being received */
#ifndef CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_MULTI_STREAMS
  /// defines the amount of msec. to be wait until sending out the next CTS to the sender IF MULTIPLE STREAMS BEING RECEIVED
#  define CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_MULTI_STREAMS 0
#endif


/*@}*/

#define Stringize(x) #x
#define ExpandAndStringize(x) Stringize(x)

#define WORD_LO_HI 0
#define WORD_HI_LO 1
#define BYTE_HI_LO 2

// WORD_LO_HI (==BYTE_LO_HI) --> typical 32bit LITTLE_ENDIAN
// WORD_HI_LO                --> typical 16bit LITTLE_ENDIAN
// BYTE_HI_LO                --> typical 32bit BIG_ENDIAN

/* ******************************************************** */
/**
 * \name Set configuration parameter for CAN FIFO handling. 
 * Please note that the CAN-FIFO is now in use regardless of the used HAL!
 *
 * Exponent of the 2^N operation, used to determine the BufferSize of s_canFifoInstance
 * NOTE : The CAN_FIFO_EXPONENT_BUFFER_SIZE must be less than  TARGET_WORDSIZE -1,
 * otherwise the overflow of the UC and AC counter can lead to loss of CAN message.
 */
#ifndef CAN_FIFO_EXPONENT_BUFFER_SIZE
#  define CAN_FIFO_EXPONENT_BUFFER_SIZE 8
#endif

/* ******************************************************** */
/**
 * \name Different time intervalls
 * Define execution time of timeEvent calls
 * and choose time for clearing of stale monitor list entries
 * All defines of this block can be overridden by project specific config file.
 */
/*@{*/

/** time to delete inactive ISO items in monitor list [msec]
    set to 0 to disable detection of stale remote nodes and deletion after a second chance
    (nodes should answer in max. 250msec, but we add some buffer for send/receive buffers)
*/
#ifndef CONFIG_ISO_ITEM_MAX_AGE
#  define CONFIG_ISO_ITEM_MAX_AGE 300
#endif

/** this define controls the time interval between regular SA requests on the bus
  * NOTE: Only used if CONFIG_ISO_ITEM_MAX_AGE is set!
  */
#ifndef SA_REQUEST_PERIOD_MSEC
#  define SA_REQUEST_PERIOD_MSEC 60000
#endif

/*@}*/

/**
  * @def CONFIG_MAX_ACTIVE_DTCS && CONFIG_MAX_PREVIOUSLY_ACTIVE_DTCS
  * use to define number of DTC to be registered for DM1 and DM2 diagnostic services
  * max value for CONFIG_MAX_ACTIVE_DTCS is 32
  * max value for CONFIG_MAX_PREVIOUSLY_ACTIVE_DTCS is 445
  */
#ifndef CONFIG_MAX_ACTIVE_DTCS
#  define CONFIG_MAX_ACTIVE_DTCS 32
#endif
#ifndef CONFIG_MAX_PREVIOUSLY_ACTIVE_DTCS
#  define CONFIG_MAX_PREVIOUSLY_ACTIVE_DTCS 445
#endif

// absolute limit CONFIG_MAX_PREVIOUSLY_ACTIVE_DTCS due to TP max size
#if CONFIG_MAX_PREVIOUSLY_ACTIVE_DTCS > 445
#  error CONFIG_MAX_PREVIOUSLY_ACTIVE_DTCS should not extend 445
#endif


#ifndef CONFIG_MAX_AUX2_PREFERRED_ASSIGNMENT_PER_FUNCTION
#  define CONFIG_MAX_AUX2_PREFERRED_ASSIGNMENT_PER_FUNCTION 5
#endif


/* ******************************************************** */
/*@{*/
#ifndef CONFIG_CAN_BLOCK_TIME
  /// BUS problem (not connected, blocked, off, ...)
  /// if CAN send lasts longer than defined
  /// --> discard send-queue and continue without sending.
  /// in case an own CAN Send FIFO is implemented
  /// and blocking is NOT wanted --> set it to 0 to disable
  /// the blocking feature completely!
#  define CONFIG_CAN_BLOCK_TIME 10
#endif

/** define interval for detection of incoming message loss.
    -> should normally NOT be changed by the user/app.
       keep it as is!
*/
#ifndef CONFIG_TIMEOUT_TRACTOR_DATA
  /// Message reception lost if no message received in this time
#  define CONFIG_TIMEOUT_TRACTOR_DATA 3000
#endif
/*@}*/

// Using 1.5s as stated in Annex F.
// There should not be a need to change this value.
#ifndef CONFIG_VT_CLIENT_ANNEX_F_COMMAND_TIMEOUT
#  define CONFIG_VT_CLIENT_ANNEX_F_COMMAND_TIMEOUT 1500
#endif

#ifndef CONFIG_VT_CLIENT_ANNEX_F_COMMAND_RETRIES
#  define CONFIG_VT_CLIENT_ANNEX_F_COMMAND_RETRIES 2
#endif

// it nothing special is defined for Bitmap storage, default to default storage.
#ifndef CONFIG_VT_CLIENT_OP_BITMAPS_MEMORY_MODIFIER
#  define CONFIG_VT_CLIENT_OP_BITMAPS_MEMORY_MODIFIER
#endif

// define how many send-queues should be set up.
#ifndef CONFIG_VT_CLIENT_NUM_SEND_PRIORITIES
#  define CONFIG_VT_CLIENT_NUM_SEND_PRIORITIES 1
#endif

// Don't keep this too low, as it will also be used for all other commands!
#ifndef CONFIG_FS_CLIENT_MAX_WRITE_SIZE
#  define CONFIG_FS_CLIENT_MAX_WRITE_SIZE 240
#endif


/* ***** Auto-set dependant defines ***** */

#ifndef USE_WORKING_SET
  /// auto-define USE_WORKING_SET dependend on the parts that need it!
#  if defined (USE_ISO_VIRTUALTERMINAL_CLIENT) || defined (USE_ISO_TASKCONTROLLER_CLIENT)
#    define USE_WORKING_SET
#  endif
#endif


/* ***** Special VT-Client optimizations ***** */

#if defined USE_ISO_VIRTUALTERMINAL_CLIENT
#  if defined PRJ_ISO_TERMINAL_OBJECT_SELECTION1
#    define _autogen_config_header1_ <PRJ_ISO_TERMINAL_OBJECT_SELECTION1>
#    include _autogen_config_header1_
#  endif
#  if defined PRJ_ISO_TERMINAL_OBJECT_SELECTION2
#    define _autogen_config_header2_ <PRJ_ISO_TERMINAL_OBJECT_SELECTION2>
#    include _autogen_config_header2_
#  endif
#  if defined PRJ_ISO_TERMINAL_OBJECT_SELECTION3
#    define _autogen_config_header3_ <PRJ_ISO_TERMINAL_OBJECT_SELECTION3>
#    include _autogen_config_header3_
#  endif
#  if defined PRJ_ISO_TERMINAL_OBJECT_SELECTION4
#    define _autogen_config_header4_ <PRJ_ISO_TERMINAL_OBJECT_SELECTION4>
#    include _autogen_config_header4_
#  endif



// e.g. #include <MaskDefinition/IsoTerminalObjectSelection.inc>
#endif


/* ******************************************************** */
/**
 * \name Define PGN codes of ISO 11783 messages
 */
/*@{*/

// <DESTINATION> PGNs
#define TIM_SERVER_TO_CLIENT_PGN    0x002300LU
#define TIM_CLIENT_TO_SERVER_PGN    0x002400LU
#define AUTH_SERVER_TO_CLIENT_PGN   0x007000LU
#define AUTH_CLIENT_TO_SERVER_PGN   0x006F00LU
#define CLIENT_TO_FS_PGN            0x00AA00LU
#define FS_TO_GLOBAL_PGN            0x00ABFFLU
#define FS_TO_CLIENT_PGN            0x00AB00LU
#define GUIDANCE_MACHINE_STATUS     0x00AC00LU
#define GUIDANCE_SYSTEM_CMD         0x00AD00LU
#define ETP_DATA_TRANSFER_PGN       0x00C700LU
#define ETP_CONN_MANAGE_PGN         0x00C800LU
#define PROCESS_DATA_PGN            0x00CB00LU
#define CAB_MESSAGE_1_PGN           0x00E000LU
#define VT_TO_GLOBAL_PGN            0x00E6FFLU
#define VT_TO_ECU_PGN               0x00E600LU
#define ECU_TO_VT_PGN               0x00E700LU
#define ECU_TO_GLOBAL_PGN           0x00E7FFLU
/* we're NOT using ACK/NACK to Global address, we're directing directly to the sender! */
#define ACKNOWLEDGEMENT_PGN         0x00E800LU
#define REQUEST_PGN_MSG_PGN         0x00EA00LU
#define TP_DATA_TRANSFER_PGN        0x00EB00LU
#define TP_CONN_MANAGE_PGN          0x00EC00LU
#define ADDRESS_CLAIM_PGN           0x00EE00LU
#define PROPRIETARY_A_PGN           0x00EF00LU
#define PROPRIETARY_A2_PGN          0x01EF00LU

// <NO DESTINATION> PGNs
#define ELECTRONIC_ENGINE_CONTROLLER_1_PGN                  0x00F004LU
#define ELECTRONIC_TRANSMISSION_CONTROLLER_2_PGN            0x00F005LU
#define ELECTRONIC_AXLE_CONTROLLER_1_PGN                    0x00F006LU
#define MACHINE_SELECTED_SPEED_PGN                          0x00F022LU
#define PRODUCT_IDENTIFICATION_PGN                          0x00FC8DLU
#define CONTROL_FUNCTION_FUNCTIONALITIES_PGN                0x00FC8ELU
#define ALL_IMPLEMENTS_STOP_OPERATIONS_SWITCH_STATE_PGN     0x00FD02LU
#define ECU_DIAGNOSTIC_PROTOCOL_PGN                         0x00FD32LU
#define ISOBUS_CERTIFICATION_PGN                            0x00FD42LU
#define SELECTED_SPEED_CMD                                  0x00FD43LU
#define ECU_IDENTIFICATION_INFORMATION_PGN                  0x00FDC5LU
#define OPERATORS_EXTERNAL_LIGHT_CONTROLS_MESSAGE_PGN       0x00FDCCLU
#define BASIC_JOYSTICK_MESSAGE_1_PGN                        0x00FDD6LU
#define TRACTOR_FACILITIES_PGN                              0x00FE09LU
#define WORKING_SET_MEMBER_PGN                              0x00FE0CLU
#define WORKING_SET_MASTER_PGN                              0x00FE0DLU
#define LANGUAGE_PGN                                        0x00FE0FLU
#define AUX_VALVE_0_ESTIMATED_FLOW                          0x00FE10LU
#define AUX_VALVE_1_ESTIMATED_FLOW                          0x00FE11LU
#define AUX_VALVE_2_ESTIMATED_FLOW                          0x00FE12LU
#define AUX_VALVE_3_ESTIMATED_FLOW                          0x00FE13LU
#define AUX_VALVE_4_ESTIMATED_FLOW                          0x00FE14LU
#define AUX_VALVE_5_ESTIMATED_FLOW                          0x00FE15LU
#define AUX_VALVE_6_ESTIMATED_FLOW                          0x00FE16LU
#define AUX_VALVE_7_ESTIMATED_FLOW                          0x00FE17LU
#define AUX_VALVE_8_ESTIMATED_FLOW                          0x00FE18LU
#define AUX_VALVE_9_ESTIMATED_FLOW                          0x00FE19LU
#define AUX_VALVE_10_ESTIMATED_FLOW                         0x00FE1ALU
#define AUX_VALVE_11_ESTIMATED_FLOW                         0x00FE1BLU
#define AUX_VALVE_12_ESTIMATED_FLOW                         0x00FE1CLU
#define AUX_VALVE_13_ESTIMATED_FLOW                         0x00FE1DLU
#define AUX_VALVE_14_ESTIMATED_FLOW                         0x00FE1ELU
#define AUX_VALVE_15_ESTIMATED_FLOW                         0x00FE1FLU
#define AUX_VALVE_0_MEASURED_FLOW                           0x00FE20LU
#define AUX_VALVE_1_MEASURED_FLOW                           0x00FE21LU
#define AUX_VALVE_2_MEASURED_FLOW                           0x00FE22LU
#define AUX_VALVE_3_MEASURED_FLOW                           0x00FE23LU
#define AUX_VALVE_4_MEASURED_FLOW                           0x00FE24LU
#define AUX_VALVE_5_MEASURED_FLOW                           0x00FE25LU
#define AUX_VALVE_6_MEASURED_FLOW                           0x00FE26LU
#define AUX_VALVE_7_MEASURED_FLOW                           0x00FE27LU
#define AUX_VALVE_8_MEASURED_FLOW                           0x00FE28LU
#define AUX_VALVE_9_MEASURED_FLOW                           0x00FE29LU
#define AUX_VALVE_10_MEASURED_FLOW                          0x00FE2ALU
#define AUX_VALVE_11_MEASURED_FLOW                          0x00FE2BLU
#define AUX_VALVE_12_MEASURED_FLOW                          0x00FE2CLU
#define AUX_VALVE_13_MEASURED_FLOW                          0x00FE2DLU
#define AUX_VALVE_14_MEASURED_FLOW                          0x00FE2ELU
#define AUX_VALVE_15_MEASURED_FLOW                          0x00FE2FLU
#define AUX_VALVE_0_COMMAND                                 0x00FE30LU
#define AUX_VALVE_1_COMMAND                                 0x00FE31LU
#define AUX_VALVE_2_COMMAND                                 0x00FE32LU
#define AUX_VALVE_3_COMMAND                                 0x00FE33LU
#define AUX_VALVE_4_COMMAND                                 0x00FE34LU
#define AUX_VALVE_5_COMMAND                                 0x00FE35LU
#define AUX_VALVE_6_COMMAND                                 0x00FE36LU
#define AUX_VALVE_7_COMMAND                                 0x00FE37LU
#define AUX_VALVE_8_COMMAND                                 0x00FE38LU
#define AUX_VALVE_9_COMMAND                                 0x00FE39LU
#define AUX_VALVE_10_COMMAND                                0x00FE3ALU
#define AUX_VALVE_11_COMMAND                                0x00FE3BLU
#define AUX_VALVE_12_COMMAND                                0x00FE3CLU
#define AUX_VALVE_13_COMMAND                                0x00FE3DLU
#define AUX_VALVE_14_COMMAND                                0x00FE3ELU
#define AUX_VALVE_15_COMMAND                                0x00FE3FLU
#define LIGHTING_DATA_PGN                                   0x00FE40LU
#define LIGHTING_COMMAND_PGN                                0x00FE41LU
#define HITCH_PTO_COMMANDS                                  0x00FE42LU
#define REAR_PTO_STATE_PGN                                  0x00FE43LU
#define FRONT_PTO_STATE_PGN                                 0x00FE44LU
#define REAR_HITCH_STATE_PGN                                0x00FE45LU
#define FRONT_HITCH_STATE_PGN                               0x00FE46LU
#define MAINTAIN_POWER_REQUEST_PGN                          0x00FE47LU
#define WHEEL_BASED_SPEED_DIST_PGN                          0x00FE48LU
#define GROUND_BASED_SPEED_DIST_PGN                         0x00FE49LU
#define VEHICLE_FLUIDS_PGN                                  0x00FE68LU
#define ELECTRONIC_TRANSMISSION_CONTROLLER_5_PGN            0x00FEC3LU
#define ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN                 0x00FECALU
#define PREVIOUSLY_ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN      0x00FECBLU
#define DIAGNOSTIC_DATA_CLEAR_PGN                           0x00FECCLU
#define SOFTWARE_IDENTIFICATION_PGN                         0x00FEDALU
#define IDLE_OPERATION_PGN                                  0x00FEDCLU
#define VEHICLE_DISTANCE_PGN                                0x00FEE0LU
#define SHUTDOWN_PGN                                        0x00FEE4LU
#define ENGINE_HOURS_REVOLUTIONS_PGN                        0x00FEE5LU
#define TIME_DATE_PGN                                       0x00FEE6LU
#define VEHICLE_HOURS_PGN                                   0x00FEE7LU
#define VEHICLE_DIRECTION_SPEED_PGN                         0x00FEE8LU
#define FUEL_CONSUMPTION_LIQUID_PGN                         0x00FEE9LU
#define VEHICLE_WEIGHT_PGN                                  0x00FEEALU
#define VEHICLE_IDENTIFICATION_PGN                          0x00FEECLU
#define ENGINE_TEMPERATURE_1_PGN                            0x00FEEELU
#define ENGINE_FLUID_LEVEL_PRESSURE_1_PGN                   0x00FEEFLU
#define POWER_TAKEOFF_INFORMATION_PGN                       0x00FEF0LU
#define CRUISE_CONTROL_VEHICLE_SPEED_PGN                    0x00FEF1LU
#define FUEL_ECONOMY_LIQUID_PGN                             0x00FEF2LU
#define VEHICLE_POSITION_PGN                                0x00FEF3LU
#define AMBIENT_CONDITIONS_PGN                              0x00FEF5LU
#define VEHICLE_ELECTRICAL_POWER_PGN                        0x00FEF7LU
#define TRANSMISSION_FLUIDS_PGN                             0x00FEF8LU
#define DASH_DISPLAY_PGN                                    0x00FEFCLU
// PROPRIETARY_B_PGN ranges from 0x00FF00 to 0x00FFFF !
#define PROPRIETARY_B_PGN                                   0x00FF00LU


// <NMEA 2000 GPS Messages>
#define NMEA_GPS_POSITION_RAPID_UPDATE_PGN                  0x01F801LU
#define NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN                   0x01F802LU
#define NMEA_GPS_HIGH_PRECISION_RAPID_UPDATE_PGN            0x01F803LU
#define NMEA_GPS_POSITION_DATA_PGN                          0x01F805LU
#define NMEA_GPS_TIME_DATE_PGN                              0x01F809LU
#define NMEA_GPS_CROSS_TRACK_ERROR_PGN                      0x01F903LU
#define NMEA_GPS_DIRECTION_DATA_PGN                         0x01FE11LU
#define NMEA_GNSS_PSEUDORANGE_NOISE_STATISTICS              0x01FA06LU



// NMEA_GPS_POSITION_RAPID_UPDATE_PGN 0x01F801LU   == 129025   IsoAgLib RECEIVES and SENDS this SINGLE-packet PGN
// NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN  0x01F802LU   == 129026   IsoAgLib RECEIVES and SENDS this SINGLE-packet PGN
// NMEA_GPS_POSITION_DATA_PGN         0x01F805LU   == 129029   IsoAgLib RECEIVES and SENDS this MULTI- packet PGN
// NMEA_GPS_DIRECTION_DATA_PGN        0x01FE11LU   == 130577   IsoAgLib RECEIVES           this MULTI- packet PGN PGN (send with NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN)

/*@}*/



/** define the debug device to use */
#ifndef INTERNAL_DEBUG_DEVICE
#  ifdef SYSTEM_PC
#    include <iostream>
#    include <fstream>
#    include "stdio.h"
#    ifndef ISOAGLIB_USE_UNICODE
#      define INTERNAL_DEBUG_DEVICE STL_NAMESPACE::cout
#      define INTERNAL_DEBUG_DEVICE_NEWLINE "\r\n"
#    else
#      define INTERNAL_DEBUG_DEVICE STL_NAMESPACE::wcout
#      define INTERNAL_DEBUG_DEVICE_NEWLINE L"\r\n"
#    endif
#    define INTERNAL_DEBUG_DEVICE_ENDL STL_NAMESPACE::endl
#    define INTERNAL_DEBUG_FLUSH fflush(0);
#  else
#    ifdef USE_RS232
#      define INTERNAL_DEBUG_DEVICE __IsoAgLib::getRs232Instance()
#    else
namespace __IsoAgLib {
struct NullOut_s {
  template <typename T> NullOut_s const &operator<<(T const &) const { return *this; }
  template <typename T> NullOut_s const &operator<<(T &) const { return *this; }
};
}
#      define INTERNAL_DEBUG_DEVICE (::__IsoAgLib::NullOut_s())
#    endif
#    ifndef ISOAGLIB_USE_UNICODE
#      define INTERNAL_DEBUG_DEVICE_ENDL "\r\n"
#      define INTERNAL_DEBUG_DEVICE_NEWLINE "\r\n"
#    else
#      define INTERNAL_DEBUG_DEVICE_ENDL L"\r\n"
#      define INTERNAL_DEBUG_DEVICE_NEWLINE L"\r\n"
#    endif
#    define INTERNAL_DEBUG_FLUSH
#  endif
#endif

/*@}*/

#endif
