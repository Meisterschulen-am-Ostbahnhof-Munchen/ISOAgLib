// File: config_2_2_LocalReadDataSimpleSetpointIso.h
// IMPORTANT: Never change the first block of this header manually!!!
//            All manual changes are overwritten by the next call of "update_makefile.sh conf_2_2_LocalReadDataSimpleSetpointIso" 
//            Perform changes direct in the feature and project setup file conf_2_2_LocalReadDataSimpleSetpointIso
//  ALLOWED ADAPTATION: Move the to be adapted defines from the middle block to the end after
//                      the line START_INDIVIDUAL_PROJECT_CONFIG and remove the comment indication there.
//                      All commented out defines in the middle block will be upated on next "update_makefile.sh conf_2_2_LocalReadDataSimpleSetpointIso" call,
//                      if the corresponding value in isoaglib_config.h changed


// include an external file for definition of pool and firmware versions
#include "version.h"
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

// #define CONFIG_DO_NOT_START_RELAIS_ON_STARTUP

#ifndef USE_PROC_DATA_DESCRIPTION_POOL 
	#define USE_PROC_DATA_DESCRIPTION_POOL 
#endif
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
// Decide if float shall be used for the project
#define USE_FLOAT_DATA_TYPE

#ifndef USE_PROCESS 
  #define USE_PROCESS 
#endif
#ifndef USE_EEPROM_IO_YN 
	#define USE_EEPROM_IO_YN NO 
#endif
#ifndef USE_DATASTREAMS_IO_YN 
	#define USE_DATASTREAMS_IO_YN NO 
#endif
#ifndef USE_ISO_11783 
	#define USE_ISO_11783 
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

/// set puffer size for CAN send
// #define CONFIG_CAN_SEND_BUFFER_SIZE 20

/// set default puffer size for CAN receive
// #define CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN 15

/// high load of some CAN idents need larger buffers
// #define CONFIG_CAN_HIGH_LOAD_REC_BUF_SIZE_MIN 25

/// set increment for automatic buffer increase after overload ( 0 == no increase )
// #define CONFIG_CAN_BUF_INCREASE_ON_OVERLOAD 5

/// define list of CAN filters where large buffers are needed due to high load
// #define CONFIG_CAN_HIGH_LOAD_IDENT_LIST { 0xcb0000 }

/// define amount of list entries in CONFIG_CAN_HIGH_LOAD_IDENT_LIST
// #define CONFIG_CAN_HIGH_LOAD_IDENT_CNT 1

/// define mask for compare with ident
// #define CONFIG_CAN_HIGH_LOAD_IDENT_MASK 0xff0000

/// Max amount of Filters which can be connected to a CAN MsgObj for ISO
// #define CONFIG_CAN_ISO_FILTER_BOX_PER_MSG_OBJ 15

/// Max amount of Filters which can be connected to a CAN MsgObj for DIN
// #define CONFIG_CAN_DIN_FILTER_BOX_PER_MSG_OBJ 5

/// define default RS232 baudrate - can be manually overridden in project config file
// #define CONFIG_RS232_DEFAULT_BAUDRATE 19200

/// define default RS232 bit encoding - can be manually overridden in project config file
// #define CONFIG_RS232_DEFAULT_DATA_MODE RS232IO_c::_8_N_1

/// define default RS232 XON-XOFF usage - can be manually overridden in project config file
// #define CONFIG_RS232_DEFAULT_XON_XOFF false

/// define default size of RS232 send puffer - can be manually overridden in project config file
// #define CONFIG_RS232_DEFAULT_SND_PUF_SIZE 200

/// define size of RS232 receive puffer - can be manually overridden in project config file
// #define CONFIG_RS232_DEFAULT_REC_PUF_SIZE 10

/// set default PWM frequency for digital output - can be manually overridden in project config file
// #define CONFIG_PWM_DEFAULT_FREQUENCY 100000

/// define size of buffer for Hdd read access - can be manually overridden in project config file
// #define CONFIG_HDD_READ_PUFFER_SIZE 100

