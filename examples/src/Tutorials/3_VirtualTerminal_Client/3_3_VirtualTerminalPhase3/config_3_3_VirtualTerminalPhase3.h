// File: config_3_3_VirtualTerminalPhase3.h
// IMPORTANT: Never change the first block of this header manually!!!
//            All manual changes are overwritten by the next call of "update_makefile.sh conf_3_3_VirtualTerminalPhase3" 
//            Perform changes direct in the feature and project setup file conf_3_3_VirtualTerminalPhase3
//  ALLOWED ADAPTATION: Move the to be adapted defines from the middle block to the end after
//                      the line START_INDIVIDUAL_PROJECT_CONFIG and remove the comment indication there.
//                      All commented out defines in the middle block will be upated on next "update_makefile.sh conf_3_3_VirtualTerminalPhase3" call,
//                      if the corresponding value in isoaglib_config.h changed


// include an external file for definition of pool and firmware versions
#include "version.h"


#define PRJ_USE_AUTOGEN_CONFIG config_3_3_VirtualTerminalPhase3.h


#define CAN_BUS_CNT 1 

#define CAN_BUS_USED 0 

#define CAN_INSTANCE_CNT 1 

#define PRT_INSTANCE_CNT 1 

#define RS232_INSTANCE_CNT 1 

// Decide if the CPU stores number variables in BIG or LITTLE endian byte order in memory.
// Most CPU will use LITTLE ENDIAN. Only some types of ARM, mostly 68k and PowerPC CPU types will use big endian.
// Please check the manual of your targret cpu. This setting is used to activate some quick number conversion algorithms,
// which provide quick conversion from number variable to CAN strings ( which are always little endian in ISO ) - and other way.
#define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN

// #define OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN

// #define USE_PCAN_LIB

// #define CONFIG_DO_NOT_START_RELAIS_ON_STARTUP

// Decide if HEAP allocation strategy shall reduce size about 5K to 10K in favour of speed
// Strong Advice: Don't activate this, as long your target has not too tight memory restrictions
// Initialization of CAN filters and of local process data might get too slow under worst case conditions
// #define OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED

/** allow configuration by parameter value YES */
#ifndef YES
  #define YES 1
#endif
/** allow configuration by parameter value NO */
#ifndef NO
  #define NO 0
#endif
#ifndef USE_PROCESS_YN 
	#define USE_PROCESS_YN NO 
#endif
#ifndef USE_EEPROM_IO 
	#define USE_EEPROM_IO 
#endif
#ifndef USE_DATASTREAMS_IO 
	#define USE_DATASTREAMS_IO 
#endif
#ifndef USE_ISO_11783 
	#define USE_ISO_11783 
#endif
#ifndef USE_ISO_TERMINAL 
	#define USE_ISO_TERMINAL 
#endif
#ifndef USE_ISO_TERMINAL_GRAPHICCONTEXT 
	#define USE_ISO_TERMINAL_GRAPHICCONTEXT 
#endif
#ifndef PRJ_ISO_TERMINAL_OBJECT_SELECTION1 
	#define PRJ_ISO_TERMINAL_OBJECT_SELECTION1 MaskDefinition/simpleVTIsoPool-objectselection.inc 
#endif
#ifndef DEF_Stream_IMPL   
	#define DEF_Stream_IMPL   StreamChunk   
#endif
#ifndef DEF_Stream_c_IMPL 
	#define DEF_Stream_c_IMPL StreamChunk_c 
#endif

// The following configuration values can be overwritten.
// These settings are initially defined in isoaglib_config.h .
// These settings are in commented-out, so that you can activate and adapt them by
// moving them below the line with START_INDIVIDUAL_PROJECT_CONFIG

/// select reaction on powerdown detection - can be manually overridden in project config file
// #define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownByExplcitCall

/// define consolidation time for CAN_EN loss detection - can be manually overridden in project config file
// #define CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC 1000

/// set buffer size for CAN send
// #define CONFIG_CAN_SEND_BUFFER_SIZE 20

/// Multiply this SizeInPackets by 7 to get the size of ONE CHUNK (ONLY OF INTEREST IF STREAM IS CHUNKED)
// #define CONFIG_MULTI_RECEIVE_CHUNK_SIZE_IN_PACKETS 16

/// Maximum amount of packets to be allowed to be CTS'd in parallel (this amount will be distributed among all running streams)
// #define CONFIG_MULTI_RECEIVE_MAX_OVERALL_PACKETS_ADDED_FROM_ALL_BURSTS 16

/// Maximum amount of packets to be allowed by a "CTS" message.
// #define CONFIG_MULTI_RECEIVE_MAX_PER_CLIENT_BURST_IN_PACKETS CONFIG_MULTI_RECEIVE_MAX_OVERALL_PACKETS_ADDED_FROM_ALL_BURSTS

/// defines the amount of msec. to be wait until sending out the next CTS to the sender IF ONE STREAM ONLY BEING RECEIVED
// #define CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_SINGLE_STREAM 0

/// defines the amount of msec. to be wait until sending out the next CTS to the sender IF MULTIPLE STREAMS BEING RECEIVED
// #define CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_MULTI_STREAMS 50

/// define default RS232 XON-XOFF usage - can be manually overridden in project config file
// #define CONFIG_RS232_DEFAULT_XON_XOFF false

/// define default size of RS232 send buffer - can be manually overridden in project config file
// #define CONFIG_RS232_DEFAULT_SND_PUF_SIZE 200

/// define size of RS232 receive buffer - can be manually overridden in project config file
// #define CONFIG_RS232_DEFAULT_REC_PUF_SIZE 10

/// set default PWM frequency for digital output - can be manually overridden in project config file
// #define CONFIG_PWM_DEFAULT_FREQUENCY 100000

/// default max execution time of iScheduler_c::timeEvent - can be manually overridden in project config file
// #define CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME 500

/// erase ISO 11783 items after time (>0), on missing address claim after request - can be manually overridden in project config file
// #define CONFIG_ISO_ITEM_MAX_AGE 3000

/// interval [ms] for value update in EEPROM for local process data - can be manually overridden in project config file
// #define CONFIG_PROC_STORE_EEPROM_INTERVAL 5000

/// CAN BUS number for IsoAgLib (0xFF forces explicit call of init, to open the CAN BUS )
// #define CONFIG_CAN_DEFAULT_BUS_NUMBER 0xFF

/// define default min number for CAN Msg-Obj used for IsoAgLib
// #define CONFIG_CAN_DEFAULT_MIN_OBJ_NR 0

/// define default max number for CAN Msg-Obj used for IsoAgLib
// #define CONFIG_CAN_DEFAULT_MAX_OBJ_NR 13

/// BUS OFF if CAN send lasts longer then defined
// #define CONFIG_CAN_MAX_SEND_WAIT_TIME 200

/// YES | NO for EEPROM editor access via CAN
// #define CONFIG_EEPROM_USE_CAN_EDITOR_YN NO

/// YES | NO for EEPROM editor access via RS232
// #define CONFIG_EEPROM_USE_RS232_EDITOR_YN NO

/// set CAN Ident_c for filtering out EEEditor msgs
// #define CONFIG_EEPROM_USE_CAN_REC_IDENT 0x700

/** set CAN BUS number for CAN EEPROM Editor */
// #define CONFIG_EEPROM_USE_CAN_BUS       0

/** set CAN buffer size for receive and send */
// #define CONFIG_EEPROM_USE_CAN_BUFFER_SIZE    10


// DONT REMOVE THIS AND THE FOLLOWING LINE AS THEY ARE NEEDED TO DETECT YOUR PERSONAL PROJECT ADAPTATIONS!!!
// START_INDIVIDUAL_PROJECT_CONFIG