/// define minimum size of data to read as one bock from Hdd - can be manually overridden in project config file
// #define CONFIG_HDD_READ_BLOCK_SIZE 20

/// define longest token which is used to translate printed numbers to number variables - can be manually overridden in project config file
// #define CONFIG_HDD_MAX_TOKEN_LEN 10

/// define value coding for RS232 connected DOS Drive - can be manually overridden in project config file
// #define CONFIG_HDD_DEFAULT_DATA_MODE RS232IO_c::_8_N_1

/// default max execution time of iScheduler_c::timeEvent - can be manually overridden in project config file
// #define CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME 500

/// erase ISO 11783 items after time (>0), on missing address claim after request - can be manually overridden in project config file
// #define CONFIG_ISO_ITEM_MAX_AGE 3000

/// erase DIN 9684 devices from monitor list after given dead time ( 0 == no delete ) - can be manually overridden in project config file
// #define CONFIG_DIN_ITEM_MAX_AGE 0

/// erase DIN 9684 services from monitor list after given dead time ( 0 == no delete ) - can be manually overridden in project config file
// #define CONFIG_DIN_SERVICE_MAX_AGE 3000

/// interval [ms] for value update in EEPROM for local process data - can be manually overridden in project config file
// #define CONFIG_PROC_STORE_EEPROM_INTERVAL 5000

/// CAN BUS number for IsoAgLib (0xFF forces explicit call of init, to open the CAN BUS )
// #define CONFIG_CAN_DEFAULT_BUS_NUMBER 0xFF

/// define default MsgObj_c number to use for CAN send
// #define CONFIG_CAN_DEFAULT_SEND_OBJ 0

/// define default min number for CAN Msg-Obj used for IsoAgLib
// #define CONFIG_CAN_DEFAULT_MIN_OBJ_NR 0

/// define default max number for CAN Msg-Obj used for IsoAgLib
// #define CONFIG_CAN_DEFAULT_MAX_OBJ_NR 13

/// select if CAN should change baudrate to fallback, if BUS deadlock is detected
// #define CONFIG_CAN_USE_FALLBACK_ON_CAN_DEADLOCK 0

/// max time intervall of CAN err before switching back from 250 to 125 kbit/s in [msec.]
// #define CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING 3000

/// define bitrate to use after fallbcack /ref CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING
// #define CONFIG_CAN_FALLBACK_BITRATE 125

/// BUS OFF if CAN send lasts longer then defined
// #define CONFIG_CAN_MAX_SEND_WAIT_TIME 200

/// BUS problem, if no message received in this time
// #define CONFIG_CAN_MAX_CAN_IDLE 3000

/// YES | NO for EEPROM editor access via CAN
// #define CONFIG_EEPROM_USE_CAN_EDITOR_YN NO

/// YES | NO for EEPROM editor access via RS232
// #define CONFIG_EEPROM_USE_RS232_EDITOR_YN NO

/// set CAN Ident_c for filtering out EEEditor msgs
// #define CONFIG_EEPROM_USE_CAN_REC_IDENT 0x700

/** set CAN Msg obejct for receiving EEEditor msg */
// #define CONFIG_EEPROM_USE_CAN_REC_OBJ   13

/** set CAN Msg obejct for sending EEEditor msg */
// #define CONFIG_EEPROM_USE_CAN_SEND_OBJ  14

/** set CAN BUS number for CAN EEPROM Editor */
// #define CONFIG_EEPROM_USE_CAN_BUS       0

/** set CAN puffer size for receive and send */
// #define CONFIG_EEPROM_USE_CAN_BUFFER_SIZE    10

/** to use standard ident (11bit) ->0, 29bit -> 1 */
// #define CONFIG_EEPROM_USE_CAN_EXT_IDENT 0


// DONT REMOVE THIS AND THE FOLLOWING LINE AS THEY ARE NEEDED TO DETECT YOUR PERSONAL PROJECT ADAPTATIONS!!!
// START_INDIVIDUAL_PROJECT_CONFIG
